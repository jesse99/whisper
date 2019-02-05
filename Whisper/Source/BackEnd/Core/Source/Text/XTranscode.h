/*
 *  File:       XTranscode.h
 *  Summary:   	Functions to convert Unicode strings to and from other formats
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	  	
 *
 *		$Log: XTranscode.h,v $
 *		Revision 1.6  2001/04/27 04:29:19  jesjones
 *		Made the narrow typedefs std::strings (basic_string for PascalString).
 *		
 *		Revision 1.5  2001/04/13 08:04:27  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.4  2000/12/10 04:00:19  jesjones
 *		Added ToAsciiString.
 *		
 *		Revision 1.3  2000/11/20 05:35:32  jesjones
 *		Includes <string>.
 *		
 *		Revision 1.2  2000/11/09 12:57:23  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <string>

#if MAC
	#include <MCoreString.h>
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XTextTranscoder;


//-----------------------------------
//	Types
//
typedef std::string AsciiString;		// 7-bit ASCII (very portable, but missing lots of handy characters)
typedef std::string PlatformString;		// 8-bit ASCII (not very portable, character meanings will change on a machine with a different language)
typedef std::string UTF8String;			// multi-byte Unicode (very portable, although not many text editors can deal with this format)

#if MAC
	typedef std::basic_string<unsigned char> PascalString;	// note that the first character is the length byte
#endif


// ===================================================================================
//	Default Platform Transcoder
// ===================================================================================	
CORE_EXPORT  void 			SetPlatformTranscoder(XTextTranscoder* takeTranscoder);
							// Used when converting to and from platform strings. Defaults 
							// to XMacRomanTranscoder on Macs and XWindowsLatin1Transcoder 
							// on Windows. Note that you can use XNaiveTranscoder if the 
							// default transcoders are too slow (and you're willing to 
							// risk generating bogus Unicode).


// ===================================================================================
//	To Unicode
// ===================================================================================	
CORE_EXPORT std::wstring 	FromPlatformStr(const char* str, uint32 length = ULONG_MAX, XTextTranscoder* transcoder = nil);

CORE_EXPORT std::wstring 	FromAsciiStr(const char* str, uint32 length = ULONG_MAX);
							// You should only use this when you're sure the str is
							// 7-bit ASCII. If the str may be extended ASCII use
							// FromPlatformStr.

CORE_EXPORT std::wstring 	FromUTF8Str(const char* str, uint32 length = ULONG_MAX);

#if MAC
CORE_EXPORT std::wstring 	FromPascalStr(const unsigned char* str, uint32 length = ULONG_MAX);

inline std::wstring 		FromCarbonStr(const MCoreString& str, uint32 length = ULONG_MAX)	{return std::wstring(str.c_str(), length);}
CORE_EXPORT std::wstring 	FromCarbonStr(CFStringRef str, uint32 length = ULONG_MAX);
#endif


// ===================================================================================
//	From Unicode
// ===================================================================================	
CORE_EXPORT UTF8String 		ToUTF8Str(const std::wstring& str, uint32 length = ULONG_MAX);
							// These narrow string types are all reference counted so it's
							// safe to copy them around. However they're a bit dangerous
							// because they have an implicit conversion operator to the
							// underlying pointer type which means that code like this:
							//		const char* utf8 = ToUTF8Str(str);
							// will compile, but the pointer will be bogus because the
							// temporary variable is destroyed at the end of the expression.

CORE_EXPORT AsciiString 	ToAsciiStr(const std::wstring& str, uint32 length = ULONG_MAX);
							// You should only use this when you're sure the str is
							// 7-bit ASCII. If not use ToUTF8Str or ToPlatformStr.

CORE_EXPORT PlatformString 	ToPlatformStr(const std::wstring& str, uint32 length = ULONG_MAX, XTextTranscoder* transcoder = nil);

#if MAC
CORE_EXPORT PascalString 	ToPascalStr(const std::wstring& str, uint32 length = ULONG_MAX);

CORE_EXPORT MCoreString 	ToCarbonStr(const std::wstring& str, uint32 length = ULONG_MAX);
#endif
			

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
