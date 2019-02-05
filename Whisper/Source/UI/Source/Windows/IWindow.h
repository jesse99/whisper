/*
 *  File:       IWindow.h
 *  Summary:   	Main window interface.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IWindow.h,v $
 *		Revision 1.8  2001/04/13 07:47:32  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.7  2001/02/08 03:50:29  jesjones
 *		Removed KCloseBoxCmd.
 *		
 *		Revision 1.6  2001/02/07 02:28:00  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.5  2001/02/04 03:34:23  jesjones
 *		Added FindControl.
 *		
 *		Revision 1.4  2000/12/14 08:26:10  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/11/17 08:54:30  jesjones
 *		Added methods to get and set the window proc.
 *		
 *		Revision 1.2  2000/11/09 09:07:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <15>	10/20/00	JDJ		Removed SWindowStyle::handleClicks and eraseContent. Removed
 *									GetContentColor and SetContentColor.
 *		 <14>	10/18/00	JDJ		Replaced the AutoPosition method with a IPositionWindow interface.
 *		 <13>	 8/05/00	JDJ		Added AutoPosition and SendToBack (from IWindowMgr). Renamed
 *									SendToBack MoveToBack.
 *		 <12>	 8/21/99	JDJ		Added FindLocalizedElement.
 *		 <11>	 8/01/99	JDJ		Added HandleAltChar, HandleMnemonic, and OnMnemonic (Win).
 *		 <10>	 7/18/99	JDJ		Added IsOpened.
 *		 <9>	 6/25/99	JDJ		Changed EWindowType and SWindowStyle to work better with Mac OS 8.5 (and Windows).
 *		 <8>	 6/22/99	JDJ		Added support for streaming.
 *		 <7>	 6/21/99	JDJ		Added __MSL_FIX_ITERATORS__ (for Pro 3).
 *		 <6>	 5/17/99	JDJ		Ctor now takes a parent window argument.
 *		 <5>	 5/17/99	JDJ		Added OnDefaultWindowProc. (Win)
 *		 <4>	 4/06/99	JDJ		Added OnEnterMenuLoop, OnMenuSelect, and OnExitMenuLoop 
 *									to Windows.
 *		 <3>	 3/04/99	JDJ		Added classStyle member to SWindowStyle (Win).
 *		 <2>	 1/10/99	JDJ		Added OnPostClose (Win).
 *		 <1>	10/12/98	JDJ		Created
 */

#pragma once

#include <IMenu.h>
#include <IWindowMgr.h>
#include <XConstants.h>
#include <XRect.h>
#include <XRGBColor.h>
#include <XWindowConstants.h>

#if MAC
	#include <Appearance.h>
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XCharEvent;
class XDrawContext;
class XMouseDownEvent;
class XMouseMoveEvent;
class XMouseUpEvent;
class XRegion;

class IControl;
typedef XInterfacePtr<IControl> IControlPtr;

class IHierarchy;
typedef XInterfacePtr<IHierarchy> IHierarchyPtr;

class IRootControl;
typedef XInterfacePtr<IRootControl> IRootControlPtr;


//-----------------------------------
//	Types and Constants
//
enum EWindowType {kDocumentWindow, kModalDialog, kAlert, kFloatingWindow};

#if MAC
	const int16	kWhisperWindowKind = 20000;	//!< XWindow requires that its windows have a window kind >= kWhisperWindowKind
	const int16	kClickOutsideModal = 42;	//!< Used by HandleClick.
#endif


// ===================================================================================
//	struct SWindowStyle
//!		Used to specify window types.
// ===================================================================================
struct UI_EXPORT SWindowStyle {
	EWindowType			type;
	bool				showNew;			//!< if true HandleOpen will make the window visible
	bool				targetable;			//!< can window or child control become the target of keystrokes and menu commands?
	bool				hideOnSuspend;
	
	// Win fields
	uint32				style;				//!< window style
	uint32				styleEx;			//!< extended window style
	uint32				classStyle;			//!< inited to CS_DBLCLKS
	uint16				iconID;				//!< used when the window is minimized (note that the platform specific members are not #ifdef'ed so we can stream out an XML document that will work on either platform)
	bool				drawSizeGrip;
	
	// Mac fields
	WindowClass			windClass;			//!< defaults to kDocumentWindowClass, kMovableModalWindowClass, kMovableAlertWindowClass, or kFloatingWindowClass
	WindowAttributes	attributes;	
	bool				clickThrough;		//!< delay selection until after handling the click
	bool				getSelectClick;		//!< call OnMouseDown after selecting the window (exclusive with clickThrough)
	
