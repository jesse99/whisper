/*
 *  File:       XSyncObjects.h
 *  Summary:   	Thread synchronization objects. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XSyncObjects.cpp,v $
 *		Revision 1.3  2001/04/27 04:36:59  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:54:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XSyncObjects.h>

#include <XDebug.h>
#include <XExceptions.h>
#include <XMiscUtils.h>

namespace Whisper {


// ===================================================================================
//	class XMutex
// ===================================================================================

//---------------------------------------------------------------
//
// XMutex::~XMutex
//
//---------------------------------------------------------------
XMutex::~XMutex()
{
}


//---------------------------------------------------------------
//
// XMutex::XMutex
//
//---------------------------------------------------------------
XMutex::XMutex(bool locked)
{
#if WIN
	mObject = ::CreateMutex(nil, locked, nil);
	ThrowIfBadHandle(mObject);
	
#elif MAC
	if (locked) {
		OSErr err = ::GetCurrentThread(&mOwner);
		ThrowIfOSErr(err);
	
		mLockCount = 1;

	} else {
		mOwner = kNoThreadID;
		mLockCount = 0;
	}
#endif
}


//---------------------------------------------------------------
//
// XMutex::Lock
//
//---------------------------------------------------------------
#if MAC
bool XMutex::Lock(MilliSecond timeout)
{
	PRECONDITION(timeout >= 0);
	PRECONDITION(mLockCount >= 0);
	
	ThreadID current;
	OSErr err = ::GetCurrentThread(&current);
	ThrowIfOSErr(err);

	bool locked = current == mOwner;

	if (!locked) {
		MilliSecond start = GetMilliSeconds();
		
		while (mOwner != kNoThreadID && (timeout == LONG_MAX || GetMilliSeconds() - start < timeout)) 
			(void) ::YieldToAnyThread();
		
		if (mOwner == kNoThreadID) {
			OSErr err = ::GetCurrentThread(&mOwner);
			ThrowIfOSErr(err);
			
			locked = true;
		}
	}
		
	++mLockCount;
	
	return locked;
}
#endif


//---------------------------------------------------------------
//
// XMutex::Unlock
//
//---------------------------------------------------------------
void XMutex::Unlock()
{
#if WIN
	int32 succeeded = ::ReleaseMutex(mObject);
	ThrowIf(!succeeded);
	
#elif MAC
	PRECONDITION(mLockCount > 0);
	
#if DEBUG
	ThreadID current;
	OSErr err = ::GetCurrentThread(&current);
	ASSERT(err == noErr);
	
	ASSERT(mOwner == current);			// unlike semaphores, mutexes can only be unlocked from the original thread
#endif
	
	if (--mLockCount == 0)
		mOwner = kNoThreadID;
#endif
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XSemaphore
// ===================================================================================

//---------------------------------------------------------------
//
// XSemaphore::~XSemaphore
//
//---------------------------------------------------------------
XSemaphore::~XSemaphore()
{
}


//---------------------------------------------------------------
//
// XSemaphore::XSemaphore (uint32)
//
//---------------------------------------------------------------
XSemaphore::XSemaphore(uint32 count)
{
	PRECONDITION(count > 0);
	
#if WIN
	PRECONDITION(count < LONG_MAX);
	
	mObject = ::CreateSemaphore(nil, (int32) count, (int32) count, nil);
	ThrowIfBadHandle(mObject);
	
#elif MAC
	mCount = count;
	mMaxCount = count;
#endif
}


//---------------------------------------------------------------
//
// XSemaphore::XSemaphore (uint32, uint32)
//
//---------------------------------------------------------------
XSemaphore::XSemaphore(uint32 initialCount, uint32 maxCount)
{
	PRECONDITION(maxCount > 0);
	PRECONDITION(initialCount <= maxCount);
	
#if WIN
	PRECONDITION(maxCount < LONG_MAX);
	
	mObject = ::CreateSemaphore(nil, (int32) initialCount, (int32) maxCount, nil);
	ThrowIfBadHandle(mObject);
	
#elif MAC
	mCount = initialCount;
	mMaxCount = maxCount;
#endif
}


//---------------------------------------------------------------
//
// XSemaphore::Lock
//
//---------------------------------------------------------------
#if MAC
bool XSemaphore::Lock(MilliSecond timeout)
{
	PRECONDITION(timeout >= 0);
		
	bool locked = false;

	MilliSecond start = GetMilliSeconds();
	
	while (mCount == 0 && (timeout == LONG_MAX || GetMilliSeconds() - start < timeout)) 
		(void) ::YieldToAnyThread();
	
	if (mCount > 0) {
		--mCount;
		locked = true;
	}
		
	return locked;
}
#endif


//---------------------------------------------------------------
//
// XSemaphore::Unlock ()
//
//---------------------------------------------------------------
void XSemaphore::Unlock()
{
#if WIN
	int32 succeeded = ::ReleaseSemaphore(mObject, 1, nil);
	ThrowIf(!succeeded);

#elif MAC
	++mCount;
	POSTCONDITION(mCount <= mMaxCount);
#endif
}


//---------------------------------------------------------------
//
// XSemaphore::Unlock (uint32)
//
//---------------------------------------------------------------
void XSemaphore::Unlock(uint32 count)
{	
#if WIN
	PRECONDITION(count < LONG_MAX);

	if (count > 0) {
		int32 succeeded = ::ReleaseSemaphore(mObject, (int32) count, nil);
		ThrowIf(!succeeded);
	}
	
#elif MAC
	mCount += count;
	POSTCONDITION(mCount <= mMaxCount);
#endif
}


}	// namespace Whisper
