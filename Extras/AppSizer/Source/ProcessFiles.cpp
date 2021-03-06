/*
 *  File:       ProcessFiles.cpp
 *  Summary:   	The thread used to process files.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ProcessFiles.cpp,v $
 *		Revision 1.2  2001/01/01 03:50:44  jesjones
 *		Misc changes for an 8x speed increase.
 *		
 *		Revision 1.1.1.1  2000/12/27 03:12:52  jesjones
 *		First Imported.		
 */

#include "AppHeader.h"
#include "ProcessFiles.h"

#include <cctype>

#include <XFile.h>
#include <XFileSystem.h>
#include <XFolderSpec.h>
#include <XHandle.h>
#include <XIntConversions.h>
#include <XLocker.h>
#include <XMiscUtils.h>
#include <XStringUtils.h>
#include <XTranscode.h>


//-----------------------------------
//	Globals
//
extern std::wstring gCodeSectionName;		// "Code section,"
extern std::wstring gImportSectionName;		// "Import container"
extern std::string gDataSectionName;		// "Data section"

extern vector<std::string> gNamespaces;		// "std", "Whisper"


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// UInt32ToNStr
//
// We'll define some ASCII versions of the Unicode functions here
// to avoid translating to Unicode.
// 
//---------------------------------------------------------------
static std::string UInt32ToNStr(uint32 n)
{
	char buffer[128];
	sprintf(buffer, "%lu", n);
	
	return buffer;
}


//---------------------------------------------------------------
//
// Before
//
//---------------------------------------------------------------
static std::string Before(const std::string& str, const std::string& sub)
{
	std::string result;
	
	uint32 index = str.find(sub);
	if (index != std::string::npos)
		result = str.substr(0, index);
	
	return result;
}


//---------------------------------------------------------------
//
// StripLeading
//
//---------------------------------------------------------------
static std::string StripLeading(const std::string& str, const std::string& padding)
{
	uint32 index = 0;
	while (index < str.length() && padding.find(str[index]) != std::string::npos)
		index++;
		
	return str.substr(index);
}


//---------------------------------------------------------------
//
// Parse
//
//---------------------------------------------------------------
static std::string Parse(std::string& str, const std::string& term)
{
	std::string token;
		
	// Start at the first character not in term.
	uint32 start = str.find_first_not_of(term);
		
	// Stop at the next occurance of a character in term.
	uint32 stop = str.find_first_of(term, start + 1);
		
	// If the start is valid return the token.
	if (start != std::string::npos) 										
		token = str.substr(start, stop-start);
		
	// Remove the token from the string.
	if (stop != std::string::npos)						
		str = str.substr(stop);						
	else					
		str.resize(0);	
		
	// Remove term chars from the start of the string. (Makes it easier
	// to determine when parsing is complete).
	str = StripLeading(str, term);
										
	return token;
}


