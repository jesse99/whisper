/*
 *  File:       XTest.h
 *  Summary:	XCallback unit test.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XCallbacksTest.h,v $
 *		Revision 1.3  2000/11/12 07:40:03  jesjones
 *		Removed the procedure adaptors.
 *		
 *		Revision 1.2  2000/11/09 12:27:40  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XCallbacksTest.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/11/12 07:40:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Removed the procedure adaptors.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
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
//	class XCallbacksTest
// ===================================================================================
class XCallbacksTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XCallbacksTest();
	
						XCallbacksTest();
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnTest();

//-----------------------------------
//	Internal Types
//
protected:
	struct SAlpha	{};
	struct SBeta	{};
	struct SGamma	{};
	struct STheta	{};

//-----------------------------------
//	Internal API
//
protected:
			void 		DoProcedure0Test();
			void 		DoFunction0Test();
			void 		DoVoidReturnTests();
	static 	bool 		DoCSetFlag()				{msFlag = true; return (bool) msFlag;}
			bool 		DoObjSetFlag()				{msFlag = true; return (bool) msFlag;}
	
			void 		DoProcedure1Test();
			void 		DoFunction1Test();
	static	int16 		DoCComputeSqr16(int16 x)	{msFlag = x*x; return (int16) (x*x);}
			int16 		DoObjComputeSqr16(int16 x)	{msFlag = x*x; return (int16) (x*x);}
	static	void 		DoFoo(const char* x)		{(void) x;}

			void 		Do2PlusTests();
	static	int32 		Do2Sum(int32 x, int32 y)					{return x + y;}
	static	int32 		Do3Sum(int32 x, int32 y, int32 z)			{return x + y + z;}
	static	int32 		Do4Sum(int32 x, int32 y, int32 z, int32 w)	{return x + y + z + w;}
	
	static 	int32 		Do2(SAlpha&, SBeta&)						{return 0;}
	static 	int32 		Do3(const SAlpha&, const SBeta&, const SGamma&)				{return 0;}
	static 	int32 		Do4(const SAlpha&, const SBeta&, const SGamma&, const STheta&)		{return 0;}

			int32 		DoSet()											{mValue = 100; return mValue;}
			int32 		DoSubtract1(int32 x)							{mValue = x; return mValue;}
			int32 		DoSubtract2(int32 x, int32 y)					{mValue = x - y; return mValue;}
			int32 		DoSubtract3(int32 x, int32 y, int32 z)			{mValue = x - y - z; return mValue;}
			int32 		DoSubtract4(int32 x, int32 y, int32 z, int32 w)	{mValue = x - y - z - w; return mValue;}

//-----------------------------------
//	Member Data
//
protected:
	static int32 	msFlag;
	int32			mValue;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}		// namespace Whisper
#endif	// DEBUG