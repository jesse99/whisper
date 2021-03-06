/*
 *  File:       WFrameWindow.cpp
 *  Summary:   	The topmost window of an MDI app.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: WFrameWindow.cpp,v $
 *		Revision 1.9  2001/04/27 09:18:55  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.8  2001/04/21 03:32:05  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.7  2001/02/25 08:25:19  jesjones
 *		Minor tweak to DoResizeClientWindow.
 *		
 *		Revision 1.6  2001/02/24 09:41:25  jesjones
 *		Special cases WM_CLOSE.
 *		
 *		Revision 1.5  2001/02/21 08:16:42  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.4  2000/12/14 08:30:47  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/10 04:06:51  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/27 03:36:12  jesjones
 *		Uses GetInterface to query for the menu handlers on the focused control/window.
 *		
 *		Revision 1.1  2000/11/20 00:57:13  jesjones
 *		WFrameWindow descends from XWindow instead of IMsgExtensible.		
 */

#include <XWhisperHeader.h>
#include <IFrameWindow.h>

#include <commctrl.h>
#include <windowsx.h>

#include <IActivated.h>
#include <IActiveMenuBar.h>
#include <IApplication.h>
#include <IControl.h>
#include <IHierarchy.h>
#include <IMenuBar.h>
#include <IMenuHandler.h>
#include <IOpened.h>
#include <ISizeChanged.h>
#include <IWindow.h>
#include <WSystemInfo.h>
#include <XEvent.h>
#include <XExceptions.h>
#include <XNumbers.h>

namespace Whisper {


// ===================================================================================
//	class WFrameWindow
//!		The topmost window of an MDI app.
// ===================================================================================
class WFrameWindow : public IFrameWindow, public IActivated, public ISizeChanged {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~WFrameWindow();
	
  						WFrameWindow(XBoss* boss);
  						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init();
			
	virtual	void 		SetStatusMesg(const std::wstring& mesg, uint32 part = 0);
	virtual	void 		SetNumParts(const int32* begin, const int32* end);
			
	virtual	HWND 		GetClientWindow() const						{return mClientWindow;}
	virtual HWND 		GetStatusWindow() const						{return mStatusWindow;}

protected:										
	virtual void 		OnActivated();
	virtual void 		OnDeactivated();
	
	virtual void 		OnOpened();
	virtual void 		OnClosing();

	virtual void 		OnSizeChanged(const XSize& oldSize, const XSize& newSize);

//-----------------------------------
//	Internal API
//
private:
	static	LRESULT WINAPI DoWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
			HWND 		DoCreateClientWindow();
			HWND 		DoCreateStatusWindow();

			void 		DoResizeClientWindow(bool minimized, bool redraw);

//-----------------------------------
//	Member Data
//
private:
	HWND					mClientWindow;
	HWND					mStatusWindow;
	std::wstring			mName;
	
