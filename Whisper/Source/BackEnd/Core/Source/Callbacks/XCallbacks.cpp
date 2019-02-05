/*
 *  File:       XCallbacks.cpp
 *  Summary:	Light-weight wrappers around a pointer to a function or method.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XCallbacks.cpp,v $
 *		Revision 1.2  2000/11/09 12:27:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 1/27/00	JDJ		Created (from Rich Hickey's callback code)
 */

#include <XWhisperHeader.h>
#include <XCallbacks.h>

#include <XUnitTest.h>

namespace Whisper {


// ===================================================================================
//	class ZCallbacksTest
// ===================================================================================
#if DEBUG
class ZCallbacksTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~ZCallbacksTest();
	
						ZCallbacksTest();
						
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

//-----------------------------------
//	Member Data
//
protected:
	static int32 	msFlag;
};

static ZCallbacksTest sCallbacksTest;
int32 ZCallbacksTest::msFlag;

//---------------------------------------------------------------
//
// ZCallbacksTest::~ZCallbacksTest
//
//---------------------------------------------------------------
ZCallbacksTest::~ZCallbacksTest()
{
}

	
//---------------------------------------------------------------
//
// ZCallbacksTest::ZCallbacksTest
//
//---------------------------------------------------------------
ZCallbacksTest::ZCallbacksTest() : XUnitTest(L"Backend", L"Callbacks")
{
}
						

//---------------------------------------------------------------
//
// ZCallbacksTest::OnTest
//
//---------------------------------------------------------------
void ZCallbacksTest::OnTest()
{
	this->DoProcedure0Test();
	this->DoFunction0Test();
	this->DoProcedure1Test();
	this->DoFunction1Test();
	this->Do2PlusTests();
	
	TRACE("Completed callbacks test.\n\n");
}

			
//---------------------------------------------------------------
//
// ZCallbacksTest::DoProcedure0Test
//										
//---------------------------------------------------------------
void ZCallbacksTest::DoProcedure0Test()
{
	// Test pointer to function
	XCallback0<bool> callback(ZCallbacksTest::DoCSetFlag);
	msFlag = false;
	callback();
	ASSERT(msFlag);

	// Test pointer to method
	callback = XCallback0<bool>(this, &ZCallbacksTest::DoObjSetFlag);
	msFlag = false;
	callback();
	ASSERT(msFlag);
	
	// Stuff that should generate compile errors
//	callback = XCallback0<void>(ZCallbacksTest::DoCComputeSqr16);	// can't use a function that takes an argument with XCallback0 (but default arguments seem to work)
//	callback = XCallback0<void>(DoObjSetFlag);						// can't treat a method as a function
//	callback = XCallback0<void>(&ZCallbacksTest::DoObjSetFlag);		// can't treat a method as a function
//	callback = XCallback1<void, int16>(5);							// can't use an integer instead of a function
}


//---------------------------------------------------------------
//
// ZCallbacksTest::DoFunction0Test
//										
//---------------------------------------------------------------
void ZCallbacksTest::DoFunction0Test()
{
	// Test pointer to function
	XCallback0<bool> callback(ZCallbacksTest::DoCSetFlag);
	msFlag = false;
	callback();
	ASSERT(msFlag);

	// Test pointer to method
	XCallback0<int32> callback2(this, &ZCallbacksTest::DoObjSetFlag);	// note that this isn't an exact match: the actual function returns a bool but our callback returns an int32
	msFlag = false;
	callback2();
	ASSERT(msFlag);

	// Stuff that should generate compile errors
//	callback = XCallback0<bool>(ZCallbacksTest::DoCComputeSqr16);		// can't use a function that takes an argument with XCallback0 (but default arguments seem to work)
//	XCallback0<int32*> callback3(ZCallbacksTest::DoCSetFlag);			// can't convert a bool into an int32*
//	callback = XCallback0<bool>(DoObjSetFlag);							// can't treat a method as a function
//	callback = XCallback0<bool>(&ZCallbacksTest::DoObjSetFlag);			// can't treat a method as a function
}


//---------------------------------------------------------------
//
// ZCallbacksTest::DoProcedure1Test
//										
//---------------------------------------------------------------
void ZCallbacksTest::DoProcedure1Test()
{
	// Test pointer to function
	XCallback1<int16, int16> callback(ZCallbacksTest::DoCComputeSqr16);
	msFlag = 0;
	callback(4);
	ASSERT(msFlag == 16);

	// Test pointer to method
	callback = XCallback1<int16, int16>(this, &ZCallbacksTest::DoObjComputeSqr16);
	msFlag = 0;
	callback(5);
	ASSERT(msFlag == 25);
	
	// Stuff that should generate compile errors
//	callback = XCallback1<void, int16>(ZCallbacksTest::DoCSetFlag);		// can't use a function that takes no arguments with XCallback1
//	callback = XCallback1<void, int16>(ZCallbacksTest::Do2Sum);			// can't use a function that takes two arguments with XCallback1
//	callback = XCallback1<void, int16>(BreakStrToDebugger);				// can't use a function with the wrong argument type
//	callback = XCallback1<void, const char*>(BreakStrToDebugger);		// can't assign to a callback with a different type
}


//---------------------------------------------------------------
//
// ZCallbacksTest::DoFunction1Test
//										
//---------------------------------------------------------------
void ZCallbacksTest::DoFunction1Test()
{
	// Test pointer to function
	XCallback1<int16, int16> callback(ZCallbacksTest::DoCComputeSqr16);
	msFlag = 0;
	callback(3);
	ASSERT(msFlag == 9);

	// Test pointer to method
	XCallback1<int32, int16> callback2(this, &ZCallbacksTest::DoObjComputeSqr16);	// note that this isn't an exact match: the actual function returns an int16 but our callback returns an int32
	msFlag = 0;
	callback2(10);
	ASSERT(msFlag == 100);		
	
	// Test a function from the standard library
	XCallback1<int32, double> callback3(std::roundtol);
	int32 value = callback3(3.14);
	ASSERT(value == 3);
	
	// Stuff that should generate compile errors
//	XCallback1<int32*> callback4(ZCallbacksTest::DoCSetFlag);			// can't convert a bool into an int32*
}


//---------------------------------------------------------------
//
// ZCallbacksTest::Do2PlusTests
//										
//---------------------------------------------------------------
void ZCallbacksTest::Do2PlusTests()
{
	SAlpha a;
	SBeta b;
	SGamma c;
	STheta d;
	
	// Test 2 argument versions
	XCallback2<int32, SAlpha, SBeta> proc2(ZCallbacksTest::Do2);
	proc2(a, b);	

	XCallback2<int32, int32, int32> func2(ZCallbacksTest::Do2Sum);	
	int32 value = func2(10, 3);
	ASSERT(value == 13);		

	// Test 3 argument versions
	XCallback3<int32, SAlpha&, SBeta&, SGamma&> proc3(ZCallbacksTest::Do3);	
	proc3(a, b, c);	

	XCallback3<int32, SAlpha&, SBeta&, SGamma&> func3(ZCallbacksTest::Do3);	
	(void) func3(a, b, c);	

	// Test 4 argument versions
	XCallback4<int32, SAlpha&, SBeta&, SGamma&, STheta&> proc4(ZCallbacksTest::Do4);	
	proc4(a, b, c, d);	

	XCallback4<int32, SAlpha&, SBeta&, SGamma&, STheta&> func4(ZCallbacksTest::Do4);	
	(void) func4(a, b, c, d);	
}

#endif	// DEBUG


}		// namespace Whisper

	