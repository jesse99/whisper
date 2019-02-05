/*
 *  File:       MDrawControl.cpp
 *  Summary:   	IDraw implementation for controls.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MDrawControl.cpp,v $
 *		Revision 1.7  2001/04/21 03:25:32  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/03/06 07:31:27  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.5  2001/02/21 08:07:00  jesjones
 *		Tweaked a comment.
 *		
 *		Revision 1.4  2001/02/07 02:28:20  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/25 00:33:44  jesjones
 *		Added a hack to allow textboxes to draw properly when inactive.
 *		
 *		Revision 1.2  2000/11/09 09:12:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IDraw.h>

#include <IControl.h>
#include <IDrawExtensible.h>
#include <IDrawHelper.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IScrollableControl.h>
#include <ITextBox.h>
#include <MQuickDrawUtils.h>
#include <XShapes.h>

namespace Whisper {


// ===================================================================================
//	class XDrawControl
//!		IDraw implementation for controls.
// ===================================================================================
class XDrawControl : public IDraw {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDrawControl();
	
						XDrawControl(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		HandleDraw(XDrawContext& canvas, const XRegion& dirtyRgn, bool isActive);
	
//-----------------------------------
//	New API
//
private:
			void 		DoDrawControl(XDrawContext& context, const XRegion& dirtyWindowRgn, const XRegion& myClip, const XRegion& osClip, bool isActive);
			void 		DoDraw(XDrawContext& context, const XRegion& osClip);
			void 		DoHideChildren();
	
//-----------------------------------
//	Member Data
//
private:
	std::list<IControlPtr>	mHidden;
	bool					mUsingBlack;
};

DEFINE_INTERFACE_FACTORY(XDrawControl)

//---------------------------------------------------------------
//
// XDrawControl::~XDrawControl
//
//---------------------------------------------------------------
XDrawControl::~XDrawControl()
{
}


//---------------------------------------------------------------
//
// XDrawControl::XDrawControl
//
//---------------------------------------------------------------
XDrawControl::XDrawControl(XBoss* boss) 
{	
	IDraw::DoSetBoss(boss);
	
	mUsingBlack = true;
}


//---------------------------------------------------------------
//
// XDrawControl::HandleDraw
//
//---------------------------------------------------------------
void XDrawControl::HandleDraw(XDrawContext& context, const XRegion& dirtyWindowRgn, bool isActive)
{	
	IControlPtr control(this);	
	ASSERT(control->IsVisible());
	
	try {
		// Get the control's origin and the dirty region in control
		// coordinates.
		IGeometryPtr geometry(this);
		XRegion extent = geometry->GetVisibleExtent();

		XPoint origin = control->LocalToWindow(kZeroPt);
		XRegion myClip = (dirtyWindowRgn - origin) & extent;
		
		IControlPtr parent = GetParent<IControl>(this);
		IGeometryPtr parentGeom(parent);

		// Get the control's dirty region in window coordinates
		// clipped to our parent. (The Control Manager deals in
		// window coordinates and will draw things like focus
		// rings outside the control extent).
		XRegion osClip;
		if (parent)
			osClip = dirtyWindowRgn & parent->LocalToWindow(parentGeom->GetVisibleExtent());	// control frames and focus rings can be drawn outside the control's extent so we'll clip to our parent
		else
			osClip = dirtyWindowRgn & control->LocalToWindow(extent);

		// If we have something to draw then,
		if (!osClip.IsEmpty()) {			
			context.SetOrigin(origin);
			context.SetClip(myClip);	
			
#if DEBUG
			// draw the dirty area in bright red to make it easier
			// to see which parts are being redrawn.
			if (gIntenseDebugging) 
				XRectShape::Fill(context, geometry->GetExtent(), kRGBRed);
#endif

			// draw the pre-adorners using the control's coordinate system
			// (it would arguably be better to draw extensions using our parent's
			// clip region but doing this on Windows would be tricky),			
			bool handled = false;
			XInterfacePtr<IDrawExtensible> extensible(this);
			if (extensible)
				handled = extensible->HandleDraw(context, geometry->GetExtent(), true, isActive);
			
			// if the adorners say it's OK to draw the control then,
			if (!handled) {
			
				// draw the control,
			 	this->DoDrawControl(context, dirtyWindowRgn, myClip, osClip, isActive);
					
				// and let the post-adorners draw.
				context.SetOrigin(origin);
				context.SetClip(myClip);	

				if (extensible)
					(void) extensible->HandleDraw(context, geometry->GetExtent(), false, isActive);
			}
		}
			
	} catch (...) {
		DEBUGSTR("Caught an exception in WDrawControl::HandleDraw!");	// don't propagate (don't want to get into an infinite loop because an error dialog causes an update event)
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XDrawControl::DoDraw
//
//---------------------------------------------------------------
void XDrawControl::DoDraw(XDrawContext& context, const XRegion& osClip)
{
	IControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	
	// $$$ The text inside editboxes doesn't dim properly when the
	// $$$ control is dectivated (OS 9). Apple's AppearanceSamplePPC
	// $$$ also exhibits this behavior. So, we'll use this gross hack
	// $$$ to get the text to draw in gray when the editbox is inactive.
	ITextBoxPtr textbox(this);
	if (textbox) {
		ControlFontStyleRec record;
		record.flags = 0;

		if (control->IsActive() && !mUsingBlack) {
			record.flags     = kControlUseForeColorMask;
			record.foreColor = kRGBBlack.GetOSColor();
			mUsingBlack = true;
			
		} else if (!control->IsActive() && mUsingBlack) {
			record.flags     = kControlUseForeColorMask;
			record.foreColor = GetGrayColor();
			mUsingBlack = false;
		}
		
		if (record.flags != 0) {
			::SetPort(control->GetOSDrawContext());
			::HidePen();
			OSErr err = SetControlFontStyle(control->GetOSControl(), &record);
			::ShowPen();
			ASSERT(err == noErr);
		}
	}
	
	// Switch from control to window coordinates
	XPoint offset = kZeroPt;
	IScrollableControlPtr scrollable = GetInterface<IScrollableControl>(this);
	if (scrollable) 
		offset = scrollable->GetScrollOffset();

	context.SetOrigin(offset);
	context.SetClip(osClip - offset);	
		
	// Draw the control
	context.MakeCurrent();
	::Draw1Control(controlH);	
}


//---------------------------------------------------------------
//
// XDrawControl::DoDrawControl
//
//---------------------------------------------------------------
void XDrawControl::DoDrawControl(XDrawContext& context, const XRegion& dirtyWindowRgn, const XRegion& myClip, const XRegion& osClip, bool isActive)
{	
	IControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	
	// Hide our children (we need to do this because Draw1Control
	// will draw the children as well which is bad because we need
	// to draw adorners before drawing controls),
	this->DoHideChildren();

	// draw the control,
	IDrawHelperPtr drawer(this);
	if (drawer)								
		drawer->OnDraw(context, myClip, isActive);
	else
		this->DoDraw(context, osClip);
	
	// draw the visible children,
	std::list<IControlPtr>::iterator iter = mHidden.begin();
	while (iter != mHidden.end()) {
		IControlPtr child = *iter;
		++iter;

		OSErr err = ::SetControlVisibility(child->GetOSControl(), true, kDontRedraw);
		if (err == noErr) {
			IDrawPtr draw(child);
			draw->HandleDraw(context, dirtyWindowRgn, isActive);
		
		} else
			DEBUGSTR("Got a ", err, " error showing child (in XDrawControl::HandleDraw)");
	}
	
	mHidden.clear();
}


//---------------------------------------------------------------
//
// XDrawControl::DoHideChildren
//
//---------------------------------------------------------------
void XDrawControl::DoHideChildren()
{
	PRECONDITION(mHidden.empty());
	
	IHierarchyPtr root(this);
	if (root) {
		IHierarchy::iterator iter = root->begin();
		while (iter != root->end()) {
			IHierarchyPtr child(*iter);
			++iter;
			
			IControlPtr control(child);
			if (control && control->IsVisible()) {
				OSErr err = ::SetControlVisibility(control->GetOSControl(), false, kDontRedraw);
				if (err == noErr)
					mHidden.push_back(control);
				else
					DEBUGSTR("Got a ", err, " error hiding child (in XDrawControl::DoHideChildren)");
			}
		}
	}
}


}	// namespace Whisper
