/*
 *  File:		XSimpleParser.h
 *  Summary:	Simplistic text parsing class.
 *  Written by:	Jesse Jones
 *
 *	Classes:	XParserException  - The exception class thrown by the parsers.
 *
 *				XPlugInParser	  - Base class for plug-in parsers.
 *				XIntegerParser	  - Default object used by XParser to parse integer numbers.
 *				XInfIntegerParser - An XIntegerParser that also accepts inf (as shorthand
 *									for the largest (or smallest if negative) number).
 *
 *				XSimpleParser	  - General purpose parser class. Uses plug-in objects to
 *								    avoid a proliferation of XSimpleParser subclasses.
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XSimpleParser.h,v $
 *		Revision 1.4  2001/05/30 01:20:11  jesjones
 *		Added HandleError to make it easier for clients to throw parser exceptions.
 *		
 *		Revision 1.3  2000/12/11 01:31:19  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:51:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 6/11/99	JDJ		Tweaked friend declaration for CW Pro 5fc3.
 *		 <4>	 6/08/99	JDJ		Fixed useCase arguments.
 *		 <3>	 6/04/99	JDJ		Added GetPosition.
 *		 <2>	 2/04/99	JDJ		ParseChars takes a XRegularExpression instead of an XString.
 *									ParseAlpha, ParseAlphaNum, and ParseIdentifier use
 *									XRegularExpression constants instead of string literals.
 *		 <1>	12/17/98	JDJ		Created (from Raven)
 */

#pragma once

#include <XConstants.h>
#include <XRegularExpression.h>
#include <XScanner.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XPlugInParser
//!		Base class for plug-in parsers.
// ===================================================================================
class PARSE_EXPORT XPlugInParser {

	friend class XSimpleParser;

public:
	virtual 			~XPlugInParser() 	{}

						XPlugInParser();

protected:
			void 		SetScanner(XScanner* scanner);
						// XSimpleParser will hook the scanner up.

protected:
	XScanner*	mScanner;
};


// ===================================================================================
//	class XIntegerParser
//!		Default object used by XParser to parse integer numbers.
// ===================================================================================
class PARSE_EXPORT XIntegerParser : public XPlugInParser {

	typedef XPlugInParser Inherited;

public:
	virtual int16 		ParseInt16();

	virtual int32 		ParseInt32();

protected:
			bool 		DoFoundMinusSign();

			int32 		DoParseDigits(bool negated);

			void 		DoHandleOverflow();

			void 		DoHandleUnderflow();
};


// ===================================================================================
//	class XInfIntegerParser
//!		An XIntegerParser that also accepts inf (as shorthand for the largest (or smallest if negative) number).
// ===================================================================================
class PARSE_EXPORT XInfIntegerParser : public XIntegerParser {

	typedef XIntegerParser Inherited;

public:
	virtual int16 		ParseInt16();

	virtual int32 		ParseInt32();

protected:
			bool 		DoFoundInfinity();
};


// ===================================================================================
//	class XSimpleParser
//!		General purpose parser class. Uses plug-in objects to avoid a proliferation of XSimpleParser subclasses.
// 		Most of these functions will throw exceptions. Any exception thrown will be 
//		mapped to XParserException.
// ===================================================================================
class PARSE_EXPORT XSimpleParser {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XSimpleParser();

						XSimpleParser(const std::wstring& text);
						/**< Uses base classes for scanner and plug-in parsers. */

						XSimpleParser(XScanner* takeScanner, XIntegerParser* takeParser = nil);

//-----------------------------------
//	API
//
public:
	//! @name Position
	//@{
			void 		Advance(int32 delta = 1)					{mScanner->Advance(delta);}

			bool 		AtEnd() const;
						/**< Calls SkipWhiteSpace. */

			XScannerPos GetPosition() const							{return mScanner->GetPosition();}
	//@}

	//! @name Finding
	//@{
			bool 		Found(wchar_t theChar, bool useCase = kUseCase);
						/**< These functions return true if the item was found at the current
						position and increment the position by the item's length. */

			bool 		Found(const std::wstring& str, bool useCase = kUseCase);
	//@}

	//! @name Skipping
	//@{
			void 		SkipPast(wchar_t theChar);
						/**< These functions search for the item and increment the position
						by the item's length. If the item isn't found XParserException
						is thrown. */

			void 		SkipPast(const std::wstring& str, bool useCase = kUseCase);
	//@}

	//! @name Parsing
	//@{
			wchar_t 	ParseChar(bool useCase = kUseCase);
						/**< Return the next character and increment the position.
						If ignore case is true these functions convert their
						characters to lower case. */

			std::wstring ParseChars(const XRegularExpression& regExp, bool useCase = kUseCase);
						/**< Returns the characters that match the specified regular expression. */

			std::wstring ParseAlpha(bool useCase = kUseCase)			{return this->ParseChars(kAlphaExpr, useCase);}
						/**< Parses letters. */

			std::wstring ParseAlphaNum(bool useCase = kUseCase)		{return this->ParseChars(kAlphaNumExpr, useCase);}
						/**< Parses letters and numbers. */

			std::wstring ParseIdentifier(bool useCase = kUseCase)	{return this->ParseChars(kIdentifierExpr, useCase);}
						/**< Parses letter followed by alpha-num. */

			std::wstring ParseQuote(bool useCase = kUseCase, wchar_t quoteMark = '"');
						/**< Returns characters inside the quote marks. */

			bool 		ParseBool();
						/**< Looks for "true" and "false". */

	virtual int16 		ParseInt16();

	virtual int32 		ParseInt32();
	//@}

	//! @name Misc
	//@{
			wchar_t 	Peek(int32 offset = 0) const		{return mScanner->Peek(offset);}
						/**< Peeking past the end returns '\0'. Note that this does not
						skip white space! */

			void 		SkipWhiteSpace()					{mScanner->SkipWhiteSpace();}
			
			void 		HandleError(const std::wstring& mesg);
	//@}

//-----------------------------------
//	Internal API
//
protected:
	virtual void 		DoParseError(const std::wstring* mesg = nil) const;

//-----------------------------------
//	Member Data
//
protected:
	XScanner*		mScanner;
	XIntegerParser*	mIntParser;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
