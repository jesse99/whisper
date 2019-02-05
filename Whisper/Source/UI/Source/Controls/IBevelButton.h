/*
 *  File:       IBevelButton.h
 *  Summary:   	A control that displays a button containing text or an image and an
 *				optional menu.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IBevelButton.h,v $
 *		Revision 1.1  2001/03/20 11:57:42  jesjones
 *		Checked in (sans menu support and with minimal testing)
 *		
 */

#pragma once

#include <XConstants.h>
#include <XControlConstants.h>
#include <XInterfacePtr.h>
#include <XWindowConstants.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XPixMap;
class XRect;

class IHierarchy;
typedef XInterfacePtr<IHierarchy> IHierarchyPtr;


//-----------------------------------
//	Constants
//
const uint32 kDefaultBevelButtonProcID = kControlBevelButtonNormalBevelProc; 				// you can change this to make the bevel more or less pronounced or to make the menu popup on the right
const uint32 kDefaultBevelButtonStyle  = WS_CHILD + WS_VISIBLE + WS_TABSTOP + BS_PUSHLIKE;	// ctors will add BS_BITMAP, BS_ICON, BS_TEXT, etc

enum EBevelBehavior {kBevelResets,		// value is reset to 0 after user releases mouse (like a push button)
					 kBevelToggles,		// value toggles between 1 and 0 (like a checkbox)
					 kBevelSticks};		// value is set to 1 (like a radio button)

enum EBevelTextAlignment {kAlignBevelTextLeft,		
						  kAlignBevelTextRight,
						  kAlignBevelTextCenter,
						  kAlignBevelTextSystem};


// ===================================================================================
//	class IBevelButton
//!		A control that displays a button containing text or an image and an optional menu.
// ===================================================================================
class IBevelButton : public XUnknown {

//-----------------------------------
//	API
//
public:
	//! @name Initialization
	//@{
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring& text,
							 EBevelBehavior behavior,
							 EBevelTextAlignment align = kAlignBevelTextCenter,
							 uint32 style = kDefaultBevelButtonStyle,
							 uint32 procID = kDefaultBevelButtonProcID) = 0;
						/**< Button draws text. $$$ we don't yet support menus $$$ */

	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, ResID iconID,
							 EBevelBehavior behavior,
							 uint32 style = kDefaultBevelButtonStyle,
							 uint32 procID = kDefaultBevelButtonProcID) = 0;
						/**< Button draws a color icon. */

	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const XPixMap& image,
							 EBevelBehavior behavior,
							 uint32 style = kDefaultBevelButtonStyle,
							 uint32 procID = kDefaultBevelButtonProcID) = 0;
						/**< Button draws an image. */

#if MAC
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const ControlButtonContentInfo& content,
							 EBevelBehavior behavior,
							 int16 align = kControlBevelButtonAlignCenter,
							 uint32 style = kDefaultBevelButtonStyle,
							 uint32 procID = kDefaultBevelButtonProcID) = 0;
						/**< Button draws a color icon, an icon suite, an icon reference (OS 8.5), or a pict. */
#endif
	//@}

	//! @name Text
	//@{
	virtual void 		SetTextAlignment(EBevelTextAlignment align, bool redraw = kRedraw) = 0;
						/**< Note that on a Mac bevel buttons can contain both text and an image. */
	
#if MAC
	virtual void 		SetTextPlacement(int16 placement, bool redraw = kRedraw) = 0;
						/**< Can be kControlBevelButtonPlaceSysDirection, kControlBevelButtonPlaceNormally, 
						kControlBevelButtonPlaceToRightOfGraphic, kControlBevelButtonPlaceToLeftOfGraphic, 
						kControlBevelButtonPlaceBelowGraphic, or kControlBevelButtonPlaceAboveGraphic. */
#endif
	//@}

	//! @name Image
	//@{
	virtual void 		SetImage(ResID iconID, bool redraw = kRedraw) = 0;
	virtual void 		SetImage(const XPixMap& image, bool redraw = kRedraw) = 0;

#if MAC
	virtual void 		SetImageAlignment(int16 align, bool redraw = kRedraw) = 0;
						/**< Can be kControlBevelButtonAlignSysDirection, kControlBevelButtonAlignCenter, 
						kControlBevelButtonAlignLeft, kControlBevelButtonAlignRight, kControlBevelButtonAlignTop, 
						kControlBevelButtonAlignBottom, kControlBevelButtonAlignTopLeft, kControlBevelButtonAlignBottomLeft, 
						kControlBevelButtonAlignTopRight, or kControlBevelButtonAlignBottomRight. */
#endif
	//@}
};


typedef XInterfacePtr<IBevelButton> IBevelButtonPtr;
typedef XInterfacePtr<const IBevelButton> IConstBevelButtonPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
