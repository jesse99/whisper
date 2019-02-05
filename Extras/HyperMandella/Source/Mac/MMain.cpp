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
 *		Revision 1.7  2001/05/06 10:34:43  jesjones
 *		Nuked the call to SetSleepTime and the MPreferences.
 *		
 *		Revision 1.6  2001/04/27 04:29:56  jesjones
 *		Nuked MPreferences.
 *		
 *		Revision 1.5  2001/04/05 23:49:31  jesjones
 *		Added on startup pref.
 *		
 *		Revision 1.4  2001/03/06 10:07:27  jesjones
 *		Added a method to XObjectModel to write the bosses and interfaces to a log file.
 *		
 *		Revision 1.3  2001/02/24 09:32:36  jesjones
 *		Tweaked a comment.
 *		
 *		Revision 1.2  2001/02/09 04:30:33  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.1  2000/12/28 02:52:44  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.3  2000/11/26 03:25:10  jesjones
 *		Calls IApplication::Init.
 *		
 *		Revision 1.2  2000/11/09 09:15:30  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */
 
#include "AppHeader.h"
#include <XBootStrap.h>

#include <IApplication.h>
#include <IDocuments.h>
#include <IWriteXML.h>
#include <XError.h>
#include <XFileDialog.h>
#include <XFileFilter.h>
#include <XObjectModel.h>
#include <XPreference.h>
#include <XRegisterXMLLoaders.h>
#include <XStringUtils.h>
#include <XTrace.h>
#include <XTraceSinks.h>
#include <XURI.h>

#include "CPersistXML.h"
#include "CRegisterClasses.h"

using namespace Whisper;


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

	if (spec.GetType() == 'TEXT' || spec.GetExtension() == L"xml")
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
		
	// Init the SIOUX debug window and some TRACE sinks.
#if DEBUG
	InitSIOUX();

	XTraceSink* sink = new XDebuggerSink;
	XTrace::Instance()->AddSink(sink);

	sink = new XDebugLogSink("Debug Log.txt");
	XTrace::Instance()->AddSink(sink);
#endif	

	// Init the UI package.
	InitUI();
	
	// Tell the object model about the interface and implementation
	// classes we're using.
	RegisterInterfaces();
	RegisterImplementations();
	
	// Tell the object model the layout for all the bosses we use.
	XURI uri(L"resource://Bosses.bXML");
	XObjectModel::Instance()->LoadBosses(nil, uri);

	// Register the loaders we're using to stream in bosses from XML.
	RegisterLoaders();

	// $$$ Muck with monitors
#if 0
	XPreference<bool> useMillions('DfNC', 1, false);
	mIdealDisplayMode.depth = (int16) (*useMillions ? 32 : 16);

	XPreference<bool> canSwitchDepth('CnSD', 1, true);
	mCanChangeDevice = *canSwitchDepth;
#endif
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


