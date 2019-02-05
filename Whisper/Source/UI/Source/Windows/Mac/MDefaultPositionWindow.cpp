/*
 *  File:       MDefaultPositionWindow.cpp
 *  Summary:   	IPositionWindow implementation that knows how to center and stagger windows.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MDefaultPositionWindow.cpp,v $
 *		Revision 1.4  2001/02/07 04:56:04  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:27:33  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:11:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IDefaultPositionWindow.h>

#include <IGeometry.h>
#include <IWindow.h>
#include <IPositionWindow.h>
#include <IWindowMgr.h>
#include <MSystemInfo.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetPositionMethod
//
//---------------------------------------------------------------
#if UNIVERSAL_INTERFACES_VERSION >= 0x0320 
static WindowPositionMethod GetPositionMethod(const IWindowPtr& parentWind, EAutoPosition pos)
{	
	WindowPositionMethod method = 0;
	
	switch (pos) {
		case kCenterOnMainMonitor:
			method = kWindowCenterOnMainScreen;
			break;

		case kAlertPositionOnMainMonitor:
			method = kWindowAlertPositionOnMainScreen;
			break;

		case kStaggerOnMainMonitor:
			method = kWindowCascadeOnMainScreen;
			break;

		case kCenterOnTopWindow:
			if (parentWind != nil)
				method = kWindowCenterOnParentWindow;
			else
				method = kWindowCenterOnMainScreen;		// RepositionWindow isn't smart enough to do this for us...
			break;

		case kAlertPositionOnTopWindow:
			if (parentWind != nil)
				method = kWindowAlertPositionOnParentWindow;
			else
				method = kWindowAlertPositionOnMainScreen;
			break;

		case kStaggerOnTopWindow:
			if (parentWind != nil)
				method = kWindowCascadeOnParentWindow;
			else
				method = kWindowCascadeOnMainScreen;
			break;

		case kCenterOnParentMonitor:
			if (parentWind != nil)
				method = kWindowCenterOnParentWindowScreen;
			else
				method = kWindowCenterOnMainScreen;
			break;

		case kAlertPositionOnParentMonitor:
			if (parentWind != nil)
				method = kWindowAlertPositionOnParentWindowScreen;
			else
				method = kWindowAlertPositionOnMainScreen;
			break;

		case kStaggerOnParentMonitor:
			if (parentWind != nil)
				method = kWindowCascadeOnParentWindow;
			else
				method = kWindowCascadeOnMainScreen;
			break;
			
		default:
			DEBUGSTR("Bad pos ", (int32) pos, " in GetPositionMethod");
	}

	return method;
}
#endif		// UNIVERSAL_INTERFACES_VERSION >= 0x0320

#pragma mark -

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
//	Member Data
//
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
}


//---------------------------------------------------------------
//
// XDefaultPositionWindow::Position
//
//---------------------------------------------------------------
void XDefaultPositionWindow::Position()
{
	if (mWhere != kDontAutoPosition && MSystemInfo::HasWindowMgr2()) {
		IPositionWindow* thisPtr = this;
		IWindowPtr window(thisPtr);
		IGeometryPtr geometry(thisPtr);
		OSWindow wptr = window->GetOSWindow();
		
		ASSERT(!window->IsVisible());			// so GetTopRegular works

		// Get our parent window
		IWindowMgrPtr mgr(L"Application");	
		IWindowPtr parentWind = mgr->GetTopModal();
		
		if (!parentWind) 
			parentWind = mgr->GetTopRegular();
		
		// Save the current window bounds
		Rect oldBounds;
		OSStatus err = ::GetWindowBounds(wptr, kWindowContentRgn, &oldBounds);
		ThrowIfOSErr(err);
		
		// Use the toolbox to position the window
		WindowPositionMethod method = Whisper::GetPositionMethod(parentWind, mWhere);
		WindowRef parentPtr = parentWind ? parentWind->GetOSWindow() : nil;
		err = ::RepositionWindow(wptr, parentPtr, method);
		ThrowIfOSErr(err);
		
		// Get the new window bounds
		Rect newBounds;
		err = ::GetWindowBounds(wptr, kWindowContentRgn, &newBounds);
		ThrowIfOSErr(err);
		
		// Restore the old window bounds
		err = ::SetWindowBounds(wptr, kWindowContentRgn, &oldBounds);
		ThrowIfOSErr(err);
		
		// Use the IGeometry interface to update the window bounds
		// (so that our internal state information is updated).
		geometry->SetFrame(newBounds);
	}
}


}	// namespace Whisper
