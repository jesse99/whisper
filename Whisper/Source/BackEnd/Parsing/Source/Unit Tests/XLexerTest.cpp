/*
 *  File:		XLexerTest.cpp
 *  Summary:	XLexer unit test.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XLexerTest.cpp,v $
 *		Revision 1.2  2000/11/09 12:41:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <2>	 8/12/00	JDJ		Placed in the Parsing suite.
 *		 <1>	 4/06/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XLexerTest.h>

#include <XLexer.h>
#include <XRegularExpression.h>
#include <XScanners.h>

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class XLexerUnitTest
// ===================================================================================	

//---------------------------------------------------------------
//
// XLexerUnitTest::~XLexerUnitTest
//
//---------------------------------------------------------------
XLexerUnitTest::~XLexerUnitTest()
{
}

	
//---------------------------------------------------------------
//
// XLexerUnitTest::XLexerUnitTest
//
//---------------------------------------------------------------
XLexerUnitTest::XLexerUnitTest() : XUnitTest(L"Parsing", L"XLexer")
{
}

						
//---------------------------------------------------------------
//
// XLexerUnitTest::OnTest
//
//---------------------------------------------------------------
void XLexerUnitTest::OnTest()
{
	const XRegularExpression kFloatExpr2(L"-?(([0-9]+\\.[0-9]*)|(\\.[0-9]+))([eE][-+]?[0-9]+)?");	// requires decimal point (avoids conflicting with kIntegerExpr)

	std::wstring text =  L"class A {												\r"	
					L"														\r"
					L"public:												\r"
					L"			 			~A();							\r"
					L"														\r"
					L"						A(int param = 23);				\r"
					L"														\r"
					L"public:												\r"
					L"			void 		Foo(const XString& text);		\r"
					L"						// comment ; A					\r"
					L"														\r"
					L"			XString 	Goo(float param = 4.5) const;	\r"
					L"														\r"
					L"protected:											\r"
					L"	XLexerGrammar*	mGrammar;							\r"
					L"	XString			mText;								\r"
					L"};													\r";
	
	const TokenNum kClassToken       = 1;
	const TokenNum kIdentToken       = 2;
	const TokenNum kOpenBraceToken   = 3;
	const TokenNum kPublicToken      = 4;
	const TokenNum kColonToken       = 5;
	const TokenNum kTildeToken       = 6;
	const TokenNum kOpenParensToken  = 7;
	const TokenNum kIntToken         = 8;
	const TokenNum kFloatToken       = 9;
	const TokenNum kConstToken       = 10;
	const TokenNum kAmpersandToken   = 11;
	const TokenNum kCloseParensToken = 12;
	const TokenNum kSemiColonToken   = 13;
	const TokenNum kProtectedToken   = 14;
	const TokenNum kStarToken        = 15;
	const TokenNum kCloseBraceToken  = 16;
	const TokenNum kEqualsToken      = 17;
	
	XScanner* scanner = new XCScanner;
	XLexerGrammar* grammar = new XLexerGrammar;
	
	grammar->AddExpr(kIdentifierExpr.GetExpression(),	kIdentToken);
	grammar->AddExpr(kIntegerExpr.GetExpression(), 		kIntToken);
	grammar->AddExpr(kFloatExpr2.GetExpression(), 		kFloatToken);
	
	grammar->AddStr(L"class", 		kClassToken);
	grammar->AddStr(L"public", 		kPublicToken);
	grammar->AddStr(L"protected",	kProtectedToken);
	grammar->AddStr(L"const", 		kConstToken);

	grammar->AddStr(L"{", kOpenBraceToken);
	grammar->AddStr(L":", kColonToken);
	grammar->AddStr(L"~", kTildeToken);
	grammar->AddStr(L"(", kOpenParensToken);
	grammar->AddStr(L"&", kAmpersandToken);
	grammar->AddStr(L")", kCloseParensToken);
	grammar->AddStr(L";", kSemiColonToken);
	grammar->AddStr(L"*", kStarToken);
	grammar->AddStr(L"}", kCloseBraceToken);
	grammar->AddStr(L"=", kEqualsToken);
	
//	TRACE("String DFA:\n");
//	grammar->GetStrDFA()->Trace();
//	TRACE("\n");
	
	XLexer lexer(scanner, grammar);
	
	lexer.Analyze(text);
	
	// line 0
	lexer.ReadToken();									// class A {
	ASSERT(lexer.GetToken().num == kClassToken);
	ASSERT(lexer.GetToken().text == L"class");
	ASSERT(lexer.GetToken().pos.GetLine() == 0);

	ASSERT(lexer.GetToken(1).num == kIdentToken);
	ASSERT(lexer.GetToken(1).text == L"class");
	ASSERT(lexer.GetToken(1).pos.GetLine() == 0);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"A");
	ASSERT(lexer.GetToken().pos.GetLine() == 0);

	lexer.ReadFixedToken(1);
	ASSERT(lexer.GetToken().num == kOpenBraceToken);	
	ASSERT(lexer.GetToken().text == L"{");				
	ASSERT(lexer.GetToken().pos.GetLine() == 0);

	// line 2
	lexer.ReadToken();									// public:
	ASSERT(lexer.GetToken().num == kPublicToken);
	ASSERT(lexer.GetToken().text == L"public");
	ASSERT(lexer.GetToken().pos.GetLine() == 2);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kColonToken);
	ASSERT(lexer.GetToken().text == L":");
	ASSERT(lexer.GetToken().pos.GetLine() == 2);

	// line 3
	lexer.ReadShortestToken();							// ~A();
	ASSERT(lexer.GetToken().num == kTildeToken);
	ASSERT(lexer.GetToken().text == L"~");
	ASSERT(lexer.GetToken().pos.GetLine() == 3);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"A");
	ASSERT(lexer.GetToken().pos.GetLine() == 3);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kOpenParensToken);
	ASSERT(lexer.GetToken().text == L"(");
	ASSERT(lexer.GetToken().pos.GetLine() == 3);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kCloseParensToken);
	ASSERT(lexer.GetToken().text == L")");
	ASSERT(lexer.GetToken().pos.GetLine() == 3);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kSemiColonToken);
	ASSERT(lexer.GetToken().text == L";");
	ASSERT(lexer.GetToken().pos.GetLine() == 3);

	// line 5										
	lexer.ReadToken();									// A(int param = 23); 
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"A");
	ASSERT(lexer.GetToken().pos.GetLine() == 5);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kOpenParensToken);
	ASSERT(lexer.GetToken().text == L"(");
	ASSERT(lexer.GetToken().pos.GetLine() == 5);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"int");
	ASSERT(lexer.GetToken().pos.GetLine() == 5);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"param");
	ASSERT(lexer.GetToken().pos.GetLine() == 5);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kEqualsToken);
	ASSERT(lexer.GetToken().text == L"=");
	ASSERT(lexer.GetToken().pos.GetLine() == 5);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kIntToken);
	ASSERT(lexer.GetToken().text == L"23");
	ASSERT(lexer.GetToken().pos.GetLine() == 5);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kCloseParensToken);
	ASSERT(lexer.GetToken().text == L")");
	ASSERT(lexer.GetToken().pos.GetLine() == 5);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kSemiColonToken);
	ASSERT(lexer.GetToken().text == L";");
	ASSERT(lexer.GetToken().pos.GetLine() == 5);

	// line 7
	lexer.ReadToken();									// public:
	ASSERT(lexer.GetToken().num == kPublicToken);
	ASSERT(lexer.GetToken().text == L"public");
	ASSERT(lexer.GetToken().pos.GetLine() == 7);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kColonToken);
	ASSERT(lexer.GetToken().text == L":");
	ASSERT(lexer.GetToken().pos.GetLine() == 7);

	// line 8
	lexer.ReadToken();									// void Foo(const XString& text);
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"void");
	ASSERT(lexer.GetToken().pos.GetLine() == 8);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"Foo");
	ASSERT(lexer.GetToken().pos.GetLine() == 8);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kOpenParensToken);
	ASSERT(lexer.GetToken().text == L"(");
	ASSERT(lexer.GetToken().pos.GetLine() == 8);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kConstToken);
	ASSERT(lexer.GetToken().text == L"const");
	ASSERT(lexer.GetToken().pos.GetLine() == 8);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"XString");
	ASSERT(lexer.GetToken().pos.GetLine() == 8);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kAmpersandToken);
	ASSERT(lexer.GetToken().text == L"&");
	ASSERT(lexer.GetToken().pos.GetLine() == 8);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"text");
	ASSERT(lexer.GetToken().pos.GetLine() == 8);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kCloseParensToken);
	ASSERT(lexer.GetToken().text == L")");
	ASSERT(lexer.GetToken().pos.GetLine() == 8);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kSemiColonToken);
	ASSERT(lexer.GetToken().text == L";");
	ASSERT(lexer.GetToken().pos.GetLine() == 8);

	// line 11
	lexer.ReadToken();									// XString Goo(float param = 4.5) const;
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"XString");
	ASSERT(lexer.GetToken().pos.GetLine() == 11);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"Goo");
	ASSERT(lexer.GetToken().pos.GetLine() == 11);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kOpenParensToken);
	ASSERT(lexer.GetToken().text == L"(");
	ASSERT(lexer.GetToken().pos.GetLine() == 11);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"float");
	ASSERT(lexer.GetToken().pos.GetLine() == 11);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"param");
	ASSERT(lexer.GetToken().pos.GetLine() == 11);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kEqualsToken);
	ASSERT(lexer.GetToken().text == L"=");
	ASSERT(lexer.GetToken().pos.GetLine() == 11);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kFloatToken);
	ASSERT(lexer.GetToken().text == L"4.5");
	ASSERT(lexer.GetToken().pos.GetLine() == 11);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kCloseParensToken);
	ASSERT(lexer.GetToken().text == L")");
	ASSERT(lexer.GetToken().pos.GetLine() == 11);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kConstToken);
	ASSERT(lexer.GetToken().text == L"const");
	ASSERT(lexer.GetToken().pos.GetLine() == 11);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kSemiColonToken);
	ASSERT(lexer.GetToken().text == L";");
	ASSERT(lexer.GetToken().pos.GetLine() == 11);

	// line 13
	lexer.ReadToken();									// protected:	
	ASSERT(lexer.GetToken().num == kProtectedToken);
	ASSERT(lexer.GetToken().text == L"protected");
	ASSERT(lexer.GetToken().pos.GetLine() == 13);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kColonToken);
	ASSERT(lexer.GetToken().text == L":");
	ASSERT(lexer.GetToken().pos.GetLine() == 13);

	// line 14
	lexer.ReadToken();									// XLexerGrammar* mGrammar;
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"XLexerGrammar");
	ASSERT(lexer.GetToken().pos.GetLine() == 14);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kStarToken);
	ASSERT(lexer.GetToken().text == L"*");
	ASSERT(lexer.GetToken().pos.GetLine() == 14);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"mGrammar");
	ASSERT(lexer.GetToken().pos.GetLine() == 14);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kSemiColonToken);
	ASSERT(lexer.GetToken().text == L";");
	ASSERT(lexer.GetToken().pos.GetLine() == 14);

	// line 15
	lexer.ReadToken();									// XString mText;
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"XString");
	ASSERT(lexer.GetToken().pos.GetLine() == 15);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kIdentToken);
	ASSERT(lexer.GetToken().text == L"mText");
	ASSERT(lexer.GetToken().pos.GetLine() == 15);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kSemiColonToken);
	ASSERT(lexer.GetToken().text == L";");
	ASSERT(lexer.GetToken().pos.GetLine() == 15);

	// line 16
	lexer.ReadToken();									// };
	ASSERT(lexer.GetToken().num == kCloseBraceToken);
	ASSERT(lexer.GetToken().text == L"}");
	ASSERT(lexer.GetToken().pos.GetLine() == 16);

	lexer.ReadToken();
	ASSERT(lexer.GetToken().num == kSemiColonToken);
	ASSERT(lexer.GetToken().text == L";");
	ASSERT(lexer.GetToken().pos.GetLine() == 16);
	
	lexer.ReadToken();
	ASSERT(lexer.GetNumTokens() == 0);
	
	TRACE("Completed XLexer test.\n\n");
}


#endif	// DEBUG
}		// namespace Whisper

