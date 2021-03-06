/*
 *  File:       IMouseDownHelper.h
 *  Summary:   	Interface used by XMouseHandler to respond to mouse down events.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IMouseDownHelper.h,v $
 *		Revision 1.2  2001/02/25 08:26:48  jesjones
 *		Added a (useless) forward ref. I'd roll it back, but I can't figure out how using WinCVS...
 *		
 *		Revision 1.1  2001/02/04 03:41:19  jesjones
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
class XMouseDownEvent;
class XMouseUpEvent;


// ===================================================================================
//	class IMouseDownHelper
//!		Interface used by XMouseHandler to respond to mouse down events.
// ===================================================================================
class IMouseDownHelper : public XUnknown {

//-----------------------------------
//	API
//
public:					
	virtual bool 		OnMouseDown(const XMouseDownEvent& event) = 0;
						/**< Return true if you handled the event. */
};

typedef XInterfacePtr<IMouseDownHelper> IMouseDownHelperPtr;
typedef XInterfacePtr<const IMouseDownHelper> IConstMouseDownHelperPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
