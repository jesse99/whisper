/*
 *  File:       MControl.cpp
 *  Summary:   	Base class for both OS and user widgets.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MControl.cpp,v $
 *		Revision 1.11  2001/04/21 03:24:24  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.10  2001/04/17 01:40:49  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.9  2001/04/13 07:49:02  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.8  2001/02/09 04:33:06  jesjones
 *		IChildNode method renaming.
 *		
 *		Revision 1.7  2001/01/30 07:10:41  jesjones
 *		Added DoChar and DoKey.
 *		
 *		Revision 1.6  2001/01/22 02:00:40  jesjones
 *		Uses IChildNode instead of XChildNode.
 *		
 *		Revision 1.5  2000/12/31 04:25:59  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.4  2000/12/14 08:27:01  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/11/09 09:10:57  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <15>	 8/04/00	JDJ		DoPostCreate calls EmbedControl.
 *		 <14>	12/24/99	JDJ		HandleContextMenu sets the key focus to 'this'.
 *		 <13>	 7/20/99	JDJ		Tweaked for Carbon.
 *		 <12>	 7/14/99	JDJ		GetObject only calls GetControlPropertySize if System 8.5 or better.
 *		 <11>	 6/28/99	JDJ		Added streaming support.
 *		 <10>	 7/01/99	JDJ		Only uses IsValidControlHandle if System 8.5 or better.
 *		 <9>	 6/30/99	JDJ		Only uses SetControlProperty if System 8.5 or better.
 *		 <8>	 6/22/99	JDJ		Added support for streaming.
 *		 <7>	 6/09/99	JDJ		HandleShow and HandleHide work if the parent control is hidden.
 *		 <6>	 4/02/99	JDJ		OnChar only returns true if HandleControlKey doesn't return
 *									kControlNoPart.
 *		 <5>	 4/01/99	JDJ		OnClicked has an event argument and calls the XClickExtension's.
 *		 <4>	 1/16/99	JDJ		HandleMouseDown makes the control the focus if kControlGetsFocusOnClick is set.
 *		 <3>	12/23/98	JDJ		OnContextMenu returns an XContextMenu* instead of
 *									a bool.
 *		 <2>	12/08/98	JDJ		Parent control is cached.
 *		 <1>	11/03/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IControl.h>

#include <IChildNode.h>
#include <IFocus.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IRootControl.h>
#include <IScrollableControl.h>
#include <IWindow.h>
#include <MSystemInfo.h>
#include <XExceptions.h>
#include <XInterfaceMacros.h>
#include <XKeyEvents.h>

namespace Whisper {


// ===================================================================================
//	class XControlContext
// ===================================================================================

//---------------------------------------------------------------
//
// XControlContext::~XControlContext
//
//---------------------------------------------------------------
XControlContext::~XControlContext()
{
}


//---------------------------------------------------------------
//
// XControlContext::XControlContext
//
//---------------------------------------------------------------
XControlContext::XControlContext(const IControlPtr& control) : XDrawContext(control->GetOSDrawContext())
{		
	XPoint origin = control->LocalToWindow(kZeroPt);
	this->SetOrigin(origin);

	IGeometryPtr geometry(control);
	XRegion clip = geometry->GetVisibleExtent();
	this->SetClip(clip);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XControl
//!		An interface that is common to all controls.
// ===================================================================================
class XControl : public IControl, public IChildNode {	
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XControl();
	
						XControl(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:	
	virtual void 		DoPostCreate(const std::wstring& name, const IHierarchyPtr& parent, OSControl controlH);

	virtual std::wstring GetName() const						{return mName;}
	virtual void 		SetName(const std::wstring& name)		{mName = name;}
	
	virtual void 		Invalidate();
	virtual void 		Invalidate(const XRect& rect)		{this->Invalidate(XRegion(rect));}
	virtual void 		Invalidate(const XRegion& rgn, bool clipToExtent = true);
			
	virtual void 		Validate();
	virtual void 		Validate(const XRect& rect)			{this->Validate(XRegion(rect));}
	virtual void 		Validate(const XRegion& rgn, bool clipToExtent = true);
	
	virtual void 		HandleUpdate();
	virtual	bool 		IsVisible() const;	
	virtual void 		Show(bool redraw = kRedraw);	
	virtual void 		Hide(bool redraw = kRedraw);
		
	virtual	bool 		IsActive() const;		
	virtual	bool 		IsEnabled() const;		
	virtual void 		Enable(bool enable = true, bool redraw = kRedraw);
	
	virtual XPoint 		WindowToLocal(const XPoint& pt) const;
	virtual XRect 		WindowToLocal(const XRect& rect) const;
	virtual XRegion 	WindowToLocal(const XRegion& rgn) const;
			
	virtual XPoint 		LocalToWindow(const XPoint& pt) const;
	virtual XRect 		LocalToWindow(const XRect& rect) const;
	virtual XRegion 	LocalToWindow(const XRegion& rgn) const;
			
	virtual XPoint 		ParentToLocal(const XPoint& pt) const;	
	virtual XRect 		ParentToLocal(const XRect& rect) const;
	virtual XRegion 	ParentToLocal(const XRegion& rgn) const;
			
	virtual XPoint 		LocalToParent(const XPoint& pt) const;
	virtual XRect		LocalToParent(const XRect& rect) const;
	virtual XRegion		LocalToParent(const XRegion& rgn) const;

	virtual OSControl 	GetOSControl() const			{return mControl;}		
	virtual OSDrawContext GetOSDrawContext() const;
	virtual OSWindow 	GetOSWindow() const;

	virtual void 		OnAdopted();
	virtual void 		OnOrphaned();

//-----------------------------------
//	Internal API
//
private:
	virtual void 		Invariant() const;
		
	virtual void 		DoKey(const XKeyDownEvent& event);
	virtual void 		DoChar(const XCharEvent& event);

//-----------------------------------
//	Member Data
//
private:
	ControlHandle	mControl;
	std::wstring	mName;
	bool			mEnabled;

	uint32			mHelpID;			// Win
};

DEFINE_INTERFACE_FACTORY(XControl)

//---------------------------------------------------------------
//
// XControl::~XControl
//
//---------------------------------------------------------------
XControl::~XControl()
{
	if (mControl != nil)		
		DisposeControl(mControl);
}


//---------------------------------------------------------------
//
// XControl::XControl
//
//---------------------------------------------------------------
XControl::XControl(XBoss* boss) 
{
	IControl::DoSetBoss(boss);
	IChildNode::DoSetBoss(boss);
	
	mControl = nil;
	mEnabled = true;
	mHelpID = 0;
}

	
//---------------------------------------------------------------
//
// XControl::Invalidate ()
//
//---------------------------------------------------------------
void XControl::Invalidate()
{
	IControl* thisPtr = this;
	IGeometryPtr geometry(thisPtr);

	XRect extent = geometry->GetExtent();	
	extent.Inset(-kDrawOutset, -kDrawOutset);
	
	this->Invalidate(extent, false);
}


//---------------------------------------------------------------
//
// XControl::Invalidate
//
//---------------------------------------------------------------
void XControl::Invalidate(const XRegion& rgn, bool clipToPane)
{
	CHECK_INVARIANT;
	
	if (this->IsVisible()) {		
		XRegion local = rgn;
		
		IControl* thisPtr = this;
		if (clipToPane) {
			IGeometryPtr geometry(thisPtr);
			XRect extent = geometry->GetExtent();
			local &= extent;
		}
		
		XRegion super = this->LocalToParent(local);
		
		IControlPtr parent = GetParent<IControl>(thisPtr);
		if (parent)
			parent->Invalidate(super);
			
		else {
			IWindowPtr window = GetParent<IWindow>(thisPtr);
			window->Invalidate(super);
		}
	}
}


//---------------------------------------------------------------
//
// XControl::Validate ()
//
//---------------------------------------------------------------
void XControl::Validate()
{
	IControl* thisPtr = this;
	IGeometryPtr geometry(thisPtr);

	XRect extent = geometry->GetExtent();	
	extent.Inset(-kDrawOutset, -kDrawOutset);
	
	this->Validate(extent, false);
}


//---------------------------------------------------------------
//
// XControl::Validate
//
//---------------------------------------------------------------
void XControl::Validate(const XRegion& rgn, bool clipToPane)
{
	CHECK_INVARIANT;
	
	if (this->IsVisible()) {
		XRegion superRgn = rgn;
		
		IControl* thisPtr = this;
		if (clipToPane) {
			IGeometryPtr geometry(thisPtr);
			XRect extent = geometry->GetExtent();
			superRgn &= extent;
		}
		
		IControlPtr parent = GetParent<IControl>(thisPtr);
		if (parent)
			parent->Validate(superRgn);
			
		else {
			IWindowPtr window = GetParent<IWindow>(thisPtr);
			window->Validate(superRgn);
		}
	}
}


//---------------------------------------------------------------
//
// XControl::HandleUpdate
//
//---------------------------------------------------------------
void XControl::HandleUpdate()
{
	CHECK_INVARIANT;
	
	IControl* thisPtr = this;
	IWindowPtr window = GetParent<IWindow>(thisPtr);
	window->HandleUpdate();
}


//---------------------------------------------------------------
//
// XControl::IsVisible
//
//---------------------------------------------------------------
bool XControl::IsVisible() const
{
	bool visible = IsControlVisible(mControl);
		
	return visible;
}


//---------------------------------------------------------------
//
// XControl::Show
//
//---------------------------------------------------------------
void XControl::Show(bool redraw)
{
	CHECK_INVARIANT;

	if (!this->IsVisible()) {	
		OSErr err = ::SetControlVisibility(mControl, true, kDontRedraw);
		ThrowIfOSErr(err);

		if (redraw)
			this->Invalidate();
	}

#if DEBUG
	IControl* thisPtr = this;
	IRootControlPtr root(thisPtr);
	if (root) {
		IWindowPtr window = GetParent<IWindow>(thisPtr);
		POSTCONDITION(this->IsVisible() || (window && !window->IsVisible()));	
	
	} else {
		IControlPtr parent = GetParent<IControl>(thisPtr);
		POSTCONDITION(this->IsVisible() || (parent && !parent->IsVisible()));	
	}
#endif
}


//---------------------------------------------------------------
//
// XControl::Hide
//
//---------------------------------------------------------------
void XControl::Hide(bool redraw)
{
	CHECK_INVARIANT;
	
	bool wasVisible = this->IsVisible();

	IControl* thisPtr = this;
	IFocusPtr focus(thisPtr);
	bool wasFocused = focus && focus->HasFocus();

	OSErr err = ::SetControlVisibility(mControl, false, kDontRedraw);	// always hide the control because we may be hidden because our parent is hidden
	ThrowIfOSErr(err);
	
	if (wasFocused) {
		IFocusPtr super = GetParent<IFocus>(thisPtr);
		while (super && !super->WantsFocus())
			super = GetParent<IFocus>(super);
		
		super->SetFocus();				// if no one else intervenes app will get the focus
	}

	if (redraw && wasVisible)
		this->Invalidate();

	POSTCONDITION(!this->IsVisible());
}


//---------------------------------------------------------------
//
// XControl::IsActive
//
//---------------------------------------------------------------
bool XControl::IsActive() const
{
	bool active = IsControlActive(mControl);
			
	return active;
}


//---------------------------------------------------------------
//
// XControl::IsEnabled
//
//---------------------------------------------------------------
bool XControl::IsEnabled() const
{
	bool enabled = false;
	
//	if (CheckCFMLink(::IsControlEnabled)) 		// $$$ get link errors with CarbonLib 1.3b2
//		enabled = ::IsControlEnabled(mControl);
//	else
		enabled = mEnabled && this->IsActive();
	
	return enabled;
} 


//---------------------------------------------------------------
//
// XControl::Enable
//
//---------------------------------------------------------------
void XControl::Enable(bool enable, bool redraw)
{
	CHECK_INVARIANT;

	if (enable != mEnabled) {			
		::SetPort(this->GetOSDrawContext());
		::HidePen();

		OSStatus err = noErr;
		if (enable) {
//			if (CheckCFMLink(::EnableControl)) // $$$ get link errors with CarbonLib 1.3b2
//				err = ::EnableControl(mControl);
//			else
				::HiliteControl(mControl, 0);	
				
		} else {
			IControl* thisPtr = this;
			IFocusPtr focus(thisPtr);
			if (focus && focus->HasFocus()) {
				IFocusPtr super = GetParent<IFocus>(thisPtr);
				while (super && !super->WantsFocus())
					super = GetParent<IFocus>(super);
				
				super->SetFocus();				// if no one else intervenes app will get the focus
			}
					
//			if (CheckCFMLink(::DisableControl)) 
//				err = ::DisableControl(mControl);
//			else
				::HiliteControl(mControl, 255);	
		}
		
		::ShowPen();
		ThrowIfOSErr(err);
		mEnabled = enable;

		if (redraw)
			this->Invalidate();
	}
	
	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// XControl::LocalToWindow (XPoint)
//
//---------------------------------------------------------------
XPoint XControl::LocalToWindow(const XPoint& localPt) const
{
	XPoint portPt;
	
	const IControl* thisPtr = this;
	IConstControlPtr parent = GetConstParent<IControl>(thisPtr);
	if (parent) {
		XPoint superPt = this->LocalToParent(localPt);
		portPt = parent->LocalToWindow(superPt);
	
	} else
		portPt = localPt;
	
	return portPt;
}


//---------------------------------------------------------------
//
// XControl::LocalToWindow (XRect)
//
//---------------------------------------------------------------
XRect XControl::LocalToWindow(const XRect& rect) const
{
	XRect result;
	
	result[topLeft]  = this->LocalToWindow(rect[topLeft]);
	result[botRight] = this->LocalToWindow(rect[botRight]);
	
	return result;
}


//---------------------------------------------------------------
//
// XControl::LocalToWindow (XRegion)
//
//---------------------------------------------------------------
XRegion XControl::LocalToWindow(const XRegion& rgn) const
{
	XPoint offset = this->LocalToWindow(kZeroPt);
		
	return rgn + offset;
}


//---------------------------------------------------------------
//
// XControl::WindowToLocal (XPoint)
//
//---------------------------------------------------------------
XPoint XControl::WindowToLocal(const XPoint& portPt) const
{
	XPoint localPt;
	
	const IControl* thisPtr = this;
	IConstControlPtr parent = GetConstParent<IControl>(thisPtr);
	if (parent) {
		XPoint superPt = parent->WindowToLocal(portPt);
		localPt = this->ParentToLocal(superPt);
	
	} else
		localPt = portPt;
	
	return localPt;
}


//---------------------------------------------------------------
//
// XControl::WindowToLocal (XRect)
//
//---------------------------------------------------------------
XRect XControl::WindowToLocal(const XRect& rect) const
{
	XRect result;
	
	result[topLeft]  = this->WindowToLocal(rect[topLeft]);
	result[botRight] = this->WindowToLocal(rect[botRight]);
	
	return result;
}


//---------------------------------------------------------------
//
// XControl::WindowToLocal (XRegion)
//
//---------------------------------------------------------------
XRegion XControl::WindowToLocal(const XRegion& rgn) const
{
	XPoint offset = this->LocalToWindow(kZeroPt);
		
	return rgn - offset;
}


//---------------------------------------------------------------
//
// XControl::LocalToParent (XPoint)
//
//---------------------------------------------------------------
XPoint XControl::LocalToParent(const XPoint& pt) const
{
	XPoint outPt = pt;
	
	const IControl* thisPtr = this;
	IConstGeometryPtr geometry(thisPtr);
	outPt += geometry->GetLocation();

	ConstScrollableControlPtr scrollable(thisPtr);
	if (scrollable) 
		outPt += scrollable->GetScrollOffset();
		
	return outPt;
}


//---------------------------------------------------------------
//
// XControl::LocalToParent (XRect)
//
//---------------------------------------------------------------
XRect XControl::LocalToParent(const XRect& rect) const
{
	XRect result;
	
	result[topLeft]  = this->LocalToParent(rect[topLeft]);
	result[botRight] = this->LocalToParent(rect[botRight]);
	
	return result;
}


//---------------------------------------------------------------
//
// XControl::LocalToParent (XRegion)
//
//---------------------------------------------------------------
XRegion XControl::LocalToParent(const XRegion& rgn) const
{
	XPoint offset = this->LocalToParent(kZeroPt);
		
	return rgn + offset;
}


//---------------------------------------------------------------
//
// XControl::ParentToLocal (XPoint)
//
//---------------------------------------------------------------
XPoint XControl::ParentToLocal(const XPoint& pt) const
{
	XPoint outPt = pt;
	
	const IControl* thisPtr = this;
	IConstGeometryPtr geometry(thisPtr);
	outPt -= geometry->GetLocation();

	ConstScrollableControlPtr scrollable(thisPtr);
	if (scrollable) 
		outPt -= scrollable->GetScrollOffset();
		
	return outPt;
}


//---------------------------------------------------------------
//
// XControl::ParentToLocal (XRect)
//
//---------------------------------------------------------------
XRect XControl::ParentToLocal(const XRect& rect) const
{
	XRect result;
	
	result[topLeft]  = this->ParentToLocal(rect[topLeft]);
	result[botRight] = this->ParentToLocal(rect[botRight]);
	
	return result;
}


//---------------------------------------------------------------
//
// XControl::ParentToLocal (XRegion)
//
//---------------------------------------------------------------
XRegion XControl::ParentToLocal(const XRegion& rgn) const
{
	XPoint offset = this->LocalToParent(kZeroPt);
		
	return rgn - offset;
}


//---------------------------------------------------------------
//
// XControl::GetOSDrawContext
//
//---------------------------------------------------------------
OSDrawContext XControl::GetOSDrawContext() const
{
	GrafPtr port = nil;
	
	WindowRef wptr = ::GetControlOwner(mControl);
	ASSERT(wptr != nil);
	
	port = GetWindowPort(wptr);

	return port;
}


//---------------------------------------------------------------
//
// XControl::GetOSWindow
//
//---------------------------------------------------------------
OSWindow XControl::GetOSWindow() const
{
	WindowRef wptr = nil;
	
	wptr = ::GetControlOwner(mControl);
	POSTCONDITION(wptr != nil);
	
	return wptr;
}


//---------------------------------------------------------------
//
// IControl::GetControl									[static]
//
//---------------------------------------------------------------
IControlPtr IControl::GetControl(OSControl controlH)
{
	PRECONDITION(controlH != nil);
	
	IControlPtr control;
	
	int32 refCon = ::GetControlReference(controlH);

	if (MSystemInfo::GetSystemVersion() >= 0x0850) {
		uint32 size = 0;
		OSStatus err = ::GetControlPropertySize(controlH, 'Wspr', 'Wspr', &size);
	
		if (err == noErr) {
			IControl* ptr = reinterpret_cast<IControl*>(refCon);
			control = IControlPtr(ptr);

			ASSERT(control);
		}

	} else {
		IControl* ptr = reinterpret_cast<IControl*>(refCon);
		control = IControlPtr(ptr);
	}
	
	if (control)
		ASSERT(control->GetOSControl() == controlH);
	
	return control;	
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XControl::OnAdopted
//
//---------------------------------------------------------------
void XControl::OnAdopted()
{
	// nothing to do here
}


//---------------------------------------------------------------
//
// XControl::OnOrphaned
//
//---------------------------------------------------------------
void XControl::OnOrphaned()
{
	IControl* thisPtr = this;
	IRootControlPtr root = GetInterface<IRootControl>(thisPtr);
	if (root) 
		root->RemovedChild(thisPtr);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XControl::Invariant
//
//---------------------------------------------------------------
void XControl::Invariant() const
{
	IControl::Invariant();
	
	ASSERT(mControl != nil);

	// See if the OS thinks the control is OK
	if (MSystemInfo::GetSystemVersion() >= 0x0850) 
		ASSERT(IsValidControlHandle(mControl));

	// Make sure the OS parent matches the hierarchy parent
	ControlHandle parentH = nil;	
	OSErr err = ::GetSuperControl(mControl, &parentH);

	const IControl* thisPtr = this;
	IConstControlPtr parent = GetConstParent<IControl>(thisPtr);
	if (parent) {
		ASSERT(err == noErr);	
		ASSERT(parentH == parent->GetOSControl());
		
	} else
		ASSERT(parentH == nil);
}


//---------------------------------------------------------------
//
// XControl::DoKey
//
//---------------------------------------------------------------
void XControl::DoKey(const XKeyDownEvent& event)
{
	UNUSED(event);		// we handle keystrokes in DoChar
}


//---------------------------------------------------------------
//
// XControl::DoChar
//
//---------------------------------------------------------------
void XControl::DoChar(const XCharEvent& event)
{
	EventRecord record = event.GetOSEvent();
	
	int16 key = (int16) ((record.message & keyCodeMask) >> 8);
	char ch   = (char) (record.message & charCodeMask);
	
	(void) ::HandleControlKey(mControl, key, ch, (uint16) record.modifiers);
}


//---------------------------------------------------------------
//
// XControl::DoPostCreate
//
//---------------------------------------------------------------
void XControl::DoPostCreate(const std::wstring& name, const IHierarchyPtr& parent, OSControl controlH)
{
	PRECONDITION(controlH != nil);
	PRECONDITION(mControl == nil);
			
	mControl = controlH;
	
	// Set the refCon to 'this' so we can get to our object when
	// given a ControlHandle.
	::SetControlReference(mControl, (int32) this);

	// Add a custom property so we can check that a ControlHandle
	// is really a Whisper control.
	OSStatus err = noErr;
	if (MSystemInfo::GetSystemVersion() >= 0x0850) {
		char dummy = 0;
		err = ::SetControlProperty(mControl, 'Wspr', 'Wspr', sizeof(dummy), &dummy);
		ThrowIfOSErr(err);
	}

	// Set the control's name
	this->SetName(name);

	// Make the control a child of the parent
	IControl* thisPtr = this;
	parent->AddChild(thisPtr);

	IRootControlPtr root(thisPtr);
	if (!root) {
		IControlPtr super(parent);
		err = ::EmbedControl(mControl, super->GetOSControl());		
		ThrowIfOSErr(err);

		// Let the root control know that we've added a control
		root = GetInterface<IRootControl>(parent);
		root->AddedChild(thisPtr);
	}
}


}		// namespace Whisper
