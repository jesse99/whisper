/*
 *  File:		XLexerGrammar.h
 *  Summary:	A class containing the grammar for lexical analysis.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XLexerGrammar.h,v $
 *		Revision 1.4  2001/04/17 01:44:15  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.3  2000/12/11 01:30:39  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:41:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <map>

#include <XInvariant.h>
#include <XSet.h>
#include <XBinaryPersistent.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XDFA;
class XNDFA;


//-----------------------------------
//	Types
//
typedef int32 TokenNum;


//-----------------------------------
//	Constants
//
const TokenNum kNoToken = LONG_MIN;


// ===================================================================================
//	class XLexerGrammar
//!		A class containing the grammar for lexical analysis.
// ===================================================================================
class PARSE_EXPORT XLexerGrammar : public XBinaryPersistentMixin {

	typedef XBinaryPersistentMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XLexerGrammar();

						XLexerGrammar();
						
private:
						XLexerGrammar(const XLexerGrammar& rhs);
						
			XLexerGrammar& operator=(const XLexerGrammar& rhs);

//-----------------------------------
//	API
//
public:
	//! @name Automata
	//@{
			XDFA* 		GetExprDFA() const;
						/**< Automata uses to map character sequences to a halt state
						representing a regular expression. (We have to use two automata
						because the same character sequence can match a string and an
						expression, eg kIdentifierExpr and "class"). */

			XDFA* 		GetStrDFA() const;
						/**< Automata uses to map character sequences to a halt state
						representing a string. */
	//@}

	//! @name Construction
	//@{
			void 		AddExpr(const std::wstring& regExpr, TokenNum num);

			void 		AddStr(const std::wstring& str, TokenNum num);
	//@}
			
	//! @name Inquiry
	//@{
			TokenNum 	GetExprToken(int32 haltState) const;

			TokenNum 	GetStrToken(int32 haltState) const;
	//@}
				
//-----------------------------------
//	Inherited API
//
protected:
		 	void 		Invariant() const;

	virtual	void 		OnStreamOut(XOutStream& stream) const;

	virtual void 		OnStreamIn(XInStream& stream);

//-----------------------------------
//	Internal Types
//
public:	
	typedef std::map<int32, TokenNum> TokenMap;

//-----------------------------------
//	Internal API
//
protected:
			void 		DoBuildDFAs();

			XDFA* 		DoBuildDFA(XNDFA* ndfa, TokenMap& tokenMap);

//-----------------------------------
//	Member Data
//
protected:
	XDFA*		mExprDFA;
	XNDFA*		mExprNDFA;
	
	XDFA*		mStrDFA;
	XNDFA*		mStrNDFA;
	
	TokenMap	mExprMap;			// maps halt states in mExprDFA to token numbers
	TokenMap	mStrMap;			// maps halt states in mStrDFA to token numbers
	
#if DEBUG
	XSet<int32>	mTokens;			// list of token numbers in use
#endif
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
