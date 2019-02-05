/*
 *  File:       XTranscode.cpp
 *  Summary:   	Functions to convert Unicode strings to and from other formats
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	  	
 *
 *		$Log: XTranscode.cpp,v $
 *		Revision 1.9  2001/04/27 09:21:07  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.8  2001/04/27 04:29:15  jesjones
 *		Made the narrow typedefs std::strings (basic_string for PascalString).
 *		
 *		Revision 1.7  2001/04/17 01:45:00  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.6  2001/04/13 08:04:23  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.5  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.4  2000/12/10 04:00:13  jesjones
 *		Added ToAsciiString.
 *		
 *		Revision 1.3  2000/11/21 09:25:07  jesjones
 *		Fixed an assert so that it works with Pro 6 on Windows.
 *		
 *		Revision 1.2  2000/11/09 12:57:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XTranscode.h>

#include <XAutoPtr.h>
#include <XCriticalSection.h>
#include <XExceptions.h>
#include <XNumbers.h>
#include <XStringUtils.h>
#include <XTextTranscoders.h>		
#include <XTinyVector.h>

namespace Whisper {

//-----------------------------------
//	Variables
//
static XAutoPtr<XTextTranscoder> sPlatformTranscoder;	// $$$ find out if the transcoders are thread safe in Carbon
static XCriticalSection	sTranscoderMutex;

static XAutoPtr<XUTF8Transcoder> sUTF8Transcoder;		

#if MAC
	static XAutoPtr<XMacRomanTranscoder> sPascalTranscoder;
#endif


// ===================================================================================
//	Internal Functions
// ===================================================================================	

//---------------------------------------------------------------
//
// GetPlatformTranscoder
//
//---------------------------------------------------------------
static XTextTranscoder* GetPlatformTranscoder(XTextTranscoder* override)
{
	XTextTranscoder* transcoder = override;
	
	if (transcoder == nil) {
		if (sPlatformTranscoder.Get() == nil) {
			XEnterCriticalSection enter(sTranscoderMutex);	

			if (sPlatformTranscoder.Get() == nil) {
#if MAC
				sPlatformTranscoder.Reset(new XMacRomanTranscoder);
#elif WIN
				sPlatformTranscoder.Reset(new XWindowsLatin1Transcoder);
#endif
			}
	}
		
		transcoder = sPlatformTranscoder.Get();
	}
	
	return transcoder;
}


//---------------------------------------------------------------
//
// GetUTF8Transcoder
//
//---------------------------------------------------------------
static XTextTranscoder* GetUTF8Transcoder()
{	
	if (sUTF8Transcoder.Get() == nil) {
		XEnterCriticalSection enter(sTranscoderMutex);	

		if (sUTF8Transcoder.Get() == nil)
			sUTF8Transcoder.Reset(new XUTF8Transcoder);
	}
		
	return sUTF8Transcoder.Get();
}


//---------------------------------------------------------------
//
// GetPascalTranscoder
//
//---------------------------------------------------------------
#if MAC
static XTextTranscoder* GetPascalTranscoder()
{	
	if (sPascalTranscoder.Get() == nil) {
		XEnterCriticalSection enter(sTranscoderMutex);	

		if (sPascalTranscoder.Get() == nil) 
			sPascalTranscoder.Reset(new XMacRomanTranscoder);
	}
		
	return sPascalTranscoder.Get();
}
#endif

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Global Functions
// ===================================================================================	

//---------------------------------------------------------------
//
// SetPlatformTranscoder
//
//---------------------------------------------------------------
void SetPlatformTranscoder(XTextTranscoder* takeTranscoder)
{
	PRECONDITION(takeTranscoder != nil);
	
	XEnterCriticalSection enter(sTranscoderMutex);	

	sPlatformTranscoder.Reset(takeTranscoder);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// FromPlatformStr
//
//---------------------------------------------------------------
std::wstring FromPlatformStr(const char* str, uint32 length, XTextTranscoder* override)
{
	PRECONDITION(str != nil);
	
	uint32 srcLen = length;
	if (srcLen == ULONG_MAX)
		srcLen = std::strlen(str);

	XTextTranscoder* transcoder = GetPlatformTranscoder(override);
	uint32 dstLen = transcoder->GetDstBytes(str, srcLen)/2;
	
	XTinyVector<wchar_t> buffer(dstLen);
	uint32 len = transcoder->ConvertToUTF16(str, srcLen, buffer.buffer(), dstLen*sizeof(wchar_t))/2;
	
	return std::wstring(buffer.buffer(), len);
}


//---------------------------------------------------------------
//
// FromUTF8Str
//
//---------------------------------------------------------------
std::wstring FromUTF8Str(const char* str, uint32 length)
{
	PRECONDITION(str != nil);
		
	uint32 srcLen = length;
	if (srcLen == ULONG_MAX)
		srcLen = std::strlen(str);

	XTextTranscoder* transcoder = GetUTF8Transcoder();
	uint32 dstLen = transcoder->GetDstBytes(str, srcLen)/2;
	
	XTinyVector<wchar_t> buffer(dstLen);
	uint32 len = transcoder->ConvertToUTF16(str, srcLen, buffer.buffer(), dstLen*sizeof(wchar_t))/2;
	
	return std::wstring(buffer.buffer(), len);
}


//---------------------------------------------------------------
//
// FromAsciiStr
//
//---------------------------------------------------------------
std::wstring FromAsciiStr(const char* str, uint32 length)
{
	PRECONDITION(str != nil);
	
	uint32 len = std::min((uint32) std::strlen(str), length);
	
	std::wstring result;
	result.resize(len);
	
	for (uint32 i = 0; i < len; ++i) {
		ASSERT(str[i] >= 0);			// for signed chars
		ASSERT(str[i] < 128L);			// for unsigned chars (shouldn't need the 'L', but CW 6 is acting up on Windows)
		
		result[i] = str[i];
	}
	
	return result;
}


//---------------------------------------------------------------
//
// FromPascalStr
//
//---------------------------------------------------------------
#if MAC
std::wstring FromPascalStr(const unsigned char* str, uint32 length)
{
	PRECONDITION(str != nil);
		
	XTextTranscoder* transcoder = GetPascalTranscoder();

	uint32 srcLen = std::min((uint32) str[0], length);
	uint32 dstLen = transcoder->GetDstBytes((const char*) str+1, srcLen)/2;
	
	XTinyVector<wchar_t> buffer(dstLen);
	uint32 len = transcoder->ConvertToUTF16((const char*) str+1, srcLen, buffer.buffer(), dstLen*sizeof(wchar_t))/2;
	
	return std::wstring(buffer.buffer(), len);
}
#endif


//---------------------------------------------------------------
//
// FromCarbonStr
//
//---------------------------------------------------------------
#if MAC
std::wstring FromCarbonStr(CFStringRef str, uint32 inLength)
{
	PRECONDITION(str != nil);
	COMPILE_CHECK(sizeof(wchar_t) == sizeof(UniChar));

	uint32 length = inLength;
	if (length == ULONG_MAX)
		length = numeric_cast<uint32>(CFStringGetLength(str));
	
	std::wstring result;
	const UniChar* chars = CFStringGetCharactersPtr(str);
	if (chars != nil) {
		result.assign((wchar_t*) chars, length);
	
	} else {		
		XTinyVector<wchar_t> buffer(length);

		CFRange range;
		range.location = 0;
		range.length   = numeric_cast<int32>(length);
		CFStringGetCharacters(str, range, (UniChar*) buffer.buffer());

		result.assign(buffer.buffer(), length);
	}
	
	return result;
}
#endif

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// ToUTF8Str
//
//---------------------------------------------------------------
UTF8String ToUTF8Str(const std::wstring& inStr, uint32 len)
{	
	XTextTranscoder* transcoder = GetUTF8Transcoder();
		
	uint32 srcBytes = std::min(len, (uint32) (inStr.length())*sizeof(wchar_t));
	uint32 dstBytes = transcoder->GetDstBytes(inStr.c_str(), srcBytes);
	
	UTF8String outStr(dstBytes, '\0');
	
	uint32 length = transcoder->ConvertFromUTF16(inStr.c_str(), srcBytes, (char*) outStr.c_str(), dstBytes);
	ASSERT(length <= dstBytes);
	outStr.resize(length);
	
	return outStr;
}


//---------------------------------------------------------------
//
// ToAsciiStr
//
//---------------------------------------------------------------
AsciiString ToAsciiStr(const std::wstring& inStr, uint32 len)
{		
	len = std::min(len, (uint32) inStr.length());
	
	AsciiString outStr(len, '\0');
	
	for (uint32 i = 0; i < len; ++i) {
		ASSERT(inStr[i] < 128);
		
		outStr[i] = static_cast<char>(inStr[i]);
	}
	
	return outStr;
}


//---------------------------------------------------------------
//
// ToPlatformStr
//
//---------------------------------------------------------------
PlatformString ToPlatformStr(const std::wstring& inStr, uint32 len, XTextTranscoder* override)
{	
	XTextTranscoder* transcoder = GetPlatformTranscoder(override);

	uint32 srcBytes = std::min(len, (uint32) (inStr.length())*sizeof(wchar_t));
	uint32 dstBytes = transcoder->GetDstBytes(inStr.c_str(), srcBytes);
	
	PlatformString outStr(dstBytes, '\0');
	
	uint32 length = transcoder->ConvertFromUTF16(inStr.c_str(), srcBytes, (char*) outStr.c_str(), dstBytes);
	ASSERT(length <= dstBytes);
	outStr.resize(length);
	
	return outStr;
}


//---------------------------------------------------------------
//
// ToPascalStr
//
//---------------------------------------------------------------
#if MAC
PascalString ToPascalStr(const std::wstring& inStr, uint32 len)
{
	XTextTranscoder* transcoder = GetPascalTranscoder();

	uint32 srcBytes = std::min(len, inStr.length())*sizeof(wchar_t);
	uint32 dstBytes = transcoder->GetDstBytes(inStr.c_str(), srcBytes);
	
	PascalString outStr(dstBytes+1, '\0');
	
	uint32 length = transcoder->ConvertFromUTF16(inStr.c_str(), srcBytes, (char*) (outStr.c_str()+1), dstBytes);
	if (length > 255)
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The string is too long to convert to a Pascal string.")));
		
	outStr.resize(length+1);
	outStr[0] = (unsigned char) length;
	
	return outStr;
}
#endif


//---------------------------------------------------------------
//
// ToCarbonStr
//
//---------------------------------------------------------------
#if MAC
MCoreString ToCarbonStr(const std::wstring& inStr, uint32 length)
{	
	CFStringRef outStr = CFStringCreateWithCharacters(kCFAllocatorSystemDefault, (const UniChar*) inStr.c_str(), (int32) std::min(length, inStr.length()));
	ThrowIfNil(outStr);
	
	return MCoreString(outStr);
}
#endif



}	// namespace Whisper
