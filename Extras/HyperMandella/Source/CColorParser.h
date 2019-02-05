/*
 *  File:		CColorParser.h
 *  Summary:	Classes that build an expression tree for the formula's used to compute fractal colors.
 *  Written by:	Jesse Jones
 *
 *	Classes:	CColorTree  	  - TParseTree that evaluates to a double.
 *					CPowTree	  - Expression tree that raises it's left child to the power of it's right child.
 *					CAddTree	  - Expression tree that sums the values of its children.
 *					CSubtractTree - Expression tree that subtracts the values of its children.
 *					CMultiplyTree - Expression tree that multiples the values of its children.
 *					CDivideTree	  - Expression tree that divides the values of its children.
 *					CFunctionTree - Expression tree that evaulates a function (eg cos, sqrt, etc).
 *					CVariableTree - Expression tree that references a variable (x, y, or z).
 *					CLiteralTree  - Expression tree that contains a (constant) double.
 *				
 *				CParseColorExpr	  - XParseConstruct subclass used to parse term plus term.
 *				CParseTerm		  - XParseConstruct subclass used to parse factor times factor.
 *				CParseFactor	  - XParseChoice subclass used to parse terminal raised to terminal.
 *				CParseTerminal	  - XParseChoice subclass used to parse doubles, variables, function calls, or sub-expressions.
 *				CParseFunction	  - XParseChoice subclass used to function calls.
 *				CParseVariable	  - XParseTerminal subclass used to parse variables.
 *				CParseNumber	  - XParseTerminal subclass used to parse doubles.
 *
 *  Copyright © 1998 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CColorParser.h,v $
 *		Revision 1.1  2000/12/28 02:48:40  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:53:29  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 2/08/98	JDJ		Created (from ZParserTest)
 */

#include <X3DPrimitives.h>
#include <XParser.h>

using namespace Whisper;
using namespace std;


//-----------------------------------
//	Forward References
//
class CColorEvaluator;


//-----------------------------------
//	Constants
//
const TokenNum kPowToken	  		= '^';			// used while parsing
const TokenNum kAddToken      		= '+';
const TokenNum kMinusToken    		= '-';
const TokenNum kProductToken  		= '*';
const TokenNum kDivideToken   		= '/';
const TokenNum kLeftParensToken		= '(';
const TokenNum kRightParensToken	= ')';
const TokenNum kVariableToken 		= 'v';
const TokenNum kNumberToken   		= '#';
const TokenNum kFunctionToken 		= 'f';

const TokenNum kXVariableToken 		= 'xVar';		// used when evaluating
const TokenNum kYVariableToken		= 'yVar';
const TokenNum kZVariableToken 		= 'zVar';

const TokenNum kSinFunctionToken	= 'sinf';
const TokenNum kCosFunctionToken	= 'cosf';
const TokenNum kTanFunctionToken	= 'tanf';
const TokenNum kSqrFunctionToken	= 'sqr ';
const TokenNum kSqrtFunctionToken	= 'sqrt';
const TokenNum kExpFunctionToken	= 'expf';
const TokenNum kLnFunctionToken		= 'lnf ';
const TokenNum kLogFunctionToken	= 'logf';
const TokenNum kRoundFunctionToken	= 'rndf';
const TokenNum kTruncFunctionToken	= 'truc';
const TokenNum kAbsFunctionToken	= 'absf';
const TokenNum kRandomFunctionToken	= 'rand';


// ===================================================================================
//	class CColorTree
// ===================================================================================
class CColorTree : public XParseTree {

	typedef XParseTree Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CColorTree();

						CColorTree(const SToken& token);
						
	static 	CColorTree* Create(const std::wstring& text);
						
//-----------------------------------
//	New API
//
public:
	virtual void 		Compile(CColorEvaluator& evaluator);
	
protected:
	virtual void 		OnCompile(CColorEvaluator& evaluator) = 0;

	static XLexerGrammar* DoCreateGrammar();
};


// ===================================================================================
//	class CAddTree
// ===================================================================================
class CAddTree : public CColorTree {

	typedef CColorTree Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CAddTree();

						CAddTree(const SToken& token);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnCompile(CColorEvaluator& evaluator);
};


// ===================================================================================
//	class CSubtractTree
// ===================================================================================
class CSubtractTree : public CColorTree {

	typedef CColorTree Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CSubtractTree();

