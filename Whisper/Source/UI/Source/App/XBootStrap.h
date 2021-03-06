/*
 *  File:       XBootStrap.h
 *  Summary:   	Functions used to intialize and terminate UI apps.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XBootStrap.h,v $
 *		Revision 1.6  2001/02/07 04:57:40  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.5  2001/01/21 00:42:12  jesjones
 *		Removed an include.
 *		
 *		Revision 1.4  2001/01/05 06:23:49  jesjones
 *		Major refactoring.
 *		
 *		Revision 1.3  2000/12/14 08:32:54  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:26:35  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <list>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	Global Functions
//		It'd probably be classier to do this stuff via interfaces, but the object model
//		does an awful lot of work starting up which could cause problems.
// ===================================================================================
UI_EXPORT void 		InitUI();
					// Does some mostly OS specific initialization. All apps should call this.

UI_EXPORT void 		ShutDownUI();
					// All apps should call this right before quitting.

UI_EXPORT void 		DefaultSystemCheck(std::list<std::wstring>& needs);
					// Checks the system to see if it can run a Whisper UI app. If not the
					// needs array will have the missing functionality, eg "System 7" and
					// "Color QuickDraw".

UI_EXPORT void 		HandleBadSystem(const std::list<std::wstring>& needs);
					// Apps will normally call DefaultSystemCheck, perform their own checks 
					// (eg for the Drag Manager or QuickTime), and then call HandleBadSystem
					// if the needs array isn't empty. HandleBadSystem will pop up an alert
					// and exit.
				
#if MAC && DEBUG
	UI_EXPORT void 	InitSIOUX();
					// Finds the best monitor for the SIOUX debug window and positions it
					// in a reasonable spot.
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

