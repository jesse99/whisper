/*
 *  File:       WControl.cpp
 *  Summary:   	An interface that is common to all controls.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WControl.cpp,v $
 *		Revision 1.18  2001/04/21 03:30:39  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.17  2001/04/17 01:41:52  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.16  2001/03/17 12:31:53  jesjones
 *		Key messages are always handled if a key handler is present.
 *		
 *		Revision 1.15  2001/03/09 10:21:31  jesjones
 *		Reworked the help stuff so that it uses IShowHelp instead of WinHelp.
 *		
 *		Revision 1.14  2001/03/03 23:58:14  jesjones
 *		Added calls to DEBUGSTR in catch (...) blocks.
 *		
 *		Revision 1.13  2001/02/24 09:38:14  jesjones
 *		Calls HandleMouseUp.
 *		
 *		Revision 1.12  2001/02/21 08:15:48  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.11  2001/01/30 07:22:10  jesjones
 *		Uses CallRepeatedMethod.
 *		
 *		Revision 1.10  2001/01/03 05:54:55  jesjones
 *		Renamed XControl.
 *		
 *		Revision 1.9  2000/12/31 12:47:55  jesjones
 *		Fixed some compile errors.
 *		
 *		Revision 1.8  2000/12/31 04:41:06  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.7  2000/12/27 01:02:12  jesjones
 *		Sets the undo context when the focus is acquired (useful for textboxes in dialogs).
 *		
 *		Revision 1.6  2000/12/14 08:30:14  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.5  2000/12/08 08:12:58  jesjones
 *		Button clicks are routed to the control instead of the extensions.
 *		
 *		Revision 1.4  2000/11/27 03:31:43  jesjones
 *		Added some XFocus TRACEFLOWs.
 *		
 *		Revision 1.3  2000/11/25 01:52:20  jesjones
 *		Key and menu handlers call GetInterface instead of querying this.
 *		
 *		Revision 1.2  2000/11/09 12:05:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IControl.h>

#include <IActiveUndoContext.h>
#include <IApplication.h>
#include <IActiveMenuBar.h>
#include <IChildNode.h>
#include <IClickExtensible.h>
#include <ICommand.h>
#include <ICommandQueue.h>
#include <ICommands.h>
#include <IDraw.h>
#include <IFocus.h>
#include <IFocused.h>
#include <IGeometry.h>
#include <IHelpText.h>
#include <IHierarchy.h>
#include <IKeyHandler.h>
#include <IMenuBar.h>
#include <IMenuHandler.h>
#include <IMouseHandler.h>
#include <IMsgExtensible.h>
#include <IRootControl.h>
#include <IScrollableControl.h>
#include <IScrollBar.h>
#include <IShowHelp.h>
#include <IUndoContext.h>
#include <IWindow.h>
#include <WSystemInfo.h>
#include <XExceptions.h>
#include <XFileSystem.h>
#include <XIntConversions.h>
#include <XInterfaceMacros.h>
#include <XKeyEvents.h>
#include <XMouseEvents.h>

namespace Whisper {


// ===================================================================================
//	class XControlContext
// ===================================================================================

//---------------------------------------------------------------
//
// XControlContext::~XControlContext
//
//---------------------------------------------------------------
XControlContext::~XControlContext()
{
}


//---------------------------------------------------------------
//
// XControlContext::XControlContext
//
//---------------------------------------------------------------
XControlContext::XControlContext(const IControlPtr& control) : XDrawContext(control->GetOSDrawContext())
{		
	XPoint offset = kZeroPt;
	IScrollableControlPtr scrollable = GetInterface<IScrollableControl>(control);
	if (scrollable) 
		offset = scrollable->GetScrollOffset();

	this->SetOrigin(offset);

	IGeometryPtr geometry(control);
	XRegion clip = geometry->GetVisibleExtent();
	this->SetClip(clip);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XControl
//!		An interface that is common to all controls.
// ===================================================================================
class XControl : public IControl, public IChildNode {	
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XControl();
	
						XControl(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:	
	virtual std::wstring GetName() const					{return mName;}
	virtual void 		SetName(const std::wstring& name)	{mName = name;}
	
	virtual void 		Invalidate();
	virtual void 		Invalidate(const XRect& rect)		{this->Invalidate(XRegion(rect));}
	virtual void 		Invalidate(const XRegion& rgn, bool clipToExtent = true);
			
	virtual void 		Validate();
	virtual void 		Validate(const XRect& rect)			{this->Validate(XRegion(rect));}
	virtual void 		Validate(const XRegion& rgn, bool clipToExtent = true);
	
	virtual void 		HandleUpdate();
	virtual	bool 		IsVisible() const;	
	virtual void 		Show(bool redraw = kRedraw);	
	virtual void 		Hide(bool redraw = kRedraw);
		
	virtual	bool 		IsActive() const;		
	virtual	bool 		IsEnabled() const;		
	virtual void 		Enable(bool enable = true, bool redraw = kRedraw);
	
	virtual XPoint 		WindowToLocal(const XPoint& pt) const;
	virtual XRect 		WindowToLocal(const XRect& rect) const;
	virtual XRegion 	WindowToLocal(const XRegion& rgn) const;
			
	virtual XPoint 		LocalToWindow(const XPoint& pt) const;
	virtual XRect 		LocalToWindow(const XRect& rect) const;
	virtual XRegion 	LocalToWindow(const XRegion& rgn) const;
			
	virtual XPoint 		ParentToLocal(const XPoint& pt) const;	
	virtual XRect 		ParentToLocal(const XRect& rect) const;
	virtual XRegion 	ParentToLocal(const XRegion& rgn) const;
			
	virtual XPoint 		LocalToParent(const XPoint& pt) const;
	virtual XRect		LocalToParent(const XRect& rect) const;
	virtual XRegion		LocalToParent(const XRegion& rgn) const;

	virtual OSControl 	GetOSControl() const			{return mControl;}		
	virtual OSDrawContext GetOSDrawContext() const;
	virtual WNDPROC 	GetSystemWindowProc() const 	{return mPrevProc;}

	virtual	bool 		HandleMessage(const MSG& message, int32& result);

	virtual void 		DoKey(const XKeyDownEvent& event);
	virtual void 		DoChar(const XCharEvent& event);
	virtual void 		DoPostCreate(const std::wstring& name, const IHierarchyPtr& parent, OSControl controlH);

protected:
	virtual void 		OnAdopted()						{}
	virtual void 		OnOrphaned()					{}

//-----------------------------------
//	Internal API
//
public:
	static LRESULT CALLBACK DoWindowProc(HWND wptr, UINT message, WPARAM wParam, LPARAM lParam);

private:
	virtual void 		Invariant() const;
		
//-----------------------------------
//	Member Data
//
private:
	OSControl		mControl;			
	std::wstring	mName;
	bool			mEnabled;

	mutable HDC		mDC;
	uint32			mStyle;
	std::wstring	mClass;
	
	uint32			mProcID;		// mac

	WNDPROC			mPrevProc;
	uint32			mProcDepth;
};

DEFINE_INTERFACE_FACTORY(XControl)

//---------------------------------------------------------------
//
// XControl::~XControl
//
//---------------------------------------------------------------
XControl::~XControl()
{
#if 0		// we do this in XHierarchy::RemoveChild since we can't query for interfaces inside dtors
	if (!this->GetBoss()->IsPrototype()) {
		IRootControlPtr root = GetParent<IRootControl>(this);
		if (root) 
			root->RemovedChild(this);
	}
#endif
	
	if (mControl != nil) {
		if (mDC != nil) 
			ReleaseDC(mControl, mDC);
			
		(void) ::SetWindowLong(mControl, GWL_USERDATA, nil);	// we're no longer an XControl
		(void) ::SetWindowLong(mControl, GWL_WNDPROC, (int32) mPrevProc);
		
		HWND controlH = mControl;
		mControl = nil;
		
		VERIFY(::DestroyWindow(controlH));
	}	
}


//---------------------------------------------------------------
//
// XControl::XControl
//
//---------------------------------------------------------------
XControl::XControl(XBoss* boss) 
{
	IControl::DoSetBoss(boss);
	IChildNode::DoSetBoss(boss);
	
	mControl = nil;
	mEnabled = true;
	mDC = nil;

	mPrevProc = nil;
	mProcDepth = 0;
}

	
//---------------------------------------------------------------
//
// XControl::Invalidate ()
//
//---------------------------------------------------------------
void XControl::Invalidate()
{
	IControl* thisPtr = this;
	IGeometryPtr geometry(thisPtr);
	XRect extent = geometry->GetExtent();		
	
	this->Invalidate(extent, false);
}


//---------------------------------------------------------------
//
// XControl::Invalidate
//
//---------------------------------------------------------------
void XControl::Invalidate(const XRegion& rgn, bool clipToPane)
{
	CHECK_INVARIANT;
	
	if (this->IsVisible()) {		
		XRegion local = rgn;
		
		IControl* thisPtr = this;
		if (clipToPane) {
			IGeometryPtr geometry(thisPtr);
			XRect extent = geometry->GetExtent();
			local &= extent;
		}
		
		XRegion super = this->LocalToParent(local);
		
		IControlPtr parent = GetParent<IControl>(thisPtr);
		if (parent)
			parent->Invalidate(super);
			
		else {
			IWindowPtr window = GetParent<IWindow>(thisPtr);
			window->Invalidate(super);
		}
	}
}


//---------------------------------------------------------------
//
// XControl::Validate ()
//
//---------------------------------------------------------------
void XControl::Validate()
{
	IControl* thisPtr = this;
	IGeometryPtr geometry(thisPtr);
	XRect extent = geometry->GetExtent();	
	
	this->Validate(extent, false);
}


//---------------------------------------------------------------
//
// XControl::Validate
//
//---------------------------------------------------------------
void XControl::Validate(const XRegion& rgn, bool clipToPane)
{
	CHECK_INVARIANT;
	
	if (this->IsVisible()) {
		XRegion superRgn = rgn;
		
		IControl* thisPtr = this;
		if (clipToPane) {
			IGeometryPtr geometry(thisPtr);
			XRect extent = geometry->GetExtent();
			superRgn &= extent;
		}
		
		IControlPtr parent = GetParent<IControl>(thisPtr);
		if (parent)
			parent->Validate(superRgn);
			
		else {
			IWindowPtr window = GetParent<IWindow>(thisPtr);
			window->Validate(superRgn);
		}
	}
}


//---------------------------------------------------------------
//
// XControl::HandleUpdate
//
//---------------------------------------------------------------
void XControl::HandleUpdate()
{
	CHECK_INVARIANT;
	
	IControl* thisPtr = this;
	IWindowPtr window = GetParent<IWindow>(thisPtr);
	window->HandleUpdate();
}


//---------------------------------------------------------------
//
// XControl::IsVisible
//
//---------------------------------------------------------------
bool XControl::IsVisible() const
{
	int32 visible = ::IsWindowVisible(mControl);

	return visible != 0;
}


//---------------------------------------------------------------
//
// XControl::Show
//
//---------------------------------------------------------------
void XControl::Show(bool redraw)
{
	CHECK_INVARIANT;

	if (!this->IsVisible()) {	
		(void) ::ShowWindow(mControl, SW_SHOWNOACTIVATE);

		if (redraw)
			this->Invalidate();
	}

#if DEBUG
	IControl* thisPtr = this;
	IRootControlPtr root(thisPtr);
	if (root) {
		IWindowPtr window = GetParent<IWindow>(thisPtr);
		POSTCONDITION(this->IsVisible() || (window && !window->IsVisible()));	
	
	} else {
		IControlPtr parent = GetParent<IControl>(thisPtr);
		POSTCONDITION(this->IsVisible() || (parent && !parent->IsVisible()));	
	}
#endif
}


//---------------------------------------------------------------
//
// XControl::Hide
//
//---------------------------------------------------------------
void XControl::Hide(bool redraw)
{
	CHECK_INVARIANT;
	
	bool wasVisible = this->IsVisible();

	IControl* thisPtr = this;
	IFocusPtr focus(thisPtr);
	bool wasFocused = focus && focus->HasFocus();

	(void) ::ShowWindow(mControl, SW_HIDE);
	
	if (wasFocused) {
		IFocusPtr super = GetParent<IFocus>(thisPtr);
		while (super && !super->WantsFocus())
			super = GetParent<IFocus>(super);
		
		super->SetFocus();				// if no one else intervenes app will get the focus
	}

	if (redraw && wasVisible)
		this->Invalidate();

	POSTCONDITION(!this->IsVisible());
}


//---------------------------------------------------------------
//
// XControl::IsActive
//
//---------------------------------------------------------------
bool XControl::IsActive() const
{
	bool active = false;
	
	const IControl* thisPtr = this;
	IWindowPtr window = GetParent<IWindow>(thisPtr);
	if (window->IsActive())
		if (this->IsVisible())
			if (::IsWindowEnabled(mControl))
				active = true;

	return active;
}


//---------------------------------------------------------------
//
// XControl::IsEnabled
//
//---------------------------------------------------------------
bool XControl::IsEnabled() const
{
	int32 enabled = ::IsWindowEnabled(mControl);
	
	return enabled != 0;
}


//---------------------------------------------------------------
//
// XControl::Enable
//
//---------------------------------------------------------------
void XControl::Enable(bool enable, bool redraw)
{
	CHECK_INVARIANT;

	if (enable != mEnabled) {			
		IControl* thisPtr = this;
		if (!enable) {
			IFocusPtr focus(thisPtr);
			if (focus && focus->HasFocus()) {
				IFocusPtr super = GetParent<IFocus>(thisPtr);
				while (super && !super->WantsFocus())
					super = GetParent<IFocus>(super);
				
				super->SetFocus();				// if no one else intervenes app will get the focus
			}
		}
			
		if (!redraw)
			VERIFY(::LockWindowUpdate(mControl));	

		(void) ::EnableWindow(mControl, enable);
		mEnabled = enable;
		
		if (!redraw)
			VERIFY(::LockWindowUpdate(nil));
	}
}


//---------------------------------------------------------------
//
// XControl::LocalToWindow (XPoint)
//
//---------------------------------------------------------------
XPoint XControl::LocalToWindow(const XPoint& localPt) const
{
	XPoint portPt;
	
	const IControl* thisPtr = this;
	IConstControlPtr parent = GetConstParent<IControl>(thisPtr);
	if (parent) {
		XPoint superPt = this->LocalToParent(localPt);
		portPt = parent->LocalToWindow(superPt);
	
	} else
		portPt = localPt;
	
	return portPt;
}


//---------------------------------------------------------------
//
// XControl::LocalToWindow (XRect)
//
//---------------------------------------------------------------
XRect XControl::LocalToWindow(const XRect& rect) const
{
	XRect result;
	
	result[topLeft]  = this->LocalToWindow(rect[topLeft]);
	result[botRight] = this->LocalToWindow(rect[botRight]);
	
	return result;
}


//---------------------------------------------------------------
//
// XControl::LocalToWindow (XRegion)
//
//---------------------------------------------------------------
XRegion XControl::LocalToWindow(const XRegion& rgn) const
{
	XPoint offset = this->LocalToWindow(kZeroPt);
		
	return rgn + offset;
}


//---------------------------------------------------------------
//
// XControl::WindowToLocal (XPoint)
//
//---------------------------------------------------------------
XPoint XControl::WindowToLocal(const XPoint& portPt) const
{
	XPoint localPt;
	
	const IControl* thisPtr = this;
	IConstControlPtr parent = GetConstParent<IControl>(thisPtr);
	if (parent) {
		XPoint superPt = parent->WindowToLocal(portPt);
		localPt = this->ParentToLocal(superPt);
	
	} else
		localPt = portPt;
	
	return localPt;
}


//---------------------------------------------------------------
//
// XControl::WindowToLocal (XRect)
//
//---------------------------------------------------------------
XRect XControl::WindowToLocal(const XRect& rect) const
{
	XRect result;
	
	result[topLeft]  = this->WindowToLocal(rect[topLeft]);
	result[botRight] = this->WindowToLocal(rect[botRight]);
	
	return result;
}


//---------------------------------------------------------------
//
// XControl::WindowToLocal (XRegion)
//
//---------------------------------------------------------------
XRegion XControl::WindowToLocal(const XRegion& rgn) const
{
	XPoint offset = this->LocalToWindow(kZeroPt);
		
	return rgn - offset;
}


//---------------------------------------------------------------
//
// XControl::LocalToParent (XPoint)
//
//---------------------------------------------------------------
XPoint XControl::LocalToParent(const XPoint& pt) const
{
	XPoint outPt = pt;
	
	const IControl* thisPtr = this;
	IConstGeometryPtr geometry(thisPtr);
	outPt += geometry->GetLocation();

	ConstScrollableControlPtr scrollable(thisPtr);
	if (scrollable) 
		outPt += scrollable->GetScrollOffset();
		
	return outPt;
}


//---------------------------------------------------------------
//
// XControl::LocalToParent (XRect)
//
//---------------------------------------------------------------
XRect XControl::LocalToParent(const XRect& rect) const
{
	XRect result;
	
	result[topLeft]  = this->LocalToParent(rect[topLeft]);
	result[botRight] = this->LocalToParent(rect[botRight]);
	
	return result;
}


//---------------------------------------------------------------
//
// XControl::LocalToParent (XRegion)
//
//---------------------------------------------------------------
XRegion XControl::LocalToParent(const XRegion& rgn) const
{
	XPoint offset = this->LocalToParent(kZeroPt);
		
	return rgn + offset;
}


//---------------------------------------------------------------
//
// XControl::ParentToLocal (XPoint)
//
//---------------------------------------------------------------
XPoint XControl::ParentToLocal(const XPoint& pt) const
{
	XPoint outPt = pt;
	
	const IControl* thisPtr = this;
	IConstGeometryPtr geometry(thisPtr);
	outPt -= geometry->GetLocation();

	ConstScrollableControlPtr scrollable(thisPtr);
	if (scrollable) 
		outPt -= scrollable->GetScrollOffset();
				
	return outPt;
}


//---------------------------------------------------------------
//
// XControl::ParentToLocal (XRect)
//
//---------------------------------------------------------------
XRect XControl::ParentToLocal(const XRect& rect) const
{
	XRect result;
	
	result[topLeft]  = this->ParentToLocal(rect[topLeft]);
	result[botRight] = this->ParentToLocal(rect[botRight]);
	
	return result;
}


//---------------------------------------------------------------
//
// XControl::ParentToLocal (XRegion)
//
//---------------------------------------------------------------
XRegion XControl::ParentToLocal(const XRegion& rgn) const
{
	XPoint offset = this->LocalToParent(kZeroPt);
		
	return rgn - offset;
}


//---------------------------------------------------------------
//
// XControl::GetOSDrawContext
//
//---------------------------------------------------------------
OSDrawContext XControl::GetOSDrawContext() const
{
	if (mDC == nil) {
		mDC = ::GetDC(mControl);
		ThrowIfNil(mDC);		
	}
	
	return mDC;
}


//---------------------------------------------------------------
//
// IControl::GetControl									[static]
//
//---------------------------------------------------------------
IControlPtr IControl::GetControl(OSControl controlH)
{
	PRECONDITION(controlH != nil);
	
	IControlPtr control;

	if (controlH != nil) {
		WNDPROC proc = reinterpret_cast<WNDPROC>(::GetWindowLong(controlH, GWL_WNDPROC));
		if (proc == XControl::DoWindowProc) {
			IControl* ptr = reinterpret_cast<IControl*>(::GetWindowLong(controlH, GWL_USERDATA));
			control = IControlPtr(ptr);
		
			ASSERT(control);
			ASSERT(!control || control->GetOSControl() == controlH);
		}
	}
	
	return control;	
}


//---------------------------------------------------------------
//
// XControl::HandleMessage
//
//---------------------------------------------------------------
bool XControl::HandleMessage(const MSG& inMsg, int32& result)
{
	MSG msg = inMsg;
	
	bool handled = false;
	IControl* thisPtr = this;
	IMsgExtensiblePtr extensible(thisPtr);
	if (extensible)
		handled = extensible->HandleMessage(msg);

	++mProcDepth;
	ASSERT(mProcDepth < 100);		// check for infinite loop

	if (!handled) {
		handled = true;
		
		switch (msg.message) {
			case WM_CHAR:
				{
				IKeyHandlerPtr handler(L"Application");
				if (handler)
					(void) handler->HandleChar(msg);
				else
					handled = false;
				}
				break;
				
			case WM_COMMAND:
				handled = false;	

				if (msg.lParam == 0) {
					IActiveMenuBarPtr active(L"Application");
					if (active) {
						IMenuBarPtr menubar = active->Get();
						if (menubar) {
							int16 id = (int16) LOWORD(msg.wParam);
							XMenuCommand command = menubar->FindCommand(id);
							
							IMenuHandlerPtr handler(L"Application");
							if (command != kNothingCmd && handler)
								handled = handler->HandleCommand(command, msg);
						}
					}
				}

				if (!handled) {
					int32 notifyCode = HIWORD(msg.wParam);
					if (notifyCode == BN_CLICKED && msg.lParam != 0) {
						HWND controlH = reinterpret_cast<HWND>(msg.lParam);
						IControlPtr control = IControl::GetControl(controlH);
						IClickExtensiblePtr extensible(control);

						if (extensible) {
							MSG clickMsg = msg;
							clickMsg.hwnd    = controlH;
							clickMsg.message = WM_LBUTTONDOWN;
							clickMsg.wParam  = 0;				// key flags
							clickMsg.lParam  = MAKELONG(1, 1);	// cursor position (client coordinates)
							
							extensible->HandleClick(clickMsg);
							handled = true;
						}
					}
				}
				break;
						
			case WM_DESTROY:
				mControl = nil;			// sometimes our parent will be destroyed before we get destroyed...
				handled = false;
				break;
				
			case WM_DRAWITEM:
				{
				handled = false;

				const DRAWITEMSTRUCT* item = reinterpret_cast<const DRAWITEMSTRUCT*>(msg.lParam);
				if (item->itemAction == ODA_DRAWENTIRE) {	
					IControlPtr control = IControl::GetControl(item->hwndItem);
					IDrawPtr draw(control);
					
					if (draw) {
						XDrawContext context(item->hDC);
						
						IGeometryPtr geometry(control);
						draw->HandleDraw(context, geometry->GetExtent(), control->IsActive());
						handled = true;
					}
				}
				}
				break;
			
			case WM_HELP:
				{ 
				IShowHelpPtr display(thisPtr);
				IHelpTextPtr text(thisPtr);
				if (display && text) {
					IWindowPtr window = GetParent<IWindow>(thisPtr);
					XPoint pt = window->GlobalToWindow(msg.pt);
					pt = this->WindowToLocal(pt);
						
					std::wstring message = text->GetMessage(pt);
					if (message.length() > 0) {
						display->Show(message, msg.pt);	
						handled = true;
					}
				}
				}
				break;
			
			case WM_HSCROLL:
			case WM_VSCROLL:
				{
				HWND windH = reinterpret_cast<HWND>(msg.lParam);
				IControlPtr control = IControl::GetControl(windH);
				IScrollBarPtr scrollbar(control);
				if (scrollbar)
					scrollbar->HandleTrack((int32) msg.wParam);
				else
					handled = false;
				}
				break;
			
			case WM_KEYDOWN:
				{
				IKeyHandlerPtr handler(L"Application");
				if (handler)
					(void) handler->HandleKeyDown(msg);
				else
					handled = false;
				}
				break;
				
//			case WM_KEYUP:
//				handled = this->HandleKeyUp(msg);
//				break;
				
			case WM_KILLFOCUS:
				TRACEFLOW("XFocus", "control '", this->GetName(), "' has lost the input focus\n");
				CallRepeatedMethod(thisPtr, &IFocused::OnLostFocus);
				handled = false;
				break;

			case WM_LBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case STN_CLICKED:
				{
				IMouseHandlerPtr handler(thisPtr);
				if (handler)
					handled = handler->HandleMouseDown(msg);
				else
					handled = false;
				}
				break;

			case WM_LBUTTONDBLCLK:
			case WM_MBUTTONDBLCLK:
			case STN_DBLCLK:
				{
				IMouseHandlerPtr handler(thisPtr);
				if (handler)
					handled = handler->HandleMouseDown(XMouseDownEvent(msg, 2));
				else
					handled = false;
				}
				break;

			case WM_LBUTTONUP:
			case WM_MBUTTONUP:
				handled = false;
				{
					IMouseHandlerPtr handler(thisPtr);
					if (handler)
						handler->HandleMouseUp(msg);
				}
				break;

			case WM_MOUSEMOVE:
				{
				IMouseHandlerPtr handler(thisPtr);
				if (handler)
					(void) handler->HandleAdjustCursor(msg);
				}
				handled = false;
				break;

			case WM_PAINT:
				{
				handled = false;

				IDrawPtr draw(thisPtr);
				if (draw) {
					XRegion clipRgn;
					int32 type = ::GetUpdateRgn(mControl, clipRgn, false);	
					if (type != NULLREGION && type != ERROR) {
						PAINTSTRUCT info;
						HDC dc = ::BeginPaint(mControl, &info);
						if (dc != nil) {
							try {
								XDrawContext context(dc);
	
								draw->HandleDraw(context, clipRgn, this->IsActive());
							} catch (...) {
								DEBUGSTR("Got an exception handling WM_PAINT in WControl");	
							}
							::EndPaint(mControl, &info);
							handled = true;
						}
					} 
				}
				}
				break;
								
			case WM_RBUTTONDOWN:
				{
				IMouseHandlerPtr handler(thisPtr);
				if (handler)
					handled = handler->HandleContextMenu(msg);
				else
					handled = false;
				}
				break;

			case WM_SETCURSOR:
				{
				HWND hwnd = (HWND) msg.wParam;      
				if (hwnd == nil)
					result = ::CallWindowProc(mPrevProc, msg.hwnd, msg.message, msg.wParam, msg.lParam);
				else
					result = 1;					// we set the cursor in WM_MOUSEMOVE
				}
				break;

			case WM_SETFOCUS:
				{
				TRACEFLOW("XFocus", "control '", this->GetName(), "' has acquired the input focus\n");
				CallRepeatedMethod(thisPtr, &IFocused::OnAcquiredFocus);
				
				IUndoContextPtr context(thisPtr);
				IActiveUndoContextPtr activeContext(L"Application");
				if (context && activeContext) 
					activeContext->Set(context);
					
				handled = false;
				}
				break;

			case WM_SYSCHAR:
				{
				IWindowPtr window = GetParent<IWindow>(thisPtr);
				handled = window->HandleAltChar(msg);
				}
				break;
							
			case WM_WINDOWPOSCHANGED:
				handled = false;
				break;
										
			default:
				handled = false;
		}
	}	
	
	--mProcDepth;

	return handled;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XControl::Invariant
//
//---------------------------------------------------------------
void XControl::Invariant() const
{
	IControl::Invariant();
	
	ASSERT(mControl != nil);
	ASSERT(::IsWindow(mControl));

	// Make sure the OS parent matches the hierarchy parent
	HWND parentH = ::GetParent(mControl);	

	const IControl* thisPtr = this;
	IConstControlPtr parent = GetConstParent<IControl>(thisPtr);
	if (parent) {
		ASSERT(parentH == parent->GetOSControl());
		
	} else {
		IConstWindowPtr window = GetConstParent<IWindow>(thisPtr);
		ASSERT(parentH == window->GetOSWindow());
	}
}


//---------------------------------------------------------------
//
// XControl::DoKey
//
//---------------------------------------------------------------
void XControl::DoKey(const XKeyDownEvent& event)
{
	MSG msg = event.GetOSEvent();
	
	WNDPROC proc = this->GetSystemWindowProc();
	(void) ::CallWindowProc(proc, msg.hwnd, msg.message, msg.wParam, msg.lParam);
}


//---------------------------------------------------------------
//
// XControl::DoChar
//
//---------------------------------------------------------------
void XControl::DoChar(const XCharEvent& event)
{
	MSG msg = event.GetOSEvent();
	
	WNDPROC proc = this->GetSystemWindowProc();	
	(void) ::CallWindowProc(proc, msg.hwnd, msg.message, msg.wParam, msg.lParam);
}


//---------------------------------------------------------------
//
// XControl::DoWindowProc								[static]
//
//---------------------------------------------------------------
LRESULT CALLBACK XControl::DoWindowProc(HWND controlH, UINT message, WPARAM wParam, LPARAM lParam)
{
	int32 result = 0;
		
	try {
		uint32 pos = ::GetMessagePos();
		
		MSG msg;
		msg.hwnd    = controlH;
		msg.message = message;
		msg.wParam  = wParam;
		msg.lParam  = lParam;
		msg.time    = (uint32) ::GetMessageTime();
		msg.pt.x    = LOWORD(pos);
		msg.pt.y    = HIWORD(pos);
		
		IApplicationPtr app(L"Application");
		bool handled = app->HandleMessage(msg, result);
		
		WNDPROC proc = reinterpret_cast<WNDPROC>(::GetWindowLong(controlH, GWL_WNDPROC));
		if (!handled && proc == XControl::DoWindowProc) {
			IControl* ptr = reinterpret_cast<IControl*>(::GetWindowLong(controlH, GWL_USERDATA));
		
			if (ptr != nil) {
				handled = ptr->HandleMessage(msg, result);
		
				if (!handled)
					result = ::CallWindowProc(ptr->GetSystemWindowProc(), controlH, message, wParam, lParam);
			}
		}

	} catch (const std::exception& e) {
		ICommandPtr command(L"Deferred Error Command");
		IDeferredErrorCommandPtr error(command);
		error->Init(LoadWhisperString(L"There was an error processing the window message (0x#1)", UInt32ToHex(message)), &e);
		
		ICommandQueuePtr queue(L"Application");
		queue->Post(command);

	} catch (...) {
		ICommandPtr command(L"Deferred Error Command");
		IDeferredErrorCommandPtr error(command);
		error->Init(LoadWhisperString(L"There was an error processing the window message (0x#1)", UInt32ToHex(message)), nil);
		
		ICommandQueuePtr queue(L"Application");
		queue->Post(command);
	}
			
	return result;
}


//---------------------------------------------------------------
//
// XControl::DoPostCreate
//
//---------------------------------------------------------------
void XControl::DoPostCreate(const std::wstring& name, const IHierarchyPtr& parent, OSControl controlH)
{
	PRECONDITION(controlH != nil);
	PRECONDITION(mControl == nil);
			
	mControl = controlH;
	
	// Set the user data to point to 'this' so we can get an IControlPtr
	// when given an HWND.
	::SetLastError(0);
	int32 oldValue = ::SetWindowLong(mControl, GWL_USERDATA, (int32) this);
	ThrowIf(oldValue == 0 && GetLastError() != noErr);

	// Switch in our message handler.
	mPrevProc = (WNDPROC) ::GetWindowLong(mControl, GWL_WNDPROC);

	::SetLastError(0);
	oldValue = ::SetWindowLong(mControl, GWL_WNDPROC, (int32) XControl::DoWindowProc);
	ThrowIf(oldValue == 0 && GetLastError() != noErr);

	// Set the control's name
	this->SetName(name);

	// Make the control a child of the parent
	IControl* thisPtr = this;
	parent->AddChild(thisPtr);
}


}	// namespace Whisper
