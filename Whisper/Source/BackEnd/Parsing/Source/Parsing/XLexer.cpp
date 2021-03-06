/*
 *  File:		XLexer.cpp
 *  Summary:	A class used to extract tokens from text (modeled after those in Eiffel's 
 *				base libraries).
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XLexer.cpp,v $
 *		Revision 1.5  2001/04/27 04:33:55  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/03/06 07:32:54  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.3  2000/11/20 05:43:22  jesjones
 *		SToken is no longer a nested class.
 *		
 *		Revision 1.2  2000/11/09 12:41:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XLexer.h>

#include <XDFA.h>
#include <XStringUtils.h>

namespace Whisper {


SToken::SToken(const SToken& rhs) : num(rhs.num), text(rhs.text), pos(rhs.pos) {}

// ===================================================================================
//	class XLexer
// ===================================================================================

//---------------------------------------------------------------
//
// XLexer::~XLexer
//
//---------------------------------------------------------------
XLexer::~XLexer()
{
	delete mScanner;
	delete mGrammar;
}


//---------------------------------------------------------------
//
// XLexer::XLexer
//
//---------------------------------------------------------------
XLexer::XLexer(XScanner* takeScanner, XLexerGrammar* takeGrammar, bool ignoreCase)
{
	PRECONDITION(takeScanner != nil);
	PRECONDITION(takeGrammar != nil);
	
	mScanner = takeScanner;
	mGrammar = takeGrammar;
	
	mIgnoreCase = ignoreCase;
}


//---------------------------------------------------------------
//
// XLexer::Analyze
//
//---------------------------------------------------------------
void XLexer::Analyze(const std::wstring& text)
{	
	mScanner->SetText(text);
	
	mTokens.clear();
}


//---------------------------------------------------------------
//
// XLexer::AtEnd
//
//---------------------------------------------------------------
bool XLexer::AtEnd() const
{
	bool atEnd = false;
	
	if (mScanner->AtEnd())
		if (mTokens.empty())
			atEnd = true;
	
	return atEnd;
}


//---------------------------------------------------------------
//
// XLexer::ReadToken
//
//---------------------------------------------------------------
void XLexer::ReadToken()
{
	mTokens.clear();

	mScanner->SkipWhiteSpace();
	
	XDFA* exprDfa = mGrammar->GetExprDFA();
	exprDfa->Reset();
		
	XDFA* strDfa = mGrammar->GetStrDFA();
	strDfa->Reset();
		
	XScannerPos startPos = mScanner->GetPosition();
	bool found = true;		
	while (!mScanner->AtEnd() && found)
		found = this->DoReadToken(exprDfa, strDfa, startPos);
		
	if (!mScanner->AtEnd() && mTokens.empty()) {
		mScanner->SetPosition(startPos);
		mScanner->Advance();
	}
}


//---------------------------------------------------------------
//
// XLexer::ReadFixedToken
//
//---------------------------------------------------------------
void XLexer::ReadFixedToken(uint32 maxChars)
{
	mTokens.clear();

	mScanner->SkipWhiteSpace();
	
	XDFA* exprDfa = mGrammar->GetExprDFA();
	exprDfa->Reset();
		
	XDFA* strDfa = mGrammar->GetStrDFA();
	strDfa->Reset();
		
	XScannerPos startPos = mScanner->GetPosition();
	bool found = true;		
	while (!mScanner->AtEnd() && found) {
		XScannerPos oldPos = mScanner->GetPosition();
		found = this->DoReadToken(exprDfa, strDfa, startPos);
		
		if (found) {
			const SToken& token = mTokens.back();
			if (token.text.size() > maxChars) {
				mTokens.pop_back();
				mScanner->SetPosition(oldPos);
				found = false;
			}
		}
	}
		
	if (!mScanner->AtEnd() && mTokens.empty()) {
		mScanner->SetPosition(startPos);
		mScanner->Advance();
	}
}


//---------------------------------------------------------------
//
// XLexer::ReadShortestToken
//
//---------------------------------------------------------------
void XLexer::ReadShortestToken()
{
	mTokens.clear();

	mScanner->SkipWhiteSpace();
	
	XDFA* exprDfa = mGrammar->GetExprDFA();
	exprDfa->Reset();
		
	XDFA* strDfa = mGrammar->GetStrDFA();
	strDfa->Reset();
		
	XScannerPos startPos = mScanner->GetPosition();
	if (!mScanner->AtEnd())
		this->DoReadToken(exprDfa, strDfa, startPos);
		
	if (!mScanner->AtEnd() && mTokens.empty()) {
		mScanner->SetPosition(startPos);
		mScanner->Advance();
	}
}


//---------------------------------------------------------------
//
// XLexer::SkipToken
//
//---------------------------------------------------------------
void XLexer::SkipToken(const SToken& token)
{
	mScanner->SetPosition(token.pos);
}


//---------------------------------------------------------------
//
// XLexer::GetToken
//
//---------------------------------------------------------------
SToken XLexer::GetToken(uint32 index) const
{	
	SToken token;
	
	if (!mTokens.empty())
		token = mTokens[mTokens.size() - index - 1];
	
	else {
		token.num  = kNoToken;
		token.text = std::wstring(1, '\0');
		token.pos  = mScanner->GetPosition();
	}
	
	return token;
}


//---------------------------------------------------------------
//
// XLexer::FindToken
//
//---------------------------------------------------------------
bool XLexer::FindToken(TokenNum num, SToken& token) const
{
	bool found = false;
	
	for (uint32 index = 0; index < mTokens.size() && !found; ++index) {
		const SToken& candidate = mTokens[index];
		
		if (num == candidate.num) {
			token = candidate;
			found = true;
		}
	}
	
	return found;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XLexer::DoReadToken
//
//---------------------------------------------------------------
bool XLexer::DoReadToken(XDFA* exprDfa, XDFA* strDfa, const XScannerPos& startPos)
{
	PRECONDITION(exprDfa != nil);
	PRECONDITION(strDfa != nil);
		
	XScannerPos oldPos = mScanner->GetPosition();

	bool strHalted  = false;				// can move from a halt state to another halt state so we'll try to eat at least one character
	bool exprHalted = false;
	
	while ((!exprDfa->IsBlocked() || !strDfa->IsBlocked()) && !strHalted && !exprHalted) {
		wchar_t ch = mScanner->Peek();
		if (mIgnoreCase)
			ch = ConvertToLowerCase(ch);
		mScanner->Advance();
		
		if (!strDfa->IsBlocked()) {
			strDfa->NextState(ch);
			strHalted = strDfa->CanHalt();
		}
		
		if (!exprDfa->IsBlocked()) {
			exprDfa->NextState(ch);
			exprHalted = exprDfa->CanHalt();
		}
	}
	
	if (strHalted || exprHalted) {		
		XScannerPos newPos = mScanner->GetPosition();
		uint32 start = startPos.GetIndex();
		uint32 count = newPos.GetIndex() - start;
		
		SToken token;
		int haltState;
		
		token.text = mScanner->GetText().substr(start, count);
		token.pos  = newPos;
			
		if (exprHalted) {
			haltState = exprDfa->GetState();
			token.num = mGrammar->GetExprToken(haltState);

			mTokens.push_back(token);
		}

		if (strHalted) {					
			haltState = strDfa->GetState();
			token.num = mGrammar->GetStrToken(haltState);

			mTokens.push_back(token);
		}
		
	} else
		mScanner->SetPosition(oldPos);
		
	return strHalted || exprHalted;
}


}		// namespace Whisper

