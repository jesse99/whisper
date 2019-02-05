/*
 *  File:		XParserTest.h
 *  Summary:	A simple expression parser for use by XParser's unit test.
 *  Written by:	Jesse Jones
 *
 *	Classes:	ZExpressionTree	  - XParseTree that contains an integer value.
 *					ZAddTree	  - Expression tree that sums the values of its children.
 *					ZSubtractTree - Expression tree that subtracts the values of its children.
 *					ZMultiplyTree - Expression tree that multiples the values of its children.
 *					ZDivideTree	  - Expression tree that divides the values of its children.
 *					ZLiteralTree  - Expression tree that contains a (constant) integer.
 *				
 *				ZParseExpression  - XParseConstruct subclass used to parse term plus term.
 *				ZParseTerm		  - XParseConstruct subclass used to parse factor times factor.
 *				ZParseFactor	  - XParseChoice subclass used to parse integers or sub-expressions.
 *				ZParseNumber	  - XParseTerminal subclass used to parse integers.
 *
 *  Copyright © 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XParserTest.h,v $
 *		Revision 1.2  2000/11/09 12:46:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	12/17/98	JDJ		Created (from Raven)
 */

#include <XParser.h>
#include <XUnitTest.h>

#if DEBUG

namespace Whisper {


// ===================================================================================
//	class ZExpressionTree
// ===================================================================================
class ZExpressionTree : public XParseTree {

	typedef XParseTree Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~ZExpressionTree();

						ZExpressionTree(const SToken& token);
						
//-----------------------------------
//	New API
//
public:
	virtual int32 		Evaluate();
	
protected:
	virtual int32 		OnEvaluate() = 0;

//-----------------------------------
//	Member Data
//
protected:
	int32 	mValue;
	bool	mComputed;
};


// ===================================================================================
//	class ZAddTree
// ===================================================================================
class ZAddTree : public ZExpressionTree {

	typedef ZExpressionTree Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~ZAddTree();

						ZAddTree(const SToken& token);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual int32 		OnEvaluate();
};


// ===================================================================================
//	class ZSubtractTree
// ===================================================================================
class ZSubtractTree : public ZExpressionTree {

	typedef ZExpressionTree Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~ZSubtractTree();

						ZSubtractTree(const SToken& token);
						
protected:
	virtual int32 		OnEvaluate();
};


// ===================================================================================
//	class ZMultiplyTree
// ===================================================================================
class ZMultiplyTree : public ZExpressionTree {

	typedef ZExpressionTree Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~ZMultiplyTree();

						ZMultiplyTree(const SToken& token);
						
protected:
	virtual int32 		OnEvaluate();
};


// ===================================================================================
//	class ZDivideTree
// ===================================================================================
class ZDivideTree : public ZExpressionTree {

	typedef ZExpressionTree Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~ZDivideTree();

						ZDivideTree(const SToken& token);
						
protected:
	virtual int32 		OnEvaluate();
};


// ===================================================================================
//	class ZLiteralTree
// ===================================================================================
class ZLiteralTree : public ZExpressionTree {

	typedef ZExpressionTree Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~ZLiteralTree();

						ZLiteralTree(const SToken& token, int32 value);
						
protected:
	virtual int32 		OnEvaluate();

//-----------------------------------
//	Member data
//
protected:
	int32 	mLiteral;
};


// ===================================================================================
//	class ZParseExpression
// ===================================================================================
class ZParseExpression : public XParseOperand {

	typedef XParseOperand Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~ZParseExpression();

						ZParseExpression();

//-----------------------------------
//	Inherited API
//
protected:
	virtual bool 		OnIsOperand(const SToken& token);

	virtual XParseTree* OnCreateOperand(const SToken& token);

	virtual XParseTree* OnParse(XLexer* lexer);
};


// ===================================================================================
//	class ZParseTerm
// ===================================================================================
class ZParseTerm : public XParseOperand {

	typedef XParseOperand Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~ZParseTerm();

						ZParseTerm();

//-----------------------------------
//	Inherited API
//
protected:
	virtual bool 		OnIsOperand(const SToken& token);

	virtual XParseTree* OnCreateOperand(const SToken& token);

	virtual XParseTree* OnParse(XLexer* lexer);
};


// ===================================================================================
//	class ZParseFactor
// ===================================================================================
class ZParseFactor : public XParseChoice {

	typedef XParseChoice Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~ZParseFactor();

						ZParseFactor();

//-----------------------------------
//	Inherited API
//
protected:
	virtual XParseTree* OnParse(XLexer* lexer);
};


// ===================================================================================
//	class ZParseNumber
// ===================================================================================
class ZParseNumber : public XParseTerminal {

	typedef XParseTerminal Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~ZParseNumber();

						ZParseNumber();

//-----------------------------------
//	Inherited API
//
protected:
	virtual XParseTree* OnCreateTree(XLexer* lexer);
};


// ===================================================================================
//	class XParserUnitTest
// ===================================================================================	
class XParserUnitTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XParserUnitTest();
	
						XParserUnitTest();
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnTest();

//-----------------------------------
//	Internal API
//
protected:
			void 		DoParserTest(const std::wstring& text, int32 result);
};


}	// namespace Whisper

#endif	// DEBUG

