/*
 *  File:       XExceptions.cpp
 *  Summary:   	ANSI derived exception classes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XExceptions.cpp,v $
 *		Revision 1.3  2001/03/03 11:12:36  jesjones
 *		Mac ThrowIfNil only throws an out of memory exception if MemError says we're out of memory.
 *		
 *		Revision 1.2  2000/11/09 12:34:49  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */
 
#include <XWhisperHeader.h>
#include <XExceptions.h>

#include <XDebug.h>
#include <XIntConversions.h>
#include <XStringUtils.h>

#if WIN
	#include <WSystemInfo.h>
#endif

namespace Whisper {


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// ThrowIfNil
//
//---------------------------------------------------------------
#if MAC
void ThrowIfNil(const void* ptr)					
{
	if (ptr == nil) {
		ThrowIfOSErr(MemError());		// if we're out of memory this should throw
		ThrowIfOSErr(ResError());		// might also be called when trying to load a resource

		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Exception: nil pointer.")));	// generic pointer was nil, probably not out of memory...
	}
}
#endif


//---------------------------------------------------------------
//
// ThrowErr
//
//---------------------------------------------------------------
void ThrowErr(OSError err)
{
#if MAC
		std::wstring mesg;
		
		switch (err) {
			case memFullErr:
				throw XSysMemoryException();
				break;
				
			case resNotFound:
			case resFNotFound:
			case addResFailed:
			case rmvResFailed:
			case resAttrErr:
			case mapReadErr:
				mesg = LoadWhisperString(L"Resource error ##1", Int32ToStr(err));
				throw XSystemException(err, mesg);				
				break;
				
			case dirFulErr:
			case dskFulErr:
			case nsvErr:
			case ioErr:
			case fnOpnErr:
			case eofErr:
			case posErr:
			case mFulErr:
			case tmfoErr:
			case fnfErr:
			case wPrErr:
			case fLckdErr:
	
			case vLckdErr:
			case fBsyErr:
			case dupFNErr:
			case opWrErr:
			case rfNumErr:
			case gfpErr:
			case volOffLinErr:
			case permErr:
			case volOnLinErr:
			case nsDrvErr:
			case noMacDskErr:
			case wrPermErr:
			case dirNFErr:
			case tmwdoErr:
			case volGoneErr:
				mesg = LoadWhisperString(L"File error ##1", Int32ToStr(err));
				throw XSystemException(err, mesg);
				break;
				
			default:
				mesg = LoadWhisperString(L"Error ##1", Int32ToStr(err));
				throw XSystemException(err, mesg);	
		}

#elif WIN
	if (err == ERROR_NOT_ENOUGH_MEMORY || err == ERROR_OUTOFMEMORY) {
		throw XSysMemoryException();

	} else if (WSystemInfo::HasUnicode()) {
		wchar_t mesg[512];
		uint32 count = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,					// source and processing options
									  nil,											// message source
									  err,											// message indentifier
									  MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),// language identifier
									  mesg,											// message buffer
									  sizeof(mesg),									// maximum size of message buffer
									  nil);											// message inserts

		throw XSystemException(err, std::wstring(mesg, count));	


	} else {
		char mesg[512];
		uint32 count = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,					// source and processing options
									  nil,											// message source
									  err,											// message indentifier
									  MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),// language identifier
									  mesg,											// message buffer
									  sizeof(mesg),									// maximum size of message buffer
									  nil);											// message inserts

		throw XSystemException(err, FromPlatformStr(mesg, count));	
	}
#endif
}		 


//---------------------------------------------------------------
//
// ThrowIfResFail
//
//---------------------------------------------------------------
#if MAC
void ThrowIfResFail(const void* ptr)										
{
	if (ptr == nil) 
		throw XSystemException(-192, LoadWhisperString(L"Resource was nil"));	
		
	ThrowIfOSErr(ResError());
}		
#endif	// MAC


//---------------------------------------------------------------
//
// ThrowIfMemFail
//
//---------------------------------------------------------------
#if MAC
void ThrowIfMemFail(const void* ptr)
{
	if (ptr == nil) 
		throw XSysMemoryException();
		
	ThrowIfOSErr(MemError());
}		
#endif	// MAC


}	// namespace Whisper

