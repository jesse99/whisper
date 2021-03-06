/*
 *	File:		XURI.cpp
 *	Function:	Encapsulates a Uniform Resource Identifier.
 *	Written by:	Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):	
 *
 *		$Log: XURI.cpp,v $
 *		Revision 1.4  2001/04/27 04:37:59  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/02/12 10:37:05  jesjones
 *		GetFile works if the path is a root level file (fix from Thomas Powers).
 *		
 *		Revision 1.2  2000/11/09 12:58:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	  7/09/99	JDJ		Added an ASSERT to the XURI (XString) ctor to check
 *									that file URI path's are absolute.
 *		 <2>	  3/23/99	JDJ		Added code to handle "." and ".." when merging uri's.
 *		 <1>	  2/06/99	JDJ		Created
 */
 
#include <XWhisperHeader.h>
#include <XURI.h>

#include <stdexcept>
#include <string>

#include <XFileSpec.h>
#include <XFolderSpec.h>
#include <XStringUtils.h>
#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// StripFileName
//
// Checks to see if the uri's path includes a filename. If one
// is present it is stripped off and a path to the folder is
// returned.
//
//---------------------------------------------------------------
static std::wstring StripFileName(const XURI& uri)
{	
	std::wstring path = uri.GetPath();
	
	if (*(path.end()-1) != '/')	{
		uint32 index = path.rfind('/');
		if (index < std::wstring::npos)
			path = path.substr(0, index+1);
		else
			path.resize(0);
	}

	POSTCONDITION(path.length() == 0 || *(path.end()-1) == '/');

	return path;
}


//---------------------------------------------------------------
//
// GetParent
//
// Returns the parent folder for a path.
//
//---------------------------------------------------------------
static std::wstring GetParent(const std::wstring& path)
{
	std::wstring parent;
	
	if (path.length() > 0) {
		if (*(path.end()-1) == '/') {
			uint32 index = path.rfind('/', path.length() - 2);
			if (index < std::wstring::npos)
				parent = path.substr(0, index+1);
					
		} else {
			uint32 index = path.rfind('/');
			if (index < std::wstring::npos)
				parent = path.substr(0, index+1);
		}
	}
		
	return parent;
}


//---------------------------------------------------------------
//
// MergePaths
//
// Merges two paths. Path1 should point to a folder. Path2
// should be a relative path.
//
//---------------------------------------------------------------
static std::wstring MergePaths(const std::wstring& path1, const std::wstring& path2)
{
	PRECONDITION(path1.length() == 0 || *(path1.end()-1) == '/');
	PRECONDITION(path2.length() == 0 || path2[0] != '/');
		
	std::wstring root = path1;
	std::wstring leaf = path2;
	
	while (leaf.length() > 1 && leaf[0] == '.') {
		if (leaf[1] == '/')
			leaf = leaf.substr(2); 
	
		else if (leaf.length() > 2 && leaf[1] == '.' && leaf[2] == '/') {
			root = GetParent(root); 
			leaf = leaf.substr(3); 
			
		} else
			break;
	}
		
	return root + leaf;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XURI
// ===================================================================================

//---------------------------------------------------------------
//
// XURI::~XURI
//
//---------------------------------------------------------------
XURI::~XURI()
{
}


//---------------------------------------------------------------
//
// XURI::XURI ()
//
//---------------------------------------------------------------
XURI::XURI()
{
	mScheme = L"empty";
}


//---------------------------------------------------------------
//
// XURI::XURI (wstring)
//
// $$$ This class doesn't fully conform to the URI spec. When
// $$$ it's rewritten to be conforming we probably want to relax
// $$$ the rules on which characters are valid inside the path.
//
//---------------------------------------------------------------
XURI::XURI(const std::wstring& uri)
{
	mScheme = Whisper::Before(uri, L":");
	if (mScheme.length() == 0)
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"The URI <#1> is missing the scheme.", uri)));

	if (uri.length() < mScheme.length() + 3)
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"The URI <#1> is missing '://' after the scheme.", uri)));
			
	mPath = Whisper::After(uri, L"://");
			
	if (mPath.find('%') != std::wstring::npos)
		mPath = this->DoUnescape(mPath);	

	mScheme = ConvertToLowerCase(mScheme);					// "For resiliency, programs interpreting URI should treat upper case letters as equivalent to lower case in scheme names"

	ASSERT_IF(mScheme == L"file", mPath.find('/') != std::wstring::npos);	// should be an absolute path
}


//---------------------------------------------------------------
//
// XURI::XURI (XURI, wstring)
//
//---------------------------------------------------------------
XURI::XURI(const XURI& baseURI, const std::wstring& uri)
{
	if (Whisper::Before(uri, L":").length() > 0) {		// uri is absolute and includes scheme
		*this = XURI(uri);
		
	} else if (uri.length() > 0 && uri[0] == '/') {		// uri is absolute, but missing scheme
		mScheme = baseURI.mScheme;
		mPath = uri.substr(1);
		
	} else {											// uri is relative to baseURI
		std::wstring base = Whisper::StripFileName(baseURI);

		mScheme = baseURI.mScheme;	
		mPath   = Whisper::MergePaths(base, uri);
	}
			
	if (mPath.find('%') != std::wstring::npos)
		mPath = this->DoUnescape(mPath);	
}


//---------------------------------------------------------------
//
// XURI::XURI (XFileSpec)
//
//---------------------------------------------------------------
XURI::XURI(const XFileSpec& spec)
{
	mScheme = L"file";
	mPath = spec.GetPath();

	for (uint32 index = 0; index < mPath.length(); ++index) {
#if MAC
		if (mPath[index] == ':')
			mPath[index] = '/';
#elif WIN
		if (mPath[index] == '\\')
			mPath[index] = '/';
#endif
	}
}


//---------------------------------------------------------------
//
// XURI::XURI (XFolderSpec)
//
//---------------------------------------------------------------
XURI::XURI(const XFolderSpec& spec)
{
	mScheme = L"file";
	mPath = spec.GetPath();
	
	for (uint32 index = 0; index < mPath.length(); ++index) {
#if MAC
		if (mPath[index] == ':')
			mPath[index] = '/';
#elif WIN
		if (mPath[index] == '\\')
			mPath[index] = '/';
#endif
	}
	
	POSTCONDITION(*(mPath.end()-1) == '/');
}


//---------------------------------------------------------------
//
// XURI::GetAddress
//
// CW Pro 5 won't inline this...
//
//---------------------------------------------------------------
std::wstring XURI::GetAddress() const						
{
	return mScheme + L"://" + mPath;
}


//---------------------------------------------------------------
//
// XURI::GetFile
//
//---------------------------------------------------------------
std::wstring XURI::GetFile() const
{
	std::wstring file;
	
	uint32 index = mPath.rfind('/');
	if (index != std::wstring::npos)
		file = mPath.substr(index+1);
	else 
		file = mPath;
		
	return file;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XURI::DoUnescape
//
//---------------------------------------------------------------
std::wstring XURI::DoUnescape(const std::wstring& inName)
{
	std::wstring outName;
	outName.reserve(inName.length());
	
	uint32 index = 0;
	while (index < inName.length()) {
		if (inName[index] == '%') {
			if (index+2 < inName.length() && IsHexDigit(inName[index+1]) && IsHexDigit(inName[index+2])) {
				outName += (wchar_t) (16*inName[index+1] + inName[index+2]);
				index += 2;
			
			} else
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"The URI <#1> has a bad escape sequence ('%' must be followed by two hex digits).", this->GetAddress())));
		
		} else
			outName += inName[index];
		++index;
	}
	
	return outName;
}


}	// namespace Whisper
