/*
 *  File:       XLocker.h
 *  Summary:    Stack based class that locks things.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XLocker.h,v $
 *		Revision 1.4  2001/03/05 05:42:25  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:53:55  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:42:24  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XConstants.h>
#include <XDebug.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XBaseLockableMixin;


// ===================================================================================
//	class XLocker
//!		Stack based class that locks things.
// ===================================================================================
class CORE_EXPORT XLocker {

public:
				~XLocker();

				XLocker(const XBaseLockableMixin& object, bool moveHigh = kDontMoveHigh);

				XLocker(const XBaseLockableMixin* object, bool moveHigh = kDontMoveHigh);

#if MAC
				XLocker(Handle hand, bool moveHigh = kDontMoveHigh);
#endif

private:
	XBaseLockableMixin*	mObject;
	
#if MAC
	Handle				mHandle;
	SignedByte			mOldState;
#endif
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

