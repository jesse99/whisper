/*
 *  File:		CColorParser.cpp
 *  Summary:	Classes that build an expression tree for the formula's used to compute fractal colors.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CColorParser.cpp,v $
 *		Revision 1.2  2001/04/21 02:23:34  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.1  2000/12/28 02:48:34  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:53:23  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
  */

#include "AppHeader.h"
#include "CColorParser.h"

#include <cctype>

#include <XAutoPtr.h>
#include <XFloatConversions.h>
#include <XLexer.h>
#include <XMiscUtils.h>
#include <XNumbers.h>
#include <XScanner.h>
#include <XTree.h>

#include "CColorEvaluator.h"


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CParseTerminal::IsFunction
//
//---------------------------------------------------------------
static bool IsFunction(const std::wstring& name)
{
	bool is = false;
	
	if (name == L"sin") 
		is = true;
	else if (name == L"cos")
		is = true;
	else if (name == L"tan")
		is = true;
	else if (name == L"sqr")
		is = true;
	else if (name == L"sqrt")
		is = true;
	else if (name == L"exp")
		is = true;
	else if (name == L"ln")
		is = true;
	else if (name == L"log")
		is = true;
	else if (name == L"round")
		is = true;
	else if (name == L"trunc")
		is = true;
	else if (name == L"abs")
		is = true;
	else if (name == L"random")
		is = true;

	return is;
}

#pragma mark -

// ===================================================================================
//	class CColorTree
// ===================================================================================

//---------------------------------------------------------------
//
// CColorTree::~CColorTree
//
//---------------------------------------------------------------
CColorTree::~CColorTree()
{
}


//---------------------------------------------------------------
//
// CColorTree::CColorTree
//
//---------------------------------------------------------------
CColorTree::CColorTree(const SToken& token) : XParseTree(token)
{
}


//---------------------------------------------------------------
//
// CColorTree::Create									[static]
//
//---------------------------------------------------------------
CColorTree* CColorTree::Create(const std::wstring& text)
{	
	XScanner* scanner = new XScanner(text);
	XLexerGrammar* grammer = CColorTree::DoCreateGrammar();
	
	XLexer lexer(scanner, grammer);
	lexer.ReadToken();
		
	CParseColorExpr parser;
	XAutoPtr<XParseTree> tree(parser.CreateParseTree(&lexer));
	if (!lexer.AtEnd())
		throw XParserException(lexer.GetScanner(), L"Found cruft at end.");
	
	CColorTree* result = dynamic_cast<CColorTree*>(tree.Release());
	ASSERT(result != nil);
	
	return result;
}


//---------------------------------------------------------------
//
// CColorTree::Compile
//
//---------------------------------------------------------------
void CColorTree::Compile(CColorEvaluator& evaluator)
{	
	this->OnCompile(evaluator);
}


//---------------------------------------------------------------
//
// CColorTree::DoCreateGrammar							[static]
//
//---------------------------------------------------------------
XLexerGrammar* CColorTree::DoCreateGrammar()
{
	XLexerGrammar* grammar = new XLexerGrammar;
	
	{
//	XCodeTimer timer(L"CColorTree::DoCreateGrammar");
		grammar->AddExpr(L"-?(([0-9]+\\.[0-9]*)|([0-9]+)|(\\.[0-9]+))([eE][-+]?[0-9]+)?", kNumberToken);
		grammar->AddExpr(L"[xyzXYZ]", kVariableToken);
		grammar->AddExpr(L"sin|cos|tan|sqr|sqrt|exp|ln|log|round|trunc|abs|random", kFunctionToken);

		grammar->AddStr(L"^", kPowToken);
		grammar->AddStr(L"+", kAddToken);
		grammar->AddStr(L"-", kMinusToken);
		grammar->AddStr(L"*", kProductToken);
		grammar->AddStr(L"/", kDivideToken);
		grammar->AddStr(L"(", kLeftParensToken);
		grammar->AddStr(L")", kRightParensToken);
	}
		
	return grammar;
}

