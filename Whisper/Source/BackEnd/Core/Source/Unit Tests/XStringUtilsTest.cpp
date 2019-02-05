/*
 *  File:       XStringUtilsTest.cpp
 *  Summary:   	Unit test for the functions in XStringUtils.h.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XStringUtilsTest.cpp,v $
 *		Revision 1.2  2000/11/09 12:54:11  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/02/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XStringUtilsTest.h>

#include <XDebug.h>
#include <XStringUtils.h>

#if WIN
	#include <WSystemInfo.h>
#endif

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class XStringUtilsUnitTest
// ===================================================================================

//---------------------------------------------------------------
//
// XStringUtilsUnitTest::~XStringUtilsUnitTest
//
//---------------------------------------------------------------
XStringUtilsUnitTest::~XStringUtilsUnitTest()
{
}

	
//---------------------------------------------------------------
//
// XStringUtilsUnitTest::XStringUtilsUnitTest
//
//---------------------------------------------------------------
XStringUtilsUnitTest::XStringUtilsUnitTest() : XUnitTest(L"Backend", L"String Utils")
{
}

						
//---------------------------------------------------------------
//
// XStringUtilsUnitTest::OnTest
//
//---------------------------------------------------------------
void XStringUtilsUnitTest::OnTest()
{
	this->DoBeforeTest();
	this->DoAfterTest();
	this->DoParseTest();
	this->DoStripTest();
	this->DoReplaceTest();
	this->DoConvertCaseTest();
	this->DoCompareTest();
	
	TRACE("Completed string utils test.\n\n");
}


//---------------------------------------------------------------
//
// XStringUtilsUnitTest::DoBeforeTest
//
//---------------------------------------------------------------
void XStringUtilsUnitTest::DoBeforeTest()
{
	std::wstring str = L"semper fi mac";
	
	ASSERT(Before(str, L" fi") == L"semper");			
	ASSERT(Before(str, L" qq") == L"");	
	ASSERT(Before(str, L"semp") == L"");	
	ASSERT(Before(str, L"mac") == L"semper fi ");	
	ASSERT(Before(str, L"Mac") == L"");	
}


//---------------------------------------------------------------
//
// XStringUtilsUnitTest::DoAfterTest
//
//---------------------------------------------------------------
void XStringUtilsUnitTest::DoAfterTest()
{
	std::wstring str = L"semper fi mac";
	
	ASSERT(After(str, L" fi") == L" mac");			
	ASSERT(After(str, L" qq") == L"");	
	ASSERT(After(str, L"semp") == L"er fi mac");	
	ASSERT(After(str, L"mac") == L"");	
	ASSERT(After(str, L"Mac") == L"");	
}


//---------------------------------------------------------------
//
// XStringUtilsUnitTest::DoParseTest
//
//---------------------------------------------------------------
void XStringUtilsUnitTest::DoParseTest()
{
	std::wstring str = L"semper fi mac";
	
	ASSERT(Parse(str, L" f") == L"semper");		
	ASSERT(str == L"i mac");
	ASSERT(Parse(str, L" f") == L"i");
	ASSERT(str == L"mac");
	ASSERT(Parse(str, L" f") == L"mac");
	ASSERT(str == L"");
}


//---------------------------------------------------------------
//
// XStringUtilsUnitTest::DoStripTest
//
//---------------------------------------------------------------
void XStringUtilsUnitTest::DoStripTest()
{
	std::wstring str = L"   oolong";								
	
	ASSERT(StripLeading(str, L" ") == L"oolong");
	ASSERT(StripLeading(str, L" o") == L"long");
	ASSERT(StripLeading(str, L" l") == L"oolong");
	ASSERT(StripLeading(str, L"o ") == L"long");
	ASSERT(StripLeading(str, L"o") == L"   oolong");
}


//---------------------------------------------------------------
//
// XStringUtilsUnitTest::DoReplaceTest
//
//---------------------------------------------------------------
void XStringUtilsUnitTest::DoReplaceTest()
{
	ASSERT(Replace(L"hello", 'l', 'k') == L"hekko");	// Replace (std::wstring, char, char)
	ASSERT(Replace(L"hekko", 'h', 'k') == L"kekko");
	ASSERT(Replace(L"kekko", 'o', 'k') == L"kekkk");
	ASSERT(Replace(L"kekkk", 'e', 'k') == L"kkkkk");
	ASSERT(Replace(L"kkkkk", 'k', 'j') == L"jjjjj");
	ASSERT(Replace(L"h", 'h', 'k') == L"k");
	ASSERT(Replace(L"", 'h', 'k') == L"");

	ASSERT(Replace(L"hello", L"l", L"k") == L"hekko");	// Replace (std::wstring, std::wstring, std::wstring)
	ASSERT(Replace(L"hekko", L"h", L"k") == L"kekko");
	ASSERT(Replace(L"kekko", L"o", L"k") == L"kekkk");
	ASSERT(Replace(L"kekkk", L"e", L"k") == L"kkkkk");
	ASSERT(Replace(L"kkkkk", L"k", L"j") == L"jjjjj");
	ASSERT(Replace(L"h", L"h", L"k") == L"k");
	ASSERT(Replace(L"", L"h", L"k") == L"");
	
	ASSERT(Replace(L"FourTenFourFour", L"Four", L"4") == L"4Ten44");
	ASSERT(Replace(L"Ten4Ten44Ten", L"Ten", L"10xxxx") == L"10xxxx410xxxx4410xxxx");
	ASSERT(Replace(L"Ten4Ten44Ten", L"xxxx", L"") == L"Ten4Ten44Ten");
		
	ASSERT(Replace(L"J&elly &&& Pea&nut Butter&", '&', '\0', '&') == L"Jelly & Peanut Butter");
	ASSERT(Replace(L"J_elly ___ Pea_nut Butter__", '_', '&', '_') == L"J&elly _& Pea&nut Butter_");
}


//---------------------------------------------------------------
//
// XStringUtilsUnitTest::DoConvertCaseTest
//
//---------------------------------------------------------------
void XStringUtilsUnitTest::DoConvertCaseTest()
{
	std::wstring str = L"WaG the DoG";
	ASSERT(ConvertToUpperCase(str) == L"WAG THE DOG");	// ConvertToUpperCase and ConvertToLowerCase
	ASSERT(ConvertToLowerCase(str) == L"wag the dog");
	
	str.resize(0);	// Unicode character name					uppercase	lowercase
	str += 0x00E1;	// LATIN SMALL LETTER A WITH ACUTE			00C1
	str += 0x00EA;	// LATIN SMALL LETTER E WITH CIRCUMFLEX		00CA
	str += 0x03B4;	// GREEK SMALL LETTER DELTA					0394
	str += 0x03A0;	// GREEK CAPITAL LETTER PI								03C0
	str += 0x0574;	// ARMENIAN SMALL LETTER MEN				0544
	str += 0x0532;	// ARMENIAN CAPITAL LETTER BEN							0562
	
	std::wstring str2 = ConvertToUpperCase(str);
	ASSERT(str2.length() == str.length());
	ASSERT(str2[0] == 0x00C1);
	ASSERT(str2[1] == 0x00CA);
	ASSERT(str2[2] == 0x0394);
	ASSERT(str2[3] == 0x03A0);
	ASSERT(str2[4] == 0x0544);
	ASSERT(str2[5] == 0x0532);

	str2 = ConvertToLowerCase(str);
	ASSERT(str2.length() == str.length());
	ASSERT(str2[0] == 0x00E1);
	ASSERT(str2[1] == 0x00EA);
	ASSERT(str2[2] == 0x03B4);
	ASSERT(str2[3] == 0x03C0);
	ASSERT(str2[4] == 0x0574);
	ASSERT(str2[5] == 0x0562);
}


//---------------------------------------------------------------
//
// XStringUtilsUnitTest::DoCompareTest
//
//---------------------------------------------------------------
void XStringUtilsUnitTest::DoCompareTest()
{
	ASSERT(CompareStrings(L"Albert", L"Bob") < 0);		
	ASSERT(CompareStrings(L"Bob", L"Albert") > 0);							
	ASSERT(CompareStrings(L"Albert", L"Albert") == 0);							
	ASSERT(CompareStrings(L"Albert", L"ALbert") != 0);							
	ASSERT(CompareStrings(L"Bob", L"Bobby") < 0);							
}

#endif	// DEBUG


}	// namespace Whisper