						CSubtractTree(const SToken& token);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnCompile(CColorEvaluator& evaluator);
};


// ===================================================================================
//	class CMultiplyTree
// ===================================================================================
class CMultiplyTree : public CColorTree {

	typedef CColorTree Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CMultiplyTree();

						CMultiplyTree(const SToken& token);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnCompile(CColorEvaluator& evaluator);
};


// ===================================================================================
//	class CDivideTree
// ===================================================================================
class CDivideTree : public CColorTree {

	typedef CColorTree Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CDivideTree();

						CDivideTree(const SToken& token);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnCompile(CColorEvaluator& evaluator);
};


// ===================================================================================
//	class CPowTree
// ===================================================================================
class CPowTree : public CColorTree {

	typedef CColorTree Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CPowTree();

						CPowTree(const SToken& token);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnCompile(CColorEvaluator& evaluator);
};


// ===================================================================================
//	class CFunctionTree
// ===================================================================================
class CFunctionTree : public CColorTree {

	typedef CColorTree Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CFunctionTree();

						CFunctionTree(const SToken& token);
						
//-----------------------------------
//	New API
//
public:
			std::wstring GetFunction() const					{return mFunction;}

//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnCompile(CColorEvaluator& evaluator);

//-----------------------------------
//	Member Data
//
protected:
	std::wstring	mFunction;
};


// ===================================================================================
//	class CVariableTree
// ===================================================================================
class CVariableTree : public CColorTree {

	typedef CColorTree Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CVariableTree();

						CVariableTree(const SToken& token);
						
//-----------------------------------
//	New API
//
public:
			char 		GetVariable() const					{return mVariable;}
						// 'X', 'Y', or 'Z'

//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnCompile(CColorEvaluator& evaluator);

//-----------------------------------
//	Member Data
//
protected:
	char mVariable;
};


// ===================================================================================
//	class CLiteralTree
// ===================================================================================
class CLiteralTree : public CColorTree {

	typedef CColorTree Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CLiteralTree();

						CLiteralTree(const SToken& token);
						
//-----------------------------------
//	New API
//
public:
			double 		GetNumber() const					{return mLiteral;}

//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnCompile(CColorEvaluator& evaluator);

//-----------------------------------
//	Member Data
//
protected:
	double 	mLiteral;
};


// ===================================================================================
//	class CParseColorExpr
// ===================================================================================
class CParseColorExpr : public XParseConstruct {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CParseColorExpr();

						CParseColorExpr();

//-----------------------------------
//	Inherited API
//
protected:
	virtual XParseTree* OnParse(XLexer* lexer);
};


// ===================================================================================
//	class CParseTerm
// ===================================================================================
class CParseTerm : public XParseConstruct {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CParseTerm();

						CParseTerm();

//-----------------------------------
//	Inherited API
//
protected:
	virtual XParseTree* OnParse(XLexer* lexer);
};


// ===================================================================================
//	class CParseFactor
// ===================================================================================
class CParseFactor : public XParseChoice {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CParseFactor();

						CParseFactor();

//-----------------------------------
//	Inherited API
//
protected:
	virtual XParseTree* OnParse(XLexer* lexer);
};


// ===================================================================================
//	class CParseTerminal
// ===================================================================================
class CParseTerminal : public XParseChoice {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CParseTerminal();

						CParseTerminal();

//-----------------------------------
//	Inherited API
//
protected:
	virtual XParseTree* OnParse(XLexer* lexer);
};


// ===================================================================================
//	class CParseFunction
// ===================================================================================
class CParseFunction : public XParseChoice {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CParseFunction();

						CParseFunction();

//-----------------------------------
//	Inherited API
//
protected:
	virtual XParseTree* OnParse(XLexer* lexer);
};


// ===================================================================================
//	class CParseVariable
// ===================================================================================
class CParseVariable : public XParseTerminal {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CParseVariable();

						CParseVariable();

//-----------------------------------
//	Inherited API
//
protected:
	virtual XParseTree* OnCreateTree(XLexer* lexer);
};


// ===================================================================================
//	class CParseNumber
// ===================================================================================
class CParseNumber : public XParseTerminal {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CParseNumber();

						CParseNumber();

//-----------------------------------
//	Inherited API
//
protected:
	virtual XParseTree* OnCreateTree(XLexer* lexer);
};



