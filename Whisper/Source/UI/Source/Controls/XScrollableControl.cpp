/*
 *  File:       XScrollableControl.cpp
 *  Summary:   	A user control that is capable of being scrolled.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XScrollableControl.cpp,v $
 *		Revision 1.9  2001/04/27 04:36:12  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.8  2001/04/17 01:44:52  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.7  2001/04/13 08:03:39  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.6  2001/02/07 02:29:30  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.5  2000/12/14 08:37:22  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.4  2000/12/10 04:11:29  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.3  2000/11/09 12:50:40  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IScrollableControl.h>

#include <IBinding.h>
#include <IControl.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IScrollerControl.h>
#include <ISizeChanged.h>
#include <IWindow.h>
#include <XExceptions.h>
#include <XMiscUtils.h>

namespace Whisper {


// ===================================================================================
//	class XScrollableControl
//!		A user control that is capable of being scrolled.
// ===================================================================================
class XScrollableControl : public IScrollableControl, public ISizeChanged {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XScrollableControl();
	
						XScrollableControl(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init();

	virtual void 		ScrollTo(const XPoint& loc, bool redraw = kRedraw);
	virtual void 		ScrollBy(const XPoint& delta, bool redraw = kRedraw);
	
	virtual void 		ScrollIntoView(const XPoint& loc, bool redraw = kRedraw);

	virtual void 		AutoScroll(const XPoint& localPt);

	virtual XPoint 		GetScrollOffset() const;

	virtual XSize		GetImageSize() const						{return mImageSize;}
	virtual void 		SetImageSize(const XSize& newSize, bool redraw = kRedraw);
	
	virtual XRect		GetRevealedRect() const						{return mRevealedRect;}
						
	virtual void 		SetAdjustOrigin(bool value = true)			{mAdjustOrigin = value;}
	virtual void 		SetReconcileOverhang(bool value = true)		{mReconcileOverhang = value;}

protected:
	virtual void 		Invariant() const;
	
	virtual void 		OnSizeChanged(const XSize& oldSize, const XSize& newSize);

//-----------------------------------
//	Internal API
//
protected:
			MilliSecond	DoGetAutoScrollDelay(int32 distance) const;
			void		DoScrollTo(const XPoint& loc, bool redraw);
	
			bool 		DoReconcileOverhang(XPoint& loc);
			void 		DoScrollBits(int32 dx, int32 dy);

//-----------------------------------
//	Member Data
//
private:
	XSize		mImageSize;			// the size of the image the control is being scrolled in
	XRect		mRevealedRect;		// the rectangle within the image the control is currently displaying (private to ensure OnSubPaneScrolled is called)

	bool		mReconcileOverhang;	// defaults to true which means the control will do everything it can to ensure mRevealedRect is always within the image
	bool		mAdjustOrigin;		// defaults to true which means the origin will be shifted according to how far the user has scrolled in the image (image size must be < 32K)
	
	MilliSecond	mMinDelay;			// auto scroll won't scroll any faster than this
	MilliSecond	mBaseDelay;			// auto scroll delay when mouse is just outside control
	MilliSecond	mDelayScaleFactor;	// number of milliseconds faster to scroll when the mouse moves one pixel farther from control
};

DEFINE_INTERFACE_FACTORY(XScrollableControl)

//---------------------------------------------------------------
//
// XScrollableControl::~XScrollableControl
//
//---------------------------------------------------------------
XScrollableControl::~XScrollableControl()
{
}


//---------------------------------------------------------------
//
// XScrollableControl::XScrollableControl
//
//---------------------------------------------------------------
XScrollableControl::XScrollableControl(XBoss* boss)
{	
	IScrollableControl::DoSetBoss(boss);
	ISizeChanged::DoSetBoss(boss);
	
	mImageSize = kZeroSize;
	mRevealedRect = kZeroRect;
	
	mReconcileOverhang = true;				// force mRevealedRect to stay in image
	mAdjustOrigin = true;
	
	mMinDelay = 5;
	mBaseDelay = 150;
	mDelayScaleFactor = 3;
}


//---------------------------------------------------------------
//
// XScrollableControl::Init
//
//---------------------------------------------------------------
void XScrollableControl::Init()
{	
	IScrollableControl* thisPtr = this;
	IGeometryPtr geometry(thisPtr);
	
	mImageSize = geometry->GetSize();	
	mRevealedRect = geometry->GetExtent();
	
	IBindingPtr binding(thisPtr);
	if (binding)
		binding->SetBinding(SBinding(true, true, true, true));
	
	this->Invariant();

	IScrollerControlPtr scroller = GetParent<IScrollerControl>(thisPtr);
	scroller->ScrollAreaHasChanged();
}


//---------------------------------------------------------------
//
// XScrollableControl::ScrollTo
//
//---------------------------------------------------------------
void XScrollableControl::ScrollTo(const XPoint& loc, bool redraw)
{
	PRECONDITION(loc.x >= 0 && loc.x < mImageSize.width);
	PRECONDITION(loc.y >= 0 && loc.y < mImageSize.height);
	CHECK_INVARIANT;
	
	if (loc != mRevealedRect[topLeft]) {
		XPoint to = loc;
		if (mReconcileOverhang)
			this->DoReconcileOverhang(to);
			
		if (to != mRevealedRect[topLeft])
			this->DoScrollTo(to, redraw);
	}
}


//---------------------------------------------------------------
//
// XScrollableControl::ScrollBy
//
//---------------------------------------------------------------
void XScrollableControl::ScrollBy(const XPoint& delta, bool redraw)
{
	CHECK_INVARIANT;
	
	if (delta != kZeroPt) {
		XPoint loc = mRevealedRect[topLeft] + delta;
		
		if (mReconcileOverhang)
			this->DoReconcileOverhang(loc);
			
		if (loc != mRevealedRect[topLeft])
			this->DoScrollTo(loc, redraw);
	}
}

	
//---------------------------------------------------------------
//
// XScrollableControl::ScrollIntoView
//
//---------------------------------------------------------------
void XScrollableControl::ScrollIntoView(const XPoint& loc, bool redraw)
{
	CHECK_INVARIANT;
	
	XRect revealed = this->GetRevealedRect();
	XRect image = XRect(kZeroPt, mImageSize);
	
	PRECONDITION(image.Contains(loc));
	
	XPoint newLoc = revealed[topLeft];
	
	if (loc.x < revealed.left && revealed.left > image.left)
		newLoc.x  = loc.x;
	else if (loc.x > revealed.right && revealed.right < image.right)
		newLoc.x  = loc.x;

	if (loc.y < revealed.top && revealed.top > image.top)
		newLoc.y  = loc.y;
	else if (loc.y > revealed.bottom && revealed.bottom < image.bottom)
		newLoc.y  = loc.y;
		
	if (newLoc != revealed[topLeft])
		this->ScrollTo(newLoc, redraw);
}


//---------------------------------------------------------------
//
// XScrollableControl::AutoScroll
//
//---------------------------------------------------------------
void XScrollableControl::AutoScroll(const XPoint& pt)
{
	CHECK_INVARIANT;
	
	IScrollableControl* thisPtr = this;
	IGeometryPtr geometry(thisPtr);

	XRect extent = geometry->GetExtent();
	XRect revealed = this->GetRevealedRect();
	XRect image = XRect(kZeroPt, mImageSize);
	
	XPoint delta = kZeroPt;
	int32 distance = 0;
	
	if (pt.x < extent.left && revealed.left > image.left) {
		delta.x  = -1;
		distance = Max(distance, extent.left - pt.x);
		
	} else if (pt.x > extent.right && revealed.right < image.right) {
		delta.x  = +1;
		distance = Max(distance, pt.x - extent.right);
	}

	if (pt.y < extent.top && revealed.top > image.top) {
		delta.y  = -1;
		distance = Max(distance, extent.top - pt.y);
		
	} else if (pt.y > extent.bottom && revealed.bottom < image.bottom) {
		delta.y  = +1;
		distance = Max(distance, pt.y - extent.bottom);
	}
		
	if (delta != kZeroPt) {
		this->ScrollBy(delta);

		MilliSecond delay = this->DoGetAutoScrollDelay(distance);
		MilliSecondDelay(delay);
	}
}


//---------------------------------------------------------------
//
// XScrollableControl::SetImageSize
//
//---------------------------------------------------------------
void XScrollableControl::SetImageSize(const XSize& newSize, bool redraw)
{
	PRECONDITION(newSize.width >= 0);
	PRECONDITION(newSize.height >= 0);
	CHECK_INVARIANT;
	
	if (newSize != mImageSize) {
		mImageSize = newSize;				
		
		XRect imageRect(kZeroPt, mImageSize);	
	
		if (!imageRect.Contains(mRevealedRect[topLeft]))
			this->DoScrollTo(kZeroPt, redraw);
	
		if (mReconcileOverhang) {
			XPoint loc = mRevealedRect[topLeft];
			
			if (this->DoReconcileOverhang(loc))
				this->DoScrollTo(loc, redraw);
		}
		
		IScrollableControl* thisPtr = this;
		IScrollerControlPtr scroller = GetParent<IScrollerControl>(thisPtr);
		scroller->ScrollAreaHasChanged();
	}
}


//---------------------------------------------------------------
//
// XScrollableControl::GetScrollOffset
//
//---------------------------------------------------------------
XPoint XScrollableControl::GetScrollOffset() const
{
	XPoint offset = kZeroPt;
	
	if (mAdjustOrigin) {
		PRECONDITION(mImageSize.width <= SHRT_MAX);			// required on Win95 and Mac (but not NT...)
		PRECONDITION(mImageSize.height <= SHRT_MAX);
		
		offset -= XPoint(mRevealedRect.left, mRevealedRect.top);
	}
	
	return offset;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XScrollableControl::Invariant
//
//---------------------------------------------------------------
void XScrollableControl::Invariant() const
{
	IScrollableControl::Invariant();
		
	ASSERT(mImageSize.width >= 0);
	ASSERT(mImageSize.height >= 0);
	
	XRect imageRect(kZeroPt, mImageSize);
	if (mImageSize != kZeroSize)
		ASSERT(imageRect.Contains(mRevealedRect[topLeft]));
	
	ASSERT(mRevealedRect.GetWidth() <= SHRT_MAX);
	ASSERT(mRevealedRect.GetHeight() <= SHRT_MAX);
	
	ASSERT(mMinDelay >= 0);
	ASSERT(mBaseDelay >= mMinDelay);
	ASSERT(mDelayScaleFactor >= 0);
}


//---------------------------------------------------------------
//
// XScrollableControl::OnSizeChanged
//
//---------------------------------------------------------------
void XScrollableControl::OnSizeChanged(const XSize& oldSize, const XSize& newSize)
{	
	PRECONDITION(oldSize != newSize);
	
	mRevealedRect.right  = mRevealedRect.left + newSize.width;
	mRevealedRect.bottom = mRevealedRect.top + newSize.height;

	if (mReconcileOverhang) {
		XPoint loc = mRevealedRect[topLeft];
		
		if (this->DoReconcileOverhang(loc))
			this->DoScrollTo(loc, kRedraw);
	}

	IScrollableControl* thisPtr = this;
	IScrollerControlPtr scroller = GetParent<IScrollerControl>(thisPtr);
	scroller->ScrollAreaHasChanged();
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XScrollableControl::DoGetAutoScrollDelay
//
// Returns the length of time AutoScroll should delay based on the
// distance of localPt from the edge of the control. Default 
// calculates a time inversely proportional to distance.
//
//---------------------------------------------------------------
MilliSecond	XScrollableControl::DoGetAutoScrollDelay(int32 distance) const
{
	PRECONDITION(distance > 0);
	
	MilliSecond delay = Max(mBaseDelay - mDelayScaleFactor*distance, mMinDelay);

#if DEBUG && 0
	static MilliSecond oldDelay = 0;
	
	if (oldDelay != delay) {
		TRACEFLOW("XAutoScroll", "auto scroll delay = ", delay, "\n");
		oldDelay = delay;
	}
#endif
	
	return delay;
}

			
//---------------------------------------------------------------
//
// XScrollableControl::DoScrollTo
//
//---------------------------------------------------------------
void XScrollableControl::DoScrollTo(const XPoint& inLoc, bool redraw)
{
	XPoint loc(Max(inLoc.x, 0L), Max(inLoc.y, 0L));
	
	XPoint oldLoc = mRevealedRect[topLeft];
	
	if (loc != oldLoc) {
		XPoint delta;
		delta.x = loc.x - mRevealedRect.left;
		delta.y = loc.y - mRevealedRect.top;

		mRevealedRect += delta;
		
		IScrollableControl* thisPtr = this;
		IScrollerControlPtr scroller = GetParent<IScrollerControl>(thisPtr);
		scroller->ScrollAreaHasChanged();

		// If part of the view will remain visible use the toolbox 
		// to scroll the bits. Otherwise invalidate the view.
		if (redraw) {
			IGeometryPtr geometry(thisPtr);
	
			XSize size = geometry->GetSize();
			if (Abs(delta.x) < size.width && Abs(delta.y) < size.height)
				this->DoScrollBits(delta.x, delta.y);
			else {
				IControlPtr control(thisPtr);
				control->Invalidate();
			}
		}
	}
}


//---------------------------------------------------------------
//
// XScrollableControl::DoReconcileOverhang
//
// Attempt to adjust loc so mRevealedRect will be within the image.
//
//---------------------------------------------------------------
bool XScrollableControl::DoReconcileOverhang(XPoint& loc)
{
	PRECONDITION(mReconcileOverhang);
	
	XPoint oldLoc = loc;
	
	int32 right = loc.x + mRevealedRect.GetWidth();
	if (right > mImageSize.width) 
		loc.x = Max(loc.x - (right - mImageSize.width), 0L);
	
	int32 bottom = loc.y + mRevealedRect.GetHeight();
	if (bottom > mImageSize.height) 
		loc.y = Max(loc.y - (bottom - mImageSize.height), 0L);

	return loc != oldLoc;
}


//---------------------------------------------------------------
//
// XScrollableControl::DoScrollBits
//
//---------------------------------------------------------------
void XScrollableControl::DoScrollBits(int32 dx, int32 dy)
{
	IScrollableControl* thisPtr = this;
	IControlPtr control(thisPtr);
	IGeometryPtr geometry(thisPtr);
	IWindowPtr window = GetParent<IWindow>(thisPtr);
	
	OSControl controlH = control->GetOSControl();

#if WIN
	XRect extent(kZeroPt, geometry->GetSize());

	XPoint offset = kZeroPt;
	offset = this->GetScrollOffset();
	
	// Scroll the window contents
	XRegion updateRgn;
	int32 result = ::ScrollWindowEx(controlH,	// window
									-dx,		// horz scrolling
									-dy,		// vertical scrolling
									nil,		// scroll rectangle (nil for entire client area)
									extent,		// clipping rectangle
									updateRgn,	// update region
									nil,		// update rectangle
									0);			// scroll flags
	ThrowIf(result == ERROR);
	
	// Scroll the old update region
//	XRegion oldUpdate;
//	result = ::GetUpdateRgn(mControl, oldUpdate, kDontRedraw);
//	ThrowIf(result == ERROR);
//	oldUpdate += XPoint(dx, dy);

	// Invalidate the dirty region
//	this->Validate();
//	this->Invalidate(oldUpdate + updateRgn + offset);
	control->Invalidate(updateRgn - offset);

#elif TARGET_CARBON
	if (control->IsVisible()) {
		WindowRef wptr = window->GetOSWindow();
		Rect extent = geometry->GetExtent().GetOSRect();
		
		// $$$ this will scroll without erasing, but I'm not sure if it will offset
		// $$$ the old update region
		OSStatus err = ::ScrollWindowRect(wptr, &extent, numeric_cast<int16>(dx), numeric_cast<int16>(dy), kScrollWindowInvalidate, nil);
		ThrowIfOSErr(err);					
	}

#elif MAC && 1
	if (control->IsVisible()) {
		WindowRef wptr = window->GetOSWindow();
		bool isWindow = ::GetWindowKind(wptr) >= kWhisperWindowKind;
		
		// Scroll the update region,	
		XRegion updateRgn;
		::GetWindowUpdateRgn(wptr, updateRgn);
		
		XPoint offset = window->GlobalToWindow(kZeroPt);
		offset = control->WindowToLocal(offset);
		offset -= XPoint(dx, dy);
		updateRgn += offset;
	
		// and scroll the bits.
		XControlContext context(control);
		context.MakeCurrent();
		XRect extent = geometry->GetExtent();
		
		XRegion dirtyRgn;
		Whisper::MyScrollRect(extent, numeric_cast<int16>(-dx), numeric_cast<int16>(-dy), dirtyRgn);
		updateRgn += dirtyRgn;
		
		// If the control isn't completely exposed (eg it may be partially
		// hidden by a floating window) we need to scroll the hidden area 
		// and invalidate whatever enters the visible region (because we're 
		// copying bits from the screen to a different part of the screen 
		// and only the writes are clipped to the window's visRgn).
		XRegion visbleRgn(wptr->visRgn);
		visbleRgn.MoveTo(kZeroPt);

		if (!visbleRgn.Contains(control->LocalToWindow(extent))) {
			visbleRgn += control->WindowToLocal(kZeroPt);
			
			XRegion hiddenRgn = XRegion(extent) - visbleRgn;
		
			updateRgn += hiddenRgn + XPoint(-dx, -dy);
			updateRgn -= hiddenRgn;
		}
		
		// Validate the entire control and invalidate the original
		// (scrolled) update region plus the newly exposed region.
		// Note that Validate will clip to the control so we're
		// OK if the original update region was larger than the
		// control.
		control->Validate();
		control->Invalidate(updateRgn);
	}

#elif MAC && 0
	if (this->IsVisible()) {								// this code works but the code above does a better job of minimizing invalidations
		WindowRef wptr = this->GetOSContext();
		XRect extent = geometry->GetExtent();
		
		bool isWindow = ::GetWindowKind(wptr) >= kWhisperWindowKind;
		
		XRegion visbleRgn(wptr->visRgn);
		visbleRgn.MoveTo(kZeroPt);
		bool visible = isWindow && visbleRgn.Contains(this->LocalToWindow(extent));
			
		// If the control is visible then,
		if (visible) {	
	
			// scroll the update region.
			XRegion updateRgn;
			::GetWindowUpdateRgn(wptr, updateRgn);
			
			XPoint offset = this->GlobalToWindow(kZeroPt);
			offset = this->WindowToLocal(offset);
			offset -= XPoint(dx, dy);
			updateRgn += offset;
		
			// and scroll the bits. Note that it's important that
			// we do this only when the entire control is visible
			// because we're copying bits from the screen to a 
			// different part of the screen and only the writes
			// are clipped to the window's visRgn.
			XControlContext context(this);
			context.MakeCurrent();
					
			XRegion dirtyRgn;
			Whisper::MyScrollRect(extent, numeric_cast<int16>(-dx), numeric_cast<int16>(-dy), dirtyRgn);
						
			updateRgn += dirtyRgn;
				
			// Validate the entire control and invalidate the original
			// (scrolled) update region plus the newly exposed region.
			// Note that Validate will clip to the control so we're
			// OK if the original update region was larger than the
			// control.
			this->Validate();
			this->Invalidate(updateRgn);
			
		} else
			// If the control isn't in a window or it's obscured by
			// another window we'll invalidate the entire control. This
			// may or may not do anything if we're not in a window...
			this->Invalidate();	
	}
#endif
}


}	// namespace Whisper
