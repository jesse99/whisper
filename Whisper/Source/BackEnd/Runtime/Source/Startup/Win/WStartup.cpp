/*
 *  File:       WStartup.cpp
 *  Summary:   	Initialization and termination routines for the app.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WStartup.cpp,v $
 *		Revision 1.6  2001/03/14 11:07:37  jesjones
 *		Updated the ifdefed out entry point with something that should work in Pro 6.
 *		
 *		Revision 1.5  2001/03/13 11:07:11  jesjones
 *		Works when building DLL Intel targets.
 *		
 *		Revision 1.4  2000/11/20 05:45:25  jesjones
 *		Removed dollar signs in the initial checkin comment.
 *		
 *		Revision 1.3  2000/11/10 06:47:32  jesjones
 *		gExitingNormally defaults to false instead of true.
 *		
 *		Revision 1.2  2000/11/09 12:12:07  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>

#include <XDebug.h>
#include <XDebugMalloc.h>
#include <XDebugNew.h>

namespace Whisper {

RUNTIME_EXPORT bool gInitedApp = false;
RUNTIME_EXPORT bool gExitingNormally = false;


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// DumpLeaks
//
//---------------------------------------------------------------
#if DEBUG && WHISPER_OPERATOR_NEW
static void DumpLeaks()
{
	if (gExitingNormally) {			// don't bother with leaks if we're terminating abnormally	
		uint32 leaks = gDebugMalloc->GetLeakCount();
		if (leaks > 0)
			gDebugMalloc->DumpLeaks();

		if (leaks == 1)
			DEBUGSTR("You have one memory leak: check the 'Leaks Log.txt' file.");
		else if (leaks > 0)
			DEBUGSTR("You have ", leaks, " memory leaks: check the 'Leaks Log.txt' file.");
	} 
}
#endif

}	// namespace Whisper

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Entry Point
// ===================================================================================

//---------------------------------------------------------------
//
// AppStartup
//
// $$$ DLLs were hosed in Pro 5, but I think this will work in
// $$$ Pro 6 if this file is linked before MWCRTLDLL.LIB and 
// $$$ MWCRTLDLLD.LIB...
//
//---------------------------------------------------------------
#if 0
extern "C" BOOL __stdcall DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved);     
BOOL __stdcall DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{	
#if DEBUG && WHISPER_OPERATOR_NEW
	if (reason == DLL_PROCESS_DETACH)
		Whisper::DumpLeaks();
#endif
	
	return 1;
}
#endif


