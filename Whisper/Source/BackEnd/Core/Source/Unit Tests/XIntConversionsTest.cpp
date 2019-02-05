/*
 *  File:       XIntConversionsTest.h
 *  Summary:   	Unit test for the int to text functions.
 *	Written by:	Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: XIntConversionsTest.cpp,v $
 *		Revision 1.2  2000/11/09 12:39:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 1/01/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XIntConversionsTest.h>

#include <stdexcept>

#include <XDebug.h>
#include <XIntConversions.h>

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class XIntConvUnitTest
// ===================================================================================	

//---------------------------------------------------------------
//
// XIntConvUnitTest::~XIntConvUnitTest
//
//---------------------------------------------------------------
XIntConvUnitTest::~XIntConvUnitTest()
{
}

	
//---------------------------------------------------------------
//
// XIntConvUnitTest::XIntConvUnitTest
//
//---------------------------------------------------------------
XIntConvUnitTest::XIntConvUnitTest() : XUnitTest(L"Backend", L"Int Conversions")
{
}

						
//---------------------------------------------------------------
//
// XIntConvUnitTest::OnTest
//
//---------------------------------------------------------------
void XIntConvUnitTest::OnTest()
{
	this->DoStrToIntTest();
	this->DoStrToUIntTest();
	this->DoBadStrToIntTest();
	
	this->DoIntToStrTest();
	this->DoUIntToStrTest();
	
	this->DoIntToHexTest();
	this->DoUIntToHexTest();
	
	this->DoIntToBinaryTest();
	this->DoUIntToBinaryTest();

	TRACE("Completed int conversions test.\n\n");
}


//---------------------------------------------------------------
//
// XIntConvUnitTest::DoStrToIntTest
//
//---------------------------------------------------------------
void XIntConvUnitTest::DoStrToIntTest()
{
	ASSERT(StrToInt8(L"0") == 0);							// StrToInt8
	ASSERT(StrToInt8(L"-11") == -11);
	ASSERT(StrToInt8(L"23") == 23);
	ASSERT(StrToInt8(L"-128") == -128);
	ASSERT(StrToInt8(L"127") == 127);
	
	ASSERT(StrToInt16(L"0") == 0);							// StrToInt16
	ASSERT(StrToInt16(L"-100") == -100);
	ASSERT(StrToInt16(L"2323") == 2323);
	ASSERT(StrToInt16(L"-32768") == -32768);
	ASSERT(StrToInt16(L"32767") == 32767);
	
	ASSERT(StrToInt32(L"000") == 0);						// StrToInt32
	ASSERT(StrToInt32(L"  -1") == -1);
	ASSERT(StrToInt32(L"00023") == 23);
	ASSERT(StrToInt32(L"  23   ") == 23);
	ASSERT(StrToInt32(L"**23", '*') == 23);
	ASSERT(StrToInt32(L"232300") == 232300L);
	ASSERT(StrToInt32(L"-2147483648") == -2147483647L-1);
	ASSERT(StrToInt32(L"2147483647") == 2147483647L);
	ASSERT(StrToInt32(L"$ff") == 255);		
	ASSERT(StrToInt32(L"$0FE") == 254);		
	ASSERT(StrToInt32(L"%1010") == 10);		
}


//---------------------------------------------------------------
//
// XIntConvUnitTest::DoStrToUIntTest
//
//---------------------------------------------------------------
void XIntConvUnitTest::DoStrToUIntTest()
{
	ASSERT(StrToUInt8(L"0") == 0);							// StrToUInt8
	ASSERT(StrToUInt8(L"23") == 23);
	ASSERT(StrToUInt8(L"127") == 127);
	ASSERT(StrToUInt8(L"255") == 255);
	
	ASSERT(StrToUInt16(L"0") == 0);							// StrToUInt16
	ASSERT(StrToUInt16(L"2323") == 2323);
	ASSERT(StrToUInt16(L"32767") == 32767);
	ASSERT(StrToUInt16(L"65535") == 65535U);
	
	ASSERT(StrToUInt32(L"000") == 0);						// StrToUInt32
	ASSERT(StrToUInt32(L"00023") == 23);
	ASSERT(StrToUInt32(L"  23   ") == 23);
	ASSERT(StrToUInt32(L"**23", '*') == 23);
	ASSERT(StrToUInt32(L"232300") == 232300U);
	ASSERT(StrToUInt32(L"2147483647") == 2147483647U);
	ASSERT(StrToUInt32(L"4294967295") == 4294967295U);
	ASSERT(StrToUInt32(L"$ff") == 255);		
	ASSERT(StrToUInt32(L"$0FE") == 254);		
	ASSERT(StrToUInt32(L"%1010") == 10);		
}


//---------------------------------------------------------------
//
// XIntConvUnitTest::DoBadStrToIntTest
//
//---------------------------------------------------------------
void XIntConvUnitTest::DoBadStrToIntTest()
{
	TRACE("Will throw 4 exceptions.\n");				

	try {
		(void) StrToInt32(L"alpha");										
		ASSERT(false);
	} catch (const std::invalid_argument&) {
	} catch (...) {
		ASSERT(false);
	}
	
	try {
		(void) StrToInt32(L"12345657321344");								
		ASSERT(false);
	} catch (const std::range_error&) {
	} catch (...) {
		ASSERT(false);
	}
	
	try {
		(void) StrToUInt32(L"12345657321344");								
		ASSERT(false);
	} catch (const std::range_error&) {
	} catch (...) {
		ASSERT(false);
	}
	
	try {
		(void) StrToInt32(L"-12345657321344");								
		ASSERT(false);
	} catch (const std::range_error&) {
	} catch (...) {
		ASSERT(false);
	}
}


//---------------------------------------------------------------
//
// XIntConvUnitTest::DoIntToStrTest
//
//---------------------------------------------------------------
void XIntConvUnitTest::DoIntToStrTest()
{
	ASSERT(Int8ToStr(0) == L"0");							// Int8ToStr
	ASSERT(Int8ToStr(-11) == L"-11");	
	ASSERT(Int8ToStr(23) == L"23");	
	ASSERT(Int8ToStr(-128) == L"-128");	
	ASSERT(Int8ToStr(127) == L"127");	

	ASSERT(Int16ToStr(0) == L"0");							// Int16ToStr
	ASSERT(Int16ToStr(-11) == L"-11");	
	ASSERT(Int16ToStr(23) == L"23");	
	ASSERT(Int16ToStr(-32768) == L"-32768");	
	ASSERT(Int16ToStr(32767) == L"32767");	

	ASSERT(Int32ToStr(0) == L"0");							// Int32ToStr
	ASSERT(Int32ToStr(-11) == L"-11");	
	ASSERT(Int32ToStr(23) == L"23");	
	ASSERT(Int32ToStr(-2147483648L) == L"-2147483648");	
	ASSERT(Int32ToStr(2147483647L) == L"2147483647");	
	ASSERT(Int32ToStr(23, 4) == L"  23");	
	ASSERT(Int32ToStr(-100, 6) == L"  -100");	
	ASSERT(Int32ToStr(23, 3, 'z') == L"z23");	
	ASSERT(Int32ToStr(230, 2, 'z') == L"**");	
}


//---------------------------------------------------------------
//
// XIntConvUnitTest::DoUIntToStrTest
//
//---------------------------------------------------------------
void XIntConvUnitTest::DoUIntToStrTest()
{
	ASSERT(UInt8ToStr(0) == L"0");							// UInt8ToStr
	ASSERT(UInt8ToStr(23) == L"23");	
	ASSERT(UInt8ToStr(127) == L"127");	
	ASSERT(UInt8ToStr(255) == L"255");	

	ASSERT(UInt16ToStr(0) == L"0");							// UInt16ToStr
	ASSERT(UInt16ToStr(23) == L"23");	
	ASSERT(UInt16ToStr(32767) == L"32767");	
	ASSERT(UInt16ToStr(65535U) == L"65535");	

	ASSERT(UInt32ToStr(0) == L"0");							// UInt32ToStr
	ASSERT(UInt32ToStr(23) == L"23");	
	ASSERT(UInt32ToStr(2147483647L) == L"2147483647");	
	ASSERT(UInt32ToStr(4294967295U) == L"4294967295");	
	ASSERT(UInt32ToStr(23, 4) == L"  23");	
	ASSERT(UInt32ToStr(23, 3, 'z') == L"z23");	
	ASSERT(UInt32ToStr(230, 2, 'z') == L"**");	
}


//---------------------------------------------------------------
//
// XIntConvUnitTest::DoIntToHexTest
//
//---------------------------------------------------------------
void XIntConvUnitTest::DoIntToHexTest()
{
	ASSERT(Int8ToHex(0) == L"00");							// Int8ToHex
	ASSERT(Int8ToHex(-2) == L"FE");	
	ASSERT(Int8ToHex(20) == L"14");	
	ASSERT(Int8ToHex(26) == L"1A");	
	ASSERT(Int8ToHex(-128) == L"80");	
	ASSERT(Int8ToHex(127) == L"7F");	

	ASSERT(Int16ToHex(0) == L"0000");						// Int16ToHex
	ASSERT(Int16ToHex(-2) == L"FFFE");	
	ASSERT(Int16ToHex(20) == L"0014");	
	ASSERT(Int16ToHex(26) == L"001A");	
	ASSERT(Int16ToHex(-32768) == L"8000");	
	ASSERT(Int16ToHex(32767) == L"7FFF");	

	ASSERT(Int32ToHex(0) == L"00000000");					// Int32ToHex
	ASSERT(Int32ToHex(-3) == L"FFFFFFFD");	
	ASSERT(Int32ToHex(20) == L"00000014");	
	ASSERT(Int32ToHex(-2147483647L-1) == L"80000000");	
	ASSERT(Int32ToHex(2147483647L) == L"7FFFFFFF");	
}


//---------------------------------------------------------------
//
// XIntConvUnitTest::DoUIntToHexTest
//
//---------------------------------------------------------------
void XIntConvUnitTest::DoUIntToHexTest()
{
	ASSERT(UInt8ToHex(0) == L"00");							// UInt8ToHex
	ASSERT(UInt8ToHex(20) == L"14");	
	ASSERT(UInt8ToHex(26) == L"1A");	
	ASSERT(UInt8ToHex(127) == L"7F");	
	ASSERT(UInt8ToHex(255) == L"FF");	

	ASSERT(UInt16ToHex(0) == L"0000");						// UInt16ToHex
	ASSERT(UInt16ToHex(20) == L"0014");	
	ASSERT(UInt16ToHex(26) == L"001A");	
	ASSERT(UInt16ToHex(32767) == L"7FFF");	
	ASSERT(UInt16ToHex(65535U) == L"FFFF");	

	ASSERT(UInt32ToHex(0) == L"00000000");					// UInt32ToHex
	ASSERT(UInt32ToHex(20) == L"00000014");	
	ASSERT(UInt32ToHex(2147483647L) == L"7FFFFFFF");	
	ASSERT(UInt32ToHex(4294967295U) == L"FFFFFFFF");	
}


//---------------------------------------------------------------
//
// XIntConvUnitTest::DoIntToBinaryTest
//
//---------------------------------------------------------------
void XIntConvUnitTest::DoIntToBinaryTest()
{
	ASSERT(Int8ToBinary(0) ==    L"00000000");				// Int8ToBinary
	ASSERT(Int8ToBinary(-2) ==   L"11111110");	
	ASSERT(Int8ToBinary(20) ==   L"00010100");	
	ASSERT(Int8ToBinary(26) ==   L"00011010");	
	ASSERT(Int8ToBinary(-128) == L"10000000");	
	ASSERT(Int8ToBinary(127) ==  L"01111111");	

	ASSERT(Int16ToBinary(0) ==      L"0000000000000000");						// Int16ToBinary
	ASSERT(Int16ToBinary(-2) ==     L"1111111111111110");	
	ASSERT(Int16ToBinary(20) ==     L"0000000000010100");	
	ASSERT(Int16ToBinary(26) ==     L"0000000000011010");	
	ASSERT(Int16ToBinary(-32768) == L"1000000000000000");	
	ASSERT(Int16ToBinary(32767) ==  L"0111111111111111");	

	ASSERT(Int32ToBinary(0) ==              L"00000000000000000000000000000000");// Int32ToBinary
	ASSERT(Int32ToBinary(-3) ==             L"11111111111111111111111111111101");	
	ASSERT(Int32ToBinary(20) ==             L"00000000000000000000000000010100");	
	ASSERT(Int32ToBinary(-2147483647L-1) == L"10000000000000000000000000000000");	
	ASSERT(Int32ToBinary(2147483647L) ==    L"01111111111111111111111111111111");	
}


//---------------------------------------------------------------
//
// XIntConvUnitTest::DoUIntToBinaryTest
//
//---------------------------------------------------------------
void XIntConvUnitTest::DoUIntToBinaryTest()
{
	ASSERT(UInt8ToBinary(0) ==   L"00000000");									// UInt8ToBinary
	ASSERT(UInt8ToBinary(20) ==  L"00010100");	
	ASSERT(UInt8ToBinary(26) ==  L"00011010");	
	ASSERT(UInt8ToBinary(127) == L"01111111");	
	ASSERT(UInt8ToBinary(255) == L"11111111");	

	ASSERT(UInt16ToBinary(0) ==      L"0000000000000000");						// UInt16ToBinary
	ASSERT(UInt16ToBinary(20) ==     L"0000000000010100");	
	ASSERT(UInt16ToBinary(26) ==     L"0000000000011010");	
	ASSERT(UInt16ToBinary(32767) ==  L"0111111111111111");	
	ASSERT(UInt16ToBinary(65535U) == L"1111111111111111");	

	ASSERT(UInt32ToBinary(0) ==           L"00000000000000000000000000000000");	// UInt32ToBinary
	ASSERT(UInt32ToBinary(20) ==          L"00000000000000000000000000010100");	
	ASSERT(UInt32ToBinary(2147483647L) == L"01111111111111111111111111111111");	
	ASSERT(UInt32ToBinary(4294967295U) == L"11111111111111111111111111111111");	
}

#endif	// DEBUG


}	// namespace Whisper

