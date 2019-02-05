/*
 *	File:		XDisplayTest.cpp
 *	Function:	XDisplay unit test.
 *	Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDisplayTest.cpp,v $
 *		Revision 1.2  2000/11/09 12:32:08  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/06/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XDisplayTest.h>

#include <XDisplay.h>
#include <XExceptions.h>
#include <XNumbers.h>

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class XDisplayUnitTest
// ===================================================================================	

//---------------------------------------------------------------
//
// XDisplayUnitTest::~XDisplayUnitTest
//
//---------------------------------------------------------------
XDisplayUnitTest::~XDisplayUnitTest()
{
}

	
//---------------------------------------------------------------
//
// XDisplayUnitTest::XDisplayUnitTest
//
//---------------------------------------------------------------
XDisplayUnitTest::XDisplayUnitTest() : XUnitTest(L"Graphics", L"XDisplay")
{
}

						
//---------------------------------------------------------------
//
// XDisplayUnitTest::OnTest
//
//---------------------------------------------------------------
void XDisplayUnitTest::OnTest()
{
#if MAC
	GDHandle device = ::GetDeviceList();
	while (device != nil) {
		this->DoTestDevice(device);

		device = ::GetNextDevice(device);
	}
#else
	this->DoTestDevice(0);
#endif

	TRACE("Completed XDisplay test.\n\n");
}


//---------------------------------------------------------------
//
// XDisplayUnitTest::DoTestDevice
//
//---------------------------------------------------------------
void XDisplayUnitTest::DoTestDevice(OSDisplay device)
{
	XDisplay display(device);
	
	TRACE("Modes for device at: ", display.GetLocation(), "\n");

	XDisplay::iterator iter = display.begin();
	while (iter != display.end()) {
		XDisplayState state = *iter;
		++iter;
		
		TRACE("   ", state.GetResolution(), " at ", state.GetDepth(), " bits and ", (int32) state.GetRefreshRate(), " Hz\n");
	}
		
	TRACE("\n");
}


#endif	// DEBUG
}		// namespace Whisper
