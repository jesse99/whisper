/*
 *  File:       XSystemInfo.cpp
 *	Summary:	Provides information about the system.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XSystemInfo.cpp,v $
 *		Revision 1.3  2000/12/10 04:12:11  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:55:08  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 8/26/00	JDJ		Fixed GetUserName and GetComputerName so they
 *									return more than just the first character of the 
 *									name on Windows.
 *		 <4>	 6/24/99	JDJ		Implemented GetUserName on Macs.
 *		 <3>	 5/28/99	JDJ		Made thread safe.
 *		 <2>	 2/25/99	JDJ		Added GetAppName.
 *		 <1>	11/17/97	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XSystemInfo.h>

#include <XDebug.h>
#include <XLocker.h>
#include <XNumbers.h>
#include <XTranscode.h>

#if MAC
	#include <Gestalt.h>
	#include <MSystemInfo.h>
#elif WIN
	#include <WSystemInfo.h>
	#include <XExceptions.h>
#endif

namespace Whisper {


// ===================================================================================
//	class XSystemInfo
// ===================================================================================

//---------------------------------------------------------------
//
// XSystemInfo::GetProcessorCount						[static]
//
//---------------------------------------------------------------
uint32 XSystemInfo::GetProcessorCount()
{
	static uint32 count = 1;
	static bool inited = false;
	
	if (!inited) {
#if MAC
		count = 1;				// Macs can have more than 1 processor, but they're of limited use.

#elif WIN
		SYSTEM_INFO info;
		GetSystemInfo(&info);

		count = info.dwNumberOfProcessors;
#endif

		inited = true;
	}
	
	return count;
}


//---------------------------------------------------------------
//
// XSystemInfo::HasFPU									[static]
//
//---------------------------------------------------------------
bool XSystemInfo::HasFPU()
{
#if MAC
	int32 result;
	static bool has = Gestalt(gestaltFPUType, &result) == noErr && result != gestaltNoFPU;

#elif WIN
	// $$$ I can't find any Windows functions that return this 
	// $$$ info. I also can't find any MSVC compiler options for 
	// $$$ targetting machines without an FPU...
	static bool has = true;
#endif

	return has;
}


//---------------------------------------------------------------
//
// XSystemInfo::GetLogicalRAM							[static]
//
//---------------------------------------------------------------
uint32 XSystemInfo::GetLogicalRAM()
{
	static uint32 bytes = 0;
	static bool inited = false;
	
	if (!inited) {
#if MAC
		VERIFY(Gestalt(gestaltLogicalRAMSize, (int32*) &bytes) == noErr);

#elif WIN
		MEMORYSTATUS status;
		status.dwLength = sizeof(status);

		GlobalMemoryStatus(&status);
		bytes = status.dwTotalVirtual;
#endif

		inited = true;
	}
	
	return bytes;
}


//---------------------------------------------------------------
//
// XSystemInfo::GetPhysicalRAM							[static]
//
//---------------------------------------------------------------
uint32 XSystemInfo::GetPhysicalRAM()
{
	static uint32 bytes = 0;
	static bool inited = false;
	
	if (!inited) {
#if MAC
		VERIFY(Gestalt(gestaltPhysicalRAMSize, (int32*) &bytes) == noErr);

#elif WIN
		MEMORYSTATUS status;
		status.dwLength = sizeof(status);

		GlobalMemoryStatus(&status);
		bytes = status.dwTotalPhys;
#endif

		inited = true;
	}
	
	return bytes;
}


//---------------------------------------------------------------
//
// XSystemInfo::IsVirtualMemOn							[static]
//
//---------------------------------------------------------------
bool XSystemInfo::IsVirtualMemOn()
{
#if MAC
	const long	kGestaltVMPresentBits = (1 << gestaltVMPresent);
	static bool has = TestGestaltMask(gestaltVMAttr, kGestaltVMPresentBits);

#elif WIN
	static bool has = true;
#endif

	return has;
}


//---------------------------------------------------------------
//
// XSystemInfo::GetAppName								[static]
//
//---------------------------------------------------------------
std::wstring XSystemInfo::GetAppName()
{	
#if MAC
	FSSpec spec = MSystemInfo::GetAppSpec();		// to be thread safe we'll re-compute this each time
	std::wstring name = FromPascalStr(spec.name);

#elif WIN
	std::wstring path = WSystemInfo::GetAppPath();
	
	uint32 pos = path.rfind('\\');	
	std::wstring name = path.substr(pos + 1);
#endif

	uint32 p = name.rfind('.');	
	if (p != std::wstring::npos)
		name = name.substr(0, p);

	return name;
}


//---------------------------------------------------------------
//
// XSystemInfo::GetComputerName							[static]
//
//---------------------------------------------------------------
std::wstring XSystemInfo::GetComputerName()
{
	std::wstring name;								// to be thread safe we'll re-compute this each time

#if MAC	
	Handle strH = GetResource('STR ', -16413);
	if (strH != nil) {
		char buffer[256];

		uint32 len = **strH;
		BlockMoveData(*strH + 1, buffer, len);
		buffer[len] = '\0';
		
		name = FromPlatformStr(buffer);
		
		ReleaseResource(strH);
	
	} else
		DEBUGSTR("GetComputerName failed with error ", ResError());

#elif WIN
	uint32 length = MAX_COMPUTERNAME_LENGTH + 1;
	if (WSystemInfo::HasUnicode()) {
		wchar_t buffer[MAX_COMPUTERNAME_LENGTH + 1];			// docs say buffer needs to be MAX_COMPUTERNAME_LENGTH plus one...
		int32 succeeded = ::GetComputerNameW(buffer, &length);
		ThrowIf(!succeeded);
		
		name.assign(buffer, numeric_cast<uint32>(length));
	
	} else {
		char buffer[MAX_COMPUTERNAME_LENGTH + 1];			
		int32 succeeded = ::GetComputerNameA(buffer, &length);
		ThrowIf(!succeeded);
		
		name = FromPlatformStr(buffer, numeric_cast<uint32>(length));
	}		
#endif
	
	return name;
}


//---------------------------------------------------------------
//
// XSystemInfo::GetUserName								[static]
//
//---------------------------------------------------------------
std::wstring XSystemInfo::GetUserName()
{
	std::wstring name;											// to be thread safe we'll re-compute this each time
	
#if MAC
	StringHandle strH = ::GetString(-16096);
	if (strH != nil && **strH != 0) {
		{
		XLocker lock((Handle) strH);
			name = FromPascalStr(*strH);
		}
	
		ReleaseResource((Handle) strH);
	}

#elif WIN
	uint32 length = MAX_COMPUTERNAME_LENGTH;	
	if (WSystemInfo::HasUnicode()) {
		wchar_t buffer[MAX_COMPUTERNAME_LENGTH + 1];			// docs say buffer needs to be MAX_COMPUTERNAME_LENGTH plus one...
		int32 succeeded = ::GetUserNameW(buffer, &length);
		ThrowIf(!succeeded);
		
		name.assign(buffer, numeric_cast<uint32>(length));
	
	} else {
		char buffer[MAX_COMPUTERNAME_LENGTH + 1];			
		int32 succeeded = ::GetUserNameA(buffer, &length);
		ThrowIf(!succeeded);
		
		name = FromPlatformStr(buffer, numeric_cast<uint32>(length));
	}		
#endif
	
	return name;
}


}	// namespace Whisper
