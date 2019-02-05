/*
 *  File:       MWindow.cpp
 *  Summary:   	A window that can contain controls.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MControlWindow.cpp,v $
 *		Revision 1.4  2001/04/21 03:24:36  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/13 07:49:08  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.2  2000/11/09 09:11:25  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <8>	 7/20/99	JDJ		Tweaked for Carbon.
 *		 <7>	 7/05/99	JDJ		Dtor only calls CHECK_INVARIANT if mOpened is true.
 *		 <6>	 6/22/99	JDJ		Added support for streaming.
 *		 <5>	 6/13/99	JDJ		No longer uses FindControlUnderMouse (didn't work 
 *									with controls embedded within XScrollableControls).
 *		 <4>	 6/13/99	JDJ		OnDeactivating no longer calls StopIdling.
 *		 <3>	 1/12/99	JDJ		OnAdjustCursor calls through to the control the
 *									mouse is over (instead of the root control).
 *		 <2>	 1/06/99	JDJ		OnSetSize accounts for min/max sizes.
 *		 <1>	11/05/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XWindow.h>

#include <XExceptions.h>
#include <XMouseEvents.h>
#include <XRootControl.h>
#include <XXMLDoc.h>

namespace Whisper {


// ===================================================================================
//	class XWindow
// ===================================================================================

//---------------------------------------------------------------
//
// XWindow::~XWindow
//
//---------------------------------------------------------------
XWindow::~XWindow()
{
	if (mOpened)
		CALL_INVARIANT;					// Invariant may not hold if there was an exception in HandleOpen
	
	if (mRootControl != nil) {
		XRootControl* root = mRootControl;
		mRootControl = nil;					// so XRootControl dtor doesn't ASSERT
		
		delete root;
		
		DisposeControl(mActualRoot);
	}
}


//---------------------------------------------------------------
//
// XWindow::XWindow
//
// On the Mac it'd be more natural if XRootControl was a real
// Control Manager root control. Unfortunately there are some
// weaknesses in the Appearance Manager that make this difficult.
// The first problem is that Control Manager root controls are
// always maximially sized and their location and size cannot be 
// changed. This is annoying, but it's not difficult to come up
// with work arounds. A more serious problem is that EmbedControl
// doesn't allow us to detach the control from a window and doesn't
// allow us to place the control in a different window. So, what
// we do is have two root controls: the Control Manager root and
// the Whisper roots. All attached controls are placed inside the
// Whisper root. When RemoveChild is called the control is hidden
// and placed inside the Control Manager root.
//
//---------------------------------------------------------------
XWindow::XWindow(OSWindow parent, const std::wstring& name, const XSize& size, const SWindowStyle& style) : XBaseWindow(parent, name, size, style)
{
	this->SetClassName(L"XWindow");

	mRootControl = nil;
	
	OSErr err = ::CreateRootControl(mWindow, &mActualRoot);
	ThrowIfOSErr(err);

//	CALL_INVARIANT;		// wait until HandleOpen executes
}


//---------------------------------------------------------------
//
// XWindow::Create 										[static]
//
//---------------------------------------------------------------
XCreateByNameMixin* XWindow::Create(XCreateByNameMixin* parent)
{
	XBaseWindow* parentWind = dynamic_cast<XBaseWindow*>(parent);
	OSWindow wptr = parentWind != nil ? parentWind->GetOSWindow() : nil;
	
	XBaseWindow* window = new XWindow(wptr, L"", XSize(256, 256), SWindowStyle());
	
	return window;
}

						
//---------------------------------------------------------------
//
// XWindow::HandleUnflatten
//
//---------------------------------------------------------------
void XWindow::HandleUnflatten()
{
	XNestableControl::iterator iter = mRootControl->begin();
	while (iter != mRootControl->end()) {
		XControl* control = *iter;
		++iter;
		
		control->HandleUnflatten();
	}

	Inherited::HandleUnflatten();
}


//---------------------------------------------------------------
//
// XWindow::HandleBalloonHelp
//
//---------------------------------------------------------------
bool XWindow::HandleBalloonHelp(const XMouseMoveEvent& event)
{
	bool handled = false;
	
	XControl* control = mRootControl->FindControl(event.GetPosition());
	if (control != nil) {
		XMouseMoveEvent childEvent = event;
		XPoint pos = control->WindowToLocal(event.GetPosition());
		childEvent.SetPosition(pos);
		
		handled = control->HandleBalloonHelp(childEvent);
	}
			
	return handled;
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XWindow::Invariant
//
//---------------------------------------------------------------
void XWindow::Invariant() const
{
	Inherited::Invariant();
	
	ASSERT(mRootControl != nil);
	if (mRootControl != nil) {			// may be nil if we're closing via an exception
		ASSERT(mRootControl->GetWindow() == this);
	
		ASSERT(mRootControl->GetLocation() == kZeroPt);
	
		if (!this->IsCollapsed())
			ASSERT(this->GetSize() == mRootControl->GetSize());
	
		ASSERT(mActualRoot != nil);
	
		ControlHandle rootH = nil;
		OSErr err = ::GetRootControl(mWindow, &rootH);
		ASSERT(err == noErr);
		ASSERT(rootH == mActualRoot);
		
		ASSERT(::IsControlVisible(mActualRoot));
		
		uint16 count = 0;
		err = ::CountSubControls(rootH, &count);
		ASSERT(err == noErr);
		ASSERT(count == 1);
	
		ControlHandle childH = nil;
		err = ::GetIndexedSubControl(rootH, 1, &childH);
		ASSERT(err == noErr);
		ASSERT(childH == mRootControl->GetOSControl());
	}
}


//---------------------------------------------------------------
//
// XWindow::OnStreamOut
//
//---------------------------------------------------------------
void XWindow::OnStreamOut(XXMLDoc& doc, XXMLElement& parent) const
{
	Inherited::OnStreamOut(doc, parent);
		
	if (mRootControl->begin() != mRootControl->end()) {
		XAutoPtr<XXMLElement> controls(new XXMLElement(L"Controls"));
	
		XNestableControl::iterator iter = mRootControl->begin();
		while (iter != mRootControl->end()) {
			XControl* control = *iter;
			++iter;
			
			XXMLPersistentMixin::Flatten(doc, *controls.Get(), control);
		}

		parent.AppendItem(controls.Release());
	}
}


//---------------------------------------------------------------
//
// XWindow::OnStreamIn
//
//---------------------------------------------------------------
void XWindow::OnStreamIn(const XXMLDoc& doc, const XXMLElement& parent, const XXMLElement& element)
{
	Inherited::OnStreamIn(doc, parent, element);
	
	if (element.GetName() == L"Controls") {
		ASSERT(element.attr_begin() == element.attr_end());		// we don't have any attributes
				
		XXMLElement::const_item_iterator iter = element.item_begin();
		while (iter != element.item_end()) {
			const XXMLItem* item = *iter;
			++iter;
			
			if (const XXMLElement* child = dynamic_cast<const XXMLElement*>(item))				
				(void) XXMLPersistentMixin::Unflatten(doc, *child, mRootControl);
		}	
	}
}


//---------------------------------------------------------------
//
// XWindow::OnBroadcast
//
//---------------------------------------------------------------
void XWindow::OnBroadcast(const SStateMessage& message)
{
	UNUSED(message);
}


//---------------------------------------------------------------
//
// XWindow::OnOpen
//
//---------------------------------------------------------------
void XWindow::OnOpen()
{	
	Inherited::OnOpen();
	
	if (mRootControl == nil)
		mRootControl = this->OnCreateRootControl();
}


//---------------------------------------------------------------
//
// XWindow::OnShow
//
//---------------------------------------------------------------
void XWindow::OnShow()
{
	Inherited::OnShow();

	mRootControl->HandleShowing();
}

	
//---------------------------------------------------------------
//
// XWindow::OnHide
//
//---------------------------------------------------------------
void XWindow::OnHide()
{
	mRootControl->HandleHiding();

	Inherited::OnHide();
}


//---------------------------------------------------------------
//
// XWindow::OnActivating
//
//---------------------------------------------------------------
void XWindow::OnActivating()
{
	Inherited::OnActivating();

	mRootControl->HandleActivating();
	
	this->Invalidate();		// $$ without this Muscle's scrollable window doesn't redraw properly...

	this->StartIdling();
}

	
//---------------------------------------------------------------
//
// XWindow::OnDeactivating
//
//---------------------------------------------------------------
void XWindow::OnDeactivating()
{
	mRootControl->HandleDeactivating();

	Inherited::OnDeactivating();
}

	
//---------------------------------------------------------------
//
// XWindow::OnAdjustCursor
//
//---------------------------------------------------------------
bool XWindow::OnAdjustCursor(const XMouseMoveEvent& event)
{
	bool handled = false;

	XControl* control = mRootControl->FindControl(event.GetPosition());
	if (control != nil) {
		XMouseMoveEvent childEvent = event;
		XPoint pos = control->WindowToLocal(event.GetPosition());
		childEvent.SetPosition(pos);
		
		handled = control->HandleAdjustCursor(childEvent);
	}
		
	if (!handled)
		handled = Inherited::OnAdjustCursor(event);

	return handled;
}


//---------------------------------------------------------------
//
// XWindow::OnMouseDown
//
//---------------------------------------------------------------
bool XWindow::OnMouseDown(const XMouseDownEvent& event)
{
	bool handled = false;

	XControl* control = mRootControl->FindControl(event.GetPosition());
	if (control != nil) {
		XMouseDownEvent childEvent = event;
		XPoint pos = control->WindowToLocal(event.GetPosition());
		childEvent.SetPosition(pos);
		
		handled = control->HandleMouseDown(childEvent);
	}
		
	if (!handled)
		handled = Inherited::OnMouseDown(event);
			
	return handled;
}


//---------------------------------------------------------------
//
// XWindow::OnContextMenu
//
//---------------------------------------------------------------
bool XWindow::OnContextMenu(const XMouseDownEvent& event)
{
	bool handled = false;

	XControl* control = mRootControl->FindControl(event.GetPosition());
	if (control != nil) {
		XMouseDownEvent childEvent = event;
		XPoint pos = control->WindowToLocal(event.GetPosition());
		childEvent.SetPosition(pos);
		
		handled = control->HandleContextMenu(childEvent);
	}
		
	if (!handled)
		handled = Inherited::OnContextMenu(event);
			
	return handled;
}


//---------------------------------------------------------------
//
// XWindow::OnDraw
//
//---------------------------------------------------------------
void XWindow::OnDraw(XDrawContext& canvas, const XRegion& dirtyRgn)
{
	Inherited::OnDraw(canvas, dirtyRgn);

	mRootControl->HandleDraw(canvas, dirtyRgn);
}


//---------------------------------------------------------------
//
// XWindow::OnSetSize		
//
//---------------------------------------------------------------
void XWindow::OnSetSize(const XSize& inSize, bool redraw)
{
	XSize newSize;
	newSize.width  = MinMax(mMinSize.width, inSize.width, mMaxSize.width);
	newSize.height = MinMax(mMinSize.height, inSize.height, mMaxSize.height);

	Inherited::OnSetSize(newSize, redraw);
	
	mRootControl->SetSize(newSize, redraw);
}


//---------------------------------------------------------------
//
// XWindow::OnIdle		
//
//---------------------------------------------------------------
void XWindow::OnIdle()
{
	if (mRootControl->WantsIdle()) {
		::SetPort(GetWindowPort(mWindow));
		::IdleControls(mWindow);
	}
}


//---------------------------------------------------------------
//
// XWindow::DoResetWindow
//
//---------------------------------------------------------------
void XWindow::DoResetWindow(OSWindow wind, const SWindowStyle& style)
{	
	ControlHandle actualRoot = nil;
	OSErr err = ::CreateRootControl(wind, &actualRoot);
	ThrowIfOSErr(err);

	XRootControl* root = nil;
	WindowRef oldWindow = mWindow;

	try {
		mWindow = wind;
		root = new XRootControl(this->GetName() + L" Root Control", this);	
		mWindow = oldWindow;
	
	} catch (...) {
		mWindow = oldWindow;
		DisposeControl(actualRoot);
		throw;
	}

	XRootControl* temp = mRootControl;
	mRootControl = nil;					// so XRootControl dtor doesn't ASSERT		
	delete temp;
	
	DisposeControl(mActualRoot);
	
	Inherited::DoResetWindow(wind, style);
	
	mActualRoot = actualRoot;
	mRootControl = root;	
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XWindow::OnCreateRootControl
//
//---------------------------------------------------------------
XRootControl* XWindow::OnCreateRootControl()
{
	std::wstring name = this->GetName();
	
	return new XRootControl(name + L" Root Control", this);	
}


}	// namespace Whisper
