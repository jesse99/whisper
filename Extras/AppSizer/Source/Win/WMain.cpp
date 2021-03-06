/*
 *  File:       WMain.cpp
 *  Summary:   	The big enchilada.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WMain.cpp,v $
 *		Revision 1.2  2001/03/09 10:03:38  jesjones
 *		Uses apartment threading.
 *		
 *		Revision 1.1.1.1  2000/12/27 03:12:48  jesjones
 *		First Imported.
 *		
 *		Revision 1.3  2000/11/21 09:19:49  jesjones
 *		Updated the registration stuff.
 *		
 *		Revision 1.2  2000/11/09 12:09:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include <XBootStrap.h>

#include <objbase.h>

#include <IApplication.h>
#include <WSystemInfo.h>
#include <XError.h>
#include <XExceptions.h>
#include <XObjectModel.h>
#include <XRegisterUI.h>
#include <XStringUtils.h>
#include <XTrace.h>
#include <XTraceSinks.h>
#include <XURI.h>

using namespace Whisper;


// ===================================================================================
//	class CBootStrap
// ===================================================================================
class CBootStrap : public XBootStrap {

	typedef XBootStrap Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CBootStrap();
	
  						CBootStrap(XBoss* boss);

//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnSystemNeeds(std::list<std::wstring>& needs);

	virtual void 		OnBoot();
};

DEFINE_INTERFACE_FACTORY(CBootStrap)

//---------------------------------------------------------------
//
// CBootStrap::~CBootStrap
//
//---------------------------------------------------------------
CBootStrap::~CBootStrap()
{
}


//---------------------------------------------------------------
//
// CBootStrap::CBootStrap
//
//---------------------------------------------------------------
CBootStrap::CBootStrap(XBoss* boss) : XBootStrap(boss)
{
}


//---------------------------------------------------------------
//
// CBootStrap::OnSystemNeeds
//
// This is the place to check for anything unusual on which your
// app depends. If your app can't run XBootStrap will pop up an
// alert listing everything that is missing and exit.
//
//---------------------------------------------------------------
void CBootStrap::OnSystemNeeds(std::list<std::wstring>& needs)
{
	Inherited::OnSystemNeeds(needs);
}


//---------------------------------------------------------------
//
// CBootStrap::OnBoot
//
// This is where you handle platform dependant initialization.
//
//---------------------------------------------------------------
void CBootStrap::OnBoot()
{
	// Initialize Microsoft's COM (some of Whisper's file code uses COM).
#if (_WIN32_WINNT >= 0x0400) || defined(_WIN32_DCOM) 
	HRESULT result = CoInitializeEx(nil, COINIT_APARTMENTTHREADED);	// IShellLink doesn't seem to work with COINIT_SPEED_OVER_MEMORY
#else
	HRESULT result = CoInitialize(nil);
#endif
	ThrowIfFailed(result);

	// Tell the preferences code which registery key to use (you'll 
	// want to change this). It's a good idea to do this even if you 
	// don't use preferences because some of the Whisper code uses 
	// preferences. 
	WSystemInfo::SetPrefsName(LoadAppString(L"SOFTWARE\\Whisper\\DropSkeleton\\1.0"));
}

#if __MWERKS__
#pragma mark -
#endif

//---------------------------------------------------------------
//
// RegisterImplementations	
//
// When someone asks for an interface on a boss Whisper needs to
// create the interface using the implementation class listed in
// a Bosses.XML file. This requires looking up a factory function
// using an implementation name. The implementation classes define
// the factories using the DEFINE_INTERFACE_FACTORY macros, but
// they don't register the factories with the object model. So,
// we register them here. 
//
//---------------------------------------------------------------
namespace Whisper {
static void RegisterImplementations()
{	
#if 0
	// We don't call RegisterUIImplementations because that will
	// prevent the linker from dead-stripping implementation classes
	// that we're not using.	
	RegisterUIImplementations();
#else
	REGISTER_INTERFACE_FACTORY(XAppEventLoop, L"XAppEventLoop");
	REGISTER_INTERFACE_FACTORY(XCallbackCommand, L"XCallbackCommand");
	REGISTER_INTERFACE_FACTORY(XClickExtensible, L"XClickExtensible");
	REGISTER_INTERFACE_FACTORY(XCommandClickExtension, L"XCommandClickExtension");
	REGISTER_INTERFACE_FACTORY(XCommandQueue, L"XCommandQueue");
	REGISTER_INTERFACE_FACTORY(XControlGeometry, L"XControlGeometry");
	REGISTER_INTERFACE_FACTORY(XControlMouseHandler, L"XControlMouseHandler");
	REGISTER_INTERFACE_FACTORY(XDefaultChangedSize, L"XDefaultChangedSize");
	REGISTER_INTERFACE_FACTORY(XDefaultDrawWindow, L"XDefaultDrawWindow");
	REGISTER_INTERFACE_FACTORY(XDefaultPositionWindow, L"XDefaultPositionWindow");
	REGISTER_INTERFACE_FACTORY(XDrawControl, L"XDrawControl");
	REGISTER_INTERFACE_FACTORY(XDropAppProgress, L"XDropAppProgress");
	REGISTER_INTERFACE_FACTORY(XHierarchy, L"XHierarchy");
	REGISTER_INTERFACE_FACTORY(XIdler, L"XIdler");
	REGISTER_INTERFACE_FACTORY(XMouseHandler, L"XMouseHandler");
	REGISTER_INTERFACE_FACTORY(XNullDrawer, L"XNullDrawer");
	REGISTER_INTERFACE_FACTORY(XStringTableLoader, L"XStringTableLoader");
	REGISTER_INTERFACE_FACTORY(XTimer, L"XTimer");
	REGISTER_INTERFACE_FACTORY(XWindow, L"Window");
	REGISTER_INTERFACE_FACTORY(XWindowFocus, L"XWindowFocus");
	
	REGISTER_INTERFACE_FACTORY(WControl, L"Control");
	REGISTER_INTERFACE_FACTORY(WControlText, L"ControlText");
	REGISTER_INTERFACE_FACTORY(WProgressBar, L"ProgressBar");
	REGISTER_INTERFACE_FACTORY(WProgressValueControl, L"WProgressValueControl");
	REGISTER_INTERFACE_FACTORY(WPushButton, L"PushButton");
	REGISTER_INTERFACE_FACTORY(WRootControl, L"RootControl");
	REGISTER_INTERFACE_FACTORY(WStaticText, L"WStaticText");
	REGISTER_INTERFACE_FACTORY(WWindowGeometry, L"WindowGeometry");
	REGISTER_INTERFACE_FACTORY(WWindowMgr, L"WindowMgr");
#endif
}
}


//---------------------------------------------------------------
//
// InitObjectModel
//
//---------------------------------------------------------------
static void InitObjectModel()
{
	// Tell the object model which interface class is associated
	// with each interface name. Note that we can do this for all
	// the Whisper interfaces because we're only dealing with
	// interfaces so there are no worries about implementation
	// code bloat.
	RegisterUIInterfaces();
	
	// Tell the object model which implementation class is associated
	// with each implementation name. This will cause bloat so we're
	// careful about which implementations we register.
	RegisterImplementations();
	
	// Register our two implementation classes.
	REGISTER_INTERFACE_FACTORY(CApp, L"CApp");
	REGISTER_INTERFACE_FACTORY(CBootStrap, L"CBootStrap");

	// Tell the object model the layout for all the bosses we use.
	XURI uri(L"resource://Bosses.bXML");
	XObjectModel::Instance()->LoadBosses(nil, uri);
}


//---------------------------------------------------------------
//
// WinMain
//
//---------------------------------------------------------------
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int showFlags)
{
	UNUSED(instance);
	UNUSED(prevInstance);
	UNUSED(cmdLine);
	UNUSED(showFlags);
				
	try {		
		InitObjectModel();
		
		IBootStrapPtr booter(L"Bootstrap");
		booter->HandleBoot();
		
		IApplicationPtr app(L"Application");
		app->Init();
		app->Run();
	
		XObjectModel::Instance()->Shutdown();

	} catch (const std::exception& e) {
		// Run will trap exceptions so we should only hit this code
		// if the code above Run throws.
		XError::Instance()->ReportError(LoadAppString(L"Couldn't initialize the application."), e);

	} catch (...) {
		XError::Instance()->ReportError(LoadAppString(L"Couldn't initialize the application."));
	}
		
	return 0;
}


