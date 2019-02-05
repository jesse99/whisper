/*
 *  File:       XCallbacksTest.cpp
 *  Summary:	XCallback unit test.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XCallbacksTest.cpp,v $
 *		Revision 1.4  2000/11/12 07:39:58  jesjones
 *		Removed the procedure adaptors.
 *		
 *		Revision 1.3  2000/11/12 06:32:43  jesjones
 *		Added an XDebug.h include.
 *		
 *		Revision 1.2  2000/11/09 12:27:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 1/27/00	JDJ		Created (from Rich Hickey's callback code)
 */

#include <XWhisperHeader.h>
#include <XCallbacksTest.h>

#include <XCallbacks.h>
#include <XDebug.h>

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class XCallbacksTest
// ===================================================================================

int32 XCallbacksTest::msFlag;

//---------------------------------------------------------------
//
// XCallbacksTest::~XCallbacksTest
//
//---------------------------------------------------------------
XCallbacksTest::~XCallbacksTest()
{
}

	
//---------------------------------------------------------------
//
// XCallbacksTest::XCallbacksTest
//
//---------------------------------------------------------------
XCallbacksTest::XCallbacksTest() : XUnitTest(L"Backend", L"Callbacks")
{
}
						

//---------------------------------------------------------------
//
// XCallbacksTest::OnTest
//
//---------------------------------------------------------------
void XCallbacksTest::OnTest()
{
	this->DoProcedure0Test();
	this->DoFunction0Test();
	this->DoProcedure1Test();
	this->DoFunction1Test();
	this->Do2PlusTests();
	this->DoVoidReturnTests();
	
	TRACE("Completed callbacks test.\n\n");
}

			
//---------------------------------------------------------------
//
// XCallbacksTest::DoProcedure0Test
//										
//---------------------------------------------------------------
void XCallbacksTest::DoProcedure0Test()
{
	// Test pointer to function
	XCallback0<bool> callback(XCallbacksTest::DoCSetFlag);
	msFlag = false;
	callback();
	ASSERT(msFlag);

	// Test pointer to method
	callback = XCallback0<bool>(this, &XCallbacksTest::DoObjSetFlag);
	msFlag = false;
	callback();
	ASSERT(msFlag);
	
	// Stuff that should generate compile errors
//	callback = XCallback0<void>(XCallbacksTest::DoCComputeSqr16);	// can't use a function that takes an argument with XCallback0 (but default arguments seem to work)
//	callback = XCallback0<void>(DoObjSetFlag);						// can't treat a method as a function
//	callback = XCallback0<void>(&XCallbacksTest::DoObjSetFlag);		// can't treat a method as a function
//	callback = XCallback1<void, int16>(5);							// can't use an integer instead of a function
}


//---------------------------------------------------------------
//
// XCallbacksTest::DoFunction0Test
//										
//---------------------------------------------------------------
void XCallbacksTest::DoFunction0Test()
{
	// Test pointer to function
	XCallback0<bool> callback(XCallbacksTest::DoCSetFlag);
	msFlag = false;
	callback();
	ASSERT(msFlag);

	// Test pointer to method
	XCallback0<int32> callback2(this, &XCallbacksTest::DoObjSetFlag);	// note that this isn't an exact match: the actual function returns a bool but our callback returns an int32
	msFlag = false;
	callback2();
	ASSERT(msFlag);

	// Stuff that should generate compile errors
//	callback = XCallback0<bool>(XCallbacksTest::DoCComputeSqr16);		// can't use a function that takes an argument with XCallback0 (but default arguments seem to work)
//	XCallback0<int32*> callback3(XCallbacksTest::DoCSetFlag);			// can't convert a bool into an int32*
//	callback = XCallback0<bool>(DoObjSetFlag);							// can't treat a method as a function
//	callback = XCallback0<bool>(&XCallbacksTest::DoObjSetFlag);			// can't treat a method as a function
}


