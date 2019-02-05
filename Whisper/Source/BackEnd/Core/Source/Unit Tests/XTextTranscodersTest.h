/*
 *  File:       XTextTranscodersTest.h
 *  Summary:   	Unit test for the text transcoders.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTextTranscodersTest.h,v $
 *		Revision 1.2  2000/11/09 12:56:12  jesjones
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
//	class XTranscoderUnitTest
// ===================================================================================	
class XTranscoderUnitTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XTranscoderUnitTest();
	
						XTranscoderUnitTest();
						
//-----------------------------------
//	API
//
protected:
	virtual void 		OnTest();

//-----------------------------------
//	Internal API
//
private:
			void 		DoNaiveTest();
			void 		DoAsciiTest();
			void 		DoMacTest();
			void 		DoWinTest();
			void 		DoUTF8Test();
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}		// namespace Whisper
#endif	// DEBUG