/*
 *  File:       WSystemInfo.cpp
 *	Summary:	Provides information about the system.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WSystemInfo.cpp,v $
 *		Revision 1.3  2001/04/21 03:34:02  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 12:12:18  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <WSystemInfo.h>

#include <XExceptions.h>
#include <XNumbers.h>

namespace Whisper {


// ===================================================================================
//	class WSystemInfo	
// ===================================================================================

std::wstring	WSystemInfo::msPrefsName;

//---------------------------------------------------------------
//
// WSystemInfo::GetAppInstance							[static]
//
//---------------------------------------------------------------
HINSTANCE WSystemInfo::GetAppInstance()						
{
	static HINSTANCE instance = nil;
	
	if (instance == nil) {
		instance = GetModuleHandle(nil);	// per _Advanced Windows_ by Richter
		ASSERT(instance != nil);
	}
	
	return instance;
}


//---------------------------------------------------------------
//
// WSystemInfo::GetAppPath								[static]
//
//---------------------------------------------------------------
std::wstring WSystemInfo::GetAppPath()
{
	std::wstring path;					// recompute this each time to be thread safe...
	
	if (WSystemInfo::HasUnicode()) {
		wchar_t buffer[_MAX_PATH];
		uint32 count = ::GetModuleFileNameW(WSystemInfo::GetAppInstance(), buffer, sizeof(buffer));
		ThrowIf(count == 0);
		
		path.assign(buffer, count);
	
	} else {
		char buffer[_MAX_PATH];
		uint32 count = ::GetModuleFileNameA(WSystemInfo::GetAppInstance(), buffer, sizeof(buffer));
		ThrowIf(count == 0);
		
		path = FromPlatformStr(buffer, count);
	}
	
	return path;
}


//---------------------------------------------------------------
//
// WSystemInfo::GetAppName								[static]
//
//---------------------------------------------------------------
std::wstring WSystemInfo::GetAppName()
{
	std::wstring path = WSystemInfo::GetAppPath();
	
	uint32 pos = path.rfind('\\');	
	std::wstring name = path.substr(pos + 1);
	
	return name;
}


//---------------------------------------------------------------
//
// WSystemInfo::IsNT									[static]
//
//---------------------------------------------------------------
bool WSystemInfo::IsNT()
{
	static bool is = false;
	static bool inited = false;
	
	if (!inited) {
		OSVERSIONINFO info;
		info.dwOSVersionInfoSize = sizeof(info);

		if (GetVersionEx(&info))
			is = info.dwPlatformId == VER_PLATFORM_WIN32_NT;

		inited = true;
	}
	
	return is;
}


//---------------------------------------------------------------
//
// WSystemInfo::IsWin32									[static]
//
//---------------------------------------------------------------
bool WSystemInfo::IsWin32()
{
	static bool is = false;
	static bool inited = false;
	
	if (!inited) {
		OSVERSIONINFO info;
		info.dwOSVersionInfoSize = sizeof(info);

		if (GetVersionEx(&info))
			is = info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS || info.dwPlatformId == VER_PLATFORM_WIN32_NT;

		inited = true;
	}
	
	return is;
}


//---------------------------------------------------------------
//
// WSystemInfo::IsWin32s								[static]
//
//---------------------------------------------------------------
bool WSystemInfo::IsWin32s()
{
	static bool is = false;
	static bool inited = false;
	
	if (!inited) {
		OSVERSIONINFO info;
		info.dwOSVersionInfoSize = sizeof(info);

		if (GetVersionEx(&info))
			is = info.dwPlatformId == VER_PLATFORM_WIN32s;

		inited = true;
	}
	
	return is;
}


//---------------------------------------------------------------
//
// WSystemInfo::IsWin95OSR2								[static]
//
//---------------------------------------------------------------
bool WSystemInfo::IsWin95OSR2()
{
	static bool is = false;
	static bool inited = false;
	
	if (!inited) {
		OSVERSIONINFO info;
		info.dwOSVersionInfoSize = sizeof(info);

		if (GetVersionEx(&info)) {
			if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
				if (info.dwMinorVersion >= 10)
					is = true;
				else if (LOWORD(info.dwBuildNumber) > 1080)
					is = true;
			}
		}

		inited = true;
	}
	
	return is;
}


//---------------------------------------------------------------
//
// WSystemInfo::IsWin98									[static]
//
//---------------------------------------------------------------
bool WSystemInfo::IsWin98()
{
	static bool is = false;
	static bool inited = false;
	
	if (!inited) {
		OSVERSIONINFO info;
		info.dwOSVersionInfoSize = sizeof(info);

		if (GetVersionEx(&info)) 
			if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) 
				is = info.dwMinorVersion >= 10;

		inited = true;
	}
	
	return is;
}


//---------------------------------------------------------------
//
// WSystemInfo::IsWin2000								[static]
//
//---------------------------------------------------------------
bool WSystemInfo::IsWin2000()
{
	static bool is = false;
	static bool inited = false;
	
	if (!inited) {
		OSVERSIONINFO info;
		info.dwOSVersionInfoSize = sizeof(info);

		if (GetVersionEx(&info))
			is = info.dwPlatformId == VER_PLATFORM_WIN32_NT &&
				 info.dwMajorVersion >= 5;

		inited = true;
	}
	
	return is;
}


//---------------------------------------------------------------
//
// WSystemInfo::GetMajorOSVersion						[static]
//
//---------------------------------------------------------------
uint8 WSystemInfo::GetMajorOSVersion()
{
	static uint8 version = 0;
	static bool inited = false;
	
	if (!inited) {
		OSVERSIONINFO info;
		info.dwOSVersionInfoSize = sizeof(info);

		if (GetVersionEx(&info))
			version = (uint8) info.dwMajorVersion;

		inited = true;
	}
	
	return version;
}


//---------------------------------------------------------------
//
// WSystemInfo::GetMinorOSVersion						[static]
//
//---------------------------------------------------------------
uint8 WSystemInfo::GetMinorOSVersion()
{
	static uint8 version = 0;
	static bool inited = false;
	
	if (!inited) {
		OSVERSIONINFO info;
		info.dwOSVersionInfoSize = sizeof(info);

		if (GetVersionEx(&info))
			version = (uint8) info.dwMinorVersion;

		inited = true;
	}
	
	return version;
}


//---------------------------------------------------------------
//
// WSystemInfo::GetPageSize								[static]
//
//---------------------------------------------------------------
uint32 WSystemInfo::GetPageSize()
{
	static uint32 pageSize = 0;
	
	if (pageSize == 0) {
		SYSTEM_INFO info;
		GetSystemInfo(&info);

		pageSize = info.dwPageSize;
		ASSERT(pageSize > 0);
	}
	
	return pageSize;
}


//---------------------------------------------------------------
//
// WSystemInfo::SetPrefsName							[static]
//
// Note that it's important that this be done in the cpp: MSVC
// won't initialize msPrefsName until a function in the cpp is
// called so if this is done in the header file msPrefsName can
// be overwritten when the first cpp method is called.
//
//---------------------------------------------------------------
void WSystemInfo::SetPrefsName(const std::wstring& name)		
{
	PRECONDITION(name.size() > 0); 
	
	msPrefsName = name;
}


}	// namespace Whisper
