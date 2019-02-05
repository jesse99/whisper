/*
 *  File:       XTextTranscodersTest.cpp
 *  Summary:   	Unit test for the text transcoders.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTextTranscodersTest.cpp,v $
 *		Revision 1.2  2000/11/09 12:56:07  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/02/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XTextTranscodersTest.h>		

#include <XTextTranscoders.h>

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class XTranscoderUnitTest
// ===================================================================================	

//---------------------------------------------------------------
//
// XTranscoderUnitTest::~XTranscoderUnitTest
//
//---------------------------------------------------------------
XTranscoderUnitTest::~XTranscoderUnitTest()
{
}

	
//---------------------------------------------------------------
//
// XTranscoderUnitTest::XTranscoderUnitTest
//
//---------------------------------------------------------------
XTranscoderUnitTest::XTranscoderUnitTest() : XUnitTest(L"Backend", L"Text Transcoders")
{
}

						
//---------------------------------------------------------------
//
// XTranscoderUnitTest::OnTest
//
//---------------------------------------------------------------
void XTranscoderUnitTest::OnTest()
{
	this->DoNaiveTest();
	this->DoAsciiTest();
	this->DoMacTest();
	this->DoWinTest();
	this->DoUTF8Test();

	TRACE("Completed text transcoder test.\n\n");
}


//---------------------------------------------------------------
//
// XTranscoderUnitTest::DoNaiveTest
//
//---------------------------------------------------------------
void XTranscoderUnitTest::DoNaiveTest()
{
	XNaiveTranscoder transcoder;
	
	const char* ascii = "beta (%) ellipsis (.) divide (/)";			// results are only valid for 7-bit ASCII
	const wchar_t* unicode = L"beta (%) ellipsis (.) divide (/)";
	
	// Test conversion to Unicode
	uint32 count = std::strlen(ascii);
	uint32 dstBytes = transcoder.GetDstBytes(ascii, count);
	ASSERT(dstBytes >= 2*count);

	wchar_t buffer1[256];
	uint32 written = transcoder.ConvertToUTF16(ascii, count, buffer1, sizeof(buffer1));
	ASSERT(written == 2*count);
	ASSERT(std::wcsncmp(unicode, buffer1, std::wcslen(unicode)) == 0);
	 
	// Test conversion from Unicode
	count = std::wcslen(unicode);
	dstBytes = transcoder.GetDstBytes(unicode, 2*count);
	ASSERT(dstBytes >= count);

	char buffer2[256];
	written = transcoder.ConvertFromUTF16(unicode, 2*count, buffer2, sizeof(buffer2));
	ASSERT(written == count);
	ASSERT(std::strncmp(ascii, buffer2, std::strlen(ascii)) == 0);
}


//---------------------------------------------------------------
//
// XTranscoderUnitTest::DoAsciiTest
//
//---------------------------------------------------------------
void XTranscoderUnitTest::DoAsciiTest()
{
	XASCIITranscoder transcoder;
	
	const char* ascii = "beta (%) ellipsis (.) divide (/)";			// results are only valid for 7-bit ASCII
	const wchar_t* unicode = L"beta (%) ellipsis (.) divide (/)";
	
	// Test conversion to Unicode
	uint32 count = std::strlen(ascii);
	uint32 dstBytes = transcoder.GetDstBytes(ascii, count);
	ASSERT(dstBytes >= 2*count);

	wchar_t buffer1[256];
	uint32 written = transcoder.ConvertToUTF16(ascii, count, buffer1, sizeof(buffer1));
	ASSERT(written == 2*count);
	ASSERT(std::wcsncmp(unicode, buffer1, std::wcslen(unicode)) == 0);
	
	// Test conversion from Unicode
	count = std::wcslen(unicode);
	dstBytes = transcoder.GetDstBytes(unicode, 2*count);
	ASSERT(dstBytes >= count);

	char buffer2[256];
	written = transcoder.ConvertFromUTF16(unicode, 2*count, buffer2, sizeof(buffer2));
	ASSERT(written == count);
	ASSERT(std::strncmp(ascii, buffer2, std::strlen(ascii)) == 0);
}


//---------------------------------------------------------------
//
// XTranscoderUnitTest::DoMacTest
//
//---------------------------------------------------------------
void XTranscoderUnitTest::DoMacTest()
{
	XMacRomanTranscoder transcoder;
	
	const char* ascii = "beta (§) ellipsis (É) divide (Ö)";
	const wchar_t* unicode = L"beta (\x00DF) ellipsis (\x2026) divide (\x00F7)";
	
	// Test conversion to Unicode
	uint32 count = std::strlen(ascii);
	uint32 dstBytes = transcoder.GetDstBytes(ascii, count);
	ASSERT(dstBytes >= 2*count);

	wchar_t buffer1[256];
	uint32 written = transcoder.ConvertToUTF16(ascii, count, buffer1, sizeof(buffer1));
	ASSERT(written == 2*count);
	ASSERT(std::wcsncmp(unicode, buffer1, std::wcslen(unicode)) == 0);
	
	// Test conversion from Unicode
	count = std::wcslen(unicode);
	dstBytes = transcoder.GetDstBytes(unicode, 2*count);
	ASSERT(dstBytes >= count);

	char buffer2[256];
	written = transcoder.ConvertFromUTF16(unicode, 2*count, buffer2, sizeof(buffer2));
	ASSERT(written == count);
	ASSERT(std::strncmp(ascii, buffer2, std::strlen(ascii)) == 0);
}


//---------------------------------------------------------------
//
// XTranscoderUnitTest::DoWinTest
//
//---------------------------------------------------------------
void XTranscoderUnitTest::DoWinTest()
{
	XWindowsLatin1Transcoder transcoder;
	
	const char* ascii = "copyright (\xA9) pound (\xA3) divide (\xF7)";
	const wchar_t* unicode = L"copyright (\x00A9) pound (\x00A3) divide (\x00F7)";
	
	// Test conversion to Unicode
	uint32 count = std::strlen(ascii);
	uint32 dstBytes = transcoder.GetDstBytes(ascii, count);
	ASSERT(dstBytes >= 2*count);

	wchar_t buffer1[256];
	uint32 written = transcoder.ConvertToUTF16(ascii, count, buffer1, sizeof(buffer1));
	ASSERT(written == 2*count);
	ASSERT(std::wcsncmp(unicode, buffer1, std::wcslen(unicode)) == 0);
	
	// Test conversion from Unicode
	count = std::wcslen(unicode);
	dstBytes = transcoder.GetDstBytes(unicode, 2*count);
	ASSERT(dstBytes >= count);

	char buffer2[256];
	written = transcoder.ConvertFromUTF16(unicode, 2*count, buffer2, sizeof(buffer2));
	ASSERT(written == count);
	ASSERT(std::strncmp(ascii, buffer2, std::strlen(ascii)) == 0);
}


//---------------------------------------------------------------
//
// XTranscoderUnitTest::DoUTF8Test
//
//---------------------------------------------------------------
void XTranscoderUnitTest::DoUTF8Test()
{
	XUTF8Transcoder transcoder;
	
	const char* utf8 = "ellipsis (\xE2\x80\xA6) divide (\xC3\xB7)";
	const wchar_t* unicode = L"ellipsis (\x2026) divide (\x00F7)";
	
	// Test conversion to Unicode
	uint32 dstBytes = transcoder.GetDstBytes(utf8, std::strlen(utf8));
	ASSERT(dstBytes >= 2*std::wcslen(unicode));

	wchar_t buffer1[256];
	uint32 written = transcoder.ConvertToUTF16(utf8, std::strlen(utf8), buffer1, sizeof(buffer1));
	ASSERT(written == 2*std::wcslen(unicode));
	ASSERT(std::wcsncmp(unicode, buffer1, std::wcslen(unicode)) == 0);
	
	// Test conversion from Unicode
	dstBytes = transcoder.GetDstBytes(unicode, 2*std::wcslen(unicode));
	ASSERT(dstBytes >= std::strlen(utf8));

	char buffer2[256];
	written = transcoder.ConvertFromUTF16(unicode, 2*std::wcslen(unicode), buffer2, sizeof(buffer2));
	ASSERT(written == std::strlen(utf8));
	ASSERT(std::strncmp(utf8, buffer2, std::strlen(utf8)) == 0);
}

#endif	// DEBUG


}	// namespace Whisper
