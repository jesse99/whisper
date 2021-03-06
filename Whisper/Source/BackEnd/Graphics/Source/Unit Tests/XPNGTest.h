/*
 *  File:       XPNGTest.h
 *  Summary:	PNG image import/export tests.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XPNGTest.h,v $
 *		Revision 1.2  2000/11/09 12:46:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	  9/09/00	JDJ		Created
 */

#pragma once

#include <XPixMap.h>
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
//	class XPNGTest
// ===================================================================================	
#if DEBUG
class XPNGTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XPNGTest();
	
						XPNGTest();
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnTest();

//-----------------------------------
//	Internal API
//
protected:
			XPixMapPtr 	DoCreate32BitImage();
			XPixMapPtr 	DoCreate8BitImage();
			XPixMapPtr 	DoCreate1BitImage();
	
			void 		DoDumpPixels(const XPixMap& pixels);
			void 		DoCompare(const XPixMap& lhs, const XPixMap& rhs);
			void 		DoTest(const XPixMap& lhs);
};
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

