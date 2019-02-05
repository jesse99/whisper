/*
 *  File:       XCriticalSection.cpp
 *  Summary:   	Provides a fast way to prevent other threads from touching a shared resource. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XCriticalSection.cpp,v $
 *		Revision 1.3  2001/04/27 04:31:35  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:29:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XCriticalSection.h>

#include <XDebug.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class XCriticalSection
// ===================================================================================

//---------------------------------------------------------------
//
// XCriticalSection::~XCriticalSection
//
//---------------------------------------------------------------
XCriticalSection::~XCriticalSection()
{
#if WIN
	::DeleteCriticalSection(const_cast<CRITICAL_SECTION*>(&mObject));
#endif
}


//---------------------------------------------------------------
//
// XCriticalSection::XCriticalSection
//
//---------------------------------------------------------------
XCriticalSection::XCriticalSection()
{
#if WIN
	::InitializeCriticalSection(const_cast<CRITICAL_SECTION*>(&mObject));
#elif MAC
	mOwner = kNoThreadID;
	mLockCount = 0;
#endif
}


//---------------------------------------------------------------
//
// XCriticalSection::Lock
//
//---------------------------------------------------------------
#if MAC
void XCriticalSection::Lock()
{
	PRECONDITION(mLockCount >= 0);

	ThreadID current;
	OSErr err = ::GetCurrentThread(&current);
	ThrowIfOSErr(err);

	if (current != mOwner) {
		while (mOwner != kNoThreadID) 		// block until the owner calls Leave
			(void) ::YieldToAnyThread();
		
		err = ::GetCurrentThread(const_cast<ThreadID*>(&mOwner));
		ThrowIfOSErr(err);
	}
	
	++mLockCount;
	
	ASSERT(mOwner != kNoThreadID);
}
#endif


//---------------------------------------------------------------
//
// XCriticalSection::Unlock
//
//---------------------------------------------------------------
#if MAC
void XCriticalSection::Unlock()
{
	PRECONDITION(mLockCount > 0);
	
#if DEBUG
	ThreadID current;
	OSErr err = ::GetCurrentThread(&current);
	ASSERT(err == noErr);						// called from XEnterCriticalSection dtor so we don't want to throw!
	
	ASSERT(mOwner == current);			
#endif
	
	if (--mLockCount == 0)
		mOwner = kNoThreadID;
}
#endif


}	// namespace Whisper
