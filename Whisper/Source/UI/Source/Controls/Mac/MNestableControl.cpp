/*
 *  File:       MNestableControl.h
 *  Summary:   	Base class for controls that can contain other controls.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MNestableControl.cpp,v $
 *		Revision 1.5  2001/04/21 03:27:13  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/03/03 23:57:44  jesjones
 *		Added calls to DEBUGSTR in catch (...) blocks.
 *		
 *		Revision 1.3  2000/12/10 04:04:35  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:16:13  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <7>	 7/20/99	JDJ		Tweaked for Carbon.
 *		 <6>	 6/29/99	JDJ		Added streaming support.
 *		 <5>	 6/11/99	JDJ		Tweaked for CW Pro 5fc3.
 *		 <4>	 6/08/99	JDJ		Added an ugly SetControlVisibility work-around to
 *									HandleDraw.
 *		 <3>	 6/07/99	JDJ		XControlIterator returns the immediate children in
 *									the order they were added.
 *		 <2>	12/08/98	JDJ		XControl caches parent control.
 *		 <1>	11/04/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XNestableControl.h>

#include <XAutoPtr.h>
#include <XExceptions.h>
#include <XMiscUtils.h>
#include <XMouseEvents.h>
#include <XXMLDoc.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateNestableControl
//
//---------------------------------------------------------------
static ControlHandle CreateNestableControl(const XNestableControl* parent, const XRect& frame, uint32 procID)
{
	XRect bounds = parent->LocalToWindow(frame);
		
	uint32 features = kControlSupportsEmbedding + kControlWantsActivate;
	::SetPort(parent->GetOSContext());
	::HidePen();
	Rect temp = bounds.GetOSRect();
	ControlHandle controlH = NewControl(parent->GetOSWindow(), &temp, "\p", true, numeric_cast<int16>(features), 0, 0, (int16) procID, 0);
	::ShowPen();
	ThrowIfNil(controlH);
	
	return controlH;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XControlIterator
// ===================================================================================

//---------------------------------------------------------------
//
// XControlIterator::XControlIterator (XNestableControl*)
//
//---------------------------------------------------------------
XControlIterator::XControlIterator(const XNestableControl* parent)
{
	mParent = parent;
}


//---------------------------------------------------------------
//
// XControlIterator::XControlIterator (XNestableControl*, bool)
//
//---------------------------------------------------------------
XControlIterator::XControlIterator(const XNestableControl* parent, bool recursive)
{
	mParent = parent;
	
	this->DoAddChildren(parent, recursive);
}


//---------------------------------------------------------------
//
// XControlIterator::operator++ (int)
//
//---------------------------------------------------------------
XControlIterator XControlIterator::operator++(int)
{
	XControlIterator temp = *this;
	
	mChildren.pop_front();
	
	return temp;
}


//---------------------------------------------------------------
//
// XControlIterator::operator==
//
//---------------------------------------------------------------
bool XControlIterator::operator==(const XControlIterator& rhs) const
{
	PRECONDITION(mParent == rhs.mParent);
	
	bool equal = mChildren.size() == rhs.mChildren.size();
	if (equal && mChildren.size() > 0)
		equal = mChildren.front() == rhs.mChildren.front();
		
	return equal;
}


//---------------------------------------------------------------
//
// XControlIterator::DoAddChildren
//
//---------------------------------------------------------------
void XControlIterator::DoAddChildren(const XNestableControl* parent, bool recursive)
{
	PRECONDITION(parent != nil);
	
	uint16 count;
	OSErr err = ::CountSubControls(parent->GetOSControl(), &count);
	ThrowIfOSErr(err);
	
	for (uint16 index = 1; index <= count; ++index) {
		ControlHandle controlH;
		err = ::GetIndexedSubControl(parent->GetOSControl(), index, &controlH);
		ThrowIfOSErr(err);
		
		XControl* child = XControl::GetObject(controlH);
		
		if (child != nil) {
			mChildren.push_back(child);
		
			if (recursive)
				if (XNestableControl* nestable = dynamic_cast<XNestableControl*>(child))
					this->DoAddChildren(nestable, recursive);
		}
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XNestableControl
// ===================================================================================

// These used to be inlined in the header, but that causes linker warnings in multi-
// fragment apps.
void XNestableControl::OnChildScrolled(XControl* /*child*/, const XPoint& /*oldLoc*/, bool /*redraw*/)			{}
void XNestableControl::OnChildChangedSize(XControl* /*child*/, const XSize& /*oldSize*/, bool /*redraw*/)		{}
void XNestableControl::OnChildChangedLocation(XControl* /*child*/, const XPoint& /*oldLoc*/, bool /*redraw*/)	{}	
void XNestableControl::OnAddedChild(XControl* /*child*/)					{}
void XNestableControl::OnRemovingChild(XControl* /*child*/)				{}

