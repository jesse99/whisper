/*
 *  File:       XStringTest.h
 *  Summary:   	XString unit test.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XStringTest.cpp,v $
 *		Revision 1.2  2000/11/09 12:53:44  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/02/00	JDJ		Created
 */
 
#include <XWhisperHeader.h>
#include <XStringTest.h>		

#include <XTextTranscoders.h>		

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class XStringUnitTest
// ===================================================================================	

//---------------------------------------------------------------
//
// XStringUnitTest::~XStringUnitTest
//
//---------------------------------------------------------------
XStringUnitTest::~XStringUnitTest()
{
}

	
//---------------------------------------------------------------
//
// XStringUnitTest::XStringUnitTest
//
//---------------------------------------------------------------
XStringUnitTest::XStringUnitTest() : XUnitTest(L"Backend", L"XString")
{
}

						
//---------------------------------------------------------------
//
// XStringUnitTest::OnTest
//
//---------------------------------------------------------------
void XStringUnitTest::OnTest()
{
	this->DoReserveTest();
	this->DoTestAssignment();
	this->DoTestAppend();
	this->DoTestInsert();
	this->DoTestRemove();
	this->DoTestReplace();
	this->DoTestAccess();
	this->DoTestFind();
	this->DoTestCompare();
	this->DoTestTranscoders();

	TRACE("Completed XString test.\n\n");
}


//---------------------------------------------------------------
//
// XStringUnitTest::DoReserveTest
//
// From Plum Hall
//
//---------------------------------------------------------------
void XStringUnitTest::DoReserveTest()
{
	mStr1.resize(0);
	mStr2.resize(0);
	mStr3.resize(0);

	mStr4 = L"s4";
	mStr5 = XString("s5xxx", 2);
	mStr6 = XString(3, 'a');
	mStr7 = XString(5, 'b');
	mStr8 = XString(1, 'c');
	
	mStr1.reserve(40);
	ASSERT(mStr1.capacity() == 40 && mStr2.capacity() == 32);		
	ASSERT(std::std::strcmp(mStr1.c_str(), "") == 0);
	ASSERT(std::strcmp(mStr2.c_str(), "") == 0);
	ASSERT(std::strcmp(mStr4.c_str(), "s4") == 0);
	ASSERT(std::strcmp(mStr5.c_str(), "s5") == 0);
	ASSERT(std::strcmp(mStr6.c_str(), "aaa") == 0);
	ASSERT(std::strcmp(mStr7.c_str(), "bbbbb") == 0);
	ASSERT(std::strcmp(mStr8.c_str(), "c") == 0);
}


//---------------------------------------------------------------
//
// XStringUnitTest::DoTestAssignment
//
// From Plum Hall
//
//---------------------------------------------------------------
void XStringUnitTest::DoTestAssignment()
{
	mStr1 = L"hello"; ASSERT(std::strcmp(mStr1.c_str(), "hello") == 0);
	mStr1 = 'x'; ASSERT(std::strcmp(mStr1.c_str(), "x") == 0);
	mStr1 = mStr4; ASSERT(std::strcmp(mStr1.c_str(), "s4") == 0);
	mStr1.assign("AB"); ASSERT(std::strcmp(mStr1.c_str(), "AB") == 0);
	mStr1.assign(1, 'C'); ASSERT(std::strcmp(mStr1.c_str(), "C") == 0);
	mStr1.assign(mStr4); ASSERT(std::strcmp(mStr1.c_str(), "s4") == 0);
}