	static WFrameWindow*	msInstance;
};

DEFINE_INTERFACE_FACTORY(WFrameWindow)
WFrameWindow* WFrameWindow::msInstance = nil;

//---------------------------------------------------------------
//
// WFrameWindow::~WFrameWindow
//
//---------------------------------------------------------------
WFrameWindow::~WFrameWindow()
{
	msInstance = nil;
}


//---------------------------------------------------------------
//
// WFrameWindow::WFrameWindow
//
//---------------------------------------------------------------
WFrameWindow::WFrameWindow(XBoss* boss) 
{	
	IFrameWindow::DoSetBoss(boss);
	IActivated::DoSetBoss(boss);
	ISizeChanged::DoSetBoss(boss);

	mStatusWindow = nil;
	mClientWindow = nil;
	
	msInstance = this;
}


//---------------------------------------------------------------
//
// WFrameWindow::Init
//
//---------------------------------------------------------------
void WFrameWindow::Init()
{
	PRECONDITION(mClientWindow == nil);
	PRECONDITION(mStatusWindow == nil);	
	
	IFrameWindow* thisPtr = this;
	IWindowPtr window(thisPtr);

	try {		
		window->SetWindowProc(DoWindowProc);

		mClientWindow = this->DoCreateClientWindow();
		mStatusWindow = this->DoCreateStatusWindow();
			
		this->DoResizeClientWindow(false, kDontRedraw);

		HWND wptr = window->GetOSWindow();
		::SetClassLong(wptr, GCL_HCURSOR, (int32) LoadCursor(nil, IDC_ARROW));
			
	} catch (...) {
		window->Close(false);
		
		throw;
	}
}


//---------------------------------------------------------------
//
// WFrameWindow::SetStatusMesg
//
//---------------------------------------------------------------
void WFrameWindow::SetStatusMesg(const std::wstring& mesg, uint32 part)
{
	PRECONDITION(part < 256);						// sanity check

	if (mStatusWindow != nil) {	
//		if (WSystemInfo::HasUnicode())
//			VERIFY(::SendMessageW(mStatusWindow, SB_SETTEXT, part, (int32) mesg.c_str()));	
//		else
			VERIFY(::SendMessageA(mStatusWindow, SB_SETTEXT, part, (int32) ToPlatformStr(mesg).c_str()));	// $$$ status bar isn't working with Unicode on NT 4
	}
}


//---------------------------------------------------------------
//
// WFrameWindow::SetNumParts
//
//---------------------------------------------------------------
void WFrameWindow::SetNumParts(const int32* begin, const int32* end)
{
	PRECONDITION(begin != nil);
	PRECONDITION(end >= begin);

	if (mStatusWindow != nil) {
		uint32 count = numeric_cast<uint32>(end - begin);
		PRECONDITION(count < 256);				// sanity check

		VERIFY(::SendMessage(mStatusWindow, SB_SETPARTS, count, (int32) begin));
	}
}
	
#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// WFrameWindow::OnActivated
//
//---------------------------------------------------------------
void WFrameWindow::OnActivated()
{
	IFrameWindow* thisPtr = this;

	IActiveMenuBarPtr active(L"Application");
	active->Set(thisPtr);
}


//---------------------------------------------------------------
//
// WFrameWindow::OnDeactivated
//
//---------------------------------------------------------------
void WFrameWindow::OnDeactivated()
{
	// nothing to do
}


	
//---------------------------------------------------------------
//
// WFrameWindow::OnOpened
//
//---------------------------------------------------------------
void WFrameWindow::OnOpened()
{
	// nothing to do
}


//---------------------------------------------------------------
//
// WFrameWindow::OnClosing
//
//---------------------------------------------------------------
void WFrameWindow::OnClosing()
{
	IApplicationPtr app(L"Application");
	app->Quit();
}


//---------------------------------------------------------------
//
// WFrameWindow::OnSizeChanged
//
//---------------------------------------------------------------
void WFrameWindow::OnSizeChanged(const XSize& oldSize, const XSize& newSize)
{
	UNUSED(oldSize);
	UNUSED(newSize);
	
	IFrameWindow* thisPtr = this;
	IWindowPtr window(thisPtr);
	HWND wptr = window->GetOSWindow();

	WINDOWPLACEMENT placement;
	placement.length = sizeof(WINDOWPLACEMENT);
	
	int32 succeeded = ::GetWindowPlacement(wptr, &placement);
	ThrowIf(!succeeded);
	
	bool minimized = placement.showCmd == SW_MINIMIZE;
	this->DoResizeClientWindow(minimized, kRedraw);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// WFrameWindow::DoWindowProc							[static]
//
//---------------------------------------------------------------
LRESULT WINAPI WFrameWindow::DoWindowProc(HWND windH, UINT message, WPARAM wParam, LPARAM lParam)
{
	PRECONDITION(msInstance != nil);
	
	int32 result = 0;
	
	if (message == WM_CLOSE) {
		IApplicationPtr app(L"Application");
		app->Quit();

	} else
		result = ::DefFrameProc(windH, msInstance->mClientWindow, message, wParam, lParam);
			
	return result;
}


//---------------------------------------------------------------
//
// WFrameWindow::DoCreateClientWindow
//
//---------------------------------------------------------------
HWND WFrameWindow::DoCreateClientWindow()
{
	HWND window = nil;
	
	uint32 style = WS_BORDER + WS_CHILD + WS_CLIPCHILDREN + WS_VISIBLE + MDIS_ALLCHILDSTYLES;
	
	CLIENTCREATESTRUCT clientInfo;
	clientInfo.hWindowMenu  = nil;
	clientInfo.idFirstChild = 256;

	IFrameWindow* thisPtr = this;
	HWND wptr = IWindowPtr(thisPtr)->GetOSWindow();

	if (WSystemInfo::HasUnicode())
		window = ::CreateWindowW(L"MDICLIENT",		// class name
								 L"Client Window",	// window name
								 style,				// style
								 0,					// location
								 0,
								 0,					// size
								 0,
								 wptr,				// parent window
								 (HMENU) 199,		// child ID
							     WSystemInfo::GetAppInstance(),	// app instance
								 &clientInfo);		// client window info
	else	
		window = ::CreateWindowA("MDICLIENT",		// class name
								 "Client Window",	// window name
								 style,				// style
								 0,					// location
								 0,
								 0,					// size
								 0,
								 wptr,				// parent window
								 (HMENU) 199,		// child ID
							     WSystemInfo::GetAppInstance(),	// app instance
								 &clientInfo);		// client window info
	ThrowIfNil(window);
	
	return window;
}


//---------------------------------------------------------------
//
// WFrameWindow::DoCreateStatusWindow
//
//---------------------------------------------------------------
HWND WFrameWindow::DoCreateStatusWindow()
{
	HWND window = nil;
		
	IFrameWindow* thisPtr = this;
	HWND wptr = IWindowPtr(thisPtr)->GetOSWindow();

	int32 style = SS_LEFT + WS_CHILD + WS_VISIBLE + SBARS_SIZEGRIP;
	if (WSystemInfo::HasUnicode())
		window = ::CreateStatusWindowW(style,			// style
									   L"",				// text
								       wptr,			// parent window
								       nil);			// child ID
	else	
		window = ::CreateStatusWindowA(style,			// style
								       "",				// text
								       wptr,			// parent window
								       nil);			// child ID
	ThrowIfNil(window);

	if (WSystemInfo::HasUnicode())
		(void) ::SendMessageW(window, SB_SETUNICODEFORMAT, false, 0);	// $$$ SB_SETTEXT isn't working with Unicode so we'll disable Unicode (even on Win 2000)

	return window;	
}


//---------------------------------------------------------------
//
// WFrameWindow::DoResizeClientWindow
//
//---------------------------------------------------------------
void WFrameWindow::DoResizeClientWindow(bool minimized, bool redraw)
{
	if (mClientWindow != nil && !minimized) {
		IFrameWindow* thisPtr = this;
		IWindowPtr window(thisPtr);
		HWND wptr = window->GetOSWindow();

		XRect area;
		int32 succeeded = ::GetClientRect(wptr, area);
		ThrowIf(!succeeded);
				
		if (mStatusWindow != nil) {
			XRect statusArea;
			succeeded = ::GetClientRect(mStatusWindow, statusArea);
			ThrowIf(!succeeded);

			area.bottom -= statusArea.GetHeight();
		}

		::MoveWindow(mClientWindow, area.left, area.top, area.GetWidth(), area.GetHeight(), redraw);
	}
}


}	// namespace Whisper
