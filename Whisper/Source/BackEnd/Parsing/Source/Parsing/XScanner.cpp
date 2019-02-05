/*
 *  File:		XScanner.cpp
 *  Summary:	A class that returns selected characters from a string.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XScanner.cpp,v $
 *		Revision 1.3  2001/04/27 04:35:59  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:50:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 3/14/00	JDJ		Made the whitespace more Unicode savvy.
 *		 <3>	 1/27/99	JDJ		Advance correctly updates mLine with Unix style line endings.
 *		 <2>	 1/26/99	JDJ		OnSkipWhiteSpace properly handles character codes
 *									larger than 255.
 *		 <1>	12/17/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XScanner.h>

#include <XDebug.h>
#include <XStringUtils.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const wchar_t kDefaultWhiteSpace[] = {kTabChar, 
									kLineFeedChar, 
									kReturnChar, 
									kSpaceChar, 
//									kNoBreakSpaceChar,
									kSixEmSpaceChar, 
									kZeroWidthSpaceChar, 
									kIdeographicSpcChar, 
									0};


// ===================================================================================
//	class XParserException
// ===================================================================================

//---------------------------------------------------------------
//
// XParserException::XParserException 
//
//---------------------------------------------------------------
XParserException::XParserException(const XScanner* scanner, const std::wstring& str) : std::runtime_error(ToUTF8Str(str))
{
	PRECONDITION(scanner != nil);
	
	mPos = scanner->GetPosition();
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XScanner
// ===================================================================================

//---------------------------------------------------------------
//
// XScanner::~XScanner
//
//---------------------------------------------------------------
XScanner::~XScanner()
{
}


//---------------------------------------------------------------
//
// XScanner::XScanner ()
//
//---------------------------------------------------------------
XScanner::XScanner() 
{
	mIndex = 0;
	mLine = 0;
	
	int32 i = 0;
	while (kDefaultWhiteSpace[i])
		mWhiteSpace.push_back(kDefaultWhiteSpace[i++]);

	std::sort(mWhiteSpace.begin(), mWhiteSpace.end());
}


//---------------------------------------------------------------
//
// XScanner::XScanner (wstring)
//
//---------------------------------------------------------------
XScanner::XScanner(const std::wstring& text) : mText(text)
{
	mIndex = 0;
	mLine = 0;

	int32 i = 0;
	while (kDefaultWhiteSpace[i])
		mWhiteSpace.push_back(kDefaultWhiteSpace[i++]);

	std::sort(mWhiteSpace.begin(), mWhiteSpace.end());
}


//---------------------------------------------------------------
//
// XScanner::SetWhiteSpace
//
//---------------------------------------------------------------
void XScanner::SetWhiteSpace(wchar_t ch, bool isWhite)	
{
	std::vector<wchar_t>::iterator iter = std::lower_bound(mWhiteSpace.begin(), mWhiteSpace.end(), ch);
	if (iter != mWhiteSpace.end() && *iter == ch) {
		if (!isWhite)
			mWhiteSpace.erase(iter);
		
	} else {
		if (isWhite)
			if (iter == mWhiteSpace.end())
				mWhiteSpace.push_back(ch);			
			else
				mWhiteSpace.insert(iter, ch);			
	}
}


//---------------------------------------------------------------
//
// XScanner::ClearWhiteSpace
//
//---------------------------------------------------------------
void XScanner::ClearWhiteSpace()
{
	mWhiteSpace.clear();
}


//---------------------------------------------------------------
//
// XScanner::SetText
//
//---------------------------------------------------------------
void XScanner::SetText(const std::wstring& text)
{
	mText  = text;							// don't test for equality!
	mIndex = 0;
	mLine  = 0;
}


//---------------------------------------------------------------
//
// XScanner::SetPosition
//
//---------------------------------------------------------------
void XScanner::SetPosition(const XScannerPos& pos)
{
	mIndex = pos.mIndex;
	mLine  = pos.mLine;
}


//---------------------------------------------------------------
//
// XScanner::Peek
//
//---------------------------------------------------------------
wchar_t XScanner::Peek(int32 offset) const
{
	wchar_t ch = '\0';
	
	uint32 index = mIndex + offset;
	
	if (index < mText.length())
		ch = mText[index];
		
	return ch;
}


//---------------------------------------------------------------
//
// XScanner::Advance
//
//---------------------------------------------------------------
void XScanner::Advance(int32 delta)
{
	if (delta > 0) {
		while (delta-- && mIndex < mText.length()) {
			if (mText[mIndex] == '\r' || (mText[mIndex] == '\n' && (mIndex == 0 || mText[mIndex-1] != '\r')))
				++mLine;

			++mIndex;
		}
		
	} else if (delta < 0) {
		while (delta++ && mIndex > 0) {
			if (mText[mIndex] == '\r' || (mText[mIndex] == '\n' && mText[mIndex-1] != '\r'))
				--mLine;

			--mIndex;
		}
	}
}


//---------------------------------------------------------------
//
// XScanner::SkipWhiteSpace
//
//---------------------------------------------------------------
void XScanner::SkipWhiteSpace()
{	
	while (!this->AtEnd()) {
		wchar_t ch = mText[mIndex];
		
		if (this->DoIsWhiteSpace(ch)) 
			this->Advance();			
		else 
			break;
	}		
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XScanner::DoIsWhiteSpace
//
//---------------------------------------------------------------
bool XScanner::DoIsWhiteSpace(wchar_t ch) const
{
	std::vector<wchar_t>::const_iterator iter = std::lower_bound(mWhiteSpace.begin(), mWhiteSpace.end(), ch);
	bool isWhite = iter != mWhiteSpace.end() && *iter == ch;
	
	return isWhite;
}


}	// namespace Whisper