//---------------------------------------------------------------
//
// XNestableControl::~XNestableControl
//
//---------------------------------------------------------------
XNestableControl::~XNestableControl()
{
	CALL_INVARIANT;
	
	this->DoDeleteChildren();
}


//---------------------------------------------------------------
//
// XNestableControl::XNestableControl (wstring, XNestableControl*, OSControl, uint32, uint32)
//
//---------------------------------------------------------------
XNestableControl::XNestableControl(const std::wstring& name, XNestableControl* parent, OSControl takeControl, uint32 style, uint32 procID) : XControl(name, parent, takeControl, style, procID)
{
	this->SetClassName(L"XNestableControl");
	mClass = L"STATIC";

	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XNestableControl::XNestableControl (wstring, XNestableControl*, XRect, uint32, uint32)
//
//---------------------------------------------------------------
XNestableControl::XNestableControl(const std::wstring& name, XNestableControl* parent, const XRect& frame, uint32 style, uint32 procID) : XControl(name, parent, CreateNestableControl(parent, frame, procID), style, procID)
{
	this->SetClassName(L"XNestableControl");
	mClass = L"STATIC";

	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XNestableControl::Create						 		[static]
//
//---------------------------------------------------------------
XCreateByNameMixin* XNestableControl::Create(XCreateByNameMixin* parent)
{
	XNestableControl* super = dynamic_cast<XNestableControl*>(parent);
	ASSERT(super != nil);
	
	XControl* control = new XNestableControl(L"", super, XRect(0, 0, 64, 64));
	
	return control;
}

						
//---------------------------------------------------------------
//
// XNestableControl::HandleUnflatten
//
//---------------------------------------------------------------
void XNestableControl::HandleUnflatten()
{
	XNestableControl::iterator iter = this->begin();
	while (iter != this->end()) {
		XControl* control = *iter;
		++iter;
		
		control->HandleUnflatten();
	}

	Inherited::HandleUnflatten();
}


//---------------------------------------------------------------
//
// XNestableControl::AddChild
//
//---------------------------------------------------------------
void XNestableControl::AddChild(XControl* child, bool redraw)
{
	PRECONDITION(child != nil);			
//	PRECONDITION(child->GetParent() == nil);		// unfortunately we can't do this with newly created controls because they're automaticly embedded in the root if they're created in the parent window and we can't use EmbedControl to	set the parent to nil or to switch windows	
	PRECONDITION(child != this);			
	CHECK_INVARIANT;
	
	TRACEFLOW("XControl", "Adding subpane '", child->GetName(), "' to '", this->GetName(), "'\n");
	
	::SetPort(this->GetOSContext());
	::HidePen();
	OSErr err = ::EmbedControl(child->GetOSControl(), mControl);		
	::ShowPen();
	ThrowIfOSErr(err);
	
	child->mParent = this;
	
	if (child->IsVisible() || child->mWasVisible) {
		if (redraw)
			child->Invalidate();
		child->OnShow();
	}
	
	XNestableControl* parent = this;
	while (parent != nil) {
		parent->OnAddedChild(child);
		parent = parent->GetParent();
	}
	
	POSTCONDITION(child->GetParent() == this);
}


//---------------------------------------------------------------
//
// XNestableControl::RemoveChild
//
//---------------------------------------------------------------
void XNestableControl::RemoveChild(XControl* child, bool redraw)
{
	PRECONDITION(child != nil);
	PRECONDITION(child->GetParent() == this);			
	CHECK_INVARIANT;
	
	TRACEFLOW("XControl", "Removing subpane '", child->GetName(), "' from '", this->GetName(), "'\n");
	
	if (child->IsVisible()) {
		if (redraw)
			child->Invalidate();
		child->OnHide();
		child->mWasVisible = true;
	}
		
	XNestableControl* parent = this;
	while (parent != nil) {
		parent->OnRemovingChild(child);
		parent = parent->GetParent();
	}
	
	ControlHandle rootH = nil;
	OSErr err = ::GetRootControl(this->GetOSWindow(), &rootH);
	ThrowIfOSErr(err);

	::SetPort(this->GetOSContext());
	::HidePen();
	err = ::EmbedControl(child->GetOSControl(), rootH);		// can't use nil for parent so we're forced to get cute (see the XWindow ctor for more details)
	::ShowPen();
	ThrowIfOSErr(err);
	
	child->mParent = nil;
	
	POSTCONDITION(child->GetParent() == nil);
}


//---------------------------------------------------------------
//
// XNestableControl::IsChild
//
//---------------------------------------------------------------
bool XNestableControl::IsChild(const XControl* control, bool recursive) const
{
	bool isChild = false;
	
	if (control != nil) {
		XNestableControl::iterator iter = this->begin(recursive);
		while (iter != this->end() && !isChild) {
			XControl* child = *iter;
			++iter;
					
			isChild = control == child;
		}
	}
			
	return isChild;	
}


//---------------------------------------------------------------
//
// XNestableControl::HandleShowing
//
//---------------------------------------------------------------
void XNestableControl::HandleShowing()
{
	CHECK_INVARIANT;

	Inherited::HandleShowing();
	
	XNestableControl::iterator iter = this->begin(kNonRecursive);
	while (iter != this->end()) {
		XControl* child = *iter;
		++iter;

		if (child->IsVisible())
			child->HandleShowing();
	}		
}


//---------------------------------------------------------------
//
// XNestableControl::HandleHiding
//
//---------------------------------------------------------------
void XNestableControl::HandleHiding()
{
	CHECK_INVARIANT;

	Inherited::HandleHiding();
	
	XNestableControl::iterator iter = this->begin(kNonRecursive);
	while (iter != this->end()) {
		XControl* child = *iter;
		++iter;

		if (child->IsVisible())
			child->HandleHiding();
	}		
}


//---------------------------------------------------------------
//
// XNestableControl::HandleActivating
//
//---------------------------------------------------------------
void XNestableControl::HandleActivating()
{
	CHECK_INVARIANT;

	if (this->IsVisible()) {
 		TRACEFLOW("XControl", "Activating '", this->GetName(), "'\n");
 		
		Inherited::HandleActivating();
	
		XNestableControl::iterator iter = this->begin(kNonRecursive);
		while (iter != this->end()) {
			XControl* child = *iter;
			++iter;

			child->HandleActivating();
		}
	}
}


//---------------------------------------------------------------
//
// XNestableControl::HandleDeactivating
//
//---------------------------------------------------------------
void XNestableControl::HandleDeactivating()
{
	CHECK_INVARIANT;

	if (this->IsVisible()) {
 		TRACEFLOW("XControl", "Deactivating '", this->GetName(), "'\n");
 		
		Inherited::HandleDeactivating();
	
		XNestableControl::iterator iter = this->begin(kNonRecursive);
		while (iter != this->end()) {
			XControl* child = *iter;
			++iter;

			child->HandleDeactivating();
		}
	}
}

	
//---------------------------------------------------------------
//
// XNestableControl::HandleDraw
//
// As far as I can tell there's no way to hook into control
// drawing and no way to draw a control without drawing it's
// children. This makes life difficult for us since we want to
// draw our draw extensions whenever we draw a control.
//
// The best work around I was able to come up with is to hide
// all the child controls before drawing a parent control. This
// is more than a little ugly, but it should be safe and reasonably
// efficient...
//
//---------------------------------------------------------------
void XNestableControl::HandleDraw(XDrawContext& context, const XRegion& dirtyWindowRgn)
{
	CHECK_INVARIANT;
	
	try {
		if (this->IsVisible()) {	
			
			// Get the clip region for the control. (We only use this
			// with draw extensions because controls draw outside their
			// extent when framing or drawing focus rings).
			XPoint origin = this->LocalToWindow(kZeroPt);
	
			XRect extent = this->GetVisibleExtent();
			XRegion myClip = (dirtyWindowRgn - origin) & extent;
	
			// Since OS controls may draw outside their extent we'll
			// clip them to their parent control.
			XRegion controlClip;
			XNestableControl* parent = this->GetParent();
			if (parent != nil)
				controlClip = dirtyWindowRgn & parent->LocalToWindow(parent->GetVisibleExtent());
			else
				controlClip = dirtyWindowRgn & this->LocalToWindow(extent);

			if (!controlClip.IsEmpty()) {					
				// Draw the pre-draw extensions.
				context.SetOrigin(origin);
				context.SetClip(myClip);	
				if (this->OnPreDraw(context)) {
			
					// Hide all the visible child controls. (If this isn't
					// done Draw1Control will draw all the child controls
					// which means we'll wind up drawing the controls multiple
					// times). $$$$$$ If the Appearance Manager wasn't a pile of
					// crap we could get by with only hiding the immediate children.
					// However it is a pile of crap and with AM 1.1 Draw1Control
					// will sometimes draw deply nested children even though their
					// parent controls are invisible. $$$$$$
					OSErr err = noErr;
					XControl* child = nil;
					std::list<XControl*> children;
					XNestableControl::iterator iter = this->begin(kRecursive);
					while (iter != this->end()) {
						child = *iter;
						++iter;
	
						if (child->IsVisible()) {
							err = ::SetControlVisibility(child->GetOSControl(), false, kDontRedraw);
							if (err == noErr)
								children.push_back(child);
							else
								DEBUGSTR("Got a ", err, " error hiding child (in XNestableControl::HandleDraw)");
						}
					}
	
					// Draw the control.
					XPoint offset = this->OnGetScrollOffset();
					context.SetOrigin(offset);
					context.SetClip(controlClip - offset);	

					context.MakeCurrent();
					::Draw1Control(mControl);	
						
					// Draw all of the visible children.
					std::list<XControl*>::iterator iter2 = children.begin();
					while (iter2 != children.end()) {
						child = *iter2;
						++iter2;
	
						err = ::SetControlVisibility(child->GetOSControl(), true, kDontRedraw);
						if (err == noErr)
							child->HandleDraw(context, dirtyWindowRgn);
						else
							DEBUGSTR(L"Got a ", err, " error showing child (in XNestableControl::HandleDraw)");
					}
					
					// Draw the post-draw extensions.
					context.SetOrigin(origin);
					context.SetClip(myClip);	
					this->OnPostDraw(context);
				}
			}
		}
	
	} catch (...) {
		DEBUGSTR("Got an exception in XNestableControl::HandleDraw");	
	}
}


//---------------------------------------------------------------
//
// XNestableControl::FindChild (wstring, bool)
//
//---------------------------------------------------------------
XControl* XNestableControl::FindChild(const std::wstring& name, bool recursive) const
{
	CHECK_INVARIANT;
	
	XControl* child = nil;
	
	XNestableControl::iterator iter = this->begin(recursive);
	while (iter != this->end() && child == nil) {
		XControl* candidate = *iter;
		++iter;
		
		if (candidate->GetName() == name)
			child = candidate;
	}
		
	return child;
}


//---------------------------------------------------------------
//
// XNestableControl::FindControl (XPoint, bool)
//
//---------------------------------------------------------------
XControl* XNestableControl::FindControl(const XPoint& localPt, bool recursive, bool findInvisible) const
{
	CHECK_INVARIANT;
	
	XControl* control = nil;
	
	if (this->GetExtent().Contains(localPt)) {

		uint16 count;
		OSErr err = ::CountSubControls(mControl, &count);
		ThrowIfOSErr(err);
		
		// Traverse the list of children backwards so we start
		// with the ones that are drawn last (ie the visually
		// frontmost).
		for (uint16 index = count; index >= 1; --index) {
			ControlHandle controlH;
			err = ::GetIndexedSubControl(mControl, index, &controlH);
			ThrowIfOSErr(err);
			
			XControl* candidate = XControl::GetObject(controlH);
		
			if (candidate != nil && (candidate->IsVisible() || findInvisible) && candidate->GetFrame().Contains(localPt)) {
			
				// We've found a candidate. If it's actually a nestable
				// recursively call FindControl. Otherwise just use the
				// candidate.
				if (XNestableControl* nestable = recursive ? dynamic_cast<XNestableControl*>(candidate) : nil) {
					XPoint subPt = nestable->ParentToLocal(localPt);
					control = nestable->FindControl(subPt, recursive, findInvisible);
					
				} else
					control = candidate;
			}
		}
		
		if (control == nil)
			control = const_cast<XNestableControl*>(this);
	}
		
	return control;
}


//---------------------------------------------------------------
//
// XNestableControl::front
//
//---------------------------------------------------------------
XControl* XNestableControl::front() const
{
	uint16 count;
	OSErr err = ::CountSubControls(mControl, &count);
	ThrowIfOSErr(err);
	
	ControlHandle controlH;
	err = ::GetIndexedSubControl(mControl, count, &controlH);
	ThrowIfOSErr(err);
	
	XControl* control = XControl::GetObject(controlH);
	
	return control;
}


//---------------------------------------------------------------
//
// XNestableControl::back
//
//---------------------------------------------------------------
XControl* XNestableControl::back() const
{
	ControlHandle controlH;
	OSErr err = ::GetIndexedSubControl(mControl, 1, &controlH);
	ThrowIfOSErr(err);
	
	XControl* control = XControl::GetObject(controlH);
	
	return control;
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XNestableControl::Invariant
//
//---------------------------------------------------------------
void XNestableControl::Invariant() const
{
	Inherited::Invariant();
	
	uint32 features = 0;
	OSErr err = ::GetControlFeatures(mControl, &features);
	ASSERT(err == noErr);
	ASSERT((features & kControlSupportsEmbedding) != 0);
	
#if DEBUG
	if (gIntenseDebugging) {
		XNestableControl::iterator iter = this->begin(kNonRecursive);
		while (iter != this->end()) {
			XControl* child = *iter;
			++iter;

			ASSERT(child != nil);
			ASSERT(child->GetParent() == this);
		}
	}
#endif
}


//---------------------------------------------------------------
//
// XNestableControl::OnStreamOut
//
//---------------------------------------------------------------
void XNestableControl::OnStreamOut(XXMLDoc& doc, XXMLElement& parent) const
{
	Inherited::OnStreamOut(doc, parent);

	if (this->begin() != this->end()) {
		XAutoPtr<XXMLElement> controls(new XXMLElement(L"Controls"));
		XNestableControl::iterator iter = this->begin(kNonRecursive);
		while (iter != this->end()) {
			XControl* child = *iter;
			++iter;
			
			XXMLPersistentMixin::Flatten(doc, *controls, child);
		}
		
		parent.AppendItem(controls.Release());
	}
}


//---------------------------------------------------------------
//
// XNestableControl::OnStreamIn
//
//---------------------------------------------------------------
void XNestableControl::OnStreamIn(const XXMLDoc& doc, const XXMLElement& parent, const XXMLElement& element)
{
	Inherited::OnStreamIn(doc, parent, element);
	
	if (&parent == &element)
		this->DoDeleteChildren();
	
	if (element.GetName() == L"Controls") {
		ASSERT(element.attr_begin() == element.attr_end());		// we don't have any attributes
			
		XXMLElement::const_item_iterator iter = element.item_begin();
		while (iter != element.item_end()) {
			const XXMLItem* item = *iter;
			++iter;
			
			if (const XXMLElement* child = dynamic_cast<const XXMLElement*>(item))
				(void) XControl::Create(doc, *child, this);
		}	
	}
}


//---------------------------------------------------------------
//
// XNestableControl::OnCreateControl
//
//---------------------------------------------------------------
OSControl XNestableControl::OnCreateControl(const XXMLDoc& doc, const XXMLElement& parent, uint32 style) const
{	
	UNUSED(doc);
	UNUSED(parent);
		
	XNestableControl* super = this->GetParent();
	ASSERT(super != nil);
			
	::SetPort(super->GetOSContext());
	::HidePen();
	Rect frame = {0, 0, 16, 16};		
	uint32 features = kControlSupportsEmbedding + kControlWantsActivate;
	OSControl controlH = ::NewControl(super->GetOSWindow(), &frame, "\p", true, numeric_cast<int16>(features), 0, 0, (int16) style, (int32) this);
	::ShowPen();
	ThrowIfNil(controlH);

	return controlH;	
}

	
//---------------------------------------------------------------
//
// XNestableControl::OnSetSize
//
//---------------------------------------------------------------
void XNestableControl::OnSetSize(const XSize& newSize, bool redraw)
{
	XSize oldSize = this->GetSize();
	PRECONDITION(newSize != oldSize);
	
	XPoint delta(newSize - oldSize);
	
	Inherited::OnSetSize(newSize, redraw);
	
	this->OnAdjustChildSizes(delta, redraw);
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XNestableControl::OnAdjustChildSizes
//
//---------------------------------------------------------------
void XNestableControl::OnAdjustChildSizes(const XPoint& delta, bool redraw)
{
	XNestableControl::iterator iter = this->begin(kNonRecursive);
	while (iter != this->end()) {
		XControl* child = *iter;
		++iter;

		child->OnParentChangedSize(delta, redraw);
	}
}


//---------------------------------------------------------------
//
// XNestableControl::DoDeleteChildren
//
//---------------------------------------------------------------
void XNestableControl::DoDeleteChildren()
{
	uint16 count;
	OSErr err = ::CountSubControls(mControl, &count);
	ThrowIfOSErr(err);
	
	while (count--) {
		XControl* child = this->front();
		
		delete child;
	}

	POSTCONDITION(::CountSubControls(mControl, &count) == noErr && count == 0);
}

	
}	// namespace Whisper