//---------------------------------------------------------------
//
// XStringUnitTest::DoTestAppend
//
// From Plum Hall
//
//---------------------------------------------------------------
void XStringUnitTest::DoTestAppend()
{
	mStr1 += L"abc"; ASSERT(std::strcmp(mStr1.c_str(), "s4abc") == 0);
	mStr1 += 'd'; ASSERT(std::strcmp(mStr1.c_str(), "s4abcd") == 0);
	mStr1 += mStr4; ASSERT(std::strcmp(mStr1.c_str(), "s4abcds4") == 0);
	mStr1 = L"A"; mStr1.append("BC");
	ASSERT(std::strcmp(mStr1.c_str(), "ABC") == 0);
	mStr1.append(1, 'D'); ASSERT(std::strcmp(mStr1.c_str(), "ABCD") == 0);
	mStr1.append(mStr4); ASSERT(std::strcmp(mStr1.c_str(), "ABCDs4") == 0);
	ASSERT(std::strcmp((mStr4 + mStr5).c_str(), "s4s5") == 0);
	ASSERT(std::strcmp((mStr4 + L"s5").c_str(), "s4s5") == 0);
	ASSERT(std::strcmp((L"s4" + mStr5).c_str(), "s4s5") == 0);
	ASSERT(std::strcmp((mStr4 + '5').c_str(), "s45") == 0);
	ASSERT(std::strcmp(('4' + mStr5).c_str(), "4s5") == 0);
}


//---------------------------------------------------------------
//
// XStringUnitTest::DoTestInsert
//
// From Plum Hall
//
//---------------------------------------------------------------
void XStringUnitTest::DoTestInsert()
{
	mStr1 = L"abc";
	mStr1.insert(3, L"Dd").insert(1, L"BC", 1).insert(0, L"A");
	ASSERT(std::strcmp(mStr1.c_str(), "AaBbcDd") == 0);
	mStr1.insert(7, 2, 'E').insert(4, 1, 'C');
	ASSERT(std::strcmp(mStr1.c_str(), "AaBbCcDdEE") == 0);
	mStr1.insert(10, mStr4).insert(0, mStr4, 1).insert(0, mStr4, 0, 1);
	ASSERT(std::strcmp(mStr1.c_str(), "s4AaBbCcDdEEs4") == 0);
}


//---------------------------------------------------------------
//
// XStringUnitTest::DoTestRemove
//
// From Plum Hall
//
//---------------------------------------------------------------
void XStringUnitTest::DoTestRemove()
{
	mStr1.erase(0, 2);
	ASSERT(std::strcmp(mStr1.c_str(), "AaBbCcDdEEs4") == 0);
	mStr1.erase(8); ASSERT(std::strcmp(mStr1.c_str(), "AaBbCcDd") == 0);
	mStr1.erase(); ASSERT(std::strcmp(mStr1.c_str(), "") == 0);
}


//---------------------------------------------------------------
//
// XStringUnitTest::DoTestReplace
//
// From Plum Hall
//
//---------------------------------------------------------------
void XStringUnitTest::DoTestReplace()
{
	mStr1.replace(0, 0, L"123ab789");
	mStr1.replace(3, 2, L"45678", 3);
	ASSERT(std::strcmp(mStr1.c_str(), "123456789") == 0);
	mStr1.replace(1, 3, 2, 'x').replace(0, 0, 1, '0');
	ASSERT(std::strcmp(mStr1.c_str(), "01xx56789") == 0);
	mStr1.replace(3, 1, mStr4, 1).replace(2, 1, mStr4);
	ASSERT(std::strcmp(mStr1.c_str(), "01s4456789") == 0);
}


//---------------------------------------------------------------
//
// XStringUnitTest::DoTestAccess
//
// From Plum Hall
//
//---------------------------------------------------------------
void XStringUnitTest::DoTestAccess()
{
	mStr1.at(2) = '2';
	ASSERT(mStr1.at(1) == '1' && mStr1[2] == '2');
	ASSERT((mStr1[3] = '3') == '3'); ASSERT(mStr1.length() == 10);
	mStr1.resize(3); ASSERT(std::strcmp(mStr1.c_str(), "012") == 0);
	mStr1.resize(5, 'X'); ASSERT(std::strcmp(mStr1.c_str(), "012XX") == 0);
	mStr1.resize(6); ASSERT(std::memcmp(mStr1.c_str(), "012XX\0", 7) == 0);
	char buf[10];
	ASSERT(mStr1.copy(buf, sizeof (buf)) == 6
		&& std::strcmp(buf, "012XX") == 0);
	ASSERT(mStr1.copy(buf, 3, 1) == 3
		&& std::memcmp(buf, "12X", 3) == 0);
}


