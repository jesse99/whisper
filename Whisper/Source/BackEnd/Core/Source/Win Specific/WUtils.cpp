/*
 *  File:       WUtils.cpp
 *	Summary:	Misc Windows specific utility functions.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WUtils.cpp,v $
 *		Revision 1.5  2001/04/27 09:19:49  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.4  2001/04/21 03:34:21  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2000/12/10 04:08:07  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:12:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	12/22/99	JDJ		Resource functions now take an optional module handle.
 *		 <4>	 8/01/99	JDJ		Added GetText.
 *		 <3>	 6/10/99	JDJ		GetWorkWindow uses WS_EX_TOOLWINDOW so it no longer
 *									appears in the taskbar.
 *		 <2>	12/17/98	JDJ		Added HasResource, GetResource, and GetResourceSize.
 *		 <1>	 9/26/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <WUtils.h>

#include <WSystemInfo.h>
#include <XExceptions.h>
#include <XNumbers.h>
#include <XTinyVector.h>

namespace Whisper {


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetWorkWindow	
//
//---------------------------------------------------------------
HWND GetWorkWindow()
{
	static HWND window = nil;
		
	if (window == nil) {
		window = CreateWindowEx(WS_EX_TOOLWINDOW,		// extended style
								"STATIC",				// window class
							    "Work Window",			// window name
							    WS_POPUP + WS_VISIBLE,	// window style 
							    -10000,					// horz position
							    -10000,					// vert position
							    100,					// window width
							    100,					// window height
							    nil,					// parent window
							    nil,					// menu or child-window identifier
							    WSystemInfo::GetAppInstance(),
							    nil);					// refcon
		ThrowIfBadHandle(window);
	}

	return window;
}

						
//---------------------------------------------------------------
//
// GetText
//
//---------------------------------------------------------------
std::wstring GetText(HWND window)
{
	PRECONDITION(window != nil);
	
	std::wstring text;
	
	int32 length = GetWindowTextLength(window);
	if (WSystemInfo::HasUnicode()) {
		XTinyVector<wchar_t> buffer(length+1UL);
		int32 count = GetWindowTextW(window, buffer.buffer(), (int32) buffer.size());
		text = std::wstring(buffer.buffer(), numeric_cast<uint32>(count));
	
	} else {
		XTinyVector<char> buffer2(length+1UL);
		int32 count2 = GetWindowTextA(window, buffer2.buffer(), (int32) buffer2.size());
		text = FromPlatformStr(buffer2.buffer(), numeric_cast<uint32>(count2));
	}

	return text;
}


//---------------------------------------------------------------
//
// HasResource (const TCHAR*, int16, HINSTANCE)
//
//---------------------------------------------------------------
bool HasResource(const TCHAR* type, int16 id, HINSTANCE moduleH)
{	
	PRECONDITION(type != nil);
	
	if (moduleH == nil)
		moduleH = WSystemInfo::GetAppInstance();
	
	HRSRC rsrc = FindResource(moduleH, MAKEINTRESOURCE(id), type);
	
	return rsrc != nil && rsrc != INVALID_HANDLE_VALUE;
}


//---------------------------------------------------------------
//
// HasResource (const TCHAR*, wstring, HINSTANCE)
//
//---------------------------------------------------------------
bool HasResource(const TCHAR* type, const std::wstring& id, HINSTANCE moduleH)
{	
	PRECONDITION(type != nil);
		
	if (moduleH == nil)
		moduleH = WSystemInfo::GetAppInstance();
	
	HRSRC rsrc = FindResource(moduleH, ToPlatformStr(id).c_str(), type);
	
	return rsrc != nil && rsrc != INVALID_HANDLE_VALUE;
}


//---------------------------------------------------------------
//
// GetResource (const TCHAR*, int16, HINSTANCE)
//
//---------------------------------------------------------------
const void* GetResource(const TCHAR* type, int16 id, HINSTANCE moduleH)
{
	PRECONDITION(type != nil);
	
	if (moduleH == nil)
		moduleH = WSystemInfo::GetAppInstance();
	
	HRSRC rsrc = FindResource(moduleH, MAKEINTRESOURCE(id), type);
	ThrowIfBadHandle(rsrc);
	
	HGLOBAL data = ::LoadResource(moduleH, rsrc);
	ThrowIfBadHandle(data);	

	const void* ptr = ::LockResource(data);
	ThrowIfNil(ptr);
	
	return ptr;
}


//---------------------------------------------------------------
//
// GetResource (const TCHAR*, wstring, HINSTANCE)
//
//---------------------------------------------------------------
const void* GetResource(const TCHAR* type, const std::wstring& id, HINSTANCE moduleH)
{
	PRECONDITION(type != nil);
	
	if (moduleH == nil)
		moduleH = WSystemInfo::GetAppInstance();
	
	HRSRC rsrc = FindResource(moduleH, ToPlatformStr(id).c_str(), type);
	ThrowIfBadHandle(rsrc);
	
	HGLOBAL data = ::LoadResource(moduleH, rsrc);
	ThrowIfBadHandle(data);	

	const void* ptr = ::LockResource(data);
	ThrowIfNil(ptr);
	
	return ptr;
}


//---------------------------------------------------------------
//
// GetResourceSize (const TCHAR*, int16, HINSTANCE)
//
//---------------------------------------------------------------
uint32 GetResourceSize(const TCHAR* type, int16 id, HINSTANCE moduleH)
{
	PRECONDITION(type != nil);
	
	if (moduleH == nil)
		moduleH = WSystemInfo::GetAppInstance();
	
	HRSRC rsrc = FindResource(moduleH, MAKEINTRESOURCE(id), type);
	ThrowIfBadHandle(rsrc);
	
	uint32 bytes = ::SizeofResource(moduleH, rsrc);
	
	return bytes;
}


//---------------------------------------------------------------
//
// GetResourceSize (const TCHAR*, wstring, HINSTANCE)
//
//---------------------------------------------------------------
uint32 GetResourceSize(const TCHAR* type, const std::wstring& id, HINSTANCE moduleH)
{
	PRECONDITION(type != nil);
		
	if (moduleH == nil)
		moduleH = WSystemInfo::GetAppInstance();
	
	HRSRC rsrc = FindResource(moduleH, ToPlatformStr(id).c_str(), type);
	ThrowIfBadHandle(rsrc);
	
	uint32 bytes = ::SizeofResource(moduleH, rsrc);
	
	return bytes;
}


}	// namespace Whisper
