/*
 *  File:       XLockable.cpp
 *  Summary:    Mixin for lockable objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XLockable.cpp,v $
 *		Revision 1.4  2001/04/27 04:34:14  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/04/17 01:44:18  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.2  2000/11/09 12:42:12  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XLockable.h>

#include <XDebug.h>
#include <XInvariant.h>

namespace Whisper {


// ===================================================================================
//	class XBaseLockableMixin
// ===================================================================================

//----------------------------------------------------------------
//
// XBaseLockableMixin::~XBaseLockableMixin
//
//----------------------------------------------------------------
XBaseLockableMixin::~XBaseLockableMixin()
{
}


//----------------------------------------------------------------
//
// XBaseLockableMixin::XBaseLockableMixin
//
//----------------------------------------------------------------
XBaseLockableMixin::XBaseLockableMixin()
{
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XLockableMixin
// ===================================================================================

//----------------------------------------------------------------
//
// XLockableMixin::~XLockableMixin
//
//----------------------------------------------------------------
XLockableMixin::~XLockableMixin()
{
	PRECONDITION(mLockCount == 0);		// Make sure calls to Lock are balanced by calls to Unlock.
}


//----------------------------------------------------------------
//
// XLockableMixin::XLockableMixin
//
//----------------------------------------------------------------
XLockableMixin::XLockableMixin()
{
	mLockCount = 0;
}


//----------------------------------------------------------------
//
// XLockableMixin::Lock
//
//----------------------------------------------------------------
void XLockableMixin::Lock(bool moveHigh)
{
	PRECONDITION(mLockCount >= 0);							
	
	if (mLockCount == 0) 
		this->OnLock(moveHigh);
	
	mLockCount++;
				
	POSTCONDITION(mLockCount > 0);
}		


//----------------------------------------------------------------
//
// XLockableMixin::Unlock
//
//----------------------------------------------------------------
void XLockableMixin::Unlock()
{
	PRECONDITION(mLockCount >= 1);							// Check for Unlock without prior lock.
	
	if (mLockCount == 1) 
		this->OnUnlock();
	
	mLockCount--;
				
	POSTCONDITION(mLockCount >= 0);
}	


//----------------------------------------------------------------
//
// XLockableMixin::IsLocked
//
//----------------------------------------------------------------
bool XLockableMixin::IsLocked() const
{
	bool locked = mLockCount > 0;
	
	return locked;
}


}	// namespace Whisper