#pragma mark -

// ===================================================================================
//	class CAddTree
// ===================================================================================

//---------------------------------------------------------------
//
// CAddTree::~CAddTree
//
//---------------------------------------------------------------
CAddTree::~CAddTree()
{
}


//---------------------------------------------------------------
//
// CAddTree::CAddTree
//
//---------------------------------------------------------------
CAddTree::CAddTree(const SToken& token) : CColorTree(token)
{
	PRECONDITION(token.num == kAddToken);
}


//---------------------------------------------------------------
//
// CAddTree::OnCompile			$$$ use an abstract CDyadicTree class?
//
//---------------------------------------------------------------
void CAddTree::OnCompile(CColorEvaluator& evaluator)
{
	PRECONDITION(this->GetNumChildren() == 2);

	CColorTree* lhs = dynamic_cast<CColorTree*>(this->GetLeftChild());
	CColorTree* rhs = dynamic_cast<CColorTree*>(this->GetRightChild());
	
	lhs->Compile(evaluator);
	rhs->Compile(evaluator);
	
	evaluator.PushToken(mToken);
}

#pragma mark -

// ===================================================================================
//	class CSubtractTree
// ===================================================================================

//---------------------------------------------------------------
//
// CSubtractTree::~CSubtractTree
//
//---------------------------------------------------------------
CSubtractTree::~CSubtractTree()
{
}


//---------------------------------------------------------------
//
// CSubtractTree::CSubtractTree
//
//---------------------------------------------------------------
CSubtractTree::CSubtractTree(const SToken& token) : CColorTree(token)
{
	PRECONDITION(token.num == kMinusToken);
}


//---------------------------------------------------------------
//
// CSubtractTree::OnCompile
//
//---------------------------------------------------------------
void CSubtractTree::OnCompile(CColorEvaluator& evaluator)
{
	PRECONDITION(this->GetNumChildren() == 2);

	CColorTree* lhs = dynamic_cast<CColorTree*>(this->GetLeftChild());
	CColorTree* rhs = dynamic_cast<CColorTree*>(this->GetRightChild());
	
	lhs->Compile(evaluator);
	rhs->Compile(evaluator);
	
	evaluator.PushToken(mToken);
}

#pragma mark -

// ===================================================================================
//	class CMultiplyTree
// ===================================================================================

//---------------------------------------------------------------
//
// CMultiplyTree::~CMultiplyTree
//
//---------------------------------------------------------------
CMultiplyTree::~CMultiplyTree()
{
}


//---------------------------------------------------------------
//
// CMultiplyTree::CMultiplyTree
//
//---------------------------------------------------------------
CMultiplyTree::CMultiplyTree(const SToken& token) : CColorTree(token)
{
	PRECONDITION(token.num == kProductToken);
}


//---------------------------------------------------------------
//
// CMultiplyTree::OnCompile
//
//---------------------------------------------------------------
void CMultiplyTree::OnCompile(CColorEvaluator& evaluator)
{
	PRECONDITION(this->GetNumChildren() == 2);

	CColorTree* lhs = dynamic_cast<CColorTree*>(this->GetLeftChild());
	CColorTree* rhs = dynamic_cast<CColorTree*>(this->GetRightChild());
	
	lhs->Compile(evaluator);
	rhs->Compile(evaluator);
	
	evaluator.PushToken(mToken);
}

#pragma mark -

// ===================================================================================
//	class CDivideTree
// ===================================================================================

//---------------------------------------------------------------
//
// CDivideTree::~CDivideTree
//
//---------------------------------------------------------------
CDivideTree::~CDivideTree()
{
}


//---------------------------------------------------------------
//
// CDivideTree::CDivideTree
//
//---------------------------------------------------------------
CDivideTree::CDivideTree(const SToken& token) : CColorTree(token)
{
	PRECONDITION(token.num == kDivideToken);
}


