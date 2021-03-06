/*
 *  File:		CTestConformance.cpp
 *  Summary:	Run the oasis XML conformance tests.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CTestConformance.cpp,v $
 *		Revision 1.3  2001/10/03 02:07:25  jesjones
 *		No longer uses XString.
 *		
 *		Revision 1.2  2000/11/09 08:57:49  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: CTestConformance.cpp,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2001/10/03 02:07:25  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the No longer uses XString.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 3/21/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include "CTestConformance.h"

#include <XAutoPtr.h>
#include <XFileSpec.h>
#include <XFolderSpec.h>
#include <XLocker.h>
#include <XResource.h>
#include <XTextTranscoders.h>
#include <XURIResolver.h>
#include <XXMLCallbacks.h>
#include <XXMLDoc.h>
#include <XXMLItems.h>
#include <XXMLParser.h>

#include "CCanonical.h"


// ===================================================================================
//	class OutputException
// ===================================================================================
class OutputException : public std::domain_error { 
public: 
						OutputException(const std::wstring& o1, const std::wstring& o2) : std::domain_error("OutputException"), mine(o1), theirs(o2) {}
public:
	std::wstring mine;
	std::wstring theirs;
};

// ===================================================================================
//	class CTestConformance
// ===================================================================================

//----------------------------------------------------------------
//
// CTestConformance::~CTestConformance
//
//----------------------------------------------------------------
CTestConformance::~CTestConformance()
{
}


//----------------------------------------------------------------
//
// CTestConformance::CTestConformance
//
// <!ELEMENT TESTSUITE (TESTCASES+)>
//
//----------------------------------------------------------------
CTestConformance::CTestConformance(const XFileSpec& spec) : mLog("Details.txt"), mRootURI(spec.GetParent())
{
	XXMLDoc doc;
	
	mNumTests = 0;
	mNumFailed = 0;

	// Parse the conformance suite
	{
	XResource data(spec);
	XXMLParser parser(&data);
		XXMLDocumentCallback builder(doc);
		parser.Parse(builder);
	}
	
	// Make sure the root element has the right name
	const XXMLElement* suite = doc.GetRootElement();
	if (suite->GetName() != L"TESTSUITE")
		throw std::domain_error("Root element isn't names 'TESTSUITE'.");
	
	// Print the conformance version info
	const XXMLAttribute& profile = suite->FindAttribute(L"PROFILE");
	if (profile.IsImplied())
		TRACE(spec.GetName(), "\n\n");
	else
		TRACE(profile.GetValue(), L"\n\n");
		
	// Run through each test case
	XXMLElement::const_item_iterator iter = suite->item_begin();

	mBaseURI = mRootURI;
	if (iter != suite->item_end()) {		// jclark-xmltest
		const XXMLItem* item = *iter;
		++iter;
		
		if (const XXMLElement* testCase = dynamic_cast<const XXMLElement*>(item))
			this->DoTestCase(testCase);
		else
			throw std::domain_error("Root element's children should be all elements.");
	}

	mBaseURI = mRootURI;
	if (iter != suite->item_end()) {		// xerox-japanese
		const XXMLItem* item = *iter;
		++iter;
		
//		if (const XXMLElement* testCase = dynamic_cast<const XXMLElement*>(item))
//			this->DoTestCase(testCase);
//		else
//			throw std::domain_error("Root element's children should be all elements.");
	}

	mBaseURI = XURI(mRootURI, L"Sun/");
	if (iter != suite->item_end()) {		// sun
		const XXMLItem* item = *iter;
		++iter;
		
		if (const XXMLElement* testCase = dynamic_cast<const XXMLElement*>(item))
			this->DoTestCase(testCase);
		else
			throw std::domain_error("Root element's children should be all elements.");
	}

	mBaseURI = mRootURI;
	if (iter != suite->item_end()) {		// nist-oasis
		const XXMLItem* item = *iter;
		++iter;
		
		if (const XXMLElement* testCase = dynamic_cast<const XXMLElement*>(item))
			this->DoTestCase(testCase);
		else
			throw std::domain_error("Root element's children should be all elements.");
	}

	while (iter != suite->item_end()) {		// ???
		const XXMLItem* item = *iter;
		++iter;
		
		if (const XXMLElement* testCase = dynamic_cast<const XXMLElement*>(item))
			this->DoTestCase(testCase);
		else
			throw std::domain_error("Root element's children should be all elements.");
	}
	
	// Trace out some summary info
	if (mNumFailed == 0)
		TRACE("Ran ", mNumTests, " tests with no failures.\n");
	else if (mNumFailed == 1)
		TRACE("Ran ", mNumTests, " tests with 1 failure.\n");
	else
		TRACE("Ran ", mNumTests, " tests with ", mNumFailed, " failures.\n");
}

#if __MWERKS__
#pragma mark �
#endif

//----------------------------------------------------------------
//
// CTestConformance::DoTestCase
//
// <!ELEMENT TESTCASES (TEST|TESTCASES)*>
//
//----------------------------------------------------------------
void CTestConformance::DoTestCase(const XXMLElement* testCase)
{
	ASSERT(testCase != nil);
	
	// Make sure the element has the right name
	if (testCase->GetName() != L"TESTCASES")
		throw std::domain_error("Root element's children should be all elements named TESTCASES.");
	
	// Print the testcase info
	const XXMLAttribute& profile = testCase->FindAttribute(L"PROFILE");
	if (profile.IsImplied())
		TRACE("Running unnamed testcase\n");
	else
		TRACE("Running %ls\n", profile.GetValue());
		
	// Run through each test
	XXMLElement::const_item_iterator iter = testCase->item_begin();
	while (iter != testCase->item_end()) {
		const XXMLItem* item = *iter;
		++iter;
		
		if (const XXMLElement* test = dynamic_cast<const XXMLElement*>(item))
			if (test->GetName() == L"TESTCASES")
				this->DoTestCase(test);
			else
				this->DoTest(test);
		else
			throw std::domain_error("TESTCASES children should be all elements.");
	}
}



//----------------------------------------------------------------
//
// CTestConformance::DoTest
//
// <!ELEMENT TEST (#PCDATA | EM | B)*>
//
//----------------------------------------------------------------
void CTestConformance::DoTest(const XXMLElement* test)
{
	ASSERT(test != nil);
	
	// Make sure the element has the right name
	if (test->GetName() != L"TEST")
		throw std::domain_error("TESTCASES children should named TEST or TESTCASES.");
	
	// Print the ID
	const XXMLAttribute& id = test->FindAttribute(L"ID");
	TRACE("   ", id.GetValue());
	
//	ASSERT(id.GetValue() != L"o-p64fail1");
		
	// Do the test
	const XXMLAttribute& type = test->FindAttribute(L"TYPE");
	const XXMLAttribute& uri = test->FindAttribute(L"URI");
		
	try {
		++mNumTests;
	
		XAutoPtr<XXMLDoc> result;
		std::vector<std::wstring> warnings;
		bool parsed = true;
		
		// valid files
		if (type.GetValue() == L"valid" || uri.GetValue().find(L"pass") != std::wstring::npos) {	// $$$ oasis tests have some valid tests marked invalid...
			result.Reset(this->DoParseTest(uri.GetValue(), warnings));
			bool version1 = uri.GetValue().find(L"xmltest") != std::wstring::npos;
			this->DoCheckOutput(*result, test, version1);
			
		// invalid files
		} else if (type.GetValue() == L"invalid") {
			try {
				result.Reset(this->DoParseTest(uri.GetValue(), warnings));

			} catch (const std::exception& e) {
				parsed = false;
				this->DoLogFailure(test, &e);
			
			} catch (...) {
				parsed = false;
				this->DoLogFailure(test, nil);
			}

			if (parsed)
				throw std::domain_error("Test is invalid, but it was successfully parsed.");
		
		// not well formed files
		} else if (type.GetValue() == L"not-wf") {
			try {
				result.Reset(this->DoParseTest(uri.GetValue(), warnings));

			} catch (const std::exception& e) {
				parsed = false;
				this->DoLogFailure(test, &e);
			
			} catch (...) {
				parsed = false;
				this->DoLogFailure(test, nil);
			}

			if (parsed)
				throw std::domain_error("Test is not well formed, but it was successfully parsed.");
		
		// files with non-fatal errors
		} else if (type.GetValue() == L"error") {
			result.Reset(this->DoParseTest(uri.GetValue(), warnings));
			bool version1 = uri.GetValue().find(L"xmltest") != std::wstring::npos;
			this->DoCheckOutput(*result, test, version1);
		
		} else
			throw std::domain_error("TEST's TYPE attribute has a bogus name.");
		
		if (warnings.size() > 0)
			this->DoLogWarnings(test, warnings);

	} catch (const OutputException& e) {				// $$ MSL 5.1's vswprintf will crash if you blow the buffer so we'll use a custom exception class to break the string up
		++mNumFailed;
		TRACE(" FAILED: should be\n");	
		TRACE("    ", e.theirs, "\n\n");	
		TRACE(" but was\n");	
		TRACE("    ", e.mine, "\n\n");	
			
		TRACE("   uri = ", uri.GetValue(), "\n");	
	
		const XXMLAttribute& section = test->FindAttribute(L"SECTIONS");
		TRACE("   section = ", section.GetValue(), "\n");	

		TRACE("   ", this->DoGetText(test), "\n");	

	} catch (const std::exception& e) {
		++mNumFailed;
		TRACE(" FAILED (", e.what(), ")\n");	
			
		TRACE("   uri = ", uri.GetValue(), "\n");	
	
		const XXMLAttribute& section = test->FindAttribute(L"SECTIONS");
		TRACE("   section = ", section.GetValue(), "\n");	

		TRACE("   ", this->DoGetText(test), "\n");	

	} catch (...) {
		++mNumFailed;
		TRACE(" FAILED");
			
		TRACE("   uri = ", uri.GetValue(), "\n");	
	
		const XXMLAttribute& section = test->FindAttribute(L"SECTIONS");
		TRACE("   section = ", section.GetValue(), "\n");	

		TRACE("   ", this->DoGetText(test), "\n");	
	}

	TRACE("\n");
}


//----------------------------------------------------------------
//
// CTestConformance::DoParseTest
//
//----------------------------------------------------------------
XXMLDoc* CTestConformance::DoParseTest(const std::wstring& uri, std::vector<std::wstring>& warnings)
{
	XAutoPtr<XXMLDoc> doc(new XXMLDoc);
	
//	ASSERT(uri != L"xmltest/valid/sa/094.xml");
	
	{
	XAutoPtr<XResource> data(XURIResolver::Instance()->Resolve(XURI(mBaseURI, uri)));
	ThrowIf(data.Get() == nil, L"Couldn't find the file!");

	XXMLParser parser(data.Get());
		XXMLDocumentCallback builder(*doc.Get());
		builder.StripInsignifcantWhiteSpace(false);
		parser.Parse(builder);

		uint32 count = parser.GetNumWarnings();
		for (uint32 index = 0; index < count; ++index)
			warnings.push_back(parser.GetWarning(index));		
	}
	
	return doc.Release();
}


//----------------------------------------------------------------
//
// CTestConformance::DoGetText
//
// <!ELEMENT TEST (#PCDATA | EM | B)*>
//
//----------------------------------------------------------------
std::wstring CTestConformance::DoGetText(const XXMLElement* test)
{
	ASSERT(test != nil);
	
	std::wstring text;
	
	XXMLElement::const_item_iterator iter = test->item_begin();
	while (iter != test->item_end()) {
		const XXMLItem* item = *iter;
		++iter;
		
		if (const XXMLCharData* chars = dynamic_cast<const XXMLCharData*>(item))
			text += chars->GetText();
		else if (const XXMLElement* elem = dynamic_cast<const XXMLElement*>(item))	// emphasis elements
			text += this->DoGetText(elem);
	}

	return text;
}


//----------------------------------------------------------------
//
// CTestConformance::DoCheckOutput
//
//----------------------------------------------------------------
void CTestConformance::DoCheckOutput(const XXMLDoc& doc, const XXMLElement* test, bool version1)
{
	ASSERT(test != nil);
	
	const XXMLAttribute& output = test->FindAttribute(L"OUTPUT");
	if (!output.IsImplied()) {
		XAutoPtr<XResource> data(XURIResolver::Instance()->Resolve(XURI(mBaseURI, output.GetValue())));
		
		if (data) {
			std::wstring outText;	
			{
			XLocker lock(*data);
				outText = FromUTF8Str((const char*) data->GetPtr(), data->GetSize());
			}
		
			CCanonical canonical(doc, version1);
			if (canonical.GetText() != outText)
				throw OutputException(canonical.GetText(), outText);
		
		} else {
			std::wstring mesg = L"Couldn't find the '" + output.GetValue() + L"' output file.";
			throw std::domain_error(ToUTF8Str(mesg));
		}
	}
}



//----------------------------------------------------------------
//
// CTestConformance::DoLogFailure
//
//----------------------------------------------------------------
void CTestConformance::DoLogFailure(const XXMLElement* test, const std::exception* e)
{
	ASSERT(test != nil);

	const XXMLAttribute& uri = test->FindAttribute(L"URI");
	mLog.Print(L"uri = %ls\n", uri.GetValue().c_str());	

	const XXMLAttribute& section = test->FindAttribute(L"SECTIONS");
	mLog.Print(L"section = %ls\n", section.GetValue().c_str());	

	mLog.Print(L"text = %ls\n", this->DoGetText(test).c_str());	

	if (e != nil)
		mLog.Print(L"error = %s\n\n", e->what());	
	else
		mLog.Print(L"error = unknown error\n\n");	
}


//----------------------------------------------------------------
//
// CTestConformance::DoLogWarnings
//
//----------------------------------------------------------------
void CTestConformance::DoLogWarnings(const XXMLElement* test, const std::vector<std::wstring>& warnings)
{
	ASSERT(test != nil);
	ASSERT(warnings.size() > 0);

	const XXMLAttribute& uri = test->FindAttribute(L"URI");
	mLog.Print(L"uri = %ls\n", uri.GetValue().c_str());	

	const XXMLAttribute& section = test->FindAttribute(L"SECTIONS");
	mLog.Print(L"section = %ls\n", section.GetValue().c_str());	

	mLog.Print(L"text = %ls\n", this->DoGetText(test).c_str());	

	uint32 count = warnings.size();
	for (uint32 index = 0; index < count; ++index)
		mLog.Print(L"warning = %ls\n", warnings[index].c_str());	

	mLog.Print(L"\n");	
}


