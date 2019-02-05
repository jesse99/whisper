/*
 *  File:       XUnitTests.h
 *  Summary:   	Singleton that manages XUnitTests.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XUnitTests.h,v $
 *		Revision 1.5  2001/03/16 10:37:12  jesjones
 *		Exported SSuite.
 *		
 *		Revision 1.4  2000/12/10 08:57:16  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.3  2000/11/20 05:33:48  jesjones
 *		Includes <string>.
 *		
 *		Revision 1.2  2000/11/09 12:58:35  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <string>
#include <vector>

#include <XCriticalSection.h>
#include <XThreadSafeIter.h>

#if DEBUG
namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XUnitTest;


// ===================================================================================
//	class XUnitTests
//!		Singleton that manages XUnitTest's.
// ===================================================================================
class CORE_EXPORT XUnitTests {

//-----------------------------------
//	Types
//
public:		
	typedef std::vector<XUnitTest*>	Suite;

	struct CORE_EXPORT SSuite {
		std::wstring	name;
		Suite			suite;
		
					~SSuite();
					SSuite()	{}
					SSuite(const std::wstring& n, const Suite& s) : name(n), suite(s)	{}
					SSuite(const SSuite& rhs);
			
			bool 	operator==(const SSuite& rhs) const	{return name == rhs.name;}
			bool 	operator!=(const SSuite& rhs) const	{return name != rhs.name;}

			bool 	operator<(const std::wstring& rhs) const	{return name < rhs;}
	};

	typedef std::vector<SSuite>	Tests;		// unit tests are essentially static so we can reduce bloat by using a sorted vector instead of a map
	typedef XThreadSafeIter<Tests::iterator, XCriticalSection>	iterator;

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XUnitTests();
	
						XUnitTests();

	static	XUnitTests*	Instance();
						
private:
						XUnitTests(const XUnitTests& rhs);
	
			XUnitTests& operator=(const XUnitTests& rhs);

//-----------------------------------
//	API
//
public:
	// ----- Running -----
			void		RunAllTests();
			
			void		RunSuiteTests(const std::wstring& suite);
			
			void		RunTest(const std::wstring& suite, const std::wstring& name);			
			
	// ----- Info -----
			uint32 		GetEditKey() const;
						/**< Returns a number that is incremented each time a test is added or removed. */
						
			iterator 	begin()							{return iterator(mTests.begin(), &mMutex);}
			iterator 	end()							{return iterator(mTests.end(), &mMutex);}
					
//-----------------------------------
//	Internal API
//
public:
			void		AddTest(XUnitTest* test);
			void		RemoveTest(XUnitTest* test);
						// Called automatically by XUnitTest.

//-----------------------------------
//	Member Data
//
protected:
	Tests						mTests;
	uint32						mKey;
	
	mutable XCriticalSection	mMutex;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}		// namespace Whisper
#endif	// DEBUG