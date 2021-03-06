/*
 *  File:		XTextTranscoders.h
 *  Summary:	Classes used to convert to and from Unicode.
 *  Written by:	Jesse Jones
 *
 *	Classes:	XNaiveTranscoder		 - inserts and drops zeros (this is fast but not correct for ASCII codes over 0x7F)
 *				XASCIITranscoder		 - converts standard 7-bit US-ASCII (ie this won't work properly for the non-standard characters used in the Mac and Windows character sets)
 *				XMacRomanTranscoder		 - converts characters used by the default Mac character set
 * 				XWindowsLatin1Transcoder - converts characters used by the default Windows character set
 *				XUTF8Transcoder			 - converts characters encoded in 8-byte Unicode (note that this is a multi-byte encoding: characters may be represented using multiple bytes)
 *
 *				XEUCJPTranscoder		 - converts characters encoded in EUC-JP
 *				XShiftJISTranscoder		 - converts characters encoded in Shift-JIS
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTextTranscoders.h,v $
 *		Revision 1.4  2001/03/05 05:43:55  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:56:28  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:56:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTextTranscoder.h>		

#if MAC
	#include <TextEncodingConverter.h>
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XNaiveTranscoder
//!		inserts and drops zeros (this is fast but not correct for ASCII codes over 0x7F)
// ===================================================================================
class CORE_EXPORT XNaiveTranscoder : public XTextTranscoder {

	typedef XTextTranscoder Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XNaiveTranscoder();

						XNaiveTranscoder(bool useDecomposedCharacters = false);
						
	virtual XNaiveTranscoder* Clone() const;
};


// ===================================================================================
//	class XASCIITranscoder
//!		converts standard 7-bit US-ASCII (ie this won't work properly for the non-standard characters used in the Mac and Windows character sets)
// ===================================================================================
class CORE_EXPORT XASCIITranscoder : public XTextTranscoder {

	typedef XTextTranscoder Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XASCIITranscoder();

						XASCIITranscoder(bool useDecomposedCharacters = false);
						
	virtual XASCIITranscoder* Clone() const;
	
//-----------------------------------
//	Inherited API
//
public:			
#if WIN
	virtual uint32 		ConvertFromUTF16(const wchar_t* src, uint32 srcBytes, char* dst, uint32 dstBytes);
#endif
};


// ===================================================================================
//	class XMacRomanTranscoder
//!		converts characters used by the default Mac character set	
// ===================================================================================
class CORE_EXPORT XMacRomanTranscoder : public XTextTranscoder {

	typedef XTextTranscoder Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XMacRomanTranscoder();

						XMacRomanTranscoder(bool useDecomposedCharacters = false);
						
	virtual XMacRomanTranscoder* Clone() const;

//-----------------------------------
//	Inherited API
//
public:			
#if WIN
	virtual uint32 		ConvertToUTF16(const char* src, uint32 srcBytes, wchar_t* dst, uint32 dstBytes);
	virtual uint32 		GetDstBytes(const char* src, uint32 srcBytes);

	virtual uint32 		ConvertFromUTF16(const wchar_t* src, uint32 srcBytes, char* dst, uint32 dstBytes);
	virtual uint32 		GetDstBytes(const wchar_t* src, uint32 srcBytes);
#endif

//-----------------------------------
//	Member Data
//
protected:	
#if WIN
	bool	mHasMacCodePage;
#endif
};


// ===================================================================================
//	class XWindowsLatin1Transcoder
//!		converts characters used by the default Windows character set
// ===================================================================================
class CORE_EXPORT XWindowsLatin1Transcoder : public XTextTranscoder {

	typedef XTextTranscoder Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XWindowsLatin1Transcoder();

						XWindowsLatin1Transcoder(bool useDecomposedCharacters = false);
						
	virtual XWindowsLatin1Transcoder* Clone() const;
};


// ===================================================================================
//	class XUTF8Transcoder
//!		converts characters encoded in 8-byte Unicode (note that this is a multi-byte encoding: characters may be represented using multiple bytes)
// ===================================================================================
class CORE_EXPORT XUTF8Transcoder : public XTextTranscoder {

	typedef XTextTranscoder Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XUTF8Transcoder();

						XUTF8Transcoder();
						
	virtual XUTF8Transcoder* Clone() const;

//-----------------------------------
//	New API
//
public:
	static	bool 		IsValidUTF8(const char* src, uint32 numChars);			
						/**< Returns false if the utf-8 characters are ill formed (note
						that this does not check to see of the Unicode characters
						are assigned). */

	static	bool 		IsValidUTF16(const wchar_t* src, uint32 numChars);			
						/**< Returns false if any of the characters are unassigned. */

//-----------------------------------
//	Inherited API
//
public:			
	virtual uint32 		ConvertToUTF16(const char* src, uint32 srcBytes, wchar_t* dst, uint32 dstBytes);
	virtual uint32 		GetDstBytes(const char* src, uint32 srcBytes);

	virtual uint32 		ConvertFromUTF16(const wchar_t* src, uint32 srcBytes, char* dst, uint32 dstBytes);
	virtual uint32 		GetDstBytes(const wchar_t* src, uint32 srcBytes);

//-----------------------------------
//	Member Data
//
protected:	
#if MAC
	TECObjectRef	mTo16TEC;
	TECObjectRef	mFrom16TEC;
#endif

	bool			mHasSystemSupport;
};


// ===================================================================================
//	class XEUCJPTranscoder
//!		converts characters encoded in EUC-JP
// ===================================================================================
#if MAC
class CORE_EXPORT XEUCJPTranscoder : public XTextTranscoder {

	typedef XTextTranscoder Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XEUCJPTranscoder();

						XEUCJPTranscoder(bool useDecomposedCharacters = false);
						
	virtual XEUCJPTranscoder* Clone() const;
};
#endif


// ===================================================================================
//	class XShiftJISTranscoder
//!		converts characters encoded in Shift-JIS
// ===================================================================================
#if MAC
class CORE_EXPORT XShiftJISTranscoder : public XTextTranscoder {

	typedef XTextTranscoder Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XShiftJISTranscoder();

						XShiftJISTranscoder(bool useDecomposedCharacters = false);
						
	virtual XShiftJISTranscoder* Clone() const;
};
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
