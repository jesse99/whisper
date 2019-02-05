/*
 *  File:       XUnitTests.cpp
 *  Summary:   	Singleton that manages XUnitTests.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XUnitTests.cpp,v $
 *		Revision 1.3  2001/04/27 04:37:52  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:58:30  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 8/29/00	JDJ		Replaced GetNumTests with GetEditKey.
 *		 <4>	 1/23/00	JDJ		Added XUnitTest and support for suites.
 *		 <3>	 5/28/99	JDJ		Made thread safe.
 *		 <2>	 5/27/99	JDJ		Made Instance method thread safe.
 *		 <1>	11/20/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XUnitTests.h>

#include <algorithm>

#include <XDebug.h>
#include <XUnitTest.h>

#if DEBUG
namespace Whisper {

XUnitTests::SSuite::~SSuite()	{}
XUnitTests::SSuite::SSuite(const SSuite& rhs) : name(rhs.name), suite(rhs.suite)	{}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XUnitTests
// ===================================================================================

//----------------------------------------------------------------
//
// XUnitTests::~XUnitTests
//
//----------------------------------------------------------------
XUnitTests::~XUnitTests()
{
}


//----------------------------------------------------------------
//
// XUnitTests::XUnitTests
//
//----------------------------------------------------------------
XUnitTests::XUnitTests()
{
	mKey = 0;
	mTests.reserve(32);
}


//----------------------------------------------------------------
//
// XUnitTests::Instance									[static]
//
// There are some order of initialization problems with using
// critical sectons here, but it shouldn't matter because we
// should be constructed before any other threads start up.
//
//----------------------------------------------------------------
XUnitTests* XUnitTests::Instance()
{
	static XUnitTests instance;
	
	return &instance;
}


//----------------------------------------------------------------
//
// XUnitTests::AddTest
//
//----------------------------------------------------------------
void XUnitTests::AddTest(XUnitTest* test)
{
	PRECONDITION(test != nil);

	XEnterCriticalSection enter(mMutex);
	
	std::wstring suite = test->GetSuite();
	Tests::iterator iter = std::lower_bound(mTests.begin(), mTests.end(), suite);
	if (iter != mTests.end() && iter->name == suite) {
		Suite& tests = iter->suite;
		
		PRECONDITION(std::find(tests.begin(), tests.end(), test) == tests.end());
		tests.push_back(test);
	
	} else {
		Suite tests;
		tests.push_back(test);
		
		mTests.insert(iter, SSuite(suite, tests));
	}
	
	++mKey;
}


//----------------------------------------------------------------
//
// XUnitTests::RemoveTest
//
//----------------------------------------------------------------
void XUnitTests::RemoveTest(XUnitTest* test)
{
	PRECONDITION(test != nil);

	XEnterCriticalSection enter(mMutex);
	
	std::wstring suite = test->GetSuite();
	Tests::iterator iter = std::lower_bound(mTests.begin(), mTests.end(), suite);
	PRECONDITION(iter != mTests.end());
	PRECONDITION(iter->name == suite);
	
	Suite& tests = iter->suite;
	Suite::iterator iter2 = std::find(tests.begin(), tests.end(), test);
	PRECONDITION(iter2 != tests.end());
	
	tests.erase(iter2);
	++mKey;
}


//----------------------------------------------------------------
//
// XUnitTests::GetEditKey
//
//----------------------------------------------------------------
uint32 XUnitTests::GetEditKey() const
{
	uint32 key = 0;
	
	{
	XEnterCriticalSection enter(mMutex);
		key = mKey;
	}
	
	return key;
}


//----------------------------------------------------------------
//
// XUnitTests::RunAllTests
//
//----------------------------------------------------------------
void XUnitTests::RunAllTests()
{
	XEnterCriticalSection enter(mMutex);
	
	Tests::iterator iter = mTests.begin();
	while (iter != mTests.end()) {
		SSuite& value = *iter;
		++iter;
		
		this->RunSuiteTests(value.name);
	}
	
	TRACE("All Done\n\n");
}


//----------------------------------------------------------------
//
// XUnitTests::RunSuiteTests
//
//----------------------------------------------------------------
void XUnitTests::RunSuiteTests(const std::wstring& suite)
{
	XEnterCriticalSection enter(mMutex);
	
	Tests::iterator iter = std::lower_bound(mTests.begin(), mTests.end(), suite);
	PRECONDITION(iter != mTests.end());
	PRECONDITION(iter->name == suite);
		
	Suite& tests = iter->suite;
	
	Suite::iterator iter2 = tests.begin();
	while (iter2 != tests.end()) {
		XUnitTest* test = *iter2;
		++iter2;
		
		test->HandleTest();
	}
}


//----------------------------------------------------------------
//
// XUnitTests::RunTest
//
//----------------------------------------------------------------
void XUnitTests::RunTest(const std::wstring& suite, const std::wstring& name)
{
	XEnterCriticalSection enter(mMutex);
	
	Tests::iterator iter = std::lower_bound(mTests.begin(), mTests.end(), suite);
	PRECONDITION(iter != mTests.end());
	PRECONDITION(iter->name == suite);
		
	Suite& tests = iter->suite;
	
	Suite::iterator iter2 = tests.begin();
	while (iter2 != tests.end()) {
		XUnitTest* test = *iter2;
		
		if (test->GetName() == name) {
			test->HandleTest();
			break;
		}

		++iter2;
	}

	POSTCONDITION(iter2 != tests.end());
}
			

}		// namespace Whisper
#endif	// DEBUG