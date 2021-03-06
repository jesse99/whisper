/*
 *	File:		IDrawExtensionHelper.h
 *	Function:	Interface used by XDrawExtension.
 *	Written by:	Jesse Jones
 *
 *  Copyright � 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: IDrawExtensionHelper.h,v $
 *		Revision 1.1  2001/02/07 05:01:22  jesjones
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
class XDrawContext;
class XRect;


// ===================================================================================
//	class IDrawExtensionHelper
//!		Interface used by XDrawExtension.
// ===================================================================================
class IDrawExtensionHelper : public XUnknown {
		
//-----------------------------------
//	API 
//
public:
	virtual void 		OnDraw(XDrawContext& canvas, const XRect& extent, bool enabled) = 0;
};


typedef XInterfacePtr<IDrawExtensionHelper> IDrawExtensionHelperPtr;
typedef XInterfacePtr<const IDrawExtensionHelper> IConstDrawExtensionHelperPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
