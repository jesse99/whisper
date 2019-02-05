/*
 *  File:       XBindTest.cpp
 *  Summary:   	Unit test for the XAdaptor template functions.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XBindTest.cpp,v $
 *		Revision 1.2  2000/11/12 07:55:05  jesjones
 *		Renamed the callback adaptor functions Bind. Removed the Procedure callback adaptors.
 *		
 *		Revision 1.1  2000/11/12 07:39:43  jesjones
 *		Removed the procedure adaptors.
 *		
 *		Revision 1.2  2000/11/09 12:25:01  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XBindTest.h>

#include <XBind.h>

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class XBindTest
// ===================================================================================

//---------------------------------------------------------------
//
// XBindTest::~XBindTest
//
//---------------------------------------------------------------
XBindTest::~XBindTest()
{
}

	
//---------------------------------------------------------------
//
// XBindTest::XBindTest
//
//---------------------------------------------------------------
XBindTest::XBindTest() : XUnitTest(L"Backend", L"Callback Binders")
{
}
						

//---------------------------------------------------------------
//
// XBindTest::OnTest
//
//---------------------------------------------------------------
void XBindTest::OnTest()
{
	this->DoBind0Test();
	this->DoBind1Test();
	this->DoBind2Test();
	this->DoBind3Test();
	
	TRACE("Completed callback binder test.\n\n");
}


//---------------------------------------------------------------
//
// XBindTest::DoBind0Test
//										
//---------------------------------------------------------------
void XBindTest::DoBind0Test()
{
	XCallback0<void> callback;	

	XCallback1<void, int32> callback1(this, &XBindTest::DoSubtract1);	
	callback = Bind1(callback1, 10);	
	callback();
	ASSERT(mValue == 10);		

	XCallback2<void, int32, int32> callback2(this, &XBindTest::DoSubtract2);	
	callback = Bind2(callback2, 10, 4);	
	callback();
	ASSERT(mValue == 6);		

	XCallback3<void, int32, int32, int32> callback3(this, &XBindTest::DoSubtract3);	
	callback = Bind3(callback3, 10, 4, 5);	
	callback();
	ASSERT(mValue == 1);		

	XCallback4<void, int32, int32, int32, int32> callback4(this, &XBindTest::DoSubtract4);	
	callback = Bind4(callback4, 10, 4, 5, 2);	
	callback();
	ASSERT(mValue == -1);		

	// Stuff that should generate compile errors
//	callback = Bind1(callback1, kUnbound);				// Bind1 requires that all arguments be bound
//	callback = Bind2(callback2, 10, kUnbound);			// Bind2 requires that all arguments be bound
//	callback = Bind2(callback2, kUnbound, kUnbound);	// Bind2 requires that all arguments be bound
//	callback = Bind1(callback2, 10, kUnbound);			// can't convert the result of Bind1 to an XCallback0
}


//---------------------------------------------------------------
//
// XBindTest::DoBind1Test
//										
//---------------------------------------------------------------
void XBindTest::DoBind1Test()
{
	XCallback1<void, int32> callback;	

	XCallback2<void, int32, int32> callback2(this, &XBindTest::DoSubtract2);	
	callback = Bind1(callback2, 10, kUnbound);	
	callback(3);
	ASSERT(mValue == 7);		

	callback = Bind1(callback2, kUnbound, 10);	
	callback(3);
	ASSERT(mValue == -7);		

	XCallback3<void, int32, int32, int32> callback3(this, &XBindTest::DoSubtract3);	
	callback = Bind2(callback3, 10, 5, kUnbound);	
	callback(3);
	ASSERT(mValue == 2);		

	callback = Bind2(callback3, 10, kUnbound, 2);	
	callback(3);
	ASSERT(mValue == 5);		

	callback = Bind2(callback3, kUnbound, 4, 2);	
	callback(3);
	ASSERT(mValue == -3);		

	XCallback4<void, int32, int32, int32, int32> callback4(this, &XBindTest::DoSubtract4);	
	callback = Bind3(callback4, 20, 10, 5, kUnbound);	
	callback(3);
	ASSERT(mValue == 2);		

	callback = Bind3(callback4, 15, 10, kUnbound, 2);	
	callback(3);
	ASSERT(mValue == 0);		

	callback = Bind3(callback4, 15, kUnbound, 4, 2);	
	callback(3);
	ASSERT(mValue == 6);		

	callback = Bind3(callback4, kUnbound, 10, 4, 2);	
	callback(30);
	ASSERT(mValue == 14);		

	// Stuff that should generate compile errors
//	callback = Bind2(callback2, 10, 4);			// can't convert the result of Bind2 to an XCallback1
}


//---------------------------------------------------------------
//
// XBindTest::DoBind2Test
//										
//---------------------------------------------------------------
void XBindTest::DoBind2Test()
{
	XCallback2<void, int32, int32> callback;	

	XCallback3<void, int32, int32, int32> callback3(this, &XBindTest::DoSubtract3);	
	callback = Bind1(callback3, 10, kUnbound, kUnbound);	
	callback(4, 2);
	ASSERT(mValue == 4);		

	callback = Bind1(callback3, kUnbound, 4, kUnbound);	
	callback(10, 2);
	ASSERT(mValue == 4);		

	callback = Bind1(callback3, kUnbound, kUnbound, 2);	
	callback(10, 2);
	ASSERT(mValue == 6);		

	XCallback4<void, int32, int32, int32, int32> callback4(this, &XBindTest::DoSubtract4);	
	callback = Bind2(callback4, kUnbound, kUnbound, 10, 2);	
	callback(10, 2);
	ASSERT(mValue == -4);		

	callback = Bind2(callback4, kUnbound, 10, kUnbound, 2);	
	callback(10, 2);
	ASSERT(mValue == -4);		

	callback = Bind2(callback4, kUnbound, 10, 2, kUnbound);	
	callback(15, 2);
	ASSERT(mValue == 1);		

	callback = Bind2(callback4, 20, kUnbound, kUnbound, 2);	
	callback(10, 2);
	ASSERT(mValue == 6);		

	callback = Bind2(callback4, 20, kUnbound, 4, kUnbound);	
	callback(10, 2);
	ASSERT(mValue == 4);		

	callback = Bind2(callback4, 20, 10, kUnbound, kUnbound);	
	callback(4, 2);
	ASSERT(mValue == 4);		
}


//---------------------------------------------------------------
//
// XBindTest::DoBind3Test
//										
//---------------------------------------------------------------
void XBindTest::DoBind3Test()
{
	XCallback3<void, int32, int32, int32> callback;	

	XCallback4<void, int32, int32, int32, int32> callback4(this, &XBindTest::DoSubtract4);	
	callback = Bind1(callback4, kUnbound, kUnbound, kUnbound, 2);	
	callback(10, 4, 3);
	ASSERT(mValue == 1);		

	callback = Bind1(callback4, kUnbound, kUnbound, 10, kUnbound);	
	callback(10, 4, 3);
	ASSERT(mValue == -7);		

	callback = Bind1(callback4, kUnbound, 10, kUnbound, kUnbound);	
	callback(10, 4, 3);
	ASSERT(mValue == -7);		

	callback = Bind1(callback4, 12, kUnbound, kUnbound, kUnbound);	
	callback(8, 4, 3);
	ASSERT(mValue == -3);		
}

#endif	// DEBUG

}		// namespace Whisper

	