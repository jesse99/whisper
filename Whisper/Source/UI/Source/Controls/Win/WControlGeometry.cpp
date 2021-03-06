/*
 *  File:       WControlGeometry.cpp
 *  Summary:   	IGeometry that works with controls.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WControlGeometry.cpp,v $
 *		Revision 1.8  2001/04/21 03:30:45  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.7  2001/04/06 04:45:40  jesjones
 *		Disabled some asserts.
 *		
 *		Revision 1.6  2001/02/21 08:15:58  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.5  2001/01/03 05:55:50  jesjones
 *		GetVisibleExtent no longer bothers with windows (root control should be good enough).
 *		
 *		Revision 1.4  2000/12/25 02:47:00  jesjones
 *		Added a gross hack to work around some Windows stupidity with textboxes.
 *		
 *		Revision 1.3  2000/12/17 03:50:18  jesjones
 *		Removed dollar signs around Log.
 *		
 *		Revision 1.2  2000/11/09 12:05:38  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IGeometry.h>

#include <IControl.h>
#include <IHierarchy.h>
#include <IParentChangedSize.h>
#include <IScrollableControl.h>
#include <ISizeChanged.h>
#include <ITextBox.h>
#include <IWindow.h>
#include <XExceptions.h>

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
	XPoint loc = kZeroPt;
	
	IConstControlPtr control(this);
	HWND controlH = control->GetOSControl();
	 
	IConstControlPtr parent = GetConstParent<IControl>(control);
	if (parent)
		(void) ::MapWindowPoints(controlH, parent->GetOSControl(), loc, 1);
	
	else {
		IConstWindowPtr window = GetConstParent<IWindow>(control);
		(void) ::MapWindowPoints(controlH, window->GetOSWindow(), loc, 1);
	}
	
	IConstTextBoxPtr textbox(this);
	if (textbox) 
		loc -= XPoint(2, 2);		// $$$ textboxes are screwy...
	
	return loc;
}


//---------------------------------------------------------------
//
// XControlGeometry::SetLocation
//
//---------------------------------------------------------------
void XControlGeometry::SetLocation(const XPoint& newLoc, bool redraw)
{	
	if (newLoc != this->GetLocation())
		this->DoSetLocation(newLoc, redraw);

//	XPoint loc = this->GetLocation();
//	POSTCONDITION(this->GetLocation() == newLoc);	// some of the more complex controls don't go right where you tell them...
}

	
//---------------------------------------------------------------
//
// XControlGeometry::GetSize
//
//---------------------------------------------------------------
XSize XControlGeometry::GetSize() const
{
	IConstControlPtr control(this);
	HWND controlH = control->GetOSControl();
	 
	XRect extent;
	VERIFY(::GetClientRect(controlH, extent));
	
	IConstTextBoxPtr textbox(this);
	if (textbox) 
		extent.Inset(-2, -2);			// $$$ textboxes are screwy...
	
	XSize size = extent.GetSize();
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
	XPoint origin = kZeroPt;
	ConstScrollableControlPtr scrollable(this);
	if (scrollable) 
		origin = -scrollable->GetScrollOffset();
		
	XRect extent(origin, this->GetSize());

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

	IConstControlPtr control = GetConstParent<IControl>(this);
	if (control) {
		XPoint offset = control->LocalToParent(kZeroPt);
		extent += offset;

		IConstGeometryPtr geometry(control);
		extent &= geometry->GetVisibleExtent();
		
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
		HWND controlH = control->GetOSControl();
		 
		// Save the old extent
		XRect oldExtent = this->GetExtent();
		ASSERT(newSize != oldExtent.GetSize());
		
		// Resize the control
		XPoint loc = this->GetLocation();
		int32 succeeded = ::MoveWindow(controlH, loc.x, loc.y, newSize.width, newSize.height, kDontRedraw);
		ThrowIf(!succeeded);
				
		// Give our children a chance to make adjustments
		XRect newExtent = this->GetExtent();
		XSize delta = newExtent.GetSize() - oldExtent.GetSize();
		this->DoParentChangedSize(delta, redraw);
		
		// Optional control notification
		ISizeChangedPtr changed(this);
		if (changed)
			changed->OnSizeChanged(oldExtent.GetSize(), newSize);
	
		// Optionally invalidate
		if (redraw && control->IsVisible())
			control->Invalidate(oldExtent | newExtent, false);
	}
	
//	XSize size = this->GetSize();
//	POSTCONDITION(this->GetSize() == newSize);	// some of the more complex controls don't size exactly as you tell them...
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
	HWND controlH = control->GetOSControl();

	if (redraw && control->IsVisible())
		control->Invalidate();
	
	XSize size = this->GetSize();
	int32 succeeded = ::MoveWindow(controlH, loc.x, loc.y, size.width, size.height, kDontRedraw);
	ThrowIf(!succeeded);
			
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
