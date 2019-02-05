/*
 *  File:       MControlGeometry.cpp
 *  Summary:   	IGeometry that works with controls.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MControlGeometry.cpp,v $
 *		Revision 1.6  2001/04/21 03:24:30  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/01 04:54:48  jesjones
 *		Changed DoSetSize so that it doesn't assert if the hook function changes the control's size.
 *		
 *		Revision 1.4  2001/02/07 02:28:06  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/10 04:02:57  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:11:08  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IGeometry.h>

#include <IControl.h>
#include <IHierarchy.h>
#include <ILocationChanged.h>
#include <IParentChangedSize.h>
#include <ISizeChanged.h>

namespace Whisper {


// ===================================================================================
//	class XControlGeometry
//!		IGeometry that works with controls.
// ===================================================================================
class XControlGeometry : public IGeometry {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XControlGeometry();
	
						XControlGeometry(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual XPoint 		GetLocation() const;
	virtual void 		SetLocation(const XPoint& newLoc, bool redraw = kRedraw);
	
	virtual int32 		GetWidth() const;	
	virtual int32 		GetHeight() const;
	virtual XSize 		GetSize() const;
	virtual void 		SetSize(const XSize& newSize, bool redraw = kRedraw);

	virtual XRect 		GetFrame() const;
	virtual XRect 		GetExtent() const;
	virtual XRegion 	GetVisibleExtent() const;
	virtual void 		SetFrame(const XRect& newFrame, bool redraw = kRedraw);
			
	virtual XSize 		GetMinSize() const 				{return mMinSize;}
	virtual XSize 		GetMaxSize() const 				{return mMaxSize;}

	virtual void		SetMinSize(const XSize& newMin, bool redraw = kRedraw);
	virtual void		SetMaxSize(const XSize& newMax, bool redraw = kRedraw);
	virtual void 		SetMinMaxSize(const XSize& newMin, const XSize& newMax, bool redraw = kRedraw);

//-----------------------------------
//	Internal API
//
private:
			void 		DoSetSize(const XSize& inSize, bool redraw);
			void 		DoSetLocation(const XPoint& newLoc, bool redraw);
			void 		DoSetMinMaxSize(const XSize& newMin, const XSize& newMax, bool redraw);
				
			void 		DoParentChangedSize(const XSize& delta, bool redraw);
			
//-----------------------------------
//	Member Data
//
private:
	XSize	mMinSize;
	XSize	mMaxSize;
};

DEFINE_INTERFACE_FACTORY(XControlGeometry)

//---------------------------------------------------------------
//
// XControlGeometry::~XControlGeometry
//
//---------------------------------------------------------------
XControlGeometry::~XControlGeometry()
{
}


//---------------------------------------------------------------
//
// XControlGeometry::XControlGeometry
//
//---------------------------------------------------------------
XControlGeometry::XControlGeometry(XBoss* boss) 
{	
	this->DoSetBoss(boss);

	mMinSize = XSize(0, 0);
	mMaxSize = XSize(16000, 16000);
}


//---------------------------------------------------------------
//
// XControlGeometry::GetLocation
//
//---------------------------------------------------------------
XPoint XControlGeometry::GetLocation() const
{
	XPoint loc;
	
	IConstControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	 
	IConstControlPtr parent = GetConstParent<IControl>(control);	// should always have a valid parent (root controls have a different geometry implementation)
	
#if TARGET_CARBON
	Rect bounds;
	(void) GetControlBounds(controlH, &bounds);

	loc = parent->WindowToLocal(XPoint(bounds.left, bounds.top));
		
#else
	XRect bounds = (**controlH).contrlRect;
	loc = parent->WindowToLocal(bounds[topLeft]);
#endif
	
	return loc;
}


//---------------------------------------------------------------
//
// XControlGeometry::SetLocation
//
//---------------------------------------------------------------
void XControlGeometry::SetLocation(const XPoint& newLoc, bool redraw)
{
	CHECK_INVARIANT;
	
	if (newLoc != this->GetLocation())
		this->DoSetLocation(newLoc, redraw);

	POSTCONDITION(this->GetLocation() == newLoc);
}

	
//---------------------------------------------------------------
//
// XControlGeometry::GetSize
//
//---------------------------------------------------------------
XSize XControlGeometry::GetSize() const
{
	IConstControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	 
#if TARGET_CARBON
	Rect frame;
	(void) GetControlBounds(controlH, &frame);

	XSize size(frame.right - frame.left, frame.bottom - frame.top);
		
#else
	XRect frame = (**controlH).contrlRect;
	
	XSize size = frame.GetSize();
#endif

	POSTCONDITION(size.width >= 0);
	POSTCONDITION(size.height >= 0);
	
	return size;
}


//---------------------------------------------------------------
//
// XControlGeometry::GetWidth
//
//---------------------------------------------------------------
int32 XControlGeometry::GetWidth() const
{
	XSize size = this->GetSize();
	
	return size.width;
}


//---------------------------------------------------------------
//
// XControlGeometry::GetHeight
//
//---------------------------------------------------------------
int32 XControlGeometry::GetHeight() const
{
	XSize size = this->GetSize();
	
	return size.height;
}


//---------------------------------------------------------------
//
// XControlGeometry::SetSize
//
//---------------------------------------------------------------
void XControlGeometry::SetSize(const XSize& newSize, bool redraw)
{
	PRECONDITION(newSize.width >= 0 && newSize.height >= 0);
	CHECK_INVARIANT;
	
	if (newSize != this->GetSize())
		this->DoSetSize(newSize, redraw);
}


//---------------------------------------------------------------
//
// XControlGeometry::GetFrame
//
//---------------------------------------------------------------
XRect XControlGeometry::GetFrame() const
{
	XRect frame(this->GetLocation(), this->GetSize());

	return frame;
}


//---------------------------------------------------------------
//
// XControlGeometry::GetExtent
//
//---------------------------------------------------------------
XRect XControlGeometry::GetExtent() const
{
	IConstControlPtr control(this);

	XPoint loc = this->GetLocation();
	loc = control->ParentToLocal(loc);
	
	XRect extent(loc, this->GetSize());

	return extent;
}


//---------------------------------------------------------------
//
// XControlGeometry::GetVisibleExtent
//
//---------------------------------------------------------------
XRegion XControlGeometry::GetVisibleExtent() const
{	
	XRegion extent = this->GetExtent();

	IConstGeometryPtr parent = GetConstParent<IGeometry>(this);
	if (parent) {
		IConstControlPtr control(this);
		
		XPoint offset = control->LocalToParent(kZeroPt);
		extent += offset;

		extent &= parent->GetVisibleExtent();

		extent -= offset;
	}
		
	return extent;
}


//---------------------------------------------------------------
//
// XControlGeometry::SetFrame
//
//---------------------------------------------------------------
void XControlGeometry::SetFrame(const XRect& newFrame, bool redraw)
{
	CHECK_INVARIANT;
	
	this->SetLocation(newFrame[topLeft]);
	this->SetSize(newFrame.GetSize(), redraw);
}


//---------------------------------------------------------------
//
// XControlGeometry::SetMinSize
//
//---------------------------------------------------------------
void XControlGeometry::SetMinSize(const XSize& newMin, bool redraw)
{
	CHECK_INVARIANT;
	
	if (newMin != mMinSize)
		this->DoSetMinMaxSize(newMin, mMaxSize, redraw);
}


//---------------------------------------------------------------
//
// XControlGeometry::SetMaxSize
//
//---------------------------------------------------------------
void XControlGeometry::SetMaxSize(const XSize& newMax, bool redraw)
{
	CHECK_INVARIANT;
	
	if (newMax != mMaxSize)
		this->DoSetMinMaxSize(mMinSize, newMax, redraw);
}


//---------------------------------------------------------------
//
// XControlGeometry::SetMinMaxSize
//
//---------------------------------------------------------------
void XControlGeometry::SetMinMaxSize(const XSize& newMin, const XSize& newMax, bool redraw)
{
	CHECK_INVARIANT;
	
	if (newMin != mMinSize || newMax != mMaxSize)
		this->DoSetMinMaxSize(newMin, newMax, redraw);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XControlGeometry::DoSetSize
//
//---------------------------------------------------------------
void XControlGeometry::DoSetSize(const XSize& inSize, bool redraw)
{
	XSize newSize;
	newSize.width  = MinMax(mMinSize.width, inSize.width, mMaxSize.width);
	newSize.height = MinMax(mMinSize.height, inSize.height, mMaxSize.height);

	if (newSize != this->GetSize()) {
		IControlPtr control(this);
		ControlHandle controlH = control->GetOSControl();
		 
		// Save the old extent
		XRect oldExtent = this->GetExtent();
		ASSERT(newSize != oldExtent.GetSize());
		
		// Resize the control
		::SetPort(control->GetOSDrawContext());
		::HidePen();
		::SizeControl(controlH, numeric_cast<int16>(newSize.width), numeric_cast<int16>(newSize.height));
		::ShowPen();
		
		// Give our children a chance to make adjustments
		XRect newExtent = this->GetExtent();
		XSize delta = newExtent.GetSize() - oldExtent.GetSize();
		this->DoParentChangedSize(delta, redraw);
		
		ASSERT(this->GetSize() == newSize);			// OnSizeChanged may adjust the size so we'll check here instead of at the bottom of the function

		// Optional control notification
		ISizeChangedPtr changed(this);
		if (changed)
			changed->OnSizeChanged(oldExtent.GetSize(), newSize);
	
		// Optionally invalidate
		if (redraw && control->IsVisible())	{		
			oldExtent.Inset(-kDrawOutset, -kDrawOutset);
			newExtent.Inset(-kDrawOutset, -kDrawOutset);
						
			control->Invalidate(oldExtent | newExtent, false);
		}
	}
}


//---------------------------------------------------------------
//
// XControlGeometry::DoSetLocation
//
//---------------------------------------------------------------
void XControlGeometry::DoSetLocation(const XPoint& loc, bool redraw)
{
	PRECONDITION(this->GetLocation() != loc);
	
	IControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();

	if (redraw && control->IsVisible())
		control->Invalidate();
		
	IControlPtr parent = GetParent<IControl>(control);
	XPoint newLoc = parent->LocalToWindow(loc);
	
	::SetPort(control->GetOSDrawContext());
	::HidePen();
	::MoveControl(controlH, numeric_cast<int16>(newLoc.x), numeric_cast<int16>(newLoc.y));
	::ShowPen();
			
	ILocationChangedPtr changed(this);
	if (changed)
		changed->OnLocationChanged(newLoc);
	
	if (redraw && control->IsVisible())
		control->Invalidate();
}


//---------------------------------------------------------------
//
// XControlGeometry::DoSetMinMaxSize
//
//---------------------------------------------------------------
void XControlGeometry::DoSetMinMaxSize(const XSize& newMin, const XSize& newMax, bool redraw)
{
	mMinSize = newMin;
	mMaxSize = newMax;
		
	// Min size must be smaller than max size.
	mMinSize.width  = Min(mMaxSize.width, mMinSize.width);
	mMinSize.height = Min(mMaxSize.height, mMinSize.height);
		
	// Max size must be larger than min size.
	mMaxSize.width  = Max(mMaxSize.width, mMinSize.width);
	mMaxSize.height = Max(mMaxSize.height, mMinSize.height);
	
	// Control size must be in [min, max].
	XSize newSize = this->GetSize();
	
	newSize.width  = MinMax(mMinSize.width, newSize.width, mMaxSize.width);
	newSize.height = MinMax(mMinSize.height, newSize.height, mMaxSize.height);

	if (newSize != this->GetSize())
		this->DoSetSize(newSize, redraw);
}


//---------------------------------------------------------------
//
// XControlGeometry::DoParentChangedSize
//
//---------------------------------------------------------------
void XControlGeometry::DoParentChangedSize(const XSize& delta, bool redraw)
{
	IHierarchyPtr root(this);
	
	IHierarchy::iterator iter = root->begin();
	while (iter != root->end()) {
		IHierarchyPtr child = *iter;
		++iter;
		
		IParentChangedSizePtr notify(child);
		if (notify)
			notify->HandleParentSizeChange(delta, redraw);
	}
}


}	// namespace Whisper
