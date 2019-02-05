/*
 *  File:		XRegularExpression.h
 *  Summary:	A regular expression class that works like CodeWarrior's find dialog.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRegularExpression.h,v $
 *		Revision 1.7  2001/05/30 01:19:20  jesjones
 *		Added operator== methods.
 *		
 *		Revision 1.6  2001/03/17 07:43:33  jesjones
 *		Exported ZConvertRegExpr.
 *		
 *		Revision 1.5  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.4  2000/12/11 01:31:05  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/11/21 09:26:22  jesjones
 *		Added <string> include.
 *		
 *		Revision 1.2  2000/11/09 12:49:27  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <string>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XDFA;
class XNDFA;
class XRegularExpression;
	

//-----------------------------------
//	Constants
//
extern PARSE_EXPORT const XRegularExpression kWhiteSpaceExpr;	//!< = "[ \n\t\r\v\f]+"
extern PARSE_EXPORT const XRegularExpression kIntegerExpr;		//!< = "-?[0-9]+"
extern PARSE_EXPORT const XRegularExpression kFloatExpr;		//!< = "-?(([0-9]+\\.[0-9]*)|([0-9]+)|(\\.[0-9]+))([eE][-+]?[0-9]+)?"
extern PARSE_EXPORT const XRegularExpression kAlphaExpr;		//!< = "[A-Za-z]+"
extern PARSE_EXPORT const XRegularExpression kLowerCaseExpr;	//!< = "[a-z]+"
extern PARSE_EXPORT const XRegularExpression kUpperCaseExpr;	//!< = "[A-Z]+"
extern PARSE_EXPORT const XRegularExpression kAlphaNumExpr;		//!< = "[0-9A-Za-z]+"
extern PARSE_EXPORT const XRegularExpression kIdentifierExpr;	//!< = "[A-Za-z_][A-Za-z0-9_]*"


// ===================================================================================
//	class XRegularExpression
//!		A regular expression class that works like CodeWarrior's find dialog.
/*!		Most characters match themselves. The exceptions are the following:
 *			- .		matches any character except a carriage return
 *			- *		matches zero or more occurances of the preceding regular expression
 *			- +		matches one or more occurances of the preceding regular expression
 *			- ()	used for grouping (typically for the * or + characters)
 *			- []	matches any single character in the brackets
 *			- -		if in the middle of a bracket indicates a range of characters
 *			- ^		1) if the first character within a bracket it matches any single 
 *					character *not* in the bracket
 *					2) if at the beginning of the entire regular expression it matches
 *					the beginning of a line
 *			- $		if at the end of the entire regular expression it matches the end of a line
 *			- \		used to match one of the special characters
 *			- ?		preceding regular expression is optional (this is supported in CW 11 but appears to be undocumented)
 *			- |		matches expression to left or right (this is supported in CW 11 but appears to be undocumented)
 *		See the CodeWarrior User's Guide for more details and examples (TestRegularExpression
 *		also has a number of examples).
 *
 *		In EBNF notation the regular expression syntax is:
 *			- RegExpr := '^'? Expression? '$'?
 *			- Expression := SubExpr ('|' SubExpr)*
 *			- SubExpr := Element+
 *			- Element := Symbol ('*' | '+' | '?')?
 *			- Symbol := '.' | BracketExpr | ('(' Expression ')') | '\' Char | Char
 *			- BracketExpr := '[' '^'? BracketChars ']' 
 *			- BracketChars := (Char ('-' Char)?)+ */
// ===================================================================================
class PARSE_EXPORT XRegularExpression {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XRegularExpression();

						XRegularExpression();

	explicit			XRegularExpression(const std::wstring& expression);
						/**< Explicit to remind people that creating an XRegularExpression
						is fairly expensive and it's therefore much better to reuse
						them. */

private:
						XRegularExpression(const XRegularExpression& rhs);
						
			XRegularExpression& operator=(const XRegularExpression& rhs);
						
//-----------------------------------
//	API
//
public:
	virtual	uint32 		Match(const std::wstring& str, uint32 start = 0) const;
						/**< Attempts to match str, starting at start, to the regular
						expression. If there's a match the number of matching
						characters in str is returned. If there's not a match
						or only a partial match zero is returned. */
							
	static	uint32 		Match(const std::wstring& expr, const std::wstring& str, uint32 start = 0);
						/**< Note that this is fairly expensive: if you're calling this a lot
						it's better to create a XRegularExpression that you can reuse. */

			const std::wstring& GetExpression() const						{return mExpression;}
						
	virtual	void 		SetExpression(const std::wstring& expression);

			bool 		operator==(const std::wstring& rhs) const			{return mExpression == rhs;}
			bool 		operator==(const XRegularExpression& rhs) const		{return mExpression == rhs.mExpression;}
						
protected:
	virtual	uint32 		OnMatch(const std::wstring& str, uint32 start) const;

//-----------------------------------
//	Member Data
//
protected:
	std::wstring	mExpression;
	XDFA*			mDFA;				// deterministic automata representing the expression
	bool			mMatchLineStart;
};


// ===================================================================================
//	class ZConvertRegExpr
//!		Used by XRegularExpression and XLexerGrammar to convert a regular expression into a non-deterministic automata.
// ===================================================================================
class PARSE_EXPORT ZConvertRegExpr {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~ZConvertRegExpr();

						ZConvertRegExpr(XNDFA* ndfa, const std::wstring& regExpr);

private:
						ZConvertRegExpr(const ZConvertRegExpr& rhs);
						
			ZConvertRegExpr& operator=(const ZConvertRegExpr& rhs);
						
//-----------------------------------
//	API
//
public:			
			bool 		MatchesLineStart() const					{return mMatchLineStart;}
						/**< Returns true if the regular expression matches the start of a line. */
			
			int32 		GetHaltState() const						{return mHaltState;}
						/**< Returns the final state of the automata. */
	
//-----------------------------------
//	Internal API
//
protected:
			void 		DoParseRegExpr(int32 left, int32 right);

			void 		DoParseExpression(int32 left, int32 right);
	
			void 		DoParseSubExpr(int32 left, int32 right);

			void 		DoParseElement(int32 left, int32 right);

			void 		DoParseSymbol(int32 left, int32 right);

			void 		DoParseBracketExpr(int32 left, int32 right);

			void 		DoParseBracketChars(bool* chars);

//-----------------------------------
//	Member Data
//
protected:
	XNDFA*			mNDFA;				
	uint32			mIndex;		
	std::wstring	mExpression;
	int32			mNextState;
	int32			mHaltState;
	bool			mMatchLineStart;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

