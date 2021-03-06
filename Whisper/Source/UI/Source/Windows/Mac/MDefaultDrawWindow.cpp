/*
 *  File:       MDefaultDrawWindow.cpp
 *  Summary:   	IDraw implementation for windows.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MDefaultDrawWindow.cpp,v $
 *		Revision 1.5  2001/02/09 04:33:31  jesjones
 *		Renamed IChildNode methods.
 *		
 *		Revision 1.4  2001/01/22 02:01:55  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:27:28  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:11:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IChildNode.h>
#include <IDrawHelper.h>

#include <Appearance.h>

#include <IWindow.h>
#include <XShapes.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetActiveBrush
//
// $$$�The Appearance Manager defines brushes for modeless dialogs
// $$$ but there's no obvious way to determine when a regular
// $$$�window is being used as a modeless dialog. One possibility
// $$$�would be to override OnReanimated and treat the window as
// $$$�modeless if it's in the regular layer and contains XControl's.
// $$$ Another possibility is to extend Quill and let the user mark
// $$$�the window as a modeless dialog or to set the brush directly.
//
//---------------------------------------------------------------
static ThemeBrush GetActiveBrush(WindowClass windClass) 
{	
	ThemeBrush brush;
		
	switch (windClass) {
		case kDocumentWindowClass:
			brush = kThemeDocumentWindowBackgroundBrush;
			break;

		case kModalWindowClass:
		case kMovableModalWindowClass:
			brush = kThemeActiveDialogBackgroundBrush;
			break;

		case kAlertWindowClass:
		case kMovableAlertWindowClass:
			brush = kThemeActiveAlertBackgroundBrush;
			break;

		case kFloatingWindowClass:
			brush = kThemeActiveUtilityWindowBackgroundBrush;
			break;

		default:
			DEBUGSTR("Bad windClass in GetActiveBrush");
			brush = kThemeDocumentWindowBackgroundBrush;
	}
	
	return brush;

}


//---------------------------------------------------------------
//
// GetInactiveBrush
//
//---------------------------------------------------------------
static ThemeBrush GetInactiveBrush(WindowClass windClass) 
{	
	ThemeBrush brush;
		
	switch (windClass) {
		case kDocumentWindowClass:
			brush = kThemeDocumentWindowBackgroundBrush;
			break;

		case kModalWindowClass:
		case kMovableModalWindowClass:
			brush = kThemeInactiveDialogBackgroundBrush;
			break;

		case kAlertWindowClass:
		case kMovableAlertWindowClass:
			brush = kThemeInactiveAlertBackgroundBrush;
			break;

		case kFloatingWindowClass:
			brush = kThemeInactiveUtilityWindowBackgroundBrush;
			break;

		default:
			DEBUGSTR("Bad windClass in GetActiveBrush");
			brush = kThemeDocumentWindowBackgroundBrush;
	}
	
	return brush;

}

#pragma mark -

// ===================================================================================
//	class XDefaultDrawWindow
//!		IDraw implementation for windows.
// ===================================================================================
class XDefaultDrawWindow : public IDrawHelper, public IChildNode {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDefaultDrawWindow();
	
						XDefaultDrawWindow(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		OnAdopted();
	virtual void 		OnOrphaned();

protected:
	virtual void 		OnDraw(XDrawContext& canvas, const XRegion& dirtyRgn, bool isActive);

//-----------------------------------
//	Member Data
//
private:
	ThemeBrush 		mActiveBrush;		// Appearance Manager window background brushes
	ThemeBrush 		mInactiveBrush;		
};

DEFINE_INTERFACE_FACTORY(XDefaultDrawWindow)

//---------------------------------------------------------------
//
// XDefaultDrawWindow::~XDefaultDrawWindow
//
//---------------------------------------------------------------
XDefaultDrawWindow::~XDefaultDrawWindow()
{
}


//---------------------------------------------------------------
//
// XDefaultDrawWindow::XDefaultDrawWindow
//
//---------------------------------------------------------------
XDefaultDrawWindow::XDefaultDrawWindow(XBoss* boss)
{	
	IDrawHelper::DoSetBoss(boss);
	IChildNode::DoSetBoss(boss);
	
	mActiveBrush   = 0;
	mInactiveBrush = 0;
}


//---------------------------------------------------------------
//
// XDefaultDrawWindow::OnAdopted
//
//---------------------------------------------------------------
void XDefaultDrawWindow::OnAdopted()
{
	IDrawHelper* thisPtr = this;
	IWindowPtr window(thisPtr);
	const SWindowStyle& style = window->GetStyle();

	mActiveBrush   = Whisper::GetActiveBrush(style.windClass);	// can't do this in the ctor because the window may not be inited
	mInactiveBrush = Whisper::GetInactiveBrush(style.windClass);
}


//---------------------------------------------------------------
//
// XDefaultDrawWindow::OnOrphaned
//
//---------------------------------------------------------------
void XDefaultDrawWindow::OnOrphaned()
{
	// nothing to do here
}


//---------------------------------------------------------------
//
// XDefaultDrawWindow::OnDraw
//
//---------------------------------------------------------------
void XDefaultDrawWindow::OnDraw(XDrawContext& context, const XRegion& dirtyRgn, bool isActive)
{
	IDrawHelper* thisPtr = this;
	IWindowPtr window(thisPtr);
	
	OSStatus err = noErr;
	if (isActive)
		err = ::SetThemeWindowBackground(window->GetOSWindow(), mActiveBrush, kDontRedraw);
	else
		err = ::SetThemeWindowBackground(window->GetOSWindow(), mInactiveBrush, kDontRedraw);
	ASSERT(err == noErr);		// app will go crazy if we throw here...

	context.MakeCurrent();
	::EraseRgn(dirtyRgn);
}


}	// namespace Whisper
