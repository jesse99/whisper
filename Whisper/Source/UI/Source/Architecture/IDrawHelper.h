/*
 *  File:       IDrawHelper.h
 *  Summary:   	Interface called by XDraw to do the actual drawing.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IDrawHelper.h,v $
 *		Revision 1.1  2001/01/27 06:33:36  jesjones
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
class XRegion;


// ===================================================================================
//	class IDrawHelper
//!		Interface called by XDraw to do the actual drawing.
// ===================================================================================
class IDrawHelper : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		OnDraw(XDrawContext& canvas, const XRegion& dirtyRgn, bool isActive) = 0;
};

typedef XInterfacePtr<IDrawHelper> IDrawHelperPtr;
typedef XInterfacePtr<const IDrawHelper> IConstDrawHelperPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