//---------------------------------------------------------------
//
// CDivideTree::OnCompile
//
//---------------------------------------------------------------
void CDivideTree::OnCompile(CColorEvaluator& evaluator)
{
	PRECONDITION(this->GetNumChildren() == 2);

	CColorTree* lhs = dynamic_cast<CColorTree*>(this->GetLeftChild());
	CColorTree* rhs = dynamic_cast<CColorTree*>(this->GetRightChild());
	
	lhs->Compile(evaluator);
	rhs->Compile(evaluator);
	
	evaluator.PushToken(mToken);
}

#pragma mark -

// ===================================================================================
//	class CPowTree
// ===================================================================================

//---------------------------------------------------------------
//
// CPowTree::~CPowTree
//
//---------------------------------------------------------------
CPowTree::~CPowTree()
{
}


//---------------------------------------------------------------
//
// CPowTree::CPowTree
//
//---------------------------------------------------------------
CPowTree::CPowTree(const SToken& token) : CColorTree(token)
{
	PRECONDITION(token.num == kPowToken);
}


//---------------------------------------------------------------
//
// CPowTree::OnCompile
//
//---------------------------------------------------------------
void CPowTree::OnCompile(CColorEvaluator& evaluator)
{
	PRECONDITION(this->GetNumChildren() == 2);

	CColorTree* lhs = dynamic_cast<CColorTree*>(this->GetLeftChild());
	CColorTree* rhs = dynamic_cast<CColorTree*>(this->GetRightChild());
	
	lhs->Compile(evaluator);
	rhs->Compile(evaluator);
	
	evaluator.PushToken(mToken);
}

#pragma mark -

// ===================================================================================
//	class CFunctionTree
// ===================================================================================

//---------------------------------------------------------------
//
// CFunctionTree::~CFunctionTree
//
//---------------------------------------------------------------
CFunctionTree::~CFunctionTree()
{
}


//---------------------------------------------------------------
//
// CFunctionTree::CFunctionTree
//
//---------------------------------------------------------------
CFunctionTree::CFunctionTree(const SToken& token) : CColorTree(token)
{
	PRECONDITION(token.num == kFunctionToken);
	PRECONDITION(IsFunction(token.text));
	
	mFunction = token.text;
}


//---------------------------------------------------------------
//
// CFunctionTree::OnCompile
//
//---------------------------------------------------------------
void CFunctionTree::OnCompile(CColorEvaluator& evaluator)
{
	PRECONDITION(this->GetNumChildren() == 1);

	CColorTree* child = dynamic_cast<CColorTree*>(this->GetLeftChild());
	child->Compile(evaluator);
		
	if (mFunction == L"sin") 
		evaluator.PushToken(kSinFunctionToken);
	else if (mFunction == L"cos")
		evaluator.PushToken(kCosFunctionToken);
	else if (mFunction == L"tan")
		evaluator.PushToken(kTanFunctionToken);
	else if (mFunction == L"sqr")
		evaluator.PushToken(kSqrFunctionToken);
	else if (mFunction == L"sqrt")
		evaluator.PushToken(kSqrtFunctionToken);
	else if (mFunction == L"exp")
		evaluator.PushToken(kExpFunctionToken);
	else if (mFunction == L"ln")
		evaluator.PushToken(kLnFunctionToken);
	else if (mFunction == L"log")
		evaluator.PushToken(kLogFunctionToken);
	else if (mFunction == L"round")
		evaluator.PushToken(kRoundFunctionToken);
	else if (mFunction == L"trunc")
		evaluator.PushToken(kTruncFunctionToken);
	else if (mFunction == L"abs")
		evaluator.PushToken(kAbsFunctionToken);
	else if (mFunction == L"random")
		evaluator.PushToken(kRandomFunctionToken);
	else
		DEBUGSTR("Bad function in CFunctionTree::OnCompile: ", mFunction);
}

