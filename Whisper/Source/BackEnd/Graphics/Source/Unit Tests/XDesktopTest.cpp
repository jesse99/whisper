/*
 *	File:		XDesktopTest.cpp
 *	Function:	XDesktop unit test.
 *	Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDesktopTest.cpp,v $
 *		Revision 1.2  2000/11/09 12:31:15  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/06/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XDesktop.h>

#include <XDesktopTest.h>

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class XDesktopUnitTest
// ===================================================================================	

//---------------------------------------------------------------
//
// XDesktopUnitTest::~XDesktopUnitTest
//
//---------------------------------------------------------------
XDesktopUnitTest::~XDesktopUnitTest()
{
}

	
//---------------------------------------------------------------
//
// XDesktopUnitTest::XDesktopUnitTest
//
//---------------------------------------------------------------
XDesktopUnitTest::XDesktopUnitTest() : XUnitTest(L"Graphics", L"XDesktop")
{
}

						
//---------------------------------------------------------------
//
// XDesktopUnitTest::OnTest
//
//---------------------------------------------------------------
void XDesktopUnitTest::OnTest()
{
	XDesktop::iterator iter = XDesktop::begin();
	while (iter != XDesktop::end()) {
		XDisplay display = *iter;
		++iter;
		
		TRACE("Location:   ", display.GetLocation(), "\n");
		TRACE("Resolution: ", display.GetResolution(), "\n");
		TRACE("Depth:      ", display.GetDepth(), "\n");
		TRACE("Is Color:   ", display.IsColor(), "\n");
		TRACE("\n");
	}

	TRACE("Completed XDesktop test.\n\n");
}


#endif	// DEBUG
}		// namespace Whisper
