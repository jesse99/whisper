/*
 *  File:       XFloatConversionsTest.h
 *  Summary:   	Unit test for the floating point to text functions.
 *	Written by:	Jesse Jones
 *
 *  Copyright � 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):	
 *
 *		$Log: XFloatConversionsTest.cpp,v $
 *		Revision 1.2  2000/11/09 12:36:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <3>	10/31/00	JDJ		No longer hard codes the decimal point char.
 *		 <2>	 7/31/00	JDJ		Added DoDoubleToFormStr test.
 *		 <1>	 1/11/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XFloatConversionsTest.h>

#include <stdexcept>

#include <XFloatConversions.h>
#include <XNumbers.h>
#include <XStringUtils.h>

namespace Whisper {
#if DEBUG

// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CheckResult
//
//---------------------------------------------------------------
static void CheckResult(const std::wstring& result, const std::wstring& expected)
{
	std::wstring temp = Replace(expected, '.', GetDecimalPoint());
	ASSERT(result == temp);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XFloatConvUnitTest
// ===================================================================================

//---------------------------------------------------------------
//
// XFloatConvUnitTest::~XFloatConvUnitTest
//
//---------------------------------------------------------------
XFloatConvUnitTest::~XFloatConvUnitTest()
{
}

	
//---------------------------------------------------------------
//
// XFloatConvUnitTest::XFloatConvUnitTest
//
//---------------------------------------------------------------
XFloatConvUnitTest::XFloatConvUnitTest() : XUnitTest(L"Backend", L"Float Conversions")
{
}

						
//---------------------------------------------------------------
//
// XFloatConvUnitTest::OnTest
//
//---------------------------------------------------------------
void XFloatConvUnitTest::OnTest()
{
	this->DoStrToDoubleTest();
	this->DoDoubleToStrTest();
	this->DoDoubleToSciStrTest();
	this->DoDoubleToFormStr();
	
	TRACE("Completed float conversions test.\n\n");
}


//---------------------------------------------------------------
//
// XFloatConvUnitTest::DoStrToDoubleTest
//
//---------------------------------------------------------------
void XFloatConvUnitTest::DoStrToDoubleTest()
{
	double result;

	result = StrToDouble(L"2.5");		ASSERT(Equal(result, 2.5));
	result = StrToDouble(L" 1.9");		ASSERT(Equal(result, 1.9));
	result = StrToDouble(L" -234");		ASSERT(Equal(result, -234.0));
	result = StrToDouble(L"-0.0");		ASSERT(Equal(result, 0.0));
	result = StrToDouble(L"0.0");		ASSERT(Equal(result, 0.0));
	result = StrToDouble(L"-.8  ");		ASSERT(Equal(result, -0.8));
	result = StrToDouble(L"###.01", '#');ASSERT(Equal(result, 0.01));
	result = StrToDouble(L"2.3E4");		ASSERT(Equal(result, 2.3e4));
	result = StrToDouble(L"5.6e-9");	ASSERT(Equal(result, 5.6e-9));
	
	result = StrToDouble(L" nan");		ASSERT(isnan(result)); 
	result = StrToDouble(L" +NaN");		ASSERT(isnan(result)); 
	result = StrToDouble(L" inf");		ASSERT(result == INFINITY);
	result = StrToDouble(L"-iNf");		ASSERT(result == -INFINITY);

	TRACE("Will throw 3 exceptions.\n");
	
	try {
		result = StrToDouble(L"alpha");
		ASSERT(false);
	} catch (const std::invalid_argument&) {
	} catch (...) {
		ASSERT(false);
	}

	try {
		result = StrToDouble(L"12.k9");
		ASSERT(false);
	} catch (const std::invalid_argument&) {
	} catch (...) {
		ASSERT(false);
	}
	
	try {
		result = StrToDouble(L"12.9  8");
		ASSERT(false);
	} catch (const std::invalid_argument&) {
	} catch (...) {
		ASSERT(false);
	}

	result = StrToDouble(L"1e9999999");
	ASSERT(result == INFINITY);
	
	result = StrToDouble(L"-1e9999999");
	ASSERT(result == -INFINITY);
}


//---------------------------------------------------------------
//
// XFloatConvUnitTest::DoDoubleToStrTest
//
//---------------------------------------------------------------
void XFloatConvUnitTest::DoDoubleToStrTest()
{
	std::wstring result;

	const double pi2 = 314.1592653589793238;
	const double pi3 = 3141.592653589793238;

	result = DoubleToStr(kPi, 4);	CheckResult(result, L" 3.1");		
	result = DoubleToStr(kPi, 5);	CheckResult(result, L" 3.14");		
	result = DoubleToStr(kPi, 6);	CheckResult(result, L" 3.142");		
	result = DoubleToStr(kPi, 7);	CheckResult(result, L" 3.1416");		
	result = DoubleToStr(kPi, 8);	CheckResult(result, L" 3.14159");	
	result = DoubleToStr(kPi, 9);	CheckResult(result, L" 3.141593");	
	result = DoubleToStr(-kPi, 9);	CheckResult(result, L"-3.141593");	
	
	result = DoubleToStr(pi3, 6);	CheckResult(result, L" 3142.");		
	result = DoubleToStr(-pi3, 6);	CheckResult(result, L"-3142.");		
	result = DoubleToStr(pi2, 6);	CheckResult(result, L" 314.2");		
	result = DoubleToStr(pi2, 7);	CheckResult(result, L" 314.16");		
	result = DoubleToStr(pi2, 8);	CheckResult(result, L" 314.159");	
	
	result = DoubleToStr(kPi, 8, 2);	CheckResult(result, L"    3.14");	
	result = DoubleToStr(kPi, 9, 3);	CheckResult(result, L"    3.142");	
	result = DoubleToStr(-kPi, 9, 4);	CheckResult(result, L"  -3.1416");	

	result = DoubleToStr(NAN, 8);	CheckResult(result, L"     NAN");	
//	result = DoubleToStr(0.0*INFINITY, 9);CheckResult(result, L"      NAN");	
	result = DoubleToStr(INFINITY, 4);	CheckResult(result, L" INF");		
	result = DoubleToStr(-INFINITY, 4);	CheckResult(result, L"-INF");		

	result = DoubleToStr(kPi, 3);	CheckResult(result, L" 3.");				
	result = DoubleToStr(kPi, 2);	CheckResult(result, L"**");				
	result = DoubleToStr(kPi, 1);	CheckResult(result, L"*");				
}


//---------------------------------------------------------------
//
// XFloatConvUnitTest::DoDoubleToSciStrTest
//
//---------------------------------------------------------------
void XFloatConvUnitTest::DoDoubleToSciStrTest()
{
	std::wstring result;

	const double pi2 = 314.1592653589793238;
	const double pi3 = 3141.592653589793238;

#if __MWERKS__
	result = DoubleToSciStr(pi2, 7);	CheckResult(result, L" 3.e+02");		
	result = DoubleToSciStr(pi2, 9);	CheckResult(result, L" 3.14e+02");	
	result = DoubleToSciStr(pi2, 9, 2);	CheckResult(result, L"  3.1e+02");	//	$$$ check 0 decPlaces
#else
	result = DoubleToSciStr(pi2, 7);	CheckResult(result, L"*******");		
	result = DoubleToSciStr(pi2, 9);	CheckResult(result, L" 3.1e+002");	
	result = DoubleToSciStr(pi2, 9, 2);CheckResult(result, L" 3.1e+002");	//	$$$ check 0 decPlaces
#endif
}

#endif	// DEBUG


//---------------------------------------------------------------
//
// XFloatConvUnitTest::DoDoubleToFormStr
//
//---------------------------------------------------------------
void XFloatConvUnitTest::DoDoubleToFormStr()
{
	std::wstring result;

	const double pi2 = 314.1592653589793238;
	const double pi3 = 3141.592653589793238;

	result = DoubleToFormStr(kPi, 4);	CheckResult(result, L" 3.1");		
	result = DoubleToFormStr(kPi, 5);	CheckResult(result, L" 3.14");		
	result = DoubleToFormStr(kPi, 6);	CheckResult(result, L" 3.142");		
	result = DoubleToFormStr(kPi, 7);	CheckResult(result, L" 3.1416");		
	result = DoubleToFormStr(kPi, 8);	CheckResult(result, L" 3.14159");	
	result = DoubleToFormStr(kPi, 9);	CheckResult(result, L" 3.141593");	
	result = DoubleToFormStr(-kPi, 9);	CheckResult(result, L"-3.141593");	
	
	result = DoubleToFormStr(pi3, 6);	CheckResult(result, L" 3142.");		
	result = DoubleToFormStr(-pi3, 6);	CheckResult(result, L"-3142.");		
	result = DoubleToFormStr(pi2, 6);	CheckResult(result, L" 314.2");		
	result = DoubleToFormStr(pi2, 7);	CheckResult(result, L" 314.16");		
	result = DoubleToFormStr(pi2, 8);	CheckResult(result, L" 314.159");	
	
	result = DoubleToFormStr(kPi, 8, 2);	CheckResult(result, L"    3.14");	
	result = DoubleToFormStr(kPi, 9, 3);	CheckResult(result, L"    3.142");	
	result = DoubleToFormStr(-kPi, 9, 4);	CheckResult(result, L"  -3.1416");	
	result = DoubleToFormStr(kPi, 8, 1);	CheckResult(result, L"     3.1");	
	result = DoubleToFormStr(kPi, 8, 0);	CheckResult(result, L"       3");	

	result = DoubleToFormStr(NAN, 8);	CheckResult(result, L"     NAN");	
//	result = DoubleToFormStr(0.0*INFINITY, 9);CheckResult(result, L"      NAN");	
	result = DoubleToFormStr(INFINITY, 4);	CheckResult(result, L" INF");		
	result = DoubleToFormStr(-INFINITY, 4);	CheckResult(result, L"-INF");		

	result = DoubleToFormStr(kPi, 3);	CheckResult(result, L" 3.");				
	result = DoubleToFormStr(kPi, 2);	CheckResult(result, L"**");				
	result = DoubleToFormStr(kPi, 1);	CheckResult(result, L"*");				
}


}	// namespace Whisper
