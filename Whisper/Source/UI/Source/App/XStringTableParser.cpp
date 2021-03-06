/*
 *  File:       XStringTableParser.cpp
 *  Summary:   	Builds a string table used for localization from an XML resource.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XStringTableParser.cpp,v $
 *		Revision 1.2  2000/11/09 12:53:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <5>	 7/14/99	JDJ		Redesigned so that the XML parser doesn't need to
 *									be invoked in release builds.
 *		 <4>	 7/12/99	JDJ		ZStringTableBuilder will use the English translation
 *									if a better translation cannot be found.
 *		 <3>	 7/12/99	JDJ		Fixed a bug in ZStringTableBuilder that could have
 *									caused a valid translation to be skipped if there
 *									were multiple translations with different languages,
 *									but the same region code.
 *		 <2>	 3/19/99	JDJ		ParseXMLStringTable takes a URI instead of a name.
 *		 <1>	 2/15/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XStringTableParser.h>

#include <XXMLDoc.h>
#include <XXMLDocVisitor.h>

namespace Whisper {


// ===================================================================================
//	class ZStringTableBuilder
// ===================================================================================
class ZStringTableBuilder : public XXMLDocVisitor {

	typedef XXMLDocVisitor Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~ZStringTableBuilder();
						
						ZStringTableBuilder(const XLocale& locale, StringTable& table, std::set<XLocale>* locales);

//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		HandleVisit(const XXMLElement& element);

protected:
	virtual bool 		OnVisit(const XXMLElement& element);

	virtual bool 		OnVisit(const XXMLCharData& text);
			
//-----------------------------------
//	Inherited API
//
protected:
	XLocale				mLocale;
	StringTable&		mTable;
	std::set<XLocale>*	mLocales;
		
	std::wstring		mOriginal;

	bool				mProcessOriginal;
	bool				mProcessText;
	int32				mWeight;
};


//---------------------------------------------------------------
//
// ZStringTableBuilder::~ZStringTableBuilder
//
//---------------------------------------------------------------
ZStringTableBuilder::~ZStringTableBuilder()
{
	mLocales = nil;
}


//---------------------------------------------------------------
//
// ZStringTableBuilder::ZStringTableBuilder
//
//---------------------------------------------------------------
ZStringTableBuilder::ZStringTableBuilder(const XLocale& locale, StringTable& table, std::set<XLocale>* locales) : mLocale(locale), mTable(table)
{
	mLocales = locales;
	mTable.clear();
}


//---------------------------------------------------------------
//
// ZStringTableBuilder::HandleVisit
//
//---------------------------------------------------------------
bool ZStringTableBuilder::HandleVisit(const XXMLElement& element)
{
	bool visiting = Inherited::HandleVisit(element);

#if DEBUG
	if (element.GetName() == L"Text") 
		if (mWeight >= 2 && mWeight < 4) 
			TRACEFLOW("XWarning", "Couldn't find a valid translation for '", mOriginal, "'\n");
#endif

	return visiting;
}


//---------------------------------------------------------------
//
// ZStringTableBuilder::OnVisit (XXMLElement)
//
//---------------------------------------------------------------
bool ZStringTableBuilder::OnVisit(const XXMLElement& element)
{
	mProcessOriginal = false;
	mProcessText = false;
	
	const std::wstring& name = element.GetName();
	
	if (name == L"Original") {
		mOriginal.resize(0);
		mProcessOriginal = true;
		mWeight = 0;
	
	} else if (name == L"Text") {			
		std::wstring language, region;

		XXMLElement::const_attr_iterator iter = element.attr_begin();
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;

			if (attr.GetName() == L"language") 
				language = attr.GetValue();			
			else if (attr.GetName() == L"region") 
				region = attr.GetValue();
			else
				DEBUGSTR("Bad attribute");
		}

		if (mLocales != nil)
			mLocales->insert(XLocale(language, region));
		
		int32 weight = (language == mLocale.GetLanguage() ? 4 : 0) +	// if the language matches the requested locale we're set
					   (language == L"English" ? 2 : 0) + 				// if the language doesn't match English will serve as a fallback language (OnEndElement will TRACEFLOW a warning)
					   (region == mLocale.GetRegion() ? 1 : 0);			// the region code will serve as a tie-breaker if there are multiple translations in the same language
						
		ASSERT(mOriginal.length() > 0);
		if (weight >= 2 && weight > mWeight) {
			mWeight = weight;
			mTable[mOriginal] = L"";									// there may have been an earlier (worse) translation
			mProcessText = true;	
		}

	} else if (name != L"StringTable" && name != L"Entry")
		DEBUGSTR("Bad element");
		
	return true;
}


//---------------------------------------------------------------
//
// ZStringTableBuilder::OnVisit (XXMLCharData)
//
//---------------------------------------------------------------
bool ZStringTableBuilder::OnVisit(const XXMLCharData& text)
{
	if (mProcessOriginal) 
		mOriginal += text.GetText();
		
	else if (mProcessText) 	
		mTable[mOriginal] += text.GetText();
		
	return true;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// ReadXMLStringTable
//
//---------------------------------------------------------------
void ReadXMLStringTable(const XURI& uri, const XLocale& locale, StringTable& table, std::set<XLocale>* locales)
{	
	XXMLDoc doc;
	doc.Load(uri);
	
	{
	ZStringTableBuilder builder(locale, table, locales);
		doc.GetRootElement()->HandleVisit(builder);
	}
	
#if DEBUG
	if (locales != nil) {
		XLocale defaultLocale;
		
		std::wstring region = defaultLocale.GetRegion();
		std::reverse(region.begin(), region.end());
		defaultLocale.SetRegion(region);
		
		locales->insert(defaultLocale);
	}
#endif
}


}	// namespace Whisper
