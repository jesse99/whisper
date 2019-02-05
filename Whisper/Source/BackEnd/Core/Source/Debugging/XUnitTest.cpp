/*
 *  File:       XUnitTest.cpp
 *  Summary:   	Abstract base class for individual unit tests.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XUnitTest.cpp,v $
 *		Revision 1.2  2000/11/09 12:58:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 1/23/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XUnitTest.h>

#include <XDebug.h>
#include <XUnitTests.h>

#if DEBUG
namespace Whisper {


// ===================================================================================
//	class XUnitTest
// ===================================================================================

//----------------------------------------------------------------
//
// XUnitTest::~XUnitTest
//
//----------------------------------------------------------------
XUnitTest::~XUnitTest()
{
	XUnitTests::Instance()->RemoveTest(this);
}


//----------------------------------------------------------------
//
// XUnitTest::XUnitTest
//
//----------------------------------------------------------------
XUnitTest::XUnitTest(const std::wstring& suite, const std::wstring& name) : mSuite(suite), mName(name)
{
	XUnitTests::Instance()->AddTest(this);
}


//----------------------------------------------------------------
//
// XUnitTest::HandleTest
//
//----------------------------------------------------------------
void XUnitTest::HandleTest()
{
	bool oldIntense = gIntenseDebugging;
	gIntenseDebugging = true;
	
	try {
		this->OnTest();
		
	} catch (const std::exception& e) {
		DEBUGSTR("Caught a '", e.what(), "' exception in the ", mName, " test!");
		
	} catch (...) {
		DEBUGSTR(L"Caught an unknown exception in the ", mName, " test!");
	}

	gIntenseDebugging = oldIntense;
}


}		// namespace Whisper
#endif	// DEBUG