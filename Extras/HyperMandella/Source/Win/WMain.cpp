/*
 *  File:       WMain.cpp
 *  Summary:   	The big enchilada.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WMain.cpp,v $
 *		Revision 1.5  2001/04/05 23:49:35  jesjones
 *		Added on startup pref.
 *		
 *		Revision 1.4  2001/03/09 10:06:39  jesjones
 *		Uses spartment threading.
 *		
 *		Revision 1.3  2001/03/06 10:07:32  jesjones
 *		Added a method to XObjectModel to write the bosses and interfaces to a log file.
 *		
 *		Revision 1.2  2001/02/24 09:32:12  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.1  2001/01/03 05:08:14  jesjones
 *		Checked in (again)
 *		
 *		Revision 1.6  2000/12/28 02:52:48  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.4  2000/11/26 03:25:02  jesjones
 *		Calls IApplication::Init.
 *		
 *		Revision 1.3  2000/11/25 01:42:30  jesjones
 *		Registered some XML readers. Calls XObjectModel::Terminate.
 *		
 *		Revision 1.2  2000/11/09 12:09:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include <XBootStrap.h>

#include <objbase.h>

#include <IApplication.h>
#include <IDocuments.h>
#include <IWriteXML.h>
#include <WSystemInfo.h>
#include <XError.h>
#include <XExceptions.h>
#include <XFileDialog.h>
#include <XFileFilter.h>
#include <XObjectModel.h>
#include <XPreference.h>
#include <XRegisterUI.h>
#include <XRegisterXMLLoaders.h>
#include <XStringUtils.h>
#include <XTrace.h>
#include <XTraceSinks.h>
#include <XURI.h>

#include "CPersistXML.h"
#include "CRegisterClasses.h"


// ===================================================================================
//	class CFileFilter		
// ===================================================================================
class CFileFilter : public XFileFilter {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~CFileFilter();
	
						CFileFilter();
												
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		operator()(const XFileSpec& spec) const;
};

//---------------------------------------------------------------
//
// CFileFilter::~CFileFilter
//
//---------------------------------------------------------------
CFileFilter::~CFileFilter()
{
}

	
//---------------------------------------------------------------
//
// CFileFilter::CFileFilter
//
//---------------------------------------------------------------
CFileFilter::CFileFilter()
{
}

												
//---------------------------------------------------------------
//
// CFileFilter::operator()
//
//---------------------------------------------------------------
bool CFileFilter::operator()(const XFileSpec& spec) const
{
	bool ok = false;

	if (spec.GetExtension() == L"xml")
		ok = true;
	else
		ok = spec.HasMetaData(L"Fractal Info");
		
	return ok;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Internal Functions		
// ===================================================================================

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
	DefaultSystemCheck(needs);			
	if (!needs.empty())
		HandleBadSystem(needs);			// won't return
		
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
	InitUI();
	
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
	ShutDownUI();
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Main Entry Point		
// ===================================================================================

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
			
			XPreference<int32> onStartup(L"On Startup", 0);
			docs->Init((EStartupAction) *onStartup);
			
			docs->SetFileFilter(new CFileFilter);
			docs->GetFileOptions().AddType(XFileType(L"XML", L"xml", 'HypM', 'TEXT'));
			docs->GetFileOptions().AddType(XFileType(L"PNG", L"png", 'HypM', 'PNGf'));
			
			app->Run();
		}
		
#if DEBUG && 1
		XObjectModel::Instance()->WriteBosses();
#endif

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


