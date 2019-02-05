/*
 *  File:       XUnitTest.h
 *  Summary:   	Abstract base class for individual unit tests.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XUnitTest.h,v $
 *		Revision 1.4  2000/12/10 08:57:12  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.3  2000/11/20 05:33:49  jesjones
 *		Includes <string>.
 *		
 *		Revision 1.2  2000/11/09 12:58:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 1/23/00	JDJ		Created
 */

#pragma once

#include <string>

#if DEBUG
namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XUnitTest
//!		Abstract base class for individual unit tests.
// ===================================================================================
class CORE_EXPORT XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XUnitTest();
	
						XUnitTest(const std::wstring& suite, const std::wstring& name);
						
private:
						XUnitTest(const XUnitTest& rhs);
	
			XUnitTest& operator=(const XUnitTest& rhs);

//-----------------------------------
//	API
//
public:
			const std::wstring& GetSuite() const					{return mSuite;}
			const std::wstring& GetName() const					{return mName;}
			
	virtual void 		HandleTest();
	
protected:
	virtual void 		OnTest() = 0;

//-----------------------------------
//	Member Data
//
private:
	std::wstring		mSuite;
	std::wstring		mName;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}		// namespace Whisper
#endif	// DEBUG