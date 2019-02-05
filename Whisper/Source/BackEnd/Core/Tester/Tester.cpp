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
 *		Revision 1.4  2001/04/13 07:59:07  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2000/11/20 05:36:50  jesjones
 *		main returns 0.
 *		
 *		Revision 1.2  2000/11/09 12:03:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>

#include <XDebugMalloc.h>
#include <XDebugNew.h>
#include <XMiscUtils.h>
#include <XRegisterCoreTests.h>
#include <XTrace.h>
#include <XTraceSinks.h>
#include <XTranscode.h>
#include <XUnitTests.h>

using namespace Whisper;

namespace Whisper {
	extern bool gExitingNormally;
}


//----------------------------------------------------------------
//
// main
//
//----------------------------------------------------------------
int main()
{
	try {
		gExitingNormally = false;
		gDebugMalloc->EnableLeakChecking();		// we don't leak check static ctors
		
		gIntenseDebugging = true;
		
//		(void) new int32;
		
		XDebuggerSink sink;
		XTrace::Instance()->AddSink(&sink);
		
		RegisterCoreTests();		
		XUnitTests::Instance()->RunAllTests();
		
		XTrace::Instance()->RemoveSink(&sink);
		
		gExitingNormally = true;
		
	} catch (std::exception& e) {
		DEBUGSTR("Caught an exception:", e.what());
		
	} catch (...) {
		DEBUGSTR("Caught an exception!");
	}
	
	return 0;
}


