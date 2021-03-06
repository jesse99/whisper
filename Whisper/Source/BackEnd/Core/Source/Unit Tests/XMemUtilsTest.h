/*
 *  File:       XMemUtilsTest.h
 *  Summary:   	Unit test for the memory utils functions.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XMemUtilsTest.h,v $
 *		Revision 1.3  2001/04/27 04:15:47  jesjones
 *		Added a test for the new Base64 encoder/decoder.
 *		
 *		Revision 1.2  2000/11/09 12:42:54  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XUnitTest.h>

#if DEBUG
namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XMemUtilsUnitTest
// ===================================================================================	
class XMemUtilsUnitTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XMemUtilsUnitTest();
	
						XMemUtilsUnitTest();
						
//-----------------------------------
//	API
//
protected:
	virtual void 		OnTest();

//-----------------------------------
//	Internal API
//
private:
			void 		DoTestMemoryOperations();
			void 		DoTestBitOperations();
			void 		DoTestBase64();

			void		DoVerifyBlock(uint8* block, uint32 size, uint8 value);
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}		// namespace Whisper
#endif	// DEBUG