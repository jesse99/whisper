/*
 *  File:       XRegisterGraphicTests.cpp
 *  Summary:   	Entry point used to install all the Graphic unit tests.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRegisterGraphicTests.cpp,v $
 *		Revision 1.2  2000/11/09 12:48:50  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/06/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XRegisterGraphicTests.h>

#include <XDesktopTest.h>
#include <XDisplayTest.h>
#include <XPNGTest.h>
#include <XShapesTest.h>

#if DEBUG
namespace Whisper {


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// RegisterGraphicTests
//
// MSVC and CodeWarrior on Windows don't seem to have any way to
// disable dead-stripping of static ctors so we'll require some
// external party to do the registration...
//
//---------------------------------------------------------------
void RegisterGraphicTests()
{
	static XDesktopUnitTest sDesktopTest;
	static XDisplayUnitTest sDisplayTest;
	static XPNGTest         sPNGTest;
	static XShapesUnitTest  sShapesTest;
}


}		// namespace Whisper
#endif	// DEBUG