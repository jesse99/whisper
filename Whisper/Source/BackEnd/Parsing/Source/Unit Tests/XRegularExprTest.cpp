/*
 *  File:		XRegularExprTest.cpp
 *  Summary:	XRegularExpression unit test.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRegularExprTest.cpp,v $
 *		Revision 1.2  2000/11/09 12:49:32  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <2>	 8/12/00	JDJ		Placed in the Parsing suite.
 *		 <1>	 4/06/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XRegularExprTest.h>

#include <XDebug.h>
#include <XRegularExpression.h>

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class XRegExprUnitTest
// ===================================================================================	

//---------------------------------------------------------------
//
// XRegExprUnitTest::~XRegExprUnitTest
//
//---------------------------------------------------------------
XRegExprUnitTest::~XRegExprUnitTest()
{
}

	
//---------------------------------------------------------------
//
// XRegExprUnitTest::XRegExprUnitTest
//
//---------------------------------------------------------------
XRegExprUnitTest::XRegExprUnitTest() : XUnitTest(L"Parsing", L"XRegularExpression")
{
}

						
//---------------------------------------------------------------
//
// XRegExprUnitTest::OnTest
//
//---------------------------------------------------------------
void XRegExprUnitTest::OnTest()
{
	XRegularExpression expr;

	TRACE("Starting XRegularExpression test");

	ASSERT(this->DoMatch(L"a", L"a"));							// matching any character
	ASSERT(!this->DoMatch(L"a", L"b"));
	ASSERT(this->DoMatch(L"\\.\\*", L".*"));
	ASSERT(!this->DoMatch(L"\\.\\*", L"dog"));
	ASSERT(this->DoMatch(L"100", L"100"));
	ASSERT(!this->DoMatch(L"100", L"ABCDEFG"));

	expr.SetExpression(L".art");
	ASSERT(this->DoMatch(expr, L"dart"));
	ASSERT(this->DoMatch(expr, L"cart"));
	ASSERT(this->DoMatch(expr, L"tart"));
	ASSERT(!this->DoMatch(expr, L"art"));
	ASSERT(!this->DoMatch(expr, L"hurt"));
	ASSERT(!this->DoMatch(expr, L"dark"));
	TRACE(".");
	
	expr.SetExpression(L"a+b");
	ASSERT(this->DoMatch(expr, L"ab"));						// repeating expressions
	ASSERT(this->DoMatch(expr, L"aaab"));		
	ASSERT(!this->DoMatch(expr, L"b"));		
	ASSERT(!this->DoMatch(expr, L"baaa"));		
	
	expr.SetExpression(L"a*b");
	ASSERT(this->DoMatch(expr, L"b"));		
	ASSERT(this->DoMatch(expr, L"ab"));		
	ASSERT(this->DoMatch(expr, L"aaab"));		
	ASSERT(!this->DoMatch(expr, L"baaa"));		
	TRACE(".");

	expr.SetExpression(L".*cat");
	ASSERT(this->DoMatch(expr, L"9393cat"));		
	ASSERT(this->DoMatch(expr, L"ca9393cat"));		
	ASSERT(this->DoMatch(expr, L"cat"));		
	ASSERT(this->DoMatch(expr, L"the old cat"));		
	ASSERT(this->DoMatch(expr, L"c7sb@#puiercat"));		
	ASSERT(this->DoMatch(L"b.*ca*t", L"bfggfhgbcaaaaat"));		
	ASSERT(this->DoMatch(L"b.*ca*t", L"bfggfhgbct"));		
	ASSERT(this->DoMatch(L"b.*ca*t", L"bct"));		
	ASSERT(!this->DoMatch(L".*cat", L"dog"));		
	TRACE(".");

	expr.SetExpression(L"(ab)*c");
	ASSERT(this->DoMatch(expr, L"abc"));					// grouping expressions		
	ASSERT(this->DoMatch(expr, L"ababababc"));		
	ASSERT(!this->DoMatch(expr, L"aabbbc"));		
	ASSERT(!this->DoMatch(expr, L"abaac"));		

	expr.SetExpression(L"(.a)+b");
	ASSERT(this->DoMatch(expr, L"xab"));		
	ASSERT(this->DoMatch(expr, L"ra5afab"));		
	ASSERT(!this->DoMatch(expr, L"b"));		
	ASSERT(!this->DoMatch(expr, L"gaab"));		
	TRACE(".");

	expr.SetExpression(L"[aeiou][0-9]");
	ASSERT(this->DoMatch(expr, L"a6"));				// choosing one character from many	
	ASSERT(this->DoMatch(expr, L"i3"));		
	ASSERT(this->DoMatch(expr, L"u2"));		
	ASSERT(!this->DoMatch(expr, L"ex"));		
	ASSERT(!this->DoMatch(expr, L"9a"));		
	ASSERT(!this->DoMatch(expr, L"$6"));		

	expr.SetExpression(L"[^cfl]og");
	ASSERT(this->DoMatch(expr, L"dog"));		
	ASSERT(this->DoMatch(expr, L"bog"));		
	ASSERT(!this->DoMatch(expr, L"cog"));		
	ASSERT(!this->DoMatch(expr, L"fog"));		
	TRACE(".");

	expr.SetExpression(L"END[.]");
	ASSERT(this->DoMatch(expr, L"END."));		
	ASSERT(!this->DoMatch(expr, L"END;"));		
	ASSERT(!this->DoMatch(expr, L"END DO"));		
	ASSERT(!this->DoMatch(expr, L"ENDIAN"));		
	TRACE(".");

	ASSERT(this->DoMatch(L"^(the cat).+", L"the cat runs"));	// matching the beginning or end of a line	
	ASSERT(this->DoMatch(L"^(the cat).+", L"the cat "));		
	ASSERT(!this->DoMatch(L"^(the cat).+", L"the cat"));		
	ASSERT(this->DoMatch(L"^(the cat).*", L"the cat"));		
	ASSERT(this->DoMatch(L"^(the cat).*", L"the cat "));		
	ASSERT(!this->DoMatch(L"^(the cat).+", L"see the cat runs"));		

	ASSERT(this->DoMatch(L".+(the cat)$", L"watch the cat\r"));		
	ASSERT(!this->DoMatch(L".+(the cat)$", L"the cat eats\r"));		
	TRACE(".");

	expr.SetExpression(kIntegerExpr.GetExpression());
	ASSERT(this->DoMatch(expr, L"345"));					// optional and or expressions
	ASSERT(this->DoMatch(expr, L"0"));	
	ASSERT(this->DoMatch(expr, L"-56"));	
	ASSERT(!this->DoMatch(expr, L"--56"));	
	ASSERT(!this->DoMatch(expr, L"56a"));	
	ASSERT(!this->DoMatch(expr, L"a0"));	
	ASSERT(!this->DoMatch(expr, L"+"));	
	ASSERT(!this->DoMatch(expr, L"-"));	
	TRACE(".");

	expr.SetExpression(kFloatExpr.GetExpression());
	ASSERT(this->DoMatch(expr, L"0"));	
	ASSERT(this->DoMatch(expr, L"24"));	
	ASSERT(this->DoMatch(expr, L"0.0"));	
	ASSERT(this->DoMatch(expr, L".5"));	
	ASSERT(this->DoMatch(expr, L"3.1415"));	
	ASSERT(this->DoMatch(expr, L"-3.1415"));	
	ASSERT(this->DoMatch(expr, L"3.1415e1"));	
	ASSERT(this->DoMatch(expr, L"3.1415e-1"));	
	ASSERT(this->DoMatch(expr, L"-3.1415e+100"));	
	ASSERT(this->DoMatch(expr, L".5E200"));	
	ASSERT(!this->DoMatch(expr, L".5E"));	
	ASSERT(!this->DoMatch(expr, L"+"));	
	ASSERT(!this->DoMatch(expr, L"-"));	
	ASSERT(!this->DoMatch(expr, L"--9"));	
	ASSERT(!this->DoMatch(expr, L"3.1415f1"));	
	ASSERT(!this->DoMatch(expr, L".5."));	
	ASSERT(!this->DoMatch(expr, L"..5"));	
	ASSERT(!this->DoMatch(expr, L"0..5"));
	TRACE(".");

	expr.SetExpression(kWhiteSpaceExpr.GetExpression());
	ASSERT(this->DoMatch(expr, L" "));					// escape sequences
	ASSERT(this->DoMatch(expr, L"   "));
	ASSERT(this->DoMatch(expr, L" 	 "));
	ASSERT(this->DoMatch(expr, L"	"));
	ASSERT(this->DoMatch(expr, std::wstring(1, '\r')));
	TRACE(".");

	expr.SetExpression(L"([a-z]+)|(a21)");
	ASSERT(this->DoMatch(expr, L"beta"));			
	ASSERT(this->DoMatch(expr, L"a21"));
	ASSERT(!this->DoMatch(expr, L"a22"));
	ASSERT(!this->DoMatch(expr, L"ba22"));
	ASSERT(this->DoMatch(expr, L"alpha"));
	
	TRACE("done\n\n");
}


//---------------------------------------------------------------
//
// DoMatch (wstring, wstring)
//
//---------------------------------------------------------------
bool XRegExprUnitTest::DoMatch(const std::wstring& expr, const std::wstring& str)
{
	uint32 count = XRegularExpression::Match(expr, str);
	
	return count == str.length();
}


//---------------------------------------------------------------
//
// DoMatch (XRegularExpression, wstring)
//
//---------------------------------------------------------------
bool XRegExprUnitTest::DoMatch(const XRegularExpression& expr, const std::wstring& str)
{
	uint32 count = expr.Match(str);
	
	return count == str.length();
}


#endif	// DEBUG
}		// namespace Whisper