//---------------------------------------------------------------
//
// XCallbacksTest::DoProcedure1Test
//										
//---------------------------------------------------------------
void XCallbacksTest::DoProcedure1Test()
{
	// Test pointer to function
	XCallback1<int16, int16> callback(XCallbacksTest::DoCComputeSqr16);
	msFlag = 0;
	callback(4);
	ASSERT(msFlag == 16);

	// Test pointer to method
	callback = XCallback1<int16, int16>(this, &XCallbacksTest::DoObjComputeSqr16);
	msFlag = 0;
	callback(5);
	ASSERT(msFlag == 25);
	
	// Stuff that should generate compile errors
//	callback = XCallback1<void, int16>(XCallbacksTest::DoCSetFlag);		// can't use a function that takes no arguments with XCallback1
//	callback = XCallback1<void, int16>(XCallbacksTest::Do2Sum);			// can't use a function that takes two arguments with XCallback1
//	callback = XCallback1<void, int16>(BreakStrToDebugger);				// can't use a function with the wrong argument type
//	callback = XCallback1<void, const char*>(BreakStrToDebugger);		// can't assign to a callback with a different type
}


//---------------------------------------------------------------
//
// XCallbacksTest::DoFunction1Test
//										
//---------------------------------------------------------------
void XCallbacksTest::DoFunction1Test()
{
	// Test pointer to function
	XCallback1<int16, int16> callback(XCallbacksTest::DoCComputeSqr16);
	msFlag = 0;
	callback(3);
	ASSERT(msFlag == 9);

	// Test pointer to method
	XCallback1<int32, int16> callback2(this, &XCallbacksTest::DoObjComputeSqr16);	// note that this isn't an exact match: the actual function returns an int16 but our callback returns an int32
	msFlag = 0;
	callback2(10);
	ASSERT(msFlag == 100);		
	
	// Test a function from the standard library
	XCallback1<int32, double> callback3(std::roundtol);
	int32 value = callback3(3.14);
	ASSERT(value == 3);
	
	// Stuff that should generate compile errors
//	XCallback1<int32*> callback4(XCallbacksTest::DoCSetFlag);			// can't convert a bool into an int32*
}


//---------------------------------------------------------------
//
// XCallbacksTest::Do2PlusTests
//										
//---------------------------------------------------------------
void XCallbacksTest::Do2PlusTests()
{
	SAlpha a;
	SBeta b;
	SGamma c;
	STheta d;
	
	// Test 2 argument versions
	XCallback2<int32, SAlpha, SBeta> proc2(XCallbacksTest::Do2);
	proc2(a, b);	

	XCallback2<int32, int32, int32> func2(XCallbacksTest::Do2Sum);	
	int32 value = func2(10, 3);
	ASSERT(value == 13);		

	// Test 3 argument versions
	XCallback3<int32, SAlpha&, SBeta&, SGamma&> proc3(XCallbacksTest::Do3);	
	proc3(a, b, c);	

	XCallback3<int32, SAlpha&, SBeta&, SGamma&> func3(XCallbacksTest::Do3);	
	(void) func3(a, b, c);	

	// Test 4 argument versions
	XCallback4<int32, SAlpha&, SBeta&, SGamma&, STheta&> proc4(XCallbacksTest::Do4);	
	proc4(a, b, c, d);	

	XCallback4<int32, SAlpha&, SBeta&, SGamma&, STheta&> func4(XCallbacksTest::Do4);	
	(void) func4(a, b, c, d);	
}


//---------------------------------------------------------------
//
// XCallbacksTest::DoVoidReturnTests
//
//---------------------------------------------------------------
void XCallbacksTest::DoVoidReturnTests()
{
	XCallback0<int32> callback0(this, &XCallbacksTest::DoSet);	
	callback0();
	ASSERT(mValue == 100);		
	
	XCallback1<int32, int32> callback1(this, &XCallbacksTest::DoSubtract1);	
	callback1(23);
	ASSERT(mValue == 23);		

	XCallback2<int32, int32, int32> callback2(this, &XCallbacksTest::DoSubtract2);	
	callback2(100, 80);
	ASSERT(mValue == 20);		

	XCallback3<int32, int32, int32, int32> callback3(this, &XCallbacksTest::DoSubtract3);	
	callback3(100, 50, 20);
	ASSERT(mValue == 30);		

	XCallback4<int32, int32, int32, int32, int32> callback4(this, &XCallbacksTest::DoSubtract4);	
	callback4(10, 4, 2, 1);
	ASSERT(mValue == 3);		
}

#endif	// DEBUG
}		// namespace Whisper

	