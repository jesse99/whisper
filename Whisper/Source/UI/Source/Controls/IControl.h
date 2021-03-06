/*
 *  File:       XControl.h
 *  Summary:   	An interface that is common to all controls.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: IControl.h,v $
 *		Revision 1.5  2001/03/09 10:19:51  jesjones
 *		Nuked SetHelpID.
 *		
 *		Revision 1.4  2001/01/30 07:10:35  jesjones
 *		Added DoChar and DoKey.
 *		
 *		Revision 1.3  2000/12/14 08:20:40  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:00:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <9>	 8/01/99	JDJ		Added HandleAltChar.
 *		 <8>	 7/19/99	JDJ		Tweaked for Carbon.
 *		 <7>	 6/28/99	JDJ		Added streaming support.
 *		 <6>	 6/27/99	JDJ		GetHelpText, SetHelpText, and SetDisabledHelpText are no longer Mac only.
 *		 <5>	 6/22/99	JDJ		Added support for streaming.
 *		 <4>	 6/11/99	JDJ		Tweaked friend declarations for CW Pro 5fc3.
 *		 <3>	 4/01/99	JDJ		OnClicked has an event argument and calls the XClickExtension's.
 *		 <2>	12/23/98	JDJ		OnContextMenu returns an XContextMenu* instead of
 *									a bool.
 *		 <1>	11/03/98	JDJ		Created
 */

#pragma once

#include <XConstants.h>
#include <XInterfacePtr.h>
#include <XControlConstants.h>
#include <XDrawContext.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XCharEvent;
class XEvent;
class XKeyDownEvent;
class XMouseMoveEvent;
class XRegion;

class IControl;
typedef XInterfacePtr<IControl> IControlPtr;

class IHierarchy;
typedef XInterfacePtr<IHierarchy> IHierarchyPtr;


//-----------------------------------
//	Constants
//
#if MAC
	const int32 kDrawOutset = 3;	///!< the Appearence Manager will draw focus rings outside the control so when we invalidate we need to catch these extra pixels ("Appearance: Not Just Another Pretty Interface" says the maximum outset is three pixels)
#endif


// ===================================================================================
//	class XControlContext
//!		XDrawContext used to draw into a control.
// ===================================================================================
class UI_EXPORT XControlContext  : public XDrawContext {
	
	typedef XDrawContext Inherited;

public:
	virtual				~XControlContext();
						
						XControlContext(const IControlPtr& control);
};


// ===================================================================================
//	class IControl
//!		An interface that is common to all controls.
// ===================================================================================
class IControl : public XUnknown {	
	
//-----------------------------------
//	API
//
public:
	//! @name Name
	//@{
	virtual std::wstring GetName() const = 0;
						/**< Note that the control name is a framework entity: it has nothing
						to do with the control's title or text. */
			
	virtual void 		SetName(const std::wstring& name) = 0;
	//@}

	//! @name Invalidation
	//@{
	virtual void 		Invalidate() = 0;
	virtual void 		Invalidate(const XRect& rect) = 0;
	virtual void 		Invalidate(const XRegion& rgn, bool clipToExtent = true) = 0;
			
	virtual void 		Validate() = 0;
	virtual void 		Validate(const XRect& rect) = 0;
	virtual void 		Validate(const XRegion& rgn, bool clipToExtent = true) = 0;
	
	virtual void 		HandleUpdate() = 0;
						/**< Immediately handles any pending update events in the control's draw context. */
	//@}
						
	//! @name Showing
	//@{
	virtual	bool 		IsVisible() const = 0;
						/**< Controls start out visible. */
			
	virtual void 		Show(bool redraw = kRedraw) = 0;	
	virtual void 		Hide(bool redraw = kRedraw) = 0;
	//@}
		
	//! @name Activating
	//@{
	virtual	bool 		IsActive() const = 0;
						/**< Controls in the frontmost window are active. */
	//@}
		
	//! @name Enabling
	//@{
	virtual	bool 		IsEnabled() const = 0;
						/**< Disabled controls do not respond to user input. */
		
	virtual void 		Enable(bool enable = true, bool redraw = kRedraw) = 0;
	//@}

	//! @name Window <-> Local Conversions
	//@{
	virtual XPoint 		WindowToLocal(const XPoint& pt) const = 0;
	virtual XRect 		WindowToLocal(const XRect& rect) const = 0;
	virtual XRegion		WindowToLocal(const XRegion& rgn) const = 0;
			
	virtual XPoint 		LocalToWindow(const XPoint& pt) const = 0;
	virtual XRect 		LocalToWindow(const XRect& rect) const = 0;
	virtual XRegion  	LocalToWindow(const XRegion& rgn) const = 0;
	//@}
			
	//! @name Parent <-> Local Conversions
	//@{
	virtual XPoint 		ParentToLocal(const XPoint& pt) const = 0;	
	virtual XRect 		ParentToLocal(const XRect& rect) const = 0;
	virtual XRegion 	ParentToLocal(const XRegion& rgn) const = 0;
			
	virtual XPoint 		LocalToParent(const XPoint& pt) const = 0;
	virtual XRect		LocalToParent(const XRect& rect) const = 0;
	virtual XRegion		LocalToParent(const XRegion& rgn) const = 0;
	//@}

	//! @name Misc
	//@{			
	virtual OSControl 	GetOSControl() const = 0;
					
	virtual OSDrawContext GetOSDrawContext() const = 0;
			
	static 	IControlPtr GetControl(OSControl controlH);
						/**< May return nil. */

#if WIN
	virtual	bool 		HandleMessage(const MSG& message, int32& result) = 0;
	
	virtual WNDPROC 	GetSystemWindowProc() const = 0;
	
#elif MAC
	virtual OSWindow 	GetOSWindow() const = 0;
						/**< Appearance Manager controls are always inside windows... */
#endif
	//@}

	//! @name Internals
	//@{			
	virtual void 		DoKey(const XKeyDownEvent& event) = 0;
	virtual void 		DoChar(const XCharEvent& event) = 0;

	virtual void 		DoPostCreate(const std::wstring& name, const IHierarchyPtr& parent, OSControl controlH) = 0;
						/**< Called immediately after creating a new control. */
	//@}
};

typedef XInterfacePtr<IControl> IControlPtr;
typedef XInterfacePtr<const IControl> IConstControlPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
