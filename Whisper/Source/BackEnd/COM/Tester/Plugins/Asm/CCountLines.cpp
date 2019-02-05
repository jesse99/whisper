/*
 *  File:       CCountLines.cpp
 *  Summary:   	Implmentation of an ICountLines interface.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CCountLines.cpp,v $
 *		Revision 1.2  2000/11/09 08:54:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	12/02/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include "ICountLines.h"

#include <XInterfaceMacros.h>
#include <XStringUtils.h>
#include <XTinyVector.h>


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// IsEOLChar
//
//---------------------------------------------------------------
inline bool IsEOLChar(int ch)
{
	return ch == '\r' || ch == '\n';
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class CCountLines
// ===================================================================================
class CCountLines : public ICountLines {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~CCountLines();

						CCountLines(XBoss* boss);

//-----------------------------------
//	Inherited API
//
public:
	virtual uint32 		CountLines(const std::wstring& text) const;
	
	virtual uint32 		GetNumExtensions() const;
	
	virtual std::wstring 	GetExtension(uint32 index) const;
	
private:
	XTinyVector<std::wstring> mExtensions;
};

DEFINE_INTERFACE_FACTORY(CCountLines)

//---------------------------------------------------------------
//
// CCountLines::~CCountLines
//
//---------------------------------------------------------------
CCountLines::~CCountLines()
{
}


//---------------------------------------------------------------
//
// CCountLines::CCountLines
//
//---------------------------------------------------------------
CCountLines::CCountLines(XBoss* boss) : mExtensions(1)
{
	this->DoSetBoss(boss);

	mExtensions[0] = L"a";		
}


//---------------------------------------------------------------
//
// CCountLines::CountLines
//
// Line = Comment | Code
//
// Comment = '*' char* '\r'
//
// Code = char* '\r'
//
//---------------------------------------------------------------
uint32 CCountLines::CountLines(const std::wstring& text) const
{
	uint32 numLines = 0;
	
	uint32 index = 0;
	while (index < text.length()) {
		if (text[index] == '*') {
			while (!IsEOLChar(text[index]) && index < text.length())
				++index;
		
		} else {
			bool foundChar = false;

			while (!IsEOLChar(text[index]) && index < text.length()) {
				if (IsWhiteSpace(text[index])) {
					++index;
			
				} else {						
					++index;
					foundChar = true;
				}
			}
			
			if (foundChar)
				++numLines;
		}
		
		++index;
	}
	
	return numLines;
}

	
//---------------------------------------------------------------
//
// CCountLines::GetNumExtensions
//
//---------------------------------------------------------------
uint32 CCountLines::GetNumExtensions() const
{
	uint32 count = mExtensions.size();
	
	return count;
}

	
//---------------------------------------------------------------
//
// CCountLines::GetExtension
//
//---------------------------------------------------------------
std::wstring CCountLines::GetExtension(uint32 index) const
{
	const std::wstring& extension = mExtensions[index];
	
	return extension;
}

