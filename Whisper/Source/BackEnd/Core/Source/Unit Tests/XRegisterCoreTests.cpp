/*
 *  File:       XRegisterCoreTests.cpp
 *  Summary:   	Entry point used to install all the Core unit tests.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRegisterCoreTests.cpp,v $
 *		Revision 1.3  2000/11/12 07:55:44  jesjones
 *		Renamed the callback adaptor functions Bind. Removed the Procedure callback adaptors.
 *		
 *		Revision 1.2  2000/11/09 12:48:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 4/02/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XRegisterCoreTests.h>

#include <XBindTest.h>
#include <XCallbacksTest.h>
#include <XFloatConversionsTest.h>
#include <XIntConversionsTest.h>
#include <XIOUTest.h>
#include <XMemUtilsTest.h>
#include <XNumbersTest.h>
#include <XStreamingTest.h>
#include <XStringUtilsTest.h>
#include <XTextTranscodersTest.h>		

#if DEBUG
namespace Whisper {


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// RegisterCoreTests
//
// MSVC and CodeWarrior on Windows don't seem to have any way to
// disable dead-stripping of static ctors so we'll require some
// external party to do the registration...
//
//---------------------------------------------------------------
void RegisterCoreTests()
{
	static XNumbersTest 		sNumbersTest;
	static XMemUtilsUnitTest 	sMemUtilsTest;
	static XTranscoderUnitTest 	sTranscoderTest;
	static XStringUtilsUnitTest sStringUtilsTest;
	static XIntConvUnitTest 	sIntConvTest;
	static XFloatConvUnitTest 	sFloatConvTest;
	static XStreamUnitTest 		sStreamTest;
	static XCallbacksTest 		sCallbacksTest;
	static XBindTest 			sBindTest;
	static XIOUTest 			sIOUTest;
}


}		// namespace Whisper
#endif	// DEBUG