#pragma mark -

// ===================================================================================
//	class CVariableTree
// ===================================================================================

//---------------------------------------------------------------
//
// CVariableTree::~CVariableTree
//
//---------------------------------------------------------------
CVariableTree::~CVariableTree()
{
}


//---------------------------------------------------------------
//
// CVariableTree::CVariableTree
//
//---------------------------------------------------------------
CVariableTree::CVariableTree(const SToken& token) : CColorTree(token)
{
	PRECONDITION(token.num == kVariableToken);
	PRECONDITION(token.text.length() == 1);

	mVariable = (char) toupper(token.text[0]);
	POSTCONDITION(mVariable == 'X' || mVariable == 'Y' || mVariable == 'Z');
}


//---------------------------------------------------------------
//
// CVariableTree::OnCompile
//
//---------------------------------------------------------------
void CVariableTree::OnCompile(CColorEvaluator& evaluator)
{
	PRECONDITION(this->GetNumChildren() == 0);
		
	if (mVariable == 'X')
		evaluator.PushToken(kXVariableToken);
	
	else if (mVariable == 'Y')
		evaluator.PushToken(kYVariableToken);
	
	else 
		evaluator.PushToken(kZVariableToken);
}

#pragma mark -

// ===================================================================================
//	class CLiteralTree
// ===================================================================================

//---------------------------------------------------------------
//
// CLiteralTree::~CLiteralTree
//
//---------------------------------------------------------------
CLiteralTree::~CLiteralTree()
{
}


//---------------------------------------------------------------
//
// CLiteralTree::CLiteralTree
//
//---------------------------------------------------------------
CLiteralTree::CLiteralTree(const SToken& token) : CColorTree(token)
{
	PRECONDITION(token.num == kNumberToken);

	mLiteral = StrToDouble(token.text);
}


//---------------------------------------------------------------
//
// CLiteralTree::OnCompile
//
//---------------------------------------------------------------
void CLiteralTree::OnCompile(CColorEvaluator& evaluator)
{
	PRECONDITION(this->GetNumChildren() == 0);
	
	evaluator.PushLiteral(mLiteral);
}

#pragma mark -

// ===================================================================================
//	class CParseColorExpr
// ===================================================================================

//---------------------------------------------------------------
//
// CParseColorExpr::~CParseColorExpr
//
//---------------------------------------------------------------
CParseColorExpr::~CParseColorExpr()
{
}


//---------------------------------------------------------------
//
// CParseColorExpr::CParseColorExpr
//
//---------------------------------------------------------------
CParseColorExpr::CParseColorExpr()
{
}


//---------------------------------------------------------------
//
// CParseColorExpr::OnParse
//
// Expression := Term (('+' | '-') Term)*
//
//---------------------------------------------------------------
XParseTree* CParseColorExpr::OnParse(XLexer* lexer)
{	
	XParseTree* tree = nil;

	CParseTerm term;
	XAutoPtr<XParseTree> lhs(term.CreateParseTree(lexer));
	
	SToken token = lexer->GetToken();
	while (token.num == kAddToken || token.num == kMinusToken) {
		lexer->ReadToken();

		XAutoPtr<XParseTree> rhs(term.CreateParseTree(lexer));

		if (token.num == kAddToken)
			tree = new CAddTree(token);
		else
			tree = new CSubtractTree(token);
		tree->AddChildToLeft(lhs.Release());
		tree->AddChildToRight(rhs.Release());
		
		lhs.Reset(tree);
		
		token = lexer->GetToken();
	}
	
	tree = lhs.Release();
	
	return tree;
}

#pragma mark -

// ===================================================================================
//	class CParseTerm
// ===================================================================================

//---------------------------------------------------------------
//
// CParseTerm::~CParseTerm
//
//---------------------------------------------------------------
CParseTerm::~CParseTerm()
{
}


