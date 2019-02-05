/*
 *  File:       CPlugin.h
 *  Summary:   	The plugin's startup code.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CPlugin.h,v $
 *		Revision 1.3  2001/03/14 09:48:37  jesjones
 *		Exported Startup and Shutdown on Windows.
 *		
 *		Revision 1.2  2000/11/09 08:56:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTypes.h>

using namespace Whisper;
using namespace std;

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif

#if WIN
	#define EXPORT __declspec(dllexport)
#else
	#define EXPORT 
#endif


// ===================================================================================
//	Global Functions
// ===================================================================================
extern "C" EXPORT void 	Startup();

extern "C" EXPORT void 	Shutdown();


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif
