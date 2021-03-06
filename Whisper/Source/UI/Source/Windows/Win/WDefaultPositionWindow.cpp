/*
 *  File:       WDefaultPositionWindow.cpp
 *  Summary:   	IPositionWindow that knows how to center and stagger windows.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WDefaultPositionWindow.cpp,v $
 *		Revision 1.6  2001/02/07 04:57:30  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.5  2000/12/27 01:06:29  jesjones
 *		Fixed DoMakeVisible. Tweaked for dialogs.
 *		
 *		Revision 1.4  2000/12/14 08:30:38  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/08 08:20:40  jesjones
 *		Improved for MDI apps. Added code to make the window visible when positioning off the parent window.
 *		
 *		Revision 1.2  2000/11/09 12:06:13  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IDefaultPositionWindow.h>

#include <IDialogBox.h>
#include <IFrameWindow.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IPositionWindow.h>
#include <IWindow.h>
#include <XDesktop.h>
#include <XDisplay.h>

namespace Whisper {


// ===================================================================================
//	class XDefaultPositionWindow
//!		IPositionWindow implementation that knows how to center and stagger windows.
// ===================================================================================
class XDefaultPositionWindow : public IPositionWindow, public IDefaultPositionWindow {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XDefaultPositionWindow();
	
						XDefaultPositionWindow(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Position();

	virtual void 		Init(EAutoPosition where)				{mWhere = where;}
	
//-----------------------------------
//	Internal API
//
protected:
#if WIN
	virtual void 		DoCenterWindow(const XRect& bounds);
	virtual void 		DoAlertWindow(const XRect& bounds);
	virtual void 		DoStaggerWindow(const XRect& bounds);
	virtual void 		DoMakeVisible(IWindowPtr& window, const XRect& deviceRect);
#endif

//-----------------------------------
//	Member Data
//
protected:
#if WIN
	XPoint			mStaggerBy;	
#endif

private:
	EAutoPosition	mWhere;
};

DEFINE_INTERFACE_FACTORY(XDefaultPositionWindow)

//---------------------------------------------------------------
//
// XDefaultPositionWindow::~XDefaultPositionWindow
//
//---------------------------------------------------------------
XDefaultPositionWindow::~XDefaultPositionWindow()
{
}


//---------------------------------------------------------------
//
// XDefaultPositionWindow::XDefaultPositionWindow
//
//---------------------------------------------------------------
XDefaultPositionWindow::XDefaultPositionWindow(XBoss* boss)
{	
	IPositionWindow::DoSetBoss(boss);
	IDefaultPositionWindow::DoSetBoss(boss);

	mStaggerBy = XPoint(4, 4);
}


//---------------------------------------------------------------
//
// XDefaultPositionWindow::Position
//
//---------------------------------------------------------------
void XDefaultPositionWindow::Position()
{
	if (mWhere != kDontAutoPosition) {
		IPositionWindow* thisPtr = this;
		IWindowPtr window(thisPtr);
		
		ASSERT(!window->IsVisible());			// so GetTopRegular works

		IWindowMgrPtr mgr(L"Application");
		IWindowPtr topWind = mgr->GetTopRegular();

		// Get the bounds of the main device.
		XDisplay mainDevice = XDesktop::GetMainDisplay();
		XRect mainRect = mainDevice.GetClientArea();

		// Get the bounds of the device the window is on.
		IGeometryPtr geometry(thisPtr);
		XDisplay maxDevice = XDesktop::GetMaxDisplay(geometry->GetFrame());
		XRect windDeviceRect = maxDevice.GetClientArea();
		
		// Get the bounds of the top window and the top window's
		// device.
		XRect topWindowRect = kZeroRect;
		XRect topDeviceRect = kZeroRect;

		if (topWind) {
			IGeometryPtr topGeom(topWind);
			if (IDialogBoxPtr(window)) {				// dialogs are in global coordinates, but MDI doc windows are relative to the frame window
				topWindowRect = topGeom->GetExtent();
				topWindowRect = topWind->WindowToGlobal(topWindowRect);
			} else
				topWindowRect = topGeom->GetFrame();
					
			maxDevice = XDesktop::GetMaxDisplay(topWindowRect);
			topDeviceRect = maxDevice.GetClientArea();
		}
		
		// If the window is a child of an MDI window then we'll set
		// the device rects to the MDI window's frame and convert
		// them to the frame window's coordinate system (because
		// window positions are relative to their parent).
		IFrameWindowPtr frameWind = GetParent<IFrameWindow>(window);
		if (frameWind && !IDialogBoxPtr(window)) {
			IGeometryPtr frameGeom(frameWind);
			mainRect = windDeviceRect = frameGeom->GetExtent();
			
			if (topDeviceRect != kZeroRect)
				topDeviceRect = mainRect;
		}
		
		// Adjust the device rects so that the window will be visible
		// if it's placed at the topLeft of the device rect.
		XPoint inset = window->GetClientAreaInset()[topLeft];

		mainRect.left += inset.x;
		mainRect.top  += inset.y;

		windDeviceRect.left += inset.x;
		windDeviceRect.top  += inset.y;

		topWindowRect.left += inset.x;
		topWindowRect.top  += inset.y;

		// Auto-position the window.
		switch (mWhere) {
			case kCenterOnMainMonitor:
				this->DoCenterWindow(mainRect);
				break;
				
			case kAlertPositionOnMainMonitor:
				this->DoAlertWindow(mainRect);
				break;
				
			case kStaggerOnMainMonitor:
				this->DoStaggerWindow(mainRect);
				break;
				
				
			case kCenterOnTopWindow:
				if (topWindowRect != kZeroRect)
					this->DoCenterWindow(topWindowRect);
				else
					this->DoCenterWindow(windDeviceRect);
				this->DoMakeVisible(window, windDeviceRect);
				break;
				
			case kAlertPositionOnTopWindow:
				if (topWindowRect != kZeroRect)
					this->DoAlertWindow(topWindowRect);
				else
					this->DoAlertWindow(windDeviceRect);
				this->DoMakeVisible(window, windDeviceRect);
				break;
				
			case kStaggerOnTopWindow:
				if (topWindowRect != kZeroRect)
					this->DoStaggerWindow(topWindowRect);
				else
					geometry->SetLocation(windDeviceRect[topLeft] + mStaggerBy/3);
				this->DoMakeVisible(window, windDeviceRect);
				break;
				
				
			case kCenterOnParentMonitor:
				if (topDeviceRect != kZeroRect)
					this->DoCenterWindow(topDeviceRect);
				else
					this->DoCenterWindow(windDeviceRect);
				break;
				
			case kAlertPositionOnParentMonitor:
				if (topDeviceRect != kZeroRect)
					this->DoAlertWindow(topDeviceRect);
				else
					this->DoAlertWindow(windDeviceRect);
				break;
				
			case kStaggerOnParentMonitor:
				if (topDeviceRect != kZeroRect)
					this->DoStaggerWindow(topDeviceRect);
				else
					this->DoStaggerWindow(windDeviceRect);
				break;
				
			default:
				DEBUGSTR("Got a bad position in XDefaultPositionWindow::AutoPosition");
		}
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XDefaultPositionWindow::DoMakeVisible
//
// Our caller will call ForceOnScreen but all that will do is
// ensure that a bit of the window is visible so that the user
// can drag it out. However we want to do better than that when
// we do things like center large dialogs on their parent window.
//
//---------------------------------------------------------------
void XDefaultPositionWindow::DoMakeVisible(IWindowPtr& window, const XRect& deviceRect)
{
	IGeometryPtr geometry(window);
	XRect windRect = geometry->GetFrame();
	
	XPoint delta = kZeroPt;
	if (windRect.left < deviceRect.left)
		delta.x = deviceRect.left - windRect.left;
	else if (windRect.right > deviceRect.right)
		delta.x = -(windRect.right - deviceRect.right);
	
	if (windRect.top < deviceRect.top)
		delta.y = deviceRect.top - windRect.top;
	else if (windRect.bottom > deviceRect.bottom)
		delta.y = -(windRect.bottom - deviceRect.bottom);
	
	if (delta != kZeroPt) 
		geometry->SetFrame(windRect + delta);
}


//---------------------------------------------------------------
//
// XDefaultPositionWindow::DoCenterWindow
//
//---------------------------------------------------------------
void XDefaultPositionWindow::DoCenterWindow(const XRect& bounds)
{	
	IPositionWindow* thisPtr = this;
	IGeometryPtr geometry(thisPtr);

	XPoint loc;
	loc.x = bounds.left + (bounds.GetWidth() - geometry->GetWidth())/2;
	loc.y = bounds.top + (bounds.GetHeight() - geometry->GetHeight())/2;
	
	geometry->SetLocation(loc);
}


//---------------------------------------------------------------
//
// XDefaultPositionWindow::DoAlertWindow
//
//---------------------------------------------------------------
void XDefaultPositionWindow::DoAlertWindow(const XRect& bounds)
{	
	IPositionWindow* thisPtr = this;
	IGeometryPtr geometry(thisPtr);

	XPoint loc;
	loc.x = bounds.left + (bounds.GetWidth() - geometry->GetWidth())/2;
	loc.y = bounds.top + bounds.GetHeight()/5;
	
	geometry->SetLocation(loc);
}


//---------------------------------------------------------------
//
// XDefaultPositionWindow::DoStaggerWindow
//
//---------------------------------------------------------------
void XDefaultPositionWindow::DoStaggerWindow(const XRect& bounds)
{
	XPoint loc = bounds[topLeft];
	loc += mStaggerBy;	

	IPositionWindow* thisPtr = this;
	IGeometryPtr geometry(thisPtr);
	geometry->SetLocation(loc);
}


}	// namespace Whisper
