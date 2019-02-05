/*
 *  File:		XLexerGrammar.cpp
 *  Summary:	A class containing the grammar for lexical analysis.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XLexerGrammar.cpp,v $
 *		Revision 1.4  2001/04/27 04:33:59  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/04/17 01:44:12  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.2  2000/11/09 12:41:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XLexerGrammar.h>

#include <XDFA.h>
#include <XExceptions.h>
#include <XNDFA.h>
#include <XNumbers.h>
#include <XRegularExpression.h>
#include <XStreaming.h>

namespace Whisper {


// ===================================================================================
//	class XLexerGrammar
// ===================================================================================

//---------------------------------------------------------------
//
// XLexerGrammar::~XLexerGrammar
//
//---------------------------------------------------------------
XLexerGrammar::~XLexerGrammar()
{
	CALL_INVARIANT;
	
	delete mExprDFA;
	delete mExprNDFA;

	delete mStrDFA;
	delete mStrNDFA;
}


//---------------------------------------------------------------
//
// XLexerGrammar::XLexerGrammar
//
//---------------------------------------------------------------
XLexerGrammar::XLexerGrammar()
{
	mExprDFA  = nil;
	mExprNDFA = new XNDFA;

	mStrDFA  = nil;
	mStrNDFA = new XNDFA;

	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XLexerGrammar::GetExprDFA
//
//---------------------------------------------------------------
XDFA* XLexerGrammar::GetExprDFA() const
{
	if (mExprDFA == nil) {
		XLexerGrammar* thisPtr = const_cast<XLexerGrammar*>(this);
		
		thisPtr->DoBuildDFAs();
	}
	
	return mExprDFA;
}


//---------------------------------------------------------------
//
// XLexerGrammar::GetStrDFA
//
//---------------------------------------------------------------
XDFA* XLexerGrammar::GetStrDFA() const
{
	if (mStrDFA == nil) {
		XLexerGrammar* thisPtr = const_cast<XLexerGrammar*>(this);
		
		thisPtr->DoBuildDFAs();
	}
	
	return mStrDFA;
}


//---------------------------------------------------------------
//
// XLexerGrammar::AddExpr
//
//---------------------------------------------------------------
void XLexerGrammar::AddExpr(const std::wstring& regExpr, TokenNum num)
{
	PRECONDITION(num != kNoToken);
#if DEBUG
	PRECONDITION(!mTokens.contains(num));
#endif
	CHECK_INVARIANT;
	
	delete mExprDFA;
	mExprDFA = nil;
	
	ZConvertRegExpr converter(mExprNDFA, regExpr);
	if (converter.MatchesLineStart())
		throw std::domain_error("XLexerGrammar doesn't support matching the start of a line.");

	int32 haltState = converter.GetHaltState();
	ASSERT(mExprMap.count(haltState) == 0);	
	mExprMap[haltState] = num;
	
#if DEBUG
	mTokens += num;
#endif

	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// XLexerGrammar::AddStr
//
//---------------------------------------------------------------
void XLexerGrammar::AddStr(const std::wstring& str, TokenNum num)
{
	PRECONDITION(num != kNoToken);
#if DEBUG
	PRECONDITION(!mTokens.contains(num));
#endif
	CHECK_INVARIANT;

	delete mStrDFA;
	mStrDFA = nil;
	
	int32 startState = mStrNDFA->GetStartState();
	int32 nextState;
	
	if (mStrNDFA->GetNumStates() > 0)
		nextState = Max(startState + 1, mStrNDFA->GetLastState() + 1);
	else
		nextState = Max(startState + 1L, 1L);
		
	int32 haltState = nextState++;
	
	int32 state = startState;
	for (uint32 index = 0; index < str.size(); index++) {
		wchar_t ch = str[index];
		
		mStrNDFA->AddTransition(state, ch, nextState);
		state = nextState++;
	}

	mStrNDFA->AddTransition(state, haltState);
	mStrNDFA->AddHaltState(haltState);

	ASSERT(mStrMap.count(haltState) == 0);	
	mStrMap[haltState] = num;
	
#if DEBUG
	mTokens += num;
#endif
}


//---------------------------------------------------------------
//
// XLexerGrammar::GetExprToken
//
//---------------------------------------------------------------
TokenNum XLexerGrammar::GetExprToken(int32 haltState) const
{
	TokenMap::const_iterator iter = mExprMap.find(haltState);
	PRECONDITION(iter != mExprMap.end());
	
	return (*iter).second;
}


//---------------------------------------------------------------
//
// XLexerGrammar::GetStrToken
//
//---------------------------------------------------------------
TokenNum XLexerGrammar::GetStrToken(int32 haltState) const
{
	TokenMap::const_iterator iter = mStrMap.find(haltState);
	PRECONDITION(iter != mStrMap.end());
	
	return (*iter).second;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XLexerGrammar::Invariant
//
//---------------------------------------------------------------
void XLexerGrammar::Invariant() const
{	
	ASSERT(mExprNDFA != nil);
	ASSERT(mStrNDFA != nil);
}


//---------------------------------------------------------------
//
// XLexerGrammar::OnStreamOut
//
//---------------------------------------------------------------
void XLexerGrammar::OnStreamOut(XOutStream& stream) const
{
	if (mExprDFA == nil || mStrDFA == nil) {
		XLexerGrammar* thisPtr = const_cast<XLexerGrammar*>(this);
		
		thisPtr->DoBuildDFAs();
	}
	
	stream << 1L
	       << *mExprDFA
		   << *mStrDFA
	
	       << mExprMap
	       << mStrMap;
}


//---------------------------------------------------------------
//
// XLexerGrammar::OnStreamIn
//
//---------------------------------------------------------------
void XLexerGrammar::OnStreamIn(XInStream& stream)
{
	int32 version;
	stream >> version;
	
	if (version != 1)
		throw std::runtime_error("Internal Error: Can't stream in this XLexerGrammar (bad version).");
		
	delete mExprDFA;
	mExprDFA = nil;
	mExprDFA = new XDFA;
	
	delete mStrDFA;
	mStrDFA = nil;
	mStrDFA = new XDFA;
	
	stream >> *mExprDFA;
	stream >> *mStrDFA;
	
	stream >> mExprMap;
	stream >> mStrMap;	
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XLexerGrammar::DoBuildDFAs
//
//---------------------------------------------------------------
void XLexerGrammar::DoBuildDFAs()
{
	PRECONDITION(mExprDFA == nil);
	PRECONDITION(mStrDFA == nil);
		
	try {
		mExprDFA = this->DoBuildDFA(mExprNDFA, mExprMap);
		mStrDFA  = this->DoBuildDFA(mStrNDFA, mStrMap);
					
	} catch (...) {
		delete mExprDFA;
		mExprDFA = nil;
				
		delete mStrDFA;
		mStrDFA = nil;
						
		throw;
	}
}


//---------------------------------------------------------------
//
// XLexerGrammar::DoBuildDFA
//
//---------------------------------------------------------------
XDFA* XLexerGrammar::DoBuildDFA(XNDFA* ndfa, TokenMap& tokenMap)
{
	PRECONDITION(ndfa != nil);
	
	TokenMap oldTokens = tokenMap;

	// Convert the ndfa to a dfa.		
	XNDFA::TransitionMap haltMap;
	XDFA* dfa = ndfa->CreateDFA(&haltMap);

	// Find out which tokens the new halt states belong to.
	try {
		tokenMap.clear();
		
		XNDFA::TransitionMap::iterator iter = haltMap.begin();
		while (iter != haltMap.end()) {
			const XNDFA::TransitionMap::value_type& value = *iter++;
			
			int32 oldHaltState = value.first;
			const XNDFA::StateSet& haltStates = value.second;
			
			XNDFA::StateSet::const_iterator iter2 = haltStates.begin();
			while (iter2 != haltStates.end()) {
				int32 newHaltState = *iter2++;
				
				ASSERT(oldTokens.count(oldHaltState) == 1);	
				ASSERT(tokenMap.count(newHaltState) == 0);	

				tokenMap[newHaltState] = oldTokens[oldHaltState];
			}
		}
		
	} catch (...) {
		delete dfa;
		tokenMap = oldTokens;
		throw;
	}
	
	return dfa;
}


}	// namespace Whisper
