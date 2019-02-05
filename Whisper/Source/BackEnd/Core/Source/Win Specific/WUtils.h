/*
 *  File:       WUtils.h
 *	Summary:	Misc Windows specific utility functions.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WUtils.h,v $
 *		Revision 1.2  2000/11/09 12:12:39  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <4>	12/22/99	JDJ		Resource functions now take an optional module handle.
 *		 <3>	 8/01/99	JDJ		Added GetText.
 *		 <2>	12/17/98	JDJ		Added HasResource, GetResource, and GetResourceSize.
 *		 <1>	 9/26/98	JDJ		Created
 */

#pragma once

#include <XTypes.h>

namespace Whisper {


// ===================================================================================
//	Global Functions
// ===================================================================================

CORE_EXPORT HWND GetWorkWindow();
			// Returns an offscreen window that can be used whenever you need an HWND
			// but don't have one handy.
			
CORE_EXPORT std::wstring GetText(HWND window);
			// Calls either GetWindowTextA or GetWindowTextW.

CORE_EXPORT bool HasResource(const TCHAR* type, int16 id, HINSTANCE moduleH = nil);
CORE_EXPORT bool HasResource(const TCHAR* type, const std::wstring& name, HINSTANCE moduleH = nil);
			// Can't use a real string for type because of Microsoft's propensity
			// for type unsafe interfaces (eg constants like RT_RCDATA are invalid
			// string pointers).

CORE_EXPORT const void* GetResource(const TCHAR* type, int16 id, HINSTANCE moduleH = nil);
CORE_EXPORT const void* GetResource(const TCHAR* type, const std::wstring& name, HINSTANCE moduleH = nil);

CORE_EXPORT uint32 GetResourceSize(const TCHAR* type, int16 id, HINSTANCE moduleH = nil);
CORE_EXPORT uint32 GetResourceSize(const TCHAR* type, const std::wstring& name, HINSTANCE moduleH = nil);

						
}	// namespace Whisper
