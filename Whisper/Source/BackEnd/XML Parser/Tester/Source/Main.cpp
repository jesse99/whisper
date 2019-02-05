/*
 *  File:       Main.cpp
 *  Summary:    Console app that runs the Oasis XML conformance tests.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: Main.cpp,v $
 *		Revision 1.2  2000/11/09 09:09:27  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 2/04/00	JDJ		Created 
 */

#include <XWhisperHeader.h>

#include <XDebugMalloc.h>
#include <XDebugNew.h>
#include <XFileSpec.h>
#include <XFolderSpec.h>
#include <XPreference.h>
#include <XTrace.h>
#include <XTraceSinks.h>

#include "CTestConformance.h"

namespace Whisper {
	extern bool gExitingNormally;
}


//----------------------------------------------------------------
//
// main
//
//----------------------------------------------------------------
void main()
{
	gExitingNormally = false;
	gDebugMalloc->EnableLeakChecking();		// we don't leak check static ctors
	
	gIntenseDebugging = true;
	
	XDebuggerSink sink1;
	XTrace::Instance()->AddSink(&sink1);
	
	XDebugLogSink sink2("Debug Log.txt");
	XTrace::Instance()->AddSink(&sink2);
	
	try {
		XFileSpec spec(XFolderSpec::GetAppFolder(), L"xmlconf.xml");
		CTestConformance tester(spec);
		
	} catch (const std::exception& e) {
		TRACE("Caught an ", e.what(), " exception!");
		
	} catch (...) {
		TRACE("Caught an unknown exception!");
	}

	XTrace::Instance()->RemoveSink(&sink1);
	XTrace::Instance()->RemoveSink(&sink2);
	
	gExitingNormally = true;
}


