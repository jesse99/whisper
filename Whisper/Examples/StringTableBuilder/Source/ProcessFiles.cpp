/*
 *  File:       ProcessFiles.cpp
 *  Summary:   	The thread used to process files.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ProcessFiles.cpp,v $
 *		Revision 1.5  2001/05/06 10:40:44  jesjones
 *		Updated for the XTranscode.h changes.
 *		
 *		Revision 1.4  2001/04/21 03:30:02  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2000/12/31 04:37:37  jesjones
 *		Fixed so that it works with std::wstring instead of XString.
 *		
 *		Revision 1.2  2000/11/09 12:02:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include "ProcessFiles.h"

#include <cctype>

#include <XAutoPtr.h>
#include <XDate.h>
#include <XFile.h>
#include <XFileSystem.h>
#include <XFolderSpec.h>
#include <XIntConversions.h>
#include <XResource.h>
#include <XStringUtils.h>
#include <XTinyVector.h>
#include <XURIResolver.h>
#include <XXMLCallback.h>
#include <XXMLParser.h>


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// ReadFile
//
//---------------------------------------------------------------
static std::wstring ReadFile(const XFileSpec& spec)
{	
	XFile file(spec);	
	file.Open(kReadPermission);

	uint32 bytes = (uint32) file.GetLength();
	XTinyVector<char> buffer(bytes);
		
	file.Read(buffer.buffer(), bytes);
	
	file.Close();

	std::wstring text = FromPlatformStr(buffer.buffer(), bytes);
	
	return text;		
}


//---------------------------------------------------------------
//
// WriteFile
//
//---------------------------------------------------------------
static void WriteFile(const XFileSpec& spec, const std::wstring& text)
{	
	XFile file(spec);	
	file.Open('CWIE', 'TEXT', kWritePermission);

	uint32 bytes = text.length();
	file.SetLength(bytes);
	
	UTF8String str = ToUTF8Str(text);
	file.Write(str.c_str(), str.length());
	
	file.Close();
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class CEntryBuilder
// ===================================================================================
class CEntryBuilder : public XXMLCallbackMixin {

	typedef XXMLCallbackMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~CEntryBuilder();
						
						CEntryBuilder(CProcessFiles::Entries& entries);

//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnBeginElement(const std::wstring& name);
	virtual void 		OnEndElement(const std::wstring& name);

	virtual void 		OnText(const std::wstring& element, const std::wstring& text, bool inElementContent);
	virtual void 		OnAttribute(const std::wstring& element, const std::wstring& name, const std::wstring& value, bool implied, bool isID);
	virtual void 		OnComment(const std::wstring& contents);
			
//-----------------------------------
//	Inherited API
//
protected:
	CProcessFiles::Entries&	mEntries;
	
	std::wstring			mOriginal;
	CProcessFiles::SEntry	mEntry;
};


//---------------------------------------------------------------
//
// CEntryBuilder::~CEntryBuilder
//
//---------------------------------------------------------------
CEntryBuilder::~CEntryBuilder()
{
}


//---------------------------------------------------------------
//
// CEntryBuilder::CEntryBuilder
//
//---------------------------------------------------------------
CEntryBuilder::CEntryBuilder(CProcessFiles::Entries& entries) : mEntries(entries)
{
}


//---------------------------------------------------------------
//
// CEntryBuilder::OnBeginElement
//
//---------------------------------------------------------------
void CEntryBuilder::OnBeginElement(const std::wstring& name)
{		
	if (name == L"Entry") {
		mOriginal = L"";
		
		mEntry.translations.clear();
		mEntry.files.clear();
		mEntry.used = false;
		mEntry.synthetic = false;
		
	} else if (name == L"Text") {
		mEntry.translations.push_back(CProcessFiles::STranslation());		
		mEntry.translations.back().locale.SetRegion(L"");	
		
	} else if (name != L"StringTable" && name != L"Original")
		DEBUGSTR("Bad name in CEntryBuilder::OnBeginElement");	// these ASSERTs will fire only if the XML parser is buggy (which is, of course, not the case :-))
}


//---------------------------------------------------------------
//
// CEntryBuilder::OnEndElement
//
//---------------------------------------------------------------
void CEntryBuilder::OnEndElement(const std::wstring& name)
{
	if (name == L"Entry") {			
		mEntries.insert(CProcessFiles::Entries::value_type(mOriginal, mEntry));
		mEntry.comments.clear();
		
	} else if (name != L"Original" && name != L"Text" && name != L"StringTable") 
		DEBUGSTR("Bad name in CEntryBuilder::OnEndElement");
}


//---------------------------------------------------------------
//
// CEntryBuilder::OnAttribute
//
//---------------------------------------------------------------
void CEntryBuilder::OnAttribute(const std::wstring& element, const std::wstring& name, const std::wstring& value, bool implied, bool isID)
{
	UNUSED(implied);
	UNUSED(isID);
	
	if (element == L"Entry") {		
		if (name == L"file") {
			std::wstring text = value;
			std::wstring name = Whisper::Parse(text, L" ");
			while (name.length() > 0) {
				mEntry.files.insert(name);
				name = Whisper::Parse(text, L" ");
			}
		
		} else if (name == L"synthetic") {
			if (value == L"true")
				mEntry.synthetic = true;

		} else
			DEBUGSTR("Entry element has a bad attribute");
			
	} else if (element == L"Text") {
		ASSERT(mEntry.translations.size() > 0);
		
		if (name == L"language")
			mEntry.translations.back().locale.SetLanguage(value);		
		
		else if (name == L"region")
			mEntry.translations.back().locale.SetRegion(value);		
		
		else
			DEBUGSTR("Text element has a bad attribute");
	} 
}


//---------------------------------------------------------------
//
// CEntryBuilder::OnText
//
//---------------------------------------------------------------
void CEntryBuilder::OnText(const std::wstring& element, const std::wstring& text, bool inElementContent)
{
	UNUSED(inElementContent);
	
	if (element == L"Original") {
		ASSERT(!inElementContent);
		mOriginal += text;
	
	} else if (element == L"Text") {
		ASSERT(!inElementContent);
		ASSERT(mEntry.translations.size() > 0);

		mEntry.translations.back().text += text;		
	}
}


//---------------------------------------------------------------
//
// CEntryBuilder::OnComment
//
//---------------------------------------------------------------
void CEntryBuilder::OnComment(const std::wstring& contents)
{
	mEntry.comments.push_back(contents);
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
CProcessFiles::CProcessFiles(const XFileSpec& tableSpec, XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs, ErrorCallback err) : XProcessFiles(fileFilter, folderFilter, specs, err), mTableSpec(tableSpec)
{
	mMesgPrefix = LoadAppString(L"Processing: ");

	mNumInserted = 0;
	mNumRemoved = 0;

	XAutoPtr<XResource> xmlData(XURIResolver::Instance()->Resolve(mTableSpec));
	ASSERT(xmlData.Get() != nil);
	
	XXMLParser parser(xmlData.Get());

	{
	CEntryBuilder callback(mEntries);		
		parser.Parse(callback);
	}
	
	uint32 count = parser.GetNumWarnings();
	if (count > 0) {
		DEBUGSTR("There were ", count, " warnings");
		
		for (uint32 index = 0; index < count; ++index)
			TRACE(parser.GetWarning(index), "\n");
	}
				
	if (mTableSpec.GetName().find(L"XString") == 0)
		mFnName = L"LoadWhisperString(";
	else
		mFnName = L"LoadAppString(";
}

						
//---------------------------------------------------------------
//
// CProcessFiles::OnRun
//
//---------------------------------------------------------------
void CProcessFiles::OnRun()
{
	Inherited::OnRun();
	
	{
	XEnterCriticalSection enter(mMesgMutex);
		mMessage = LoadAppString(L"Writing: #1", mTableSpec.GetName());
	}
				
	if (mNumInserted > 0 && !mAborted)
		this->DoWriteTable();
}


//---------------------------------------------------------------
//
// CProcessFiles::OnProcessFile
//
//---------------------------------------------------------------
void CProcessFiles::OnProcessFile(const XFileSpec& spec)	
{
#if MAC
	PRECONDITION(spec.GetType() == 'TEXT');
#endif

	std::wstring text = ReadFile(spec);
	
	uint32 index = text.find(mFnName);
	while (index != std::wstring::npos) {
		uint32 offset = mFnName.length();
		
		if (text[index + offset] == '"') 
			this->DoProcessQuote(spec.GetName(), text, index + offset);
			
		else if (text[index + offset] == 'L' && text[index + offset + 1] == '"') 
			this->DoProcessQuote(spec.GetName(), text, index + offset + 1);
		
		index = text.find(mFnName, index+offset);
	}
}


//---------------------------------------------------------------
//
// CProcessFiles::DoProcessQuote
//
//---------------------------------------------------------------
void CProcessFiles::DoProcessQuote(const std::wstring& fileName, const std::wstring& text, uint32 offset)
{
	PRECONDITION(text[offset] == '"');
	
	std::wstring original;
	
	uint32 index = offset + 1;
	while (index < text.length() && (text[index] != '"' || text[index-1] == '\\')) {
		original += text[index];
		++index;
	}
	
	if (original.length() > 0) {
		Entries::iterator iter = mEntries.find(original);
		if (iter != mEntries.end()) {
			iter->second.used = true;
			iter->second.files.insert(fileName);
			
		} else {
			SEntry entry;
			entry.used = true;
			entry.synthetic = false;
			entry.files.insert(fileName);
			
			mEntries.insert(Entries::value_type(original, entry));
			++mNumInserted;
		}
	}
}


//---------------------------------------------------------------
//
// CProcessFiles::DoWriteTable
//
//---------------------------------------------------------------
void CProcessFiles::DoWriteTable()
{
	// Build a string containing the new file's contents:
	std::wstring contents;
	contents.reserve(100*mEntries.size());
	
	// add the header
	contents += L"<?xml version=\"1.0\" encoding=\"UTF-8\" ";
	contents += L"standalone=\"no\"?>\n<!DOCTYPE StringTable SYSTEM \"XStringTable.dtd\">\n\n";
	
	// add the StringTable start tag
	contents += L"<StringTable ";
	
	XDate date = XDate::GetCurrentDate();
	contents += L"date = \"" + date.GetDateString() + L"\" ";
	contents += L"time = \"" + date.GetTimeString() + L"\"";
	
	contents += L">\n";
				
	// and for each entry
	Entries::const_iterator iter = mEntries.begin();
	while (iter != mEntries.end() && !mAborted) {
		std::wstring original = iter->first;
		const SEntry& entry = iter->second;
		++iter;
		
		XThread::Yield();
		
		if (entry.used || entry.synthetic) {
							
			// write the comments 
			Comments::const_iterator iter2 = entry.comments.begin();
			while (iter2 != entry.comments.end()) {
				contents += L"\t<!--" + *iter2 + L"-->\n";
				++iter2;
			}
			
			// write the Entry start tag with the synthetic and file attributes
			uint32 count = 0;
			contents += L"\t<Entry ";
			
			if (entry.synthetic)
				contents += L"synthetic = \"true\" ";
			
			contents += L"file = \"";
			Files::const_iterator iter3 = entry.files.begin();
			while (iter3 != entry.files.end() && count < 4) {
				if (count > 0)
					contents += L" ";
				contents += *iter3;
				++iter3; ++count;
			}
			if (count < entry.files.size())
				contents += L" etc";
				
			contents += L"\">\n";
			
			// write the Original element
			contents += L"\t\t<Original>";
			this->DoAppendText(contents, original);
			contents += L"</Original>\n";
	
			// write the Text elements
			Translations::const_iterator iter4 = entry.translations.begin();
			while (iter4 != entry.translations.end()) {
				contents += L"\t\t<Text language = \"" + iter4->locale.GetLanguage() + L"\"";
				if (iter4->locale.GetRegion().length() > 0)
					contents += L" region = \"" + iter4->locale.GetRegion() + L"\"";
				contents += L">";
				
				this->DoAppendText(contents, iter4->text);
				contents += L"</Text>\n";
	
				++iter4;
			}
			
			// and write the Entry stop tag	
			contents += L"\t</Entry>\n\n";
		
		} else
			++mNumRemoved;
	}
	
	// add the StringTable end tag
	contents += L"</StringTable>\n";

	if (!mAborted) {
		// Rename the old file
		XFileSpec newSpec(mTableSpec.GetParent(), mTableSpec.GetPrefix() + L" (Old)." + mTableSpec.GetExtension());
	
		uint32 temp = 2;
		while (XFileSystem::FileExists(newSpec))
			newSpec = XFileSpec(mTableSpec.GetParent(), mTableSpec.GetPrefix() + L" (Old " + UInt32ToStr(temp++) + L")." + mTableSpec.GetExtension());
	
		XFileSystem::RenameFile(mTableSpec, newSpec.GetName());
	
		// Write out the new file
		WriteFile(mTableSpec, contents);
	}
}

		
//---------------------------------------------------------------
//
// CProcessFiles::DoAppendText
//
//---------------------------------------------------------------
void CProcessFiles::DoAppendText(std::wstring& contents, const std::wstring& text)
{
	for (uint32 index = 0; index < text.length(); ++index) {
		wchar_t ch = text[index];
		
		switch (ch) {
			case '&':
				contents += L"&amp;";
				break;
				
			case '<':
				contents += L"&lt;";
				break;
				
			case '>':
				contents += L"&gt;";
				break;

#if 0				
			case '"':
				contents += L"&quot;";
				break;
				
			case '\'':
				contents += L"&apos;";
				break;
#endif
	
			default:
				contents += ch;
		}
	}
}



