/*
 *  File:		XTextTranscoders.cpp
 *  Summary:	Classes used to convert to and from Unicode.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTextTranscoders.cpp,v $
 *		Revision 1.5  2001/04/27 04:37:09  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/04/13 08:04:16  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2000/12/10 04:12:25  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:55:29  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 3/22/00	JDJ		Added XEUCJPTranscoder and XShiftJISTranscoder (Mac only).
 *		 <4>	 1/26/00	JDJ		Added a unit test.
 *		 <3>	 5/03/99	JDJ		XUTF8Transcoder will do a proper conversion if OS support is missing.
 *		 <2>	 4/19/99	JDJ		No longer calls CreateTextEncoding without checking for TEC.
 *		 <1>	 1/25/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XTextTranscoders.h>		

#include <cvtutf.h>
#include <XExceptions.h>
#include <XNumbers.h>
#include <XStringUtils.h>

#if MAC	
	#include <MSystemInfo.h>
#endif

namespace Whisper {


//-----------------------------------
//	Tables
//
#if WIN
const wchar_t kMacToUnicode[256] = {
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 
	0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F, 
	0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 
	0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F, 
	0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, 
	0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 
	0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F, 
	0x00C4, 0x00C5, 0x00C7, 0x00C9, 0x00D1, 0x00D6, 0x00DC, 0x00E1, 0x00E0, 0x00E2, 0x00E4, 0x00E3, 0x00E5, 0x00E7, 0x00E9, 0x00E8, 
	0x00EA, 0x00EB, 0x00ED, 0x00EC, 0x00EE, 0x00EF, 0x00F1, 0x00F3, 0x00F2, 0x00F4, 0x00F6, 0x00F5, 0x00FA, 0x00F9, 0x00FB, 0x00FC, 
	0x2020, 0x00B0, 0x00A2, 0x00A3, 0x00A7, 0x2022, 0x00B6, 0x00DF, 0x00AE, 0x00A9, 0x2122, 0x00B4, 0x00A8, 0x2260, 0x00C6, 0x00D8, 
	0x221E, 0x00B1, 0x2264, 0x2265, 0x00A5, 0x00B5, 0x2202, 0x2211, 0x220F, 0x03C0, 0x222B, 0x00AA, 0x00BA, 0x03A9, 0x00E6, 0x00F8, 
	0x00BF, 0x00A1, 0x00AC, 0x221A, 0x0192, 0x2248, 0x2206, 0x00AB, 0x00BB, 0x2026, 0x00A0, 0x00C0, 0x00C3, 0x00D5, 0x0152, 0x0153, 
	0x2013, 0x2014, 0x201C, 0x201D, 0x2018, 0x2019, 0x00F7, 0x25CA, 0x00FF, 0x0178, 0x2044, 0x20AC, 0x2039, 0x203A, 0xFB01, 0xFB02, 
	0x2021, 0x00B7, 0x201A, 0x201E, 0x2030, 0x00C2, 0x00CA, 0x00C1, 0x00CB, 0x00C8, 0x00CD, 0x00CE, 0x00CF, 0x00CC, 0x00D3, 0x00D4, 
	0xF8FF, 0x00D2, 0x00DA, 0x00DB, 0x00D9, 0x0131, 0x02C6, 0x02DC, 0x00AF, 0x02D8, 0x02D9, 0x02DA, 0x00B8, 0x02DD, 0x02DB, 0x02C7
};

struct SCharCode {
	uint8 	code;
	wchar_t	unicode;
	
		bool operator==(const SCharCode& rhs) const	{return unicode == rhs.unicode;}
		bool operator<(const SCharCode& rhs) const	{return unicode < rhs.unicode;}
};

const SCharCode kUnicodeToMac[256] = {
	{0x00, 0x0000}, {0x01, 0x0001}, {0x02, 0x0002}, {0x03, 0x0003}, {0x04, 0x0004}, {0x05, 0x0005}, {0x06, 0x0006}, {0x07, 0x0007}, 
	{0x08, 0x0008}, {0x09, 0x0009}, {0x0A, 0x000A}, {0x0B, 0x000B}, {0x0C, 0x000C}, {0x0D, 0x000D}, {0x0E, 0x000E}, {0x0F, 0x000F}, 
	{0x10, 0x0010}, {0x11, 0x0011}, {0x12, 0x0012}, {0x13, 0x0013}, {0x14, 0x0014}, {0x15, 0x0015}, {0x16, 0x0016}, {0x17, 0x0017}, 
	{0x18, 0x0018}, {0x19, 0x0019}, {0x1A, 0x001A}, {0x1B, 0x001B}, {0x1C, 0x001C}, {0x1D, 0x001D}, {0x1E, 0x001E}, {0x1F, 0x001F}, 
	{0x20, 0x0020}, {0x21, 0x0021}, {0x22, 0x0022}, {0x23, 0x0023}, {0x24, 0x0024}, {0x25, 0x0025}, {0x26, 0x0026}, {0x27, 0x0027}, 
	{0x28, 0x0028}, {0x29, 0x0029}, {0x2A, 0x002A}, {0x2B, 0x002B}, {0x2C, 0x002C}, {0x2D, 0x002D}, {0x2E, 0x002E}, {0x2F, 0x002F}, 
	{0x30, 0x0030}, {0x31, 0x0031}, {0x32, 0x0032}, {0x33, 0x0033}, {0x34, 0x0034}, {0x35, 0x0035}, {0x36, 0x0036}, {0x37, 0x0037}, 
	{0x38, 0x0038}, {0x39, 0x0039}, {0x3A, 0x003A}, {0x3B, 0x003B}, {0x3C, 0x003C}, {0x3D, 0x003D}, {0x3E, 0x003E}, {0x3F, 0x003F}, 
	{0x40, 0x0040}, {0x41, 0x0041}, {0x42, 0x0042}, {0x43, 0x0043}, {0x44, 0x0044}, {0x45, 0x0045}, {0x46, 0x0046}, {0x47, 0x0047}, 
	{0x48, 0x0048}, {0x49, 0x0049}, {0x4A, 0x004A}, {0x4B, 0x004B}, {0x4C, 0x004C}, {0x4D, 0x004D}, {0x4E, 0x004E}, {0x4F, 0x004F}, 
	{0x50, 0x0050}, {0x51, 0x0051}, {0x52, 0x0052}, {0x53, 0x0053}, {0x54, 0x0054}, {0x55, 0x0055}, {0x56, 0x0056}, {0x57, 0x0057}, 
	{0x58, 0x0058}, {0x59, 0x0059}, {0x5A, 0x005A}, {0x5B, 0x005B}, {0x5C, 0x005C}, {0x5D, 0x005D}, {0x5E, 0x005E}, {0x5F, 0x005F}, 
	{0x60, 0x0060}, {0x61, 0x0061}, {0x62, 0x0062}, {0x63, 0x0063}, {0x64, 0x0064}, {0x65, 0x0065}, {0x66, 0x0066}, {0x67, 0x0067}, 
	{0x68, 0x0068}, {0x69, 0x0069}, {0x6A, 0x006A}, {0x6B, 0x006B}, {0x6C, 0x006C}, {0x6D, 0x006D}, {0x6E, 0x006E}, {0x6F, 0x006F}, 
	{0x70, 0x0070}, {0x71, 0x0071}, {0x72, 0x0072}, {0x73, 0x0073}, {0x74, 0x0074}, {0x75, 0x0075}, {0x76, 0x0076}, {0x77, 0x0077}, 
	{0x78, 0x0078}, {0x79, 0x0079}, {0x7A, 0x007A}, {0x7B, 0x007B}, {0x7C, 0x007C}, {0x7D, 0x007D}, {0x7E, 0x007E}, {0x7F, 0x007F}, 
	{0xCA, 0x00A0}, {0xC1, 0x00A1}, {0xA2, 0x00A2}, {0xA3, 0x00A3}, {0xB4, 0x00A5}, {0xA4, 0x00A7}, {0xAC, 0x00A8}, {0xA9, 0x00A9}, 
	{0xBB, 0x00AA}, {0xC7, 0x00AB}, {0xC2, 0x00AC}, {0xA8, 0x00AE}, {0xF8, 0x00AF}, {0xA1, 0x00B0}, {0xB1, 0x00B1}, {0xAB, 0x00B4}, 
	{0xB5, 0x00B5}, {0xA6, 0x00B6}, {0xE1, 0x00B7}, {0xFC, 0x00B8}, {0xBC, 0x00BA}, {0xC8, 0x00BB}, {0xC0, 0x00BF}, {0xCB, 0x00C0}, 
	{0xE7, 0x00C1}, {0xE5, 0x00C2}, {0xCC, 0x00C3}, {0x80, 0x00C4}, {0x81, 0x00C5}, {0xAE, 0x00C6}, {0x82, 0x00C7}, {0xE9, 0x00C8}, 
	{0x83, 0x00C9}, {0xE6, 0x00CA}, {0xE8, 0x00CB}, {0xED, 0x00CC}, {0xEA, 0x00CD}, {0xEB, 0x00CE}, {0xEC, 0x00CF}, {0x84, 0x00D1}, 
	{0xF1, 0x00D2}, {0xEE, 0x00D3}, {0xEF, 0x00D4}, {0xCD, 0x00D5}, {0x85, 0x00D6}, {0xAF, 0x00D8}, {0xF4, 0x00D9}, {0xF2, 0x00DA}, 
	{0xF3, 0x00DB}, {0x86, 0x00DC}, {0xA7, 0x00DF}, {0x88, 0x00E0}, {0x87, 0x00E1}, {0x89, 0x00E2}, {0x8B, 0x00E3}, {0x8A, 0x00E4}, 
	{0x8C, 0x00E5}, {0xBE, 0x00E6}, {0x8D, 0x00E7}, {0x8F, 0x00E8}, {0x8E, 0x00E9}, {0x90, 0x00EA}, {0x91, 0x00EB}, {0x93, 0x00EC}, 
	{0x92, 0x00ED}, {0x94, 0x00EE}, {0x95, 0x00EF}, {0x96, 0x00F1}, {0x98, 0x00F2}, {0x97, 0x00F3}, {0x99, 0x00F4}, {0x9B, 0x00F5}, 
	{0x9A, 0x00F6}, {0xD6, 0x00F7}, {0xBF, 0x00F8}, {0x9D, 0x00F9}, {0x9C, 0x00FA}, {0x9E, 0x00FB}, {0x9F, 0x00FC}, {0xD8, 0x00FF}, 
	{0xF5, 0x0131}, {0xCE, 0x0152}, {0xCF, 0x0153}, {0xD9, 0x0178}, {0xC4, 0x0192}, {0xF6, 0x02C6}, {0xFF, 0x02C7}, {0xF9, 0x02D8}, 
	{0xFA, 0x02D9}, {0xFB, 0x02DA}, {0xFE, 0x02DB}, {0xF7, 0x02DC}, {0xFD, 0x02DD}, {0xBD, 0x03A9}, {0xB9, 0x03C0}, {0xD0, 0x2013}, 
	{0xD1, 0x2014}, {0xD4, 0x2018}, {0xD5, 0x2019}, {0xE2, 0x201A}, {0xD2, 0x201C}, {0xD3, 0x201D}, {0xE3, 0x201E}, {0xA0, 0x2020}, 
	{0xE0, 0x2021}, {0xA5, 0x2022}, {0xC9, 0x2026}, {0xE4, 0x2030}, {0xDC, 0x2039}, {0xDD, 0x203A}, {0xDA, 0x2044}, {0xDB, 0x20AC}, 
	{0xAA, 0x2122}, {0xB6, 0x2202}, {0xC6, 0x2206}, {0xB8, 0x220F}, {0xB7, 0x2211}, {0xC3, 0x221A}, {0xB0, 0x221E}, {0xBA, 0x222B}, 
	{0xC5, 0x2248}, {0xAD, 0x2260}, {0xB2, 0x2264}, {0xB3, 0x2265}, {0xD7, 0x25CA}, {0xF0, 0xF8FF}, {0xDE, 0xFB01}, {0xDF, 0xFB02}
};
#endif	// WIN


//-----------------------------------
//	Constants
//
#if MAC
	const OSError kNoTranslation     = kTECUnmappableElementErr;
	const OSError kPartialConversion = kTECPartialCharErr;
	const OSError kBufferExhausted   = kTECOutputBufferFullStatus;
#elif WIN
	const OSError kNoTranslation     = ERROR_NO_UNICODE_TRANSLATION;
	const OSError kPartialConversion = ERROR_INVALID_PARAMETER;
	const OSError kBufferExhausted   = ERROR_INSUFFICIENT_BUFFER;
#endif


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// MyCreateTextEncoding
//
//---------------------------------------------------------------
#if MAC
static TextEncoding MyCreateTextEncoding(TextEncodingBase base, TextEncodingVariant variant, TextEncodingFormat format)
{
	TextEncoding encoding = 0;		// encoding is only used when the TEC is present
	
	if (MSystemInfo::HasTextEncodingConverter())
		encoding = ::CreateTextEncoding(base, variant, format);
		
	return encoding;
}
#endif

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XNaiveTranscoder
// ===================================================================================

//---------------------------------------------------------------
//
// XNaiveTranscoder::~XNaiveTranscoder
//
//---------------------------------------------------------------
XNaiveTranscoder::~XNaiveTranscoder()
{
}


//---------------------------------------------------------------
//
// XNaiveTranscoder::XNaiveTranscoder
//
//---------------------------------------------------------------
XNaiveTranscoder::XNaiveTranscoder(bool useDecomposedCharacters) : 
#if MAC
	XTextTranscoder(MyCreateTextEncoding(kTextEncodingUS_ASCII, (uint32) (useDecomposedCharacters ? kUnicodeCanonicalDecompVariant : kTextEncodingDefaultVariant), kTextEncodingDefaultFormat), useDecomposedCharacters)
#elif WIN
	XTextTranscoder(CP_ACP, useDecomposedCharacters)
#endif
{
	mUseOS = false;
	
	mEncoding = L"naive";
}


//---------------------------------------------------------------
//
// XNaiveTranscoder::Clone
//
//---------------------------------------------------------------
XNaiveTranscoder* XNaiveTranscoder::Clone() const
{
	return new XNaiveTranscoder;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XASCIITranscoder
// ===================================================================================

//---------------------------------------------------------------
//
// XASCIITranscoder::~XASCIITranscoder
//
//---------------------------------------------------------------
XASCIITranscoder::~XASCIITranscoder()
{
}


//---------------------------------------------------------------
//
// XASCIITranscoder::XASCIITranscoder	
//
//---------------------------------------------------------------
XASCIITranscoder::XASCIITranscoder(bool useDecomposedCharacters) : 
#if MAC
	XTextTranscoder(MyCreateTextEncoding(kTextEncodingUS_ASCII, (uint32) (useDecomposedCharacters ? kUnicodeCanonicalDecompVariant : kTextEncodingDefaultVariant), kTextEncodingDefaultFormat), useDecomposedCharacters)
#elif WIN
	XTextTranscoder(CP_ACP, useDecomposedCharacters)
#endif
{
	mEncoding = L"iso646-us";
}

//---------------------------------------------------------------
//
// XASCIITranscoder::Clone
//
//---------------------------------------------------------------
XASCIITranscoder* XASCIITranscoder::Clone() const
{
	return new XASCIITranscoder;
}


//---------------------------------------------------------------
//
// XASCIITranscoder::ConvertFromUTF16
//
//---------------------------------------------------------------
#if WIN
uint32 XASCIITranscoder::ConvertFromUTF16(const wchar_t* src, uint32 srcBytes, char* dst, uint32 dstBytes)
{
	PRECONDITION(src != nil);
	PRECONDITION(dst != nil);
			
	uint32 charBytes = charBytes = Inherited::ConvertFromUTF16(src, srcBytes, dst, dstBytes);
		
	// WideCharToMultiByte doesn't support conversion to standard
	// US-ASCII so we convert using the Windows ANSI code page
	// (which is equivalent to US ASCII for the first 128 characters)
	// and then check to see that no eight bit characters were created).
	for (uint32 index = 0; index < charBytes; ++index) {
		if (dst[index] > 127 || dst[index] < 0) {			// handle unsigned and signed characters...
			if (mFromUTF16Flags & MB_ERR_INVALID_CHARS) 
				ThrowErr(ERROR_NO_UNICODE_TRANSLATION);	
			else
				dst[index] = '?';
		}
	}

	return charBytes;
}
#endif

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XMacRomanTranscoder
// ===================================================================================

//---------------------------------------------------------------
//
// XMacRomanTranscoder::~XMacRomanTranscoder
//
//---------------------------------------------------------------
XMacRomanTranscoder::~XMacRomanTranscoder()
{
}


//---------------------------------------------------------------
//
// XMacRomanTranscoder::XMacRomanTranscoder
//
//---------------------------------------------------------------
XMacRomanTranscoder::XMacRomanTranscoder(bool useDecomposedCharacters) : 
#if MAC
	XTextTranscoder(MyCreateTextEncoding(kTextEncodingMacRoman, (uint32) (useDecomposedCharacters ? kUnicodeCanonicalDecompVariant : kTextEncodingDefaultVariant), kTextEncodingDefaultFormat), useDecomposedCharacters)
#elif WIN
	XTextTranscoder(CP_MACCP, useDecomposedCharacters)		
#endif
{
	mEncoding = L"mac-roman";

#if MAC
	mToUTF16Flags   |= kUnicodeLooseMappingsMask;	
	mFromUTF16Flags |= kUnicodeLooseMappingsMask;
	
#elif WIN
	CPINFO info;
	int32 succeeded = ::GetCPInfo(CP_MACCP, &info);
	mHasMacCodePage = succeeded != 0;					// Win 98 doesn't usually have the Mac code page installed...
#endif
}


//---------------------------------------------------------------
//
// XMacRomanTranscoder::Clone
//
//---------------------------------------------------------------
XMacRomanTranscoder* XMacRomanTranscoder::Clone() const
{
	return new XMacRomanTranscoder;
}


//---------------------------------------------------------------
//
// XMacRomanTranscoder::ConvertToUTF16
//
//---------------------------------------------------------------
#if WIN
uint32 XMacRomanTranscoder::ConvertToUTF16(const char* src, uint32 srcBytes, wchar_t* dst, uint32 dstBytes)
{
	PRECONDITION(src != nil);
	PRECONDITION(dst != nil);
	
	uint32 uniBytes = 0;
	
	if (srcBytes > 0) {
		if (mHasMacCodePage)
			uniBytes = Inherited::ConvertToUTF16(src, srcBytes, dst, dstBytes);
			
		else {
			uint32 dstLen = dstBytes/2;
				
			uint32 index = 0;
			while (src[index] != '\0' && index < dstLen) {
				uint32 ch = src[index] & 0xFFUL;
				dst[index] = kMacToUnicode[ch];
				++index;
			}
			
			uniBytes = 2*index;
		}
	}
		
#if DEBUG
	if (msFlagToUTF16Conversions > 0) 
		DEBUGSTR("Converted char*: '", dst);
#endif

	return uniBytes;
}
#endif


//---------------------------------------------------------------
//
// XMacRomanTranscoder::GetDstBytes (char*, uint32)
//
//---------------------------------------------------------------
#if WIN
uint32 XMacRomanTranscoder::GetDstBytes(const char* src, uint32 srcBytes)
{	
	uint32 dstBytes = 0;
	
	if (!mUseOS || !mHasMacCodePage)
		dstBytes = 2*srcBytes;
	else 
		dstBytes = Inherited::GetDstBytes(src, srcBytes);

	return dstBytes;
}
#endif


//---------------------------------------------------------------
//
// XMacRomanTranscoder::ConvertFromUTF16
//
//---------------------------------------------------------------
#if WIN
uint32 XMacRomanTranscoder::ConvertFromUTF16(const wchar_t* src, uint32 srcBytes, char* dst, uint32 dstBytes)
{
	PRECONDITION(src != nil);
	PRECONDITION(dst != nil);
			
	uint32 charBytes = 0;	
	
	if (srcBytes > 0) {
		if (mHasMacCodePage)
			charBytes = Inherited::ConvertFromUTF16(src, srcBytes, dst, dstBytes);
			
		else {	
			// $$ This code won't work quite right if the Unicode uses 
			// $$ decomposed characters. It would be possible to use
			// $$ FoldString to get rid of the combining characters,
			// $$ except that FoldString isn't available on Win95...
			SCharCode code;
			uint32 index = 0;
			while (src[index] != '\0' && index < dstBytes) {
				code.unicode = src[index];
	
				const SCharCode* iter = std::lower_bound(kUnicodeToMac, kUnicodeToMac + 256, code);
				if (iter < kUnicodeToMac + 256) {
					dst[index] = (char) (iter->code);
				
				} else {					
					if (mFromUTF16Flags & MB_ERR_INVALID_CHARS)
						ThrowErr(ERROR_NO_UNICODE_TRANSLATION);
					else
						dst[index] = '?';
				}	
				
				++index;
			}
			
			charBytes = index;
		}
	}
		
	return charBytes;
}
#endif


//---------------------------------------------------------------
//
// XMacRomanTranscoder::GetDstBytes (wchar_t*, uint32)
//
//---------------------------------------------------------------
#if WIN
uint32 XMacRomanTranscoder::GetDstBytes(const wchar_t* src, uint32 srcBytes)
{
	uint32 dstBytes = 0;
	
	if (!mUseOS || !mHasMacCodePage)
		dstBytes = srcBytes/2;
	else 
		dstBytes = Inherited::GetDstBytes(src, srcBytes);

	return dstBytes;
}
#endif

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XWindowsLatin1Transcoder
// ===================================================================================

//---------------------------------------------------------------
//
// XWindowsLatin1Transcoder::~XWindowsLatin1Transcoder
//
//---------------------------------------------------------------
XWindowsLatin1Transcoder::~XWindowsLatin1Transcoder()
{
}


//---------------------------------------------------------------
//
// XWindowsLatin1Transcoder::XWindowsLatin1Transcoder
//
//---------------------------------------------------------------
XWindowsLatin1Transcoder::XWindowsLatin1Transcoder(bool useDecomposedCharacters) : 
#if MAC
	XTextTranscoder(MyCreateTextEncoding(kTextEncodingWindowsLatin1, (uint32) (useDecomposedCharacters ? kUnicodeCanonicalDecompVariant : kTextEncodingDefaultVariant), kTextEncodingDefaultFormat), useDecomposedCharacters)
#elif WIN
	XTextTranscoder(CP_ACP, useDecomposedCharacters)
#endif
{
	mEncoding = L"iso-8859-1";
}


//---------------------------------------------------------------
//
// XWindowsLatin1Transcoder::Clone
//
//---------------------------------------------------------------
XWindowsLatin1Transcoder* XWindowsLatin1Transcoder::Clone() const
{
	return new XWindowsLatin1Transcoder;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XUTF8Transcoder
// ===================================================================================

//---------------------------------------------------------------
//
// XUTF8Transcoder::~XUTF8Transcoder
//
//---------------------------------------------------------------
XUTF8Transcoder::~XUTF8Transcoder()
{
#if MAC
	if (mTo16TEC != nil)
		VERIFY(::TECDisposeConverter(mTo16TEC) == noErr);
	
	if (mFrom16TEC != nil)
		VERIFY(::TECDisposeConverter(mFrom16TEC) == noErr);
#endif
}


//---------------------------------------------------------------
//
// XUTF8Transcoder::XUTF8Transcoder
//
//---------------------------------------------------------------
XUTF8Transcoder::XUTF8Transcoder() :	
#if MAC
	XTextTranscoder(MyCreateTextEncoding(kTextEncodingUS_ASCII, kTextEncodingDefaultVariant, kTextEncodingDefaultFormat))	// this encoding isn't used...
#elif WIN
	XTextTranscoder(CP_UTF8)
#endif
{
	mEncoding = L"iso-10646-utf-2";

#if MAC
	mTo16TEC = nil;
	mFrom16TEC = nil;

	mHasSystemSupport = MSystemInfo::GetTextEncodingConverterVersion() >= 0x121;
	mUseOS = mHasSystemSupport;

#elif WIN
	CPINFO info;
	int32 succeeded = ::GetCPInfo(CP_UTF8, &info);
	mHasSystemSupport = succeeded != 0;					// Win 95 doesn't support utf-8 conversions...
#endif
}


//---------------------------------------------------------------
//
// XUTF8Transcoder::Clone
//
//---------------------------------------------------------------
XUTF8Transcoder* XUTF8Transcoder::Clone() const
{
	return new XUTF8Transcoder;
}


//---------------------------------------------------------------
//
// XUTF8Transcoder::IsValidUTF8							[static]
//
//---------------------------------------------------------------
bool XUTF8Transcoder::IsValidUTF8(const char* src, uint32 numChars)
{
	PRECONDITION(src != nil);
	
	bool valid = true;

	uint32 index = 0;
	while (index < numChars && valid) {
		char ch = src[index];
		int32 count = 0;
		
		// 0xxxxxxx 
		if ((ch & 0x80) == 0)
			++index;

		// 11110uuu 10uuzzzz 10yyyyyy 10xxxxxx
		else if ((ch & 0xF8) == 0xF0) {
			++index;
			count = 3;
		
		// 1110zzzz 10yyyyyy 10xxxxxx
		} else if ((ch & 0xE0) == 0xE0) {
			++index;
			count = 2;
		
		// 110yyyyy 10xxxxxx
		} else if ((ch & 0xE0) == 0xC0) {
			++index;
			count = 1;
			
		} else
			valid = false;
			
		while (valid && count--) {
			if (index >= numChars)
				valid = false;
			
			else if ((src[index++] & 0xC0) != 0x80)		// 10uuzzzz
				valid = false;
		}
	}
	
	return valid;
}


//---------------------------------------------------------------
//
// XUTF8Transcoder::IsValidUTF16						[static]
//
//---------------------------------------------------------------
bool XUTF8Transcoder::IsValidUTF16(const wchar_t* src, uint32 numChars)
{
	PRECONDITION(src != nil);
	
	bool valid = true;
	uint32 index = 0;
	while (index < numChars && valid) {
		wchar_t ch = src[index++];
		
		if (ch >= 0xD800 && ch <= 0xDBFF) {			// high surrogate
			if (index >= numChars)
				valid = false;
			
			else {
				ch = src[index++];
				if (ch < 0xDC00 || ch > 0xDFFF)		// low surrogate expected
					valid = false;
			}
		
		} else if (ch >= 0xDC00 && ch <= 0xDFFF)	// misplaced low surrogate
			valid = false;
			
		else if (ch == 0xFFFD)						// replacement character
			valid = false;
			
		else
			valid = Whisper::IsValid(ch);
	}
		
	return valid;
}


//---------------------------------------------------------------
//
// XUTF8Transcoder::ConvertToUTF16
//
// The Unicode Converter doesn't support Unicode to Unicode
// conversions so we have to fall back on the Text Encoding
// Converter. (It'd be simpler to use TEC for everything, but
// the Unicode Converter offers more control over how the
// conversion is performed via the flags argument).
//
//---------------------------------------------------------------
uint32 XUTF8Transcoder::ConvertToUTF16(const char* src, uint32 srcBytes, wchar_t* dst, uint32 dstBytes)
{
	PRECONDITION(src != nil);
	PRECONDITION(dst != nil);
	
	uint32 uniBytes = 0;

#if MAC
	bool rejectInvalid = (mToUTF16Flags & kUnicodeUseFallbacksMask) == 0;
#elif WIN
	bool rejectInvalid = (mToUTF16Flags & MB_ERR_INVALID_CHARS) == MB_ERR_INVALID_CHARS;
#endif
	if (rejectInvalid)
		if (!XUTF8Transcoder::IsValidUTF8(src, srcBytes))	// TEC won't do this for us
			ThrowErr(kNoTranslation);						// MultiByteToWideChar should do this, but I get a bad flags error when I use MB_ERR_INVALID_CHARS
				
	if (mHasSystemSupport && mUseOS && srcBytes > 0) {
#if MAC
		if (mTo16TEC == nil) {
			uint32 encoding16 = CreateTextEncoding(kTextEncodingUnicodeDefault, kTextEncodingDefaultVariant, kUnicode16BitFormat);
			uint32 encoding8  = CreateTextEncoding(kTextEncodingUnicodeDefault, kTextEncodingDefaultVariant, kUnicodeUTF8Format);
	
			OSStatus err = ::TECCreateConverter(&mTo16TEC, encoding8, encoding16);
			ThrowIfOSErr(err);
		}
				
		OSStatus err = ::TECClearConverterContextInfo(mTo16TEC);
		ThrowIfOSErr(err);

		uint32 bytesConverted = 0;
		err = ::TECConvertText(mTo16TEC, 			// the text converter object
							   (ConstTextPtr) src, 	// the input stream
							   srcBytes, 			// byte length of input stream
							   &bytesConverted, 	// number of input bytes that were translated
							   (TextPtr) dst, 		// output buffer
							   dstBytes, 			// byte length of output buffer
							   &uniBytes);			// byte length of converted chars
	
		if (err != noErr && err != kTECUsedFallbacksStatus)
			ThrowErr(err);
		ASSERT(bytesConverted == srcBytes);

#elif WIN			
		// $$$ NT 4 and Win98 don't properly handle surrogate characters (they
		// $$$ get mapped to 0x0000 and the fallback character 0xFFFD). 
		uniBytes = Inherited::ConvertToUTF16(src, srcBytes, dst, dstBytes);
#endif

	} else if (srcBytes > 0) {
		UTF8* source = (UTF8*) src;
		UTF16* target = (UTF16*) dst;
		
		ConversionResult result = ConvertUTF8toUTF16(&source, source + srcBytes, &target, target + dstBytes/2);
		if (result == sourceExhausted)
			ThrowErr(kPartialConversion);
		else if (result == targetExhausted)
			ThrowErr(kBufferExhausted);
			
		uniBytes = 2UL*(target - (UTF16*) dst);
	}
	
	if (rejectInvalid)  
		if (!XUTF8Transcoder::IsValidUTF16(dst, uniBytes/2))
			ThrowErr(kNoTranslation);

	return uniBytes;
}


//---------------------------------------------------------------
//
// XUTF8Transcoder::GetDstBytes (char*, uint32)
//
//---------------------------------------------------------------
uint32 XUTF8Transcoder::GetDstBytes(const char* src, uint32 srcBytes)
{	
	uint32 dstBytes = 0;
	
	if (mUseOS && mHasSystemSupport)
		dstBytes = Inherited::GetDstBytes(src, srcBytes);
	else 
		dstBytes = 2*srcBytes;

	return dstBytes;
}


//---------------------------------------------------------------
//
// XUTF8Transcoder::ConvertFromUTF16
//
//---------------------------------------------------------------
uint32 XUTF8Transcoder::ConvertFromUTF16(const wchar_t* src, uint32 srcBytes, char* dst, uint32 dstBytes)
{
	PRECONDITION(src != nil);
	PRECONDITION(dst != nil);
			
	uint32 charBytes = 0;	
	
#if MAC
	bool rejectInvalid = (mFromUTF16Flags & kUnicodeUseFallbacksMask) == 0;
#elif WIN
	bool rejectInvalid = !mHasSystemSupport || !mUseOS;
#endif
	if (rejectInvalid)
		if (!XUTF8Transcoder::IsValidUTF16(src, srcBytes/2))	// TEC won't do this for us
			ThrowErr(kNoTranslation);							
				
	if (mHasSystemSupport && mUseOS && srcBytes > 0) {
#if MAC
		if (mFrom16TEC == nil) {						// transcoder objects are cloned when copying strings so we'll defer the heavy lifting until as late as possible
			uint32 encoding16 = CreateTextEncoding(kTextEncodingUnicodeDefault, kTextEncodingDefaultVariant, kUnicode16BitFormat);
			uint32 encoding8  = CreateTextEncoding(kTextEncodingUnicodeDefault, kTextEncodingDefaultVariant, kUnicodeUTF8Format);
		
			OSStatus err = ::TECCreateConverter(&mFrom16TEC, encoding16, encoding8);
			ThrowIfOSErr(err);
		} 
			
		OSStatus err = ::TECClearConverterContextInfo(mFrom16TEC);
		ThrowIfOSErr(err);

		uint32 bytesConverted = 0;
		err = ::TECConvertText(mFrom16TEC, 			// the text converter object
							   (ConstTextPtr) src, 	// the input stream
							   srcBytes, 			// byte length of input stream
							   &bytesConverted, 	// number of input bytes that were translated
							   (TextPtr) dst, 		// output buffer
							   dstBytes, 			// byte length of output buffer
							   &charBytes);			// byte length of converted chars
	
		if (err != noErr && err != kTECUsedFallbacksStatus)
			ThrowErr(err);
		ASSERT(bytesConverted == srcBytes);

			
#elif WIN 
		charBytes = Inherited::ConvertFromUTF16(src, srcBytes, dst, dstBytes);
#endif

	} else if (srcBytes > 0) {
		UTF16* source = (UTF16*) src;
		UTF8* target = (UTF8*) dst;
		
		ConversionResult result = ConvertUTF16toUTF8(&source, source + srcBytes/2, &target, target + dstBytes);
		if (result == sourceExhausted)
			ThrowErr(kPartialConversion);
		else if (result == targetExhausted)
			ThrowErr(kBufferExhausted);
			
		charBytes = numeric_cast<uint32>(target - (UTF8*) dst);
	}
		
	return charBytes;
}


//---------------------------------------------------------------
//
// XUTF8Transcoder::GetDstBytes (wchar_t*, uint32)
//
//---------------------------------------------------------------
uint32 XUTF8Transcoder::GetDstBytes(const wchar_t* src, uint32 srcBytes)
{
	uint32 dstBytes = 0;
	
	if (mUseOS && mHasSystemSupport)
		dstBytes = Inherited::GetDstBytes(src, srcBytes);
	else 
		dstBytes = 2*srcBytes;		// worst case: each utf-16 character is 4 utf-8 bytes

	return dstBytes;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XEUCJPTranscoder
// ===================================================================================

#if MAC
//---------------------------------------------------------------
//
// XEUCJPTranscoder::~XEUCJPTranscoder
//
//---------------------------------------------------------------
XEUCJPTranscoder::~XEUCJPTranscoder()
{
}


//---------------------------------------------------------------
//
// XEUCJPTranscoder::XEUCJPTranscoder
//
//---------------------------------------------------------------
XEUCJPTranscoder::XEUCJPTranscoder(bool useDecomposedCharacters) : 
	XTextTranscoder(MyCreateTextEncoding(kTextEncodingEUC_JP, (uint32) (useDecomposedCharacters ? kUnicodeCanonicalDecompVariant : kTextEncodingDefaultVariant), kTextEncodingDefaultFormat), useDecomposedCharacters)
{
	mEncoding = L"iso-646";
}


//---------------------------------------------------------------
//
// XEUCJPTranscoder::Clone
//
//---------------------------------------------------------------
XEUCJPTranscoder* XEUCJPTranscoder::Clone() const
{
	return new XEUCJPTranscoder;
}
#endif	// MAC

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XShiftJISTranscoder
// ===================================================================================

#if MAC
//---------------------------------------------------------------
//
// XShiftJISTranscoder::~XShiftJISTranscoder
//
//---------------------------------------------------------------
XShiftJISTranscoder::~XShiftJISTranscoder()
{
}


//---------------------------------------------------------------
//
// XShiftJISTranscoder::XShiftJISTranscoder
//
//---------------------------------------------------------------
XShiftJISTranscoder::XShiftJISTranscoder(bool useDecomposedCharacters) : 
	XTextTranscoder(MyCreateTextEncoding(kTextEncodingShiftJIS, (uint32) (useDecomposedCharacters ? kUnicodeCanonicalDecompVariant : kTextEncodingDefaultVariant), kTextEncodingDefaultFormat), useDecomposedCharacters)
{
	mEncoding = L"shift-jis";
}


//---------------------------------------------------------------
//
// XShiftJISTranscoder::Clone
//
//---------------------------------------------------------------
XShiftJISTranscoder* XShiftJISTranscoder::Clone() const
{
	return new XShiftJISTranscoder;
}
#endif	// MAC


}	// namespace Whisper