			SWindowStyle(EWindowType type = kDocumentWindow, bool showNew = true);
};


// ===================================================================================
//	class IWindow
//!		Main window interface.
// ===================================================================================
class IWindow : public XUnknown {

//-----------------------------------
//	API
//
public:
	//! @name Initialization
	//@{
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, const XSize& size, const SWindowStyle& style) = 0;
						/**< Window position will be set according to style.autoPosition. */
	//@}
												
	//! @name Opening and Closing
	//@{
	virtual void 		Open() = 0;
						/**< Call this immediately after creating a window. */
						
	virtual bool 		IsOpened() const = 0;
		
	virtual bool		Close(bool quitting, bool forceClose = false) = 0;
						/**< Call this before the window's boss goes out of scope. Returns true if the window was closed. */
	//@}
						
	//! @name Showing
	//@{
	virtual	bool 		IsVisible() const = 0;
			
	virtual void 		Show() = 0;	
	virtual void 		Hide() = 0;
	
	virtual void 		AnimatedShow(const XRect* startBounds) = 0;
	virtual void		AnimatedHide(const XRect* endBounds) = 0;
						/**< These routines will show and hide the window with an accompanying
						theme specific animation. The bounds should be in global coordinates. 
						If bounds is nil the bounds is the center of the window's monitor. */
	//@}
	
	//! @name Front Window
	//@{
	virtual bool 		IsActive() const = 0;
						/**< Returns true if 'this' is the active window (ie a window that can
						accept user input). */
			
	virtual void 		HandleActivate() = 0;
	virtual void		HandleDeactivate() = 0;

	virtual void 		MoveToFront() = 0;
						/**< Makes 'this' the front window in its layer. */

	virtual void 		MoveToBack() = 0;
	//@}
	
	//! @name Name
	//@{
	virtual std::wstring GetName() const = 0;
			
	virtual void 		SetName(const std::wstring& name) = 0;
	//@}

	//! @name Invalidation
	//@{
	virtual void 		Invalidate() = 0;
	virtual void 		Invalidate(const XRect& rect) = 0;
	virtual void 		Invalidate(const XRegion& rgn) = 0;
			
	virtual void 		Validate() = 0;
	virtual void 		Validate(const XRect& rect) = 0;
	virtual void 		Validate(const XRegion& rgn) = 0;
	
	virtual void 		HandleUpdate() = 0;
						/**< Immediately handles any pending update events. */
	//@}
					
	//! @name Coordinate Conversions
	//@{
	virtual XPoint 		GlobalToWindow(const XPoint& pt) const = 0;	
	virtual XRect 		GlobalToWindow(const XRect& rect) const = 0;
	
	virtual XPoint 		WindowToGlobal(const XPoint& pt) const = 0;	
	virtual XRect 		WindowToGlobal(const XRect& rect) const = 0;
	//@}
	
	//! @name Misc Getters
	//@{		
	virtual OSWindow 	GetOSWindow() const = 0;
	virtual OSDrawContext GetOSDrawContext() const = 0;
							
	virtual EWindowType GetType() const = 0;
	virtual EWindowLayer GetLayer() const = 0;
	virtual const SWindowStyle& GetStyle() const = 0;

	virtual IRootControlPtr GetRootControl() const = 0;

	static 	IWindowPtr GetWindow(OSWindow wptr);
						/**< May return nil. */
	//@}

	//! @name Misc
	//@{	
	virtual void 		ForceOnScreen() = 0;
						/**< Moves the window so that a minimal drag area is accessible. */
	//@}
								
//-----------------------------------
//	Internal API
//
public:
#if MAC
	virtual IControlPtr FindControl(const XPoint& loc) const = 0;

	virtual void 		HandleSuspend() = 0;
	virtual void 		HandleResume() = 0;
	
	virtual XRect 		GetContentBounds() const = 0;

	virtual void 		Zoom(int16 part) = 0;

#elif WIN
	virtual bool 		HandleAltChar(const XCharEvent& event) = 0;

	virtual	bool 		HandleMessage(const MSG& msg, int32& result) = 0;

	virtual void		HandleMenuSelect(const IMenu::SHelp& help, uint16 flags) = 0;
	
	virtual void 		DoComputeClientInset() = 0;
	
	typedef LRESULT WINAPI (*WindowProc)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void 		SetWindowProc(WindowProc proc) = 0;
	virtual WindowProc 	GetWindowProc() = 0;
#endif

	virtual XRect 		GetClientAreaInset() = 0;
};

typedef XInterfacePtr<IWindow> IWindowPtr;
typedef XInterfacePtr<const IWindow> IConstWindowPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
