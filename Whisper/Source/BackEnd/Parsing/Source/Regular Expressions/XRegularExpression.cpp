/*
 *  File:		XRegularExpression.cpp
 *  Summary:	A regular expression class that works like CodeWarrior's find dialog.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRegularExpression.cpp,v $
 *		Revision 1.3  2001/04/27 04:35:43  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:49:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 3/12/00	JDJ		DoParseBracketChars throws for stuff like [C-A]. 
 *		 <2>	 2/04/99	JDJ		XRegularExpression constants are XRegularExpression 
 *									objects (instead of XStrings).
 *		 <1>	12/17/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XRegularExpression.h>

#include <cctype>
#include <list>

#include <XDebug.h>
#include <XDFA.h>
#include <XExceptions.h>
#include <XNDFA.h>
#include <XNumbers.h>

namespace Whisper {


// ===================================================================================
//	Constants
// ===================================================================================
const XRegularExpression kWhiteSpaceExpr(L"[ \n\t\r\v\f]+");
const XRegularExpression kIntegerExpr(L"-?[0-9]+");
const XRegularExpression kFloatExpr(L"-?(([0-9]+\\.[0-9]*)|([0-9]+)|(\\.[0-9]+))([eE][-+]?[0-9]+)?");
const XRegularExpression kAlphaExpr(L"[A-Za-z]+");
const XRegularExpression kLowerCaseExpr(L"[a-z]+");
const XRegularExpression kUpperCaseExpr(L"[A-Z]+");
const XRegularExpression kAlphaNumExpr(L"[0-9A-Za-z]+");
const XRegularExpression kIdentifierExpr(L"[A-Za-z_][A-Za-z0-9_]*");


// ===================================================================================
//	class ZConvertRegExpr
// ===================================================================================

//---------------------------------------------------------------
//
// ZConvertRegExpr::~ZConvertRegExpr
//
//---------------------------------------------------------------
ZConvertRegExpr::~ZConvertRegExpr()
{
	mNDFA = nil;
}


//---------------------------------------------------------------
//
// ZConvertRegExpr::ZConvertRegExpr
//
//---------------------------------------------------------------
ZConvertRegExpr::ZConvertRegExpr(XNDFA* ndfa, const std::wstring& regExpr)
{
	PRECONDITION(ndfa != nil);
	
	mNDFA = ndfa;

	mIndex = 0;	
	mExpression = regExpr;
	
	mMatchLineStart = false;
	
	int32 startState = mNDFA->GetStartState();		
	
	if (mNDFA->GetNumStates() > 0)
		mNextState = Max(startState + 1, mNDFA->GetLastState() + 1);
	else
		mNextState = Max(startState + 1L, 1L);		
		
	mHaltState = mNextState++;						
	
	int32 left  = mNextState++;						
	int32 right = mNextState++;						

	mNDFA->AddTransition(startState, left);			
	mNDFA->AddTransition(right, mHaltState);		
	
	mNDFA->SetStartState(startState);
	mNDFA->AddHaltState(mHaltState);
	
	this->DoParseRegExpr(left, right);				
}


//---------------------------------------------------------------
//
// ZConvertRegExpr::DoParseRegExpr
//
// RegExpr := '^'? Expression? '$'?
//
//---------------------------------------------------------------
void ZConvertRegExpr::DoParseRegExpr(int32 left, int32 right)
{
	// '^'?
	if (mIndex < mExpression.length() && mExpression[mIndex] == '^') {	
		mMatchLineStart = true;
		mIndex++;
	}
	
	// Expression?
	if (mIndex+1 < mExpression.length() || (mIndex < mExpression.length() && mExpression[mIndex] != '$')) {
		int32 newRight = mNextState++;					
		this->DoParseExpression(left, newRight);											

		mNDFA->AddTransition(newRight, mNextState);
		left = mNextState++;
	}
	
	// '$'?
	if (mIndex < mExpression.length() && mExpression[mIndex] == '$')	
		mNDFA->AddTransition(left, '\r', right);
	else
		mNDFA->AddTransition(left, right);
}


//---------------------------------------------------------------
//
// ZConvertRegExpr::DoParseExpression
//
// Expression := SubExpr ('|' SubExpr)*
//
//---------------------------------------------------------------
void ZConvertRegExpr::DoParseExpression(int32 left, int32 right)
{
	int32 oldLeft = left;							
	int32 oldRight = right;							
	
	left = mNextState++;							
	mNDFA->AddTransition(oldLeft, left);		
	
	right = mNextState++;							
	mNDFA->AddTransition(right, oldRight);		

	// SubExpr 
	this->DoParseSubExpr(left, right);						

	// ('|' SubExpr)*
	while (mIndex < mExpression.length() && mExpression[mIndex] == '|') {	
		mIndex++;
		
		left = mNextState++;
		mNDFA->AddTransition(oldLeft, left);
		
		right = mNextState++;
		mNDFA->AddTransition(right, oldRight);

		this->DoParseSubExpr(left, right); 
	}
}


//---------------------------------------------------------------
//
// ZConvertRegExpr::DoParseSubExpr
//
// SubExpr := Element+
//
//---------------------------------------------------------------
void ZConvertRegExpr::DoParseSubExpr(int32 left, int32 right)
{	
	int32 oldRight = right;							
	
	do {
		right = mNextState++;						
		this->DoParseElement(left, right);			
		left = right;		
	} while (mIndex < mExpression.length() && mExpression[mIndex] != '$' && mExpression[mIndex] != '|' && mExpression[mIndex] != ')');

	mNDFA->AddTransition(right, oldRight);
}


//---------------------------------------------------------------
//
// ZConvertRegExpr::DoParseElement
//
// Element := Symbol ('*' | '+' | '?')?
//
//---------------------------------------------------------------
void ZConvertRegExpr::DoParseElement(int32 left, int32 right)
{
	int32 oldLeft = left;							
	int32 oldRight = right;							
	
	left = mNextState++;							
	mNDFA->AddTransition(oldLeft, left);		
	
	right = mNextState++;							
	mNDFA->AddTransition(right, oldRight);		

	this->DoParseSymbol(left, right);				

	if (mIndex < mExpression.length()) {
	
		// '*'?
		if (mExpression[mIndex] == '*') {				
			mNDFA->AddTransition(oldLeft, oldRight);
			mNDFA->AddTransition(oldRight, oldLeft);
			mIndex++;
		
		// '+'?
		} else if (mExpression[mIndex] == '+') {			
			mNDFA->AddTransition(oldRight, oldLeft);
			mIndex++;
		
		// '?'?
		} else if (mExpression[mIndex] == '?') {			
			mNDFA->AddTransition(oldLeft, oldRight);
			mIndex++;
		}
	}
}


//---------------------------------------------------------------
//
// ZConvertRegExpr::DoParseSymbol
//
// Symbol := '.' | BracketExpr | ('(' Expression ')') | '\' Char | Char
//
//---------------------------------------------------------------
void ZConvertRegExpr::DoParseSymbol(int32 left, int32 right)
{
	bool found = false;
	
	if (mIndex < mExpression.length()) {
		found = true;
		
		// '.'
		if (mExpression[mIndex] == '.') {			
			for (int32 index = 0; index <= 0xFF; index++)			// $$$ not Unicode savvy
				if (index != '\r')
					mNDFA->AddTransition(left, index, right);		
			mIndex++;
		
		// BracketExpr
		} else if (mExpression[mIndex] == '[') {		
			this->DoParseBracketExpr(left, right);
			
		// '(' Expression ')'
		} else if (mExpression[mIndex] == '(') {		
			mIndex++;

			this->DoParseExpression(left, right);

			if (mIndex < mExpression.length() && mExpression[mIndex] == ')')
				mIndex++;
			else
				throw std::domain_error("Expected a closing parenthesis.");
		
		// '\' Char
		} else if (mExpression[mIndex] == '\\') {	
			if (++mIndex < mExpression.length())
				mNDFA->AddTransition(left, mExpression[mIndex++], right);						
			else
				throw std::domain_error("Expected a character after the back slash.");
		
		// Char
		} else if (mExpression[mIndex] != ')' && mExpression[mIndex] != '$') {	
			mNDFA->AddTransition(left, mExpression[mIndex++], right);						
		
		} else
			found = false;
	}
	
	if (!found)		
		throw std::domain_error("Expected a period, bracket, parenthesis, backslash, or character.");
}

struct SCharRange {
	wchar_t first;
	wchar_t	last;
	
				SCharRange()					{}
				SCharRange(wchar_t a, wchar_t b) 	{first = a; last = b;}
		
		bool 	operator==(const SCharRange& rhs) const	{return first == rhs.first && last == rhs.last;}
		bool 	operator<(const SCharRange& rhs) const	{return first < rhs.first || (first == rhs.first && last < rhs.last);}
};
	
//---------------------------------------------------------------
//
// ZConvertRegExpr::DoParseBracketExpr
//
// BracketExpr := '[' '^'? BracketChars ']' 
//
// $$�It'd be nice to replace sequences like [a-z] with a single 
// $$ state. Unfortunately this is non-trivial (for example
// $$ "([a-z]+)|(a21)" can only be reduced to a DFA if [a-z]
// $$ results in a transition for just the 'a' character).
// $$
// $$ Another option is to change the automata classes so that
// $$ they deal with symbol ranges instead of just symbols.
//
//---------------------------------------------------------------
void ZConvertRegExpr::DoParseBracketExpr(int32 left, int32 right)
{
	// '['
	VERIFY(mExpression[mIndex++] == '[');								
	
	// '^'?
	bool invert = false;
	
	if (mIndex < mExpression.length() && mExpression[mIndex] == '^') {	
		invert = true;
		mIndex++;
	}
	
	// BracketChars
	bool chars[256];							// $$$ not Unicode savvy!
	this->DoParseBracketChars(chars);
	
	for (int32 index = 0; index < 256; index++) {
		if (invert)
			chars[index] = !chars[index];

		if (chars[index])
			mNDFA->AddTransition(left, index, right);		
	}
		
	// ']'
	if (mIndex < mExpression.length() && mExpression[mIndex] == ']')	
		mIndex++;
	else
		throw std::domain_error("Expected a closing bracket.");
}


//---------------------------------------------------------------
//
// ZConvertRegExpr::DoParseBracketChars
//
// BracketChars := (Char ('-' Char)?)+
//
//---------------------------------------------------------------
void ZConvertRegExpr::DoParseBracketChars(bool* chars)
{
	bool foundOne = false;			// use this in case the first character is a right bracket
	
	uint32 index;
	for (index = 0; index < 256; index++)
		chars[index] = false;

	while (mIndex < mExpression.length() && (!foundOne || mExpression[mIndex] != ']')) {
		wchar_t first = mExpression[mIndex++];
		wchar_t last  = first;
	
		if (mIndex < mExpression.length() && mExpression[mIndex] == '-') {
			mIndex++;
			
			if (mIndex < mExpression.length())
				last = mExpression[mIndex++];
			else
				throw std::domain_error("Expected a character after the dash.");

			if (first > last)
				throw std::domain_error("Character before the dash is larger than the character after the dash!");
		}
		
		for (index = first; index <= last; index++)
			chars[index] = true;
			
		foundOne = true;
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XRegularExpression
// ===================================================================================

//---------------------------------------------------------------
//
// XRegularExpression::~XRegularExpression
//
//---------------------------------------------------------------
XRegularExpression::~XRegularExpression()
{
	delete mDFA;
}


//---------------------------------------------------------------
//
// XRegularExpression::XRegularExpression ()
//
//---------------------------------------------------------------
XRegularExpression::XRegularExpression()
{
	mDFA = nil;
	
	mMatchLineStart = false;
}


//---------------------------------------------------------------
//
// XRegularExpression::XRegularExpression (string)
//
//---------------------------------------------------------------
XRegularExpression::XRegularExpression(const std::wstring& expression)
{
	mExpression = expression;
		
	mDFA = nil;
	
	mMatchLineStart = false;
	
	XNDFA ndfa;					
	ZConvertRegExpr converter(&ndfa, mExpression);		// do this right away so we can find out if the string is malformed right away	
	mMatchLineStart = converter.MatchesLineStart();

	XDFA* dfa = ndfa.CreateDFA();	
	delete mDFA;
	mDFA = dfa;
}


//---------------------------------------------------------------
//
// XRegularExpression::Match (string, uint32)
//
//---------------------------------------------------------------
uint32 XRegularExpression::Match(const std::wstring& str, uint32 start) const
{
	uint32 count = 0;
	
	if (mDFA != nil)
		count = this->OnMatch(str, start);
		
	return count;
}
	
	
//---------------------------------------------------------------
//
// XRegularExpression::Match (string, string, uint32)	[static]
//
//---------------------------------------------------------------
uint32 XRegularExpression::Match(const std::wstring& expr, const std::wstring& str, uint32 start)
{
	XRegularExpression matcher(expr);
	
	return matcher.Match(str, start);
}


//---------------------------------------------------------------
//
// XRegularExpression::SetExpression
//
//---------------------------------------------------------------
void XRegularExpression::SetExpression(const std::wstring& expression)			
{
	if (expression != mExpression) {
		mExpression = expression;
		
		mMatchLineStart = false;
		
		XNDFA ndfa;
		ZConvertRegExpr converter(&ndfa, mExpression);		// do this right away so we can find out if the string is malformed right away	
		mMatchLineStart = converter.MatchesLineStart();

		XDFA* dfa = ndfa.CreateDFA();	
		delete mDFA;
		mDFA = dfa;
	}
}

#if __MWERKS__
#pragma mark �~
#endif

//---------------------------------------------------------------
//
// XRegularExpression::OnMatch
//
//---------------------------------------------------------------
uint32 XRegularExpression::OnMatch(const std::wstring& str, uint32 start) const
{
	uint32 count = 0;
	uint32 index = start;
	
	if (!mMatchLineStart || (index < str.length() && (index == 0 || str[index - 1] == '\r'))) {
		mDFA->Reset();
		
		while (index < str.length() && !mDFA->IsBlocked()) {
			wchar_t ch = str[index++];
			
			mDFA->NextState(ch);
			
			if (mDFA->CanHalt())
				count = index - start;			// go for the longest match
		}
	}
			
	return count;
}


}		// namespace Whisper

