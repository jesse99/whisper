/*
 *  File:       XStringTableLoader.cpp
 *  Summary:   	IStringTableLoader implementation that uses an XML file.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XStringTableLoader.cpp,v $
 *		Revision 1.3  2001/02/17 10:23:59  jesjones
 *		IStringTableLoader::Load takes an XResource instead of an XURI.
 *		
 *		Revision 1.2  2000/11/09 12:53:32  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IStringTableLoader.h>

#include <XInterfaceMacros.h>
#include <XLoadXML.h>
#include <XObjectModel.h>
#include <XXMLDoc.h>
#include <XXMLDocVisitor.h>
#include <XXMLItems.h>

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
						
						ZStringTableBuilder(const XLocale& locale, StringTable& table);

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
	XLocale			mLocale;
	StringTable&	mTable;
		
	std::wstring	mOriginal;

	bool			mProcessOriginal;
	bool			mProcessText;
	int32			mWeight;
};


//---------------------------------------------------------------
//
// ZStringTableBuilder::~ZStringTableBuilder
//
//---------------------------------------------------------------
ZStringTableBuilder::~ZStringTableBuilder()
{
}


//---------------------------------------------------------------
//
// ZStringTableBuilder::ZStringTableBuilder
//
//---------------------------------------------------------------
ZStringTableBuilder::ZStringTableBuilder(const XLocale& locale, StringTable& table) : mLocale(locale), mTable(table)
{
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
			TRACEFLOW("XWarning", "Couldn't find a valid translation for ", mOriginal, "\n");
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
//	class XStringTableLoader
// ===================================================================================
class XStringTableLoader : public IStringTableLoader {	

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XStringTableLoader();
	
						XStringTableLoader(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Load(const XResource& data, const XLocale& locale, StringTable& table);
};

DEFINE_INTERFACE_FACTORY(XStringTableLoader)

//---------------------------------------------------------------
//
// XStringTableLoader::~XStringTableLoader
//
//---------------------------------------------------------------
XStringTableLoader::~XStringTableLoader()
{
}


//---------------------------------------------------------------
//
// XStringTableLoader::XStringTableLoader
//
//---------------------------------------------------------------
XStringTableLoader::XStringTableLoader(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XStringTableLoader::Load
//
//---------------------------------------------------------------
void XStringTableLoader::Load(const XResource& data, const XLocale& locale, StringTable& table)
{
	XXMLDoc doc;
	Whisper::Load(doc, data);
	
	{
	ZStringTableBuilder builder(locale, table);
		doc.GetRootElement()->HandleVisit(builder);
	}
}


}	// namespace Whisper

