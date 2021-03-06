/*
 *  File:		XParser.h
 *  Summary:	Text parsing class modeled after those in Eiffel's base libraries.
 *  Written by:	Jesse Jones
 *
 *	Abstract:	The file provides classes to help you implement custom parsers. When the
 *				parsing is complete you will have a tree of XParseTree's. Typically each
 *				node in the tree will be a subclass of XParseTree. To do the parsing you
 *				can specialize XParseConstruct or (more often) one of the subclasses
 *				listed below.
 *
 *	Usage:		XParserTest.h contains code to build a parse tree for simple expressions.
 *
 *	Classes:	XParseTree		 - Base class for subtrees of a parse tree.
 *				XParseConstruct	 - Abstract base class for classes used to build a parse tree.
 *				XParseAggregate	 - Abstract base class for parsing "token1 token2 ...".
 *				XParseChoice	 - Abstract base class for parsing "token1 | token2 ...".
 *				XParseRepetition - Abstract base class for parsing "(separator token)*".
 *				XParseTerminal	 - Concrete class for parsing "token".
 *
 *  Copyright � 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XParser.h,v $
 *		Revision 1.4  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/11 01:30:54  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:45:42  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	10/23/99	JDJ		Renamed TParseOperand XParseOperand.
 *		 <2>	 6/09/99	JDJ		Fixed spelling of mSeparator.
 *		 <1>	12/17/98	JDJ		Created (from Raven)
 */

#pragma once

#include <XLexer.h>
#include <XTree.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XLexer;


// ===================================================================================
//	class XParseTree
//!		Base class for subtrees of a parse tree.
// ===================================================================================
class PARSE_EXPORT XParseTree : public XTree<XScannerPos> {

	typedef XTree<XScannerPos> Inherited;
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual						~XParseTree();

								XParseTree(const SToken& token);

//-----------------------------------
//	New API
//
public:
			XScannerPos 		GetPosition() const 		{return this->GetNode();}

			TokenNum 			GetToken() const			{return mToken;}

//-----------------------------------
//	Member Data
//
protected:
	TokenNum	mToken;
};


// ===================================================================================
//	class XParseConstruct
//!		Abstract base class for classes used to build a parse tree.
// 		Most of these functions will throw exceptions. Any exception thrown will be 
//		mapped to XParserException.
// ===================================================================================
class PARSE_EXPORT XParseConstruct {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XParseConstruct();

						XParseConstruct();

//-----------------------------------
//	API
//
public:
			XParseTree* CreateParseTree(XLexer* lexer)		{return this->HandleParse(lexer);}
	
	virtual XParseTree* HandleParse(XLexer* lexer);
	
protected:
	virtual XParseTree* OnParse(XLexer* lexer) = 0;
};


// ===================================================================================
//	class XParseAggregate
//!		Abstract base class for parsing "token1 token2 ...".
// ===================================================================================
class PARSE_EXPORT XParseAggregate : public XParseConstruct {

	typedef XParseConstruct Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XParseAggregate();

						XParseAggregate();
						/**< Parses text of the form "token1 token2 token3 ...". */

//-----------------------------------
//	Inherited API
//
public:
	virtual XParseTree* HandleParse(XLexer* lexer);
	
protected:
	virtual XParseTree* OnParse(XLexer* lexer) = 0;
						/**< Called for each token in the list. */

//-----------------------------------
//	Internal API
//
protected:
	virtual bool 		OnCanParse(XLexer* lexer) const = 0;
						/**< Return true if lexer points to a valid token. */
						
	virtual XParseTree* OnCreateTop(XLexer* lexer) = 0;
						/**< This will be the tree returned by HandleParse. The
						trees returned by OnParse will be immediate children
						of this tree. */
};


// ===================================================================================
//	class XParseRepetition
//!		Abstract base class for parsing "(separator token)*".
// ===================================================================================
class PARSE_EXPORT XParseRepetition : public XParseConstruct {

	typedef XParseConstruct Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XParseRepetition();

						XParseRepetition(TokenNum separator);
						/**< Parses text of the form "(separator token)*".
						This is useful for things like comma delimited lists. */
				
//-----------------------------------
//	Inherited API
//
public:
	virtual XParseTree* HandleParse(XLexer* lexer);

protected:
	virtual XParseTree* OnParse(XLexer* lexer) = 0;
						/**< Called for each token in the list (but not the separators). */

//-----------------------------------
//	Internal API
//
protected:
	virtual XParseTree* OnCreateTop(XLexer* lexer) = 0;
						/**< This will be the tree returned by HandleParse. The
						trees returned by OnParse will be immediate children
						of this tree. Separators are stripped out. */
						
//-----------------------------------
//	Member Data
//
protected:
	TokenNum	mSeparator;
};


// ===================================================================================
//	class XParseOperand
// ===================================================================================
class PARSE_EXPORT XParseOperand : public XParseConstruct {

	typedef XParseConstruct Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XParseOperand();

						XParseOperand();
						/**< Parses text of the form "token (operand token)*".
						This is useful when parsing expressions. */
				
//-----------------------------------
//	Inherited API
//
public:
	virtual XParseTree* HandleParse(XLexer* lexer);
						/**< Returns either a one node tree whose value is token or a
						binary tree with the root being an operand and the leafs
						tokens. */

protected:
	virtual XParseTree* OnParse(XLexer* lexer) = 0;
						/**< Called for each token in the list (but not the operands). */

//-----------------------------------
//	Internal API
//
protected:
	virtual bool 		OnIsOperand(const SToken& token) = 0;
						/**< Return true if the token is an operand. */

	virtual XParseTree* OnCreateOperand(const SToken& token) = 0;
						/**< Creates a tree for an operand. */

	virtual	bool 		OnFoundToken(XLexer* lexer, SToken& token);
						/**< You should rarely have to override this. */
};


// ===================================================================================
//	class XParseChoice
//!		Abstract base class for parsing "token1 | token2 ...".
// ===================================================================================
class PARSE_EXPORT XParseChoice : public XParseConstruct {

	typedef XParseConstruct Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XParseChoice();

						XParseChoice();
						/**< Parses text of the form "token1 | token2 | token3 ...". */
};


// ===================================================================================
//	class XParseTerminal
//!		Concrete class for parsing "token".
// ===================================================================================
class PARSE_EXPORT XParseTerminal : public XParseConstruct {

	typedef XParseConstruct Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XParseTerminal();

						XParseTerminal();
						/**< Parses text of the form "token1". */

//-----------------------------------
//	Inherited API
//
protected:
	virtual XParseTree* OnParse(XLexer* lexer);

//-----------------------------------
//	Internal API
//
protected:
	virtual XParseTree* OnCreateTree(XLexer* lexer);
						/**< Override this if you want to return a XParseTree subclass. */
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
