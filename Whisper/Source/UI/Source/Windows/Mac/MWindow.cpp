/*
 *  File:       MWindow.cpp
 *  Summary:   	Base class for cross platform window objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MWindow.cpp,v $
 *		Revision 1.20  2001/08/29 01:04:44  jesjones
 *		Wrapped XDisableInvariant with #if DEBUG.
 *		
 *		Revision 1.19  2001/04/27 04:22:45  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.18  2001/04/21 03:29:34  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.17  2001/04/17 01:41:29  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.16  2001/04/13 07:54:05  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.15  2001/03/03 23:58:04  jesjones
 *		Added calls to DEBUGSTR in catch (...) blocks.
 *		
 *		Revision 1.14  2001/02/08 03:52:25  jesjones
 *		Removed KCloseBoxCmd.
 *		
 *		Revision 1.13  2001/02/07 02:28:50  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.12  2001/02/04 03:37:39  jesjones
 *		Changed the way mouse events are routed.
 *		
 *		Revision 1.11  2001/01/30 07:21:03  jesjones
 *		Calls IActivated.
 *		
 *		Revision 1.10  2001/01/21 00:35:37  jesjones
 *		Did some work on the activate/deactivate code.
 *		
 *		Revision 1.9  2000/12/31 04:35:50  jesjones
 *		Renamed the class. Calls SetPort before calling GetWindowRegion (work around for Apple crasher).
 *		
 *		Revision 1.8  2000/12/17 03:48:31  jesjones
 *		OnActivate and OnDeactivate invalidate after calling HiliteWindow.
 *		
 *		Revision 1.7  2000/12/05 09:12:29  jesjones
 *		Nuked the PRECONDITION and POSTCONDITIONs in SetName (they're a pain in the ass when building windows).
 *		
 *		Revision 1.6  2000/11/27 07:17:07  jesjones
 *		Can't contruct an interface pointer with nil anymore.
 *		
 *		Revision 1.5  2000/11/17 08:53:58  jesjones
 *		Calls the new IRestoreWindowPosition interface.
 *		
 *		Revision 1.4  2000/11/12 07:54:55  jesjones
 *		Renamed the callback adaptor functions Bind. Removed the Procedure callback adaptors.
 *		
 *		Revision 1.3  2000/11/10 06:46:27  jesjones
 *		Init calls AddChild at the end instead of at the start.
 *		
 *		Revision 1.2  2000/11/09 09:21:57  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <12>	10/20/00	JDJ		Removed SWindowStyle::handleClicks.
 *		 <11>	10/18/00	JDJ		Replaced the AutoPosition method with a IPositionWindow interface.
 *		 <10>	 8/05/00	JDJ		Added AutoPosition and SendToBack (from IWindowMgr). Renamed
 *									SendToBack MoveToBack.
 *		 <9>	 8/10/99	JDJ		Tweaked kWindowDragRgn code to handle windows without drag regions.
 *		 <8>	 7/20/99	JDJ		Tweaked for Carbon.
 *		 <7>	 7/05/99	JDJ		Dtor only calls CHECK_INVARIANT if mOpened is true.
 *		 <6>	 6/22/99	JDJ		Added support for streaming.
 *		 <5>	 6/13/99	JDJ		OnActivating and OnDeactivating always invalidate the entire window.
 *		 <4>	 6/11/99	JDJ		Tweaked for CW Pro 5fc3.
 *		 <3>	 3/31/99	JDJ		HandleSuspend and HandleResume no longer deal with activation
 *									(this is now handled in MWindowMgr).
 *		 <2>	 1/10/99	JDJ		HandleOpen deletes the window if an exception is thrown.
 *		 <1>	10/12/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IWindow.h>

#include <IActivated.h>
#include <IActiveUndoContext.h>
#include <ICanClose.h>
#include <ICommand.h>
#include <ICommands.h>
#include <ICommandQueue.h>
#include <IControl.h>
#include <IDraw.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IInit.h>
#include <ILocationChanged.h>
#include <IOpened.h>
#include <IParentChangedSize.h>
#include <IPositionWindow.h>
#include <IRestoreWindowPosition.h>
#include <IRootControl.h>
#include <ISizeChanged.h>
#include <IUndoContext.h>
#include <IWindowMgr.h>
#include <MQuickDrawUtils.h>
#include <MSystemInfo.h>
#include <XBind.h>
#include <XDesktop.h>
#include <XDrawContext.h>
#include <XExceptions.h>
#include <XStateBroadcaster.h>

#if GARBAGE_COLLECT_COM
	#include <XGarbageCollector.h>
#endif

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetContentRegion		
//
//---------------------------------------------------------------
static XRegion GetContentRegion(WindowRef wptr)
{
	XRegion rgn, growRgn;
	
	SetPort(GetWindowPort(wptr));		// GetWindowRegion sometimes crashes without this... (Mac OS 9)
		
	OSStatus err = ::GetWindowRegion(wptr, kWindowContentRgn, rgn);
	ThrowIfOSErr(err);

	return rgn;
} 


//---------------------------------------------------------------
//
// GetStructRegion				
//
//---------------------------------------------------------------
static XRegion GetStructRegion(WindowRef wptr)
{
	XRegion rgn;
	
	OSStatus err = ::GetWindowRegion(wptr, kWindowStructureRgn, rgn);
	ThrowIfOSErr(err);
	
	return rgn;
} 

#pragma mark -

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
	virtual IRootControlPtr GetRootControl() const;
	
	virtual const SWindowStyle& GetStyle() const;

	virtual void 		ForceOnScreen();
						
	virtual IControlPtr FindControl(const XPoint& loc) const;

	virtual void 		HandleSuspend();
	virtual void 		HandleResume();

	virtual XRect 		GetContentBounds() const;
	virtual void 		Zoom(int16 part);

	virtual XRect 		GetClientAreaInset()				{return mClientAreaInset;}

protected:
	virtual void 		Invariant() const;

//-----------------------------------
//	Internal API
//
private:
			void		DoClose();
			bool 		DoCanClose(bool quitting) const;

			void 		DoShow();
			void 		DoHide();

			void 		DoActivate();
			void 		DoDeactivate();
			void 		DoActivating(const IHierarchyPtr& node);
			void 		DoDeactivating(const IHierarchyPtr& node);

			bool 		DoIsCollapsed() const;
	
	virtual bool 		DoIsDraggable(const XRect& whichRect);
						// Returns true if any of the corner points of whichRect are draggable. 
								
	virtual GDHandle 	DoGetMaxIntersectedDevice(XRect* screenRect);
						// Returns the screenRect of the most intersected device & its GDHandle. 

	virtual XPoint 		DoGetLocationAdjustment();

			XRect 		DoGetWindowFrameBounds() const;
						// These are all in global coordinates.
							
			XRegion 	DoGetWindowFrameRgn() const;
			XRegion 	DoGetContentRgn() const;
			
	virtual	OSWindow 	DoCreateWindow(OSWindow parent, const SWindowStyle& style, const std::wstring& name, const XSize& size) const;
									
//-----------------------------------
//	Member Data
//
private:
	std::wstring	mName;				// this could be omitted, but having it makes it much easier to figure out which window is which when debugging...
	std::wstring	mID;				// used when streaming in
	
	OSWindow		mWindow;
	XRect 			mClientAreaInset;	// client area is at window rect + mClientAreaInset[topLeft]
	
	bool			mOpened;
	bool			mClosing;
	bool			mWasHidden;
			
	XRect*			mShowAnimBounds;
	XRect*			mHideAnimBounds;
	bool			mActive;

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
	PRECONDITION(mClosing);					
			
	if (mWindow != nil)
		DisposeWindow(mWindow);
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
	
	mShowAnimBounds = nil;
	mHideAnimBounds = nil;
	
	mOpened    = false;
	mClosing   = false;
	mWasHidden = false;
	mActive    = false;
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
		mName = name;
		
		mWindow = this->DoCreateWindow(nil, style, name, size);
		mStyle = style;

		XRect contentBounds = this->GetContentBounds();
		XRect structBounds  = this->DoGetWindowFrameBounds();
					
		mClientAreaInset.left   = contentBounds.left - structBounds.left;
		mClientAreaInset.top    = contentBounds.top - structBounds.top;
		mClientAreaInset.right  = structBounds.right - contentBounds.right;
		mClientAreaInset.bottom = structBounds.bottom - contentBounds.bottom;
		
		ASSERT(mClientAreaInset.left >= 0);
		ASSERT(mClientAreaInset.top >= 0);
		ASSERT(mClientAreaInset.right >= 0);
		ASSERT(mClientAreaInset.bottom >= 0);

		parent->AddChild(this);

		CallRepeatedMethod(this, &IInit::OnInit);
	}
	
//	CALL_INVARIANT;			// wait for Open
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
	CHECK_INVARIANT;
			
	if (!mClosing) {
		if (forceClose || this->DoCanClose(quitting)) {
			TRACEFLOW("XWindow", "Closing window '", this->GetName(), "'\n");

			const IWindow* window = this;
			XStateBroadcaster::Instance()->Broadcast(kClosingWindow, XAnyValue(window));

			this->Hide();				// Note that this will also deactivate the window.
			
			IRestoreWindowPositionPtr restore(this);
			if (restore) 
				restore->Save();

			CallRepeatedMethod(this, &IInit::OnCleanup);
 			CallRepeatedMethod(this, &IOpened::OnClosing);
			mClosing = true;
			
			this->DoClose();
		}

		XStateBroadcaster::Instance()->Broadcast(kClosedWindow);
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
		if (IsWindowVisible(mWindow))
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

	if (!this->IsVisible())	
		this->DoShow();

	POSTCONDITION(this->IsVisible());
}


//---------------------------------------------------------------
//
// XWindow::Hide
//
//---------------------------------------------------------------
void XWindow::Hide()
{
	CHECK_INVARIANT;
	
	if (this->IsActive())
		this->DoDeactivate();

	if (this->IsVisible())	
		this->DoHide();
}


//---------------------------------------------------------------
//
// XWindow::AnimatedShow
//
//---------------------------------------------------------------
void XWindow::AnimatedShow(const XRect* startBounds)
{
	XRect bounds;
	
	if (startBounds != nil)
		bounds = *startBounds;
	
	else {
		IGeometryPtr geometry(this);
		
		XDisplay device = XDesktop::GetMaxDisplay(geometry->GetFrame());
		XRect devRect = device.GetFrame();
		
		bounds = XRect(0, 0, 2, 2);
		bounds.CenterIn(devRect);
	}
	
	mShowAnimBounds = &bounds;
	this->Show();
	mShowAnimBounds = nil;
}


//---------------------------------------------------------------
//
// XWindow::AnimatedHide
//
//---------------------------------------------------------------
void XWindow::AnimatedHide(const XRect* endBounds)
{
	XRect bounds;
	
	if (endBounds != nil)
		bounds = *endBounds;
	
	else {
		IGeometryPtr geometry(this);
		XDisplay device = XDesktop::GetMaxDisplay(geometry->GetFrame());
		XRect devRect = device.GetFrame();
		
		bounds = XRect(0, 0, 2, 2);
		bounds.CenterIn(devRect);
	}
	
	mHideAnimBounds = &bounds;
	this->Hide();
	mHideAnimBounds = nil;
}


//---------------------------------------------------------------
//
// XWindow::IsActive
//
//---------------------------------------------------------------
bool XWindow::IsActive() const
{
	bool active = this->IsVisible() && mActive;
		
	return active;
}


//---------------------------------------------------------------
//
// XWindow::HandleActivate
//
//---------------------------------------------------------------
void XWindow::HandleActivate()
{
	if (this->IsVisible() && !mActive) {
 		TRACEFLOW("XWindow", "Activating '", this->GetName(), "'\n");
 		
		this->DoActivate();
		
		mActive = true;
	}
	
	CHECK_INVARIANT;				// if gIntenseDebugging ASSERT in TWindow::Invariant can occasionally fire if do this on entry... 
}


//---------------------------------------------------------------
//
// XWindow::HandleDeactivate
//
//---------------------------------------------------------------
void XWindow::HandleDeactivate()
{
	CHECK_INVARIANT;

	if (this->IsVisible() && mActive) {
		TRACEFLOW("XWindow", "Deactivating '", this->GetName(), "'\n");
	
		this->DoDeactivate();
		
		mActive = false;
	}
}


//---------------------------------------------------------------
//
// XWindow::MoveToFront
//
//---------------------------------------------------------------
void XWindow::MoveToFront()
{
	CHECK_INVARIANT;
	
	::SelectWindow(mWindow);
}
	

//---------------------------------------------------------------
//
// XWindow::MoveToBack
//
//---------------------------------------------------------------
void XWindow::MoveToBack()
{
	CHECK_INVARIANT;
	
	::SendBehind(mWindow, nil);	
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
	if (name != mName) {
		mName = name;
		::SetWTitle(mWindow, ToPascalStr(mName).c_str());

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
	
	MSetPort setter(mWindow);					
	::SetOrigin(0, 0);
	
	try {		
#if TARGET_CARBON
		XRegion clipRgn;	
		OSStatus err = ::GetWindowRegion(mWindow, kWindowUpdateRgn, clipRgn);
		ASSERT(err == noErr);
		
		Point offset = {0, 0};
		::LocalToGlobal(&offset);
		clipRgn -= XPoint(offset);

		if (err == noErr) 				// don't want to throw since haven't called BeginUpdate yet
#endif
		{
			::BeginUpdate(mWindow);	
		
#if !TARGET_CARBON
			XRegion clipRgn = mWindow->visRgn;	
#endif

			XDrawContext context(GetWindowPort(mWindow));
			context.SetClip(clipRgn);
			
			IDrawPtr draw(this);
			if (draw)
				draw->HandleDraw(context, clipRgn, this->IsActive());
			
			IRootControlPtr root = this->GetRootControl();
			draw = IDrawPtr(root);
			if (draw)
				draw->HandleDraw(context, clipRgn, this->IsActive());

			::EndUpdate(mWindow);
		}
		
	} catch (...) {
		::EndUpdate(mWindow);
		// Ignore any exceptions (putting up a dialog box could
		// lead to an infinite loop).
		DEBUGSTR("Got an exception in XWindow::HandleUpdate");	
	}
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
	MSetPort setter(mWindow);					
	::SetOrigin(0, 0);
	
#if 0
	Rect rect = {-1000, -1000, 1000, 1000};	
	::ClipRect(&rect);
	
	RGBColor temp = kRGBRed.GetOSColor();
	::RGBForeColor(&temp);
	::PaintRgn(rgn);
#endif

#if TARGET_CARBON
	OSStatus err = ::InvalWindowRgn(mWindow, rgn);
	ASSERT(err == noErr);							// don't throw for this?
#else
	::InvalRgn(rgn);
#endif
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
	MSetPort setter(mWindow);					
	::SetOrigin(0, 0);
	
#if TARGET_CARBON
	OSStatus err = ::ValidWindowRgn(mWindow, rgn);
	ASSERT(err == noErr);							// don't throw for this?
#else
	::ValidRgn(rgn);
#endif
}

	
//---------------------------------------------------------------
//
// XWindow::GlobalToWindow (XPoint)
//
//---------------------------------------------------------------
XPoint XWindow::GlobalToWindow(const XPoint& globalPt) const
{
	IConstGeometryPtr geometry(this);
	return globalPt - geometry->GetLocation();
}


//---------------------------------------------------------------
//
// XWindow::GlobalToWindow (XRect)
//
//---------------------------------------------------------------
XRect XWindow::GlobalToWindow(const XRect& rect) const
{
	IConstGeometryPtr geometry(this);
	return rect - geometry->GetLocation();
}


//---------------------------------------------------------------
//
// XWindow::WindowToGlobal (XPoint)
//
//---------------------------------------------------------------
XPoint XWindow::WindowToGlobal(const XPoint& portPt) const
{
	IConstGeometryPtr geometry(this);
	return portPt + geometry->GetLocation();
}

		
//---------------------------------------------------------------
//
// XWindow::WindowToGlobal (XRect)
//
//---------------------------------------------------------------
XRect XWindow::WindowToGlobal(const XRect& rect) const
{
	IConstGeometryPtr geometry(this);
	return rect + geometry->GetLocation();
}


//---------------------------------------------------------------
//
// XWindow::GetOSDrawContext
//
//---------------------------------------------------------------
GrafPtr	XWindow::GetOSDrawContext() const
{
	return GetWindowPort(mWindow);
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
// XWindow::ForceOnScreen (from MacApp)
//
//---------------------------------------------------------------
void XWindow::ForceOnScreen()
{
	CHECK_INVARIANT;

	const int16 kMinDragArea = 4;

	// On many systems (including Color QD && the Radius FPD), it's
	// possible to have a non-rectangular desktop. Try to be nice to 
	// people who saved windows on secondary screens. GrayRgn is the 
	// true indicator of the shape of the desktop--screenBits.bounds 
	// is the size of the screen with the menu bar on it.
	XRegion draggableRgn;

	IGeometryPtr geometry(this);

	OSStatus err = ::GetWindowRegion(mWindow, kWindowDragRgn, draggableRgn);
	if (err != noErr)									// GetWindowRegion will return an error if the window doesn't have a drag region, unfortunately the error code isn't documented so we'll assume that all errors are due to missing regions...
		draggableRgn = geometry->GetFrame();
	
	XRegion deskTopRgn = ::GetGrayRgn();
	deskTopRgn.Inset(kMinDragArea, kMinDragArea);
	deskTopRgn &= draggableRgn;							// do drag rgn && desktop rgn instersect? 

	if (deskTopRgn.IsEmpty() || !this->DoIsDraggable(deskTopRgn.GetEnclosingRect())) {
		XPoint delta = this->DoGetLocationAdjustment();	// no => adjust the window's location 

		XRect bounds = geometry->GetFrame();
		geometry->SetLocation(bounds[topLeft] + delta);
	}
}


//---------------------------------------------------------------
//
// XWindow::HandleSuspend
//
//---------------------------------------------------------------
void XWindow::HandleSuspend()
{
	CHECK_INVARIANT;
	
	TRACEFLOW("XWindow", "Suspending window '", this->GetName(), "'\n");
	
	if (this->IsVisible() && mStyle.hideOnSuspend) {
		this->Hide();
		mWasHidden = true;	
	}
}


//---------------------------------------------------------------
//
// XWindow::HandleResume
//
//---------------------------------------------------------------
void XWindow::HandleResume()
{
	CHECK_INVARIANT;
	
	TRACEFLOW("XWindow", "Resuming window '", this->GetName(), "'\n");
	
	if (mStyle.hideOnSuspend && mWasHidden) {
		this->Show();
		mWasHidden = false;
	}
}


//---------------------------------------------------------------
//
// XWindow::GetContentBounds
//
//---------------------------------------------------------------
XRect XWindow::GetContentBounds() const
{
	XRegion rgn = this->DoGetContentRgn();
	
	XRect bounds = rgn.GetEnclosingRect();
	
	return bounds;
}


//---------------------------------------------------------------
//
// XWindow::Zoom		
//
//---------------------------------------------------------------
void XWindow::Zoom(int16 part)
{	
	XRect oldBounds = this->GetContentBounds();

	IGeometryPtr geometry(this);
	Point idealSize = geometry->GetMaxSize().GetOSPoint();
	::ZoomWindowIdeal(mWindow, part, &idealSize);
		
	XRect newBounds = this->GetContentBounds();
	
	if (newBounds[topLeft] != oldBounds[topLeft]) {	// $$$ can we handle these notifications using Carbon Events?
		ILocationChangedPtr changed(this);
		if (changed)
			changed->OnLocationChanged(newBounds[topLeft]);
		
		XStateBroadcaster::Instance()->Broadcast(kMovedWindow, XAnyValue(this));
	}
	
	XSize oldSize = oldBounds.GetSize();
	XSize newSize = newBounds.GetSize();
	if (newSize != oldBounds.GetSize()) {
		XSize delta = newSize - oldSize;
		
		IHierarchyPtr root(this);					// $$ copy of XWindowGeometry::DoParentChangedSize
		IHierarchy::iterator iter = root->begin();
		while (iter != root->end()) {
			IHierarchyPtr child = *iter;
			++iter;
			
			IParentChangedSizePtr notify(child);
			if (notify)
				notify->HandleParentSizeChange(delta, kRedraw);
		}

		ISizeChangedPtr changed(this);
		if (changed)
			changed->OnSizeChanged(oldSize, newSize);
	
		XStateBroadcaster::Instance()->Broadcast(kResizedWindow, XAnyValue(this));
	}
}


//---------------------------------------------------------------
//
// XWindow::FindControl
//
//---------------------------------------------------------------
IControlPtr XWindow::FindControl(const XPoint& loc) const
{
	IControlPtr control;
	
	IConstWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	if (root) {
		WindowRef wptr = window->GetOSWindow();
	
		int16 part;
		ControlHandle controlH = ::FindControlUnderMouse(loc.GetOSPoint(), wptr, &part);
		if (controlH != nil) 
			control = IControl::GetControl(controlH);
	}
	
	return control;
}


//---------------------------------------------------------------
//
// IWindow::GetWindow									[static]
//
//---------------------------------------------------------------
IWindowPtr IWindow::GetWindow(OSWindow wptr)
{
	IWindowPtr ptr;
	
	if (wptr != nil && ::GetWindowKind(wptr) >= kWhisperWindowKind) {
		IWindow* wind = reinterpret_cast<IWindow*>(::GetWRefCon(wptr));
	
		ASSERT(wind != nil);
		ASSERT(wind->GetOSWindow() == wptr);
		
		ptr = IWindowPtr(wind);
	}
	
	return ptr;
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
	
	IConstHierarchyPtr node(this);
	IHierarchyPtr parent = node->GetParent();
	ASSERT(parent || mClosing);

	IConstGeometryPtr geometry(this);

	if (mWindow != nil) {			// may be nil if there was an error during construction	
		if (!this->DoIsCollapsed()) {	
			XRect frame = this->GetContentBounds();
			ASSERT(geometry->GetLocation() == frame[topLeft]);

			XSize size = frame.GetSize();
			XSize minSize = geometry->GetMinSize();
			XSize maxSize = geometry->GetMaxSize();
			ASSERT(size.width >= minSize.width && size.width <= maxSize.width);
			ASSERT(size.height >= minSize.height && size.height <= maxSize.height);
			
			ASSERT(geometry->GetSize() == size);
		}
				
#if DEBUG
		if (gIntenseDebugging) {
			Str255 title;	
			::GetWTitle(mWindow, title);
			ASSERT(mName == FromPascalStr(title));				// uses a text transcoder object so it's fairly expensive...
		}
#endif
	}
}


//---------------------------------------------------------------
//
// XWindow::DoClose
//
//---------------------------------------------------------------
void XWindow::DoClose()
{
	PRECONDITION(mClosing);		

	IHierarchyPtr node(this);
	
	IHierarchyPtr parent = node->GetParent();

#if GARBAGE_COLLECT_COM
	// Remove ourself from our parent. When garbage collection is on 
	// it's best to do this directly (because the callback command has
	// a reference to 'this' so we won't be deleted when the garbage
	// collect command executes).
	parent->RemoveChild(this);
		
	// After the window is closed collect the garbage (do this via a
	// callback so we're not executing a method in an object that is
	// being deleted).
	XCallback0<void> proc(XGarbageCollector::Instance(), &XGarbageCollector::Collect);	

	ICommandPtr command(L"Callback Command");
	ICallbackCommandPtr callback(command);
	callback->Init(proc);
	
	ICommandQueuePtr queue(L"Application");
	queue->Post(command);

#else
	// When garbage collection is off we need to manually break cycles.
	node->RemoveChildren();

	IUndoContextPtr context(this);
	if (context)
		context->ClearHistory();

	// Post a command that removes 'this' from our parent. If garbage
	// collection is off we'll be deleted in RemoveChild (this is why
	// we use a command: we don't want to be executing XWindow code
	// after we're deleted).
	XCallback1<void, IHierarchyPtr> function(parent.Get(), &IHierarchy::RemoveChild);
	XCallback0<void> proc = Bind1(function, this);	

	ICommandPtr command(L"Callback Command");
	ICallbackCommandPtr callback(command);
	callback->Init(proc);
	
	ICommandQueuePtr queue(L"Application");
	queue->Post(command);
#endif
}


//---------------------------------------------------------------
//
// XWindow::DoShow
//
//---------------------------------------------------------------
void XWindow::DoShow()
{
	const IWindow* window = this;
	XStateBroadcaster::Instance()->Broadcast(kShowingWindow, XAnyValue(window));
					
#if UNIVERSAL_INTERFACES_VERSION >= 0x0320 
	if (MSystemInfo::HasFloatingWindows()) {
		if (mShowAnimBounds != nil) {
			Rect temp = mShowAnimBounds->GetOSRect();
			OSStatus err = ::TransitionWindow(mWindow, kWindowZoomTransitionEffect, kWindowShowTransitionAction, &temp);
			ThrowIfOSErr(err);			
			
		} else
			::MacShowWindow(mWindow);
		
	} else
#endif
		::MacShowWindow(mWindow);

	XStateBroadcaster::Instance()->Broadcast(kShowedWindow, XAnyValue(window));
}


//---------------------------------------------------------------
//
// XWindow::DoHide
//
//---------------------------------------------------------------
void XWindow::DoHide()
{
	const IWindow* window = this;
	XStateBroadcaster::Instance()->Broadcast(kHidingWindow, XAnyValue(window));

#if UNIVERSAL_INTERFACES_VERSION >= 0x0320 
	if (MSystemInfo::HasFloatingWindows()) {
		if (mHideAnimBounds != nil) {
			Rect temp = mHideAnimBounds->GetOSRect();
			OSStatus err = ::TransitionWindow(mWindow, kWindowZoomTransitionEffect, kWindowHideTransitionAction, &temp);
			ThrowIfOSErr(err);			
			
		} else
			::HideWindow(mWindow);
		
	} else
#endif
		::HideWindow(mWindow);

	XStateBroadcaster::Instance()->Broadcast(kHidWindow, XAnyValue(window));

	// It would be more orthogonal to reset the context in deactivate,
	// but then you wouldn't be able to pop up a toolbox dialog and
	// post an undoable command immediately afterwards.
	IUndoContextPtr myContext(this);
	if (myContext) {
		IActiveUndoContextPtr activeContext(L"Application");
		if (activeContext && activeContext->Get() == myContext) 
			activeContext->Set(IUndoContextPtr());
	}
}


//---------------------------------------------------------------
//
// XWindow::DoActivate
//
//---------------------------------------------------------------
void XWindow::DoActivate()
{		
	if (mStyle.targetable) {
		IUndoContextPtr context(this);
		IActiveUndoContextPtr activeContext(L"Application");
		if (activeContext) 
			activeContext->Set(context);
	}
	
	IControlPtr root = this->GetRootControl();
	if (root) {
		OSErr err = ::ActivateControl(root->GetOSControl());
		ASSERT(err == noErr);				// not worth throwing for
	}
		
	::HiliteWindow(mWindow, true);			// sample Appearance code doesn't call this but it's required for IsWindowHilited to work properly
	
	IHierarchyPtr node(this);
	XCallback1<void, IHierarchyPtr> callback(this, &XWindow::DoActivating);
	node->Traverse(callback);
	
	const IWindow* window = this;
	XStateBroadcaster::Instance()->Broadcast(kActivatingWindow, XAnyValue(window));

	this->Invalidate();
}


//---------------------------------------------------------------
//
// XWindow::DoDeactivate
//
//---------------------------------------------------------------
void XWindow::DoDeactivate()
{		
	IControlPtr root = this->GetRootControl();
	if (root) {
		OSErr err = ::DeactivateControl(root->GetOSControl());
		ASSERT(err == noErr);				// not worth throwing for
	}
	
	const IWindow* window = this;
	XStateBroadcaster::Instance()->Broadcast(kDeactivatingWindow, XAnyValue(window));

	::HiliteWindow(mWindow, false);

	IHierarchyPtr node(this);
	XCallback1<void, IHierarchyPtr> callback(this, &XWindow::DoDeactivating);
	node->Traverse(callback);

	this->Invalidate();
}


//---------------------------------------------------------------
//
// XWindow::DoActivating
//
//---------------------------------------------------------------
void XWindow::DoActivating(const IHierarchyPtr& node)
{
	CallRepeatedMethod(node.Get(), &IActivated::OnActivated);
}


//---------------------------------------------------------------
//
// XWindow::DoDeactivating
//
//---------------------------------------------------------------
void XWindow::DoDeactivating(const IHierarchyPtr& node)
{
	CallRepeatedMethod(node.Get(), &IActivated::OnDeactivated);
}


//---------------------------------------------------------------
//
// XWindow::DoIsCollapsed
//
//---------------------------------------------------------------
bool XWindow::DoIsCollapsed() const
{	
	bool collapsed = IsWindowCollapsed(mWindow);
	
	return collapsed;
}


//---------------------------------------------------------------
//
// XWindow::DoIsDraggable (from MacApp)
//
//---------------------------------------------------------------
bool XWindow::DoIsDraggable(const XRect& whichRect)
{
	XRegion draggableRgn;
	
	OSStatus err = ::GetWindowRegion(mWindow, kWindowDragRgn, draggableRgn);
	if (err != noErr) {									// GetWindowRegion will return an error if the window doesn't have a drag region, unfortunately the error code isn't documented so we'll assume that all errors are due to missing regions...
		IGeometryPtr geometry(this);
		draggableRgn = geometry->GetFrame();
	}
	
	bool draggable = draggableRgn.Intersects(whichRect);
	
	return draggable;
} 


//---------------------------------------------------------------
//
// XWindow::DoGetMaxIntersectedDevice (from MacApp)
//
//---------------------------------------------------------------
GDHandle XWindow::DoGetMaxIntersectedDevice(XRect* screenRect)
{
	PRECONDITION(screenRect != nil);
	
	int32 maxSectArea = 0;

	XRegion deskTopRgn = ::GetGrayRgn();
	XRect moveBounds = deskTopRgn.GetEnclosingRect();
	moveBounds.Inset(XPoint(4, 4));

	XRect globalStrucRect = Whisper::GetStructRegion(mWindow).GetEnclosingRect();
	
	GDHandle aGDHandle = GetDeviceList();
	GDHandle maxSectGD = GetMainDevice();			// set as best choice default 
	while (aGDHandle) {								// calc which scrn intersects largest part of window
		if (TestDeviceAttribute(aGDHandle, screenDevice) && TestDeviceAttribute(aGDHandle, screenActive)) {
			XRect aGDScreenRect = (*aGDHandle)->gdRect;
			XRect gdSectRect = globalStrucRect & aGDScreenRect;
			if (aGDScreenRect.Intersects(moveBounds) && !gdSectRect.IsEmpty()) {
				int32 sectArea = gdSectRect.GetHeight() * gdSectRect.GetWidth();
				if (sectArea > maxSectArea) {		// do we have a new winner? 
					maxSectArea = sectArea;
					maxSectGD = aGDHandle;
				}
			}
		}
		aGDHandle = GetNextDevice(aGDHandle);
	}

	if (maxSectGD != GetMainDevice())
		*screenRect = (*maxSectGD)->gdRect;
	else {
		// Account for menu bar on the main screen.
		XRegion tempRgn;
		Rect gdRect = (*maxSectGD)->gdRect;

		RectRgn(tempRgn, &gdRect);						// main screen with menubar 
		SectRgn(tempRgn, GetGrayRgn(), tempRgn);		// GetGrayRgn == desktop rgn w/o menubar 
		*screenRect = tempRgn.GetEnclosingRect();		// => main screen w/o menubar 
	}
	
	return maxSectGD;
} 


//---------------------------------------------------------------
//
// XWindow::DoGetLocationAdjustment (from MacApp)
//
//---------------------------------------------------------------
XPoint XWindow::DoGetLocationAdjustment()
{
	const int16 kMinDragArea = 4;
	
	XPoint delta = kZeroPt;
										
	// since we don't _really_ know what the drag rgn is, we'll assume 
	// that moving the topleft pt of the window on screen is sufficient 
	// to make it draggable, so calculate the deltas necessary to move 
	// the topleft pt into visible screen XRect.
	XRect visScreenRect;
	this->DoGetMaxIntersectedDevice(&visScreenRect);
	visScreenRect.Inset(XPoint(kMinDragArea, kMinDragArea));

	IGeometryPtr geometry(this);

	XRect frame = geometry->GetFrame();
	if (frame.top < visScreenRect.top)
		delta.y = visScreenRect.top - frame.top + mClientAreaInset.top;	
	else if (frame.top > visScreenRect.bottom)
		delta.y = visScreenRect.bottom - frame.top - mClientAreaInset.top;

	if (frame.left < visScreenRect.left)
		delta.x = visScreenRect.left - frame.left + mClientAreaInset.left;
	else if (frame.left > visScreenRect.right)
		delta.x = visScreenRect.right - frame.right - mClientAreaInset.left;
		
	return XPoint(delta.x, delta.y);
} 


//---------------------------------------------------------------
//
// XWindow::DoGetWindowFrameBounds
//
//---------------------------------------------------------------
XRect XWindow::DoGetWindowFrameBounds() const
{
	XRegion rgn = this->DoGetWindowFrameRgn();
	
	XRect bounds = rgn.GetEnclosingRect();
	
	return bounds;
}


//---------------------------------------------------------------
//
// XWindow::DoGetWindowFrameRgn
//
//---------------------------------------------------------------
XRegion XWindow::DoGetWindowFrameRgn() const
{
	XRegion frameRgn = Whisper::GetStructRegion(mWindow);

	XRegion contentRgn = this->DoGetContentRgn();
	frameRgn -= contentRgn;
		
	return frameRgn;
}


//---------------------------------------------------------------
//
// XWindow::DoGetContentRgn
//
//---------------------------------------------------------------
XRegion XWindow::DoGetContentRgn() const
{
	XRegion rgn = Whisper::GetContentRegion(mWindow);
		
	return rgn;
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
// XWindow::DoCreateWindow
//
//---------------------------------------------------------------
OSWindow XWindow::DoCreateWindow(OSWindow parent, const SWindowStyle& style, const std::wstring& name, const XSize& size) const
{			
	PRECONDITION(parent == nil);
	
	WindowRef wptr = nil;
	
	// This should not be neccesary, but I was getting some really
	// bizarre behavior with some of the windows in one of my apps:
	// immediately after calling NewCWindow the portPixMap member
	// was incorrect (eg the bounds member was too small). My guess
	// is that this is a bug in Mac OS 8.5.1.
	::SetGWorld(GetWorkPort(), nil);
	
	Rect area = XRect(kZeroPt, size).GetOSRect();

	OSStatus err = ::CreateNewWindow(style.windClass, style.attributes, &area, &wptr);
	ThrowIfOSErr(err);
		
	SetWTitle(wptr, ToPascalStr(name).c_str());
	ASSERT(!::IsWindowVisible(wptr));
	
	::SetWindowKind(wptr, kWhisperWindowKind);
	::SetWRefCon(wptr, (int32) this);
		
	return wptr;
}


}	// namespace Whisper
