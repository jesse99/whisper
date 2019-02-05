/*
 *  File:       MEventDispatcher.cpp
 *  Summary:   	Dispatches toolbox events to the proper objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MEventDispatcher.cpp,v $
 *		Revision 1.18  2001/04/27 09:12:33  jesjones
 *		Nuked the idle code.
 *		
 *		Revision 1.17  2001/04/21 03:25:39  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.16  2001/04/13 07:50:11  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.15  2001/04/03 04:41:13  jesjones
 *		Focus is updated on suspend and resume events.
 *		
 *		Revision 1.14  2001/03/28 09:42:35  jesjones
 *		Exposed UpdateFocus. Saved focus is now static (so it doesn't get messed up when dialogs are up).
 *		
 *		Revision 1.13  2001/03/01 11:41:22  jesjones
 *		Added #if DEBUG to a bit of code.
 *		
 *		Revision 1.12  2001/02/12 09:41:44  jesjones
 *		Changed when DoUpdateFocus is called.
 *		
 *		Revision 1.11  2001/02/08 03:51:38  jesjones
 *		Renamed a bunch of functions.
 *		
 *		Revision 1.10  2001/02/07 02:28:28  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.9  2001/02/04 03:36:51  jesjones
 *		Changed the way mouse events are routed.
 *		
 *		Revision 1.8  2001/01/30 07:20:08  jesjones
 *		Added an icky hack to support IFocused.
 *		
 *		Revision 1.7  2001/01/21 00:27:50  jesjones
 *		Renamed some method in IMenuHandler.
 *		
 *		Revision 1.6  2001/01/05 06:18:51  jesjones
 *		Implementation is no longer exposed.
 *		
 *		Revision 1.5  2000/12/22 09:25:15  jesjones
 *		Idlers and timers are acquired using the app.
 *		
 *		Revision 1.4  2000/12/19 07:50:51  jesjones
 *		Menubar and focus is acquired using the app (so dialogs work).
 *		
 *		Revision 1.3  2000/12/05 09:09:32  jesjones
 *		Moved the declaration into a header. HandleKeyPress routes keys to the key handler even if the target doesn't have a menu handler. Resume and suspend events work if the dispatcher isn't on the app (eg it might be on a dialog handler).
 *		
 *		Revision 1.2  2000/11/09 09:13:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IEventDispatcher.h>

#include <Sound.h>

#include <MProcess.h>
#include <MSystemInfo.h>
#include <IApplication.h>
#include <IControl.h>
#include <ICurrentFocus.h>
#include <IEventExtensible.h>
#include <IFocus.h>
#include <IFocused.h>
#include <IHierarchy.h>
#include <IKeyHandler.h>
#include <IMenuBar.h>
#include <IMenuHandler.h>
#include <IMouseHandler.h>
#include <ITimer.h>
#include <IWindow.h>
#include <IWindowClick.h>
#include <IWindowMgr.h>
#include <XCursor.h>
#include <XKeyEvents.h>
#include <XMouseEvents.h>
#include <XPreference.h>
#include <XRegion.h>
#include <XStateBroadcaster.h>
#include <XStreaming.h>

#if DEBUG
	#include <MetroNubUtils.h>
	#include <Sioux.h>
#endif

namespace Whisper {


// ===================================================================================
//	class ZUnhiliteMenu
// ===================================================================================
class ZUnhiliteMenu {

public:
			 			~ZUnhiliteMenu()				{while (TickCount() < mEndTicks) (void) 0; HiliteMenu(0);}	// $$$Êprobably want to sleep in Carbon
	
  						ZUnhiliteMenu(uint32 delay = 6)	{mEndTicks = TickCount() + delay;}
	
private:
	uint32	mEndTicks;
};


// ===================================================================================
//	class MEventDispatcher
//!		Dispatches toolbox events to the proper objects.
// ===================================================================================
class MEventDispatcher : public IEventDispatcher {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~MEventDispatcher();
	
  						MEventDispatcher(XBoss* boss);
	
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		DispatchEvent(const EventRecord& event);
	virtual void 		UpdateFocus();
	
	virtual void 		HandleTime();
						
	virtual void 		HandleAdjustCursor(const EventRecord& event);
		
#if DEBUG
	virtual bool 		GetZapPort() const				{return *mZapPort;}
	virtual void 		ToggleZapPort()					{*mZapPort = !(*mZapPort);}
#endif

//-----------------------------------
//	Internal API
//
private:	
			void 		DoMouseDown(const EventRecord& event);
			void 		DoMenuClick(const EventRecord& event);
			void 		DoMouseUp(const EventRecord& event);
	
			void 		DoKeyPress(const EventRecord& event);
			void 		DoAutoKey(const EventRecord& event);
			void 		DoKeyRelease(const EventRecord& event);
		
			void 		DoUpdateEvent(const EventRecord& event);
			void 		DoActivateEvent(const EventRecord& event);
	
			void 		DoOSEvent(const EventRecord& event);
			void 		DoHighLevelEvent(const EventRecord& event);
	
			void 		DoResumeEvent(const EventRecord& event);
			void 		DoSuspendEvent(const EventRecord& event);

			void 		DoWindowMouseDown(IWindowPtr window, const EventRecord& event, int16 partCode);
			void 		DoWindowMouseUp(IWindowPtr window, const EventRecord& event);
			bool 		DoWindowAdjustCursor(IWindowPtr window, const EventRecord& event);
	
//-----------------------------------
//	Member Data
//
private:
	XCursor				mDefaultCursor;		// inited to an arrow
	
	bool				mIsSuspended;		// true if we've processed a suspend event (use IsInForeground if you want to know if the app is in the foreground)

	static IFocusPtr	msFocus;			// what we think the focus is
	
#if DEBUG
	XPreference<bool>	mZapPort;
#endif	
};

DEFINE_INTERFACE_FACTORY(MEventDispatcher)
IFocusPtr MEventDispatcher::msFocus;			

//---------------------------------------------------------------
//
// MEventDispatcher::~MEventDispatcher
//
//---------------------------------------------------------------
MEventDispatcher::~MEventDispatcher()
{
}


//---------------------------------------------------------------
//
// MEventDispatcher::MEventDispatcher
//
//---------------------------------------------------------------
MEventDispatcher::MEventDispatcher(XBoss* boss) : mDefaultCursor(kArrowCursor)
#if DEBUG
	,mZapPort(L"Zap Port", false)
#endif
{	
	IEventDispatcher::DoSetBoss(boss);

	mIsSuspended = false;
}


//---------------------------------------------------------------
//
// MEventDispatcher::DispatchEvent
//
// Note that we call SIOUXHandleOneEvent only for events that
// really belong to SIOUX since SIOUXHandleOneEvent has a bad
// tendency to steal things like command-Q.
//
//---------------------------------------------------------------
void MEventDispatcher::DispatchEvent(const EventRecord& inEvent)
{	
	EventRecord event = inEvent;

	bool handled = false;
	IEventExtensiblePtr extensible(this);
	if (extensible)
		handled = extensible->HandleEvent(event);

	if (!handled) {
		switch (event.what) {	
			case mouseDown:
				this->HandleAdjustCursor(event);
				this->DoMouseDown(event);
				break;
				
			case mouseUp:
				this->DoMouseUp(event);
				break;
				
			case keyDown:
				this->DoKeyPress(event);
				break;
				
			case keyUp:
				this->UpdateFocus();
				this->DoKeyRelease(event);
				break;
		
			case autoKey:
				this->UpdateFocus();
				this->DoAutoKey(event);
				break;
				
			case updateEvt:
				this->DoUpdateEvent(event);
				break;
								
			case activateEvt:
				this->DoActivateEvent(event);
				break;
	
			case osEvt:
				this->DoOSEvent(event);
				break;
				
			case kHighLevelEvent:
				this->DoHighLevelEvent(event);
				break;
				
			case nullEvent:
				break;
				
			default:
				DEBUGSTR("Bad event in MEventDispatcher::DispatchEvent");
		}
	}
	
	this->HandleTime();		

	IApplicationPtr app(L"Application");
	if (app->IsQuitting())
		msFocus.Release();
}


//---------------------------------------------------------------
//
// MEventDispatcher::UpdateFocus
//
//---------------------------------------------------------------
void MEventDispatcher::UpdateFocus()
{
	ICurrentFocusPtr current(L"Application");
	if (current) {
		IFocusPtr target = current->GetFocus();
		if (target != msFocus) {						// $$$ hopefully Carbon Events will let us do this without polling...
			if (msFocus) 
				CallRepeatedMethod(msFocus.Get(), &IFocused::OnLostFocus);
					
			msFocus = target;

			if (msFocus)
				CallRepeatedMethod(msFocus.Get(), &IFocused::OnAcquiredFocus);
		}
	}
}


//---------------------------------------------------------------
//
// MEventDispatcher::HandleTime
//
//---------------------------------------------------------------
void MEventDispatcher::HandleTime()
{		
#if DEBUG 
	if (*mZapPort)							
		::SetPort(nil);				// make sure no one is making assumptions about the current port being valid...
#endif
	
	if (MSystemInfo::HasThreadMgr())
		(void) ::YieldToAnyThread();
}


//---------------------------------------------------------------
//
// MEventDispatcher::HandleAdjustCursor
//
//---------------------------------------------------------------
void MEventDispatcher::HandleAdjustCursor(const EventRecord& event)
{
	Point globalPt = event.where;

	WindowRef wptr;
	(void) ::FindWindow(globalPt, &wptr);

	bool handled = false;	
	
	if (wptr != nil) {		
		IWindowPtr window = IWindow::GetWindow(wptr);
		if (window && window->IsActive())
			handled = this->DoWindowAdjustCursor(window, event);
	}
	
	if (!handled)					
		XCursorUtils::SetCursor(mDefaultCursor);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// MEventDispatcher::DoMouseDown
//
//---------------------------------------------------------------
void MEventDispatcher::DoMouseDown(const EventRecord& event)
{
	TRACEFLOW("XEvents", "MEventDispatcher::DoMouseDown\n");
	
	WindowRef wptr;
	int16 partCode = ::FindWindow(event.where, &wptr);
	
	switch (partCode) {
		case inMenuBar:
			this->DoMenuClick(event);
			break;
						
		case inDesk:			
#if DEBUG
			if (!gMonkeyLives)
#endif
				::SysBeep(1);		
			break;				
			
		case inContent:
			{
			IMenuBarPtr menubar(L"Application");
			if (menubar)
				menubar->Invalidate();
			}
			// fall thru
			
		case inGoAway:
		case inDrag:
		case inGrow:
		case inZoomIn:
		case inZoomOut:
#if UNIVERSAL_INTERFACES_VERSION >= 0x0320 
		case inProxyIcon:
#endif
			{
			// Check to see if the user has clicked on a window behind a modal
			// window.
			if (wptr != ::FrontWindow()) {
				IWindowPtr frontWind = IWindow::GetWindow(FrontWindow());
				if (frontWind && frontWind->GetLayer() == kModalLayer) {
					 if (partCode == inDrag && (event.modifiers & cmdKey)) ;
					 	// command-drag is kosher since the windows aren't reordered					 	
					 else
					 	// otherwise change the partcode to reflect the users faux pas
						partCode = kClickOutsideModal;
				}
			}
		
			// Dispatch the click to the appropiate window.
			IWindowPtr window = IWindow::GetWindow(wptr);
			if (window)
				this->DoWindowMouseDown(window, event, partCode);

#if DEBUG	
			else if (wptr == GetSIOUXWindow()) {
//				DEBUGSTR("handling SIOUX click");
				(void) SIOUXHandleOneEvent(const_cast<EventRecord*>(&event));
			}
#endif		
			}
			break;
			
		default:
			DEBUGSTR("MEventDispatcher::DoMouseDown got an unknown part code");
	}
}


//---------------------------------------------------------------
//
// MEventDispatcher::DoMenuClick
//
//---------------------------------------------------------------
void MEventDispatcher::DoMenuClick(const EventRecord& event)
{	
	bool handled = false;
	
	ZUnhiliteMenu unhilite;
	
	IMenuBarPtr menubar(L"Application");

	if (menubar) {
		menubar->Invalidate();				// need to update the menubar in case the user is holding down a modifier key (I used to do this only if a modifier key was down and call Invalidate again after GetCommandFromClick returned but this caused problems if the user held the modifier down after releasing the mouse button)

		IApplicationPtr app(L"Application");
		app->HandleUpdateMenus(event);
				
		XMenuCommand command = menubar->GetCommandFromClick(event);
		
		if (command != kNothingCmd) {
			IMenuHandlerPtr target;

			ICurrentFocusPtr current(L"Application");
			if (current)
				target = GetInterface<IMenuHandler>(current->GetFocus());
				
			if (target) 
				handled = target->HandleCommand(command, event);
		}
		
		if (handled)
			menubar->Invalidate();	
	}
}


//---------------------------------------------------------------
//
// MEventDispatcher::DoMouseUp
//
//---------------------------------------------------------------
void MEventDispatcher::DoMouseUp(const EventRecord& event)
{
	TRACEFLOW("XEvents", "MEventDispatcher::DoMouseUp\n");
	
	WindowRef wptr;
	int16 partCode = ::FindWindow(event.where, &wptr);
	
	if (partCode == inContent && wptr == FrontWindow()) {
		IWindowPtr window = IWindow::GetWindow(wptr);
		if (window)
			this->DoWindowMouseUp(window, event);
	}
}

//---------------------------------------------------------------
//
// MEventDispatcher::DoKeyPress
//
//---------------------------------------------------------------
void MEventDispatcher::DoKeyPress(const EventRecord& inEvent)
{	
	XMenuCommand command = kNothingCmd;
	
	bool handled = false;
	
	ICurrentFocusPtr current(L"Application");
	IFocusPtr target = current->GetFocus();

	IMenuBarPtr menubar(L"Application");
	if (menubar) {
		if ((inEvent.modifiers & cmdKey) != 0) {
			command = menubar->GetCommandFromKey(inEvent);

			if (command != kNothingCmd) {
				ZUnhiliteMenu unhilite;
				
				EventRecord event = inEvent;
				event.modifiers &= ~cmdKey;
				
				menubar->Invalidate();

				IMenuHandlerPtr menuHandler = GetInterface<IMenuHandler>(target);
				if (menuHandler) {
					if (menuHandler->HandleCommand(command, event)) {
						menubar->Invalidate();
						handled = true;
					}
				}
			}	
		} 
	}
		
	if (!handled) {
		IKeyHandlerPtr keyHandler = GetInterface<IKeyHandler>(target);
		if (keyHandler) {
			if (keyHandler->HandleKeyDown(inEvent)) 		// need to do this even if command key is down to handle things like command-arrow key
				if (menubar) 
					menubar->Invalidate();
				
			if (keyHandler->HandleChar(inEvent)) 
				if (menubar) 
					menubar->Invalidate();
		}
	}
}


//---------------------------------------------------------------
//
// MEventDispatcher::DoAutoKey
//
//---------------------------------------------------------------
void MEventDispatcher::DoAutoKey(const EventRecord& event)
{
	ICurrentFocusPtr current(L"Application");
	IKeyHandlerPtr target = GetInterface<IKeyHandler>(current->GetFocus());
		
	if (target) {
		if (target->HandleKeyDown(event)) {
			IMenuBarPtr menubar(L"Application");
			if (menubar)
				menubar->Invalidate();
		}
	}
}


//---------------------------------------------------------------
//
// MEventDispatcher::DoKeyRelease
//
//---------------------------------------------------------------
void MEventDispatcher::DoKeyRelease(const EventRecord& event)
{
	UNUSED(event);
	
#if 0				// disabled because Windows keyup handling is whacked
	IFocusPtr focus(L"Application");
	IFocusPtr target = focus->GetCurrentFocus();
	if (target)
		target->HandleKeyUp(event);
	else
		DEBUGSTR("Target was nil in MEventDispatcher::DoKeyRelease!");
#endif
}


//---------------------------------------------------------------
//
// MEventDispatcher::DoUpdateEvent
//
//---------------------------------------------------------------
void MEventDispatcher::DoUpdateEvent(const EventRecord& event)
{
	WindowRef wptr = reinterpret_cast<WindowRef>(event.message);
	
	IWindowPtr window = IWindow::GetWindow(wptr);
	if (window)
		window->HandleUpdate();

#if DEBUG
	else if (wptr == GetSIOUXWindow())
		(void) SIOUXHandleOneEvent(const_cast<EventRecord*>(&event));
#endif
}


//---------------------------------------------------------------
//
// MEventDispatcher::DoActivateEvent
//
//---------------------------------------------------------------
void MEventDispatcher::DoActivateEvent(const EventRecord& event)
{	
	WindowRef wptr = reinterpret_cast<WindowRef>(event.message);

	IWindowPtr window = IWindow::GetWindow(wptr);
	if (window) {
		if (event.modifiers & activeFlag) {
			TRACEFLOW("XEvents", "MEventDispatcher::DoActivateEvent\n");
	
			IMenuBarPtr menubar(L"Application");
			if (menubar)
				menubar->Invalidate();
			
			window->HandleActivate();
			
		} else {
			TRACEFLOW("XEvents", "MEventDispatcher::HandleDeactivateEvent\n");
			
			window->HandleDeactivate();
		}
	} 
#if DEBUG
	else if (wptr == GetSIOUXWindow())
		(void) SIOUXHandleOneEvent(const_cast<EventRecord*>(&event));
#endif
}


//---------------------------------------------------------------
//
// MEventDispatcher::DoOSEvent
//
//---------------------------------------------------------------
void MEventDispatcher::DoOSEvent(const EventRecord& event)
{
	uint8 osEvtFlag = (uint8) (((uint32) event.message) >> 24);
	
	if (osEvtFlag == mouseMovedMessage && MProcess::InFront()) {
		this->HandleAdjustCursor(event);
//		XMouseEnterMixin::HandleMouse(event);	// $$$$
		
	} else if (osEvtFlag == suspendResumeMessage) {
		if (event.message & resumeFlag)
			this->DoResumeEvent(event);
		else
			this->DoSuspendEvent(event);
	}
}


//---------------------------------------------------------------
//
// MEventDispatcher::DoResumeEvent
//
//---------------------------------------------------------------
void MEventDispatcher::DoResumeEvent(const EventRecord& event)
{	
	TRACEFLOW("XEvents", "MEventDispatcher::DoResumeEvent\n");
	
	// After Dark 3.0 doesn't give me a suspend event when it kicks
	// in. However it does give me a resume event when it goes away.
	// To ensure that things work correctly we'll fake a suspend
	// event and then process the resume event.
	if (!mIsSuspended) {
#if DEBUG && !TARGET_CARBON							// $$$ AmIBeingMWDebugged crashes?
		if (!AmIBeingMWDebugged())					// debugger often makes this fire
			TRACEFLOW("XWarning", "Got a resume event without being suspended!\n");
#endif
		this->DoSuspendEvent(event);
	}
	
	XStateBroadcaster::Instance()->Broadcast(kResumingApp);
	
	IWindowMgrPtr windowMgr(L"Application");
	windowMgr->Resume();

	IMenuBarPtr menubar(L"Application");
	if (menubar)
		menubar->Invalidate();
				
	mIsSuspended = false;
	this->UpdateFocus();
}


//---------------------------------------------------------------
//
// MEventDispatcher::DoSuspendEvent
//
//---------------------------------------------------------------
void MEventDispatcher::DoSuspendEvent(const EventRecord& event)
{
	UNUSED(event);
	
	TRACEFLOW("XEvents", "MEventDispatcher::DoSuspendEvent\n");
		
	XStateBroadcaster::Instance()->Broadcast(kSuspendingApp);
	
	IWindowMgrPtr windowMgr(L"Application");
	windowMgr->Suspend();

	mIsSuspended = true;
	this->UpdateFocus();
}


//---------------------------------------------------------------
//
// MEventDispatcher::DoHighLevelEvent
//
//---------------------------------------------------------------
void MEventDispatcher::DoHighLevelEvent(const EventRecord& event)
{
	TRACEFLOW("XEvents", "MEventDispatcher::DoHighLevelEvent\n");
	
	IMenuBarPtr menubar(L"Application");
	if (menubar)
		menubar->Invalidate();
	
	OSErr err = ::AEProcessAppleEvent(&event);
	ASSERT(err == noErr || err == errAEEventNotHandled);
}


//---------------------------------------------------------------
//
// MEventDispatcher::DoWindowMouseDown
//
//---------------------------------------------------------------
void MEventDispatcher::DoWindowMouseDown(IWindowPtr window, const EventRecord& event, int16 partCode)
{	
	bool handled = false;
	
	XBoss* boss = window->GetBoss();
	for (XBoss::iterator iter = boss->begin(); iter != boss->end() && !handled; ++iter) {
		IWindowClickPtr click(boss, iter);
		
		if (click)
			handled = click->OnClick(event, partCode);
	}
	
	POSTCONDITION(handled);
}


//---------------------------------------------------------------
//
// MEventDispatcher::DoWindowMouseUp
//
//---------------------------------------------------------------
void MEventDispatcher::DoWindowMouseUp(IWindowPtr window, const EventRecord& inEvent)
{	
	bool handled = false;
	
	XMouseUpEvent event = inEvent;
	XPoint pt = window->GlobalToWindow(event.GetPosition());
	event.SetPosition(pt);

	IControlPtr control = window->FindControl(event.GetPosition());
	if (control) {	
		XMouseUpEvent childEvent = event;
		XPoint pos = control->WindowToLocal(event.GetPosition());
		childEvent.SetPosition(pos);
	
		IMouseHandlerPtr handler(control);
		if (handler)
			handled = handler->HandleMouseUp(childEvent);
	}
	
	if (!handled) {
		IMouseHandlerPtr handler(window);
		if (handler) 
			handled = handler->HandleMouseUp(event);
	}
}


//---------------------------------------------------------------
//
// MEventDispatcher::DoWindowAdjustCursor
//
//---------------------------------------------------------------
bool MEventDispatcher::DoWindowAdjustCursor(IWindowPtr window, const EventRecord& inEvent)
{	
	bool handled = false;
	
	XMouseMoveEvent event = inEvent;
	XPoint pt = window->GlobalToWindow(event.GetPosition());
	event.SetPosition(pt);

	IControlPtr control = window->FindControl(event.GetPosition());
	if (control) {	
		XMouseMoveEvent childEvent = event;
		XPoint pos = control->WindowToLocal(event.GetPosition());
		childEvent.SetPosition(pos);
	
		IMouseHandlerPtr handler(control);
		if (handler) {
			(void) handler->HandleHelp(childEvent);
			handled = handler->HandleAdjustCursor(childEvent);
		}
	}
	
	if (!handled) {
		IMouseHandlerPtr handler(window);
		if (handler) {
			(void) handler->HandleHelp(event);
			handled = handler->HandleAdjustCursor(event);
		}
	}
	
	return handled;
}


}	// namespace Whisper
