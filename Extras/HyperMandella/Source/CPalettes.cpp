/*
 *  File:       CPalettes.cpp
 *  Summary:   	Interface used to manage saved palettes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CPalettes.cpp,v $
 *		Revision 1.6  2001/04/21 02:24:31  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/03 04:35:35  jesjones
 *		DoSave calls Trim.
 *		
 *		Revision 1.4  2001/04/01 04:53:17  jesjones
 *		Uses callbacks to notify on add palette and delete palette.
 *		
 *		Revision 1.3  2001/03/28 09:39:40  jesjones
 *		Switched to using binary streams instead of XML.
 *		
 *		Revision 1.2  2001/03/08 08:08:47  jesjones
 *		Removed an include.
 *		
 *		Revision 1.1  2000/12/28 02:50:08  jesjones
 *		Moved from the Whisper to the Extras module. 
 *		
 *		Revision 1.2  2000/11/09 08:56:01  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include "IPalettes.h"

#include <XError.h>
#include <XFile.h>
#include <XFileStream.h>
#include <XFloatConversions.h>
#include <XFolderSpec.h>
#include <XResource.h>
#include <XStreaming.h>
#include <XXMLCallbacks.h>
#include <XXMLItems.h>
#include <XXMLParser.h>

#define USE_XML		0			// $$$ arguably better to use XML, but palette files get pretty big and the parser is kinda slow on Windows

// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// operator>> (XColorTable)
//
//---------------------------------------------------------------
static XInStream& operator>>(XInStream& stream, XColorTable& value)
{
	uint32 size;
	stream >> size;
	
	value.SetSize(size);
	
	XRGBColor color;
	for (uint32 i = 0; i < size; ++i) {
		stream >> color;
		value.SetColor(i, color);
	}
		
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (XColorTable)
//
//---------------------------------------------------------------
static XOutStream& operator<<(XOutStream& stream, const XColorTable& value)
{
	uint32 size = value.GetSize();
	stream << size;
	
	for (uint32 i = 0; i < size; ++i) {
		XRGBColor color = value.GetColor(i);
		stream << color;
	}
		
	return stream;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class CPalettes
//!		Implementation used to manage saved palettes.
// ===================================================================================
class CPalettes : public IPalettes {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CPalettes();
	
						CPalettes(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual uint32 		GetCount() const;
	virtual XColorTable Get(uint32 index) const;

	virtual bool 		Has(const std::wstring& name) const;
	virtual XColorTable Get(const std::wstring& name) const;
	virtual std::wstring GetName(uint32 index) const;
	
	virtual XColorTable GetDefault() const;
	virtual std::wstring GetDefaultName() const;
	
	virtual void 		Add(const std::wstring& name, const XColorTable& palette);
	virtual void 		Delete(const std::wstring& name);
	
	virtual void 		MungeColorTable(XColorTable& palette) const;
	
	virtual void 		SetAddCallback(XCallback1<void, std::wstring> callback)		{mAddCallback = callback;}
	virtual void 		SetDeleteCallback(XCallback1<void, std::wstring> callback)	{mDeleteCallback = callback;}

//-----------------------------------
//	Internal API
//
private:
			void 		DoLoad();
			void 		DoSave();

#if USE_XML		
			XColorTable DoLoadPalette(const XXMLElement& elem);
#endif

//-----------------------------------
//	Internal Types
//
private:
	struct SPalette {
		std::wstring	name;
		XColorTable		colors;
		
					SPalette()			 						{}
					SPalette(const std::wstring& n, const XColorTable& c) : name(n), colors(c) {}
		
			bool 	operator==(const SPalette& rhs) const		{return name == rhs.name;}
			bool 	operator<(const SPalette& rhs) const		{return name < rhs.name;}
		
			bool 	operator==(const std::wstring& rhs) const	{return name == rhs;}
			bool 	operator<(const std::wstring& rhs) const	{return name < rhs;}
			
			friend XInStream&  operator>>(XInStream& stream, SPalette& value)			{stream >> value.name >> value.colors; return stream;}
			friend XOutStream& operator<<(XOutStream& stream, const SPalette& value)	{stream << value.name << value.colors; return stream;}
	};

//-----------------------------------
//	Member Data
//
private:
	vector<SPalette>				mPalettes;
	bool							mSaveFailed;
	
	XCallback1<void, std::wstring>	mAddCallback;
	XCallback1<void, std::wstring>	mDeleteCallback;
};

DEFINE_INTERFACE_FACTORY(CPalettes)

//---------------------------------------------------------------
//
// CPalettes::~CPalettes
//
//---------------------------------------------------------------
CPalettes::~CPalettes()
{
}


//---------------------------------------------------------------
//
// CPalettes::CPalettes
//
//---------------------------------------------------------------
CPalettes::CPalettes(XBoss* boss) 
{	
	IPalettes::DoSetBoss(boss);
	
	mSaveFailed = false;
	
	this->DoLoad();
}


//---------------------------------------------------------------
//
// CPalettes::GetDefault
//
//---------------------------------------------------------------
XColorTable CPalettes::GetDefault() const
{	
	XColorTable colors;
	
	vector<SPalette>::const_iterator iter = std::lower_bound(mPalettes.begin(), mPalettes.end(), L"Default");
	if (iter != mPalettes.end() && iter->name == L"Default")
		colors = iter->colors;
		
	else if (mPalettes.size() > 0)
		colors = mPalettes.front().colors;
		
	else {
		colors.SetSize(2);
		colors.SetColor(0, kRGBWhite);
		colors.SetColor(1, kRGBBlack);
	}	
	
	return colors;
}

	
//---------------------------------------------------------------
//
// CPalettes::GetDefaultName
//
//---------------------------------------------------------------
std::wstring CPalettes::GetDefaultName() const
{
	std::wstring name;
	
	vector<SPalette>::const_iterator iter = std::lower_bound(mPalettes.begin(), mPalettes.end(), L"Default");
	if (iter != mPalettes.end() && iter->name == L"Default")
		name = L"Default";
		
	else if (mPalettes.size() > 0)
		name = mPalettes.front().name;
		
	else 
		name = L"White & Black";
	
	return name;
}


//---------------------------------------------------------------
//
// CPalettes::GetCount
//
//---------------------------------------------------------------
uint32 CPalettes::GetCount() const
{
	uint32 count = mPalettes.size();
		
	return count;
}


//---------------------------------------------------------------
//
// CPalettes::Get (uint32)
//
//---------------------------------------------------------------
XColorTable CPalettes::Get(uint32 index) const
{
	PRECONDITION(index < this->GetCount());
	
	const SPalette& palette = mPalettes[index];
	
	return palette.colors;
}

	
//---------------------------------------------------------------
//
// CPalettes::GetName
//
//---------------------------------------------------------------
std::wstring CPalettes::GetName(uint32 index) const
{
	PRECONDITION(index < this->GetCount());
	
	const SPalette& palette = mPalettes[index];
	
	return palette.name;
}


//---------------------------------------------------------------
//
// CPalettes::Has
//
//---------------------------------------------------------------
bool CPalettes::Has(const std::wstring& name) const
{	
	vector<SPalette>::const_iterator iter = std::lower_bound(mPalettes.begin(), mPalettes.end(), L"Default");
	
	bool has = iter != mPalettes.end() && iter->name == name;
	
	return has;
}


//---------------------------------------------------------------
//
// CPalettes::Get (wstring)
//
//---------------------------------------------------------------
XColorTable CPalettes::Get(const std::wstring& name) const
{
	vector<SPalette>::const_iterator iter = std::lower_bound(mPalettes.begin(), mPalettes.end(), name);
	PRECONDITION(iter != mPalettes.end() && iter->name == name);
	
	const SPalette& palette = *iter;

	return palette.colors;
}


//---------------------------------------------------------------
//
// CPalettes::Add
//
//---------------------------------------------------------------
void CPalettes::Add(const std::wstring& name, const XColorTable& palette)
{
	PRECONDITION(name.length() > 0);
	
	vector<SPalette>::iterator iter = std::lower_bound(mPalettes.begin(), mPalettes.end(), name);
	
	if (iter != mPalettes.end() && iter->name == name) 
		iter->colors = palette;
	else 
		mPalettes.insert(iter, SPalette(name, palette));
		
	if (!mSaveFailed)
		this->DoSave();

	if (mAddCallback.IsValid())
		mAddCallback(name);
}


//---------------------------------------------------------------
//
// CPalettes::Delete
//
//---------------------------------------------------------------
void CPalettes::Delete(const std::wstring& name)
{
	PRECONDITION(name.length() > 0);
	
	if (mDeleteCallback.IsValid())
		mDeleteCallback(name);
	
	vector<SPalette>::iterator iter = std::lower_bound(mPalettes.begin(), mPalettes.end(), name);
	PRECONDITION(iter != mPalettes.end());
	
	mPalettes.erase(iter);
		
	if (!mSaveFailed)
		this->DoSave();
}

			
//---------------------------------------------------------------
//
// CPalettes::MungeColorTable
//
//---------------------------------------------------------------
void CPalettes::MungeColorTable(XColorTable& colors) const
{
	uint32 size = colors.GetSize();
	if (size > 4 && colors.GetColor(0) == kRGBWhite && colors.GetColor(size-1) == kRGBBlack) {
		for (uint32 index = 1; index < size-1; index++) {
			XRGBColor color = colors.GetColor(index);
			colors.SetColor(index-1, color);
		}
		colors.SetSize(size-2);
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CPalettes::DoLoad
//
//---------------------------------------------------------------
void CPalettes::DoLoad()
{
	mPalettes.clear();
	
	XFolderSpec dir(XFolderSpec::GetAppFolder(), L"Settings");
	XFileSpec spec(dir, L"Palettes");

#if !USE_XML
	XFile file(spec);
	file.Open(kReadPermission);
	
	XInFileStream stream(file);
	
	uint32 count;
	stream >> count;
	mPalettes.reserve(count);
	
	for (uint32 i = 0; i < count; ++i) {
		SPalette palette;
		stream >> palette;
		mPalettes.push_back(palette);
	}
	
	file.Close();

#else
	// Parse the XML
	XXMLDoc doc;
	{
	XResource data(spec);
	XXMLParser parser(&data);
	XXMLDocumentCallback builder(doc);
		parser.Parse(builder);
	}
	
	XXMLElement* root = doc.GetRootElement();
	ASSERT(root->GetName() == L"Palettes");
	
	// Loop through all the elements,
	XXMLElement::item_iterator iter = root->item_begin();
	while (iter != root->item_end()) {
		XXMLItem* item = *iter;
		++iter;
		
		// and save off each palette.
		if (XXMLElement* elem = dynamic_cast<XXMLElement*>(item)) {	
			ASSERT(elem->GetName() == L"Palette");
			
			const XXMLAttribute& attr = elem->FindAttribute(L"name");
			const std::wstring& name = attr.GetValue();
			
			try {
				SPalette palette(name, this->DoLoadPalette(*elem));
				mPalettes.push_back(palette);
	
			} catch (const std::exception& e) {
				XError::Instance()->ReportError(LoadWhisperString(L"Couldn't load the '%ls' palette.", name), e);	// values not in [0.0, 1.0] for example

			} catch (...) {
				XError::Instance()->ReportError(LoadWhisperString(L"Couldn't load the '%ls' palette.", name));
			}		
		}
	}

	// And sort the palettes
	std::sort(mPalettes.begin(), mPalettes.end());
#endif
}


//---------------------------------------------------------------
//
// CPalettes::DoLoadPalette
//
//---------------------------------------------------------------
#if USE_XML
XColorTable CPalettes::DoLoadPalette(const XXMLElement& elem)
{
	PRECONDITION(elem.GetName() == L"Palette");
	
	uint32 capacity = 256;
	uint32 count = 0;
	
	XColorTable colors;
	colors.SetSize(capacity);

	XXMLElement::const_item_iterator iter = elem.item_begin();
	while (iter != elem.item_end()) {
		XXMLItem* item = *iter;
		++iter;
		
		XRGBColor color;
		if (XXMLElement* entry = dynamic_cast<XXMLElement*>(item)) {
			ASSERT(entry->GetName() == L"Color");
			
			const XXMLAttribute& red = entry->FindAttribute(L"red");
			red.Get(0.0f, color.red, 1.0f);
			
			const XXMLAttribute& green = entry->FindAttribute(L"green");
			green.Get(0.0f, color.green, 1.0f);
			
			const XXMLAttribute& blue = entry->FindAttribute(L"blue");
			blue.Get(0.0f, color.blue, 1.0f);
		}
		
		if (count == capacity) {
			capacity *= 2;
			colors.SetSize(capacity);
		}
		
		colors.SetColor(count++, color);
	}
	
	colors.SetSize(count);	
	
	return colors;
}
#endif

			
//---------------------------------------------------------------
//
// CPalettes::DoSave
//
//---------------------------------------------------------------
void CPalettes::DoSave()
{
	PRECONDITION(!mSaveFailed);		// might be on a locked volume so don't keep pestering the user

	try {
		XFolderSpec dir(XFolderSpec::GetAppFolder(), L"Settings");
		XFileSpec spec(dir, L"Palettes");
		XFile file(spec);
#if !USE_XML
		file.Open('HypM', 'Setg', kWritePermission);
		
		XOutFileStream stream(file);
		
		uint32 count = mPalettes.size();
		stream << count;
		for (uint32 i = 0; i < mPalettes.size(); ++i) {
			const SPalette& palette = mPalettes[i];
			stream << palette;
		}

#else
		std::wstring text = L"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n\n";
		text += L"<Palettes>\n";
		
		for (uint32 i = 0; i < mPalettes.size(); ++i) {
			const SPalette& palette = mPalettes[i];
			text += L"\t<Palette name = \"" + FormatAsXML(palette.name) + L"\">\n";
			
			for (uint32 j = 0; j < palette.colors.GetSize(); ++j) {
				XRGBColor color = palette.colors.GetColor(j);
				text += L"\t\t<Color red = \""   + DoubleToFormStr(color.red, 4, 2) + L"\" " +
								   L"green = \"" + DoubleToFormStr(color.green, 4, 2) + L"\" " +
								   L"blue = \""  + DoubleToFormStr(color.blue, 4, 2) + L"\"/>";
			}
			text += L"\t</Palette>\n";
		}
		text += L"</Palettes>\n\n";
		
		file.Open('HypM', 'TEXT', kWritePermission);
		file.Write(ToUTF8Str(text), text.length());
#endif	
		
		file.Trim();
		file.Close();
	
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't save the palettes."), e);
		mSaveFailed = true;

	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't save the palettes."));
		mSaveFailed = true;
	}	
}


