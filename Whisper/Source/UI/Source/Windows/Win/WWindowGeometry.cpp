/*
 *  File:       WWindowGeometry.cpp
 *  Summary:   	IGeometry that works with windows.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WWindowGeometry.cpp,v $
 *		Revision 1.10  2001/04/21 03:34:40  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.9  2001/02/24 09:44:53  jesjones
 *		WWindow calls ISizeChanged now.
 *		
 *		Revision 1.8  2001/02/21 08:17:19  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.7  2000/12/31 04:45:16  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.6  2000/12/27 01:10:13  jesjones
 *		Added some ugly code to GetLocation for dialogs.
 *		
 *		Revision 1.5  2000/12/14 08:32:23  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.4  2000/12/08 08:17:18  jesjones
 *		Uses interface pointer typedefs.
 *		
 *		Revision 1.3  2000/11/27 03:38:00  jesjones
 *		Deleted an unused local variable.
 *		
 *		Revision 1.2  2000/11/09 12:13:07  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	  8/04/00	JDJ		OnParentChangedSize queries the child instead of 'this'
 *									for the IParentChangedSize interface.
 *		 <1>	  6/29/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IGeometry.h>

#include <IDialogBox.h>
#include <IHierarchy.h>
#include <IMenuBar.h>
#include <IParentChangedSize.h>
#include <ISizeChanged.h>
#include <IWindow.h>
#include <XDesktop.h>
#include <XNumbers.h>
#include <XExceptions.h>
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
			void 		DoSetLocation(const XPoint& newLoc);
			void 		DoSetMinMaxSize(const XSize& newMin, const XSize& newMax, bool redraw);
							
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
	XPoint loc = kZeroPt;
	
	IConstWindowPtr window(this);
	HWND windH = window->GetOSWindow();
	
	HWND parentH = ::GetParent(windH);
	if (parentH == nil)
		ThrowIf(ClientToScreen(windH, loc) == 0);

	else {
		XRect frame;
		int32 succeeded = ::GetClientRect(windH, frame);
		ThrowIf(!succeeded);
		
		loc = frame[topLeft];
		ThrowIf(ClientToScreen(windH, loc) == 0);
		ThrowIf(ScreenToClient(parentH, loc) == 0);

		XPoint offset = kZeroPt;
		IConstDialogBoxPtr dialog(window);
		if (dialog) {
			succeeded = ::GetClientRect(parentH, frame);
			ThrowIf(!succeeded);
			
			offset = frame[topLeft];
			ThrowIf(ClientToScreen(parentH, offset) == 0);
		}
		
		loc += offset;
	}
	
	return loc;
}


//---------------------------------------------------------------
//
// XWindowGeometry::SetLocation
//
//---------------------------------------------------------------
void XWindowGeometry::SetLocation(const XPoint& newLoc, bool redraw)
{
	UNUSED(redraw);
	
	if (newLoc != this->GetLocation())
		this->DoSetLocation(newLoc);
}

	
//---------------------------------------------------------------
//
// XWindowGeometry::GetWidth
//
//---------------------------------------------------------------
int32 XWindowGeometry::GetWidth() const
{
	IConstWindowPtr window(this);
	HWND windH = window->GetOSWindow();
	
	XRect extent;
	VERIFY(::GetClientRect(windH, extent));
	
	int32 width = extent.GetWidth();
	
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
	HWND windH = window->GetOSWindow();
	
	XRect extent;
	VERIFY(::GetClientRect(windH, extent));
	
	int32 height = extent.GetHeight();
	
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
	HWND windH = window->GetOSWindow();
	
	XRect extent;
	VERIFY(::GetClientRect(windH, extent));
	
	XSize size = extent.GetSize();
	
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
	
	IConstWindowPtr window(this);
	XPoint offset = window->WindowToGlobal(kZeroPt);
	extent -= offset;
	
	return extent;
}


//---------------------------------------------------------------
//
// XWindowGeometry::SetFrame
//
//---------------------------------------------------------------
void XWindowGeometry::SetFrame(const XRect& newFrame, bool redraw)
{
	this->SetLocation(newFrame[topLeft]);
	this->SetSize(newFrame.GetSize(), redraw);
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
// XWindowGeometry::SetMinSize
//
//---------------------------------------------------------------
void XWindowGeometry::SetMinSize(const XSize& newMin, bool redraw)
{
	if (newMin != mMinSize)
		this->DoSetMinMaxSize(newMin, mMaxSize, redraw);
}


//---------------------------------------------------------------
//
// XWindowGeometry::SetMaxSize
//
//---------------------------------------------------------------
void XWindowGeometry::SetMaxSize(const XSize& newMax, bool redraw)
{
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
	IWindowPtr window(this);
	HWND windH = window->GetOSWindow();
	
	XSize oldSize = this->GetSize();

	XSize newSize;
	newSize.width  = MinMax(mMinSize.width, inSize.width, mMaxSize.width);
	newSize.height = MinMax(mMinSize.height, inSize.height, mMaxSize.height);
	
	XRect inset = window->GetClientAreaInset();

	XSize ncSize = newSize;
	ncSize.width  += inset.left + inset.right;
	ncSize.height += inset.top + inset.bottom;
	
	uint32 flags = SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOZORDER;
	if (!redraw)
		flags += SWP_NOREDRAW;
		
	int32 succeeded = ::SetWindowPos(windH, nil, 0, 0, ncSize.width, ncSize.height, flags);
	ThrowIf(!succeeded);
	
	XStateBroadcaster::Instance()->Broadcast(kResizedWindow, XAnyValue(window.Get()));
	
	POSTCONDITION(this->GetSize() == newSize);
}


//---------------------------------------------------------------
//
// XWindowGeometry::DoSetLocation
//
//---------------------------------------------------------------
void XWindowGeometry::DoSetLocation(const XPoint& newLoc)
{
	IWindowPtr window(this);
	HWND windH = window->GetOSWindow();
	
	XPoint loc = newLoc - window->GetClientAreaInset()[topLeft];
	
	uint32 flags = SWP_NOACTIVATE + SWP_NOSIZE + SWP_NOZORDER;
	int32 succeeded = ::SetWindowPos(windH, nil, loc.x, loc.y, 0, 0, flags);
	ThrowIf(!succeeded);
	
	XStateBroadcaster::Instance()->Broadcast(kMovedWindow, XAnyValue(window.Get()));

	XPoint loc2 = this->GetLocation();
	POSTCONDITION(this->GetLocation() == newLoc);		// set in wnd proc
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


}	// namespace Whisper
