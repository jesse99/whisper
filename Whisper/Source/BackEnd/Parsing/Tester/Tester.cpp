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
 *		Revision 1.2  2000/11/09 12:03:52  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 2/04/00	JDJ		Created 
 */

#include <XWhisperHeader.h>

#include <XDebugMalloc.h>
#include <XDebugNew.h>
#include <XExceptions.h>
#include <XPreference.h>
#include <XRegisterCoreTests.h>
#include <XRegisterParseTests.h>
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
	
	XDebuggerSink sink;
	XTrace::Instance()->AddSink(&sink);
	
	RegisterCoreTests();
	RegisterParseTests();
	XUnitTests::Instance()->RunAllTests();
		
	XTrace::Instance()->RemoveSink(&sink);
	
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
