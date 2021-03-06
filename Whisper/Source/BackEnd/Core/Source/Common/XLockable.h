/*
 *  File:       XLockable.h
 *  Summary:    Mixin for lockable objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XLockable.h,v $
 *		Revision 1.4  2001/03/05 05:42:22  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:53:51  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:42:16  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XConstants.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XBaseLockableMixin
//!		Abstract base class for things that can be locked (eg handles).
// ===================================================================================
class CORE_EXPORT XBaseLockableMixin {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XBaseLockableMixin();

						XBaseLockableMixin();

//-----------------------------------
//	API
//
public:					
	virtual void 		Lock(bool moveHigh = kDontMoveHigh) = 0;
						/**< Note that moveHigh is just a hint. kMoveHigh should be 
						used when the object is going to be locked for a long time. */

	virtual void 		Unlock() = 0;

	virtual bool 		IsLocked() const = 0;
};


// ===================================================================================
//	class XLockableMixin
//!		A lockable mixin that maintains a lock count.
// ===================================================================================
class CORE_EXPORT XLockableMixin : public XBaseLockableMixin {

	typedef XBaseLockableMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XLockableMixin();

						XLockableMixin();

//-----------------------------------
//	Inherited API
//
public:					
	virtual void 		Lock(bool moveHigh = kDontMoveHigh);

	virtual void 		Unlock();

	virtual bool 		IsLocked() const;

//-----------------------------------
//	New API
//
protected:
	virtual void 		OnLock(bool moveHigh) = 0;

	virtual void 		OnUnlock() = 0;

//-----------------------------------
//	Member Data
//
protected:
	int32	mLockCount;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

