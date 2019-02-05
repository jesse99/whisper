/*
 *  File:       MApplication.cpp
 *  Summary:   	The application object.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MApplication.cpp,v $
 *		Revision 1.18  2002/01/29 06:51:16  jesjones
 *		Disabled a couple of asserts that are firing on OS X.
 *		
 *		Revision 1.17  2001/04/27 09:11:39  jesjones
 *		Nuked Get and SetSleepTime and the idler code.
 *		
 *		Revision 1.16  2001/04/21 03:23:56  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.15  2001/04/13 07:48:41  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.14  2001/03/28 09:41:32  jesjones
 *		HandleUpdateMenus calls UpdateFocus.
 *		
 *		Revision 1.13  2001/03/27 06:34:06  jesjones
 *		Hook cleanup methods are now called in DoQuit instead of Quit.
 *		
 *		Revision 1.12  2001/02/17 10:23:53  jesjones
 *		IStringTableLoader::Load takes an XResource instead of an XURI.
 *		
 *		Revision 1.11  2001/02/07 02:26:13  jesjones
 *		Uses CallRepeatedMethods with IInit.
 *		
 *		Revision 1.10  2001/01/30 07:16:45  jesjones
 *		Uses CallRepeatedMethods.
 *		
 *		Revision 1.9  2001/01/22 01:59:41  jesjones
 *		Nuked the menu extensions.
 *		
 *		Revision 1.8  2001/01/21 00:25:42  jesjones
 *		Worked on menu code. String loaders no longer throw if the string table is missing.
 *		
 *		Revision 1.7  2001/01/05 06:18:02  jesjones
 *		Implementation is no longer exposed.
 *		
 *		Revision 1.6  2000/12/31 04:20:10  jesjones
 *		Removed DisplayAboutBox.
 *		
 *		Revision 1.5  2000/11/27 07:15:01  jesjones
 *		Can't use nil to construct an interface pointer.
 *		
 *		Revision 1.4  2000/11/26 03:29:01  jesjones
 *		Added an Init method.
 *		
 *		Revision 1.3  2000/11/09 09:09:54  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <25>	11/05/00	JDJ		Added an IEventLoop interface.
 *		 <24>	10/18/00	JDJ		Replaced OnInit and OnQuit with an IInit interface.
 *		 <23>	 7/30/00	JDJ		HandleInit inits IActiveMenuBar.
 *		 <22>	12/21/99	JDJ		Removed the app and Whisper data folder specs.
 *		 <21>	12/11/99	JDJ		GetDataFolder saves aliases's instead of paths.
 *		 <20>	11/20/99	JDJ		XObjectHeapExtension is only installed if WHISPER_OPERATOR_NEW is defined.
 *		 <19>	10/02/99	JDJ		Renamed WhisperStrings.xml XStrings.xml and AppStrings.xml CStrings.xml.
 *		 <18>	 8/10/99	JDJ		Removed mGlobalUndoContext.
 *		 <17>	 7/23/99	JDJ		DoAEIdle, DoAEFilter, and DoAEQuitApp trap exceptions.
 *		 <16>	 7/19/99	JDJ		Tweaked for Carbon.
 *		 <15>	 7/18/99	JDJ		OnBroadcast calls XWindow::IsOpened.
 *		 <14>	 7/14/99	JDJ		Added OnGetMenuBarURI.
 *		 <13>	 7/14/99	JDJ		Only parses the string table in debug builds.
 *		 <12>	 7/05/99	JDJ		Added XFlattenWindowExtension.
 *		 <11>	 6/19/99	JDJ		DoReportError uses \r instead of \n.
 *		 <10>	 6/02/99	JDJ		HandleQuit does nothing if the app is already quitting.
 *		 <9>	 6/02/99	JDJ		It's now OK for OnCreateDefaultMenuBar to return nil.
 *		 <8>	 5/12/99	JDJ		Added OnPostInit.
 *		 <7>	 5/03/99	JDJ		Removed OnOpenWithoutDoc.
 *		 <6>	 5/03/99	JDJ		HandleInit calls XFileDialog::SetModalFilter.
 *		 <5>	 3/19/99	JDJ		ParseXMLStringTable takes a URI instead of a name.
 *		 <4>	 2/16/99	JDJ		Installs a string table loader. Added locale_begin and locale_end
 *									for iterating over locales supported by the app.
 *		 <3>	 1/15/99	JDJ		Replaced hogging processor flag with sleep time.
 *		 <2>	 1/06/99	JDJ		Added MSystemInfoExtension.
 *		 <1>	10/16/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <IApplication.h>

#include <Sound.h>

#include <IActiveMenuBar.h>
#include <ICommandQueue.h>
#include <ICommand.h>
#include <ICommands.h>
#include <IEventDispatcher.h>
#include <IEventLoop.h>
#include <IHierarchy.h>
#include <IInit.h>
#include <IInitMenuBar.h>
#include <IInstallMenuHandler.h>
#include <IMenu.h>
#include <IMenuBar.h>
#include <IMenuHandler.h>
#include <IStringTableLoader.h>
#include <ITimer.h>
#include <IWindow.h>
#include <IWindowMgr.h>
#include <MAppleEvent.h>
#include <MDialogUtils.h>
#include <MNotify.h>
#include <MProcess.h>
#include <MSystemInfo.h>
#include <SLFunctions.h>
#include <XCursor.h>
#include <XError.h>
#include <XEvent.h>
#include <XExceptions.h>
#include <XFileDialog.h>
#include <XLocker.h>
#include <XRegion.h>
#include <XResource.h>
#include <XTrace.h>
#include <XURI.h>
#include <XURIResolver.h>

namespace Whisper {


//-----------------------------------
//	Types
//
#pragma options align=mac68k
struct SKindRecord {
	OSType			appSignature;
	int16			regionCode;
	int16 			filler;
	int16 			numKinds;
	OSType			fileType;
	unsigned char 	fileStr[1];
};
#pragma options align=reset


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetAppName
//
//---------------------------------------------------------------
static std::wstring GetAppName()
{
	std::wstring name = L"";
	
	SLDisable();
	Handle hand = Get1IndResource('kind', 1);
	SLEnable();
	
	if (hand != nil) {
		SKindRecord* data = reinterpret_cast<SKindRecord*>(*hand);
		if (data->fileType == 'apnm') {
			XLocker lock(hand);
			
			name = FromPascalStr(data->fileStr);
			ASSERT(name.length() > 0);
			
		} else
			DEBUGSTR("First kind string should be the app's name!");
			
		ReleaseResource(hand);
	}
	
	if (name.length() == 0) 
		name = FromPascalStr(MSystemInfo::GetAppSpec().name);
		
	return name;
}

#pragma mark -

// ===================================================================================
//	class XApplication
//!		The application object.
// ===================================================================================
class XApplication : public IApplication, public XListenerMixin<SStateMessage> {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XApplication();
	
  						XApplication(XBoss* boss);

//-----------------------------------
//	Inherited API
//
public:
 	virtual void 		Init();

	virtual void 		Run();
												
	virtual void 		Quit();
	
	virtual bool 		IsQuitting() const;
					
 	virtual std::wstring GetName() const;

 	virtual bool 		IsMemoryLow() const;
 							
	virtual void		HandleUpdateMenus(const XEvent& event);

protected:
	virtual void 		Invariant() const;
							
	virtual void 		OnBroadcast(const SStateMessage& message);
	
//-----------------------------------
//	Internal API
//
private:
	virtual void 		DoQuit();
	
	virtual void 		DoInstallAEHandlers();	

	virtual bool 		DoUpdateMenu(IMenuPtr menu, const XEvent& event);

	static	void 		DoDeferError(const std::wstring& summary, const std::wstring& narrative);
	
	static 	void 		DoLoadWhisperStrings(const XLocale& locale, StringTable& table);
	static 	void 		DoLoadAppStrings(const XLocale& locale, StringTable& table);

	static	void 		DoReportError(const std::wstring& summary, const std::wstring& narrative);

	static pascal Boolean DoAEIdle(EventRecord* event, int32* sleepTime, RgnHandle* mouseRgn);

	static pascal Boolean DoAEFilter(EventRecord* event, int32 returnID, int32 transactionID, const AEAddressDesc* sender);

	static pascal OSErr DoAEQuitApp(const AppleEvent* event, AppleEvent* reply, int32 refCon);

	static 	bool 		DoDialogFilter(EventRecord* event);
						/**< This is called by the default dialog filter (see MDialogUtils).
						It handles update, activate, deactivate events and gives time
						to idlers and timers. */
	
	static	void 		DoNavServicesEvent(const EventRecord& event);

