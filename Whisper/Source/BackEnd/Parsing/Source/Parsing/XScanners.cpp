/*
 *  File:		XScanners.cpp
 *  Summary:	XScanner subclasses.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XScanners.cpp,v $
 *		Revision 1.3  2001/04/27 04:36:02  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:50:29  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XScanners.h>

#include <XDebug.h>

namespace Whisper {


// ===================================================================================
//	class XCScanner
// ===================================================================================

//---------------------------------------------------------------
//
// XCScanner::~XCScanner
//
//---------------------------------------------------------------
XCScanner::~XCScanner()
{
}


//---------------------------------------------------------------
//
// XCScanner::XCScanner ()
//
//---------------------------------------------------------------
XCScanner::XCScanner() 
{
}


//---------------------------------------------------------------
//
// XCScanner::XCScanner (wstring)
//
//---------------------------------------------------------------
XCScanner::XCScanner(const std::wstring& text) : XScanner(text)
{
}


//---------------------------------------------------------------
//
// XCScanner::SkipWhiteSpace
//
//---------------------------------------------------------------
void XCScanner::SkipWhiteSpace()
{
	while (!this->AtEnd()) {
		wchar_t ch = mText[mIndex];
		
		if (this->DoIsWhiteSpace(ch)) 
			this->Advance();			

		else if (ch == '/' && this->Peek(1) == '*')		// Note that it's legal to Peek past the end.
			this->DoSkipCComment();
		
		else if (ch == '/' && this->Peek(1) == '/')
			this->DoSkipCPlusComment();
		
		else 
			break;
	}		
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XCScanner::DoSkipCComment
//
//---------------------------------------------------------------
void XCScanner::DoSkipCComment()
{
	PRECONDITION(this->Peek() == '/' && this->Peek(1) == '*');
	
	this->Advance(2);
	
	while (!this->AtEnd()) {								
		wchar_t ch = this->Peek();
		this->Advance();
		
		if (ch == '*' && this->Peek() == '/') {
			this->Advance();
			break;
		}
	}
}


//---------------------------------------------------------------
//
// XCScanner::DoSkipCPlusComment
//
//---------------------------------------------------------------
void XCScanner::DoSkipCPlusComment()
{
	PRECONDITION(this->Peek() == '/' && this->Peek(1) == '/');
	
	this->Advance(2);
	
	while (!this->AtEnd()) {								
		wchar_t ch = this->Peek();
		this->Advance();
		
		if (ch == '\r')
			break;
	}
}


}	// namespace Whisper
