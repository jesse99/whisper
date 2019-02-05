/*
 *  File:       IColorSwatch.h
 *  Summary:   	A control that draws a colored square and lets the user change the
 *				color using an OS color picker dialog.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IColorSwatch.h,v $
 *		Revision 1.1  2000/12/17 03:38:53  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XCallbacks.h>
#include <XColorSet.h>
#include <XConstants.h>
#include <XInterfacePtr.h>
#include <XRGBColor.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IColorSwatch
//!		A control that draws a colored square and lets the user change the color using an OS color picker dialog.
// ===================================================================================
class IColorSwatch : public XUnknown {

//-----------------------------------
//	Types
//
public:	
	typedef XCallback1<void, XRGBColor> ChangeCallback;

//-----------------------------------
//	API
//
public:
	// ----- Color -----
	virtual XRGBColor 	GetColor() const = 0;
	virtual void 		SetColor(const XRGBColor& color, bool redraw = kRedraw) = 0;

	// ----- Prompt -----
	virtual void 		SetPrompt(const std::wstring& prompt) = 0;
						/**< Displayed in the color picker dialog. */

	// ----- Color Set -----
	virtual void 		SetColorSet(const XColorSet& colors) = 0;
						/**< Used with the context menu. Defaults to kCrayolaColors. */

	// ----- Notification -----
	virtual void 		SetChangedCallback(const ChangeCallback& callback) = 0;
						/**< Called if the user changes the color. Default does nothing. */
};

typedef XInterfacePtr<IColorSwatch> IColorSwatchPtr;
typedef XInterfacePtr<const IColorSwatch> IConstColorSwatchPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
