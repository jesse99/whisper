/*
 *  File:       IStaticText.h
 *  Summary:   	Interface for a control that displays non-editable text.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IStaticText.h,v $
 *		Revision 1.4  2001/03/05 05:38:42  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:25:15  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:07:16  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XConstants.h>
#include <XControlConstants.h>
#include <XInterfacePtr.h>
#include <XFont.h>
#include <XUnknown.h>
#include <XWindowConstants.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XRect;

class IHierarchy;
typedef XInterfacePtr<IHierarchy> IHierarchyPtr;


//-----------------------------------
//	Constants
//
const uint32 kDefaultStaticTextProcID = kControlStaticTextProc; 
const uint32 kDefaultStaticTextStyle  = WS_CHILD + WS_VISIBLE + SS_LEFT;


// ===================================================================================
//	class IStaticText
//!		Interface for a control that displays non-editable text.
// ===================================================================================
class IStaticText : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Initialization -----
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring& text, 
							 const XFont& font = kSmallSystemFont,
							 uint32 style = kDefaultStaticTextStyle,
							 uint32 procID = kDefaultStaticTextProcID) = 0;
	
	// ----- Font -----
	virtual const XFont& GetFont() const = 0;

	virtual void 		SetFont(const XFont& font, bool redraw = kRedraw) = 0;

	// ----- Alignment -----
	virtual ETextAlignment GetAlignment() const = 0;
						/**< Defaults to kLeftTopAlign. */

	virtual void 		SetAlignment(ETextAlignment alignment, bool redraw = kRedraw) = 0;
						/**< Alignment can be kLeftTopAlign, kCenterTopAlign, kRightTopAlign, 
						or kDefaultAlign. */
};


typedef XInterfacePtr<IStaticText> IStaticTextPtr;
typedef XInterfacePtr<const IStaticText> IConstStaticTextPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
