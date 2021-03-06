/*
 *  File:		XParser.cpp
 *  Summary:	Text parsing class modeled after those in Eiffel's base libraries.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XParser.cpp,v $
 *		Revision 1.3  2001/04/27 04:35:02  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:45:37  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XParser.h>

#include <XAutoPtr.h>
#include <XLexer.h>

namespace Whisper {


// ===================================================================================
//	class XParseTree
// ===================================================================================

//---------------------------------------------------------------
//
// XParseTree::~XParseTree
//
//---------------------------------------------------------------
XParseTree::~XParseTree()
{
}


//---------------------------------------------------------------
//
// XParseTree::XParseTree
//
//---------------------------------------------------------------
XParseTree::XParseTree(const SToken& token) : XTree<XScannerPos>(token.pos)
{
	mToken = token.num;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XParseConstruct
// ===================================================================================

//---------------------------------------------------------------
//
// XParseConstruct::~XParseConstruct
//
//---------------------------------------------------------------
XParseConstruct::~XParseConstruct()
{
}


//---------------------------------------------------------------
//
// XParseConstruct::XParseConstruct
//
//---------------------------------------------------------------
XParseConstruct::XParseConstruct()
{
}


//---------------------------------------------------------------
//
// XParseConstruct::HandleParse
//
//---------------------------------------------------------------
XParseTree* XParseConstruct::HandleParse(XLexer* lexer)
{
	PRECONDITION(lexer != nil);
	
	XParseTree* tree = this->OnParse(lexer);
	
	return tree;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XParseAggregate
// ===================================================================================

//---------------------------------------------------------------
//
// XParseAggregate::~XParseAggregate
//
//---------------------------------------------------------------
XParseAggregate::~XParseAggregate()
{
}


//---------------------------------------------------------------
//
// XParseAggregate::XParseAggregate
//
//---------------------------------------------------------------
XParseAggregate::XParseAggregate()
{
}


//---------------------------------------------------------------
//
// XParseAggregate::HandleParse
//
//---------------------------------------------------------------
XParseTree* XParseAggregate::HandleParse(XLexer* lexer)
{
	PRECONDITION(lexer != nil);
	
	XAutoPtr<XParseTree> tree(this->OnCreateTop(lexer));
		
	while (!lexer->AtEnd() && this->OnCanParse(lexer)) {		
		XParseTree* subtree = this->OnParse(lexer);	
		tree->AddChildToRight(subtree);
	}
	
	return tree.Release();
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XParseChoice
// ===================================================================================

//---------------------------------------------------------------
//
// XParseChoice::~XParseChoice
//
//---------------------------------------------------------------
XParseChoice::~XParseChoice()
{
}


//---------------------------------------------------------------
//
// XParseChoice::XParseChoice
//
//---------------------------------------------------------------
XParseChoice::XParseChoice()
{
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XParseRepetition
// ===================================================================================

//---------------------------------------------------------------
//
// XParseRepetition::~XParseRepetition
//
//---------------------------------------------------------------
XParseRepetition::~XParseRepetition()
{
}


//---------------------------------------------------------------
//
// XParseRepetition::XParseRepetition
//
//---------------------------------------------------------------
XParseRepetition::XParseRepetition(TokenNum separator)
{
	mSeparator = separator;
}


//---------------------------------------------------------------
//
// XParseRepetition::HandleParse
//
//---------------------------------------------------------------
XParseTree* XParseRepetition::HandleParse(XLexer* lexer)
{
	PRECONDITION(lexer != nil);
	
	XAutoPtr<XParseTree> tree(this->OnCreateTop(lexer));
		
	SToken token = lexer->GetToken();
	
	while (token.num == mSeparator) {		
		lexer->ReadToken();
		
		XParseTree* subtree = this->OnParse(lexer);	
		tree->AddChildToRight(subtree);

		token = lexer->GetToken();
	}
	
	return tree.Release();
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XParseOperand
// ===================================================================================

//---------------------------------------------------------------
//
// XParseOperand::~XParseOperand
//
//---------------------------------------------------------------
XParseOperand::~XParseOperand()
{
}


//---------------------------------------------------------------
//
// XParseOperand::XParseOperand
//
//---------------------------------------------------------------
XParseOperand::XParseOperand()
{
}


//---------------------------------------------------------------
//
// XParseOperand::HandleParse
//
//---------------------------------------------------------------
XParseTree* XParseOperand::HandleParse(XLexer* lexer)
{
	XParseTree* tree = nil;

	XAutoPtr<XParseTree> lhs(this->OnParse(lexer));
	
	SToken token;
	while (this->OnFoundToken(lexer, token)) {
		lexer->SkipToken(token);
		lexer->ReadToken();

		XAutoPtr<XParseTree> rhs(this->OnParse(lexer));

		tree = this->OnCreateOperand(token);
		tree->AddChildToLeft(lhs.Release());
		tree->AddChildToRight(rhs.Release());
		
		lhs.Reset(tree);
	}
	
	tree = lhs.Release();
	
	return tree;
}


//---------------------------------------------------------------
//
// XParseOperand::OnFoundToken
//
// We do this so that stuff like "i-1" parses as minus sign
// followed by a literal instead of a negative literal.
//
//---------------------------------------------------------------
bool XParseOperand::OnFoundToken(XLexer* lexer, SToken& token)
{
	bool found = false;
	
	uint32 count = lexer->GetNumTokens();
	for (uint32 index = 0; index < count && !found; ++index) {
		token = lexer->GetToken(index);		// longer tokens are returned first
		found = this->OnIsOperand(token);
	}
		
	return found;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XParseTerminal
// ===================================================================================

//---------------------------------------------------------------
//
// XParseTerminal::~XParseTerminal
//
//---------------------------------------------------------------
XParseTerminal::~XParseTerminal()
{
}


//---------------------------------------------------------------
//
// XParseTerminal::XParseTerminal
//
//---------------------------------------------------------------
XParseTerminal::XParseTerminal()
{
}


//---------------------------------------------------------------
//
// XParseTerminal::OnParse 
//
//---------------------------------------------------------------
XParseTree* XParseTerminal::OnParse(XLexer* lexer)
{	
	XParseTree* tree = this->OnCreateTree(lexer);
		
	return tree;
}


//---------------------------------------------------------------
//
// XParseTerminal::OnCreateTree 
//
//---------------------------------------------------------------
XParseTree* XParseTerminal::OnCreateTree(XLexer* lexer)
{
	SToken token = lexer->GetToken();
	lexer->ReadToken();
	
	XParseTree* tree = new XParseTree(token);
	
	return tree;
}


}	// namespace Whisper