//---------------------------------------------------------------
//
// CParseTerm::CParseTerm
//
//---------------------------------------------------------------
CParseTerm::CParseTerm()
{
}


//---------------------------------------------------------------
//
// CParseTerm::OnParse
//
// Term := Factor (('*' | '/') Factor)*
//
//---------------------------------------------------------------
XParseTree* CParseTerm::OnParse(XLexer* lexer)
{	
	XParseTree* tree = nil;

	CParseFactor factor;
	XAutoPtr<XParseTree> lhs(factor.CreateParseTree(lexer));
	
	SToken token = lexer->GetToken();
	while (token.num == kProductToken || token.num == kDivideToken) {
		lexer->ReadToken();

		XAutoPtr<XParseTree> rhs(factor.CreateParseTree(lexer));

		if (token.num == kProductToken)
			tree = new CMultiplyTree(token);
		else
			tree = new CDivideTree(token);
		tree->AddChildToLeft(lhs.Release());
		tree->AddChildToRight(rhs.Release());
		
		lhs.Reset(tree);
		
		token = lexer->GetToken();
	}
	
	tree = lhs.Release();
	
	return tree;
}

#pragma mark -

// ===================================================================================
//	class CParseFactor
// ===================================================================================

//---------------------------------------------------------------
//
// CParseFactor::~CParseFactor
//
//---------------------------------------------------------------
CParseFactor::~CParseFactor()
{
}


//---------------------------------------------------------------
//
// CParseFactor::CParseFactor
//
//---------------------------------------------------------------
CParseFactor::CParseFactor()
{
}


//---------------------------------------------------------------
//
// CParseFactor::OnParse
//
// Factor := Terminal ('^' Terminal)?
//
//---------------------------------------------------------------
XParseTree* CParseFactor::OnParse(XLexer* lexer)
{
	XParseTree* tree = nil;

	CParseTerminal terminal;
	XAutoPtr<XParseTree> lhs(terminal.CreateParseTree(lexer));
	
	SToken token = lexer->GetToken();
	if (token.num == kPowToken) {
		lexer->ReadToken();

		XAutoPtr<XParseTree> rhs(terminal.CreateParseTree(lexer));

		tree = new CPowTree(token);
		tree->AddChildToLeft(lhs.Release());
		tree->AddChildToRight(rhs.Release());
		
		lhs.Reset(tree);
		
		token = lexer->GetToken();
	}
	
	tree = lhs.Release();
	
	return tree;
}


#pragma mark -

// ===================================================================================
//	class CParseTerminal
// ===================================================================================

//---------------------------------------------------------------
//
// CParseTerminal::~CParseTerminal
//
//---------------------------------------------------------------
CParseTerminal::~CParseTerminal()
{
}


//---------------------------------------------------------------
//
// CParseTerminal::CParseTerminal
//
//---------------------------------------------------------------
CParseTerminal::CParseTerminal()
{
}


//---------------------------------------------------------------
//
// CParseTerminal::OnParse
//
// Terminal := Number | Variable | (Function '(' Expression ')') | ('(' Expression ')')
// Function := sin | cos | tan | sqr | sqrt | exp | ln | log | round | trunc | abs | tan | random
//
//---------------------------------------------------------------
XParseTree* CParseTerminal::OnParse(XLexer* lexer)
{
	XParseTree* tree = nil;
	
	SToken token = lexer->GetToken();
	wchar_t ch = (char) toupper(token.text[0]);
	
	if (isdigit(ch) || ch == '.' || ch == '-') {
		CParseNumber number;
		tree = number.CreateParseTree(lexer);
	
	} else if ((ch == 'X' || ch == 'Y' || ch == 'Z') && token.text.length() == 1) {
		CParseVariable variable;
		tree = variable.CreateParseTree(lexer);
	
	} else if (IsFunction(token.text)) {
		CParseFunction function;
		tree = function.CreateParseTree(lexer);
	
	} else if (ch == '(') {
		lexer->ReadToken();
	
		CParseColorExpr expression;
		tree = expression.CreateParseTree(lexer);

		token = lexer->GetToken();
		ch = token.text[0];
		
		if (ch == ')')
			lexer->ReadToken();
		else {
			delete tree;
			throw XParserException(lexer->GetScanner(), L"Expected a right parenthesis.");
		}

	} else
		throw XParserException(lexer->GetScanner(), L"Expected an number, variable, or left parenthesis.");
		
	return tree;
}

