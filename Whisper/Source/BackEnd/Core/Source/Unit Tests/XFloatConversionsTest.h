/*
 *  File:       XFloatConversionsTest.h
 *  Summary:   	Unit test for the floating point to text functions.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFloatConversionsTest.h,v $
 *		Revision 1.2  2000/11/09 12:36:30  jesjones
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
//	class XFloatConvUnitTest
// ===================================================================================
class XFloatConvUnitTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XFloatConvUnitTest();
	
						XFloatConvUnitTest();
						
//-----------------------------------
//	API
//
protected:
	virtual void 		OnTest();

//-----------------------------------
//	Internal API
//
private:
			void 		DoStrToDoubleTest();

			void 		DoDoubleToStrTest();

			void 		DoDoubleToSciStrTest();
			
			void        DoDoubleToFormStr();
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}		// namespace Whisper
#endif	// DEBUG