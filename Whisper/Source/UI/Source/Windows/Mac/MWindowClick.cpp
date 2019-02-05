/*
 *  File:       MWindowClick.cpp
 *  Summary:   	IWindowClick implementation that handles the standard window parts.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MWindowClick.cpp,v $
 *		Revision 1.4  2001/04/13 07:54:12  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2001/03/01 11:41:28  jesjones
 *		Added #if DEBUG to a bit of code.
 *		
 *		Revision 1.2  2001/02/08 03:53:22  jesjones
 *		Updated the close window stuff so that it works again.
 *		
 *		Revision 1.1  2001/02/07 02:32:15  jesjones
 *		Checked in	
 */

#include <XWhisperHeader.h>
#include <IWindowClick.h>

#include <Sound.h>

#include <IControl.h>
#include <IDocuments.h>
#include <IGeometry.h>
#include <IMenuHandler.h>
#include <IMouseHandler.h>
#include <IWindow.h>
#include <MSystemInfo.h>
#include <XMouseEvents.h>

namespace Whisper {


// ===================================================================================
//	class MWindowClick
//!		IWindowClick implementation that handles the standard window parts.
// ===================================================================================
class MWindowClick : public IWindowClick {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~MWindowClick();
	
						MWindowClick(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual bool 		OnClick(const XMouseDownEvent& globalEvent, int16 partCode);

//-----------------------------------
//	Internal API
//
private:
			bool 		DoIsDoubleClick(const XMouseDownEvent& oldEvent, const XMouseDownEvent& newEvent) const;
			bool 		DoIsContextClick(const XMouseDownEvent& event) const;
						
			void 		DoClickInContent(const XMouseDownEvent& globalEvent);
			void 		DoContentClick(const XMouseDownEvent& globalEvent);
			void 		DoContextClick(const XMouseDownEvent& globalEvent);

			void 		DoClickInGoAway(const XMouseDownEvent& globalEvent);
			void 		DoClickInDrag(const XMouseDownEvent& globalEvent);
			void 		DoClickInGrow(const XMouseDownEvent& globalEvent);
			void 		DoClickInZoom(const XMouseDownEvent& globalEvent);
};

DEFINE_INTERFACE_FACTORY(MWindowClick)

//---------------------------------------------------------------
//
// MWindowClick::~MWindowClick
//
//---------------------------------------------------------------
MWindowClick::~MWindowClick()
{
}


//---------------------------------------------------------------
//
// MWindowClick::MWindowClick
//
//---------------------------------------------------------------
MWindowClick::MWindowClick(XBoss* boss) 
{	
	IWindowClick::DoSetBoss(boss);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// MWindowClick::OnClick
//
//---------------------------------------------------------------
bool MWindowClick::OnClick(const XMouseDownEvent& globalEvent, int16 partCode)
{	
	bool handled = true;
		
	switch (partCode) {
		case kClickOutsideModal:
#if DEBUG
			if (!gMonkeyLives)	
#endif
				::SysBeep(1);			
			break;
	
		case inContent:
			this->DoClickInContent(globalEvent);
			break;
			
		case inDrag:
		case inProxyIcon:						// MDocWindowClick should have been called before us...
			this->DoClickInDrag(globalEvent);
			break;
			
		case inGrow:
			this->DoClickInGrow(globalEvent);
			break;
			
		case inGoAway:
			this->DoClickInGoAway(globalEvent);
			break;
			
		case inZoomIn:
		case inZoomOut:
			this->DoClickInZoom(globalEvent);
			break;
			
		default:
			handled = false;
	}
	
	return handled;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// MWindowClick::DoIsDoubleClick
//
//---------------------------------------------------------------
bool MWindowClick::DoIsDoubleClick(const XMouseDownEvent& oldEvent, const XMouseDownEvent& newEvent) const
{
	bool doubleClick = false;
	
	if (Abs((int) (newEvent.GetOSEvent().when - oldEvent.GetOSEvent().when)) <= ::GetDblTime()) {
		XPoint deltaPt = newEvent.GetPosition() - oldEvent.GetPosition();
		
		int32 delta = (int32) deltaPt.x*deltaPt.x + (int32) deltaPt.y*deltaPt.y;
		
		doubleClick = delta <= 16;
	}
	
	return doubleClick;
}


//---------------------------------------------------------------
//
// MWindowClick::DoIsContextClick
//
//---------------------------------------------------------------
bool MWindowClick::DoIsContextClick(const XMouseDownEvent& event) const
{
	bool isContext;
	
	if (MSystemInfo::HasContextMenus())	{
		EventRecord temp = event.GetOSEvent();
		isContext = IsShowContextualMenuClick(&temp);	
	} else
		isContext = event.WasControlKeyDown();
	
	return isContext;
}


//---------------------------------------------------------------
//
// MWindowClick::DoClickInContent
//
//---------------------------------------------------------------
void MWindowClick::DoClickInContent(const XMouseDownEvent& globalEvent)
{
	IWindowMgrPtr windowMgr(L"Application");

	IWindowPtr window(this);
	OSWindow wptr = window->GetOSWindow();
	const SWindowStyle& style = window->GetStyle();
	
	if (style.clickThrough) {
		EWindowLayer layer = window->GetLayer();
		IWindowPtr front = windowMgr->GetTop(layer);
		bool doSelect = true;
		
		if (this->DoIsContextClick(globalEvent))
			this->DoContextClick(globalEvent);
		else
			this->DoContentClick(globalEvent);
			
		// Don't select the window if the mouse was released outside
		// of the window.
		EventRecord	mouseUpEvent;
		if (!::StillDown() && ::GetNextEvent(mUpMask, &mouseUpEvent)) {
			WindowPtr upWindow;
			::FindWindow(mouseUpEvent.where, &upWindow);
			doSelect = upWindow == wptr;
		}

		// If the front window has changed don't select the window!
		if (doSelect && front == windowMgr->GetTop(layer))		
			window->MoveToFront();

	} else if (style.getSelectClick) {
		if (!window->IsActive())
			::SelectWindow(wptr);

		if (this->DoIsContextClick(globalEvent))
			this->DoContextClick(globalEvent);
		else
			this->DoContentClick(globalEvent);
	
	} else {
		if (!window->IsActive())
			::SelectWindow(wptr);
			
		else 
			if (this->DoIsContextClick(globalEvent))
				this->DoContextClick(globalEvent);
			else
				this->DoContentClick(globalEvent);
	}
}


//---------------------------------------------------------------
//
// MWindowClick::DoContentClick
//
//---------------------------------------------------------------
void MWindowClick::DoContentClick(const XMouseDownEvent& globalEvent)
{	
	IWindowPtr window(this);

	// Convert the click location to window coordinates
	XMouseDownEvent event = globalEvent;
	XPoint pt = window->GlobalToWindow(globalEvent.GetPosition());
	event.SetPosition(pt);
	
	// See if its a double click
	uint32 clickCount;
	static XMouseDownEvent lastMouseDown;	
	if (this->DoIsDoubleClick(lastMouseDown, event))
		clickCount = lastMouseDown.GetClickCount() + 1;
	else
		clickCount = 1;
	event.SetClickCount(clickCount);
	
	// Try to route the click to a control
	bool handled = false;
	IControlPtr control = window->FindControl(event.GetPosition());
	if (control) {	
		XMouseDownEvent childEvent = event;
		XPoint pos = control->WindowToLocal(event.GetPosition());
		childEvent.SetPosition(pos);
	
		IMouseHandlerPtr handler(control);
		if (handler)	
			handled = handler->HandleMouseDown(childEvent);
	}
	
	// If a control didn't handle it we'll handle it
	if (!handled) {
		IMouseHandlerPtr handler(this);
		if (handler)	
			handled = handler->HandleMouseDown(event);
	}
	
	lastMouseDown = event;
}


//---------------------------------------------------------------
//
// MWindowClick::DoContextClick
//
//---------------------------------------------------------------
void MWindowClick::DoContextClick(const XMouseDownEvent& globalEvent)
{
	IWindowPtr window(this);

	XMouseDownEvent event = globalEvent;
	XPoint pt = window->GlobalToWindow(globalEvent.GetPosition());
	event.SetPosition(pt);
	
	bool handled = false;
	
	IControlPtr control = window->FindControl(event.GetPosition());
	if (control) {	
		XMouseDownEvent childEvent = event;
		XPoint pos = control->WindowToLocal(event.GetPosition());
		childEvent.SetPosition(pos);
	
		IMouseHandlerPtr handler(control);
		if (handler)	
			handled = handler->HandleContextMenu(childEvent);
	}
	
	if (!handled) {
		IMouseHandlerPtr handler(this);
		if (handler)	
			handled = handler->HandleContextMenu(event);
	}
}


//---------------------------------------------------------------
//
// MWindowClick::DoClickInGoAway
//
//---------------------------------------------------------------
void MWindowClick::DoClickInGoAway(const XMouseDownEvent& globalEvent)
{
	IWindowPtr window(this);
	const SWindowStyle& style = window->GetStyle();
	ASSERT(style.attributes & kWindowCloseBoxAttribute);
	
	OSWindow wptr = window->GetOSWindow();
	
	if (::TrackGoAway(wptr, globalEvent.GetPosition().GetOSPoint())) {
		IDocumentsPtr docs(L"Application");
		if (globalEvent.WasOptionKeyDown() && docs) 
			docs->CloseAll();
		
		else if (globalEvent.WasCommandKeyDown() && docs)
			docs->ForceCloseAll();
		
		else
			window->Close(false);
	}
}


//---------------------------------------------------------------
//
// MWindowClick::DoClickInDrag
//
//---------------------------------------------------------------
void MWindowClick::DoClickInDrag(const XMouseDownEvent& globalEvent)
{
	IWindowPtr window(this);
	OSWindow wptr = window->GetOSWindow();
	
	XRegion deskTopRgn = ::GetGrayRgn();
	XRect dragRect = deskTopRgn.GetEnclosingRect();
	dragRect.Inset(4, 4);
		
	Rect temp = dragRect.GetOSRect();
	::DragWindow(wptr, globalEvent.GetOSEvent().where, &temp);
}
	

//---------------------------------------------------------------
//
// MWindowClick::DoClickInGrow
//
//---------------------------------------------------------------
void MWindowClick::DoClickInGrow(const XMouseDownEvent& globalEvent)
{
	IWindowPtr window(this);
	const SWindowStyle& style = window->GetStyle();
	ASSERT(style.attributes & kWindowResizableAttribute);
	
	IGeometryPtr geometry(this);
	XSize minSize = geometry->GetMinSize();
	XSize maxSize = geometry->GetMaxSize();

	Rect minMaxSize = XRect(minSize.width, minSize.height, maxSize.width, maxSize.height).GetOSRect();

	OSWindow wptr = window->GetOSWindow();
	if (MSystemInfo::HasWindowMgr2() && false) {	// $$$ÊResizeWindow does nothing in 8.5fc2...
		Rect newFrame;
		bool resized = ::ResizeWindow(wptr, globalEvent.GetPosition().GetOSPoint(), &minMaxSize, &newFrame);
				
		if (resized)
			geometry->SetFrame(newFrame);
					
	} else {
		// GrowWindow is off by one when growing to max size.
		minMaxSize.right++;
		minMaxSize.bottom++;

		int32 newSize = ::GrowWindow(wptr, globalEvent.GetPosition().GetOSPoint(), &minMaxSize);
				
		// Occasionally returns zero when you try to grow a window past its max size.
		if (newSize != 0) {
			geometry->SetSize(XSize(LoWord(newSize), HiWord(newSize)));
			
			// Make sure size box gets redrawn.
			XRect sizeBox = geometry->GetExtent();
			sizeBox.left = sizeBox.right - 16;
			sizeBox.top  = sizeBox.bottom - 16;
			window->Invalidate(sizeBox);
		}
	}
}


//---------------------------------------------------------------
//
// MWindowClick::DoClickInZoom
//
//---------------------------------------------------------------
void MWindowClick::DoClickInZoom(const XMouseDownEvent& globalEvent)
{
	IWindowPtr window(this);
	OSWindow wptr = window->GetOSWindow();
	
	IGeometryPtr geometry(this);
	Point idealSize = geometry->GetMaxSize().GetOSPoint();
	
	int16 part;
	if (::IsWindowInStandardState(wptr, &idealSize, nil))	// this is how Apple recommends getting the zoom direction in the brave new Window Manager 2.0 world
		part = inZoomIn;
	else
		part = inZoomOut;

	if (::TrackBox(wptr, globalEvent.GetPosition().GetOSPoint(), part))
		window->Zoom(part);
}


}	// namespace Whisper
