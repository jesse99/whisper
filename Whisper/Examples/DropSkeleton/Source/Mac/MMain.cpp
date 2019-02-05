/*
 *  File:       MMain.cpp
 *  Summary:   	The big enchilada.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MMain.cpp,v $
 *		Revision 1.9  2001/05/06 10:34:39  jesjones
 *		Nuked the call to SetSleepTime and the MPreferences.
 *		
 *		Revision 1.8  2001/04/13 07:51:04  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.7  2001/02/08 03:52:02  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.6  2001/01/05 06:19:07  jesjones
 *		Refactored boot strap code.
 *		
 *		Revision 1.5  2001/01/01 03:50:11  jesjones
 *		Fixed the class registration.
 *		
 *		Revision 1.4  2000/12/10 03:58:04  jesjones
 *		Inits the app.
 *		
 *		Revision 1.3  2000/11/09 09:15:25  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */
 
#include "AppHeader.h"

#include <IApplication.h>
#include <IDropApp.h>
#include <MSystemInfo.h>
#include <XBootStrap.h>
#include <XError.h>
#include <XFileFilter.h>
#include <XFolderFilter.h>
#include <XObjectModel.h>
#include <XRegisterUI.h>
#include <XStringUtils.h>
#include <XTrace.h>
#include <XTraceSinks.h>
#include <XURI.h>

using namespace Whisper;


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
	REGISTER_INTERFACE_FACTORY(XActivateMenuHandlers, L"XActivateMenuHandlers");
	REGISTER_INTERFACE_FACTORY(XAppEventLoop, L"XAppEventLoop");
	REGISTER_INTERFACE_FACTORY(XApplication, L"XApplication");
	
	REGISTER_INTERFACE_FACTORY(XCallbackCommand, L"XCallbackCommand");
	REGISTER_INTERFACE_FACTORY(XClickExtensible, L"XClickExtensible");
	REGISTER_INTERFACE_FACTORY(XControl, L"XControl");
	REGISTER_INTERFACE_FACTORY(XCommand, L"XCommand");
	REGISTER_INTERFACE_FACTORY(XCommandClickExtension, L"XCommandClickExtension");
	REGISTER_INTERFACE_FACTORY(XCommandQueue, L"XCommandQueue");
	REGISTER_INTERFACE_FACTORY(XControlGeometry, L"XControlGeometry");
	REGISTER_INTERFACE_FACTORY(XControlMouseHandler, L"XControlMouseHandler");
	REGISTER_INTERFACE_FACTORY(XControlText, L"XControlText");

	REGISTER_INTERFACE_FACTORY(XDefaultDrawWindow, L"XDefaultDrawWindow");
	REGISTER_INTERFACE_FACTORY(XDefaultPositionWindow, L"XDefaultPositionWindow");
	REGISTER_INTERFACE_FACTORY(XDraw, L"XDraw");
	REGISTER_INTERFACE_FACTORY(XDrawControl, L"XDrawControl");
	REGISTER_INTERFACE_FACTORY(XDropApp, L"XDropApp");
	REGISTER_INTERFACE_FACTORY(XDropAppProgress, L"XDropAppProgress");
	
	REGISTER_INTERFACE_FACTORY(XHierarchy, L"XHierarchy");
	
	REGISTER_INTERFACE_FACTORY(XMouseHandler, L"XMouseHandler");

	REGISTER_INTERFACE_FACTORY(XProgressBar, L"XProgressBar");
	REGISTER_INTERFACE_FACTORY(XPushButton, L"XPushButton");

	REGISTER_INTERFACE_FACTORY(XRootControl, L"XRootControl");

	REGISTER_INTERFACE_FACTORY(XStringTableLoader, L"XStringTableLoader");

	REGISTER_INTERFACE_FACTORY(XTimer, L"XTimer");
	
	REGISTER_INTERFACE_FACTORY(XWindow, L"XWindow");
	REGISTER_INTERFACE_FACTORY(XWindowFocus, L"XWindowFocus");
	REGISTER_INTERFACE_FACTORY(XWindowGeometry, L"XWindowGeometry");
	REGISTER_INTERFACE_FACTORY(XWindowMgr, L"XWindowMgr");
	
	REGISTER_INTERFACE_FACTORY(M16BitValueControl, L"M16BitValueControl");
	REGISTER_INTERFACE_FACTORY(MEventDispatcher, L"MEventDispatcher");
	REGISTER_INTERFACE_FACTORY(MRootGeometry, L"MRootGeometry");
	REGISTER_INTERFACE_FACTORY(MStaticText, L"MStaticText");
	REGISTER_INTERFACE_FACTORY(MValueControl, L"MValueControl");
	REGISTER_INTERFACE_FACTORY(MWindowClick, L"MWindowClick");
#endif
}
}


//---------------------------------------------------------------
//
// Init
//
//---------------------------------------------------------------
static void Init()
{
	// See if we can run on this system.
	std::list<std::wstring> needs;
	
	DefaultSystemCheck(needs);			
	if (!MSystemInfo::HasThreadMgr())
		needs.push_back(LoadAppString(L"the Thread Manager"));	// drag and drop apps require the Thread Manager

	if (!needs.empty())
		HandleBadSystem(needs);			// won't return
		
	// Init some TRACE sinks.
#if DEBUG
	XTraceSink* sink = new XDebuggerSink;
	XTrace::Instance()->AddSink(sink);

	sink = new XDebugLogSink("Debug Log.txt");
	XTrace::Instance()->AddSink(sink);
#endif	

	// Init the UI package.
	InitUI();
	
	// Tell the object model about the interface and implementation
	// classes we're using.
	RegisterUIInterfaces();
	RegisterImplementations();
	
	REGISTER_INTERFACE_FACTORY(CDropAppHelper, L"CDropAppHelper");

	// Tell the object model the layout for all the bosses we use.
	XURI uri(L"resource://Bosses.bXML");
	XObjectModel::Instance()->LoadBosses(nil, uri);
}


//---------------------------------------------------------------
//
// ShutDown
//
//---------------------------------------------------------------
static void ShutDown()
{
	ShutDownUI();
}


//---------------------------------------------------------------
//
// Main
//
//---------------------------------------------------------------
int main()
{		
	try {							
		Init();

		{
		IApplicationPtr app(L"Application");
		IDropAppPtr drop(L"Application");
			app->Init();

			// You can customize which files your app will process by fiddling
			// with these filters. For example, StringTableBuilder filters out 
			// files that don't have a C++ extension and folders in parenthesis.
			// Note that if you want to filter by file type you should also
			// change the 'BNDL' resource in the .rsrc file.
			drop->Start(new XFileFilter, new XFolderFilter);

			app->Run();
		}
		
		ShutDown();

	} catch (const std::exception& e) {
		// Run will trap exceptions so we should only hit this code
		// if the code above Run throws.
		XError::Instance()->ReportError(LoadAppString(L"Couldn't initialize the application."), e);

	} catch (...) {
		XError::Instance()->ReportError(LoadAppString(L"Couldn't initialize the application."));
	}

	return 0;
}



