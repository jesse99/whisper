/*
 *  File:		XSimpleParser.cpp
 *  Summary:	Simplistic text parsing class.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XSimpleParser.cpp,v $
 *		Revision 1.5  2001/05/30 01:19:56  jesjones
 *		Added HandleError to make it easier for clients to throw parser exceptions.
 *		
 *		Revision 1.4  2001/04/27 04:36:24  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2000/12/10 04:11:36  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:51:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 3/14/00	JDJ		Uses ConvertToLowerCase instead of toupper for case insensitive
 *									compairisons.
 *		 <4>	 6/08/99	JDJ		Fixed useCase arguments.
 *		 <3>	 2/04/99	JDJ		ParseChars takes a XRegularExpression instead of an XString.
 *		 <2>	 1/26/99	JDJ		Uses IsDigit instead of isdigit.
 *		 <1>	12/17/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XSimpleParser.h>

#include <cctype>
#include <climits>

#include <XNumbers.h>
#include <XRegularExpression.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// Equal
//
//---------------------------------------------------------------
static bool Equal(wchar_t ch1, wchar_t ch2, bool useCase)
{
	if (useCase)
		return ch1 == ch2;
	else
		return ConvertToLowerCase(ch1) == ConvertToLowerCase(ch2);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XPlugInParser
// ===================================================================================

//---------------------------------------------------------------
//
// XPlugInParser::XPlugInParser
//
//---------------------------------------------------------------
XPlugInParser::XPlugInParser()
{
	mScanner = nil;
}


//---------------------------------------------------------------
//
// XPlugInParser::SetScanner
//
//---------------------------------------------------------------
void XPlugInParser::SetScanner(XScanner* scanner)
{
	PRECONDITION(mScanner == nil);
	PRECONDITION(scanner != nil);

	mScanner = scanner;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XIntegerParser
// ===================================================================================

//---------------------------------------------------------------
//
// XIntegerParser::ParseInt16
//
//---------------------------------------------------------------
short XIntegerParser::ParseInt16()
{
	PRECONDITION(mScanner != nil);
	
	bool negated = this->DoFoundMinusSign();
	
	int32 value = this->DoParseDigits(negated);

	if (value > SHRT_MAX)
		this->DoHandleOverflow();
	else if (value < SHRT_MIN)
		this->DoHandleUnderflow();
		
	return numeric_cast<int16>(value);
}


//---------------------------------------------------------------
//
// XIntegerParser::ParseInt32
//
//---------------------------------------------------------------
int32 XIntegerParser::ParseInt32()
{
	PRECONDITION(mScanner != nil);
	
	bool negated = this->DoFoundMinusSign();
	
	int32 value = this->DoParseDigits(negated);
	
	return value;
}


//---------------------------------------------------------------
//
// XIntegerParser::DoFoundMinusSign
//
//---------------------------------------------------------------
bool XIntegerParser::DoFoundMinusSign()
{
	bool negated = false;
	
	mScanner->SkipWhiteSpace();
	wchar_t ch = mScanner->Peek();

	if (ch == '+') {
		negated = false;
		mScanner->Advance();
		
	} else if (ch == '-') {
		negated = true;
		mScanner->Advance();
	}
	
	return negated;
}


//---------------------------------------------------------------
//
// XIntegerParser::DoParseDigits
//
//---------------------------------------------------------------
int32 XIntegerParser::DoParseDigits(bool negated)
{
	mScanner->SkipWhiteSpace();

	wchar_t ch = mScanner->Peek();
	if (!IsDigit(ch))
		throw XParserException(mScanner, LoadWhisperString(L"Expected a digit."));

	int32 value = 0;
			
	while (IsDigit(mScanner->Peek())) {
		int32 digit = mScanner->Peek() - '0';
		mScanner->Advance();
		
		if (negated)
			if (value >= (LONG_MIN + digit)/10)
				value = 10*value - digit;
			else
				this->DoHandleUnderflow();
		else
			if (value <= (LONG_MAX - digit)/10)
				value = 10*value + digit;
			else
				this->DoHandleOverflow();
	}
	
	return value;
}


//---------------------------------------------------------------
//
// XIntegerParser::DoHandleOverflow
//
//---------------------------------------------------------------
void XIntegerParser::DoHandleOverflow()
{	
	throw XParserException(mScanner, LoadWhisperString(L"Number was too large."));
}


//---------------------------------------------------------------
//
// XIntegerParser::DoHandleUnderflow
//
//---------------------------------------------------------------
void XIntegerParser::DoHandleUnderflow()
{	
	throw XParserException(mScanner, LoadWhisperString(L"Number was too small."));
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XInfIntegerParser
// ===================================================================================

//---------------------------------------------------------------
//
// XInfIntegerParser::ParseInt16
//
//---------------------------------------------------------------
short XInfIntegerParser::ParseInt16()
{
	PRECONDITION(mScanner != nil);
	
	int32 value;
	bool negated = this->DoFoundMinusSign();
	
	if (this->DoFoundInfinity()) {
		if (negated)
			value = SHRT_MIN;
		else
			value = SHRT_MAX;
			
	} else {
		value = this->DoParseDigits(negated);
	
		if (value > SHRT_MAX)
			this->DoHandleOverflow();
		else if (value < SHRT_MIN)
			this->DoHandleUnderflow();
	}

	return numeric_cast<int16>(value);
}


//---------------------------------------------------------------
//
// XInfIntegerParser::ParseInt32
//
//---------------------------------------------------------------
int32 XInfIntegerParser::ParseInt32()
{
	PRECONDITION(mScanner != nil);
	
	int32 value;
	bool negated = this->DoFoundMinusSign();
	
	if (this->DoFoundInfinity())
		if (negated)
			value = LONG_MIN;
		else
			value = LONG_MAX;
	else
		value = this->DoParseDigits(negated);
		
	return value;
}


//---------------------------------------------------------------
//
// XInfIntegerParser::DoFoundInfinity
//
//---------------------------------------------------------------
bool XInfIntegerParser::DoFoundInfinity()
{
	mScanner->SkipWhiteSpace();
	
	if (ConvertToUpperCase(mScanner->Peek(0)) == 'I') {
		if (ConvertToUpperCase(mScanner->Peek(1)) == 'N') {
			if (ConvertToUpperCase(mScanner->Peek(2)) == 'F') {
				mScanner->Advance(3);
				return true;
			}
		}
	}
	
	return false;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XSimpleParser
// ===================================================================================

//---------------------------------------------------------------
//
// XSimpleParser::~XSimpleParser
//
//---------------------------------------------------------------
XSimpleParser::~XSimpleParser()
{
	delete mScanner;
	delete mIntParser;
}


//---------------------------------------------------------------
//
// XSimpleParser::XSimpleParser (wstring)
//
//---------------------------------------------------------------
XSimpleParser::XSimpleParser(const std::wstring& text) 
{
	mScanner = nil;
	mIntParser = nil;
	
	mScanner = new XScanner(text);
	mIntParser = new XIntegerParser();
	
	mIntParser->SetScanner(mScanner);
}


//---------------------------------------------------------------
//
// XSimpleParser::XSimpleParser (XScanner*, XIntegerParser*)
//
//---------------------------------------------------------------
XSimpleParser::XSimpleParser(XScanner* takeScanner, XIntegerParser* intParser)
{
	PRECONDITION(takeScanner != nil);
	
	mScanner = nil;
	mIntParser = nil;
	
	mScanner = takeScanner;
	
	if (intParser == nil)
		intParser = new XIntegerParser();
	mIntParser = intParser;
	
	mIntParser->SetScanner(mScanner);
}


//---------------------------------------------------------------
//
// XSimpleParser::SkipPast
//
//---------------------------------------------------------------
void XSimpleParser::SkipPast(wchar_t theChar)
{
	while (this->Peek() != theChar) {		// It'd be more natural to write this using ParseChar, but then we couldn't skip past white space (eg '\n')
		this->Advance();

		if (mScanner->AtEnd())
			this->DoParseError();
	}

	if (this->Peek() == theChar) 
		this->Advance();
}


//---------------------------------------------------------------
//
// XSimpleParser::SkipPast
//
//---------------------------------------------------------------
void XSimpleParser::SkipPast(const std::wstring& str, bool useCase)
{
	while (!this->Found(str, useCase)) {
		this->Advance();
		
		if (this->AtEnd())
			this->DoParseError();
	}
}


//---------------------------------------------------------------
//
// XSimpleParser::Found (wchar_t, bool)
//
//---------------------------------------------------------------
bool XSimpleParser::Found(wchar_t theChar, bool useCase)
{
	this->SkipWhiteSpace();
	
	wchar_t ch = this->Peek();
	bool found = Equal(ch, theChar, useCase);
	if (found)
		this->Advance();
		
	return found;
}


//---------------------------------------------------------------
//
// XSimpleParser::Found (wstring, bool)
//
//---------------------------------------------------------------
bool XSimpleParser::Found(const std::wstring& str, bool useCase)
{
	this->SkipWhiteSpace();

	for (int32 index = 0; index < str.length(); index++) {		
		wchar_t ch = this->Peek(index);
		if (!Equal(ch, str[(uint32) index], useCase))
			return false;
	}
	
	this->Advance((int32) str.length());
		
	return true;
}


//---------------------------------------------------------------
//
// XSimpleParser::ParseChar
//
//---------------------------------------------------------------
wchar_t XSimpleParser::ParseChar(bool useCase)
{
	if (this->AtEnd())
		this->DoParseError();
		
	wchar_t ch = this->Peek();
	this->Advance();
	
	if (!useCase)
		ch = ConvertToLowerCase(ch);
		
	return ch;
}


//---------------------------------------------------------------
//
// XSimpleParser::ParseChars
//
//---------------------------------------------------------------
std::wstring XSimpleParser::ParseChars(const XRegularExpression& regExp, bool useCase)
{
	std::wstring chars;
	
	this->SkipWhiteSpace();
	
	try {
		const std::wstring& str = mScanner->GetText();
		uint32 start = (uint32) mScanner->GetPosition().GetIndex();
		
		uint32 count = regExp.Match(str, start);
		chars = str.substr(start, count);
	
		this->Advance((int32) count);
		
	} catch (...) {
		throw XParserException(mScanner, LoadWhisperString(L"Not enough memory."));	// If the string has growing pains we want to convert the exception to a parser exception.
	}
	
	if (chars.length() == 0)
		this->DoParseError();
		
	if (!useCase)	
		chars = ConvertToLowerCase(chars);
		
	return chars;
}


//---------------------------------------------------------------
//
// XSimpleParser::ParseQuote
//
//---------------------------------------------------------------
std::wstring XSimpleParser::ParseQuote(bool useCase, wchar_t quoteMark)
{
	std::wstring str;
	
	this->SkipWhiteSpace();
	
	wchar_t ch = this->Peek();
	if (ch != quoteMark)
		this->DoParseError();	
		
	this->Advance();
	ch = this->Peek();

	try {
		while (ch != quoteMark) {
			if (!useCase)
				ch = ConvertToLowerCase(ch);

			str += ch;
			
			this->Advance();
			ch = this->Peek();
		}
		
		this->Advance();
		
	} catch (...) {
		throw XParserException(mScanner, LoadWhisperString(L"Not enough memory."));	// If the string has growing pains we want to convert the exception to a parser exception.
	}
		
	return str;
}


//---------------------------------------------------------------
//
// XSimpleParser::ParseBool
//
//---------------------------------------------------------------
bool XSimpleParser::ParseBool()
{
	bool value = false;
	
	if (this->Found(L"true"))
		value = true;		
	else if (this->Found(L"false"))
		value = false;		
	else
		this->DoParseError();
		
	return value;
}


//---------------------------------------------------------------
//
// XSimpleParser::ParseInt16
//
//---------------------------------------------------------------
short XSimpleParser::ParseInt16()
{
	return mIntParser->ParseInt16();
}


//---------------------------------------------------------------
//
// XSimpleParser::ParseInt32
//
//---------------------------------------------------------------
int32 XSimpleParser::ParseInt32()
{
	return mIntParser->ParseInt32();
}


//---------------------------------------------------------------
//
// XSimpleParser::AtEnd
//
//---------------------------------------------------------------
bool XSimpleParser::AtEnd() const
{
	mScanner->SkipWhiteSpace();
	
	return mScanner->AtEnd();
}


//---------------------------------------------------------------
//
// XSimpleParser::HandleError
//
//---------------------------------------------------------------
void XSimpleParser::HandleError(const std::wstring& mesg)
{
	this->DoParseError(&mesg);
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XSimpleParser::DoParseError
//
//---------------------------------------------------------------
void XSimpleParser::DoParseError(const std::wstring* mesg) const
{	
	if (mesg)
		throw XParserException(mScanner, *mesg);
	else
		throw XParserException(mScanner, LoadWhisperString(L"Unknown error."));
}


}	// namespace Whisper

