/*
 *  File:       MWindowGeometry.cpp
 *  Summary:   	IGeometry that works with windows.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MWindowGeometry.cpp,v $
 *		Revision 1.8  2001/04/21 03:29:41  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.7  2001/04/13 07:54:16  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.6  2001/02/07 02:29:00  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.5  2000/12/31 04:36:12  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.4  2000/12/14 08:29:22  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/10 04:05:54  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:22:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IGeometry.h>

#include <ILocationChanged.h>
#include <IHierarchy.h>
#include <IParentChangedSize.h>
#include <ISizeChanged.h>
#include <IWindow.h>
#include <XDesktop.h>
#include <XNumbers.h>
#include <XStateBroadcaster.h>

namespace Whisper {


// ===================================================================================
//	class XWindowGeometry
//!		IGeometry that works with windows.
// ===================================================================================
class XWindowGeometry : public IGeometry {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XWindowGeometry();
	
						XWindowGeometry(XBoss* boss);
						
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
			
	virtual XSize 		GetMinSize() const;
	virtual XSize 		GetMaxSize() const;

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

DEFINE_INTERFACE_FACTORY(XWindowGeometry)

//---------------------------------------------------------------
//
// XWindowGeometry::~XWindowGeometry
//
//---------------------------------------------------------------
XWindowGeometry::~XWindowGeometry()
{
}


//---------------------------------------------------------------
//
// XWindowGeometry::XWindowGeometry
//
//---------------------------------------------------------------
XWindowGeometry::XWindowGeometry(XBoss* boss) 
{	
	this->DoSetBoss(boss);

	mMinSize = XSize(16, 16);
	mMaxSize = XSize(16000, 16000);
}


//---------------------------------------------------------------
//
// XWindowGeometry::GetLocation
//
//---------------------------------------------------------------
XPoint XWindowGeometry::GetLocation() const
{
	IConstWindowPtr window(this);
	
	XRect frame = window->GetContentBounds();
	
	return frame[topLeft];
}


//---------------------------------------------------------------
//
// XWindowGeometry::SetLocation
//
//---------------------------------------------------------------
void XWindowGeometry::SetLocation(const XPoint& newLoc, bool redraw)
{
	CHECK_INVARIANT;
	
	if (newLoc != this->GetLocation())
		this->DoSetLocation(newLoc, redraw);

	POSTCONDITION(this->GetLocation() == newLoc);
}

	
//---------------------------------------------------------------
//
// XWindowGeometry::GetWidth
//
//---------------------------------------------------------------
int32 XWindowGeometry::GetWidth() const
{
	IConstWindowPtr window(this);
	
	XRect frame = window->GetContentBounds();
	
	int32 width = frame.GetWidth();
	
	return width;
}


//---------------------------------------------------------------
//
// XWindowGeometry::GetHeight
//
//---------------------------------------------------------------
int32 XWindowGeometry::GetHeight() const
{
	IConstWindowPtr window(this);
	
	XRect frame = window->GetContentBounds();
	
	int32 height = frame.GetHeight();
	
	return height;
}


//---------------------------------------------------------------
//
// XWindowGeometry::GetSize
//
//---------------------------------------------------------------
XSize XWindowGeometry::GetSize() const
{
	IConstWindowPtr window(this);
	
	XRect frame = window->GetContentBounds();
	
	XSize size = frame.GetSize();
	
	return size;
}


//---------------------------------------------------------------
//
// XWindowGeometry::SetSize
//
//---------------------------------------------------------------
void XWindowGeometry::SetSize(const XSize& newSize, bool redraw)
{
	PRECONDITION(newSize.width >= 0 && newSize.height >= 0);
	CHECK_INVARIANT;
	
	if (newSize != this->GetSize())
		this->DoSetSize(newSize, redraw);
}


//---------------------------------------------------------------
//
// XWindowGeometry::GetFrame
//
//---------------------------------------------------------------
XRect XWindowGeometry::GetFrame() const
{
	return XRect(this->GetLocation(), this->GetSize());
}


//---------------------------------------------------------------
//
// XWindowGeometry::GetExtent
//
//---------------------------------------------------------------
XRect XWindowGeometry::GetExtent() const
{
	return XRect(kZeroPt, this->GetSize());
}


//---------------------------------------------------------------
//
// XWindowGeometry::GetVisibleExtent
//
//---------------------------------------------------------------
XRegion XWindowGeometry::GetVisibleExtent() const
{	
	XRect frame = this->GetFrame();
	XRegion desktop = XDesktop::GetClientArea();
	
	XRegion extent = desktop & frame;
	extent.MoveTo(kZeroPt);
	
	return extent;
}


//---------------------------------------------------------------
//
// XWindowGeometry::SetFrame
//
//---------------------------------------------------------------
void XWindowGeometry::SetFrame(const XRect& newFrame, bool redraw)
{
	CHECK_INVARIANT;
	
	this->SetLocation(newFrame[topLeft]);
	this->SetSize(newFrame.GetSize(), redraw);
}


//---------------------------------------------------------------
//
// XWindowGeometry::SetMinSize
//
//---------------------------------------------------------------
void XWindowGeometry::SetMinSize(const XSize& newMin, bool redraw)
{
	CHECK_INVARIANT;
	
	if (newMin != mMinSize)
		this->DoSetMinMaxSize(newMin, mMaxSize, redraw);
}


//---------------------------------------------------------------
//
// XWindowGeometry::GetMinSize
//
//---------------------------------------------------------------
XSize XWindowGeometry::GetMinSize() const
{
	return mMinSize;
}


//---------------------------------------------------------------
//
// XWindowGeometry::GetMaxSize
//
//---------------------------------------------------------------
XSize XWindowGeometry::GetMaxSize() const
{
	return mMaxSize;
}


//---------------------------------------------------------------
//
// XWindowGeometry::SetMaxSize
//
//---------------------------------------------------------------
void XWindowGeometry::SetMaxSize(const XSize& newMax, bool redraw)
{
	CHECK_INVARIANT;
	
	if (newMax != mMaxSize)
		this->DoSetMinMaxSize(mMinSize, newMax, redraw);
}


//---------------------------------------------------------------
//
// XWindowGeometry::SetMinMaxSize
//
//---------------------------------------------------------------
void XWindowGeometry::SetMinMaxSize(const XSize& newMin, const XSize& newMax, bool redraw)
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
// XWindowGeometry::DoSetSize
//
//---------------------------------------------------------------
void XWindowGeometry::DoSetSize(const XSize& inSize, bool redraw)
{
	XSize newSize;
	newSize.width  = MinMax(mMinSize.width, inSize.width, mMaxSize.width);
	newSize.height = MinMax(mMinSize.height, inSize.height, mMaxSize.height);

	if (newSize != this->GetSize()) {
		IWindowPtr window(this);
	
		XSize oldSize = this->GetSize();
		::SizeWindow(window->GetOSWindow(), numeric_cast<int16>(newSize.width), numeric_cast<int16>(newSize.height), redraw);
					
		XSize delta = newSize - oldSize;
		this->DoParentChangedSize(delta, redraw);
		
		ISizeChangedPtr changed(this);
		if (changed)
			changed->OnSizeChanged(oldSize, newSize);
	
		XStateBroadcaster::Instance()->Broadcast(kResizedWindow, XAnyValue(window.Get()));
	}
	
	POSTCONDITION(this->GetSize() == newSize);
}


//---------------------------------------------------------------
//
// XWindowGeometry::DoSetLocation
//
//---------------------------------------------------------------
void XWindowGeometry::DoSetLocation(const XPoint& newLoc, bool redraw)
{
	UNUSED(redraw);
	
	IWindowPtr window(this);
	
	::MoveWindow(window->GetOSWindow(), numeric_cast<int16>(newLoc.x), numeric_cast<int16>(newLoc.y), false);
			
	ILocationChangedPtr changed(this);
	if (changed)
		changed->OnLocationChanged(newLoc);
	
	XStateBroadcaster::Instance()->Broadcast(kMovedWindow, XAnyValue(window.Get()));
}


//---------------------------------------------------------------
//
// XWindowGeometry::DoSetMinMaxSize
//
//---------------------------------------------------------------
void XWindowGeometry::DoSetMinMaxSize(const XSize& newMin, const XSize& newMax, bool redraw)
{
	PRECONDITION(newMin.width > 0 && newMin.height > 0);
	PRECONDITION(newMax.width > 0 && newMax.height > 0);
	PRECONDITION(newMin.width <= newMax.width && newMin.height <= newMax.height);
	
	mMinSize = newMin;
	mMaxSize = newMax;
		
	// Min size must be smaller than max size.
	mMinSize.width  = Min(mMaxSize.width, mMinSize.width);
	mMinSize.height = Min(mMaxSize.height, mMinSize.height);
		
	// Max size must be larger than min size.
	mMaxSize.width  = Max(mMaxSize.width, mMinSize.width);
	mMaxSize.height = Max(mMaxSize.height, mMinSize.height);
	
	// Window size must also be in [min, max].
	XSize newSize = this->GetSize();
	
	newSize.width  = MinMax(mMinSize.width, newSize.width, mMaxSize.width);
	newSize.height = MinMax(mMinSize.height, newSize.height, mMaxSize.height);

	if (newSize != this->GetSize())
		this->DoSetSize(newSize, redraw);
}


//---------------------------------------------------------------
//
// XWindowGeometry::DoParentChangedSize
//
//---------------------------------------------------------------
void XWindowGeometry::DoParentChangedSize(const XSize& delta, bool redraw)
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
