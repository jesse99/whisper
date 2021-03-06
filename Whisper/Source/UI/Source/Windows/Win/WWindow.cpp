/*
 *  File:       WBaseWindow.cpp
 *  Summary:   	Base class for cross platform window objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WWindow.cpp,v $
 *		Revision 1.29  2001/08/30 01:13:09  jesjones
 *		Wrapped XDisableInvariant with #if DEBUG.
 *		
 *		Revision 1.28  2001/04/27 09:19:59  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.27  2001/04/21 03:34:32  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.26  2001/04/17 01:42:46  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.25  2001/03/17 12:38:19  jesjones
 *		Mouse move message always sets handled to false.
 *		
 *		Revision 1.24  2001/03/03 23:58:38  jesjones
 *		Added calls to DEBUGSTR in catch (...) blocks.
 *		
 *		Revision 1.23  2001/02/25 08:27:55  jesjones
 *		Fixed the parent window code in Init. Tweaked the WM_SIZE code.
 *		
 *		Revision 1.22  2001/02/24 09:44:24  jesjones
 *		Resizes the status window. Special cases WM_CLOSE for frame windows. Keeps track of the activation state so the debugger doesn't hose us.
 *		
 *		Revision 1.21  2001/02/23 10:11:46  jesjones
 *		Fixed IsActive.Fixed  HandleActivate and HandleDeactivate for MDI document windows.
 *		
 *		Revision 1.20  2001/02/21 08:17:10  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.19  2001/02/17 09:28:10  jesjones
 *		Removed UI_EXPORT.
 *		
 *		Revision 1.18  2001/02/08 03:54:06  jesjones
 *		Removed KCloseBoxCmd.
 *		
 *		Revision 1.17  2001/02/07 02:29:11  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.16  2001/01/30 07:22:48  jesjones
 *		Uses CallRepeatedMethod.
 *		
 *		Revision 1.15  2001/01/21 00:35:59  jesjones
 *		Did some work on the activate/deactivate code.
 *		
 *		Revision 1.14  2001/01/03 05:59:58  jesjones
 *		Tweak HandleMessage so that MDI child windows activate properly.
 *		
 *		Revision 1.13  2000/12/27 01:08:48  jesjones
 *		DoCreateWindow uses the parentH argument for non-MDI windows.
 *		
 *		Revision 1.12  2000/12/10 04:08:17  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.11  2000/12/08 08:18:42  jesjones
 *		DoCreateWindow defaults to creating larger windows and no longer makes alerts MDI child windows.
 *		
 *		Revision 1.10  2000/11/27 03:38:48  jesjones
 *		Calls IRestoreWindowPosition after creating the menubar.
 *		
 *		Revision 1.9  2000/11/26 03:35:13  jesjones
 *		Init uses GetInterface to find the frame window.
 *		
 *		Revision 1.8  2000/11/25 02:20:15  jesjones
 *		OnClose resets the active menubar instead of the dtor. Menu and key handlers use GetInterface instead of a direct query.
 *		
 *		Revision 1.7  2000/11/21 09:29:07  jesjones
 *		Introduced a temporary to avoid premature destruction.
 *		
 *		Revision 1.6  2000/11/20 05:50:03  jesjones
 *		Added a cheesy overload work around.
 *		
 *		Revision 1.5  2000/11/20 00:57:54  jesjones
 *		Added OnMessage.
 *		
 *		Revision 1.4  2000/11/17 08:50:01  jesjones
 *		Added get and set window proc methods. Added support for the new IRestoreWindowPosition interface. OnEnterMenuLoop, OnMenuSelect, and OnExitMenuLoop call the IShowHelp interface.
 *		
 *		Revision 1.3  2000/11/13 00:53:51  jesjones
 *		Uses Bind instead of Adapt.
 *		
 *		Revision 1.2  2000/11/09 12:12:57  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <15>	10/20/00	JDJ		Removed SWindowStyle::handleClicks.
 *		 <14>	10/18/00	JDJ		Replaced the AutoPosition method with a IPositionWindow interface.
 *		 <13>	 8/05/00	JDJ		Added AutoPosition and SendToBack (from IWindowMgr). Renamed
 *									SendToBack MoveToBack.
 *		 <12>	 8/01/99	JDJ		Added code to handle mnemonics
 *		 <11>	 7/05/99	JDJ		Dtor only calls CHECK_INVARIANT if mOpened is true.
 *		 <10>	 6/21/99	JDJ		Removed an ASSERT in WM_MDIACTIVATE.
 *		 <9>	 5/18/99	JDJ		MoveToFront uses BringWindowToTop instead of SetForegroundWindow.
 *		 <8>	 5/17/99	JDJ		Ctor now takes a parent window argument.
 *		 <7>	 5/17/99	JDJ		Added OnDefaultWindowProc. (Win)
 *		 <6>	 4/06/99	JDJ		Added HandleMenuSelect.
 *		 <5>	 3/12/99	JDJ		DoWindowProc uses XDeferredErrorCommand if an exception is caught.
 *		 <4>	 3/04/99	JDJ		New window classes are registered depending on SWindowStyle::classStyle
 *									and iconID.
 *		 <3>	 1/13/99	JDJ		Client area inset is recomputed when menu is added and when
 *									window changes size.
 *		 <2>	 1/10/99	JDJ		HandleOpen deletes the window if an exception is thrown.
 *		 <1>	10/12/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IWindow.h>

#include <tchar.h>
#include <winuser.h>

#include <IActivated.h>
#include <IActiveMenuBar.h>
#include <IActiveUndoContext.h>
#include <IApplication.h>
#include <ICanClose.h>
#include <ICommand.h>
#include <ICommands.h>
#include <ICommandQueue.h>
#include <IControl.h>
#include <IDraw.h>
#include <IFocus.h>
#include <IFocused.h>
#include <IFrameWindow.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IInit.h>
#include <IInitMenuBar.h>
#include <IKeyHandler.h>
#include <IMenuBar.h>
#include <IMenuHandler.h>
#include <IMouseHandler.h>
#include <IMsgExtensible.h>
#include <IOpened.h>
#include <IParentChangedSize.h>
#include <IPositionWindow.h>
#include <IRestoreWindowPosition.h>
#include <IRootControl.h>
#include <IShowHelp.h>
#include <ISizeChanged.h>
#include <IUndoContext.h>
#include <IWindowMgr.h>
#include <WSystemInfo.h>
#include <WUtils.h>
#include <XBind.h>
#include <XCursor.h>
#include <XDesktop.h>
#include <XDrawContext.h>
#include <XError.h>
#include <XExceptions.h>
#include <XIntConversions.h>
#include <XKeyEvents.h>
#include <XMiscUtils.h>
#include <XMouseEvents.h>
#include <XShapes.h>
#include <XStateBroadcaster.h>
#include <XStringUtils.h>
#include <XTinyVector.h>

#if(WINVER < 0x0500)
	typedef struct tagWINDOWINFO
	{
	    DWORD cbSize;
	    RECT  rcWindow;
	    RECT  rcClient;
	    DWORD dwStyle;
	    DWORD dwExStyle;
	    DWORD dwWindowStatus;
	    UINT  cxWindowBorders;
	    UINT  cyWindowBorders;
	    ATOM  atomWindowType;
	    WORD  wCreatorVersion;
	} WINDOWINFO, *PWINDOWINFO, *LPWINDOWINFO;

	#define WS_ACTIVECAPTION    0x0001

	extern "C"
	WINUSERAPI
	BOOL
	WINAPI
	GetWindowInfo(
	    IN HWND hwnd,
	    OUT PWINDOWINFO pwi
	);
#endif

namespace Whisper {


// ===================================================================================
//	class XWindow
// ===================================================================================
class XWindow : public IWindow {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XWindow();
	
						XWindow(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, const XSize& size, const SWindowStyle& style);

	virtual void 		Open();
	virtual bool 		IsOpened() const;
	virtual bool		Close(bool quitting, bool forceClose = false);						

	virtual	bool 		IsVisible() const;
	virtual void 		Show();	
	virtual void 		Hide();
	virtual void 		AnimatedShow(const XRect* startBounds);
	virtual void		AnimatedHide(const XRect* endBounds);
		
	virtual bool 		IsActive() const;
	virtual void 		HandleActivate();
	virtual void		HandleDeactivate();

	virtual void 		MoveToFront();
	virtual void 		MoveToBack();

	virtual std::wstring GetName() const;
	virtual void 		SetName(const std::wstring& name);
		
	virtual void 		Invalidate();
	virtual void 		Invalidate(const XRect& rect);
	virtual void 		Invalidate(const XRegion& rgn);

	virtual void 		Validate();
	virtual void 		Validate(const XRect& rect);
	virtual void 		Validate(const XRegion& rgn);
	
	virtual void 		HandleUpdate();
						
	virtual XPoint 		GlobalToWindow(const XPoint& pt) const;	
	virtual XRect 		GlobalToWindow(const XRect& rect) const;
	virtual XPoint 		WindowToGlobal(const XPoint& pt) const;	
	virtual XRect 		WindowToGlobal(const XRect& rect) const;
	
	virtual OSWindow 	GetOSWindow() const				{return mWindow;}
	virtual OSDrawContext GetOSDrawContext() const;
	virtual EWindowType GetType() const;
	virtual EWindowLayer GetLayer() const;
	virtual const SWindowStyle& GetStyle() const;
	virtual IRootControlPtr GetRootControl() const;
	static 	IWindowPtr GetWindow(OSWindow wptr);
	
	virtual void 		ForceOnScreen();
	
	virtual bool 		HandleAltChar(const XCharEvent& event);
	virtual bool 		HandleMessage(const MSG& msg, int32& result);
	virtual void		HandleMenuSelect(const IMenu::SHelp& help, uint16 flags);
	virtual void		DoComputeClientInset();
	virtual void 		SetWindowProc(WindowProc proc);
	virtual WindowProc 	GetWindowProc() 					{return mWindowProc;}
	
	virtual XRect 		GetClientAreaInset()				{return mClientAreaInset;}

protected:
	virtual void 		Invariant() const;

//-----------------------------------
//	Internal API
//
public:
	static	LRESULT CALLBACK DoWindowProc(HWND wptr, UINT message, WPARAM wParam, LPARAM lParam);

private:
			OSWindow 	DoCreateWindow(OSWindow parent, const SWindowStyle& style, const std::wstring& name, const XSize& size) const;
			HWND		DoCreateSizeGrip();

			bool 		DoCanClose(bool quitting) const;
			void		DoClose();

			void 		DoActivate(const IHierarchyPtr& node);
			void 		DoDeactivate(const IHierarchyPtr& node);
				
			void 		DoParentChangedSize(const XSize& delta);
			void 		DoGetLatentFocus(const IControlPtr& control);

			bool 		DoMessage(const MSG& msg, int32& result);
			void		DoMenuSelect(const std::wstring& help);

			bool 		DoMnemonic(HWND control, const std::wstring& mnemonic);
			bool 		DoMnemonic(HWND control);

	static	std::wstring DoGetWindowClass(uint32 style, uint16 iconID);
										
//-----------------------------------
//	Member Data
//
private:
	std::wstring	mName;						// this could be omitted, but having it makes it much easier to figure out which window is which when debugging...
	std::wstring	mID;						// used when streaming in
	
	OSWindow		mWindow;
	XRect 			mClientAreaInset;			// client area is at window rect + mClientAreaInset[topLeft]
	
	bool			mOpened;
	bool			mClosing;
	bool			mWasHidden;
	bool			mActive;
		
	mutable HDC		mDC;
	XSize			mSize;
	HWND			mSizeGrip;
	std::wstring	mLatentFocus;				// the control that will become the focus when the window is activated
	WindowProc		mWindowProc;

	SWindowStyle	mStyle;
};

DEFINE_INTERFACE_FACTORY(XWindow)

//---------------------------------------------------------------
//
// XWindow::~XWindow
//
//---------------------------------------------------------------
XWindow::~XWindow()
{
	PRECONDITION(!this->IsVisible());
	
	// The preferred way to delete a window is to use DoClose which
	// will set mClosing and post a command to delete the window.
	// This makes it possible for a button within a window to close
	// the window without having the window (and consequently the
	// button) deleted while we're executing a button method. However
	// if an exception is thrown while constructing a window the 
	// window will be deleted without going through DoClose and the
	// ASSERT below will fire. This is normal and perfectly OK.
	if (mOpened)
		PRECONDITION(mClosing);					
					
	if (mDC != nil) {
		ReleaseDC(mWindow, mDC);
		mDC = nil;
	}
		
	if (mWindow != nil) {
		(void) ::SetMenu(mWindow, nil);
	
		(void) ::SetWindowLong(mWindow, GWL_USERDATA, nil);	// we're no longer an XWindow
		VERIFY(::DestroyWindow(mWindow));
	}
}


//---------------------------------------------------------------
//
// XWindow::XWindow
//
//---------------------------------------------------------------
XWindow::XWindow(XBoss* boss)
{
	this->DoSetBoss(boss);

	mWindow = nil;
	mDC     = nil;

	mSizeGrip = nil;
	mOpened = false;
	mClosing  = false;
	mWasHidden = false;
	mActive = false;
	
	mClientAreaInset = kZeroRect;
	mWindowProc = DefWindowProc;
	
//	CALL_INVARIANT;			// wait for HandleOpen
}


//---------------------------------------------------------------
//
// XWindow::Init
//
//---------------------------------------------------------------
void XWindow::Init(const IHierarchyPtr& parent, const std::wstring& name, const XSize& size, const SWindowStyle& style)
{	
	{
#if DEBUG
	XDisableInvariant disabler(this);
#endif
		parent->AddChild(this);

		mName = name;
		
		HWND parentH = nil;
		if (style.type == kDocumentWindow || style.type == kModalDialog || style.type == kAlert) {
			XCallback1<bool, IHierarchyPtr> predicate(MatchInterface<IFrameWindow>());
			IHierarchyPtr app(L"Application");
			IFrameWindowPtr frameWind = app->FindChild(predicate);
			if (frameWind)
				parentH = frameWind->GetClientWindow();
		}
		
		mWindow = this->DoCreateWindow(parentH, style, name, size);
		mStyle = style;
		mSize = size;

		this->DoComputeClientInset();	
		
		if (mStyle.drawSizeGrip)
			mSizeGrip = this->DoCreateSizeGrip();
				
		IGeometryPtr geometry(this);
		geometry->SetSize(size);

		CallRepeatedMethod(this, &IInit::OnInit);
	}
	
//	CALL_INVARIANT;			// wait for Open
}


//---------------------------------------------------------------
//
// XWindow::SetWindowProc
//
//---------------------------------------------------------------
void XWindow::SetWindowProc(WindowProc proc)
{
	PRECONDITION(proc != nil);
	
	mWindowProc = proc;
}


//---------------------------------------------------------------
//
// XWindow::Open
//
//---------------------------------------------------------------
void XWindow::Open()
{
	PRECONDITION(!mOpened);
	
	TRACEFLOW("XWindow", "Opening window '", this->GetName(), "'\n");
		
	const IWindow* window = this;
	XStateBroadcaster::Instance()->Broadcast(kOpeningWindow, XAnyValue(window));

	try {
#if DEBUG
		XDisableInvariant disabler(this);
#endif
	
		IPositionWindowPtr position(this);
		if (position)
			position->Position();
				
		if (mStyle.type == kDocumentWindow) {
			IInitMenuBarPtr menuInit(this);
			if (menuInit)
				menuInit->Init(mWindow);

			IMenuBarPtr menubar(this);
			if (menubar) {
				int32 succeeded = ::SetMenu(mWindow, menubar->GetOSMenu());
				ThrowIf(!succeeded);

				this->DoComputeClientInset();	// adding a menu changes the size of the client area
			}
		}

		IRestoreWindowPositionPtr restore(this);
		if (restore) 
			restore->Restore();
		
		this->ForceOnScreen();

		mOpened = true;
	
		CallRepeatedMethod(this, &IOpened::OnOpened);

		if (mStyle.showNew)
			this->Show();
	
	} catch (...) {
		mClosing = true;		

		this->DoClose();
		throw;
	}

	XStateBroadcaster::Instance()->Broadcast(kOpenedWindow, XAnyValue(window));

	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XWindow::IsOpened
//
//---------------------------------------------------------------
bool XWindow::IsOpened() const
{
	return mOpened;
}


//---------------------------------------------------------------
//
// XWindow::Close
//
//---------------------------------------------------------------
bool XWindow::Close(bool quitting, bool forceClose)
{
	PRECONDITION(mOpened);
	UNUSED(quitting);
		
	if (!mClosing) {
		if (forceClose || this->DoCanClose(quitting)) {
			TRACEFLOW("XWindow", "Closing window '", this->GetName(), "'\n");

			const IWindow* window = this;
			XStateBroadcaster::Instance()->Broadcast(kClosingWindow, XAnyValue(window));

			this->Hide();				
			
			IRestoreWindowPositionPtr restore(this);
			if (restore) 
				restore->Save();

			CallRepeatedMethod(this, &IInit::OnCleanup);
			CallRepeatedMethod(this, &IOpened::OnClosing);
			mClosing = true;
			
			this->DoClose();

			XStateBroadcaster::Instance()->Broadcast(kClosedWindow);
		}
	}
		
	POSTCONDITION(true);

	return mClosing;
}


//---------------------------------------------------------------
//
// XWindow::IsVisible
//
//---------------------------------------------------------------
bool XWindow::IsVisible() const
{
	bool visible = false;

	if (mWindow != nil)
		if (::IsWindowVisible(mWindow))
			visible = true;

	return visible;
}


//---------------------------------------------------------------
//
// XWindow::Show
//
//---------------------------------------------------------------
void XWindow::Show()
{
	CHECK_INVARIANT;

	if (!this->IsVisible())	{
		const IWindow* window = this;
		XStateBroadcaster::Instance()->Broadcast(kShowingWindow, XAnyValue(window));
		
		(void) ::ShowWindow(mWindow, SW_SHOWNOACTIVATE);	
				
		if (mLatentFocus.empty()) {
			IRootControlPtr root = this->GetRootControl();
			if (root)
				this->DoGetLatentFocus(IControlPtr(root));
		}

		XStateBroadcaster::Instance()->Broadcast(kShowedWindow, XAnyValue(window));
	}
}


//---------------------------------------------------------------
//
// XWindow::Hide
//
//---------------------------------------------------------------
void XWindow::Hide()
{
	CHECK_INVARIANT;

	if (this->IsVisible()) {
		const IWindow* window = this;
		XStateBroadcaster::Instance()->Broadcast(kHidingWindow, XAnyValue(window));

		IFrameWindowPtr frameWind = GetParent<IFrameWindow>(this);
		if (this->IsActive() && frameWind)
			this->HandleDeactivate();				// child windows don't get the deactivate message...
	
		(void) ::ShowWindow(mWindow, SW_HIDE);

		XStateBroadcaster::Instance()->Broadcast(kHidWindow, XAnyValue(window));
	}
}


//---------------------------------------------------------------
//
// XWindow::AnimatedShow
//
//---------------------------------------------------------------
void XWindow::AnimatedShow(const XRect* startBounds)
{
	UNUSED(startBounds);
	
	this->Show();
}


//---------------------------------------------------------------
//
// XWindow::AnimatedHide
//
//---------------------------------------------------------------
void XWindow::AnimatedHide(const XRect* endBounds)
{
	UNUSED(endBounds);
	
	this->Hide();
}


//---------------------------------------------------------------
//
// XWindow::IsActive
//
//---------------------------------------------------------------
bool XWindow::IsActive() const
{
	bool active = false;
	
	if (mWindow != nil) {
		WINDOWINFO info;
		info.cbSize = sizeof(WINDOWINFO);
		
		int32 succeeded = GetWindowInfo(mWindow, &info);
		active = succeeded && info.dwWindowStatus == WS_ACTIVECAPTION;
	}

	return active;
}


//---------------------------------------------------------------
//
// XWindow::HandleActivate
//
//---------------------------------------------------------------
void XWindow::HandleActivate()
{
	PRECONDITION(!mActive);
 	TRACEFLOW("XWindow", "Activating '", this->GetName(), "'\n");
 	
	if (mStyle.type != kFloatingWindow) {
		if (::GetParent(mWindow) == nil) {
			IActiveMenuBarPtr active(L"Application");
			IMenuBarPtr menubar(this);
			if (active)
				active->Set(menubar);
		}
	}
	
	if (mStyle.targetable) {
		IUndoContextPtr context(this);
		IActiveUndoContextPtr activeContext(L"Application");
		if (activeContext) 
			activeContext->Set(context);

		if (!mLatentFocus.empty()) {
			IRootControlPtr root = this->GetRootControl();
			if (root) {
				IControlPtr child = root->FindChild(mLatentFocus);
				IFocusPtr focus(child);
				if (focus && focus->WantsFocus())
					focus->SetFocus();
				else
					mLatentFocus.clear();
			}
		}
	}
	
	IFrameWindowPtr frameWind(this);	
	if (!frameWind) {
		IHierarchyPtr node(this);
		XCallback1<void, IHierarchyPtr> callback(this, &XWindow::DoActivate);
		node->Traverse(callback);

	} else
		CallRepeatedMethod(this, &IActivated::OnActivated);
		
	const IWindow* window = this;
	XStateBroadcaster::Instance()->Broadcast(kActivatingWindow, XAnyValue(window));
	
	mActive = true;
	
	CHECK_INVARIANT;
	POSTCONDITION(mActive);
}


//---------------------------------------------------------------
//
// XWindow::HandleDeactivate
//
//---------------------------------------------------------------
void XWindow::HandleDeactivate()
{
	PRECONDITION(mActive);
	CHECK_INVARIANT;

	TRACEFLOW("XWindow", "Deactivating '", this->GetName(), "'\n");

	const IWindow* window = this;
	XStateBroadcaster::Instance()->Broadcast(kDeactivatingWindow, XAnyValue(window));

	IFrameWindowPtr frameWind(this);	
	if (!frameWind) {
		IHierarchyPtr node(this);
		XCallback1<void, IHierarchyPtr> callback(this, &XWindow::DoDeactivate);
		node->Traverse(callback);

	} else		
		CallRepeatedMethod(this, &IActivated::OnDeactivated);
		
	mActive = false;

	POSTCONDITION(!mActive);
}


//---------------------------------------------------------------
//
// XWindow::MoveToFront	
//
//---------------------------------------------------------------
void XWindow::MoveToFront()
{
	CHECK_INVARIANT;
	
	int32 succeeded = ::BringWindowToTop(mWindow);
	ThrowIf(!succeeded);
				
//	(void) ::SetActiveWindow(mWindow);
}


//---------------------------------------------------------------
//
// XWindow::MoveToBack
//
//---------------------------------------------------------------
void XWindow::MoveToBack()
{
	CHECK_INVARIANT;
	
	int32 succeeded = ::SetWindowPos(mWindow, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE+SWP_NOSIZE);
	ThrowIf(!succeeded);
	
	IWindowMgrPtr mgr(L"Application");
	IWindowPtr top = mgr->GetTopRegular();
	succeeded = ::BringWindowToTop(top->GetOSWindow());
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XWindow::GetName
//
//---------------------------------------------------------------
std::wstring XWindow::GetName() const
{
	return mName;
}


//---------------------------------------------------------------
//
// XWindow::SetName
//
//---------------------------------------------------------------
void XWindow::SetName(const std::wstring& name)
{
	if (name != this->GetName()) {
		int32 succeeded;
		if (WSystemInfo::HasUnicode())
			succeeded = ::SetWindowTextW(mWindow, name.c_str());
		else
			succeeded = ::SetWindowTextA(mWindow, ToPlatformStr(name).c_str());
		ThrowIf(!succeeded);

		mName = name;
		
		const IWindow* window = this;
		XStateBroadcaster::Instance()->Broadcast(kRenamedWindow, XAnyValue(window));
	}
}


//---------------------------------------------------------------
//
// XWindow::HandleUpdate
//
//---------------------------------------------------------------
void XWindow::HandleUpdate()
{
	CHECK_INVARIANT;
	
	MSG message;
	while (::PeekMessage(&message, mWindow, WM_PAINT, WM_PAINT, PM_NOREMOVE))
		if (::GetMessage(&message, mWindow, WM_PAINT, WM_PAINT))
			::DispatchMessage(&message);
		else
			break;
}


//---------------------------------------------------------------
//
// XWindow::Invalidate () 
//
//---------------------------------------------------------------
void XWindow::Invalidate()
{
	IGeometryPtr geometry(this);
	this->Invalidate(geometry->GetExtent());
}


//---------------------------------------------------------------
//
// XWindow::Invalidate (XRect)  
//
//---------------------------------------------------------------
void XWindow::Invalidate(const XRect& rect)
{
	this->Invalidate(XRegion(rect));
}


//---------------------------------------------------------------
//
// XWindow::Invalidate (XRegion)
//
//---------------------------------------------------------------
void XWindow::Invalidate(const XRegion& rgn)
{
	VERIFY(::InvalidateRgn(mWindow, rgn, true));
}


//---------------------------------------------------------------
//
// XWindow::Validate ()  
//
//---------------------------------------------------------------
void XWindow::Validate()
{
	IGeometryPtr geometry(this);
	this->Validate(geometry->GetExtent());
}


//---------------------------------------------------------------
//
// XWindow::Validate (XRect)  
//
//---------------------------------------------------------------
void XWindow::Validate(const XRect& rect)
{
	this->Validate(XRegion(rect));
}


//---------------------------------------------------------------
//
// XWindow::Validate (XRegion)  
//
//---------------------------------------------------------------
void XWindow::Validate(const XRegion& rgn)
{
	VERIFY(::ValidateRgn(mWindow, rgn));
}


//---------------------------------------------------------------
//
// XWindow::GlobalToWindow (XPoint)
//
//---------------------------------------------------------------
XPoint XWindow::GlobalToWindow(const XPoint& globalPt) const
{
	XPoint pt = globalPt;
	
	int32 succeeded = ::ScreenToClient(mWindow, pt);
	ThrowIf(!succeeded);
	
	return pt;
}


//---------------------------------------------------------------
//
// XWindow::GlobalToWindow  (XRect)
//
//---------------------------------------------------------------
XRect XWindow::GlobalToWindow(const XRect& globalRect) const
{
	XRect rect = globalRect;
	
	int32 succeeded = ::ScreenToClient(mWindow, rect[topLeft]);
	ThrowIf(!succeeded);
	
	succeeded = ::ScreenToClient(mWindow, rect[botRight]);
	ThrowIf(!succeeded);
	
	return rect;
}


//---------------------------------------------------------------
//
// XWindow::WindowToGlobal (XPoint)
//
//---------------------------------------------------------------
XPoint XWindow::WindowToGlobal(const XPoint& windowPt) const
{
	XPoint pt = windowPt;
	
	int32 succeeded = ::ClientToScreen(mWindow, pt);
	ThrowIf(!succeeded);
	
	return pt;
}


//---------------------------------------------------------------
//
// XWindow::WindowToGlobal  (XRect)
//
//---------------------------------------------------------------
XRect XWindow::WindowToGlobal(const XRect& windowRect) const
{
	XRect rect = windowRect;
	
	int32 succeeded = ::ClientToScreen(mWindow, rect[topLeft]);
	ThrowIf(!succeeded);
	
	succeeded = ::ClientToScreen(mWindow, rect[botRight]);
	ThrowIf(!succeeded);
	
	return rect;
}


//---------------------------------------------------------------
//
// XWindow::GetOSDrawContext
//
//---------------------------------------------------------------
OSDrawContext XWindow::GetOSDrawContext() const
{
	if (mDC == nil) {
		mDC = ::GetDC(mWindow);
		ThrowIfNil(mDC);		
	}
	
	return mDC;
}


//---------------------------------------------------------------
//
// XWindow::GetType
//
//---------------------------------------------------------------
EWindowType XWindow::GetType() const
{
	return mStyle.type;
}
 
 
//---------------------------------------------------------------
//
// XWindow::GetLayer
//
//---------------------------------------------------------------
EWindowLayer XWindow::GetLayer() const
{
	EWindowLayer layer;
	
	if (mStyle.windClass == kDocumentWindowClass)
		layer = kRegularLayer;
		
	else if (mStyle.windClass == kFloatingWindowClass)
		layer = kFloatingLayer;
		
	else
		layer = kModalLayer;

	return layer;
}


//---------------------------------------------------------------
//
// XWindow::GetStyle
//
//---------------------------------------------------------------
const SWindowStyle& XWindow::GetStyle() const
{
	return mStyle;
}


//---------------------------------------------------------------
//
// XWindow::GetRootControl
//
//---------------------------------------------------------------
IRootControlPtr XWindow::GetRootControl() const
{
	IRootControlPtr root;
	
	IConstHierarchyPtr node(this);
	IHierarchy::iterator iter = node->begin();
	while (iter != node->end() && !root) {
		IHierarchyPtr child = *iter;
		++iter;
		
		root = IRootControlPtr(child);
	}
	
	return root;
}


//---------------------------------------------------------------
//
// XWindow::ForceOnScreen
//
//---------------------------------------------------------------
void XWindow::ForceOnScreen()
{
	CHECK_INVARIANT;

#if 0								// $$$ needs to be implemented
	const int32 kMinDragArea = 4;	// $$$ be sure to account for MDI child windows and multiple screens

	XRegion draggableRgn;

	if (true && MSystemInfo::GetAppearanceVersion() >= 0x0101) {
		OSStatus err = ::GetWindowRegion(mWindow, kWindowDragRgn, draggableRgn);
		if (err != noErr)									// GetWindowRegion will return an error if the window doesn't have a drag region, unfortunately the error code isn't documented so we'll assume that all errors are due to missing regions...
			draggableRgn = this->GetFrame();
	
	} else {
		ZBuildWindowRegions build(mWindow);
		
		WindowRecord& theWindowRecord = *((WindowPeek) mWindow); 
		draggableRgn = XRegion(theWindowRecord.strucRgn) - theWindowRecord.contRgn;	
		if (draggableRgn.IsEmpty())
			draggableRgn = theWindowRecord.strucRgn;	// at least get the strucRgn 
	}
	

	XRegion deskTopRgn = ::GetGrayRgn();
	deskTopRgn.Inset(kMinDragArea, kMinDragArea);
	deskTopRgn &= draggableRgn;							// do drag rgn && desktop rgn instersect ? 

	if (deskTopRgn.IsEmpty() || !this->IsDraggable((*(RgnHandle) deskTopRgn)->rgnBBox)) {
		XPoint delta = this->DoGetLocationAdjustment();	// no => adjust the window's location 

		XRect bounds = this->GetFrame();
		this->SetLocation(bounds[topLeft] + delta);
	}
#endif
}


//---------------------------------------------------------------
//
// XWindow::HandleMenuSelect
//
//---------------------------------------------------------------
void XWindow::HandleMenuSelect(const IMenu::SHelp& help, uint16 flags)
{
	if ((flags & MF_CHECKED) == MF_CHECKED)		// this is a fairly dumb algorithm, but it's the same one used by balloon help on the Mac...
		this->DoMenuSelect(help.checked);

	else if ((flags & MF_DISABLED) == MF_DISABLED)
		this->DoMenuSelect(help.disabled);

	else 
		this->DoMenuSelect(help.enabled);
}


//---------------------------------------------------------------
//
// XWindow::HandleAltChar
//
//---------------------------------------------------------------
bool XWindow::HandleAltChar(const XCharEvent& event)
{		
	wchar_t ch = event.GetChar();

	std::wstring mnemonic(2, '&');
	mnemonic[1] = ConvertToLowerCase(ch);

	bool handled = this->DoMnemonic(::GetWindow(mWindow, GW_CHILD), mnemonic);
			
	return handled;
}


//---------------------------------------------------------------
//
// XWindow::DoMnemonic	
//
// I haven't been able to find any good discussions of windows
// and mnemonics in MSDN or my reference books. However I think
// that the way this normally works is that IsDialogMessage and
// the dialog box window proc work together to make this happen.
// This isn't really what we want since it's restricted to dialog
// boxes (and Whisper doesn't use dialog box window procs).
//
// So, we'll take care of all the mnemonic key handling ourselves.
//
//---------------------------------------------------------------
bool XWindow::DoMnemonic(HWND control, const std::wstring& mnemonic)
{
	bool handled = false;
	
	while (control != nil && !handled) {
	
		// In order for the control to be used as a mnemonic it must
		// be visible, enabled,
		if (::IsWindowVisible(control) && ::IsWindowEnabled(control)) {
		
			// cannot have the SS_NOPREFIX style,
			int32 style = ::GetWindowLong(control, GWL_STYLE);
			if ((style & SS_NOPREFIX) == 0) {
			
				// and the mnemonic character must be part of the
				// control text.
				std::wstring name = ConvertToLowerCase(Whisper::GetText(control));
				uint32 pos = name.find(mnemonic);
				if (pos != std::wstring::npos) 
					handled = this->DoMnemonic(control);
			}
		}
		
		if (!handled) 
			handled = this->DoMnemonic(::GetWindow(control, GW_CHILD), mnemonic);
		
		control = ::GetWindow(control, GW_HWNDNEXT);
	}

	return handled;
}


//---------------------------------------------------------------
//
// IWindow::GetWindow									[static]
//
//---------------------------------------------------------------
IWindowPtr IWindow::GetWindow(OSWindow windH)
{
	PRECONDITION(windH != nil);
	
	IWindowPtr ptr;

	WNDPROC proc = reinterpret_cast<WNDPROC>(::GetWindowLong(windH, GWL_WNDPROC));
	if (proc == XWindow::DoWindowProc) {
		IWindow* wind = reinterpret_cast<IWindow*>(::GetWindowLong(windH, GWL_USERDATA));
	
//		ASSERT(wind != nil);		// can't do this because we get a bunch of messages while executing CreateWindowEx, but we can't attach the 'this' pointer until CreateWindowEx returns
		ASSERT(wind == nil || wind->GetOSWindow() == windH);

		ptr = IWindowPtr(wind);
	}
	
	return ptr;
}


//---------------------------------------------------------------
//
// XWindow::HandleMessage
//
//---------------------------------------------------------------
bool XWindow::HandleMessage(const MSG& inMsg, int32& result)
{
	MSG msg = inMsg;
	
	bool handled = false;
	IMsgExtensiblePtr extensible(this);
	if (extensible)
		handled = extensible->HandleMessage(msg);

	if (!handled)
		handled = this->DoMessage(msg, result);
	
	return handled;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XWindow::Invariant
//
//---------------------------------------------------------------
void XWindow::Invariant() const
{	
	ASSERT(mWindow != nil);
	
	if (mWindow != nil) {					// may be nil if there was an error during construction
		ASSERT(::IsWindow(mWindow));

		if (::GetWindow(mWindow, GW_CHILD) == nil) {	// the frame window title changes if a child window is maximized...
			int32 length = GetWindowTextLength(mWindow);
			if (WSystemInfo::HasUnicode()) {
				XTinyVector<wchar_t> buffer(length+1UL);
				int32 count = GetWindowTextW(mWindow, buffer.buffer(), numeric_cast<int32>(buffer.size()));
				ASSERT(mName == std::wstring(buffer.buffer(), numeric_cast<uint32>(count)));
			
			} else {
				XTinyVector<char> buffer2(length+1UL);
				int32 count2 = GetWindowTextA(mWindow, buffer2.buffer(), numeric_cast<int32>(buffer2.size()));
				ASSERT(mName == FromPlatformStr(buffer2.buffer(), numeric_cast<uint32>(count2)));
			}
		}
		
		if (!::IsIconic(mWindow)) {
			XRect extent;
			VERIFY(::GetClientRect(mWindow, extent));

			IConstGeometryPtr geometry(this);
			XSize minSize = geometry->GetMinSize();
			XSize maxSize = geometry->GetMaxSize();

			XSize size = extent.GetSize();
			ASSERT(size.width >= minSize.width);
			ASSERT(size.width <= maxSize.width);
//			ASSERT(size.height >= minSize.height);		// $$ this can fire because the window can be resized such that the menu bar wraps before OnSetSize is called so that mClientAreaInset still has the old value
			if (!mClosing)								// otherwise ASSERT can fire because menubar has been deleted
				ASSERT(size.height <= maxSize.height);
			
			ASSERT(geometry->GetSize() == size);
		}
									
	} else {
		ASSERT(mDC == nil);
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XWindow::DoClose
//
//---------------------------------------------------------------
void XWindow::DoClose()
{
	PRECONDITION(mClosing);		
	
	// Remove our controls (need to break cyclic references)
	IHierarchyPtr node(this);
	node->RemoveChildren();
	
	// Make sure we're not still the active menubar (another cycle)
	IMenuBarPtr menubar(this);
	if (menubar) {
		IActiveMenuBarPtr activeMenuBar(L"Application");
		if (activeMenuBar && activeMenuBar->Get() == menubar) {
			IMenuBar* nilMenubar = nil;
			activeMenuBar->Set(nilMenubar);		
		}

		menubar->RemoveMenus();
	}
	
	// Zap the undo actions
	IUndoContextPtr context(this);
	if (context)
		context->ClearHistory();
		
	// Reset the active menubar
	IUndoContextPtr myContext(this);
	IActiveUndoContextPtr activeContext(L"Application");
	if (activeContext && activeContext->Get() == myContext) {
		IMenuBar* nilMenubar = nil;
		activeContext->Set(nilMenubar);
	}

	// Get rid of the size grip and the menu.
	if (mSizeGrip != nil) {
		VERIFY(::DestroyWindow(mSizeGrip));
		mSizeGrip = nil;
	}
		
	(void) ::SetMenu(mWindow, nil);

	// Post a command to remove ourselves from our parent (we use
	// a command so we're not deleted while we're still executing
	// one of our methods)
	IHierarchyPtr parent = node->GetParent();
	XCallback1<void, IHierarchyPtr> function(parent.Get(), &IHierarchy::RemoveChild);	// 'this' may be deleted after RemoveChild executes so we'll wait to call this until we return to the main event loop
	XCallback0<void> proc = Bind1(function, node);	

	ICommandPtr command(L"Callback Command");
	ICallbackCommandPtr callback(command);
	callback->Init(proc);
	
	ICommandQueuePtr queue(L"Application");
	queue->Post(command);
}


//---------------------------------------------------------------
//
// XWindow::DoMessage
//
//---------------------------------------------------------------
bool XWindow::DoMessage(const MSG& msg, int32& result)
{	
	bool handled = true;
		
	switch (msg.message) {
		case WM_CHAR:
			{	
			IKeyHandlerPtr handler = GetInterface<IKeyHandler>(this);
			if (handler)
				handled = handler->HandleChar(msg);
			else
				handled = false;
			}
			break;
			
		case WM_CLOSE:
			{
			IFrameWindowPtr frame(this);
			if (frame)
				handled = false;
			else
				(void) this->Close(false, false);
			}
			break;
		
		case WM_COMMAND:
			{	
			int16 id = (int16) LOWORD(msg.wParam);
			XMenuCommand command = kNothingCmd;

			IActiveMenuBarPtr activeMenubar(L"Application");
			IMenuBarPtr menubar;
			if (activeMenubar) {
				menubar = activeMenubar->Get();
				
				if (menubar) 
					command = menubar->FindCommand(id);
			}
		
			IMenuHandlerPtr handler(L"Application");
			if (command != kNothingCmd && handler)
				handled = handler->HandleCommand(command, msg);
			}
			break;

#if 0								
		case WM_DESTROY:
			{
			// Normally the window will be torn down in the dtor. However it's
			// possible that we'll get a WM_DESTROY out of the blue. If this
			// happens the code below will ensure that all of our objects are
			// deleted.
			if (mDC != nil) {
				::ReleaseDC(mWindow, mDC);
				mDC = nil;
			}
			
			(void) ::SetWindowLong(mWindow, GWL_USERDATA, nil);
			mWindow = nil;
		
			const IWindow* window = this;
			XStateBroadcaster::Instance()->Broadcast(kClosedWindow);

//				XCommand* command = new XDeferredDeleteCommand<XWindow>(this);
//				command->Post();
			}
			break;
#endif

		case WM_DISPLAYCHANGE:
			this->ForceOnScreen();		// $$$ make sure this is still OK with multiple monitors
			break;
		
		case WM_ENTERMENULOOP:
			{
			bool isContextMenu = msg.wParam != 0;
			if (!isContextMenu) {
				IShowHelpPtr show(this);
				if (show)
					show->Show(L"", kZeroPt);
			}
			handled = false;
			}
			break;
	
		case WM_ERASEBKGND:
			result = 1;					// we take care of this in OnDraw
			break;

		case WM_EXITMENULOOP:
			{
			bool isContextMenu = msg.wParam != 0;
			if (!isContextMenu) {
				IShowHelpPtr show(this);
				if (show)
					show->Show(L"", kZeroPt);
			}
			handled = false;
			}
			break;
	
		case WM_GETMINMAXINFO:
			{
			// compute our preferred size
			XSize minSize, maxSize;
			
			if (mStyle.style & WS_THICKFRAME) {
				IGeometryPtr geometry(this);
				minSize = geometry->GetMinSize();
				maxSize = geometry->GetMaxSize();
			
			} else {	
				maxSize = mSize;
				minSize = mSize;
			}
			
			maxSize.width  += mClientAreaInset.left + mClientAreaInset.right;
			maxSize.height += mClientAreaInset.top + mClientAreaInset.bottom;
			
			minSize.width  += mClientAreaInset.left + mClientAreaInset.right;
			minSize.height += mClientAreaInset.top + mClientAreaInset.bottom;
			
			// compute the system's preferred size (this is important with MDI child windows)
			result = ::DefWindowProc(msg.hwnd, msg.message, msg.wParam, msg.lParam);
			handled = true;
			
			// use the smaller of the two
			MINMAXINFO* info = reinterpret_cast<MINMAXINFO*>(msg.lParam);
			
			info->ptMaxTrackSize.x = Min(info->ptMaxTrackSize.x, maxSize.width);
			info->ptMaxTrackSize.y = Min(info->ptMaxTrackSize.y, maxSize.height);

			info->ptMinTrackSize.x = Max(info->ptMinTrackSize.x, minSize.width);
			info->ptMinTrackSize.y = Max(info->ptMinTrackSize.y, minSize.height);
			}
			break;
	
		case WM_INITMENU:
			{
			IApplicationPtr app(L"Application");
			app->HandleUpdateMenus(msg);
			}
			break;
											
		case WM_KEYDOWN:
			{	
			IKeyHandlerPtr handler = GetInterface<IKeyHandler>(this);
			if (handler)
				handled = handler->HandleKeyDown(msg);
			else
				handled = false;
			}
			break;
			
//			case WM_KEYUP:
//				handled = this->HandleKeyUp(msg);
//				break;
			
		case WM_KILLFOCUS:
			TRACEFLOW("XFocus", "window '", this->GetName(), "' has lost the input focus\n");
			CallRepeatedMethod(this, &IFocused::OnLostFocus);
			handled = false;
			break;

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
			handled = false;
			{
				IMouseHandlerPtr handler(this);
				if (handler)
					handler->HandleMouseDown(msg);
			}
			break;

		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
			handled = false;
			{
				IMouseHandlerPtr handler(this);
				if (handler)
					handler->HandleMouseDown(XMouseDownEvent(msg, 2));
			}
			break;

		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
			handled = false;
			{
				IMouseHandlerPtr handler(this);
				if (handler)
					handler->HandleMouseUp(msg);
			}
			break;

		case WM_MENUSELECT:
			{
			IMenu::SHelp help;
			uint16 flags = HIWORD(msg.wParam);

			IMenuBarPtr menubar(this);
			if (menubar) {
				HMENU menuH = (HMENU) msg.lParam;
				if (menuH == nil || (flags & MF_POPUP) == 0) {
					int16 id = (int16) LOWORD(msg.wParam); 
					if (id != 0 && flags != 0xFFFF)
						help = menubar->FindHelp(id);

				} else {
					if (menuH == menubar->GetOSMenu()) {	
						uint32 index = LOWORD(msg.wParam); 
						IMenuPtr menu = menubar->GetMenuByIndex(index);
						help = menu->GetHelp();
					
					} else {
						IMenuPtr menu = menubar->GetMenuByHandle(menuH);
						if (menu != nil)
							help = menu->GetHelp();
					}
				}

				this->HandleMenuSelect(help, flags);
			}
			
			handled = false;
			}
			break;

		case WM_MOUSEMOVE:
			{
			IMouseHandlerPtr handler(this);
			if (handler)
				(void) handler->HandleAdjustCursor(msg);

			handled = false;
			}
			break;

		case WM_MOVE:
			{
			XPoint loc;
			loc.x = LOWORD(msg.lParam);   
			loc.y = HIWORD(msg.lParam);   

			IGeometryPtr geometry(this);
			if (loc != geometry->GetLocation()) {
				const IWindow* window = this;
				XStateBroadcaster::Instance()->Broadcast(kMovedWindow, XAnyValue(window));
			}
			handled = false;
			}
			break;

		case WM_NCACTIVATE:				// we use this instead of WM_ACTIVATE because MDI child windows don't receive WM_ACTIVATE (and only receive WM_MDIACTIVATE once)
			if (!mClosing) {			
				if (msg.wParam) {
					if (!mActive)		// debugger gives us activate events, but not deactivate events...
						this->HandleActivate();
				} else if (mActive)
					this->HandleDeactivate();
			}
			
			handled = false;
			break;
					
		case WM_PAINT:
			{
			XRegion clipRgn;
			int32 type = ::GetUpdateRgn(mWindow, clipRgn, false);	
			if (type != NULLREGION && type != ERROR) {
				PAINTSTRUCT info;
				HDC dc = ::BeginPaint(mWindow, &info);
				if (dc != nil) {
					try {
						XDrawContext context(dc);
						
						IDrawPtr draw(this);
						draw->HandleDraw(context, clipRgn, this->IsActive());
					} catch (...) {
						DEBUGSTR("Got an exception handling WM_PAINT in WWindow");	
					}
					::EndPaint(mWindow, &info);
				}
			}
			}
			break;
			
		case WM_QUERYENDSESSION:
			{
			IApplicationPtr app(L"Application");
			app->Quit();
			result = app->IsQuitting();
			}
			break;
				
		case WM_RBUTTONDOWN:
			{
			IMouseHandlerPtr handler(this);
			if (handler)
				handler->HandleContextMenu(msg);
			else
				handled = false;
			}
			break;

		case WM_SETFOCUS:
			TRACEFLOW("XFocus", "window '", this->GetName(), "' has acquired the input focus\n");
			CallRepeatedMethod(this, &IFocused::OnAcquiredFocus);
			handled = false;
			break;

		case WM_SETCURSOR:
			{
			HWND hwnd = (HWND) msg.wParam; 
			uint32 hitTestCode = LOWORD(msg.lParam);
			if (hwnd != nil && hitTestCode == HTCLIENT)
				result = 1;					// we'll set the cursor in WM_MOUSEMOVE
			else
				result = ::DefWindowProc(msg.hwnd, msg.message, msg.wParam, msg.lParam);
			}
			break;

		case WM_SIZE:
			{
			IFrameWindowPtr frame(this);
			if (frame) {
				HWND statusH = frame->GetStatusWindow();
				(void) ::SendMessage(statusH, WM_SIZE, msg.wParam, msg.lParam);
			}
			}
		
			if (msg.wParam == SIZE_RESTORED) {
				XSize newSize;
				newSize.width  = LOWORD(msg.lParam); 	 // docs say this is for client area
				newSize.height = HIWORD(msg.lParam); 

				IMenuBarPtr menubar(this);
				if (menubar)
					this->DoComputeClientInset();		// changing the window size can cause the menubar to wrap around which changes the client area
	
				XSize oldSize = mSize;
				mSize = newSize;
				this->DoParentChangedSize(newSize - oldSize);
	
				if (mSizeGrip != nil) {
					IGeometryPtr geometry(this);
				
					XRect bounds = geometry->GetExtent();
					bounds.left = bounds.right - ::GetSystemMetrics(SM_CXVSCROLL);
					bounds.top  = bounds.bottom - ::GetSystemMetrics(SM_CYHSCROLL);
					VERIFY(::MoveWindow(mSizeGrip, bounds.left, bounds.top, bounds.GetWidth(), bounds.GetHeight(), kRedraw));					
				}

				ISizeChangedPtr changed(this);
				if (changed)
					changed->OnSizeChanged(oldSize, newSize);
	
				const IWindow* window = this;
				XStateBroadcaster::Instance()->Broadcast(kResizedWindow, XAnyValue(window));
			}
			handled = true;
			break;
			
		case WM_SYSCHAR:
			{
			IKeyHandlerPtr handler(this);
			if (handler)
				handled = handler->HandleChar(msg);
			else
				handled = false;
			}
			break;
						
		default:
			handled = false;
	} 
	
	return handled;
}


//---------------------------------------------------------------
//
// XWindow::DoMenuSelect
//
//---------------------------------------------------------------
void XWindow::DoMenuSelect(const std::wstring& help)	
{
	IShowHelpPtr show(this);
	if (show)
		show->Show(help, kZeroPt);
}


//---------------------------------------------------------------
//
// XWindow::DoMnemonic
//
//---------------------------------------------------------------
bool XWindow::DoMnemonic(HWND control)
{
	PRECONDITION(control != nil);
	
	bool handled = false;

	char className[32];
	int32 count = ::GetClassNameA(control, className, sizeof(className));
	if (count > 0) {
		if (stricmp(className, "Button") == 0) {
			(void) ::SetFocus(control);
			(void) ::PostMessage(control, WM_COMMAND, MAKELONG(0, BN_CLICKED), (int32) control);
			handled = true;
			
		} else if (stricmp(className, "Static") == 0) {
			HWND next = ::GetWindow(control, GW_HWNDPREV);		// this is pretty goofy, but it's what Windows does...
			while (next != nil && !handled) {
				if (::IsWindowVisible(next) && ::IsWindowEnabled(next)) {
					int32 style = ::GetWindowLong(next, GWL_STYLE);
					if ((style & WS_TABSTOP) == WS_TABSTOP)
						handled = this->DoMnemonic(next);
				}
								
				next = ::GetWindow(next, GW_HWNDPREV);
			}

		} else {
			(void) ::SetFocus(control);
			handled = true;
		}
	}

	return handled;
}


//---------------------------------------------------------------
//
// XWindow::DoActivate
//
//---------------------------------------------------------------
void XWindow::DoActivate(const IHierarchyPtr& node)
{
	CallRepeatedMethod(node.Get(), &IActivated::OnActivated);
}


//---------------------------------------------------------------
//
// XWindow::DoDeactivate
//
//---------------------------------------------------------------
void XWindow::DoDeactivate(const IHierarchyPtr& node)
{
	CallRepeatedMethod(node.Get(), &IActivated::OnDeactivated);
}


//---------------------------------------------------------------
//
// XWindow::DoCreateSizeGrip
//
//---------------------------------------------------------------
HWND XWindow::DoCreateSizeGrip()
{
	IGeometryPtr geometry(this);

	XRect bounds = geometry->GetExtent();
	bounds.left = bounds.right - ::GetSystemMetrics(SM_CXVSCROLL);
	bounds.top  = bounds.bottom - ::GetSystemMetrics(SM_CYHSCROLL);

	uint32 style = WS_CHILD + WS_VISIBLE + SBS_SIZEGRIP;

	HWND grip = ::CreateWindowEx(0,						// extended style
								 _TEXT("SCROLLBAR"),	// class name
								 _TEXT(""),				// window name
								 style,					// style
								 bounds.left,			// location
								 bounds.top,
								 bounds.GetWidth(),		// size
								 bounds.GetHeight(),
								 mWindow,				// parent
								 nil,					// menu
								 WSystemInfo::GetAppInstance(),	// app instance
								 nil);					// user data
	POSTCONDITION(grip != nil);	// not worth throwing for

	return grip;
}


//---------------------------------------------------------------
//
// XWindow::DoWindowProc								[static]
//
//---------------------------------------------------------------
LRESULT CALLBACK XWindow::DoWindowProc(HWND windH, UINT message, WPARAM wParam, LPARAM lParam)
{
	int32 result = 0;
	
	try {
		uint32 pos = GetMessagePos();
		
		MSG msg;
		msg.hwnd    = windH;
		msg.message = message;
		msg.wParam  = wParam;
		msg.lParam  = lParam;
		msg.time    = (uint32) GetMessageTime();
		msg.pt.x    = LOWORD(pos);
		msg.pt.y    = HIWORD(pos);
		
		IApplicationPtr app(L"Application");
		if (app) {
			bool handled = app->HandleMessage(msg, result);
			
			IWindowPtr window = IWindow::GetWindow(windH);
			if (!handled && window) 
				handled = window->HandleMessage(msg, result);
			
			if (!handled)
				if (window)
					result = window->GetWindowProc()(windH, message, wParam, lParam);
				else
					result = ::DefWindowProc(windH, message, wParam, lParam);
		}
		
	} catch (const std::exception& e) {
		std::wstring narrative = XError::Instance()->GetText(&e);
		XError::Instance()->DeferError(LoadWhisperString(L"There was an error processing the window message (0x#1)", UInt32ToHex(message)), narrative);

	} catch (...) {
		XError::Instance()->DeferError(LoadWhisperString(L"There was an error processing the window message (0x#1)", UInt32ToHex(message)));
	}
			
	return result;
}


//---------------------------------------------------------------
//
// XWindow::DoComputeClientInset
//
//---------------------------------------------------------------
void XWindow::DoComputeClientInset()
{
	XRect clientArea;
	int32 succeeded = ::GetClientRect(mWindow, clientArea);
	ThrowIf(!succeeded);

	XRect ncClientArea;
	succeeded = ::GetWindowRect(mWindow, ncClientArea);
	ThrowIf(!succeeded);

	XPoint offset = kZeroPt;
	VERIFY(ClientToScreen(mWindow, offset));
	clientArea += offset;

	mClientAreaInset[topLeft]  = clientArea[topLeft]  - ncClientArea[topLeft];
	mClientAreaInset[botRight] = ncClientArea[botRight] - clientArea[botRight];
}


//---------------------------------------------------------------
//
// XWindow::DoGetLatentFocus
//
//---------------------------------------------------------------
void XWindow::DoGetLatentFocus(const IControlPtr& control)
{
	PRECONDITION(mLatentFocus.empty());
	
	IFocusPtr focus(control);
	if (focus && focus->WantsFocus())
		mLatentFocus = control->GetName();
	
	else {
		IHierarchyPtr node(control);
		
		IHierarchy::iterator iter = node->begin();
		while (iter != node->end() && mLatentFocus.empty()) {
			IHierarchyPtr child(*iter);
			++iter;

			this->DoGetLatentFocus(IControlPtr(child));
		}
	}
}


//---------------------------------------------------------------
//
// XWindow::DoParentChangedSize
//
//---------------------------------------------------------------
void XWindow::DoParentChangedSize(const XSize& delta)
{
	IHierarchyPtr root(this);
	
	IHierarchy::iterator iter = root->begin();
	while (iter != root->end()) {
		IHierarchyPtr child = *iter;
		++iter;
		
		IParentChangedSizePtr notify(child);
		if (notify)
			notify->HandleParentSizeChange(delta);
	}
}


//---------------------------------------------------------------
//
// XWindow::DoCanClose
//
//---------------------------------------------------------------
bool XWindow::DoCanClose(bool quitting) const
{
	bool can = true;
	
	XBoss* boss = this->GetBoss();
	for (XBoss::iterator iter = boss->begin(); iter != boss->end() && can; ++iter) {
		IConstCanClosePtr close(boss, iter);
		
		if (close)
			can = close->OnCanClose(quitting);
	}
	
	return can;
}


//---------------------------------------------------------------
//
// XWindow::DoGetWindowClass							[static]
//
//---------------------------------------------------------------
std::wstring XWindow::DoGetWindowClass(uint32 style, uint16 iconID)
{
	std::wstring name = L"XWindow " + UInt32ToStr(style) + L"/" + UInt32ToStr(iconID);
	
	WNDCLASS info;
	if (::GetClassInfo(WSystemInfo::GetAppInstance(), ToPlatformStr(name).c_str(), &info)) {
		ASSERT(info.style == style);
	
	} else {
		PlatformString str = ToPlatformStr(name);	// use a temp so it sticks around

		info.style         = style;
		info.lpfnWndProc   = XWindow::DoWindowProc;
		info.cbClsExtra    = 0;
		info.cbWndExtra    = 0;
		info.hInstance     = WSystemInfo::GetAppInstance();
		info.hIcon         = LoadIcon(WSystemInfo::GetAppInstance(), MAKEINTRESOURCE(iconID));
		info.hCursor       = nil;					// we take care of adjusting the cursor
		info.hbrBackground = nil;					// we take care of erasing
		info.lpszMenuName  = nil;
		info.lpszClassName = str.c_str();			// this isn't speed critical and is invisible to the user so we don't need to bother special casing for NT
		
		ATOM id = ::RegisterClass(&info);
		ThrowIf(id == 0);
	}
	
	return name;
}


//---------------------------------------------------------------
//
// XWindow::DoCreateWindow
//
//---------------------------------------------------------------
OSWindow XWindow::DoCreateWindow(OSWindow parent, const SWindowStyle& style, const std::wstring& name, const XSize& size) const
{			
	UNUSED(size);				// ctor will set this using IGeometry (so we get client size instead of non-client size)
	
	OSWindow wind = nil;
	
	// Find (and register) the window's class name.
	std::wstring className = XWindow::DoGetWindowClass(style.classStyle, style.iconID);
	
	// Create the window.
	if (parent != nil && style.type == kDocumentWindow) {	// this assumes that Windows with parents are MDI child windows...
		if (WSystemInfo::HasUnicode())
			wind = ::CreateMDIWindowW(const_cast<wchar_t*>(className.c_str()),	// class name
									  const_cast<wchar_t*>(name.c_str()),		// window name
									  style.style | WS_CHILD,		// style
									  0,				// location (will be set for real according to style.autoPosition)
									  0,
									  64,				// size
									  64,
									  parent,			// parent window
									  WSystemInfo::GetAppInstance(),	// app instance
									  nil);				// user data
		else
			wind = ::CreateMDIWindowA(const_cast<char*>(ToPlatformStr(className).c_str()),	// class name
									  const_cast<char*>(ToPlatformStr(name).c_str()),		// window name
									  style.style | WS_CHILD,		// style
									  0,				// location (will be set for real according to style.autoPosition)
									  0,
									  64,				// size
									  64,
									  parent,			// parent window
									  WSystemInfo::GetAppInstance(),	// app instance
									  nil);				// user data
	
	} else {
		if (WSystemInfo::HasUnicode())
			wind = ::CreateWindowExW(style.styleEx,		// extended style 
									 className.c_str(),	// class name
									 name.c_str(),		// window name
									 style.style,		// style
									 0,					// location (will be set for real according to style.autoPosition)
									 0,
									 64,				// size
									 64,
									 parent,			// parent window
									 nil,				// menu
									 WSystemInfo::GetAppInstance(),	// app instance
									 nil);				// user data
		else
			wind = ::CreateWindowExA(style.styleEx,		// extended style 
									 ToPlatformStr(className).c_str(),	// class name
									 ToPlatformStr(name).c_str(),		// window name
									 style.style,		// style
									 0,					// location (will be set for real according to style.autoPosition)
									 0,
									 64,				// size
									 64,
									 parent,			// parent window
									 nil,				// menu
									 WSystemInfo::GetAppInstance(),	// app instance
									 nil);				// user data
	}
	ThrowIfNil(wind);
	
	POSTCONDITION(!::IsWindowVisible(wind));

	::SetLastError(0);
	int32 oldValue = ::SetWindowLong(wind, GWL_USERDATA, (int32) this);
	ThrowIf(oldValue == 0 && GetLastError() != noErr);
		
	return wind;
}


}	// namespace Whisper
