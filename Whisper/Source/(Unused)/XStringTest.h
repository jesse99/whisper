/*
 *  File:       XStringTest.h
 *  Summary:   	XString unit test.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XStringTest.h,v $
 *		Revision 1.2  2000/11/09 12:53:50  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/02/00	JDJ		Created
 */

#pragma once

#include <XString.h>
#include <XUnitTest.h>

#if DEBUG
namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XStringUnitTest
// ===================================================================================	
class XStringUnitTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XStringUnitTest();
	
						XStringUnitTest();
						
//-----------------------------------
//	API
//
protected:
	virtual void 		OnTest();

//-----------------------------------
//	Internal API
//
private:
			void 		DoReserveTest();
			void 		DoTestAssignment();
			void 		DoTestAppend();
			void 		DoTestInsert();
			void 		DoTestRemove();
			void 		DoTestReplace();
			void 		DoTestAccess();
			void 		DoTestFind();
			void 		DoTestCompare();
			void 		DoTestTranscoders();

//-----------------------------------
//	Member Data
//
private:
	XString mStr1;
	XString mStr2;
	XString mStr3;
	XString mStr4;
	XString mStr5;
	XString mStr6;
	XString mStr7;
	XString mStr8;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}		// namespace Whisper
#endif	// DEBUG