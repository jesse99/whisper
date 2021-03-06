/*
 *  File:       XNumbersTest.h
 *  Summary:   	Unit test for the stuff in XNumbers.h
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XNumbersTest.h,v $
 *		Revision 1.2  2000/11/09 12:44:50  jesjones
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
//	class XNumbersTest
// ===================================================================================
class XNumbersTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XNumbersTest();
	
						XNumbersTest();
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnTest();

//-----------------------------------
//	Internal API
//
protected:
			void 		DoConstantTest();
			
			void 		DoTestRandomInt();
			void 		DoTestFloatInt();
			
			void 		DoPlotUniform();
			void 		DoPlotNormal();
			
			void 		DoSpeedTest();
			void 		DoFlushCache(const char* buffer, uint32 count);
			
private:
	uint32	mSum;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}		// namespace Whisper
#endif	// DEBUG