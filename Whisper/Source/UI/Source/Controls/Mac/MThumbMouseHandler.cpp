/*
 *  File:       MThumbMouseHandler.cpp
 *  Summary:   	Mouse handler that handles clicking on things like sliders and scrollbars.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MThumbMouseHandler.cpp,v $
 *		Revision 1.9  2001/05/14 10:54:03  jesjones
 *		DoThumbProc and DoActionProc trap exceptions.
 *		
 *		Revision 1.8  2001/04/21 03:29:13  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.7  2001/04/13 07:53:44  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.6  2001/03/17 08:57:09  jesjones
 *		OnMouseDown no longer calls DoTrack.
 *		
 *		Revision 1.5  2001/02/12 09:46:08  jesjones
 *		Added calls to IClickExtensible.
 *		
 *		Revision 1.4  2001/02/04 03:35:45  jesjones
 *		Removed the implementation inheritance from the mouse handling classes.
 *		
 *		Revision 1.3  2000/12/17 03:47:34  jesjones
 *		Got rid of the dollar signs around Log.
 *		
 *		Revision 1.2  2000/11/09 09:20:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IMouseDownHelper.h>

#include <IClickExtensible.h>
#include <IControl.h>
#include <IControlPressedHelper.h>
#include <IHierarchy.h>
#include <IRootControl.h>
#include <ITrackControlHelper.h>
#include <IValueControl.h>
#include <IWindow.h>
#include <MQuickDrawUtils.h>
#include <MSystemInfo.h>
#include <XExceptions.h>
#include <XMouseEvents.h>

namespace Whisper {


// ===================================================================================
//	class MThumbMouseHandler
//!		Mouse handler that handles clicking on things like sliders and scrollbars.
// ===================================================================================
class MThumbMouseHandler : public IMouseDownHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~MThumbMouseHandler();
	
						MThumbMouseHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual bool 		OnMouseDown(const XMouseDownEvent& event);

//-----------------------------------
//	Internal API
//
private:
			void		DoTrack(int32 delta);
			void		DoTrackThumb();

	static pascal void	DoActionProc(ControlHandle control, ControlPartCode part);
	static pascal void	DoThumbProc(ControlHandle theControl, ControlPartCode partCode);

//-----------------------------------
//	Member Data
//
private:
	ControlActionUPP		    mTrackUPP;
	ControlActionUPP		    mThumbUPP;
	
	static MThumbMouseHandler*	msTrackingControl;	// thumb callback doesn't give you the control so we'll save it here
	static XMouseDownEvent	    msStartEvent;		// callback also doesn't give you the original event...
};

DEFINE_INTERFACE_FACTORY(MThumbMouseHandler)

MThumbMouseHandler*	MThumbMouseHandler::msTrackingControl = nil;
XMouseDownEvent	    MThumbMouseHandler::msStartEvent;		

//---------------------------------------------------------------
//
// MThumbMouseHandler::~MThumbMouseHandler
//
//---------------------------------------------------------------
MThumbMouseHandler::~MThumbMouseHandler()
{
#if TARGET_CARBON
	::DisposeControlActionUPP(mThumbUPP);
	::DisposeControlActionUPP(mTrackUPP);
#else
	::DisposeRoutineDescriptor(mThumbUPP);
	::DisposeRoutineDescriptor(mTrackUPP);
#endif
}


//---------------------------------------------------------------
//
// MThumbMouseHandler::MThumbMouseHandler
//
//---------------------------------------------------------------
MThumbMouseHandler::MThumbMouseHandler(XBoss* boss)
{
	IMouseDownHelper::DoSetBoss(boss);
	
	mThumbUPP = NewControlActionUPP(MThumbMouseHandler::DoThumbProc);		
	ThrowIfNil(mThumbUPP);

	mTrackUPP = NewControlActionUPP(MThumbMouseHandler::DoActionProc);
	ThrowIfNil(mTrackUPP);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// MThumbMouseHandler::OnMouseDown
//
//---------------------------------------------------------------
bool MThumbMouseHandler::OnMouseDown(const XMouseDownEvent& event)	
{
	IControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();

	XPoint loc = event.GetPosition();
	loc = control->LocalToWindow(loc);
	
	IControlPtr root = GetParent<IRootControl>(this);
	XControlContext context(root);
	context.MakeCurrent();
	
	int16 partCode = ::TestControl(controlH, loc.GetOSPoint());
	if (partCode != kControlNoPart) {
		ASSERT(msTrackingControl == nil);
		msTrackingControl = this;
		msStartEvent = event;
		
		ControlActionUPP actionProc = partCode == kControlIndicatorPart ? mThumbUPP : mTrackUPP;
		(void) ::TrackControl(controlH, loc.GetOSPoint(), actionProc);
		
		msTrackingControl = nil;
	}

	return kHandled;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// MThumbMouseHandler::DoTrack
//
//---------------------------------------------------------------
void MThumbMouseHandler::DoTrack(int32 delta)
{
	PRECONDITION(delta != 0);
	
	IValueControlPtr value(this);
	
	int32 oldValue = value->GetValue();	
	int32 newValue = oldValue + delta;
	
	if (value->SetValue(newValue)) {		
		IWindowPtr window = GetParent<IWindow>(this);
		IControlPtr control(this);

		EventRecord event;
		event.what      = mouseDown;
		event.message   = 0;				// undefined for mouseDown events
		event.when      = TickCount();
		event.where     = window->WindowToGlobal(control->LocalToWindow(XPoint(1, 1))).GetOSPoint();
		event.modifiers = 0;

		IClickExtensiblePtr extensible(this);
		if (extensible)
			extensible->HandleClick(event);
		
		IControlPressedHelperPtr pressed(this);
		if (pressed)
			pressed->OnPressed(event);
	
		window->HandleUpdate();				// will handle any pending update in the window
	}
}

	
//---------------------------------------------------------------
//
// MThumbMouseHandler::DoTrackThumb
//
//---------------------------------------------------------------
void MThumbMouseHandler::DoTrackThumb()
{
	IWindowPtr window = GetParent<IWindow>(this);
	IControlPtr control(this);

	EventRecord event;
	event.what      = mouseDown;
	event.message   = 0;				// undefined for mouseDown events
	event.when      = TickCount();
	event.where     = window->WindowToGlobal(control->LocalToWindow(XPoint(1, 1))).GetOSPoint();
	event.modifiers = 0;
	
	IClickExtensiblePtr extensible(this);
	if (extensible)
		extensible->HandleClick(event);
		
	IControlPressedHelperPtr pressed(this);
	if (pressed)
		pressed->OnPressed(event);		// allows for active scrolling

	window->HandleUpdate();				// will handle any pending update in the window
}


//---------------------------------------------------------------
//
// MThumbMouseHandler::DoActionProc						[static]
//
//---------------------------------------------------------------
pascal void	MThumbMouseHandler::DoActionProc(ControlHandle controlH, ControlPartCode partCode)
{
#if DEBUG
	PRECONDITION(msTrackingControl != nil);
	PRECONDITION(partCode != kControlIndicatorPart);

	IControlPtr control(msTrackingControl);
	PRECONDITION(controlH == control->GetOSControl());
#endif

	try {
		ITrackControlHelperPtr track(msTrackingControl);
		if (track) {
			int32 delta = track->OnTrackAction(partCode);
			
			if (delta != 0) {
				IMouseDownHelperPtr down(msTrackingControl);
				MThumbMouseHandler* thisPtr = static_cast<MThumbMouseHandler*>(down.Get());
				thisPtr->DoTrack(delta);
			}
		}
		
	} catch (...) {
		DEBUGSTR("Caught an exception in MThumbMouseHandler::DoActionProc");
	}
}

	
//---------------------------------------------------------------
//
// MThumbMouseHandler::DoThumbProc						[static]
//
//---------------------------------------------------------------
pascal void	MThumbMouseHandler::DoThumbProc(ControlHandle controlH, ControlPartCode partCode)
{
#if DEBUG
	PRECONDITION(msTrackingControl != nil);
	PRECONDITION(partCode == kControlIndicatorPart);

	IControlPtr control(msTrackingControl);
	PRECONDITION(controlH == control->GetOSControl());
#endif

	try {
		msTrackingControl->DoTrackThumb();
		
	} catch (...) {
		DEBUGSTR("Caught an exception in MThumbMouseHandler::DoThumbProc");
	}
}


}	// namespace Whisper
