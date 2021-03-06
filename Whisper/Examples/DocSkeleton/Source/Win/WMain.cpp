/*
 *  File:       WMain.cpp
 *  Summary:   	Main entry point for Windows apps.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WMain.cpp,v $
 *		Revision 1.5  2001/03/09 10:13:10  jesjones
 *		Uses apartment threading.
 *		
 *		Revision 1.4  2001/02/23 10:07:48  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.3  2001/02/07 02:24:31  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.2  2001/01/01 00:27:07  jesjones
 *		Added an include.
 *		
 *		Revision 1.1  2000/12/31 04:30:41  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include <XBootStrap.h>

#include <commctrl.h>
#include <objbase.h>

#include <IApplication.h>
#include <IDocuments.h>
#include <WSystemInfo.h>
#include <XBootStrap.h>
#include <XError.h>
#include <XExceptions.h>
#include <XObjectModel.h>
#include <XStringUtils.h>
#include <XTrace.h>
#include <XTraceSinks.h>
#include <XURI.h>

#include "CRegisterClasses.h"

using namespace Whisper;


//---------------------------------------------------------------
//
// Init
//
//---------------------------------------------------------------
static void Init()
{
	// See if we can run on this system (you may want to do some
	// additional checks).
	std::list<std::wstring> needs;
	Whisper::DefaultSystemCheck(needs);			
	if (!needs.empty())
		Whisper::HandleBadSystem(needs);			// won't return
		
	// Initialize Microsoft's COM (some of Whisper's file code uses COM).
#if (_WIN32_WINNT >= 0x0400) || defined(_WIN32_DCOM) 
	HRESULT result = CoInitializeEx(nil, COINIT_APARTMENTTHREADED);	// IShellLink doesn't seem to work with COINIT_SPEED_OVER_MEMORY
#else
	HRESULT result = CoInitialize(nil);
#endif
	ThrowIfFailed(result);
	
	// Init some TRACE sinks.
#if DEBUG
	XTraceSink* sink = new XDebuggerSink;
	XTrace::Instance()->AddSink(sink);

	sink = new XDebugLogSink("Debug Log.txt");
	XTrace::Instance()->AddSink(sink);
#endif	

	// Init the UI package.
	Whisper::InitUI();
	
	// Initialize the preferences
	WSystemInfo::SetPrefsName(LoadAppString(L"SOFTWARE\\Whisper\\HyperMandella\\1.0"));

	// Tell the object model about the interface and implementation
	// classes we're using.
	RegisterInterfaces();
	RegisterImplementations();
	
	// Tell the object model the layout for all the bosses we use.
	XURI uri(L"resource://Bosses.bXML");
	XObjectModel::Instance()->LoadBosses(nil, uri);

	// Register the loaders we're using to stream in bosses from XML.
	RegisterLoaders();
}


//---------------------------------------------------------------
//
// ShutDown
//
//---------------------------------------------------------------
static void ShutDown()
{
	Whisper::ShutDownUI();
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
		IDocumentsPtr docs(L"Application");
			app->Init();
			docs->Init(kNewDocOnStartup);
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