//-----------------------------------
//	Member Data
//
private:	
	std::wstring			mName;					
	bool					mQuitting;
	bool					mInited;
	
	uint32					mNumDocWindows;
	
	XURI					mWhisperStrings;	// defaults to "resource://XStrings.bXML"
	XURI					mAppStrings;		// defaults to "resource://Strings.bXML"
	
	static XApplication*	msInstance;
};

DEFINE_INTERFACE_FACTORY(XApplication)

XApplication* XApplication::msInstance = nil;

//---------------------------------------------------------------
//
// XApplication::~XApplication
//
//---------------------------------------------------------------
XApplication::~XApplication()
{
	CALL_INVARIANT;
	PRECONDITION(mNumDocWindows == 0);
	
	TRACEFLOW("XApplication", "XApplication::~XApplication\n");
	
	msInstance = nil;
			
	XFileDialog::SetEventCallback(nil);
	XFileDialog::SetModalFilter(nil);
}


//---------------------------------------------------------------
//
// XApplication::XApplication
//
//---------------------------------------------------------------
XApplication::XApplication(XBoss* boss) : mWhisperStrings(L"resource://XStrings.bXML"), mAppStrings(L"resource://Strings.bXML")
{
//	REQUIRE((MSystemInfo::GetAppSizeFlags() & modeNeedSuspendResume) != 0);			// we expect suspend and resume events $$$ should do these two but there seems to be a bug in OS 10.1.2 $$$
//	REQUIRE((MSystemInfo::GetAppSizeFlags() & modeDoesActivateOnFGSwitch) != 0);	// we activate windows when we're switched in
	REQUIRE((MSystemInfo::GetAppSizeFlags() & modeHighLevelEventAware) != 0);		// we support the core apple events
			
	this->DoSetBoss(boss);

#if DEBUG
	// New TRACEFLOW categories are on by default. Here we'll 
	// turn off the most common categories since they're usually 
	// just an annoyance. Note that XTraceExtension will restore
	// the saved values when it is created.
	XTrace::Instance()->Enable("XApplication", false);	
	XTrace::Instance()->Enable("XControl", false);
	XTrace::Instance()->Enable("XEvents", false);
	XTrace::Instance()->Enable("XFocus", false);
//	XTrace::Instance()->Enable("XGarbage", true);		
	XTrace::Instance()->Enable("XMenuHandler", false);
	XTrace::Instance()->Enable("XPersistXML", false);
	XTrace::Instance()->Enable("XStateBroadcaster", false);
	XTrace::Instance()->Enable("XWindow", false);
#endif

	mName = Whisper::GetAppName();
	mQuitting = false;
	mInited = false;
		
	mNumDocWindows = 0;
	XStateBroadcaster::Instance()->AddListener(this);

	msInstance = this;

//	CALL_INVARIANT;				// wait till Init is called	
}
	

