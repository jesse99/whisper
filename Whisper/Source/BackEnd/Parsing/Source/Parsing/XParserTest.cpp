/*
 *  File:		XParserTest.cpp
 *  Summary:	A simple expression parser for use by XParser's unit test.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XParserTest.cpp,v $
 *		Revision 1.3  2001/04/27 04:35:06  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:45:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XParserTest.h>

#if DEBUG
#include <XAutoPtr.h>
#include <XIntConversions.h>
#include <XLexer.h>
#include <XScanner.h>
#include <XStringUtils.h>
#include <XTree.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const TokenNum kAddToken     = '+';
const TokenNum kMinusToken   = '-';
const TokenNum kProductToken = '*';
const TokenNum kDivideToken  = '/';
const TokenNum kNumberToken  = '#';


// ===================================================================================
//	class ZExpressionTree
// ===================================================================================

//---------------------------------------------------------------
//
// ZExpressionTree::~ZExpressionTree
//
//---------------------------------------------------------------
ZExpressionTree::~ZExpressionTree()
{
}


//---------------------------------------------------------------
//
// ZExpressionTree::ZExpressionTree
//
//---------------------------------------------------------------
ZExpressionTree::ZExpressionTree(const SToken& token) : XParseTree(token)
{
	mComputed = false;
}


//---------------------------------------------------------------
//
// ZExpressionTree::Evaluate
//
//---------------------------------------------------------------
int32 ZExpressionTree::Evaluate()
{	
	if (!mComputed) {		
		mValue = this->OnEvaluate();
	
		mComputed = true;
	}
	
	return mValue;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZAddTree
// ===================================================================================

//---------------------------------------------------------------
//
// ZAddTree::~ZAddTree
//
//---------------------------------------------------------------
ZAddTree::~ZAddTree()
{
}


//---------------------------------------------------------------
//
// ZAddTree::ZAddTree
//
//---------------------------------------------------------------
ZAddTree::ZAddTree(const SToken& token) : ZExpressionTree(token)
{
	PRECONDITION(token.num == kAddToken);
}


//---------------------------------------------------------------
//
// ZAddTree::OnEvaluate
//
//---------------------------------------------------------------
int32 ZAddTree::OnEvaluate()
{
	PRECONDITION(this->GetNumChildren() == 2);

	ZExpressionTree* lhs = dynamic_cast<ZExpressionTree*>(this->GetLeftChild());
	ZExpressionTree* rhs = dynamic_cast<ZExpressionTree*>(this->GetRightChild());
	
	int32 left  = lhs->Evaluate();
	int32 right = rhs->Evaluate();
	
	int32 value = left + right;
		
	return value;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZSubtractTree
// ===================================================================================

//---------------------------------------------------------------
//
// ZSubtractTree::~ZSubtractTree
//
//---------------------------------------------------------------
ZSubtractTree::~ZSubtractTree()
{
}


//---------------------------------------------------------------
//
// ZSubtractTree::ZSubtractTree
//
//---------------------------------------------------------------
ZSubtractTree::ZSubtractTree(const SToken& token) : ZExpressionTree(token)
{
	PRECONDITION(token.num == kMinusToken);
}


//---------------------------------------------------------------
//
// ZSubtractTree::OnEvaluate
//
//---------------------------------------------------------------
int32 ZSubtractTree::OnEvaluate()
{
	PRECONDITION(this->GetNumChildren() == 2);

	ZExpressionTree* lhs = dynamic_cast<ZExpressionTree*>(this->GetLeftChild());
	ZExpressionTree* rhs = dynamic_cast<ZExpressionTree*>(this->GetRightChild());
	
	int32 left  = lhs->Evaluate();
	int32 right = rhs->Evaluate();
	
	int32 value = left - right;
		
	return value;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZMultiplyTree
// ===================================================================================

//---------------------------------------------------------------
//
// ZMultiplyTree::~ZMultiplyTree
//
//---------------------------------------------------------------
ZMultiplyTree::~ZMultiplyTree()
{
}


//---------------------------------------------------------------
//
// ZMultiplyTree::ZMultiplyTree
//
//---------------------------------------------------------------
ZMultiplyTree::ZMultiplyTree(const SToken& token) : ZExpressionTree(token)
{
	PRECONDITION(token.num == kProductToken);
}


//---------------------------------------------------------------
//
// ZMultiplyTree::OnEvaluate
//
//---------------------------------------------------------------
int32 ZMultiplyTree::OnEvaluate()
{
	PRECONDITION(this->GetNumChildren() == 2);

	ZExpressionTree* lhs = dynamic_cast<ZExpressionTree*>(this->GetLeftChild());
	ZExpressionTree* rhs = dynamic_cast<ZExpressionTree*>(this->GetRightChild());
	
	int32 left  = lhs->Evaluate();
	int32 right = rhs->Evaluate();
	
	int32 value = left*right;
		
	return value;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZDivideTree
// ===================================================================================

//---------------------------------------------------------------
//
// ZDivideTree::~ZDivideTree
//
//---------------------------------------------------------------
ZDivideTree::~ZDivideTree()
{
}


//---------------------------------------------------------------
//
// ZDivideTree::ZDivideTree
//
//---------------------------------------------------------------
ZDivideTree::ZDivideTree(const SToken& token) : ZExpressionTree(token)
{
	PRECONDITION(token.num == kDivideToken);
}


//---------------------------------------------------------------
//
// ZDivideTree::OnEvaluate
//
//---------------------------------------------------------------
int32 ZDivideTree::OnEvaluate()
{
	PRECONDITION(this->GetNumChildren() == 2);

	ZExpressionTree* lhs = dynamic_cast<ZExpressionTree*>(this->GetLeftChild());
	ZExpressionTree* rhs = dynamic_cast<ZExpressionTree*>(this->GetRightChild());
	
	int32 left  = lhs->Evaluate();
	int32 right = rhs->Evaluate();
	
	int32 value = left/right;
		
	return value;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZLiteralTree
// ===================================================================================

//---------------------------------------------------------------
//
// ZLiteralTree::~ZLiteralTree
//
//---------------------------------------------------------------
ZLiteralTree::~ZLiteralTree()
{
}


//---------------------------------------------------------------
//
// ZLiteralTree::ZLiteralTree
//
//---------------------------------------------------------------
ZLiteralTree::ZLiteralTree(const SToken& token, int32 value) : ZExpressionTree(token)
{
	PRECONDITION(token.num == kNumberToken);

	mLiteral = value;
}


//---------------------------------------------------------------
//
// ZLiteralTree::OnEvaluate
//
//---------------------------------------------------------------
int32 ZLiteralTree::OnEvaluate()
{
	PRECONDITION(this->GetNumChildren() == 0);
	
	return mLiteral;
}

#if __MWERKS__
#pragma mark -
#pragma mark -
#endif

// ===================================================================================
//	class ZParseExpression
// ===================================================================================

//---------------------------------------------------------------
//
// ZParseExpression::~ZParseExpression
//
//---------------------------------------------------------------
ZParseExpression::~ZParseExpression()
{
}


//---------------------------------------------------------------
//
// ZParseExpression::ZParseExpression
//
//---------------------------------------------------------------
ZParseExpression::ZParseExpression()
{
}


//---------------------------------------------------------------
//
// ZParseExpression::OnIsOperand
//
//---------------------------------------------------------------
bool ZParseExpression::OnIsOperand(const SToken& token)
{
	bool isToken = token.num == kAddToken || token.num == kMinusToken;
	
	return isToken;
}


//---------------------------------------------------------------
//
// ZParseExpression::OnCreateOperand
//
//---------------------------------------------------------------
XParseTree* ZParseExpression::OnCreateOperand(const SToken& token)
{
	XParseTree* tree = nil;

	if (token.num == kAddToken)
		tree = new ZAddTree(token);
	else
		tree = new ZSubtractTree(token);

	return tree;
}


//---------------------------------------------------------------
//
// ZParseExpression::OnParse
//
// Expression := Term (('+' | '-') Term)*
//
//---------------------------------------------------------------
XParseTree* ZParseExpression::OnParse(XLexer* lexer)
{	
	XParseTree* tree = ZParseTerm().CreateParseTree(lexer);
			
	return tree;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZParseTerm
// ===================================================================================

//---------------------------------------------------------------
//
// ZParseTerm::~ZParseTerm
//
//---------------------------------------------------------------
ZParseTerm::~ZParseTerm()
{
}


//---------------------------------------------------------------
//
// ZParseTerm::ZParseTerm
//
//---------------------------------------------------------------
ZParseTerm::ZParseTerm()
{
}


//---------------------------------------------------------------
//
// ZParseTerm::OnIsOperand
//
//---------------------------------------------------------------
bool ZParseTerm::OnIsOperand(const SToken& token)
{
	bool isToken = token.num == kProductToken || token.num == kDivideToken;
	
	return isToken;
}


//---------------------------------------------------------------
//
// ZParseTerm::OnCreateOperand
//
//---------------------------------------------------------------
XParseTree* ZParseTerm::OnCreateOperand(const SToken& token)
{
	XParseTree* tree = nil;

	if (token.num == kProductToken)
		tree = new ZMultiplyTree(token);
	else
		tree = new ZDivideTree(token);

	return tree;
}


//---------------------------------------------------------------
//
// ZParseTerm::OnParse
//
// Term := Factor (('*' | '/') Factor)*
//
//---------------------------------------------------------------
XParseTree* ZParseTerm::OnParse(XLexer* lexer)
{	
	XParseTree* tree = ZParseFactor().CreateParseTree(lexer);
			
	return tree;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZParseFactor
// ===================================================================================

//---------------------------------------------------------------
//
// ZParseFactor::~ZParseFactor
//
//---------------------------------------------------------------
ZParseFactor::~ZParseFactor()
{
}


//---------------------------------------------------------------
//
// ZParseFactor::ZParseFactor
//
//---------------------------------------------------------------
ZParseFactor::ZParseFactor()
{
}


//---------------------------------------------------------------
//
// ZParseFactor::OnParse
//
// Factor := Number | ('(' Expression ')')
//
//---------------------------------------------------------------
XParseTree* ZParseFactor::OnParse(XLexer* lexer)
{
	XParseTree* tree = nil;
	
	SToken token = lexer->GetToken();
	wchar_t ch = token.text[0];
	
	if (IsDigit(ch)) {
		ZParseNumber number;
		tree = number.CreateParseTree(lexer);
	
	} else if (ch == '(') {
		lexer->ReadToken();
	
		ZParseExpression expression;
		tree = expression.CreateParseTree(lexer);

		token = lexer->GetToken();
		ch = token.text[0];
		
		if (ch == ')')
			lexer->ReadToken();
		else {
			delete tree;
			throw XParserException(lexer->GetScanner(), LoadWhisperString(L"Expected a right parenthesis."));
		}

	} else
		throw XParserException(lexer->GetScanner(), LoadWhisperString(L"Expected an integer or a left parenthesis."));
		
	return tree;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZParseNumber
// ===================================================================================

//---------------------------------------------------------------
//
// ZParseNumber::~ZParseNumber
//
//---------------------------------------------------------------
ZParseNumber::~ZParseNumber()
{
}


//---------------------------------------------------------------
//
// ZParseNumber::ZParseNumber
//
//---------------------------------------------------------------
ZParseNumber::ZParseNumber()
{
}


//---------------------------------------------------------------
//
// ZParseNumber::OnCreateTree
//
// Number := Digit Digit*
// Digit  := '0'..'9'
//
//---------------------------------------------------------------
XParseTree* ZParseNumber::OnCreateTree(XLexer* lexer)
{
	SToken token = lexer->GetToken();
	lexer->ReadToken();
	
	int32 value = StrToInt32(token.text);
	ZLiteralTree* tree = new ZLiteralTree(token, value);
	
	return tree;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XParserUnitTest
// ===================================================================================	

//---------------------------------------------------------------
//
// XParserUnitTest::~XParserUnitTest
//
//---------------------------------------------------------------
XParserUnitTest::~XParserUnitTest()
{
}

	
//---------------------------------------------------------------
//
// XParserUnitTest::XParserUnitTest
//
//---------------------------------------------------------------
XParserUnitTest::XParserUnitTest() : XUnitTest(L"Parsing", L"XParser")
{
}

						
//---------------------------------------------------------------
//
// XParserUnitTest::OnTest
//
//---------------------------------------------------------------
void XParserUnitTest::OnTest()
{
	this->DoParserTest(L"4", 				4);
	this->DoParserTest(L"2-3", 				-1);
	this->DoParserTest(L"2+3", 				5);
	this->DoParserTest(L"2*3", 				6);
	this->DoParserTest(L"2+3*5", 			17);
	this->DoParserTest(L"(2+3)*5", 			25);
	this->DoParserTest(L"100 + 3*5",		115);
	this->DoParserTest(L"(((2)) - (9/3))",	-1);
	this->DoParserTest(L" 2+ 3  *4 * 5+1", 	63);
	this->DoParserTest(L"2-3	 ", 		 -1);
	
	TRACE("Will throw four exceptions\n");

	this->DoBadTest(L"2-3+");
	this->DoBadTest(L"2-*3");
	this->DoBadTest(L"2-3 5");
	this->DoBadTest(L"*2-3");

	TRACE("Completed Parser test.\n\n");
}


//---------------------------------------------------------------
//
// XParserUnitTest::DoBadTest
//
//---------------------------------------------------------------
void XParserUnitTest::DoBadTest(const wchar_t* str)
{
	try {
		this->DoParserTest(str, -1);
		DEBUGSTR("how'd we wind up here?");
	} catch (const XParserException&) {
	} catch (...) {
		DEBUGSTR("Caught the wrong exception!");
	}
}


//---------------------------------------------------------------
//
// XParserUnitTest::DoParserTest
//
//---------------------------------------------------------------
void XParserUnitTest::DoParserTest(const std::wstring& text, int32 result)
{	
	XScanner* scanner = new XScanner(text);
	XLexerGrammar* grammar = new XLexerGrammar;
	
	grammar->AddExpr(L"[0-9]+", kNumberToken);		// in a real app you'd read the grammar in from a resource
	grammar->AddStr(L"+", kAddToken);
	grammar->AddStr(L"-", kMinusToken);
	grammar->AddStr(L"*", kProductToken);
	grammar->AddStr(L"/", kDivideToken);
	grammar->AddStr(L"(", '(');
	grammar->AddStr(L")", ')');
	
	XLexer lexer(scanner, grammar);
	lexer.ReadToken();
	
	ZParseExpression parser;
	XAutoPtr<XParseTree> tree(parser.CreateParseTree(&lexer));
	if (!lexer.AtEnd())
		throw XParserException(lexer.GetScanner(), LoadWhisperString(L"Found cruft at the end."));
	
	ZExpressionTree* node = dynamic_cast<ZExpressionTree*>(tree.Get());
	ASSERT(node != nil);
	
	int32 value = node->Evaluate();
	ASSERT(value == result);
}

}		// namespace Whisper

#endif	// DEBUG
