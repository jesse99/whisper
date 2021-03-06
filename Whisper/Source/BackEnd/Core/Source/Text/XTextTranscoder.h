/*
 *  File:		XTextTranscoder.h
 *  Summary:	Abstract base class used to convert to and from Unicode.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTextTranscoder.h,v $
 *		Revision 1.4  2001/03/05 05:43:51  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:56:23  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:55:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XDebug.h>
#include <XTypes.h>		

#if MAC
	#include <UnicodeConverter.h>
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Constants
//
// ConvertToUTF16 and ConvertFromUTF16 flags										
const uint32 kRejectInvalidCharacters 	= 0x1000;	//!< invalid characters or characters that cannot be mapped exactly will be approximated will cause the conversion routines to throw an exception if an invalid character is encountered
const uint32 kStrictCharMapping			= 0x2000;	//!< character conversions that don't preserve round trip fidelity are treated as invalid (eg Unicode has HYPHEN, MINUS SIGN, and HYPHEN-MINUS characters, when mapping to ASCII HYPHEN-MINUS is strict but HYPHEN and MINUS SIGN are loose). This is ignored on Windows.

// ConvertFromUTF16 flags										
const uint32 kLeftToRightText			= 0x3000;	//!< indicates the global line direction (used for resolving neutral characters such as spaces that occur between sets of characters having different directions (eg Latin and Arabic))
const uint32 kRightToLeftText			= 0x4000;	//!< these two are ignored on Windows


// ===================================================================================
//	class XTextTranscoder
//!		Abstract base class used to convert to and from Unicode.
// ===================================================================================
class CORE_EXPORT XTextTranscoder {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XTextTranscoder();

						XTextTranscoder(uint32 format, bool useDecomposedCharacters = false);
						/**< If useDecomposedCharacters is true characters that can be 
						represented with a base character and a non-spacing character 
						(eg an accented e) are converted to two Unicode characters 
						otherwise only one Unicode character is generated. */
						
	virtual XTextTranscoder* Clone() const = 0;
						/**< The Mac's Text Encoding Converter uses "objects" which may
						maintain internal state info so, to be thread safe, XString
						must copy the transcoder objects. */
						
private:
						XTextTranscoder(const XTextTranscoder& rhs);
						
			XTextTranscoder& operator=(const XTextTranscoder& rhs);

//-----------------------------------
//	API
//
public:				
	virtual uint32 		ConvertToUTF16(const char* src, uint32 srcBytes, wchar_t* dst, uint32 dstBytes);
	virtual uint32 		GetDstBytes(const char* src, uint32 srcBytes);
			void 		SetToUTF16Flags(uint32 flags);
						/**< ConvertToUTF16 returns the number of Unicode bytes written to dst.
						GetDstBytes returns an estimate of the number of bytes needed in
						the dst buffer. Note that ConvertToUTF16 does not append a
						trailing zero to denote the end of the string. */

	virtual uint32 		ConvertFromUTF16(const wchar_t* src, uint32 srcBytes, char* dst, uint32 dstBytes);
	virtual uint32 		GetDstBytes(const wchar_t* src, uint32 srcBytes);
			void 		SetFromUTF16Flags(uint32 flags);
			
			const wchar_t* GetEncoding() const					{ASSERT(mEncoding != nil); return mEncoding;}
						/**< Returns the standard ISO name for the transcoder:
						   us-ascii		iso646-us
						   utf-8		iso-10646-utf-2
						   utf-16		iso-10646-ucs-2
						   win-latin1	iso-8859-1
						   mac-roman	mac-roman (there doesn't appear to be an ISO name for this)
						   naive		naive */
						
//-----------------------------------
//	Member Data
//
#if DEBUG
public:
	static int32		msFlagToUTF16Conversions;		//!< if greater than zero ConvertToUTF16 will DEBUGSTR 
#endif

protected:	
	bool				mUseOS;
	uint32				mFormat;
	uint32 				mToUTF16Flags;
	uint32 				mFromUTF16Flags;
	bool				mUseDecomposedCharacters;
	
	const wchar_t* 		mEncoding;
	
#if MAC
	TextToUnicodeInfo	mToInfo;
	UnicodeToTextInfo	mFromInfo;
#endif
};


// ===================================================================================
//	class XDisableUTF16Warning
//		XString uses XTextTranscoder whenever it needs to convert to or from a char*.
//		Unfortunately this is a fairly expensive operations. Because of this it's
//		much better to use wide string literals. Clients can set msFlagToUTF16Conversions
//		so that ConvertToUTF16 DEBUGSTR's whenever it's called. However there are times
//		when there's no alternative to using a char* with XString. When this happens
//		yuo can use this class to temporarily disable the DEBUGSTR.
// ===================================================================================
class CORE_EXPORT XDisableUTF16Warning {

public:
						~XDisableUTF16Warning();

						XDisableUTF16Warning();
};


// ===================================================================================
//	Inlines
// ===================================================================================
#if DEBUG
	inline XDisableUTF16Warning::~XDisableUTF16Warning()
	{
		XTextTranscoder::msFlagToUTF16Conversions += 1;
	}
	
	inline XDisableUTF16Warning::XDisableUTF16Warning()
	{
		XTextTranscoder::msFlagToUTF16Conversions -= 1;
	}
#else
	inline XDisableUTF16Warning::~XDisableUTF16Warning()
	{
	}
	
	inline XDisableUTF16Warning::XDisableUTF16Warning()
	{
	}
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
