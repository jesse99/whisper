/*
 *	File:		XDisplayTest.h
 *	Function:	XDisplay unit test.
 *	Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDisplayTest.h,v $
 *		Revision 1.2  2000/11/09 12:32:11  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/06/00	JDJ		Created
 */

#pragma once

#include <XTypes.h>
#include <XUnitTest.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XDisplayUnitTest
// ===================================================================================	
class XDisplayUnitTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XDisplayUnitTest();
	
						XDisplayUnitTest();
						
//-----------------------------------
//	API
//
protected:
	virtual void 		OnTest();

//-----------------------------------
//	Internal API
//
private:
			void 		DoTestDevice(OSDisplay device);
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
