/*
 *  File:       WApplication.cpp
 *  Summary:   	The application object.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998--2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WApplication.cpp,v $
 *		Revision 1.16  2001/04/27 09:14:39  jesjones
 *		Nuked the idle code.
 *		
 *		Revision 1.15  2001/04/21 03:30:10  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.14  2001/02/25 08:24:47  jesjones
 *		DoUpdateMenu disables everything if a modal dialog is up.
 *		
 *		Revision 1.13  2001/02/21 08:15:18  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.12  2001/02/07 02:26:52  jesjones
 *		Uses CallRepeatedMethod with IInit.
 *		
 *		Revision 1.11  2001/01/30 07:21:38  jesjones
 *		Calls IInstallCommands.
 *		
 *		Revision 1.10  2001/01/21 00:41:27  jesjones
 *		Changed the menu code.
 *		
 *		Revision 1.9  2001/01/05 06:22:20  jesjones
 *		Implementation is no longer exposed.
 *		
 *		Revision 1.8  2000/12/31 04:20:35  jesjones
 *		Removed DisplayAboutBox.
 *		
 *		Revision 1.7  2000/12/10 04:06:16  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.6  2000/11/26 03:29:53  jesjones
 *		Added an Init method. Quit closes the frame window.
 *		
 *		Revision 1.5  2000/11/25 01:50:39  jesjones
 *		OnNoMoreDocWindows uses a callback to call Quit.
 *		
 *		Revision 1.4  2000/11/20 05:46:55  jesjones
 *		Added std:: to a few function calls.
 *		
 *		Revision 1.3  2000/11/13 00:52:07  jesjones
 *		Added an ICommand include.
 *		
 *		Revision 1.2  2000/11/09 12:04:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <20>	11/05/00	JDJ		Added an IEventLoop interface.
 *		 <19>	11/04/00	JDJ		Replaced OnCommandLine with an ICommandLine interface.
 *		 <18>	10/18/00	JDJ		Replaced OnInit and OnQuit with an IInit interface.
 *		 <17>	10/02/99	JDJ		Renamed WhisperStrings.xml XStrings.xml and AppStrings.xml CStrings.xml.
 *		 <16>	 8/10/99	JDJ		Removed mGlobalUndoContext.
 *		 <15>	 7/18/99	JDJ		OnBroadcast calls XWindow::IsOpened.
 *		 <14>	 7/14/99	JDJ		Added OnGetMenuBarURI.
 *		 <13>	 7/05/99	JDJ		Added XFlattenWindowExtension.
 *		 <12>	 6/04/99	JDJ		Ctor calls SetSleepTime. Swapped calls to OnQuit and 
 *									PostQuitMessage.
 *		 <11>	 6/02/99	JDJ		HandleQuit does nothing if the app is already quitting.
 *		 <10>	 5/17/99	JDJ		Added OnTranslateAccelerator. (Win)
 *		 <9>	 5/12/99	JDJ		Added OnPostInit.
 *		 <8>	 5/11/99	JDJ		Added OnCommandLine.
 *		 <7>	 4/19/99	JDJ		Ctor disables XMessage TRACEFLOW category.
 *		 <6>	 3/19/99	JDJ		ParseXMLStringTable takes a URI instead of a name.
 *		 <5>	 2/21/99	JDJ		OnAddDebugExtensions creates a XLocaleExtension.
 *		 <4>	 2/16/99	JDJ		Installs a string table loader. Added locale_begin and locale_end
 *									for iterating over locales supported by the app.
 *		 <3>	 1/15/99	JDJ		Replaced hogging processor flag with sleep time.
 *		 <2>	 1/06/99	JDJ		Added WSystemInfoExtension.
 *		 <1>	10/25/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IApplication.h>

#include <shellapi.h>

#include <IActiveMenuBar.h>
#include <ICommand.h>
#include <ICommandQueue.h>
#include <ICommandLine.h>
#include <ICommands.h>
#include <IEventLoop.h>
#include <IFocus.h>
#include <IHierarchy.h>
#include <IInit.h>
#include <IInstallMenuHandler.h>
#include <IMenuBar.h>
#include <IMenuHandler.h>
#include <IMsgExtensible.h>
#include <IStringTableLoader.h>
#include <ITimer.h>
#include <IWindow.h>
#include <IWindowMgr.h>
#include <WSystemInfo.h>
#include <XCursor.h>
#include <XError.h>
#include <XIntConversions.h>
#include <XListener.h>
#include <XNumbers.h>
#include <XResource.h>
#include <XStateBroadcaster.h>
#include <XStringTableParser.h>
#include <XTinyVector.h>
#include <XTrace.h>
#include <XTranscode.h>
#include <XURI.h>
#include <XURIResolver.h>

#if __MWERKS__
	#include <crtl.h>
#endif

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetWindowTitle
//
//---------------------------------------------------------------
static std::wstring GetWindowTitle(HWND wnd)
{
	std::wstring title;

//	XControl* control = XControl::GetObject(wnd);	// $$$
//	if (control != nil) 
//		title = control->GetName();

//	else {
		int32 length = GetWindowTextLength(wnd);
		if (WSystemInfo::HasUnicode()) {
			XTinyVector<wchar_t> buffer(length+1UL);
			int32 count = GetWindowTextW(wnd, buffer.buffer(), (int32) buffer.size());
			title = std::wstring(buffer.buffer(), numeric_cast<uint32>(count));
		
		} else {
			XTinyVector<char> buffer2(length+1UL);
			int32 count2 = GetWindowTextA(wnd, buffer2.buffer(), (int32) buffer2.size());
			title = FromPlatformStr(buffer2.buffer(), numeric_cast<uint32>(count2));
		}
//	}

	return title;
}

#if __MWERKS__
#pragma mark -
#endif

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
						
	virtual void		SetSleepTime(MilliSecond time);

	virtual MilliSecond GetSleepTime() const;
						
	virtual void 		Quit();
			
	virtual bool 		IsQuitting() const;
					
 	virtual std::wstring GetName() const;

 	virtual bool 		IsMemoryLow() const;
 							
	virtual void		HandleUpdateMenus(const XEvent& event);

 	virtual	bool 		HandleMessage(const MSG& msg, int32& result);

	virtual XFileSpec 	GetHelpFile() const;
	
	virtual void 		HandleTime();

protected:
	virtual void 		OnBroadcast(const SStateMessage& message);
	
//-----------------------------------
//	Internal API
//	
private:
	virtual void 		DoExtractCommandLine();
	virtual void 		DoNoMoreDocWindows();

	virtual bool 		DoUpdateMenu(IMenuPtr menu, const XEvent& event);

	static	void 		DoDeferError(const std::wstring& summary, const std::wstring& narrative);
	
	static 	void 		DoLoadWhisperStrings(const XLocale& locale, StringTable& table);
	static 	void 		DoLoadAppStrings(const XLocale& locale, StringTable& table);

#if DEBUG
	virtual void 		DoTraceMsg(const MSG& msg) const;
#endif

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
	
	MilliSecond				mSleepTime;

	XFileSpec				mHelpFile;
	uint32					mProcDepth;
	uint32					mTimerID;

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
	PRECONDITION(mNumDocWindows == 0);
	
	TRACEFLOW("XApplication", "XApplication::~XApplication\n");
	
	msInstance = nil;
}


//---------------------------------------------------------------
//
// XApplication::XApplication	
//
//---------------------------------------------------------------
XApplication::XApplication(XBoss* boss) : mWhisperStrings(L"resource://XStrings.bXML"), mAppStrings(L"resource://Strings.bXML")
{	
#if DEBUG
	// New TRACEFLOW categories are on by default. Here we'll 
	// turn off the most common categories since they're usually 
	// just an annoyance. Note that XTraceExtension will restore
	// the saved values when it is created.
	XTrace::Instance()->Enable("XApplication", false);
	XTrace::Instance()->Enable("XControl", false);
	XTrace::Instance()->Enable("XEvents", false);
	XTrace::Instance()->Enable("XFocus", false);
	XTrace::Instance()->Enable("XMenuHandler", false);
	XTrace::Instance()->Enable("XMessage", false);
	XTrace::Instance()->Enable("XPersistXML", false);
	XTrace::Instance()->Enable("XStateBroadcaster", false);	
	XTrace::Instance()->Enable("XWindow", false);
#endif

	this->DoSetBoss(boss);

	mInited = false;
	mQuitting = false;
	mProcDepth = 0;

	mSleepTime = 0;
	mTimerID = 0;
	
	mName = WSystemInfo::GetAppName();
		
	mNumDocWindows = 0;
	XStateBroadcaster::Instance()->AddListener(this);
	
	this->SetSleepTime(100);

	msInstance = this;
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
		XError::Instance()->SetDeferredErrorHandler(XApplication::DoDeferError);
			
		IStringTableLoaderPtr loader(this);
		if (loader) {
			Whisper::InstallWhisperLoader(XApplication::DoLoadWhisperStrings);
			Whisper::InstallAppLoader(XApplication::DoLoadAppStrings);
		}
	
		CallRepeatedMethod(this, &IInit::OnInit);					
		CallRepeatedMethod(this, &IInstallMenuHandler::InstallCommands);
			
		mInited = true;

	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't initialize the application."), e);
		std::exit(1);

	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't initialize the application."));
		std::exit(1);
	}		
}


//---------------------------------------------------------------
//
// XApplication::Run
//
//---------------------------------------------------------------
void XApplication::Run()
{
	PRECONDITION(mInited);
		
	TRACEFLOW("XApplication", "XApplication::HandleEventLoop\n");
	
	this->DoExtractCommandLine();
	
	XStateBroadcaster::Instance()->Broadcast(kStartingApp);

	XCursorUtils::Init();
				
	IEventLoopPtr loop(this);
	loop->Run();	

	XStateBroadcaster::Instance()->Broadcast(kQuittingApp);

	ICommandQueuePtr commands(this);
	if (commands)
		commands->ExecuteCommands();		// give commands a chance to execute

	IHierarchyPtr node(this);
	node->RemoveChildren();

	POSTCONDITION(mQuitting);
}


//---------------------------------------------------------------
//
// XApplication::HandleTime
//
//---------------------------------------------------------------
void XApplication::HandleTime()
{
	ITimerPtr timer(this);
	if (timer)
		timer->Tickle();
}


//---------------------------------------------------------------
//
// XApplication::SetSleepTime
//
//---------------------------------------------------------------
void XApplication::SetSleepTime(MilliSecond time)
{
	PRECONDITION(time >= 0);
	
	if (time != mSleepTime) {
		if (mTimerID != 0) {
			VERIFY(::KillTimer(nil, mTimerID));
			mTimerID = 0;
		}

		if (time > 0 && time < LONG_MAX) {				// note that sleep times of zero are special cased
			mTimerID = ::SetTimer(nil, 0, numeric_cast<uint32>(time), nil);
			ASSERT(mTimerID != 0);

			mSleepTime = time;
		}
	}
}


//---------------------------------------------------------------
//
// XApplication::GetSleepTime
//
//---------------------------------------------------------------
MilliSecond XApplication::GetSleepTime() const
{
	return mSleepTime;
}


//---------------------------------------------------------------
//
// XApplication::Quit
//
//---------------------------------------------------------------
void XApplication::Quit()
{
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
			
			if (window->GetType() == kDocumentWindow)
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
		
		// Close the frame window
		if (mQuitting) {
			HWND windH = ::GetTopWindow(nil);
			while (windH != nil && !IWindow::GetWindow(windH)) 
				windH = ::GetNextWindow(windH, GW_HWNDNEXT);

			if (windH != nil) {
				IWindowPtr wind = IWindow::GetWindow(windH);
				VERIFY(wind->Close(true));
			}
		}
		
		// Post a quit message and call Cleanup.
		if (mQuitting) {
			CallRepeatedMethod(this, &IInstallMenuHandler::RemoveCommands);
			CallRepeatedMethod(this, &IInit::OnCleanup);

			::PostQuitMessage(0);
		}
	}
}


//---------------------------------------------------------------
//
// XApplication::HandleMessage
//
//---------------------------------------------------------------
bool XApplication::HandleMessage(const MSG& inMsg, int32& result)
{		
	if (inMsg.message == WM_GETTEXTLENGTH || inMsg.message == WM_GETTEXT)
		return false;

	++mProcDepth;
	ASSERT(mProcDepth < 100);		// check for infinite loop

#if DEBUG
	this->DoTraceMsg(inMsg);
#endif
		
	MSG msg = inMsg;

	bool handled = false;
	IMsgExtensiblePtr extensible(this);
	if (extensible)
		handled = extensible->HandleMessage(msg);

	if (!handled) {
		handled = true;
		
		switch (msg.message) {
			case WM_ACTIVATE:
			case WM_CHAR:
			case WM_COMMAND:
			case WM_CREATE:
			case WM_DESTROY:
			case WM_KILLFOCUS:
			case WM_LBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_LBUTTONDBLCLK:
			case WM_MBUTTONDBLCLK:
			case WM_RBUTTONDBLCLK:
			case WM_LBUTTONUP:
			case WM_MBUTTONUP:
			case WM_RBUTTONUP:
			case WM_SETFOCUS:
			case WM_SHOWWINDOW:
				{
				IActiveMenuBarPtr activeMenubar(this);
				IMenuBarPtr menubar;
				if (activeMenubar) {
					menubar = activeMenubar->Get();
					
					if (menubar)
						menubar->Invalidate();
				}
				handled = false;
				}
				break;
				
			case WM_NULL:
				break;
				
			case WM_QUERYENDSESSION:
				this->Quit();
				result = mQuitting;
				break;
				
			case WM_QUIT:
				ASSERT(mQuitting);
				break;
				
			default:
				handled = false;
		}
	}
			
	--mProcDepth;

	return handled;
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
// XApplication::GetHelpFile
//
//---------------------------------------------------------------
XFileSpec XApplication::GetHelpFile() const
{
	return mHelpFile;
}


//---------------------------------------------------------------
//
// XApplication::HandleUpdateMenus
//
//---------------------------------------------------------------
void XApplication::HandleUpdateMenus(const XEvent& event)
{
	IActiveMenuBarPtr activeMenuBar(L"Application");
	IMenuBarPtr menubar = activeMenuBar->Get();

	if (!menubar->IsValid()) {
		uint32 count = menubar->GetCount();
		for (uint32 index = 0; index < count; index++) {
			IMenuPtr menu = menubar->GetMenuByIndex(index);
			
			(void) this->DoUpdateMenu(menu, event);
		}	
		
		menubar->Validate();
	}
}

#if __MWERKS__
#pragma mark ~
#endif

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
			if (--mNumDocWindows == 0) {
				this->DoNoMoreDocWindows();
				ASSERT(mNumDocWindows >= 0);
			}
		}
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XApplication::DoExtractCommandLine
//
//---------------------------------------------------------------
void XApplication::DoExtractCommandLine()
{
	std::vector<std::wstring> argList;

	if (WSystemInfo::HasUnicode()) {
		const wchar_t* cmdLine = ::GetCommandLineW();

		int count = 0;
		wchar_t** args = ::CommandLineToArgvW(cmdLine, &count);
		argList.reserve(count - 1UL);
		
		if (args != nil) {
			for (int32 index = 1; index < count; ++index) { 	// first argument is our app name
				argList.push_back(args[index]);
			}

			::GlobalFree(args);
		}

	} else {
		argList.reserve(__argc - 1UL);
		for (int32 index = 1; index < __argc; ++index) 
			argList.push_back(FromPlatformStr(__argv[index]));	// stdlib.h includes a __wargv variable, but AFAIK this is undocumented, and was set to nil when I tried to use it
	}
	
	ICommandLinePtr cl(this);
	if (cl)
		cl->OnCommandLine(argList);
}


//---------------------------------------------------------------
//
// XApplication::DoNoMoreDocWindows
//
//---------------------------------------------------------------
void XApplication::DoNoMoreDocWindows()
{
	XCallback0<void> function(this, &IApplication::Quit);

	ICommandPtr command(L"Callback Command");
	ICallbackCommandPtr callback(command);
	callback->Init(function);
	
	ICommandQueuePtr queue(this);
	queue->Post(command);
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
		
	if (handler) {
		IWindowMgrPtr windowMgr(this);
		IWindowPtr modal = windowMgr->GetTopModal();
		
		uint32 count = menu->GetCount();
		for (uint32 index = 0; index < count; index++) {
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
					if (modal == nil)
						(void) handler->HandleEnableCommand(command, status, event);
					
					// Update the menu item to reflect its new status.
					menu->EnableItem(index, status.enabled);	
					menu->CheckItem(index, status.checkmark);

					if (status.name.length() > 0)
						menu->SetName(index, status.name);
				}
			}
	
			// If the menu item is enabled we need to enable the menu.
			if (status.enabled)
				enableMenu = true;
		}
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
// XApplication::DoTraceMsg
//
//---------------------------------------------------------------
#if DEBUG
void XApplication::DoTraceMsg(const MSG& msg) const
{
	std::wstring str;

	switch (msg.message) {
		case BM_SETCHECK:
			if (msg.wParam == 0)
				str = L"BM_SETCHECK: unchecking '" + GetWindowTitle(msg.hwnd) + L"'";
			else if (msg.wParam == 1)
				str = L"BM_SETCHECK: checking '" + GetWindowTitle(msg.hwnd) + L"'";
			else if (msg.wParam == 2)
				str = L"BM_SETCHECK: checking '" + GetWindowTitle(msg.hwnd) + L"' (indeterminate)";
			else 
				str = L"BM_SETCHECK: checking ''" + GetWindowTitle(msg.hwnd) + L"' (bad state?)";
			break;

		case WM_ACTIVATE:
			{
			uint16 activeFlag = LOWORD(msg.wParam);           
			bool minimizeFlag = HIWORD(msg.wParam) != 0; 
			HWND hwnd         = (HWND) msg.lParam;         

			if (activeFlag == WA_INACTIVE)
				if (minimizeFlag)
					str = L"WM_ACTIVATE: deactivating minimized window '" + GetWindowTitle(hwnd) + L"'";
				else
					str = L"WM_ACTIVATE: deactivating window '" + GetWindowTitle(hwnd) + L"'";
			else
				if (minimizeFlag)
					str = L"WM_ACTIVATE: activating minimized window '" + GetWindowTitle(hwnd) + L"'";
				else
					str = L"WM_ACTIVATE: activating window '" + GetWindowTitle(hwnd) + L"'";
			}
			break;

		case WM_ACTIVATEAPP:
			if (msg.wParam)
				str = L"WM_ACTIVATEAPP: activating app";
			else
				str = L"WM_ACTIVATEAPP: deactivating app";
			break;

		case WM_CANCELMODE:
			str = L"WM_CANCELMODE: canceling mouse capture (usual case)";
			break;

		case WM_CAPTURECHANGED:
			{
			HWND hwnd = (HWND) msg.lParam;         
			str = L"WM_CAPTURECHANGED: window '" + GetWindowTitle(hwnd) + L"' is gaining the mouse capture";
			}
			break;

		case WM_CHAR:
			str = L"WM_CHAR: window '" + GetWindowTitle(msg.hwnd) + L"' received a char ('" + (wchar_t) msg.wParam + L"') message";
			break;

		case WM_CLEAR:
			str = L"WM_CLEAR: window '" + GetWindowTitle(msg.hwnd) + L"' was sent clear message";
			break;

		case WM_CLOSE:
			str = L"WM_CLOSE: app has received a close message";
			break;

		case WM_COMMAND:
			{	
			int16 id = (int16) LOWORD(msg.wParam);
			XMenuCommand command = kNothingCmd;

			XInterfacePtr<const IActiveMenuBar> activeMenubar(this);
			IMenuBarPtr menubar;
			if (activeMenubar) {
				menubar = activeMenubar->Get();
				
				if (menubar) 
					command = menubar->FindCommand(id);
			}
			
			if (command != kNothingCmd)
				str = L"WM_COMMAND: window '" + GetWindowTitle(msg.hwnd) + L"' was sent a '" + command + L"' command";
			else
				str = L"WM_COMMAND: window '" + GetWindowTitle(msg.hwnd) + L"' was sent a " + UInt32ToStr(id) + L" command";
			}
			break;

		case WM_CONTEXTMENU:
			{
			HWND hwnd = (HWND) msg.lParam;         
			str = L"WM_CONTEXTMENU: window '" + GetWindowTitle(hwnd) + L"' has received a context menu message";
			}
			break;

		case WM_COPY:
			str = L"WM_COPY: window '" + GetWindowTitle(msg.hwnd) + L"' was sent copy message";
			break;

		case WM_CREATE:
			str = L"WM_CREATE: window '" + GetWindowTitle(msg.hwnd) + L"' received a create message";
			break;

		case WM_CUT:
			str = L"WM_CUT: window '" + GetWindowTitle(msg.hwnd) + L"' was sent cut message";
			break;

		case WM_DEADCHAR:
			str = L"WM_DEADCHAR: window '" + GetWindowTitle(msg.hwnd) + L"' received a dead char ('" + (wchar_t) msg.wParam + L"') message";	
			break;

		case WM_DESTROY:
			str = L"WM_DESTROY: window '" + GetWindowTitle(msg.hwnd) + L"' received a destroy message";
			break;

		case WM_DROPFILES:
			str = L"WM_DROPFILES: window '" + GetWindowTitle(msg.hwnd) + L"' has received a drop files message";
			break;

		case WM_ENABLE:
			if (msg.wParam)
				str = L"WM_ENABLE: enabling window '" + GetWindowTitle(msg.hwnd) + L"'";
			else
				str = L"WM_ENABLE: disabling window '" + GetWindowTitle(msg.hwnd) + L"'";
			break;

		case WM_HELP:
			str = L"WM_HELP: window '" + GetWindowTitle(msg.hwnd) + L"' was sent a help message";
			break;

		case WM_HSCROLL:
			str = L"WM_HSCROLL: window '" + GetWindowTitle(msg.hwnd) + L"' has received a horzizontal scroll message";
			break;

		case WM_KEYDOWN:
			str = L"WM_KEYDOWN: window '" + GetWindowTitle(msg.hwnd) + L"' received a keydown (0x" + Int8ToHex((int8) msg.wParam) + L") message";
			break;

		case WM_KEYUP:
			str = L"WM_KEYUP: window '" + GetWindowTitle(msg.hwnd) + L"' received a keyup (0x" + Int8ToHex((int8) msg.wParam) + L") message";
			break;

		case WM_KILLFOCUS:
			{
			HWND hwnd = (HWND) msg.wParam;         
			str = L"WM_KILLFOCUS: window focus is changing from '" + GetWindowTitle(msg.hwnd) + L"' to '" + GetWindowTitle(hwnd) + L"'";
			}
			break;

		case WM_LBUTTONDOWN:
			str = L"WM_LBUTTONDOWN: window '" + GetWindowTitle(msg.hwnd) + L"' has received a left button down message at (" + Int32ToStr(LOWORD(msg.lParam)) + L", " + Int32ToStr(HIWORD(msg.lParam)) + L")";
			break;

		case WM_LBUTTONDBLCLK:
			str = L"WM_LBUTTONDBLCLK: window '" + GetWindowTitle(msg.hwnd) + L"' has received a left button double click message at (" + Int32ToStr(LOWORD(msg.lParam)) + L", " + Int32ToStr(HIWORD(msg.lParam)) + L")";
			break;

		case WM_LBUTTONUP:
			str = L"WM_LBUTTONUP: window '" + GetWindowTitle(msg.hwnd) + L"' has received a left button up message at (" + Int32ToStr(LOWORD(msg.lParam)) + L", " + Int32ToStr(HIWORD(msg.lParam)) + L")";
			break;

		case WM_MBUTTONDOWN:
			str = L"WM_MBUTTONDOWN: window '" + GetWindowTitle(msg.hwnd) + L"' has received a middle button down message at (" + Int32ToStr(LOWORD(msg.lParam)) + L", " + Int32ToStr(HIWORD(msg.lParam)) + L")";
			break;

		case WM_MBUTTONDBLCLK:
			str = L"WM_MBUTTONDBLCLK: window '" + GetWindowTitle(msg.hwnd) + L"' has received a middle button double click message at (" + Int32ToStr(LOWORD(msg.lParam)) + L", " + Int32ToStr(HIWORD(msg.lParam)) + L")";
			break;

		case WM_MBUTTONUP:
			str = L"WM_MBUTTONUP: window '" + GetWindowTitle(msg.hwnd) + L"' has received a middle button up message at (" + Int32ToStr(LOWORD(msg.lParam)) + L", " + Int32ToStr(HIWORD(msg.lParam)) + L")";
			break;

#if _WIN32_WINNT >= 0x0400
		case WM_MOUSEWHEEL:
			str = L"WM_MOUSEWHEEL: window '" + GetWindowTitle(msg.hwnd) + L"' has received a mouse wheel message";
			break;
#endif

		case WM_MOVE:
			str = L"WM_MOVE: window '" + GetWindowTitle(msg.hwnd) + L"' has received a move message (" + Int32ToStr(LOWORD(msg.lParam)) + L", " + Int32ToStr(HIWORD(msg.lParam)) + L")";
			break;

		case WM_PALETTEISCHANGING:
			str = L"WM_PALETTEISCHANGING: window '" + GetWindowTitle(msg.hwnd) + L"' is changing the palette";
			break;

		case WM_PASTE:
			str = L"WM_PASTE: window '" + GetWindowTitle(msg.hwnd) + L"' was sent paste message";
			break;

		case WM_PRINT:
			str = L"WM_PRINT: printing window '" + GetWindowTitle(msg.hwnd) + L"'";
			break;

		case WM_QUIT:
			str = L"WM_QUIT: app has received a quit message";
			break;

		case WM_RBUTTONDOWN:
			str = L"WM_RBUTTONDOWN: window '" + GetWindowTitle(msg.hwnd) + L"' has received a right button down message at (" + Int32ToStr(LOWORD(msg.lParam)) + L", " + Int32ToStr(HIWORD(msg.lParam)) + L")";
			break;

		case WM_RBUTTONDBLCLK:
			str = L"WM_RBUTTONDBLCLK: window '" + GetWindowTitle(msg.hwnd) + L"' has received a right button double click message at (" + Int32ToStr(LOWORD(msg.lParam)) + L", " + Int32ToStr(HIWORD(msg.lParam)) + L")";
			break;

		case WM_RBUTTONUP:
			str = L"WM_RBUTTONUP: window '" + GetWindowTitle(msg.hwnd) + L"' has received a right button up message at (" + Int32ToStr(LOWORD(msg.lParam)) + L", " + Int32ToStr(HIWORD(msg.lParam)) + L")";
			break;

		case WM_SETFOCUS:
			{
			HWND hwnd = (HWND) msg.wParam;         
			str = L"WM_SETFOCUS: window focus is changing from '" + GetWindowTitle(hwnd) + L"' to '" + GetWindowTitle(msg.hwnd) + L"'";
			}
			break;

		case WM_SIZE:
			if (msg.wParam == SIZE_MAXIMIZED)
				str = L"WM_SIZE: window '" + GetWindowTitle(msg.hwnd) + L"' has received a maximized size message (" + Int32ToStr(LOWORD(msg.lParam)) + L", " + Int32ToStr(HIWORD(msg.lParam)) + L")";
			else if (msg.wParam == SIZE_MINIMIZED)
				str = L"WM_SIZE: window '" + GetWindowTitle(msg.hwnd) + L"' has received a minimized size message (" + Int32ToStr(LOWORD(msg.lParam)) + L", " + Int32ToStr(HIWORD(msg.lParam)) + L")";
			else
				str = L"WM_SIZE: window '" + GetWindowTitle(msg.hwnd) + L"' has received a size message (" + Int32ToStr(LOWORD(msg.lParam)) + L", " + Int32ToStr(HIWORD(msg.lParam)) + L")";
			break;

		case WM_SHOWWINDOW:
			if (msg.wParam)
				str = L"WM_SHOWWINDOW: showing window '" + GetWindowTitle(msg.hwnd) + L"'";
			else
				str = L"WM_SHOWWINDOW: hiding window '" + GetWindowTitle(msg.hwnd) + L"'";
			break;

		case WM_UNDO:
			str = L"WM_UNDO: window '" + GetWindowTitle(msg.hwnd) + L"' was sent undo message";
			break;

		case WM_VSCROLL:
			str = L"WM_VSCROLL: window '" + GetWindowTitle(msg.hwnd) + L"' has received a vertical scroll message";
			break;
			
		case BM_GETCHECK:
		case SBM_GETPOS:
		case SBM_GETSCROLLINFO:
		case SBM_SETSCROLLINFO:
		case WM_COMPAREITEM:
		case WM_COPYDATA:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
		case WM_DRAWITEM:
		case WM_ENTERIDLE:
		case WM_ENTERMENULOOP:
		case WM_ENTERSIZEMOVE:
		case WM_ERASEBKGND:
		case WM_EXITMENULOOP:
		case WM_EXITSIZEMOVE:
		case WM_GETFONT:
		case WM_GETICON:
		case WM_GETMINMAXINFO:
		case WM_GETTEXT:
		case WM_GETTEXTLENGTH:
		case WM_ICONERASEBKGND:
		case WM_INITMENU:
		case WM_INITMENUPOPUP:
		case WM_MEASUREITEM:
		case WM_MENUCHAR:
		case WM_MENUSELECT:
		case WM_MOUSEACTIVATE:
#if _WIN32_WINNT >= 0x0400
		case WM_MOUSEHOVER:
		case WM_MOUSELEAVE:
#endif
		case WM_MOUSEMOVE:
		case WM_MOVING:
		case WM_NCACTIVATE:
		case WM_NCCALCSIZE:
		case WM_NCCREATE:
		case WM_NCDESTROY:
		case WM_NCHITTEST:
		case WM_NCLBUTTONDBLCLK:
		case WM_NCLBUTTONDOWN:
		case WM_NCLBUTTONUP:
		case WM_NCMBUTTONDBLCLK:
		case WM_NCMBUTTONDOWN:
		case WM_NCMBUTTONUP:
		case WM_NCMOUSEMOVE:
		case WM_NCPAINT:
		case WM_NCRBUTTONDBLCLK:
		case WM_NCRBUTTONDOWN:
		case WM_NCRBUTTONUP:
		case WM_NEXTDLGCTL:
		case WM_NEXTMENU:
		case WM_NULL:
		case WM_PAINT:
		case WM_PAINTICON:
		case WM_PARENTNOTIFY:
		case WM_QUERYDRAGICON:
		case WM_SETCURSOR:
		case WM_SETFONT:
		case WM_SETICON:
		case WM_SETTEXT:
		case WM_SIZING:
		case WM_SYSCOMMAND:
		case WM_TIMER:
		case WM_WINDOWPOSCHANGED:
		case WM_WINDOWPOSCHANGING:
			// ignore these messages
//			if (msg.message != WM_GETTEXTLENGTH)
//				str = L"WM_????: window '" + GetWindowTitle(msg.hwnd) + L"' has received a 0x" + UInt32ToHex(msg.message) + L" message";
			break;
			
		default:
			str = L"WM_????: window '" + GetWindowTitle(msg.hwnd) + L"' has received a 0x" + UInt32ToHex(msg.message) + L" message";
	}
	
	if (str.length() > 0) {
		for (uint32 index = 1; index < mProcDepth; ++index)
			TRACEFLOW("XMessage", "   ");

		TRACEFLOW("XMessage", str, "\n");
	}
}
#endif	// DEBUG


}	// namespace Whisper
