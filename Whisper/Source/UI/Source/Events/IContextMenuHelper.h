/*
 *  File:       IContextMenuHelper.h
 *  Summary:   	Interface used by XMouseHandler to display contextual menus.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IContextMenuHelper.h,v $
 *		Revision 1.1  2001/02/04 03:41:12  jesjones
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
//	class IContextMenuHelper
//!		Interface used by XMouseHandler to display contextual menus.
// ===================================================================================
class IContextMenuHelper : public XUnknown {

//-----------------------------------
//	API
//
public:					
	virtual bool 		OnContextMenu(const XMouseDownEvent& event) = 0;
						/**< Return true if you handled the event. */
};

typedef XInterfacePtr<IContextMenuHelper> IContextMenuHelperPtr;
typedef XInterfacePtr<const IContextMenuHelper> IConstContextMenuHelperPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
