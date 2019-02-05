/*
 *  File:       IAdjustCursorHelper.h
 *  Summary:   	Interface used by XMouseHandler to adjust the cursor.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IAdjustCursorHelper.h,v $
 *		Revision 1.1  2001/02/04 03:41:08  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XMouseMoveEvent;


// ===================================================================================
//	class IAdjustCursorHelper
//!		Interface used by XMouseHandler to adjust the cursor.
// ===================================================================================
class IAdjustCursorHelper : public XUnknown {

//-----------------------------------
//	API
//
public:					
	virtual bool 		OnAdjustCursor(const XMouseMoveEvent& event) = 0;
						/**< Return true if you set the cursor. */
};

typedef XInterfacePtr<IAdjustCursorHelper> IAdjustCursorHelperPtr;
typedef XInterfacePtr<const IAdjustCursorHelper> IConstAdjustCursorHelperPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
