/*
 *  File:       Tester.cpp
 *  Summary:    Console app that runs unit tests.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: Tester.cpp,v $
 *		Revision 1.3  2001/10/03 02:09:27  jesjones
 *		Removed references to MPreferences.
 *		
 *		Revision 1.2  2000/11/09 12:03:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: Tester.cpp,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2001/10/03 02:09:27  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Removed references to MPreferences.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 2/04/00	JDJ		Created 
 */

#include <XWhisperHeader.h>

#include <XDebugMalloc.h>
#include <XDebugNew.h>
#include <XExceptions.h>
#include <XPreference.h>
#include <XRegisterCoreTests.h>
#include <XRegisterFileTests.h>
#include <XRegisterGraphicTests.h>
#include <XTrace.h>
#include <XTraceSinks.h>
#include <XUnitTests.h>

#if WIN
	#include <objbase.h>
#endif

using namespace Whisper;

namespace Whisper {
	extern bool gExitingNormally;
}


//----------------------------------------------------------------
//
// DoTest	
//
//----------------------------------------------------------------
static void DoTest()
{
	gExitingNormally = false;
	gDebugMalloc->EnableLeakChecking();		// we don't leak check static ctors
	
	gIntenseDebugging = true;
	
	XDebuggerSink sink1;
	XTrace::Instance()->AddSink(&sink1);
	
	XDebugLogSink sink2("Debug Log.txt");
	XTrace::Instance()->AddSink(&sink2);
	
//	RegisterCoreTests();
//	RegisterFileTests();
	RegisterGraphicTests();
	XUnitTests::Instance()->RunAllTests();
		
	XTrace::Instance()->RemoveSink(&sink1);
	XTrace::Instance()->RemoveSink(&sink2);
	
	gExitingNormally = true;
}


//----------------------------------------------------------------
//
// WinMain	
//
//----------------------------------------------------------------
#if WIN
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int showFlags)
{
	UNUSED(instance);
	UNUSED(prevInstance);
	UNUSED(cmdLine);
	UNUSED(showFlags);

#if _WIN32_WINNT >= 0x0400
	HRESULT result = CoInitializeEx(nil, COINIT_SPEED_OVER_MEMORY | COINIT_APARTMENTTHREADED);	// IShellLink will only work if we specify COINIT_APARTMENTTHREADED...
#else
	HRESULT result = CoInitialize(nil);
#endif
	ThrowIfFailed(result);
	
	DoTest();
	
	return 0;
}
#endif


//----------------------------------------------------------------
//
// main	
//
//----------------------------------------------------------------
#if !WIN
void main()
{
	DoTest();
}
#endif