#pragma mark -

// ===================================================================================
//	class CParseFunction
// ===================================================================================

//---------------------------------------------------------------
//
// CParseFunction::~CParseFunction
//
//---------------------------------------------------------------
CParseFunction::~CParseFunction()
{
}


//---------------------------------------------------------------
//
// CParseFunction::CParseFunction
//
//---------------------------------------------------------------
CParseFunction::CParseFunction()
{
}


//---------------------------------------------------------------
//
// CParseFunction::OnParse
//
// Function := Name '(' Expression ')'
// Name := sin | cos | tan | sqr | sqrt | exp | ln | log | round | trunc | abs | tan | random
//
//---------------------------------------------------------------
XParseTree* CParseFunction::OnParse(XLexer* lexer)
{
	SToken token = lexer->GetToken();
	XAutoPtr<XParseTree> tree(new CFunctionTree(token));
	
	lexer->ReadToken();
	token = lexer->GetToken();

	if (token.num == kLeftParensToken) {
		lexer->ReadToken();

		CParseColorExpr expression;
		XParseTree* child = expression.CreateParseTree(lexer);

		token = lexer->GetToken();
		
		if (token.num == kRightParensToken)
			lexer->ReadToken();
		else {
			delete child;
			throw XParserException(lexer->GetScanner(), L"Expected a right parenthesis.");
		}
		
		tree->AddChildToLeft(child);
					
	} else
		throw XParserException(lexer->GetScanner(), L"Expected a left parenthesis.");
		
	return tree.Release();
}

#pragma mark -

// ===================================================================================
//	class CParseVariable
// ===================================================================================

//---------------------------------------------------------------
//
// CParseVariable::~CParseVariable
//
//---------------------------------------------------------------
CParseVariable::~CParseVariable()
{
}


//---------------------------------------------------------------
//
// CParseVariable::CParseVariable
//
//---------------------------------------------------------------
CParseVariable::CParseVariable()
{
}


//---------------------------------------------------------------
//
// CParseVariable::OnCreateTree
//
// Variable := [xyzXYZ]
//
//---------------------------------------------------------------
XParseTree* CParseVariable::OnCreateTree(XLexer* lexer)
{
	SToken token = lexer->GetToken();
	lexer->ReadToken();
	
	CVariableTree* tree = new CVariableTree(token);
	
	return tree;
}

#pragma mark -

// ===================================================================================
//	class CParseNumber
// ===================================================================================

//---------------------------------------------------------------
//
// CParseNumber::~CParseNumber
//
//---------------------------------------------------------------
CParseNumber::~CParseNumber()
{
}


//---------------------------------------------------------------
//
// CParseNumber::CParseNumber
//
//---------------------------------------------------------------
CParseNumber::CParseNumber()
{
}


//---------------------------------------------------------------
//
// CParseNumber::OnCreateTree
//
// Number := -? ((Digit+ '.' Digit*) | (Digit+) | ('.' Digit+)) (('e' | 'E') ('+' | '-')? Digit+)?
// Digit  := '0'..'9'
//
//---------------------------------------------------------------
XParseTree* CParseNumber::OnCreateTree(XLexer* lexer)
{
	SToken token = lexer->GetToken();
	lexer->ReadToken();
	
	CLiteralTree* tree = new CLiteralTree(token);
	
	return tree;
}