//---------------------------------------------------------------
//
// XApplication::Init
//
//---------------------------------------------------------------
void XApplication::Init()
{	
	PRECONDITION(!mInited);

	try {
		XError::Instance()->SetErrorHandler(XApplication::DoReportError);
		XError::Instance()->SetDeferredErrorHandler(XApplication::DoDeferError);
			
		IStringTableLoaderPtr loader(this);
		if (loader) {
			Whisper::InstallWhisperLoader(XApplication::DoLoadWhisperStrings);
			Whisper::InstallAppLoader(XApplication::DoLoadAppStrings);
		}
	
		this->DoInstallAEHandlers();
		
		Whisper::AddEventProc(DoDialogFilter);
	
		XFileDialog::SetEventCallback(XApplication::DoNavServicesEvent);
		XFileDialog::SetModalFilter(Whisper::DefaultFilter);

		MAppleEvent::msIdleProc = NewAEIdleUPP(XApplication::DoAEIdle);

		IInitMenuBarPtr menuInit(this);
		if (menuInit)
			menuInit->Init(nil);
		
		IActiveMenuBarPtr activeMenuBar(this);
		IMenuBarPtr menubar(this);
		if (menubar)
			activeMenuBar->Set(menubar);
			
		CallRepeatedMethod(this, &IInit::OnInit);
		CallRepeatedMethod(this, &IInstallMenuHandler::InstallCommands);
			
		mInited = true;
				
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't initialize the application."), e);
		ExitToShell();

	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't initialize the application."));
		ExitToShell();
	}		
	
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XApplication::Run
//
//---------------------------------------------------------------
void XApplication::Run()
{
	PRECONDITION(mInited);					
	CHECK_INVARIANT;
		
	TRACEFLOW("XApplication", "XApplication::Run\n");
	
	XStateBroadcaster::Instance()->Broadcast(kStartingApp);

	XCursorUtils::Init();
	
	IEventLoopPtr loop(this);
	loop->Run();	

	XStateBroadcaster::Instance()->Broadcast(kQuittingApp);

	ICommandQueuePtr commands(this);
	if (commands)
		commands->ExecuteCommands();		// give commands a chance to execute

	this->DoQuit();

	POSTCONDITION(mQuitting);
}


//---------------------------------------------------------------
//
// XApplication::Quit
//
//---------------------------------------------------------------
void XApplication::Quit()
{
	CALL_INVARIANT;

	if (!mQuitting) {
		TRACEFLOW("XApplication", "XApplication::Quit\n");
		
		// Don't quit if there's a modal up.
		IWindowMgrPtr windowMgr(this);
		mQuitting = windowMgr->GetTopModal() == nil;		
		
		// Close all the regular windows. If one of the can't be closed
		// (eg user canceled out of save changes dialog) then abort quit.
		XWindowIter iter = windowMgr->begin(false);
		while (mQuitting && iter != windowMgr->end()) {
			IWindowPtr window = *iter;
			++iter;
			
			if (window->GetLayer() == kRegularLayer)
				mQuitting = window->Close(true);
		}
		
		// If all the regular windows have been closed go ahead and
		// close the floaters (we do this here so that the users
		// floaters won't be disturbed if he cancels out of the
		// save dialog).
		iter = windowMgr->begin(false);
		while (mQuitting && iter != windowMgr->end()) {
			IWindowPtr window = *iter;
			++iter;
			
			mQuitting = window->Close(true);
		}
	}
}


//---------------------------------------------------------------
//
// XApplication::IsQuitting
//
//---------------------------------------------------------------
bool XApplication::IsQuitting() const
{
	return mQuitting;
}


//---------------------------------------------------------------
//
// XApplication::GetName
//
//---------------------------------------------------------------
std::wstring XApplication::GetName() const
{
	return mName;
}


//---------------------------------------------------------------
//
// XApplication::IsMemoryLow
//
//---------------------------------------------------------------
bool XApplication::IsMemoryLow() const
{
	return false;		// $$$ need to implement this, perhaps with a new interface?
}


//---------------------------------------------------------------
//
// XApplication::HandleUpdateMenus
//
//---------------------------------------------------------------
void XApplication::HandleUpdateMenus(const XEvent& event)
{
	CHECK_INVARIANT;
	
	IEventDispatcherPtr dispatcher(this);
	dispatcher->UpdateFocus();
		
	IActiveMenuBarPtr activeMenuBar(L"Application");
	IMenuBarPtr menubar = activeMenuBar->Get();

	if (!menubar->IsValid()) {
		uint32 count = menubar->GetCount();
		for (uint32 index = 0; index < count; index++) {
			IMenuPtr menu = menubar->GetMenuByIndex(index);
			
			bool enable = this->DoUpdateMenu(menu, event);
		
#if MAC							// $$$ 1.0d10c3 CarbonLib doesn't appear to have kMenuAttrAutoDisable implemented
//#if MAC && !TARGET_CARBON
			// Don't want to do this on Windows because the menu
			// won't pop down. On Carbon the OS will automatically
			// disable the menu.
			menu->Enable(enable);					
#endif
		}	
		
		menubar->Validate();
	}
}

#if __MERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XApplication::Invariant
//
//---------------------------------------------------------------
void XApplication::Invariant() const
{
	IApplication::Invariant();
}


//---------------------------------------------------------------
//
// XApplication::OnBroadcast
//
//---------------------------------------------------------------
void XApplication::OnBroadcast(const SStateMessage& mesg)
{	
	if (mesg.mesg == kOpenedWindow) {
		const IWindow* window = mesg.data.Get<const IWindow*>();
		
		if (window->GetType() == kDocumentWindow) {
			++mNumDocWindows;
			ASSERT(mNumDocWindows > 0);
		}
		
	} else if (mesg.mesg == kClosingWindow) {
		const IWindow* window = mesg.data.Get<const IWindow*>();
		
		if (window->GetType() == kDocumentWindow && window->IsOpened()) {
			if (--mNumDocWindows == 0)
				ASSERT(mNumDocWindows >= 0);
		}
	}
}

#if __MERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XApplication::DoQuit
//
//---------------------------------------------------------------
void XApplication::DoQuit()
{		
	CallRepeatedMethod(this, &IInstallMenuHandler::RemoveCommands);		
	CallRepeatedMethod(this, &IInit::OnCleanup);

	IHierarchyPtr node(this);
	node->RemoveChildren();

	IActiveMenuBarPtr activeMenuBar(this);
	if (activeMenuBar) {
		IMenuBar* nilBar = nil;
		activeMenuBar->Set(nilBar);		
	}

	IMenuBarPtr menubar(this);
	if (menubar)
		menubar->RemoveMenus();
}


//---------------------------------------------------------------
//
// XApplication::DoInstallAEHandlers
//
//---------------------------------------------------------------
void XApplication::DoInstallAEHandlers()
{
	OSErr 				err;
	AEEventHandlerUPP	handler;
	
	handler = NewAEEventHandlerUPP(DoAEQuitApp);
	err = AEInstallEventHandler(kCoreEventClass, kAEQuitApplication, handler, 0, false);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// XApplication::DoUpdateMenu
//
//---------------------------------------------------------------
bool XApplication::DoUpdateMenu(IMenuPtr menu, const XEvent& event)
{	
	// The menu will only be enabled if at least one menu item is enabled.
	bool enableMenu = false;
	
	// Loop through each item in the menu.
	IMenuHandlerPtr handler(this);
		
	uint32 count = menu->GetCount();
	for (uint32 index = 0; index < count; ++index) {
		SCommandStatus status;

		if (menu->IsSubMenu(index)) {
			IMenuPtr subMenu = menu->GetSubMenu(index);
			status.enabled = this->DoUpdateMenu(subMenu, event);

			menu->EnableItem(index, status.enabled);	
			
		} else {		
			XMenuCommand command = menu->GetCommand(index);
			if (command != kNothingCmd) {
				
				// Ask the command handlers for the status of the command 
				// associated with the menu item.
				(void) handler->HandleEnableCommand(command, status, event);
				
				// Update the menu item to reflect its new status.
				menu->EnableItem(index, status.enabled);	
				menu->CheckItem(index, status.checkmark);
#if MAC
				if (status.style != 0)
					menu->SetStyle(index, status.style);
#endif										
				if (status.name.length() > 0)
					menu->SetName(index, status.name);
			}
		}

		// If the menu item is enabled we need to enable the menu.
		if (status.enabled)
			enableMenu = true;
	}
		
	return enableMenu;					
}


//---------------------------------------------------------------
//
// XApplication::DoLoadWhisperStrings					[static]
//
//---------------------------------------------------------------
void XApplication::DoLoadWhisperStrings(const XLocale& locale, StringTable& table)
{
	if (msInstance->mWhisperStrings.GetPath().length() > 0) {
		XAutoPtr<XResource> data(XURIResolver::Instance()->Resolve(msInstance->mWhisperStrings));
		if (data.Get() != nil) {
			IStringTableLoaderPtr loader(L"Application");
			loader->Load(*data, locale, table);	
		}
	}
}


//---------------------------------------------------------------
//
// XApplication::DoLoadAppStrings						[static]
//
//---------------------------------------------------------------
void XApplication::DoLoadAppStrings(const XLocale& locale, StringTable& table)
{
	if (msInstance->mAppStrings.GetPath().length() > 0) {
		XAutoPtr<XResource> data(XURIResolver::Instance()->Resolve(msInstance->mAppStrings));
		if (data.Get() != nil) {
			IStringTableLoaderPtr loader(L"Application");
			loader->Load(*data, locale, table);
		}
	}
}


//---------------------------------------------------------------
//
// XApplication::DoReportError							[static]
//
//---------------------------------------------------------------
void XApplication::DoReportError(const std::wstring& summary, const std::wstring& narrative)
{
	if (!MProcess::InFront()) {	
		MNotify* note = new MNotify(summary + L"\r" + narrative);
		note->Post();
	
	} else
		XError::Instance()->DefaultErrorHandler(summary, narrative);
}


//---------------------------------------------------------------
//
// XApplication::DoDeferError							[static]
//
//---------------------------------------------------------------
void XApplication::DoDeferError(const std::wstring& summary, const std::wstring& narrative)
{
	ICommandPtr command(L"Deferred Error Command");
	IDeferredErrorCommandPtr error(command);
	error->Init(summary, narrative);
	
	ICommandQueuePtr queue(L"Application");
	queue->Post(command);
}


//---------------------------------------------------------------
//
// XApplication::DoAEIdle								[static]
//
//---------------------------------------------------------------
pascal Boolean XApplication::DoAEIdle(EventRecord* event, int32* sleepTime, RgnHandle* mouseRgn)
{	
	UNUSED(sleepTime);
	
	bool abort = false;
	
	try {
		IApplicationPtr app(L"Application");
		IEventDispatcherPtr dispatcher(app);
					
		if (mouseRgn != nil && *mouseRgn != nil) {
			static XRegion rgn(XRect(0, 0, 1, 1));
			CopyRgn(rgn, *mouseRgn);
		}
			
		if (event != nil)
			dispatcher->DispatchEvent(*event);
			
		XCursorUtils::StillBusy();
	
	} catch (...) {
		DEBUGSTR("Caught an exception in XApplication::DoAEIdle");
	}
			
	return abort;
}


//---------------------------------------------------------------
//
// XApplication::DoAEQuitApp							[static]
//
//---------------------------------------------------------------
pascal OSErr XApplication::DoAEQuitApp(const AppleEvent* event, AppleEvent* reply, int32 refCon)
{
	#pragma unused(event, reply, refCon)
	
	try {
		IApplicationPtr app(L"Application");
		app->Quit();
	
	} catch (...) {
		DEBUGSTR("Caught an exception in XApplication::DoAEQuitApp");
	}
	
	return noErr;
}


//---------------------------------------------------------------
//
// XApplication::DoDialogFilter							[static]
//
//---------------------------------------------------------------
bool XApplication::DoDialogFilter(EventRecord* event)
{			
	bool handled = false;					// Note that returning true dismisses the dialog.
	
	try {
		IEventDispatcherPtr dispatcher(L"Application");
	
		if (event->what == updateEvt) {
			WindowRef wptr = reinterpret_cast<WindowRef>(event->message);
			IWindowPtr wind = IWindow::GetWindow(wptr);
			if (wptr != nil && wind)
				dispatcher->DispatchEvent(*event);
					
		} else if (event->what == activateEvt) {
			WindowRef wptr = reinterpret_cast<WindowRef>(event->message);
			IWindowPtr wind = IWindow::GetWindow(wptr);
			if (wptr != nil && wind)
				dispatcher->DispatchEvent(*event);
		}
		
	} catch (...) {
		DEBUGSTR("Caught an exception in XApplication::DoDialogFilter");
	}

	return handled;
}


//---------------------------------------------------------------
//
// XApplication::DoNavServicesEvent						[static]
//
//---------------------------------------------------------------
void XApplication::DoNavServicesEvent(const EventRecord& event)
{	
	try {
		IEventDispatcherPtr dispatcher(L"Application");
		dispatcher->DispatchEvent(event);

	} catch (...) {
		DEBUGSTR("Caught an exception in XApplication::DoNavServicesEvent");
	}
}


}	// namespace Whisper
