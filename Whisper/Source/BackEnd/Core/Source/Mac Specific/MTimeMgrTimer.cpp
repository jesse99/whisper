/*
 *  File:       MTimeMgrTimer.cpp
 *  Summary:   	Timer that gets time via a Time manager task
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MTimeMgrTimer.cpp,v $
 *		Revision 1.7  2001/04/21 03:29:17  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/04/13 07:53:48  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.5  2001/03/14 11:07:01  jesjones
 *		Don't use the new timer calls for normal targets, sigh.
 *		
 *		Revision 1.4  2001/03/14 10:35:33  jesjones
 *		Reverted to using the old timer calls when building DLLs (the new symbols don't seem to be exported from the stub libraries).
 *		
 *		Revision 1.3  2001/03/13 11:05:11  jesjones
 *		Switched to using the new time manager calls that return errors.
 *		
 *		Revision 1.2  2000/11/09 09:21:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 2/02/00	JDJ		No longer descends from MExitActionMixIn (this is only
 *									needed for OS 7 and is a bit sticky to implement in a
 *									multi-fragment app).
 *		 <2>	 7/19/99	JDJ		Tweaked for Carbon.
 *		 <1>	 9/20/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <MTimeMgrTimer.h>

#include <SLFunctions.h>
#include <MSystemInfo.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XNumbers.h>

namespace Whisper {


// ===================================================================================
//	class MTimeMgrTimerMixIn
// ===================================================================================

//---------------------------------------------------------------
//
// MTimeMgrTimerMixIn::~MTimeMgrTimerMixIn
//
//---------------------------------------------------------------
MTimeMgrTimerMixIn::~MTimeMgrTimerMixIn()
{
	PRECONDITION(!this->TimerIsRunning());		// Otherwise OnTime can be called while the object is being destroyed.
	PRECONDITION(!mTimeMgrInfo.inTask);			// Can't call this from within the time manager task!

	if (mTimerInstalled)
		this->DoRemoveTimer();
		
	if (mTimeMgrInfo.task.tmAddr != nil)
		DisposeTimerUPP(mTimeMgrInfo.task.tmAddr);
}

	
//---------------------------------------------------------------
//
// MTimeMgrTimerMixIn::MTimeMgrTimerMixIn
//
// Note that the timer has to start out stopped since the vtable 
// entry for OnTime is still nil.
//
// $$�It seems like we could get rid of this by providing a definition 
// $$�for OnTime (while keeping it pure virtual), but this doesn't
// $$ work with CW9. (Creating a base class with a definition of
// $$�OnTime doesn't work either).
//
//---------------------------------------------------------------
MTimeMgrTimerMixIn::MTimeMgrTimerMixIn(MilliSecond freq, bool running)
{
	PRECONDITION(!running);
	PRECONDITION(freq >= 0);
	PRECONDITION(MSystemInfo::GetSystemVersion() >= 0x0800);	// System 8 was the first OS that cleaned up Time Manager tasks when the app exited abnormally

	mTimerFreq      = freq;
	mTimerIsRunning = running;

	mTimeMgrInfo.task.tmAddr     = NewTimerUPP(DoInterrupt);
	mTimeMgrInfo.task.tmWakeUp   = 0;
	mTimeMgrInfo.task.tmReserved = 0;
	
	mTimeMgrInfo.mMyA5   = SetCurrentA5();
	mTimeMgrInfo.thisPtr = this;
	mTimeMgrInfo.inTask  = false;
	
	ThrowIfNil(mTimeMgrInfo.task.tmAddr);
	
	mTimerInstalled = false;
	
	if (this->TimerIsRunning())
		this->DoInstallTimer();
}


//---------------------------------------------------------------
//
// MTimeMgrTimerMixIn::StopTimer
//
//---------------------------------------------------------------
void MTimeMgrTimerMixIn::StopTimer()
{
	this->DoRemoveTimer();

	mTimerIsRunning = false;
}

	
//---------------------------------------------------------------
//
// MTimeMgrTimerMixIn::StartTimer
//
//---------------------------------------------------------------
void MTimeMgrTimerMixIn::StartTimer()
{
	mTimerIsRunning = true;
	
	this->DoInstallTimer();
}


//---------------------------------------------------------------
//
// MTimeMgrTimerMixIn::SetTimerFreq
//
//---------------------------------------------------------------
void MTimeMgrTimerMixIn::SetTimerFreq(MilliSecond freq)
{
	PRECONDITION(freq >= 0); 
	
	mTimerFreq = freq;
	
	if (mTimerInstalled) {
		this->DoRemoveTimer();
		this->DoInstallTimer();
	}
}

#pragma mark ~

//---------------------------------------------------------------
//
// MTimeMgrTimerMixIn::DoInstallTimer
//
//---------------------------------------------------------------
void MTimeMgrTimerMixIn::DoInstallTimer()
{
	PRECONDITION(!mTimeMgrInfo.inTask);					// Can't call this from within the time manager task!

	if (!mTimerInstalled) {
		mTimeMgrInfo.task.tmWakeUp   = 0;			// Disable drift-free execution (can cause problems with small frequencies. See IM:Processes pg 3-8, 3-9)
		mTimeMgrInfo.task.tmReserved = 0;			// $$ Might want to enable drift-free execution for large frequencies.

		OSErr err = InstallXTimeTask((QElemPtr) &mTimeMgrInfo.task);
		ThrowIfOSErr(err);

		mTimerInstalled = true;
		
		err = PrimeTimeTask((QElemPtr) &mTimeMgrInfo.task, this->GetTimerFreq());
		ThrowIfOSErr(err);
	}
}


//---------------------------------------------------------------
//
// MTimeMgrTimerMixIn::DoRemoveTimer
//
//---------------------------------------------------------------
void MTimeMgrTimerMixIn::DoRemoveTimer()
{
	if (mTimerInstalled) {
		OSErr err = RemoveTimeTask((QElemPtr) &mTimeMgrInfo.task);	// Note that IM VI says that RmvTime can be called at interrupt time... 
		ASSERT(err == noErr);										// don't throw for this
		
		mTimerInstalled = false;
	}
}


//---------------------------------------------------------------
//
// MTimeMgrTimerMixIn::DoInterrupt						[static]
//
//---------------------------------------------------------------
#pragma profile off
pascal void MTimeMgrTimerMixIn::DoInterrupt(TMTask* taskPtr)		// interrupt code
{
	STimeMgrRecord* dataPtr = reinterpret_cast<STimeMgrRecord*>(taskPtr);

	volatile int32 oldA5 = SetA5(dataPtr->mMyA5);
	
	SLEnterInterrupt();
	
	SAFE_ASSERT(dataPtr != nil);
	
	try {
		// Set a flag so we can complain if anyone does something they
		// shouldn't while we're in the time manager interrupt.
		SAFE_ASSERT(!dataPtr->inTask);
		dataPtr->inTask = true;
		
		SAFE_ASSERT(dataPtr->thisPtr != nil);
		
		if (dataPtr->thisPtr->TimerIsRunning())
			dataPtr->thisPtr->OnTime();
		
		// Prime the pump so we get called again.
		if (dataPtr->thisPtr->TimerIsRunning()) {
			OSErr err = PrimeTimeTask((QElemPtr) &dataPtr->task, dataPtr->thisPtr->GetTimerFreq());	
			SAFE_ASSERT(err == noErr);
		}
		
	} catch (...) {
	}
	
	dataPtr->inTask = false;
	
	SLLeaveInterrupt();
		
	SetA5(oldA5);	
}
#pragma profile reset


}	// namespace Whisper
