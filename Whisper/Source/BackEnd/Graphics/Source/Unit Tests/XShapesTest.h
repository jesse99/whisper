/*
 *  File:       XShapesTest.h
 *  Summary:	XShapes unit test.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XShapesTest.h,v $
 *		Revision 1.2  2000/11/09 12:51:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	  4/06/00	JDJ		Created
 */

#pragma once

#include <XUnitTest.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XPixMap;


// ===================================================================================
//	class XShapesUnitTest
// ===================================================================================	
class XShapesUnitTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XShapesUnitTest();
	
						XShapesUnitTest();
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnTest();

//-----------------------------------
//	Internal API
//
protected:
			void 		DoTestLine();
			void 		DoTestRect();
			void 		DoTestRegion();

			void 		DoCompare(const XPixMap& pixels, const uint8* bits);
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

