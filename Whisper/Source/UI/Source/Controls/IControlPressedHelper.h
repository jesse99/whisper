/*
 *  File:       IControlPressedHelper.h
 *  Summary:   	Interface that is called when the mouse is pressed and released inside a control.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IControlPressedHelper.h,v $
 *		Revision 1.1  2001/02/04 03:41:16  jesjones
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


// ===================================================================================
//	class IControlPressedHelper
//!		Interface that is called when the mouse is pressed and released inside a control.
// ===================================================================================
class IControlPressedHelper : public XUnknown {

//-----------------------------------
//	API
//
public:					
	virtual void 		OnPressed(const XMouseDownEvent& event) = 0;
};

typedef XInterfacePtr<IControlPressedHelper> IControlPressedHelperPtr;
typedef XInterfacePtr<const IControlPressedHelper> IConstControlPressedHelperPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