//---------------------------------------------------------------
//
// XStringUnitTest::DoTestFind
//
// From Plum Hall
//
//---------------------------------------------------------------
void XStringUnitTest::DoTestFind()
{
	mStr1 = L"s4s4";
	ASSERT(mStr1.find(mStr4) == 0 && mStr1.find(mStr4, 1) == 2
		&& mStr1.find(mStr4, 3) == XString::npos);
	ASSERT(mStr1.find(L"s4") == 0 && mStr1.find(L"s4", 3) == XString::npos
		&& mStr1.find(L"s4XX", 1, 2) == 2);
	ASSERT(mStr1.find('s') == 0 && mStr1.find('s', 1) == 2
		&& mStr1.find('x') == XString::npos);
	ASSERT(mStr1.rfind(mStr4) == 2 && mStr1.rfind(mStr4, 1) == 0
		&& mStr1.rfind(mStr5, 3) == XString::npos);
	ASSERT(mStr1.rfind(L"s4") == 2 && mStr1.rfind(L"s4", 3) == 2
		&& mStr1.rfind(L"s4XX", 1, 3) == XString::npos);
	ASSERT(mStr1.rfind('s') == 2 && mStr1.rfind('s', 2) == 2
		&& mStr1.rfind('x') == XString::npos);
	ASSERT(mStr1.find_first_of(mStr4) == 0
		&& mStr1.find_first_of(mStr4, 1) == 1
		&& mStr1.find_first_of(mStr4, 4) == XString::npos);
	ASSERT(mStr1.find_first_of(L"s4") == 0
		&& mStr1.find_first_of(L"s4", 3) == 3
		&& mStr1.find_first_of(L"abs", 1, 2) == XString::npos);
	ASSERT(mStr1.find_first_of('s') == 0
		&& mStr1.find_first_of('s', 1) == 2
		&& mStr1.find_first_of('x') == XString::npos);
	ASSERT(mStr1.find_last_of(mStr4) == 3
		&& mStr1.find_last_of(mStr4, 1) == 1
		&& mStr1.find_last_of(mStr6) == XString::npos);
	ASSERT(mStr1.find_last_of(L"s4") == 3
		&& mStr1.find_last_of(L"s4", 2) == 2
		&& mStr1.find_last_of(L"abs", 1, 2) == XString::npos);
	ASSERT(mStr1.find_last_of('s') == 2
		&& mStr1.find_last_of('s', 1) == 0
		&& mStr1.find_last_of('x') == XString::npos);
	ASSERT(mStr1.find_first_not_of(mStr5) == 1
		&& mStr1.find_first_not_of(mStr5, 2) == 3
		&& mStr1.find_first_not_of(mStr4) == XString::npos);
	ASSERT(mStr1.find_first_not_of(L"s5") == 1
		&& mStr1.find_first_not_of(L"s5", 2) == 3
		&& mStr1.find_first_not_of(L"s4a", 1, 2) == XString::npos);
	ASSERT(mStr1.find_first_not_of('s') == 1
		&& mStr1.find_first_not_of('s', 2) == 3
		&& mStr1.find_first_not_of('s', 4) == XString::npos);
	ASSERT(mStr1.find_last_not_of(mStr5) == 3
		&& mStr1.find_last_not_of(mStr5, 2) == 1
		&& mStr1.find_last_not_of(mStr4) == XString::npos);
	ASSERT(mStr1.find_last_not_of(L"s5") == 3
		&& mStr1.find_last_not_of(L"s5", 2) == 1
		&& mStr1.find_last_not_of(L"s4a", 1, 2) == XString::npos);
	ASSERT(mStr1.find_last_not_of('s') == 3
		&& mStr1.find_last_not_of('s', 2) == 1
		&& mStr1.find_last_not_of('s', 0) == XString::npos);
}