//---------------------------------------------------------------
//
// Unmangle
//
// Incomplete reverse engineered unmangler. May not be 100% 
// correct. Here are some examples of mangled names:
//
// .HandleColorChange
// .CheckCFMLink__7WhisperFPCv					
// .OnQuit__12CApplicationFv
// .SetMemory__7WhisperFPvUcUl
// .OnDo__Q27Whisper23XCutCommand<9CDocument>Fv
// .Draw__Q27Whisper12XPixMapShapeFRQ27Whisper12XDrawContextRCQ27Whisper7XPixMapRCQ27Whisper5XRectRCQ27Whisper5XRectQ27Whisper9EDrawMode
// .@128@Invariant__Q27Whisper11XTabControlCFv
// .__ls__FRQ27Whisper10XOutStreamRC11CIconBitMap
// 
//---------------------------------------------------------------
static std::string Unmangle(const std::string& inStr) 		
{
	std::string outStr;
	
	uint32 index = 0;
	if (inStr[index] == '.')
		++index;
		
	if (inStr[index] == '@') {
		++index;
		while (index < inStr.length() && inStr[index] >= '0' && inStr[index] <= '9')
			++index;

		ASSERT(inStr[index] == '@');
		++index;
	}

	if ((inStr[index] >= 'a' && inStr[index] <= 'z') || (inStr[index] >= 'A' && inStr[index] <= 'Z') || inStr[index] == '_') {
		uint32 k = inStr.find("__", index+1);
		if (k != std::string::npos) {
		
			// Get the function name
			std::string fnName = inStr.substr(index, k-index);	
			index += 2 + fnName.length();
			
			if (fnName.find("__ls") == 0)
				fnName = "operator<<";
			else if (fnName.find("__rs") == 0)
				fnName = "operator>>";
				
			uint32 z = fnName.find('<');
			if (z != std::string::npos)
				fnName = fnName.substr(0, z);
		
			// If we hit a 'Q' we have namespaces
			while (index < inStr.length() && inStr[index] != 'Q' && ((inStr[index] >= 'a' && inStr[index] <= 'z') || (inStr[index] >= 'A' && inStr[index] <= 'Z'))) 
				++index;
			
			uint32 numNames = 1;
			if (index < inStr.length() && inStr[index] == 'Q') {
				numNames = (uint32) (inStr[index+1] - '0');
				index += 2;
			}
			
			for (uint32 i = 0; i < numNames; ++i) {
			
				// Names start with a decimal length field
				uint32 length = 0;
				while (index < inStr.length() && inStr[index] >= '0' && inStr[index] <= '9')
					length = 10*length + (inStr[index++] - '0');
				
				// Followed by the name
				if (length > 0) {
					if (outStr.length() > 0)
						outStr += "::";
						
					for (uint32 k = 0; k < length; ++k) {
						if (IsLetter(inStr[index+k]) || IsArabicDigit(inStr[index+k]) || inStr[index+k] == '_')	// don't include mangled argument names
							outStr += inStr[index+k];
						else
							break;
					}
					index += length;
					
				} else {
//					DEBUGSTR("Couldn't unmangle %ls", inStr.w_str());
					return inStr;
				}
			}
			
			outStr = outStr + "::" + fnName;
			
		} else
			outStr = inStr.substr(index);			// global C function
	
	} else
		outStr = inStr;								// invalid mangled name
	
	return outStr;
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
CProcessFiles::CProcessFiles(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs, const vector<XHandle>& files, ErrorCallback err) : XProcessFiles(fileFilter, folderFilter, specs, err), mFiles(files)
{
	mIter = nil;

	// This is used in the progress dialog to generate messages
	// that look like "Processing: Foo.txt". You might want to use
	// a more meaningful string.
	mMesgPrefix = LoadAppString(L"Processing: ");
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CProcessFiles::OnProcessFile
//
//---------------------------------------------------------------
void CProcessFiles::OnProcessFile(const XFileSpec& spec)	
{
#if MAC
	ASSERT(spec.GetType() == 'TEXT');
#endif

	const XHandle& data = mFiles[mCount];
	XLocker lock(data);
		
	const char* begin = (const char*) data.GetPtr();
	mEnd = begin + data.GetSize();
	
	mIter = std::search(begin, mEnd, gCodeSectionName.c_str(), gCodeSectionName.c_str() + gCodeSectionName.length());				
	ASSERT(mIter != mEnd);
		
	uint32 startCount = mCount;
	mLine = 1UL + std::count(begin, mIter, '\n');		// Pro 6 saves map files as Unix...
	
	begin = mIter;

	mEnd = std::search(begin, mEnd, gImportSectionName.c_str(), gImportSectionName.c_str() + gImportSectionName.length());	
	
	this->DoSkipPastEOL();
	++mLine;
	
	mEntries.clear();
	mOffset = 0;

	mOffset = this->DoParseOffset();
	ASSERT(mOffset == 0);
	
	while (mIter < mEnd && !mAborted) {
		XThread::Yield();
		mCount = startCount + (mIter - begin);
	
		this->DoProcessLine();		

		{
		XEnterCriticalSection enter(mMesgMutex);		// critical sections are quite fast if there's no contention so this should be OK
			mMessage = LoadAppString(L"Processing line: #1", UInt32ToStr(mLine));
		}
	}
	
	if (!mAborted)
		this->DoWriteInfo(spec);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CProcessFiles::DoWriteInfo
//
//---------------------------------------------------------------
void CProcessFiles::DoWriteInfo(const XFileSpec& mapFile)
{
	const uint32 kMaxEntries = 100;
	const uint32 kNameWidth  = 32;
	
	std::wstring name = XFileSystem::GetValidName(mapFile.GetName() + L" Sizes", mapFile.GetVolume());
	XFileSpec spec(mapFile.GetParent(), name);
	
	{
	XEnterCriticalSection enter(mMesgMutex);	
		mMessage = LoadAppString(L"Sorting...");
	}
	XThread::Yield();

	vector<SEntry> entries(mEntries.begin(), mEntries.end());
	sort(entries.begin(), entries.end(), CProcessFiles::CompareSize());
	
	XFile file(spec);	
	file.Open('CWIE', 'TEXT', kWritePermission);
	file.SetLength(0);
	
	std::string text;
	uint32 i;
	for (i = 0; i < entries.size(); ++i) {
		if ((i & 0x0F) == 0) {
			{
			XEnterCriticalSection enter(mMesgMutex);		
				mMessage = LoadAppString(L"Writing entry: #1", UInt32ToStr(i));
			}
			XThread::Yield();
		}
		
		const SEntry& entry = entries[i];
		text = entry.name;
		if (text.length() < kNameWidth)
			text += std::string(kNameWidth - text.length(), ' ');
		else
			text += " ";
		
		if (entry.bytes >= 1024)
			text += UInt32ToNStr(entry.bytes/1024) + "K";
		else
			text += UInt32ToNStr(entry.bytes);
			
		text += "\r";
	
		file.Write(text.c_str(), text.length());
	}
	
	vector<uint32> namespaceBytes(gNamespaces.size(), 0UL);
	
	uint32 bytes = 0;
	for (i = 0; i < entries.size(); ++i) {
		const SEntry& entry = entries[i];
		
		bytes += entry.bytes;
		
		std::string name = Before(entry.name, "::");
		if (name.length() > 0) {
			vector<std::string>::iterator iter = find(gNamespaces.begin(), gNamespaces.end(), name);
			if (iter != gNamespaces.end())
				namespaceBytes[uint32(iter - gNamespaces.begin())] += entry.bytes;
		}
	}

	text = "\rCode and data bytes = " + UInt32ToNStr(bytes/1024) + "K\r";
	file.Write(text.c_str(), text.length());

	for (i = 0; i < namespaceBytes.size(); ++i) {
		bytes = namespaceBytes[i];
		if (bytes >= 1024) {
			text = gNamespaces[i] + " bytes = " + UInt32ToNStr(bytes/1024) + "K\r";
			file.Write(text.c_str(), text.length());

		} else if (bytes > 0) {
			text = gNamespaces[i] + " bytes = " + UInt32ToNStr(bytes) + "\r";
			file.Write(text.c_str(), text.length());
		}
	}

	file.Close();
}


//---------------------------------------------------------------
//
// CProcessFiles::DoProcessLine
//
// DS = Descriptor
// TI = Exception Handler Indexes (addressed relative to the Code Section)
// TC = TOC (i.e. pointer to variable)
// TD = TOC Data (i.e. small data stored directly in the TOC)
// TB = Exception Handler data for unwinding the stack
// RW = Read Write Memory
// PR = Read Only Program Code
// GL = Linker Generated Glue routines to call functions in other dynamic (shared) libraries
// RO = Read Only Data (read only strings etc)
//
//---------------------------------------------------------------
void CProcessFiles::DoProcessLine()
{	
	this->DoSkipWhiteSpace();
	FastString type(mIter, 2);
	mIter += 2;
	this->DoSkipWhiteSpace();
		
	FastString inName = this->DoParseName();	
	if (type == "GL") 
		mName = "shared library glue";
	else if (inName.find("__RTTI") != std::string::npos) 
		mName = "RTTI";
	else if (inName.find("__vt") != std::string::npos) 
		mName = "vtables";
	else if (inName.find("__sinit") != std::string::npos) 
		mName = "static objects";
	else if (inName.find("ExPPC") != std::string::npos || type == "TI" || type == "TB") 
		mName = "exceptions";
	else if (inName.find("stringBase") != std::string::npos) 
		mName = "wide string literals";
	else if (inName.find("stringBase") != std::string::npos) 
		mName = "narrow string literals";
	else if (type == "RO") 
		mName = "read-only data";
	else if (type == "RW") 
		mName = "read-write data";
	else if (type == "TC" || type == "TD") 
		mName = "TOC data";
	else 
		mName = Unmangle(std::string(inName.begin(), inName.end()));
		
	mEntry.name.clear();
	mEntry.bytes = 0;	
		
	bool isNamespace = true;						// copy over names until we hit the first one that's not a namespace
	while (mName.length() > 0 && isNamespace) {
		mTemp = Parse(mName, "::");
		isNamespace = find(gNamespaces.begin(), gNamespaces.end(), mTemp) != gNamespaces.end();
		
		if (mEntry.name.length() > 0)
			mEntry.name += "::";
		mEntry.name += mTemp;
	}
		
	this->DoSkipPastEOL();
	++mLine;
	
	while (*mIter == '\r' || *mIter == '\n') {
		this->DoSkipPastEOL();
		++mLine;
	}
	
	if (strncmp(mIter, gDataSectionName.c_str(), gDataSectionName.length()) == 0) {				
		this->DoSkipPastEOL();
		++mLine;
		
		mOffset = this->DoParseOffset();
		ASSERT(mOffset == 0);
			
	} else if (mIter < mEnd) {						
		uint32 offset = this->DoParseOffset();
		ASSERT(offset > mOffset);
		
		mEntry.bytes = offset - mOffset;
		mOffset = offset;
		
		if (mEntry.name.length() > 0) {
			Entries::const_iterator iter = mEntries.find(mEntry);
			if (iter != mEntries.end()) {
				SEntry& old = const_cast<SEntry&>(*iter);
				old.bytes += mEntry.bytes;
			
			} else
				mEntries += mEntry;		
		}
	}
}


//---------------------------------------------------------------
//
// CProcessFiles::DoSkipWhiteSpace
//
//---------------------------------------------------------------
void CProcessFiles::DoSkipWhiteSpace()
{
	while (isspace(*mIter))
		++mIter;
}


//---------------------------------------------------------------
//
// CProcessFiles::DoSkipPastEOL
//
//---------------------------------------------------------------
void CProcessFiles::DoSkipPastEOL()
{
	while (*mIter > 0 && *mIter != '\r' && *mIter != '\n')		
		++mIter;

	while (*mIter == '\r' || *mIter == '\n')		
		++mIter;
}


//---------------------------------------------------------------
//
// CProcessFiles::DoParseName
//
//---------------------------------------------------------------
FastString CProcessFiles::DoParseName()
{
	const char* str = mIter;
	uint32 len = 0;
	
	while (!isspace(*mIter)) {
		++len;
		++mIter;
	}
		
	return FastString(str, len);		// very long names may have an ellipsis at the end so we can't use FromAsciiStr
}


//---------------------------------------------------------------
//
// CProcessFiles::DoParseOffset
//
//---------------------------------------------------------------
uint32 CProcessFiles::DoParseOffset()
{
	std::wstring text(L"$");
	
	while (!isspace(*mIter))
		text += *mIter++;
		
	return StrToUInt32(text);
}


