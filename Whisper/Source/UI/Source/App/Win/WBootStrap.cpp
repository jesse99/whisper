/*
 *  File:       WBootStrap.cpp
 *  Summary:   	Functions used to intialize and terminate UI apps.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WBootStrap.cpp,v $
 *		Revision 1.10  2001/04/27 09:18:01  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.9  2001/04/21 03:30:17  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.8  2001/03/16 10:23:17  jesjones
 *		Fixed gExitingNormally for Intel DLL targets.
 *		
 *		Revision 1.7  2001/03/06 07:31:51  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.6  2001/03/01 11:43:03  jesjones
 *		Added #if DEBUG to a bit of code.
 *		
 *		Revision 1.5  2001/01/21 00:41:52  jesjones
 *		Added an include.
 *		
 *		Revision 1.4  2001/01/05 06:22:39  jesjones
 *		Major refactoring.
 *		
 *		Revision 1.3  2000/11/20 05:46:55  jesjones
 *		Added std:: to a few function calls.
 *		
 *		Revision 1.2  2000/11/09 12:04:50  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XBootStrap.h>

#include <WSystemInfo.h>
#include <XDebugMalloc.h>
#include <XDebugNew.h>
#include <XObjectModel.h>
#include <XStringUtils.h>
#include <XTranscode.h>

namespace Whisper { 
 
#if DEBUG
	extern RUNTIME_EXPORT bool gExitingNormally;
#endif


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// InitUI
//
//---------------------------------------------------------------
void InitUI()
{
#if DEBUG
	gExitingNormally = false;
#endif

#if WHISPER_OPERATOR_NEW && DEBUG
	gDebugMalloc->EnableLeakChecking();		// we don't leak check static ctors
#endif
}


//---------------------------------------------------------------
//
// ShutDownUI
//
//---------------------------------------------------------------
void ShutDownUI()
{
#if DEBUG
	gExitingNormally = true;
#endif

	XObjectModel::Instance()->Shutdown();
}


//---------------------------------------------------------------
//
// DefaultSystemCheck
//
//---------------------------------------------------------------
void DefaultSystemCheck(std::list<std::wstring>& needs)
{
	if (!WSystemInfo::IsWin32())
		needs.push_back(LoadWhisperString(L"NT 4 or Win 95 or later"));					
	
	if (!WSystemInfo::IsNT() && WSystemInfo::GetMajorOSVersion() < 4)
		needs.push_back(LoadWhisperString(L"NT 4 or later"));					
}


//---------------------------------------------------------------
//
// HandleBadSystem
//
//---------------------------------------------------------------
void HandleBadSystem(const std::list<std::wstring>& needs)
{
	PRECONDITION(!needs.empty());
	
	// Build a string containing all of the reasons the app won't run.
	std::wstring mesg;
	if (needs.size() == 1)
		mesg += needs.front();
	
	else if (needs.size() == 2)
		mesg += needs.front() + LoadWhisperString(L" and ") + needs.back();
	
	else {
		std::list<std::wstring>::const_iterator iter = needs.begin();
		while (iter != needs.end()) {
			std::wstring item = *iter++;
			
			if (iter == needs.end())
				mesg += LoadWhisperString(L"and ");
			mesg += item;
			if (iter != needs.end())
				mesg += LoadWhisperString(L", ");
		}
	}

	// Put up an alert to let the user know how badly their PC sucks.
	std::wstring errorStr = LoadWhisperString(L"Unable to run the app because the system components below are missing:");
	std::wstring text = errorStr + L"\n" + mesg;
	
	uint32 flags = MB_OK +				// just an OK button
				   MB_ICONSTOP +		// display the stop sign icon
				   MB_DEFBUTTON1 +		// select the OK button
				   MB_TASKMODAL +		// don't let the user do anything else in the app
				   MB_SETFOREGROUND;	// bring the app to the front

	if (WSystemInfo::HasUnicode())
		(void) MessageBoxW(nil, text.c_str(), nil, flags);
	else
		(void) MessageBoxA(nil, ToPlatformStr(text).c_str(), nil, flags);
		
	// Bail
	std::exit(1);
}


}	// namespace Whisper

