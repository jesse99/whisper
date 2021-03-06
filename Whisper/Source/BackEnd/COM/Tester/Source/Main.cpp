/*
 *  File:       Main.cpp
 *  Summary:    Console app that runs the Oasis XML conformance tests.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: Main.cpp,v $
 *		Revision 1.6  2001/03/14 09:50:09  jesjones
 *		Removed some bogus code.
 *		
 *		Revision 1.5  2001/03/13 11:04:49  jesjones
 *		Fixed some minor Windows compile errors.
 *		
 *		Revision 1.4  2001/03/06 07:31:12  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.3  2001/03/03 11:15:32  jesjones
 *		Instead of specializing selected algorithms for POD types, we use the new MSL mechanism to declare POD types.
 *		
 *		Revision 1.2  2000/11/09 09:09:23  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>

#include <XDebugMalloc.h>
#include <XDebugNew.h>
#include <XError.h>
#include <XExceptions.h>
#include <XFileFilter.h>
#include <XFileIterator.h>
#include <XFileSpec.h>
#include <XFolderFilter.h>
#include <XFolderSpec.h>
#include <XFragment.h>
#include <XNumbers.h>
#include <XObjectModel.h>
#include <XPlugin.h>
#include <XPreference.h>
#include <XStringUtils.h>
#include <XTrace.h>
#include <XTraceSinks.h>
#include <XURI.h>

#include "CCounter.h"
#include "ICountLines.h"
#include "ICountSemiColons.h"
#include "IPlugins.h"

#if WIN
	#include <objbase.h>
#endif

namespace Whisper {
	extern RUNTIME_EXPORT bool gExitingNormally;
}


//-----------------------------------
//	Variables
//
static vector<XPlugin*>      sPlugins;
static map<std::wstring, std::wstring> sExtMap;	// maps file extensions to boss names


// ===================================================================================
//	class CFragmentFilter		
// ===================================================================================
class CFragmentFilter : public XFileFilter {

public:
	virtual bool 		operator()(const XFileSpec& spec) const;
};


//---------------------------------------------------------------
//
// CFragmentFilter::operator()
//
//---------------------------------------------------------------
bool CFragmentFilter::operator()(const XFileSpec& spec) const
{
	bool valid = false;
	
#if MAC
	valid = spec.GetType() == 'shlb';
#elif WIN
	valid = spec.GetExtension() == L"dll";
#endif

	return valid;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Internal Functions		
// ===================================================================================

//---------------------------------------------------------------
//
// RegisterPlugins
//
//---------------------------------------------------------------
static void RegisterPlugins()
{	
	vector<XLoadFragmentException> errors;
	
	XFolderSpec folder(XFolderSpec::GetAppFolder(), L"Plugins");
	XFolderSpec::iterator iter = folder.begin(new CFragmentFilter, new XFolderFilter, kRecursive);
	while (iter != folder.end()) {
		XFileSpec spec = *iter;
		++iter;
		
		try {
			sPlugins.push_back(new XPlugin(spec));
			
		} catch (const XLoadFragmentException& e) {
			errors.push_back(e);
			
		} catch (const XSystemException& e) {
			errors.push_back(XLoadFragmentException(e.mError, spec.GetName(), spec.GetName()));
						
		} catch (const exception& e) {
			errors.push_back(XLoadFragmentException(e, spec.GetName(), spec.GetName()));
						
		} catch (...) {
			errors.push_back(XLoadFragmentException(1, spec.GetName(), spec.GetName()));
		}
	}
	
	if (!errors.empty()) {
		// If there were 3 or fewer errors then pop up alerts describing exactly what went wrong.
		if (errors.size() <= 3) {			
			for (uint32 i = 0; i < Min((uint32) errors.size(), 3UL); ++i)				
				XError::Instance()->ReportError(LoadAppString(L"Couldn't load one of the effects plugins."), FromPlatformStr(errors[i].what()));
			
		} else {
			// Otherwise pop up an alert describing what went wrong with the first library and
			// list the other plugins that failed to load.
			std::wstring summary   = LoadAppString(L"Couldn't load some of the effects plugins.");
			std::wstring narrative = FromPlatformStr(errors[0].what()) + LoadAppString(L" There were also errors loading the following plugins: ");
						
			for (uint32 i = 1; i < errors.size(); ++i) {
				if (i+1 == errors.size())
					narrative += LoadWhisperString(L"and ");
				narrative += errors[i].mName;
				if (i+1 < errors.size())
					narrative += LoadWhisperString(L", ");
			}
			narrative += L".";
			
			XError::Instance()->ReportError(summary, narrative);
		}
	}
}


//---------------------------------------------------------------
//
// Startup
//
//---------------------------------------------------------------
static void Startup()
{
	REGISTER_INTERFACE_NAME(ICountLines, L"ICountLines");
	REGISTER_INTERFACE_NAME(ICountSemiColons, L"ICountSemiColons");
	REGISTER_INTERFACE_NAME(IPlugins, L"IPlugins");
	
	REGISTER_INTERFACE_FACTORY(CPlugins, L"CPlugins");

	XURI uri(L"resource://Bosses.bXML");
	XObjectModel::Instance()->LoadBosses(nil, uri);

	RegisterPlugins();
}


//---------------------------------------------------------------
//
// Shutdown
//
//---------------------------------------------------------------
static void Shutdown()
{
	for (uint32 i = 0; i < sPlugins.size(); ++i)
		delete sPlugins[i];
	sPlugins.clear();

	XObjectModel::Instance()->UnloadBosses(nil);
	XObjectModel::Instance()->Shutdown();
}


//---------------------------------------------------------------
//
// BuildExtMap
//
//---------------------------------------------------------------
static void BuildExtMap()
{
	std::wstring badExtensions;
	
	IPluginsPtr plugins(L"Plugins Boss");
	for (uint32 i = 0; i < plugins->GetNumPlugins(); ++i) {
		const std::wstring& boss = plugins->GetBoss(i);
		ICountLinesPtr counter(boss);

		for (uint32 j = 0; j < counter->GetNumExtensions(); ++j) {
			std::wstring ext = counter->GetExtension(j);
			bool added = sExtMap.insert(map<std::wstring, std::wstring>::value_type(ext, boss)).second;
			if (!added) 
				badExtensions += ext + L" ";
		}
	}
	
	if (badExtensions.length() > 0)
		XError::Instance()->ReportError(L"Multiple plugins are capable of processing the following extensions:", badExtensions);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Entry Point		
// ===================================================================================

//----------------------------------------------------------------
//
// main
//
//----------------------------------------------------------------
int main()
{	
#if WIN
#if (_WIN32_WINNT >= 0x0400) || defined(_WIN32_DCOM) 
	HRESULT result = CoInitializeEx(nil, COINIT_APARTMENTTHREADED);	// IShellLink doesn't seem to work with COINIT_SPEED_OVER_MEMORY
#else
	HRESULT result = CoInitialize(nil);
#endif
	ThrowIfFailed(result);
#endif

	const uint32 kNumFiles = 3;
	const uint32 kNumLines = 9 + 20 + 36;
	const uint32 kNumSemi  = 6;
	
	gExitingNormally = false;
//	gDebugMalloc->EnableLeakChecking();		// we don't leak check static ctors
	
	gIntenseDebugging = true;
	
	XDebuggerSink sink;
	XTrace::Instance()->AddSink(&sink);
	
	Startup();

	try {
		BuildExtMap();
		
		XFolderSpec spec(XFolderSpec::GetAppFolder(), L"Files");

		CCounter counter(spec, sExtMap);
		uint32 numFiles = counter.GetNumFiles();
		uint32 numLines = counter.GetNumLines();
		uint32 numSemi  = counter.GetNumSemiColons();
				
		if (numFiles == kNumFiles && numLines == kNumLines && numSemi == kNumSemi)
			TRACE("Passed\n");
		else if (numFiles == kNumFiles && numLines != kNumLines)
			TRACE("FAILED: got ", numLines, " lines, but there should have been ", kNumLines, " lines!\n");
		else if (numFiles != kNumFiles && numLines == kNumLines)
			TRACE("FAILED: got ", numFiles, " files, but there should have been ", kNumFiles, " files!\n");
		else
			TRACE("FAILED: got ", numFiles, " files and ", numLines, " lines, but there should have been ", kNumFiles, " files and ", kNumLines, " lines!\n");
				
		if (numSemi != kNumSemi)
			TRACE("FAILED: got ", numSemi, " semi-colons, but there should have been ", kNumSemi, " semi-colons!\n");
				
	} catch (const std::exception& e) {
		TRACE("Caught an ", e.what(), " exception!");
		
	} catch (...) {
		TRACE("Caught an unknown exception!");
	}
		
	Shutdown();

	XTrace::Instance()->RemoveSink(&sink);
	
	gExitingNormally = true;
	
	return 0;
}


