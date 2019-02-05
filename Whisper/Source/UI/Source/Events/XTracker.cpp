/*
 *  File:       XTracker.cpp
 *  Summary:   	ITracker helper.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTracker.cpp,v $
 *		Revision 1.5  2001/04/27 04:37:36  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/02/17 10:22:48  jesjones
 *		Renamed HandleStart TrackMouse.
 *		
 *		Revision 1.3  2001/02/07 04:59:14  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.2  2000/12/31 12:48:17  jesjones
 *		Added an include for Windows.
 *		
 *		Revision 1.1  2000/12/31 04:30:49  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <ITracker.h>

#include <IControl.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <ITrackerHelper.h>
#include <IWindow.h>
#include <XExceptions.h>

#if MAC
	#include <Appearance.h>
	#include <IEventDispatcher.h>
	#include <MProcess.h>
#endif

namespace Whisper {


// ===================================================================================
//	class XTracker
//!		ITracker helper.
// ===================================================================================
class XTracker : public ITracker {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XTracker();
	
						XTracker(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:													
	virtual void 		Init(bool trackNonMovement = kDontTrackNonMovement, bool useDragSound = false);
	
	virtual void 		TrackMouse(const IHierarchyPtr& context, const XPoint& startPt);
										
	virtual XRect 		GetArea() const;

	virtual IControlPtr GetControl() const				{return mControl;}						
	virtual IWindowPtr 	GetWindow() const				{return mWindow;}

//-----------------------------------
//	Internal API
//
private:														
			void 		DoTracking();
			void 		DoTrack();
			void		DoStop();
		
			bool 		DoKeepTracking() const;
			void 		DoConstrain(const XPoint& anchorPt, const XPoint& prevPt, XPoint& nextPt);
			
//-----------------------------------
//	Member Data
//
private:
	IWindowPtr		mWindow;			// always non-nil
	IControlPtr		mControl;			// may be nil
	IHierarchyPtr	mContext;
	
	bool			mTrackNonMovement;	// defaults to false, which means don't track mouse unless it moves
	bool 			mUseDragSound;		// ignored on Windows
	bool			mTracked;			// true if DoTrack has been called
	bool			mInited;

	XPoint			mStartPoint;
	XPoint			mAnchorPoint;		// defaults to mStartPoint
	XPoint			mPrevPoint;
	XPoint			mNextPoint;
	
	XPoint			mHysteresis;		// amount the mouse must move before it's considered to have actually moved. Defaults to one pixel

#if WIN
	HWND			mContextH;
	uint32			mMouseDownMsg;		// used to abort track loop, defaults to WM_LBUTTONDOWN
	uint32			mMouseUpMsg;		// and WM_LBUTTONUP
#endif
};

DEFINE_INTERFACE_FACTORY(XTracker)

//---------------------------------------------------------------
//
// XTracker::~XTracker
//
//---------------------------------------------------------------
XTracker::~XTracker()
{
}


//---------------------------------------------------------------
//
// XTracker::XTracker
//
//---------------------------------------------------------------
XTracker::XTracker(XBoss* boss)
{
	this->DoSetBoss(boss);
	
	mInited = false;
}


//---------------------------------------------------------------
//
// XTracker::Init
//
//---------------------------------------------------------------
void XTracker::Init(bool trackNonMovement, bool useDragSound)
{
	mTrackNonMovement = trackNonMovement;
	mUseDragSound = useDragSound;
	
	mHysteresis.x = 1;
	mHysteresis.y = 1;
	
#if WIN
	mMouseDownMsg = WM_LBUTTONDOWN;
	mMouseUpMsg   = WM_LBUTTONUP;
#endif

	mInited = true;
}

	
//---------------------------------------------------------------
//
// XTracker::TrackMouse
//
//---------------------------------------------------------------
void XTracker::TrackMouse(const IHierarchyPtr& context, const XPoint& startPt)
{
	ASSERT(mInited);
	
	mContext = context;
	mControl = IControlPtr(context);
	if (mControl)
		mWindow = GetParent<IWindow>(context);
	else
		mWindow = IWindowPtr(context);
	ASSERT(mWindow);
	
	mStartPoint  = startPt;
	mAnchorPoint = startPt;
	
#if MAC && UNIVERSAL_INTERFACES_VERSION >= 0x0320 
	if (mUseDragSound) {
		OSStatus err = ::BeginThemeDragSound(kThemeDragSoundDragging);
		ASSERT(err == noErr);	
	}

#elif WIN
	mContextH = mControl ? mControl->GetOSControl() : mWindow->GetOSWindow();
	
	(void) ::SetCapture(mContextH);
	if (::GetCapture() != mContextH)
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't capture the mouse.")));	// $$ pretty lame error message...
#endif

	this->DoConstrain(mAnchorPoint, mAnchorPoint, mAnchorPoint);
	this->DoConstrain(mAnchorPoint, mAnchorPoint, mStartPoint);
	
	ITrackerHelperPtr helper(this);
	helper->OnStart(mStartPoint);
	
	mNextPoint = mPrevPoint = mStartPoint;
	mTracked = false;

	this->DoTracking();
}

										
//---------------------------------------------------------------
//
// XTracker::GetArea
//
//---------------------------------------------------------------
XRect XTracker::GetArea() const
{
	XRect rect;
	
	if (mNextPoint.y < mAnchorPoint.y) {	
		if (mNextPoint.x < mAnchorPoint.x) {
			rect[topLeft]  = mNextPoint;
			rect[botRight] = mAnchorPoint;
			
		} else {
			rect.top    = mNextPoint.y;
			rect.left   = mAnchorPoint.x;
			rect.right  = mNextPoint.x;
			rect.bottom = mAnchorPoint.y;
		}
		
	} else {	
		if (mNextPoint.x > mAnchorPoint.x) {
			rect[topLeft]  = mAnchorPoint;
			rect[botRight] = mNextPoint;
			
		} else {
			rect.top    = mAnchorPoint.y;
			rect.left   = mNextPoint.x;
			rect.right  = mAnchorPoint.x;
			rect.bottom = mNextPoint.y;
		}
	}
	
	return rect;
}


//---------------------------------------------------------------
//
// XTracker::DoTracking
//
//---------------------------------------------------------------
void XTracker::DoTracking()
{
	try {
#if MAC
		do {
			// Get the current mouse position and call DoTrack
			// (note that using mouse moved events is dubious because
			// we want to call DoTrack even if the mouse
			// doesn't move).
			::SetPort(mWindow->GetOSDrawContext());
			::SetOrigin(0, 0);
				
			Point pt;
			::GetMouse(&pt);
			if (mControl)
				mNextPoint = mControl->WindowToLocal(pt);
			else
				mNextPoint = pt;
			
			this->DoTrack();

			// Let the app process update events while we're tracking
			// and abort tracking when the mouse is released (or is
			// pressed, but this should never happen). Note that it's
			// important that this be done after calling DoTrack
			// because we promised to call DoTrack at least once.
			EventRecord event;
			if (::WaitNextEvent(updateMask + mUpMask + mDownMask, &event, 0, nil)) {
				ASSERT(event.what != mouseDown);
				
				if (event.what == updateEvt) {
					IEventDispatcherPtr dispatcher(L"Application");
					dispatcher->DispatchEvent(event);
				
				} else
					break;
			}
		} while (this->DoKeepTracking() && MProcess::InFront());

#elif WIN
		do {
			// Get the current mouse position and call DoTrack
			// (note that using mouse moved messages is dubious because
			// we want to call DoTrack even if the mouse doesn't move).
			XPoint pt;
			int32 succeeded = ::GetCursorPos(pt);
			ThrowIf(!succeeded);

			mNextPoint = mWindow->GlobalToWindow(pt);
			if (mControl)
				mNextPoint = mControl->WindowToLocal(mNextPoint);
			
			this->DoTrack();

			// Let the app process paint messages while we're tracking
			// and abort tracking when the mouse is released (or is
			// pressed, but this should never happen). Note that it's
			// important that this be done after calling DoTrack
			// because we promised to call DoTrack at least
			// once.
			MSG msg;
			if (::PeekMessage(&msg, nil, WM_PAINT, WM_PAINT, PM_REMOVE) != 0)
				::DispatchMessage(&msg);

			if (::PeekMessage(&msg, nil, mMouseDownMsg, mMouseUpMsg, PM_REMOVE) != 0)
				break;
		} while (this->DoKeepTracking());
#endif

	} catch (...) {
		this->DoStop();
		throw;
	}		
	
	// Let the tracker perform any neccesary cleanup.
	this->DoStop();
}

	
//---------------------------------------------------------------
//
// XTracker::DoTrack
//
//---------------------------------------------------------------
void XTracker::DoTrack()
{
	this->DoConstrain(mAnchorPoint, mPrevPoint, mNextPoint);
	
	XPoint delta = mNextPoint - mPrevPoint;
	bool mouseMoved = Abs(delta.x) >= mHysteresis.x || Abs(delta.y) >= mHysteresis.y;
	
	if (mouseMoved || mTrackNonMovement || !mTracked) {
		ITrackerHelperPtr helper(this);
		helper->OnTrack(mAnchorPoint, mPrevPoint, mNextPoint, mouseMoved);
	}
	
	mPrevPoint = mNextPoint;
	
	mTracked = true;
}

	
//---------------------------------------------------------------
//
// XTracker::DoStop
//
//---------------------------------------------------------------
void XTracker::DoStop()
{
#if MAC && UNIVERSAL_INTERFACES_VERSION >= 0x0320 
	if (mUseDragSound) {
		OSStatus err = ::EndThemeDragSound();					
		ASSERT(err == noErr);	
	}
	
#elif WIN
	ASSERT(::GetCapture() == mContextH);
	VERIFY(::ReleaseCapture());
#endif

	ITrackerHelperPtr helper(this);
	helper->OnStop(mNextPoint);
}
		

//---------------------------------------------------------------
//
// XTracker::DoKeepTracking
//
//---------------------------------------------------------------
bool XTracker::DoKeepTracking() const
{
#if MAC
	bool tracking = ::StillDown();
#elif WIN
	bool tracking = true;
#endif

	return tracking;
}

	
//---------------------------------------------------------------
//
// XTracker::DoConstrain
//
//---------------------------------------------------------------
void XTracker::DoConstrain(const XPoint& anchorPt, const XPoint& prevPt, XPoint& nextPt)
{	
	IConstGeometryPtr geometry(mContext);
	XRect bounds = geometry->GetExtent();
	
	ITrackerHelperPtr helper(this);
	helper->OnConstrain(anchorPt, prevPt, nextPt, bounds);
}

	
}	// namespace Whisper
