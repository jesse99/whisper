/*
 *  File:		XLexer.h
 *  Summary:	A class used to extract tokens from text (modeled after those in Eiffel's 
 *				base libraries).
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XLexer.h,v $
 *		Revision 1.4  2001/03/06 07:32:58  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.3  2000/12/11 01:30:35  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:41:09  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <vector>

#include <XLexerGrammar.h>
#include <XScanner.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	struct SToken
//!		Token object returned by XLexer.
// ===================================================================================
struct PARSE_EXPORT SToken {
	TokenNum		num;
	std::wstring	text;
	XScannerPos		pos;	
	
		SToken()																			 {}	
		SToken(TokenNum n, const std::wstring& t, const XScannerPos& p) : num(n), text(t), pos(p) {}
		SToken(const SToken& rhs);
};


// ===================================================================================
//	class XLexer
//!		A class used to extract tokens from text (modeled after those in Eiffel's base libraries).
// ===================================================================================
class PARSE_EXPORT XLexer {

//-----------------------------------
//	Initialization/Destruction
// 
public:
			 			~XLexer();

						XLexer(XScanner* takeScanner, XLexerGrammar* takeGrammar, bool ignoreCase = false);
						/**< If ignoreCase is true case is ignored when checking against
						the grammar (but the original text is still returned in the
						token). */
						
private:
						XLexer(const XLexer& rhs);
						
			XLexer& 	operator=(const XLexer& rhs);
						
//-----------------------------------
//	API
//
public:
	//! @name Analysis
	//@{
			void 		Analyze(const std::wstring& text);
						/**< Initialize the lexer to read from text. */
	//@}

	//! @name Reading
	//@{
			bool 		AtEnd() const;
						/**< Returns true if there aren't any more characters and there
						isn't an unprocessed token. */
			
			void 		ReadToken();
						/**< Attempt to read a new token using the longest possible
						string. If no token is recognized mToken.num is set to 
						kNoToken and the scanner is advanced by one character. */

			void 		ReadFixedToken(uint32 maxChars);
						/**< Like ReadToken except it will not match strings larger
						than maxChars. */
			
			void 		ReadShortestToken();
						/**< Reads the shortest possible token. */
						
			void 		SkipToken(const SToken& token);
						/**< ReadToken sets the scanner to point one beyond the longest
						token found. If you used a shorter token you can call
						SkipToken to adjust the scanner location. (You'll normally 
						want to call one of the read methods after calling this). */
	//@}
									
	//! @name Access
	//@{
			uint32 		GetNumTokens() const				{return mTokens.size();}
						/**< Returns the number of possible tokens. */
							
			SToken 		GetToken(uint32 index = 0) const;
						/**< Returns the possible tokens for the last read. Tokens
						for the longer strings are returned first. For example
						if the string is ">=" >= and then > will be returned.
						If there isn't a token the returned token will be kNoToken. */
						
			bool 		FindToken(TokenNum num, SToken& token) const;
						/**< Rather than looping through the tokens to find a match
						you can use this function. It will return true if the
						token is found. (You should call SkipToken after using this
						function). */
						
			void 		PopToken()							{ASSERT(!mTokens.empty()); mTokens.pop_back();}
						/**< Removes the longest token. */
	//@}
			
	//! @name Access
	//@{
	const XScanner* 	GetScanner() const					{return mScanner;}
	//@}

//-----------------------------------
//	Internal API
//
protected:
			bool 		DoReadToken(XDFA* exprDfa, XDFA* strDfa, const XScannerPos& startPos);
						
//-----------------------------------
//	Member Data
//
protected:
	XScanner*			mScanner;
	XLexerGrammar*		mGrammar;
	bool				mIgnoreCase;
	
	std::vector<SToken>	mTokens;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
