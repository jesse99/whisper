/*
 *  File:       CProcessFiles.cpp
 *  Summary:   	The thread used to process files.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ProcessFiles.cpp,v $
 *		Revision 1.2  2001/04/21 03:29:53  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.1  2001/03/05 04:51:50  jesjones
 *		Checked in	
 */

#include "AppHeader.h"
#include "ProcessFiles.h"

#include <XDate.h>
#include <XFile.h>
#include <XFolderSpec.h>
#include <XIntConversions.h>
#include <XStringUtils.h>
#include <XTinyVector.h>
#include <XTranscode.h>


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// FormatAsHTML
//
//---------------------------------------------------------------
static std::string FormatAsHTML(const std::string& inText)
{
	std::string text;
	text.reserve(inText.length());
	
	for (uint32 i = 0; i < inText.length(); ++i) {
		char ch = inText[i];
		
		switch (ch) {
			case '<':
				text += "&lt;";
				break;
				
			case '>':
				text += "&gt;";
				break;
				
			case '&':
				text += "&amp;";
				break;
				
			case '"':
				text += "&quot;";
				break;
				
			default:
				text += ch;
		}
	}
	
	return text;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	struct SDate
// ===================================================================================

//---------------------------------------------------------------
//
// SDate::ToStr
//
//---------------------------------------------------------------
std::string CProcessFiles::SDate::ToStr() const
{	
	PRECONDITION(month >= 1);
	PRECONDITION(month <= 12);
	
	static const wchar_t* months[12] = {L"January ", L"February ", L"March ", L"April ", L"May ", L"June ", L"July ", L"August ", L"September ", L"October ", L"November ", L"December "};
	std::wstring str = months[month-1] + Int32ToStr(year);
	
	return ToAsciiStr(str);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class CProcessFiles
// ===================================================================================

//---------------------------------------------------------------
//
// CProcessFiles::~CProcessFiles
//
//---------------------------------------------------------------
CProcessFiles::~CProcessFiles()
{
}


//---------------------------------------------------------------
//
// CProcessFiles::CProcessFiles
//
//---------------------------------------------------------------
CProcessFiles::CProcessFiles(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs, ErrorCallback err) : XProcessFiles(fileFilter, folderFilter, specs, err)
{
	mNumFiles = 0;

	mMesgPrefix = LoadAppString(L"Processing: ");
}

						
//---------------------------------------------------------------
//
// CProcessFiles::OnProcessFile
//
//---------------------------------------------------------------
void CProcessFiles::OnProcessFile(const XFileSpec& spec)	
{
	this->DoReadFile(spec);
	this->DoGetComment();
	
	mIndex = mText.find("$Log:");
	if (mIndex != std::string::npos) {
		++mNumFiles;
		
		this->DoSkipToNextLine();
		
		std::string fileName = ToPlatformStr(spec.GetName());
		while (this->DoProcessRevision(fileName))
			;
	}
	
	if (!mListing.empty())
		this->DoWriteListing();
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CProcessFiles::DoProcessRevision
//
// Process a line that looks like:
// 		*	Revision 1.8  2001/02/09 04:33:06  jesjones
// 		*	IChildNode method renaming.
// mIndex points to the start of the line.
//
//---------------------------------------------------------------
bool CProcessFiles::DoProcessRevision(const std::string& fileName)
{
	bool processed = false;
	
	this->DoSkipCommentStart();
	
	if (mText.compare(mIndex, std::strlen("Revision"), "Revision") == 0) {
		mIndex += std::strlen("Revision");	// "Revision"
		this->DoSkipWhiteSpace();
		
		this->DoSkipDigits();				// Revision number
		this->DoSkip('.');
		this->DoSkipDigits();
		this->DoSkipWhiteSpace();
		
		SDate date;
		date.year = this->DoParseNumber();	// Date
		this->DoSkip('/');
		date.month = this->DoParseNumber();	
		
		this->DoSkipToNextLine();
		this->DoSkipCommentStart();
		
		std::string comment;				// Comment
		while (mIndex < mLength && !IsWhiteSpace(mText[mIndex]) && mText[mIndex] != '/') {
			comment += this->DoParseLine();
		
			this->DoSkipToNextLine();
			this->DoSkipCommentStart();
		}

		History& history = mListing[date][fileName];
		history.push_back(comment);
		
		if (mIndex < mLength && IsWhiteSpace(mText[mIndex]))
			this->DoSkipWhiteSpace();
		
		processed = true;
	}
		
	return processed;
}


//---------------------------------------------------------------
//
// CProcessFiles::DoParseLine
//
// Returns the text from the current index to the end of the line.
//
//---------------------------------------------------------------
std::string CProcessFiles::DoParseLine()
{
	std::string line;
	line.reserve(128);

	while (mIndex < mLength && mText[mIndex] != '\r' && mText[mIndex] != '\n')
		line += mText[mIndex++];

	return FormatAsHTML(line);	
}


//---------------------------------------------------------------
//
// CProcessFiles::DoParseNumber
//
//---------------------------------------------------------------
int32 CProcessFiles::DoParseNumber()
{
	PRECONDITION(mIndex < mLength);
	PRECONDITION(IsArabicDigit(mText[mIndex]));
	
	int32 value = 0;
	while (mIndex < mLength && IsArabicDigit(mText[mIndex]))
		value = 10*value + (mText[mIndex++] - '0');

	return value;	
}


//---------------------------------------------------------------
//
// CProcessFiles::DoSkipCommentStart
//
// Skip the comment and whitespace at the start of a line.
//
//---------------------------------------------------------------
void CProcessFiles::DoSkipCommentStart()
{
	while (mIndex < mLength && (mText[mIndex] == ' ' || mText[mIndex] == '\t' || mText[mIndex] == '*'))
		++mIndex;
}


//---------------------------------------------------------------
//
// CProcessFiles::DoSkipToNextLine
//
//---------------------------------------------------------------
void CProcessFiles::DoSkipToNextLine()
{
	while (mIndex < mLength && mText[mIndex] != '\r' && mText[mIndex] != '\n')
		++mIndex;

	while (mIndex < mLength && (mText[mIndex] == '\r' || mText[mIndex] == '\n'))
		++mIndex;
}


//---------------------------------------------------------------
//
// CProcessFiles::DoSkipWhiteSpace
//
//---------------------------------------------------------------
void CProcessFiles::DoSkipWhiteSpace()
{
	while (mIndex < mLength && IsWhiteSpace(mText[mIndex]))
		++mIndex;
}


//---------------------------------------------------------------
//
// CProcessFiles::DoSkipDigits
//
//---------------------------------------------------------------
void CProcessFiles::DoSkipDigits()
{
	while (mIndex < mLength && (mText[mIndex] >= '0' && mText[mIndex] <= '9'))
		++mIndex;
}


//---------------------------------------------------------------
//
// CProcessFiles::DoSkip
//
//---------------------------------------------------------------
void CProcessFiles::DoSkip(char ch)
{
	PRECONDITION(mIndex < mLength);
	PRECONDITION(mText[mIndex] == ch);
	
	++mIndex;
}


//---------------------------------------------------------------
//
// CProcessFiles::DoGetComment
//
// Returns the comment block at the top of the file.
//
//---------------------------------------------------------------
void CProcessFiles::DoGetComment()
{	
	uint32 index = 0;	
	while (index < mText.length() && IsWhiteSpace(mText[index]))
		++index;
	
	if (index+1 < mText.length() && mText[index] == '/' && mText[index+1] == '*') {	// $$ assume that comment blocks are C-style
		index += 2; 
		
		while (index+1 < mText.length() && !(mText[index] == '*' && mText[index+1] == '/'))
			++index;
	}
	
	mLength = index;
}


//---------------------------------------------------------------
//
// CProcessFiles::DoReadFile
//
// Reads the entire file into a string.
//
//---------------------------------------------------------------
void CProcessFiles::DoReadFile(const XFileSpec& spec)
{	
	XFile file(spec);	
	file.Open(kReadPermission);

	uint32 bytes = (uint32) file.GetLength();
	XTinyVector<char> buffer(bytes);
		
	file.Read(buffer.buffer(), bytes);
	
	file.Close();

	mText = buffer.buffer(), bytes;
	
	mIndex = 0;
	mLength = mText.length();
}


//---------------------------------------------------------------
//
// CProcessFiles::DoWriteListing
//
//---------------------------------------------------------------
void CProcessFiles::DoWriteListing()
{
	std::string text;
	text.reserve(64*1024);
	
	text += "<HTML>\n";
	text += "<BODY>\n";
	
	text += "\t<H1><CENTER>";
	XDate date = XDate::GetCurrentDate();
	text += "Changes as of ";
	text += ToPlatformStr(date.GetDateString(false));
	text += "</CENTER></H1>\n\n";
	
	for (Listing::const_iterator iter = mListing.begin(); iter != mListing.end(); ++iter) {
		text += "\t<H3>" + iter->first.ToStr() + "</H3>\n";
		
		const FileHistories& histories = iter->second;
		for (FileHistories::const_iterator iter2 = histories.begin(); iter2 != histories.end(); ++iter2) {
			text += "\t\t<P>";
			text += iter2->first;
			text += "</P>\n";
			text += "\t\t<UL>\n";

			const History& history = iter2->second;
			for (History::const_iterator iter3 = history.begin(); iter3 != history.end(); ++iter3) {
				text += "\t\t\t<LI>";
				text += *iter3;
				text += "</LI>\n";
			}

			text += "\t\t</UL>\n\n";
		}
	}
	
	text += "</BODY>\n";
	text += "</HTML>\n";
	
	XFileSpec spec(XFolderSpec::GetAppFolder(), L"Changes.html");
	this->DoWriteFile(spec, text);
}


//---------------------------------------------------------------
//
// CProcessFiles::DoWriteFile
//
// Writes text out to the specified file.
//
//---------------------------------------------------------------
void CProcessFiles::DoWriteFile(const XFileSpec& spec, const std::string& text) const
{	
	XFile file(spec);	
	file.Open('MOSS', 'TEXT', kWritePermission);
	
	file.Write(text.c_str(), text.length());
	
	file.Trim();
	file.Close();
}

