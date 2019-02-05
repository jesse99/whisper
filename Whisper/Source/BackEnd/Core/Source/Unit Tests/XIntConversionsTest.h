/*
 *  File:       XIntConversionsTest.h
 *  Summary:   	Unit test for the int to text functions.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XIntConversionsTest.h,v $
 *		Revision 1.2  2000/11/09 12:39:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/02/00	JDJ		Created
 */

#pragma once

#include <XUnitTest.h>

#if DEBUG
namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XIntConvUnitTest
// ===================================================================================	
class XIntConvUnitTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XIntConvUnitTest();
	
						XIntConvUnitTest();
						
//-----------------------------------
//	API
//
protected:
	virtual void 		OnTest();

//-----------------------------------
//	Internal API
//
private:
			void 		DoStrToIntTest();
			void 		DoStrToUIntTest();
			void 		DoBadStrToIntTest();
			
			void 		DoIntToStrTest();
			void 		DoUIntToStrTest();

			void 		DoIntToHexTest();
			void 		DoUIntToHexTest();

			void 		DoIntToBinaryTest();
			void 		DoUIntToBinaryTest();
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}		// namespace Whisper
#endif	// DEBUG