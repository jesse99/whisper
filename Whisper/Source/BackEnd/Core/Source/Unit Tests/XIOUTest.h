/*
 *  File:       XIOUTest.h
 *  Summary:   	XIOU and XThread unit test.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XIOUTest.h,v $
 *		Revision 1.2  2000/11/09 12:40:33  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/02/00	JDJ		Created
 */

#pragma once

#include <XIOU.h>
#include <XUnitTest.h>

#if DEBUG
namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XIOUTest
// ===================================================================================	
class XIOUTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XIOUTest();
	
						XIOUTest();
						
//-----------------------------------
//	API
//
protected:
	virtual void 		OnTest();

//-----------------------------------
//	Internal API
//
private:
			void 		DoComputePi(XIOU<double>& result, double epsilon);
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}		// namespace Whisper
#endif	// DEBUG