/*
 *  File:       IMouseUpHelper.h
 *  Summary:   	Interface used by XMouseHandler to respond to mouse up events.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IMouseUpHelper.h,v $
 *		Revision 1.2  2001/02/24 09:42:17  jesjones
 *		Made the method pure virtual.
 *		
 *		Revision 1.1  2001/02/04 03:41:22  jesjones
 *		Checked in
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
class XMouseUpEvent;


// ===================================================================================
//	class IMouseUpHelper
//!		Interface used by XMouseHandler to respond to mouse up events.
// ===================================================================================
class IMouseUpHelper : public XUnknown {

//-----------------------------------
//	API
//
public:					
	virtual bool 		OnMouseUp(const XMouseUpEvent& event) = 0;
						/**< Return true if you handled the event. */
};

typedef XInterfacePtr<IMouseUpHelper> IMouseUpHelperPtr;
typedef XInterfacePtr<const IMouseUpHelper> IConstMouseUpHelperPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
