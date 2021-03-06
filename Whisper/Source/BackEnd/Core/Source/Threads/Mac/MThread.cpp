/*
 *  File:       MThread.cpp
 *  Summary:   	Abstract base class for threads. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: MThread.cpp,v $
 *		Revision 1.6  2001/05/06 10:40:06  jesjones
 *		Dtor asserts that the thread isn't running.
 *		
 *		Revision 1.5  2001/04/21 03:29:09  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/04/13 07:53:40  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2000/12/10 04:05:39  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:20:52  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 1/23/00	JDJ		Added InMainThread.
 *		 <4>	 7/19/99	JDJ		Tweaked for Carbon.
 *		 <3>	 7/13/99	JDJ		Added profiling support.
 *		 <2>	 6/19/99	JDJ		Sleep was only using the delay argument with the main thread.
 *		 <1>	 5/23/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XThread.h>

#include <MSystemInfo.h>
#include <XExceptions.h>
#include <XMiscUtils.h>
#include <XNumbers.h>
#include <XStringUtils.h>

namespace Whisper {

#define PROFILE_THREADS	(__profile__ && 1)


inline ThreadID GetStartupThread()
{
	ThreadID id;
	OSErr err = ::GetCurrentThread(&id);
	REQUIRE(err == noErr);
	
	return id;
}

static ThreadID sMainThread = GetStartupThread();


// ===================================================================================
//	class XThread
// ===================================================================================

TimerUPP 			XThread::msWakeUPP = nil;		
XCriticalSection	XThread::msThreadsSection;
XThread::ThreadList	XThread::msThreads;

//---------------------------------------------------------------
//
// XThread::~XThread
//
//---------------------------------------------------------------
XThread::~XThread()
{
	PRECONDITION(!mRunning);
	
#if PROFILE_THREADS
	::ProfilerDeleteThread(mProfileRef);
#endif

#if TARGET_CARBON
	::DisposeThreadEntryUPP(mEntryUPP);
#endif
}


//---------------------------------------------------------------
//
// XThread::XThread
//
//---------------------------------------------------------------
XThread::XThread(uint32 stackBytes)
{
	if (!MSystemInfo::HasThreadMgr())
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"The Thread Manager isn't installed.")));
		
	if (msWakeUPP == nil) {
		msWakeUPP = NewTimerUPP(XThread::DoWake);
		ThrowIfNil(msWakeUPP);
	}
	
#if TARGET_CARBON
	mEntryUPP = ::NewThreadEntryUPP(XThread::DoEntry);
	ThrowIfNil(mEntryUPP);
	
	ThreadEntryUPP entryPoint = mEntryUPP;
#else
	ThreadEntryProcPtr entryPoint = XThread::DoEntry;
#endif
		
	OSErr err = ::NewThread(kCooperativeThread, 		// thread style
							entryPoint,					// entry point
							this,						// refCon
							numeric_cast<int32>(stackBytes),		// stack size
							kNewSuspend,				// options
							nil,						// result
							&mThread);					// thread ID
	ThrowIfOSErr(err);

	mPreemptive = false;
	mSleeping   = 1;
	mRunning    = false;
	mAborted    = false;

#if PROFILE_THREADS
	uint32 stackSize;
	err = ::ThreadCurrentStackSpace(mThread, &stackSize);
	ThrowIfOSErr(err);
	
	err = ::ProfilerCreateThread(numeric_cast<int16>(stackSize/64), numeric_cast<int32>(stackSize), &mProfileRef);	// this is what Pro 5's LThread does...
	ThrowIfOSErr(err);
#endif
}


//---------------------------------------------------------------
//
// XThread::Sleep										[static]
//
//---------------------------------------------------------------
void XThread::Sleep(MilliSecond delay)
{
	XThread* thread = XThread::GetActiveThread();
	if (thread != nil)
		thread->DoSleep(delay);
	else
		MilliSecondDelay(delay);		// should only happen for the main thread
}


//---------------------------------------------------------------
//
// XThread::Suspend
//
//---------------------------------------------------------------
void XThread::Suspend()
{
	this->DoSleep(LONG_MAX);
}


//---------------------------------------------------------------
//
// XThread::Wake
//
//---------------------------------------------------------------
void XThread::Wake()
{
	PRECONDITION(mRunning);

	mSleeping = Max(--mSleeping, 0L);
	
	if (mSleeping == 0) {
		::RmvTime((QElemPtr) &mWakeTask);

		OSErr err = ::SetThreadState(mThread, kReadyThreadState, kNoThreadID);
		ThrowIfOSErr(err);
	}
}


//---------------------------------------------------------------
//
// XThread::Yield										[static]
//
// Calling Yield too often can seriously degrade performance, but
// calling Yield too little can cause the UI to become sluggish.
// It would be possible to require clients to only call Yield at
// a reasonable frequency but, in general, the only good way to
// do this is by timing how long it's been since the last Yield
// call and that's too painful.
//
// So what we do here is ensure that the client code gets the
// processor for at least a tick before we call Yield. Note that
// this can have a substantial effect. For example, when XDropApp
// is initializing there are only two threads running: the main
// thread which handles the UI and a worker thread which counts
// the number of files. The worker thread calls Yield after every
// file. Without this optimization counting 172 files takes 13.4
// seconds. With the optimization it only takes 2.0 seconds.
//
//---------------------------------------------------------------
void XThread::Yield()
{
	static uint32 lastTime = TickCount();
	
	uint32 time = TickCount();
	if (time != lastTime) {
		OSErr err = ::YieldToAnyThread();
		ThrowIfOSErr(err);
		
		lastTime = TickCount();
	}
}


//---------------------------------------------------------------
//
// XThread::Join
//
// Might be better to sleep for a while inside the loops, but
// this is difficult to do in the most common case where Join
// is called from the main thread.
//
//---------------------------------------------------------------
void XThread::Join(MilliSecond timeout)
{
	if (timeout == LONG_MAX) {
		while (this->IsRunning())
			XThread::Yield();
	
	} else {
		MilliSecond startTime = GetMilliSeconds();
		while (this->IsRunning() && GetMilliSeconds() - startTime < timeout)
			XThread::Yield();
	}
}


//---------------------------------------------------------------
//
// XThread::GetActiveThread								[static]
//
//---------------------------------------------------------------
XThread* XThread::GetActiveThread()
{
	XThread* thread =nil;

	ThreadID id;
	OSErr err = ::GetCurrentThread(&id);
	ThrowIfOSErr(err);
	
	{
	XEnterCriticalSection enter(msThreadsSection);
		iterator iter = msThreads.begin();
		while (iter != msThreads.end() && thread == nil) {
			XThread* candidate = *iter;
			++iter;
			
			if (candidate->GetOSThread() == id)
				thread = candidate;
		}
	}
		
	return thread;
}


//---------------------------------------------------------------
//
// XThread::InMainThread								[static]		
//
//---------------------------------------------------------------
bool XThread::InMainThread()
{
	ThreadID id;
	OSErr err = ::GetCurrentThread(&id);
	ThrowIfOSErr(err);
	
	bool mainThread = id == sMainThread;
	
	return mainThread;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XThread::DoSleep		
//
//---------------------------------------------------------------
void XThread::DoSleep(MilliSecond delay)
{
	PRECONDITION(delay >= 0);
	PRECONDITION(mRunning);
	PRECONDITION(!mAborted);
	
	if (delay > 0) {
		::RmvTime((QElemPtr) &mWakeTask);
		
		if (delay < LONG_MAX) {
			mWakeTask.tmAddr     = msWakeUPP;	
			mWakeTask.tmWakeUp   = 0;
			mWakeTask.tmCount    = 0;	
			mWakeTask.tmReserved = 0;	
			mWakeTask.thisPtr    = this;	
		
			::InsTime((QElemPtr) &mWakeTask);
			::PrimeTime((QElemPtr) &mWakeTask, delay);
		}
		
		if (++mSleeping == 1) {
			OSErr err = ::SetThreadState(mThread, kStoppedThreadState, kNoThreadID);
			ThrowIfOSErr(err);
		}
		
	} else
		XThread::Yield();
}


//---------------------------------------------------------------
//
// XThread::DoEntry										[static]
//
//---------------------------------------------------------------
#pragma profile off
pascal void* XThread::DoEntry(void* refCon)
{
	PRECONDITION(refCon != nil);
	
	try {
		XThread* thisPtr = reinterpret_cast<XThread*>(refCon);
		
		thisPtr->Run();
		
	} catch (...) {
		DEBUGSTR("Unhandled thread exception");	// can hit this if OnException winds up throwing
	}

	return nil;
}
#pragma profile reset


//---------------------------------------------------------------
//
// XThread::DoEntry										[static]
//
//---------------------------------------------------------------
#if	__profile__
#pragma profile off
pascal void XThread::DoSwapIn(ThreadID, void* refCon)
{
#if PROFILE_THREADS
	XThread* thisPtr = reinterpret_cast<XThread*>(refCon);
		
	::ProfilerSwitchToThread(thread->mProfileRef);
#endif
}
#pragma profile reset
#endif


//---------------------------------------------------------------
//
// XThread::DoWake										[static]
//
//---------------------------------------------------------------
#pragma profile off
pascal void XThread::DoWake(TMTask* task)
{
	PRECONDITION(task != nil);
	
	SMyTask* myTask = reinterpret_cast<SMyTask*>(task);
	myTask->thisPtr->Wake();
}
#pragma profile reset


}	// namespace Whisper
