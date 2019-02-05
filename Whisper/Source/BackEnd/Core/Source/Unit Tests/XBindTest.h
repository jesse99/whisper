/*
 *  File:       XBindTest.h
 *  Summary:   	Unit test for the Bind template functions.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XBindTest.h,v $
 *		Revision 1.1  2000/11/12 07:39:47  jesjones
 *		Removed the procedure adaptors.
 *		
 */

#pragma once

#include <XUnitTest.h>

#if DEBUG
namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XBindTest
// ===================================================================================
class XBindTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XBindTest();
	
						XBindTest();
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnTest();

//-----------------------------------
//	Internal API
//
protected:
			void 		DoBind0Test();
			void 		DoBind1Test();
			void 		DoBind2Test();
			void 		DoBind3Test();
			
			void 		DoSubtract1(int32 x)							{mValue = x;}
			void 		DoSubtract2(int32 x, int32 y)					{mValue = x - y;}
			void 		DoSubtract3(int32 x, int32 y, int32 z)			{mValue = x - y - z;}
			void 		DoSubtract4(int32 x, int32 y, int32 z, int32 w)	{mValue = x - y - z - w;}

//-----------------------------------
//	Member Data
//
protected:
	int32	mValue;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}		// namespace Whisper
#endif	// DEBUG