//---------------------------------------------------------------
//
// XStringUnitTest::DoTestCompare
//
//---------------------------------------------------------------
void XStringUnitTest::DoTestCompare()
{
	// From Plum Hall
	ASSERT(mStr1.compare(mStr1) == 0 && mStr1.compare(2, 100, mStr4) == 0		
		&& mStr1.compare(2, 2, mStr4) == 0);								
	ASSERT(mStr1.compare(L"s4s4") == 0 && mStr1.compare(2, 100, L"s4") == 0
		&& mStr1.compare(2, 2, L"s4xx") != 0);
	ASSERT(mStr8 == 'c');
	ASSERT(mStr1.compare(mStr4) != 0 && mStr1.compare(1, 100, mStr1) != 0
		&& mStr1.compare(0, 2, mStr4) == 0);
	ASSERT(mStr1.compare(L"s4s5") != 0 && mStr1.compare(3, 100, L"s44") != 0
		&& mStr1.compare(0, 2, L"s4xx") != 0);
	ASSERT(mStr1 != 'c');
	ASSERT(mStr1 == mStr1 && mStr1 == L"s4s4" && mStr8 == 'c' && L"s4" == mStr4
		&& 'c' == mStr8);
	ASSERT(mStr1 != mStr4 && mStr1 != L"xx" && mStr1 != 's' && L"s4" != mStr1
		&& 'a' != mStr6);
	
	// Stuff I added
	mStr1 = L"hello";
	mStr2 = L"hello";
	mStr3 = L"goodbye";
	mStr4 = L"guten tag";
	mStr5 = L"good";
	
	ASSERT(mStr1 == mStr1);
	ASSERT(mStr1 <= mStr1);
	ASSERT(mStr1 >= mStr1);
	ASSERT(!(mStr1 < mStr1));
	ASSERT(!(mStr1 > mStr1));

	ASSERT(mStr1 == mStr2);
	ASSERT(mStr1 <= mStr2);
	ASSERT(mStr1 >= mStr2);
	ASSERT(!(mStr1 < mStr2));
	ASSERT(!(mStr1 > mStr2));

	ASSERT(mStr1 != mStr3);
	ASSERT(mStr1 >= mStr3);
	ASSERT(!(mStr1 <= mStr3));
	ASSERT(mStr1 > mStr3);
	ASSERT(!(mStr1 < mStr3));

	ASSERT(mStr4 != mStr3);
	ASSERT(mStr4 >= mStr3);
	ASSERT(!(mStr4 <= mStr3));
	ASSERT(mStr4 > mStr3);
	ASSERT(!(mStr4 < mStr3));

	ASSERT(!(mStr1 == mStr5));
	ASSERT(!(mStr5 == mStr1));
	ASSERT(mStr1 != mStr5);
	ASSERT(mStr5 != mStr1);
}


//---------------------------------------------------------------
//
// XStringUnitTest::DoTestTranscoders
//
//---------------------------------------------------------------
void XStringUnitTest::DoTestTranscoders()
{
	const char* asciiStr = "four score and seven years ago";
	const char* macStr = "pi (�) infinity (�) bullet (�) TM (�) elipsis (�) check (�) omega (�)";

	const char* winStr = "copyright (�) plus/minus (�) beta (�)";
	
	XDisableUTF16Warning disabler;

	mStr1.SetTranscoder(new XMacRomanTranscoder);
	mStr1.assign(macStr);
	ASSERT(std::strcmp(mStr1.c_str(), macStr) == 0);

	mStr1.SetTranscoder(new XASCIITranscoder);
	mStr1.assign(asciiStr);
	ASSERT(std::strcmp(mStr1.c_str(), asciiStr) == 0);
	
	mStr1.assign(macStr);
	ASSERT(std::strcmp(mStr1.c_str(), macStr) != 0);
	
	mStr1.SetTranscoder(new XWindowsLatin1Transcoder);

	mStr1.assign(winStr);

	ASSERT(std::strcmp(mStr1.c_str(), winStr) == 0);
}

#endif	// DEBUG		


}	// namespace Whisper
