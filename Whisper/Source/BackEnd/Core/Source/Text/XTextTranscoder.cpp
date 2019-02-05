/*
 *  File:		XTextTranscoder.cpp
 *  Summary:	Abstract base class used to convert to and from Unicode.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTextTranscoder.cpp,v $
 *		Revision 1.4  2001/04/27 04:37:03  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2000/12/10 04:12:17  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:55:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 2/04/00	JDJ		ConvertToUTF16 casts the char to a uint8 before casting
 *									to an XChar.
 *		 <2>	 5/07/99	JDJ		Added mEncoding.
 *		 <1>	 1/25/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XTextTranscoder.h>		

#include <XExceptions.h>
#include <XNumbers.h>

#if MAC
	#include <MSystemInfo.h>
#endif

namespace Whisper {


// ===================================================================================
//	class XTextTranscoder
// ===================================================================================

#if DEBUG
	int32 XTextTranscoder::msFlagToUTF16Conversions = 0;
#endif

//---------------------------------------------------------------
//
// XTextTranscoder::~XTextTranscoder
//
//---------------------------------------------------------------
XTextTranscoder::~XTextTranscoder()
{
#if MAC	
	if (mToInfo != nil)
		VERIFY(::DisposeTextToUnicodeInfo(&mToInfo) == noErr);
		
	if (mFromInfo != nil)
		VERIFY(::DisposeUnicodeToTextInfo(&mFromInfo) == noErr);
#endif
}


//---------------------------------------------------------------
//
// XTextTranscoder::XTextTranscoder	
//
// $$ The TEC docs warn against using the same Unicode converter 
// $$ object with different text streams.  The reason given is 
// $$ "because the Unicode Converter stores private state information
// $$ in a Unicode converter object that is relevant only to the 
// $$ single text stream for which it is used". Presumably this
// $$ applies to converting formats like utf-8 in chunks where
// $$ the chunk boundaries can split characters. Since our API
// $$ doesn't support this sort of chunking we should be able to	
// $$ get away with reusing the converter objects...
//
//---------------------------------------------------------------
XTextTranscoder::XTextTranscoder(uint32 format, bool useDecomposedCharacters)
{
	mFormat = format;
	mUseDecomposedCharacters = useDecomposedCharacters;
	
	mEncoding = nil;			// set by subclasses
	
#if MAC	
	mToInfo = nil;
	mFromInfo = nil;
	mUseOS = false;
	
#else
	mUseOS = true;
#endif

	this->SetToUTF16Flags(0);
	this->SetFromUTF16Flags(0);
}


//---------------------------------------------------------------
//
// XTextTranscoder::SetToUTF16Flags
//
//---------------------------------------------------------------
void XTextTranscoder::SetToUTF16Flags(uint32 flags)										
{
#if MAC
	mToUTF16Flags = kUnicodeUseFallbacksMask | kUnicodeLooseMappingsMask;

	if ((flags & kRejectInvalidCharacters) == kRejectInvalidCharacters)
		mToUTF16Flags &= ~kUnicodeUseFallbacksMask;

	if ((flags & kStrictCharMapping) == kStrictCharMapping)
		mToUTF16Flags &= ~kUnicodeLooseMappingsMask;

#elif WIN
	mToUTF16Flags = 0;

	if ((flags & kRejectInvalidCharacters) == kRejectInvalidCharacters)
		mToUTF16Flags |= MB_ERR_INVALID_CHARS;
#endif
}


//---------------------------------------------------------------
//
// XTextTranscoder::SetFromUTF16Flags
//
//---------------------------------------------------------------
void XTextTranscoder::SetFromUTF16Flags(uint32 flags)										
{
#if MAC
	mFromUTF16Flags = kUnicodeUseFallbacksMask | kUnicodeLooseMappingsMask;

	if ((flags & kRejectInvalidCharacters) == kRejectInvalidCharacters)
		mFromUTF16Flags &= ~kUnicodeUseFallbacksMask;

	if ((flags & kStrictCharMapping) == kStrictCharMapping)
		mFromUTF16Flags &= ~kUnicodeLooseMappingsMask;

	if ((flags & kLeftToRightText) == kLeftToRightText)
		mFromUTF16Flags |= kUnicodeLeftToRightMask;
	else if ((flags & kRightToLeftText) == kRightToLeftText)
		mFromUTF16Flags |= kUnicodeRightToLeftMask;
	else
		mFromUTF16Flags = mFromUTF16Flags | kUnicodeDefaultDirectionMask;	// get a side-effect warning if I use |=

#elif WIN
	mFromUTF16Flags = 0;

	if ((flags & kRejectInvalidCharacters) == kRejectInvalidCharacters)
		mFromUTF16Flags |= MB_ERR_INVALID_CHARS;	
#endif
}


//---------------------------------------------------------------
//
// XTextTranscoder::ConvertToUTF16
//
//---------------------------------------------------------------
uint32 XTextTranscoder::ConvertToUTF16(const char* src, uint32 srcBytes, wchar_t* dst, uint32 dstBytes)
{
	PRECONDITION(src != nil);
	PRECONDITION(dst != nil);
	
	uint32 uniBytes = 0;
	
#if MAC						
	if (mToInfo == nil && MSystemInfo::HasTextEncodingConverter()) {	// transcoder objects are cloned when copying strings so we'll defer the heavy lifting until as late as possible
		UnicodeMapping mapping;
		mapping.unicodeEncoding = CreateTextEncoding(kTextEncodingUnicodeV2_1, kTextEncodingDefaultVariant, kUnicode16BitFormat);
		mapping.otherEncoding   = mFormat;
		mapping.mappingVersion  = kUnicodeUseLatestMapping;
	
		OSStatus err = ::CreateTextToUnicodeInfo(&mapping, &mToInfo);	
		ThrowIfOSErr(err);

		mUseOS = true;
	}
#endif
	
	if (mUseOS) {
#if MAC						
		uint32 bytesConverted = 0;
		OSStatus err = ::ConvertFromTextToUnicode(mToInfo, 				// OS converter object
												  srcBytes, 			// byte length of src string
												  src, 					// pointer to src string
												  mToUTF16Flags,		// flags
												  0, 					// number of offsets
												  nil, 					// offset array (for style changes)
												  nil, 					// returns number of offsets that were mapped
												  nil, 					// returns an array of new offsets
												  dstBytes, 			// byte length of dst string
												  &bytesConverted, 		// returns the number of converted bytes from src
												  &uniBytes, 			// byte length of dst string
												  (UniChar*) dst);		// dst string
	
		if (err != noErr && err != kTECUsedFallbacksStatus)
			ThrowErr(err);
			
#if DEBUG
	if (msFlagToUTF16Conversions > 0) 
		DEBUGSTR("Converted char*: ", std::wstring(dst, dstBytes/sizeof(wchar_t)));
#endif

#elif WIN
		if (srcBytes > 0 && dstBytes >= 2) {
			uint32 flags = mToUTF16Flags;
			if (mUseDecomposedCharacters)	
				flags |= MB_COMPOSITE;
			else if (mFormat != CP_UTF8)
				flags |= MB_PRECOMPOSED;	
			if (mFormat == CP_UTF8)
				flags &= (uint32) (~MB_ERR_INVALID_CHARS);		// XUTF8Transcoder will handle the check
			
			uniBytes = 2UL*::MultiByteToWideChar(mFormat, flags, src, numeric_cast<int32>(srcBytes), dst, numeric_cast<int32>(dstBytes/2));	// note that this function is present in Win95
			ThrowIf(uniBytes == 0);
		}

#if DEBUG
	if (msFlagToUTF16Conversions > 0 && mFormat != CP_UTF8) 
		DEBUGSTR("Converted char*: '", std::wstring(dst, dstBytes/sizeof(wchar_t)));
#endif

#endif
	
	} else {
		uint32 dstLen = dstBytes/2;
			
		uint32 index = 0;
		while (src[index] != '\0' && index < dstLen) {
			dst[index] = (wchar_t) (uint8) src[index];
			++index;
		}
		
		uniBytes = 2*index;
	}
	
	return uniBytes;
}


//---------------------------------------------------------------
//
// XTextTranscoder::GetDstBytes (char*, uint32)	CP_UTF8
//
//---------------------------------------------------------------
uint32 XTextTranscoder::GetDstBytes(const char* src, uint32 srcBytes)
{
	PRECONDITION(src != nil);
	
	uint32 dstBytes = 0;
	
	if (mUseOS) {
#if MAC	
		dstBytes = 4*srcBytes;				// this is the value recommended by Apple, note that MacRoman and WindowsLatin1 can use more than 2x as many bytes with noncomposed Unicode

#elif WIN
		if (srcBytes > 0) {
			uint32 flags = mToUTF16Flags;	
			if (mUseDecomposedCharacters)	
				flags |= MB_COMPOSITE;
			else if (mFormat != CP_UTF8)
				flags |= MB_PRECOMPOSED;
			if (mFormat == CP_UTF8)
				flags &= (uint32) (~MB_ERR_INVALID_CHARS);		// XUTF8Transcoder will handle the check

			dstBytes = 2UL*::MultiByteToWideChar(mFormat, flags, src, numeric_cast<int32>(srcBytes), nil, 0);
			ThrowIf(dstBytes == 0);			
		} 
#endif

	} else 
		dstBytes = 2*srcBytes;

	return dstBytes;
}


//---------------------------------------------------------------
//
// XTextTranscoder::ConvertFromUTF16
//
//---------------------------------------------------------------
uint32 XTextTranscoder::ConvertFromUTF16(const wchar_t* src, uint32 srcBytes, char* dst, uint32 dstBytes)
{
	PRECONDITION(src != nil);
	PRECONDITION(dst != nil);
			
	uint32 charBytes = 0;	
	
#if MAC						
	if (mFromInfo == nil && MSystemInfo::HasTextEncodingConverter()) {
		UnicodeMapping mapping;
		mapping.unicodeEncoding = CreateTextEncoding(kTextEncodingUnicodeV2_1, kTextEncodingDefaultVariant, kUnicode16BitFormat);
		mapping.otherEncoding   = mFormat;
		mapping.mappingVersion  = kUnicodeUseLatestMapping;
	
		OSStatus err = ::CreateUnicodeToTextInfo(&mapping, &mFromInfo);	
		ThrowIfOSErr(err);

		mUseOS = true;
	}
#endif
	
	if (mUseOS) {
#if MAC
		uint32 bytesConverted = 0;
		OSStatus err = ::ConvertFromUnicodeToText(mFromInfo, 			// OS converter object
												  srcBytes, 			// byte length of src string
												  (UniChar*) src, 		// pointer to src string
												  mFromUTF16Flags,		// flags
												  0, 					// number of offsets
												  nil, 					// offset array (for style changes)
												  nil, 					// returns number of offsets that were mapped
												  nil, 					// returns an array of new offsets
												  dstBytes, 			// byte length of dst string
												  &bytesConverted, 		// returns the number of converted bytes from src
												  &charBytes, 			// byte length of dst string
												  (UniChar*) dst);		// dst string
	
		if (err != noErr && err != kTECUsedFallbacksStatus)
			ThrowErr(err);
			
#elif WIN
		if (srcBytes >= 2 && dstBytes > 0) {
			uint32 flags = mFromUTF16Flags;
			if (mUseDecomposedCharacters)
				flags |= WC_COMPOSITECHECK + WC_SEPCHARS;
			
			if (flags & MB_ERR_INVALID_CHARS) {
				BOOL usedDefault;
				charBytes = (uint32) ::WideCharToMultiByte(mFormat, flags & ~MB_ERR_INVALID_CHARS, src, numeric_cast<int32>(srcBytes/2), dst, numeric_cast<int32>(dstBytes), nil, &usedDefault);	
				if (usedDefault)
					ThrowErr(ERROR_NO_UNICODE_TRANSLATION);				

			} else
				charBytes = (uint32) ::WideCharToMultiByte(mFormat, flags, src, numeric_cast<int32>(srcBytes/2), dst, numeric_cast<int32>(dstBytes), nil, nil);	// note that this function is present in Win95
			ThrowIf(charBytes == 0);				
		}
#endif

	} else {
		uint32 index = 0;
		while (src[index] != '\0' && index < dstBytes) {
			dst[index] = (char) src[index];
			++index;
		}
		
		charBytes = index;
	}
	
	return charBytes;
}


//---------------------------------------------------------------
//
// XTextTranscoder::GetDstBytes (wchar_t*, uint32)
//
//---------------------------------------------------------------
uint32 XTextTranscoder::GetDstBytes(const wchar_t* src, uint32 srcBytes)
{
	PRECONDITION(src != nil);
	
	uint32 dstBytes = 0;
			
	if (mUseOS) {
#if MAC	
		dstBytes = srcBytes;				// should work for everything but the really whacked formats like EUC-JP which has 3-byte Kanji characters

#elif WIN
		if (srcBytes >= 2) {
			uint32 flags = mFromUTF16Flags;
			if (flags & MB_ERR_INVALID_CHARS)
				flags &= (uint32) (~MB_ERR_INVALID_CHARS);		

			dstBytes = (uint32) ::WideCharToMultiByte(mFormat, flags, src, numeric_cast<int32>(srcBytes/2), nil, 0, nil, nil);
			ThrowIf(dstBytes == 0);			
		} 
#endif
	
	} else
		dstBytes = srcBytes/2;
		
	return dstBytes;
}


}	// namespace Whisper
