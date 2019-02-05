/*
 *  File:       XRubberBandTracker.cpp
 *  Summary:   	ITrackerHelper implementation that draws a rectangle as the user drags the mouse.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRubberBandTracker.cpp,v $
 *		Revision 1.3  2001/02/07 04:59:08  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.2  2001/01/03 05:58:50  jesjones
 *		Draw with flicker on Windows because GDI is lame.
 *		
 *		Revision 1.1  2000/12/31 04:30:45  jesjones
 *		Checked in	
 */

#include <XWhisperHeader.h>
#include <ITrackerHelper.h>

#include <IControl.h>
#include <ITracker.h>
#include <IWindow.h>
#include <XBrush.h>
#include <XDrawContexts.h>
#include <XShapes.h>

namespace Whisper {


// ===================================================================================
//	class XRubberBandTracker
//!		ITracker implementation that draws a rectangle as the user drags the mouse.
// ===================================================================================
class XRubberBandTracker : public ITrackerHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XRubberBandTracker();
	
						XRubberBandTracker(XBoss* boss);
	
//-----------------------------------
//	Inherited API
//
protected:
	virtual	void		OnStart(const XPoint& startPt);
	
	virtual	void 		OnConstrain(const XPoint& anchorPt, const XPoint& prevPt, XPoint& nextPt, const XRect& bounds);

	virtual	void 		OnTrack(const XPoint& anchorPt, const XPoint& prevPt, const XPoint& nextPt, bool mouseMoved);
	
	virtual	void		OnStop(const XPoint& stopPt);

//-----------------------------------
//	Internal API
//
private:	
	virtual XRegion 	DoGetSelectionRegion(const XRect& selection) const;
	virtual void 		DoDrawSelection(const XRegion& rgn);

//-----------------------------------
//	Member Data
//
private:
	XRegion		mDrawnBits;			// the pixels that have been drawn
	XRect		mSelection;
	
	XSize		mMinSize;			// minimum selection size (defaults to 3, 3)
	XBrush		mBrush;
};

DEFINE_INTERFACE_FACTORY(XRubberBandTracker)

//---------------------------------------------------------------
//
// XRubberBandTracker::~XRubberBandTracker
//
//---------------------------------------------------------------
XRubberBandTracker::~XRubberBandTracker()
{
	mBrush.SetDrawMode(kCopyMode);	// brushes have reference semantics so we'll restore kSelectionPens[0] here...
}


//---------------------------------------------------------------
//
// XRubberBandTracker::XRubberBandTracker
//
//---------------------------------------------------------------
XRubberBandTracker::XRubberBandTracker(XBoss* boss) 
{
	ITrackerHelper::DoSetBoss(boss);
	
	mBrush = kSelectionBrushes[0];
	mBrush.SetDrawMode(kXOrMode);
			
	mMinSize.width  = 3;
	mMinSize.height = 3;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XRubberBandTracker::OnStart
//
//---------------------------------------------------------------
void XRubberBandTracker::OnStart(const XPoint& startPt)
{
	UNUSED(startPt);

	mSelection = kZeroRect;
}

	
//---------------------------------------------------------------
//
// XRubberBandTracker::OnConstrain
//
//---------------------------------------------------------------
void XRubberBandTracker::OnConstrain(const XPoint& anchorPt, const XPoint& prevPt, XPoint& nextPt, const XRect& bounds)
{
	UNUSED(anchorPt);
	UNUSED(prevPt);

	nextPt = bounds.Pin(nextPt);
}


//---------------------------------------------------------------
//
// XRubberBandTracker::OnTrack
//
//---------------------------------------------------------------
void XRubberBandTracker::OnTrack(const XPoint& anchorPt, const XPoint& prevPt, const XPoint& nextPt, bool mouseMoved)
{
	UNUSED(anchorPt);
	UNUSED(prevPt);
	UNUSED(nextPt);
	UNUSED(mouseMoved);
	
	ITrackerPtr tracker(this);
	mSelection = tracker->GetArea();

	if (mSelection.GetWidth() >= mMinSize.width && mSelection.GetHeight() >= mMinSize.height) {
		XRegion newRgn = this->DoGetSelectionRegion(mSelection);

#if MAC
		this->DoDrawSelection(mDrawnBits ^ newRgn);		// only draw the pixels that have changed

		mDrawnBits = newRgn;

#else											// Windows regions pretty much suck so we have to live with a bit of flicker
		this->DoDrawSelection(mDrawnBits);		// erase the old rect
		this->DoDrawSelection(newRgn);			// draw the new rect

		mDrawnBits = newRgn;
#endif
	}
}

	
//---------------------------------------------------------------
//
// XRubberBandTracker::OnStop
//
//---------------------------------------------------------------
void XRubberBandTracker::OnStop(const XPoint& stopPt)
{
	UNUSED(stopPt);

	ITrackerPtr tracker(this);
	mSelection = tracker->GetArea();

	this->DoDrawSelection(mDrawnBits);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XRubberBandTracker::DoGetSelectionRegion
//
//---------------------------------------------------------------
XRegion XRubberBandTracker::DoGetSelectionRegion(const XRect& selection) const
{
	XRegion rgn;
	
	XRect inner = selection;
	inner.Inset(1, 1);

	IConstTrackerPtr tracker(this);
	IControlPtr control = tracker->GetControl();
	
	if (control) {
		XControlContext context(control);
		rgn = (XRegion(selection) & context.GetClip()) - XRegion(inner);
		
	} else {
		IWindowPtr window = tracker->GetWindow();
	
		XWindowContext context(window->GetOSWindow());
		rgn = (XRegion(selection) & context.GetClip()) - XRegion(inner);
	}
	
	return rgn;
}

	
//---------------------------------------------------------------
//
// XRubberBandTracker::DoDrawSelection
//
//---------------------------------------------------------------
void XRubberBandTracker::DoDrawSelection(const XRegion& rgn)
{
	ITrackerPtr tracker(this);
	IControlPtr control = tracker->GetControl();
	
	if (control) {
		XControlContext context(control);			// create a new context each time in case the control scrolls
		XRegionShape::Frame(context, rgn, mBrush);	
	
	} else {
		IWindowPtr window = tracker->GetWindow();
	
		XWindowContext context(window->GetOSWindow());
		XRegionShape::Frame(context, rgn, mBrush);	
	}
}
		

}	// namespace Whisper
