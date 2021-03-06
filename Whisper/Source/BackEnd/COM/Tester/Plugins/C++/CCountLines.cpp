/*
 *  File:       CCountLines.h
 *  Summary:   	Implmentation of an ICountLines interface.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CCountLines.cpp,v $
 *		Revision 1.2  2000/11/09 08:54:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	12/02/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include "ICountLines.h"

#include <XInterfaceMacros.h>
#include <XStringUtils.h>
#include <XTinyVector.h>

#include "ICountSemiColons.h"


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
class CCountLines : public ICountLines, public ICountSemiColons {

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
	virtual uint32 		CountSemiColons(const std::wstring& text) const;

	virtual uint32 		CountLines(const std::wstring& text) const;
	
	virtual uint32 		GetNumExtensions() const;
	
	virtual std::wstring GetExtension(uint32 index) const;
	
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
CCountLines::CCountLines(XBoss* boss) : mExtensions(9)
{
	ICountLines::DoSetBoss(boss);
	ICountSemiColons::DoSetBoss(boss);

	mExtensions[0] = L"h";		
	mExtensions[1] = L"c";
	mExtensions[2] = L"cc";
	mExtensions[3] = L"cp";
	mExtensions[4] = L"cpp";
	mExtensions[5] = L"hpp";
	mExtensions[6] = L"c++";
	mExtensions[7] = L"inl";
	mExtensions[8] = L"inc";
}


//---------------------------------------------------------------
//
// CCountLines::CountSemiColons
//
//---------------------------------------------------------------
uint32 CCountLines::CountSemiColons(const std::wstring& text) const
{
	int32 count = std::count(text.begin(), text.end(), L';');
	
	return (uint32) count;
}


//---------------------------------------------------------------
//
// CCountLines::CountLines
//
// line = (NewComment || ((char* || OldComment)* NewComment?)) Return
//
// NewComment = '//' char*
//
// OldComment = '/*' char* '*/'
//
//---------------------------------------------------------------
uint32 CCountLines::CountLines(const std::wstring& text) const
{
	uint32 numLines = 0;
	
	uint32 index = 0;
	while (index < text.length()) {
		if (IsWhiteSpace(text[index])) 
			index++;
			
		else if (index+1 < text.length()) {
			if (text[index] == '/' && text[index+1] == '/') {
				while (!IsEOLChar(text[index]) && index < text.length()) {
					index++;
				}
					
			} else {
				bool foundChar = false;
				
				while (!IsEOLChar(text[index]) && index+1 < text.length()) {
					if (text[index] == '/' && text[index+1] == '*') {
						index += 2;
						
						while (index+1 < text.length() && (text[index] != '*' || text[index+1] != '/')) {
							index++;
						}
						index += 2;
							
					} else if (text[index] == '/' && text[index+1] == '/') {
						while (!IsEOLChar(text[index]) && index < text.length()) {
							index++;
						}

					} else {						
						index++;
						foundChar = true;
					}
				}
				
				if (foundChar)
					numLines++;
			}
					
		} else
			index++;
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

