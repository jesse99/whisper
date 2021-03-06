/*
 *  File:       WThread.cpp
 *  Summary:   	Abstract base class for threads. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: WThread.cpp,v $
 *		Revision 1.4  2001/05/06 10:40:18  jesjones
 *		Dtor asserts that the thread isn't running.
 *		
 *		Revision 1.3  2001/04/21 03:34:13  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 12:12:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XThread.h>

#include <process.h>

#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class XThread
// ===================================================================================

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
	
	VERIFY(::CloseHandle(mThread));
	mThread = nil;
}


//---------------------------------------------------------------
//
// XThread::XThread
//
// We call _beginthreadex instead of CreateThread to give the
// C runtime a chance to allocate thread-local storage (this works
// with MSVC and CodeWarrior).
//
//---------------------------------------------------------------
XThread::XThread(uint32 stackBytes)
{
	UNUSED(stackBytes);
	
	mThread = (HANDLE) _beginthreadex(nil,				// security attributes
							 		  0,				// stack size
							 		  XThread::DoEntry,	// entry point
							 		  this,				// refCon
							 		  CREATE_SUSPENDED,	// flags
							 		  &mID);			// thread id
	ThrowIfBadHandle(mThread);

	mPreemptive = true;
	mRunning    = false;
	mAborted    = false;
}


//---------------------------------------------------------------
//
// XThread::Suspend
//
//---------------------------------------------------------------
void XThread::Suspend()
{
	PRECONDITION(mRunning);
	PRECONDITION(!mAborted);
	
	uint32 suspendCount = ::SuspendThread(mThread);
	ThrowIf(suspendCount == 0xFFFFFFFF);
}


//---------------------------------------------------------------
//
// XThread::Wake
//
//---------------------------------------------------------------
void XThread::Wake()
{	
	PRECONDITION(mRunning);

	uint32 suspendCount = ::ResumeThread(mThread);
	ThrowIf(suspendCount == 0xFFFFFFFF);
}


//---------------------------------------------------------------
//
// XThread::GetPriority
//
//---------------------------------------------------------------
int32 XThread::GetPriority() const
{
	int32 priority = ::GetThreadPriority(mThread);
	ThrowIf(priority == THREAD_PRIORITY_ERROR_RETURN);
	
	return priority;
}


//---------------------------------------------------------------
//
// XThread::SetPriority
//
//---------------------------------------------------------------
void XThread::SetPriority(int32 priority)
{
	int32 succeeded = ::SetThreadPriority(mThread, priority);
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XThread::Join
//
//---------------------------------------------------------------
void XThread::Join(MilliSecond timeout)
{
	if (timeout == LONG_MAX)
		timeout = (MilliSecond) INFINITE;
		
	uint32 result = ::WaitForSingleObject(mThread, (uint32) timeout);
	ThrowIf(result == WAIT_ABANDONED || result == WAIT_FAILED);
}


//---------------------------------------------------------------
//
// XThread::Sleep										[static]
//
//---------------------------------------------------------------
void XThread::Sleep(MilliSecond delay)
{
	PRECONDITION(delay >= 0);
	
	if (delay == LONG_MAX)
		delay = (MilliSecond) INFINITE;
	
	::Sleep((uint32) delay);
}


//---------------------------------------------------------------
//
// XThread::GetActiveThread								[static]
//
//---------------------------------------------------------------
XThread* XThread::GetActiveThread()
{
	XThread* thread = nil;

	uint32 id = ::GetCurrentThreadId();
	
	{
	XEnterCriticalSection enter(msThreadsSection);
		iterator iter = msThreads.begin();
		while (iter != msThreads.end() && thread == nil) {
			XThread* candidate = *iter;
			++iter;
			
			if (candidate->mID == id)
				thread = candidate;
		}
	}
		
	return thread;
}

static uint32 sMainThread = ::GetCurrentThreadId();

//---------------------------------------------------------------
//
// XThread::InMainThread								[static]		
//
//---------------------------------------------------------------
bool XThread::InMainThread()
{
	uint32 id = ::GetCurrentThreadId();
	
	bool mainThread = id == sMainThread;	// $$ is there a better way to do this?
	
	return mainThread;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XThread::DoEntry										[static]
//
//---------------------------------------------------------------
unsigned int CALLBACK XThread::DoEntry(void* refCon)
{
	PRECONDITION(refCon != nil);
	
	try {
		XThread* thisPtr = reinterpret_cast<XThread*>(refCon);
		thisPtr->Run();
		
	} catch (...) {
		DEBUGSTR("Unhandled thread exception");	// can hit this if OnException winds up throwing
	}

	return 0;
}


}	// namespace Whisper
