/*
 *  File:       XIOU.cpp
 *  Summary:   	A class used to return the result of a threaded computation (also
 *				known as a future). 
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XIOU.cpp,v $
 *		Revision 1.2  2000/11/09 12:40:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 5/31/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XIOU.h>

#include <cmath>

#include <XAdaptors.h>
#include <XMiscUtils.h>
#include <XNumbers.h>
#include <XThread.h>
#include <XUnitTest.h>

#if MAC
	#include <MSystemInfo.h>
#endif

namespace Whisper {


// ===================================================================================
//	class ZIOUTest
// ===================================================================================	
#if DEBUG
class ZIOUTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~ZIOUTest();
	
						ZIOUTest();
						
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

static ZIOUTest sIOUTest;

//---------------------------------------------------------------
//
// ZIOUTest::~ZIOUTest
//
//---------------------------------------------------------------
ZIOUTest::~ZIOUTest()
{
}

	
//---------------------------------------------------------------
//
// ZIOUTest::ZIOUTest
//
//---------------------------------------------------------------
ZIOUTest::ZIOUTest() : XUnitTest(L"Backend", L"XIOU")
{
}

						
//---------------------------------------------------------------
//
// ZIOUTest::OnTest
//
//---------------------------------------------------------------
void ZIOUTest::OnTest()
{
#if MAC
	if (!MSystemInfo::HasThreadMgr()) {
		TRACE("Couldn't run the XIOU test (the Thread Manager isn't installed).\n\n");
		return;
	}	
#endif

	const double kEpsilon = 0.000001;
	
	XIOU<double> result;
	XCallback2<void, XIOU<double>&, double> temp(this, &ZIOUTest::DoComputePi);
	XCallback0<void> function = Adapt0(temp, result, kEpsilon);
	
	XThread::ErrorHandler handler(&result, &XIOU<double>::Abort);
	
	XThread* thread = XThread::Create(function, handler);
	thread->Start();
	thread->RemoveReference();				// XIOU's are ref counted so we don't need to hang onto the thread
	
	result.Wait();
				
	if (result.Redeemable())
		TRACE("Completed XIOU test, pi = ", result.Redeem(), "\n\n");
	else
		TRACE(L"Couldn't compute pi. ", result.GetAbortText(), "\n");		
}


//---------------------------------------------------------------
//
// ZIOUTest::DoComputePi
//
// Uses the identity pi/4 = Sum[(-1)^k/(2*k + 1)] for k = 0 to infinity.
//
//---------------------------------------------------------------
void ZIOUTest::DoComputePi(XIOU<double>& result, double epsilon)
{
	double value = 0.0;
	double delta;
	
	uint32 i = 0;
	
	do {
		delta = 1.0/(2*i + 1);
		value += delta;
		++i;
		
		delta = 1.0/(2*i + 1);
		value -= delta;
		++i;
		
//		throw std::bad_alloc();
				
		XThread::Yield();
	} while (delta > epsilon*value && i < ULONG_MAX-2);		
	
	result.Fulfill(4.0*value);
}

#endif	// DEBUG
}		// namespace Whisper
