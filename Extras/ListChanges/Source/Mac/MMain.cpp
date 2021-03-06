/*
 *  File:       MMain.cpp
 *  Summary:   	The big enchilada.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MMain.cpp,v $
 *		Revision 1.3  2001/05/06 10:34:24  jesjones
 *		Nuked the call to SetSleepTime and the MPreferences.
 *		
 *		Revision 1.2  2001/04/13 07:50:46  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.1  2001/03/05 04:51:41  jesjones
 *		Checked in	
 */
 
#include "AppHeader.h"
#include <XBootStrap.h>

#include <IApplication.h>
#include <IDropApp.h>
#include <MSystemInfo.h>
#include <XBootStrap.h>
#include <XError.h>
#include <XExceptions.h>
#include <XFileFilter.h>
#include <XFolderFilter.h>
#include <XFolderSpec.h>
#include <XObjectModel.h>
#include <XRegisterUI.h>
#include <XStringUtils.h>
#include <XTrace.h>
#include <XTraceSinks.h>
#include <XURI.h>

using namespace Whisper;


//-----------------------------------
//	Types
//
typedef std::set<std::wstring> StringSet;


//-----------------------------------
//	Variables
//
static StringSet sExtensions;


// ===================================================================================
//	class CMyFileFilter
//		$$$ When we get a version of string that supports a traits template argument
//		$$$�we need to switch to a string type that does a case insensitive compare.
// ===================================================================================
class CMyFileFilter : public XFileFilter {

public:
						CMyFileFilter();
						
	virtual bool		operator()(const XFileSpec& file) const;
};


//---------------------------------------------------------------
//
// CMyFileFilter::CMyFileFilter
//
//---------------------------------------------------------------
CMyFileFilter::CMyFileFilter() 
{
}


//---------------------------------------------------------------
//
// CMyFileFilter::operator()
//
//---------------------------------------------------------------
bool CMyFileFilter::operator()(const XFileSpec& file) const
{
	bool good = true;

#if MAC
	good = file.GetType() == 'TEXT';
#endif
	
	if (good) {	
		// Make sure the file's extension is in our list.
		std::wstring name = file.GetName();
		uint32 index = name.rfind('.');
		if (index != std::wstring::npos) {
			std::wstring extension = Whisper::ConvertToLowerCase(name.substr(index));
			StringSet::iterator iter = sExtensions.find(extension);
			good = iter != sExtensions.end();
			
		} else
			good = false;
	}
	
	return good;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class CMyFolderFilter
// ===================================================================================
class CMyFolderFilter : public XFolderFilter {

public:
	virtual bool		operator()(const XFolderSpec& folder) const;
};


//---------------------------------------------------------------
//
// CMyFolderFilter::operator()
//
//---------------------------------------------------------------
bool CMyFolderFilter::operator()(const XFolderSpec& folder) const
{
	std::wstring name = folder.GetName();
	
	uint32 len = name.length();
	bool shielded = name[0] == '(' && name[len-1] == ')';
	
	return !shielded;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Internal Functions
// ===================================================================================

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

	// Load our extensions table.
	Handle extensions = GetResource('STR#', 256);
	ThrowIfResFail(extensions);
	HNoPurge(extensions);
	
	int16* ptr = reinterpret_cast<int16*>(*extensions);
	int16 count = *ptr;
	
	Str255 str;
	for (int16 index = 1; index <= count; index++) {
		GetIndString(str, 256, index);
		
		sExtensions.insert(ConvertToLowerCase(FromPascalStr(str))); 
	}
	
	ReleaseResource(extensions);
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

			drop->Start(new CMyFileFilter, new CMyFolderFilter);

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


