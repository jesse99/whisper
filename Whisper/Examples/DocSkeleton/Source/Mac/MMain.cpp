/*
 *  File:       MMain.cpp
 *  Summary:   	Main entry point for Mac apps.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MMain.cpp,v $
 *		Revision 1.5  2001/04/19 04:11:08  jesjones
 *		Deleted MPreferences.
 *		
 *		Revision 1.4  2001/02/17 10:25:09  jesjones
 *		Tweaked some comments.
 *		
 *		Revision 1.3  2001/02/07 02:22:58  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.2  2001/01/05 06:19:11  jesjones
 *		Refactored boot strap code.
 *		
 *		Revision 1.1  2000/12/31 04:30:16  jesjones
 *		Checked in	
 */
 
#include "AppHeader.h"

#include <IApplication.h>
#include <IDocuments.h>
#include <XBootStrap.h>
#include <XError.h>
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
		
	// Init the SIOUX debug window and some TRACE sinks.
#if DEBUG
	Whisper::InitSIOUX();

	XTraceSink* sink = new XDebuggerSink;
	XTrace::Instance()->AddSink(sink);

	sink = new XDebugLogSink("Debug Log.txt");
	XTrace::Instance()->AddSink(sink);
#endif	

	// Init the UI package.
	Whisper::InitUI();
	
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
// Main
//
//---------------------------------------------------------------
int main()
{		
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


