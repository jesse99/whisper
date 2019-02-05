/*
 *  File:		XXMLLexer.h
 *  Summary:	A class used to extract tokens from XML documents
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLLexer.h,v $
 *		Revision 1.5  2001/03/06 07:33:53  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.4  2001/03/05 05:44:47  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/11 01:31:47  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/10 04:47:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <vector>

#include <XLexerGrammar.h>		// we only use the TokenNum declaration...
#include <XScanner.h>
#include <XURI.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XXMLParser;


//-----------------------------------
//	Tokens
//
const TokenNum kFixedToken			= 'fixd';	//!< "#FIXED"
const TokenNum kImpliedToken		= 'impl';	//!< "#IMPLIED"
const TokenNum kPCDataToken			= 'pcdt';	//!< "#PCDATA"
const TokenNum kRequiredToken		= 'req';	//!< "#REQUIRED"
const TokenNum kAttListTagToken		= 'attt';	//!< "<!ATTLIST"
const TokenNum kDocTypeTagToken		= 'dott';	//!< "<!DOCTYPE"
const TokenNum kElementTagToken		= 'elmt';	//!< "<!ELEMENT"
const TokenNum kEntityTagToken		= 'entt';	//!< "<!ENTITY"
const TokenNum kNotationTagToken	= 'ntnt';	//!< "<!NOTATION"
const TokenNum kCDSectTagToken		= 'cdtt';	//!< "<![CDATA["
const TokenNum kXMLTagToken			= 'xmlt';	//!< "<?xml"
const TokenNum kCommentStartToken	= 'cmts';	//!< "<!--"
const TokenNum kDataStartToken		= 'dats';	//!< "<!["
const TokenNum kDataEndToken		= 'date';	//!< "]]>"
const TokenNum kCommentEndToken		= 'cmte';	//!< "-->"
const TokenNum kPIStartToken		= '<?';		//!< "<?"
const TokenNum kPIEndToken			= '?>';		//!< "?>"
const TokenNum kEndTagToken			= '</';		//!< "</"			
const TokenNum kEmptyTagToken		= '/>';		//!< "/>"			
const TokenNum kLeftBracketToken	= '[';		//!< "["
const TokenNum kRightBracketToken	= ']';		//!< "]"
const TokenNum kLeftParensToken		= '(';		//!< "("
const TokenNum kRightParensToken	= ')';		//!< ")"
const TokenNum kLessToken			= '<';		//!< "<"
const TokenNum kGreaterToken		= '>';		//!< ">"
const TokenNum kPercentToken		= '%';		//!< "%"
const TokenNum kAmpersandToken		= '&';		//!< "&"
const TokenNum kPoundToken			= '#';		//!< "#"
const TokenNum kSemiColonToken		= ';';		//!< ";"
const TokenNum kCommaToken			= ',';		//!< "," 
const TokenNum kDoubleQuoteToken	= '"';		//!< """
const TokenNum kSingleQuoteToken	= '\'';		//!< "'"
const TokenNum kBarToken			= '|';		//!< "|"
const TokenNum kQuestionToken		= '?';		//!< "?"
const TokenNum kStarToken			= '*';		//!< "*"
const TokenNum kPlusToken			= '+';		//!< "+"

const TokenNum kCharToken			= 'char';	//!< Char ::= #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]
const TokenNum kWhiteSpaceToken		= 'wspc';	//!< S ::= (#x20 | #x9 | #xD | #xA)+
const TokenNum kEqualToken			= '=';		//!< Eq ::= S? '=' S?
const TokenNum kAlphaNumToken		= 'name';	//!< Nmtoken ::= (NameChar)+


// ===================================================================================
//	struct SXMLToken
//!		Tokens returns by XXMLLexer.
// ===================================================================================
struct XML_EXPORT SXMLToken {
	TokenNum		num;
	XScannerPos		pos;	
	uint32			length;
	
		SXMLToken()																		 	{}	
		SXMLToken(TokenNum n, const XScannerPos& p, uint32 l) : num(n), pos(p), length(l) 	{}
};


// ===================================================================================
//	class XXMLLexer
//!		Used by the XML Parser to extract tokens.
// ===================================================================================
class XML_EXPORT XXMLLexer {

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XXMLLexer();

						XXMLLexer(XXMLParser* parser, XScanner* takeScanner, const XURI& uri, bool internal);
						
private:
						XXMLLexer(const XXMLLexer& rhs);
						
			XXMLLexer& 	operator=(const XXMLLexer& rhs);
						
//-----------------------------------
//	API
//
public:
	//! @name Reading
	//@{
			void 		Reset();
						/**< Clears mTokens and moves the scanner to the start of the text. */
						
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
									
			void 		SkipToken(const SXMLToken& token);
						/**< ReadToken sets the scanner to point one beyond the longest
						token found. If you used a shorter token you can call
						SkipToken to adjust the scanner location. (You'll normally 
						want to call one of the read methods after calling this). */
						
			void 		SkipWhiteSpace()					{mScanner->SkipWhiteSpace();}
			
			void 		ExpandInternalParameterReferences(bool expand = true);
						/**< Defaults to false. */
	//@}
									
	//! @name Access
	//@{
			uint32 		GetNumTokens() const				{return mTokens.size();}
						/**< Returns the number of possible tokens. */
							
			SXMLToken 	GetToken(uint32 index = 0) const;
						/**< Returns the possible tokens for the last read. Tokens
						for the longer strings are returned first. For example
						if the string is ">=" >= and then > will be returned.
						If there isn't a token the returned token will be kNoToken. */
						
			bool 		FindToken(TokenNum num, SXMLToken& token) const;
						/**< Rather than looping through the tokens to find a match
						you can use this function. It will return true if the
						token is found. (You should call SkipToken after using this
						function). */
						
			void 		PopToken()							{ASSERT(!mTokens.empty()); mTokens.pop_back();}
						/**< Removes the longest token. */
						
			std::wstring GetText(const SXMLToken& token) const;
			wchar_t 	GetChar(const SXMLToken& token) const					{return mScanner->GetText()[token.pos.GetIndex()];}
			void 		AppendText(std::wstring& str, const SXMLToken& token) const	{str.append(mScanner->GetText(), token.pos.GetIndex(), token.length);}
						/**< The lexer is the main bottle neck of the parser so,
						instead of returning the string in SXMLToken, we'll
						use these methods to access the token's text. */
	//@}
			
	//! @name Misc
	//@{
			void 		UpdateLanguage(const std::wstring& newText);
						/**< Replaces the scanner text while leaving mTokens and the
						scanner position undisturbed (this is OK because all
						the characters up to the language encoding are ASCII). */
						
			const XURI& GetURI() const						{return mURI;}
						
			bool 		GetInternal() const					{return mInternal;}
			
			const XScanner* GetScanner() const				{return mScanner;}
	//@}

	//! @name Classification
	//@{
	static 	bool 		IsChar(wchar_t ch);
	
	static 	bool 		IsNameChar(wchar_t ch);
	
	static 	bool 		IsLetter(wchar_t ch);
	//@}

//-----------------------------------
//	Internal API
//
protected:
			bool 		DoExpandPE();
			
			void 		DoReadTokens(uint32 maxChars);
						
//-----------------------------------
//	Member Data
//
protected:
	XURI					mURI;
	XXMLParser*				mParser;
	XScanner*				mScanner;
	std::vector<SXMLToken>	mTokens;
	
	bool					mInternal;
	int32					mExpandPEs;
	std::wstring			mCurrentEntity;
	uint32					mCurrentEnd;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
