/*
 *  File:       CColorFormulas.cpp
 *  Summary:   	Interface used to manage saved color formulas (eg "sin(.5*x)").
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CColorFormulas.cpp,v $
 *		Revision 1.4  2001/04/27 04:17:46  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.3  2001/04/21 02:23:29  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2001/04/03 04:35:12  jesjones
 *		Added Has method and notifications. DoSave calls Trim.
 *		
 *		Revision 1.1  2000/12/28 02:48:29  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:53:12  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include "IColorFormulas.h"

#include <XError.h>
#include <XFile.h>
#include <XFolderSpec.h>
#include <XResource.h>
#include <XStringUtils.h>
#include <XXMLCallbacks.h>
#include <XXMLItems.h>
#include <XXMLParser.h>


// ===================================================================================
//	class CColorFormulas
// ===================================================================================
class CColorFormulas : public IColorFormulas {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CColorFormulas();
	
						CColorFormulas(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual uint32 		GetCount() const;
	virtual std::wstring Get(uint32 index) const;
	
	virtual std::wstring GetDefault() const;
	virtual bool 		Has(const std::wstring& formula) const;
	
	virtual void 		Add(const std::wstring& formula);
	virtual void 		Delete(const std::wstring& formula);
	
	virtual void 		SetAddCallback(XCallback1<void, std::wstring> callback)		{mAddCallback = callback;}
	virtual void 		SetDeleteCallback(XCallback1<void, std::wstring> callback)	{mDeleteCallback = callback;}
	
//-----------------------------------
//	Internal API
//
private:
			void 		DoSaveFormulas();

//-----------------------------------
//	Member Data
//
private:
	vector<std::wstring>			mFormulas;
	bool							mSaveFailed;
	
	XCallback1<void, std::wstring>	mAddCallback;
	XCallback1<void, std::wstring>	mDeleteCallback;
};

DEFINE_INTERFACE_FACTORY(CColorFormulas)

//---------------------------------------------------------------
//
// CColorFormulas::~CColorFormulas
//
//---------------------------------------------------------------
CColorFormulas::~CColorFormulas()
{
}


//---------------------------------------------------------------
//
// CColorFormulas::CColorFormulas
//
//---------------------------------------------------------------
CColorFormulas::CColorFormulas(XBoss* boss) 
{	
	IColorFormulas::DoSetBoss(boss);
	
	mSaveFailed = false;
	
	// Get a file spec that points to the Color Formulas file
	XFolderSpec dir(XFolderSpec::GetAppFolder(), L"Settings");
	XFileSpec file(dir, L"Color Formulas");

	// Parse the XML
	XXMLDoc doc;
	{
	XResource data(file);
	XXMLParser parser(&data);
	XXMLDocumentCallback builder(doc);
		parser.Parse(builder);
	}
	
	XXMLElement* root = doc.GetRootElement();
	ASSERT(root->GetName() == L"Formulas");
	
	// Loop through all the elements,
	XXMLElement::item_iterator iter = root->item_begin();
	while (iter != root->item_end()) {
		XXMLItem* item = *iter;
		++iter;
		
		// and save off each formula.
		if (XXMLElement* elem = dynamic_cast<XXMLElement*>(item)) {
			ASSERT(elem->GetName() == L"Formula");
			
			const XXMLAttribute& attr = elem->FindAttribute(L"value");
			const std::wstring& formula = attr.GetValue();
			
			mFormulas.push_back(formula);
		}
	}
	
	// And sort the formulas
	std::sort(mFormulas.begin(), mFormulas.end());
}


//---------------------------------------------------------------
//
// CColorFormulas::GetDefault
//
//---------------------------------------------------------------
std::wstring CColorFormulas::GetDefault() const
{	
	return L"sin(0.5*x)";
}

	
//---------------------------------------------------------------
//
// CColorFormulas::Has
//
//---------------------------------------------------------------
bool CColorFormulas::Has(const std::wstring& formula) const
{
	vector<wstring>::const_iterator iter = std::lower_bound(mFormulas.begin(), mFormulas.end(), formula);
	
	bool has = iter != mFormulas.end() && *iter == formula;
	
	return has;
}


//---------------------------------------------------------------
//
// CColorFormulas::GetCount
//
//---------------------------------------------------------------
uint32 CColorFormulas::GetCount() const
{
	uint32 count = mFormulas.size();
		
	return count;
}


//---------------------------------------------------------------
//
// CColorFormulas::Get
//
//---------------------------------------------------------------
std::wstring CColorFormulas::Get(uint32 index) const
{
	PRECONDITION(index < this->GetCount());
	
	const std::wstring& formula = mFormulas[index];
	
	return formula;
}

	
//---------------------------------------------------------------
//
// CColorFormulas::Add
//
//---------------------------------------------------------------
void CColorFormulas::Add(const std::wstring& formula)
{
	PRECONDITION(formula.length() > 0);
	
	vector<std::wstring>::iterator iter = std::lower_bound(mFormulas.begin(), mFormulas.end(), formula);
	if (iter == mFormulas.end() || *iter != formula) {
		mFormulas.insert(iter, formula);
		
		if (!mSaveFailed)
			this->DoSaveFormulas();
	
		if (mAddCallback.IsValid())
			mAddCallback(formula);
	}
}


//---------------------------------------------------------------
//
// CColorFormulas::Delete
//
//---------------------------------------------------------------
void CColorFormulas::Delete(const std::wstring& formula)
{
	PRECONDITION(formula.length() > 0);
	
	if (mDeleteCallback.IsValid())
		mDeleteCallback(formula);
		
	vector<std::wstring>::iterator iter = std::lower_bound(mFormulas.begin(), mFormulas.end(), formula);
	PRECONDITION(iter != mFormulas.end());
	
	mFormulas.erase(iter);
		
	if (!mSaveFailed)
		this->DoSaveFormulas();
}

			
//---------------------------------------------------------------
//
// CColorFormulas::DoSaveFormulas
//
//---------------------------------------------------------------
void CColorFormulas::DoSaveFormulas()
{
	PRECONDITION(!mSaveFailed);		// might be on a locked volume so don't keep pestering the user
	
	try {
		std::wstring text = L"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n\n";
		text += L"<Formulas>\n";
		
		for (uint32 i = 0; i < mFormulas.size(); ++i) {
			const std::wstring& formula = mFormulas[i];
			text += L"\t<Formula value = \"" + formula + L"\"/>\n";
		}
		text += L"</Formulas>\n";
		
		XFolderSpec dir(XFolderSpec::GetAppFolder(), L"Settings");
		XFileSpec spec(dir, L"Color Formulas");
		
		UTF8String str = ToUTF8Str(text);
		
		XFile file(spec);
		file.Open('HypM', 'TEXT', kWritePermission);
		file.Write(str.c_str(), str.length());
		file.Trim();
		file.Close();
	
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't save the color formulas."), e);
		mSaveFailed = true;

	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't save the color formulas."));
		mSaveFailed = true;
	}		
}


