/*
 *  File:       WMain.cpp
 *  Summary:   	The big enchilada.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WMain.cpp,v $
 *		Revision 1.7  2001/03/09 10:13:52  jesjones
 *		Uses apartment threading.
 *		
 *		Revision 1.6  2001/02/22 07:25:15  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.5  2001/01/03 05:51:56  jesjones
 *		Fixed the registration code.
 *		
 *		Revision 1.4  2000/12/05 09:16:51  jesjones
 *		Inits the app.
 *		
 *		Revision 1.3  2000/11/21 09:21:23  jesjones
 *		Updated the registration stuff.
 *		
 *		Revision 1.2  2000/11/09 12:09:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include <XBootStrap.h>

#include <objbase.h>
#include <shlobj.h>

#include <IApplication.h>
#include <IDropApp.h>
#include <WSystemInfo.h>
#include <XError.h>
#include <XExceptions.h>
#include <XFileFilter.h>
#include <XFolderFilter.h>
#include <XObjectModel.h>
#include <XRegisterUI.h>
#include <XResource.h>
#include <XStringUtils.h>
#include <XTrace.h>
#include <XTraceSinks.h>
#include <XURI.h>
#include <XURIResolver.h>
#include <XXMLCallbacks.h>
#include <XXMLParser.h>

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
	REGISTER_INTERFACE_FACTORY(XControlFocus, L"XControlFocus");
	REGISTER_INTERFACE_FACTORY(XControlGeometry, L"XControlGeometry");
	REGISTER_INTERFACE_FACTORY(XControlMouseHandler, L"XControlMouseHandler");
	REGISTER_INTERFACE_FACTORY(XControlText, L"XControlText");

	REGISTER_INTERFACE_FACTORY(XDefaultChangedSize, L"XDefaultChangedSize");
	REGISTER_INTERFACE_FACTORY(XDefaultDrawWindow, L"XDefaultDrawWindow");
	REGISTER_INTERFACE_FACTORY(XDefaultPositionWindow, L"XDefaultPositionWindow");
	REGISTER_INTERFACE_FACTORY(XDraw, L"XDraw");
	REGISTER_INTERFACE_FACTORY(XDrawControl, L"XDrawControl");
	REGISTER_INTERFACE_FACTORY(XDropApp, L"XDropApp");
	REGISTER_INTERFACE_FACTORY(XDropAppProgress, L"XDropAppProgress");
	
	REGISTER_INTERFACE_FACTORY(XHierarchy, L"XHierarchy");
	
	REGISTER_INTERFACE_FACTORY(XIdler, L"XIdler");	

	REGISTER_INTERFACE_FACTORY(XKeyHandler, L"XKeyHandler");

	REGISTER_INTERFACE_FACTORY(XMouseHandler, L"XMouseHandler");

	REGISTER_INTERFACE_FACTORY(XNullDrawer, L"XNullDrawer");

	REGISTER_INTERFACE_FACTORY(XProgressBar, L"XProgressBar");
	REGISTER_INTERFACE_FACTORY(XPushButton, L"XPushButton");

	REGISTER_INTERFACE_FACTORY(XRootControl, L"XRootControl");

	REGISTER_INTERFACE_FACTORY(XStringTableLoader, L"XStringTableLoader");

	REGISTER_INTERFACE_FACTORY(XTimer, L"XTimer");
	
	REGISTER_INTERFACE_FACTORY(XWindow, L"XWindow");
	REGISTER_INTERFACE_FACTORY(XWindowFocus, L"XWindowFocus");
	REGISTER_INTERFACE_FACTORY(XWindowGeometry, L"XWindowGeometry");
	REGISTER_INTERFACE_FACTORY(XWindowMgr, L"XWindowMgr");

	REGISTER_INTERFACE_FACTORY(WButtonKeyHandler, L"WButtonKeyHandler");
	REGISTER_INTERFACE_FACTORY(WProgressValueControl, L"WProgressValueControl");
	REGISTER_INTERFACE_FACTORY(WStaticText, L"WStaticText");
#endif
}
}


//---------------------------------------------------------------
//
// Load
//
// We don't want to require a bXML file to build the compiler so
// we'll read the bosses file using the XML file.
//
//---------------------------------------------------------------
static bool Load(const XURI& uri, XXMLDoc& doc)
{	
	bool parsed = false;
	
	XAutoPtr<XResource> data(XURIResolver::Instance()->Resolve(uri));
	
	{
	XXMLParser parser(data.Get());
	XXMLDocumentCallback builder(doc);
		parser.Parse(builder);
		parsed = true;

#if DEBUG
		uint32 count = parser.GetNumWarnings();
		if (count > 0) {
			DEBUGSTR("There were ", count, " warnings");
			
			for (uint32 index = 0; index < count; ++index)
				TRACE(parser.GetWarning(index), "\n");
		}
#endif
	}
		
	return parsed;
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
	WSystemInfo::SetPrefsName(LoadAppString(L"SOFTWARE\\Whisper\\XML Compiler\\1.0"));

	// Tell the object model about the interface and implementation
	// classes we're using.
	RegisterUIInterfaces();
	RegisterImplementations();
	
	REGISTER_INTERFACE_FACTORY(CDropAppHelper, L"CDropAppHelper");

	// Tell the object model the layout for all the bosses we use.
	XXMLDoc doc;
	XURI uri(L"resource://Bosses.XML");
	Load(uri, doc);
	XObjectModel::Instance()->LoadBosses(nil, doc);
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
			XFileFilter* filter = new XFileFilter;
			filter->AddType(XFileType(L"XML Files", L"XML"));
			drop->Start(filter, new XFolderFilter);

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


