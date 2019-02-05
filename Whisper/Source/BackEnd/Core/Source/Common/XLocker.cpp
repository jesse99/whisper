/*
 *  File:       XLocker.cpp
 *  Summary:    Stack based class that locks things.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XLocker.cpp,v $
 *		Revision 1.3  2001/04/27 04:34:17  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:42:20  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XLocker.h>

#if MAC
	#include <Memory.h>
#endif

#include <XLockable.h>

namespace Whisper {


// ===================================================================================
//	class XLocker
// ===================================================================================

//----------------------------------------------------------------
//
// XLocker::~XLocker
//
//----------------------------------------------------------------
XLocker::~XLocker()
{
#if MAC
	if (mObject != nil)
		mObject->Unlock();
	else
		HSetState(mHandle, mOldState);

#elif WIN
	mObject->Unlock();
#endif

	mObject = nil;
}


//---------------------------------------------------------------
//
// XLocker::XLocker (XBaseLockableMixin, bool)
//
//---------------------------------------------------------------
XLocker::XLocker(const XBaseLockableMixin& object, bool moveHigh) : mObject(const_cast<XBaseLockableMixin*>(&object))
{
	mObject->Lock(moveHigh); 
	
#if MAC
	mHandle = nil;				// Locking an XBaseLockableMixin instead of a toolbox Handle.
#endif
}


//---------------------------------------------------------------
//
// XLocker::XLocker (XBaseLockableMixin*, bool)
//
//---------------------------------------------------------------
XLocker::XLocker(const XBaseLockableMixin* object, bool moveHigh) 
{
	PRECONDITION(object != nil); 
	
	mObject = const_cast<XBaseLockableMixin*>(object);
	mObject->Lock(moveHigh); 
	
#if MAC
	mHandle = nil;				// Locking an XBaseLockableMixin instead of a toolbox Handle.
#endif
}


//----------------------------------------------------------------
//
// XLocker::XLocker (Handle, bool)
//
//----------------------------------------------------------------
#if MAC
XLocker::XLocker(Handle hand, bool moveHigh)
{
	PRECONDITION(hand != nil);
	PRECONDITION(*hand != nil);
	
	mHandle = hand; 
	mOldState = HGetState(mHandle);

	if (!(mOldState & 0x0080))			// handle state bit 7 is the "locked" bit
		if (moveHigh)
			HLockHi(mHandle);
		else
			HLock(mHandle);
	
	mObject = nil;						// Locking a toolbox Handle instead of an XBaseLockableMixin.
}
#endif	// MAC


}	// namespace Whisper


