/*
 *  File:       MAppEventLoop.cpp
 *  Summary:   	Main event loop.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MAppEventLoop.cpp,v $
 *		Revision 1.8  2001/04/27 09:10:56  jesjones
 *		Got rid of the processor hogging code (don't need this now that we use Carbon timers).
 *		
 *		Revision 1.7  2001/04/21 03:23:46  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/04/13 07:48:34  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.5  2001/03/27 06:33:27  jesjones
 *		No longer updates menus if we're quitting.
 *		
 *		Revision 1.4  2001/03/26 04:23:03  jesjones
 *		No longer updates menus if we're quitting.
 *		
 *		Revision 1.3  2001/01/05 06:17:39  jesjones
 *		Implementation is no longer exposed.
 *		
 *		Revision 1.2  2000/11/09 09:09:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IEventLoop.h>

#include <IActiveMenuBar.h>
#include <IApplication.h>
#include <ICommandQueue.h>
#include <IEventDispatcher.h>
#include <IMenuBar.h>
#include <ITimer.h>
#include <MProcess.h>
#include <XError.h>
#include <XEvent.h>
#include <XRegion.h>
#include <XStringUtils.h>
#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// EventPending
//
//---------------------------------------------------------------
static bool EventPending(EventRecord* event)
{
	PRECONDITION(event != nil);
	
	bool found = false;
	found = EventAvail(everyEvent, event);	

	if (!found)
		found = CheckUpdate(event);		// update events aren't queued...

	return found;
}

#pragma mark -

// ===================================================================================
//	class XAppEventLoop
//!		Main event loop.
// ===================================================================================
class XAppEventLoop : public IEventLoop {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XAppEventLoop();
	
  						XAppEventLoop(XBoss* boss);

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Run();

	virtual void 		SetErrHandler(ErrHandler handler)	{mErrHandler = handler;}

//-----------------------------------
//	Internal API
//
private:
	virtual void 		DoException(const std::exception& e);
						
	virtual void 		DoNextEvent();

//-----------------------------------
//	Member Data
//
private:	
	uint16		mEventMask;
	ErrHandler	mErrHandler;
};

DEFINE_INTERFACE_FACTORY(XAppEventLoop)

//---------------------------------------------------------------
//
// XAppEventLoop::~XAppEventLoop
//
//---------------------------------------------------------------
XAppEventLoop::~XAppEventLoop()
{
}


//---------------------------------------------------------------
//
// XAppEventLoop::XAppEventLoop
//
//---------------------------------------------------------------
XAppEventLoop::XAppEventLoop(XBoss* boss) 
{	
	IEventLoop::DoSetBoss(boss);

	mEventMask = everyEvent;
}


//---------------------------------------------------------------
//
// XAppEventLoop::Run
//
//---------------------------------------------------------------
void XAppEventLoop::Run()
{
	IApplicationPtr app(this);
	IEventDispatcherPtr dispatcher(this);
	ICommandQueuePtr commands(this);
	IActiveMenuBarPtr activeMenubar(this);
	IMenuBarPtr menubar;
	ITimerPtr timer(this);
	if (activeMenubar)
		menubar = activeMenubar->Get();

	while (!app->IsQuitting()) {
		try {			
			// Call WaitNextEvent to process whatever's pending. If we
			// get a null, mouse moved, or update event then we're still 
			// idling.
			this->DoNextEvent();	
			
		} catch (const std::exception& e) {
			this->DoException(e);

		} catch (...) {
			this->DoException(std::runtime_error(ToUTF8Str(LoadWhisperString(L"Unknown error"))));
		}
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XAppEventLoop::DoNextEvent
//
//---------------------------------------------------------------
void XAppEventLoop::DoNextEvent()
{
	IApplicationPtr app(this);
	
	uint32 sleep = 10*60UL;

	EventRecord event;
	static XRegion mouseRgn(XRect(0, 0, 1, 1));
	bool gotEvent = ::WaitNextEvent(mEventMask, &event, sleep, mouseRgn);
	
	IEventDispatcherPtr dispatcher(this);
	dispatcher->DispatchEvent(event);
		
	// Note that it's important that commands be executed only while
	// the app is in the foreground. For example, an app that pops
	// up a dialog after accepting a drag has to wait until the app
	// is in the forefround. It can do this by calling SetFrontProcess
	// and then posting a command to display the dialog.
	IMenuBarPtr menubar(this);
	ICommandQueuePtr commands(this);

	bool inFront = MProcess::InFront();
	if (inFront && commands && commands->ExecuteCommands() && menubar)
		menubar->Invalidate();
	
	// Update the menus (we do this here to ensure menu titles are redrawn in gray
	// when the menu doesn't have any enabled items).
	if (inFront && menubar && !menubar->IsValid() && !app->IsQuitting())
		app->HandleUpdateMenus(event);
}


//---------------------------------------------------------------
//
// XAppEventLoop::DoException
//
//---------------------------------------------------------------
void XAppEventLoop::DoException(const std::exception& e)
{
	if (mErrHandler.IsValid())
		mErrHandler(e);
	else
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't complete the operation."), e);
}


}	// namespace Whisper

