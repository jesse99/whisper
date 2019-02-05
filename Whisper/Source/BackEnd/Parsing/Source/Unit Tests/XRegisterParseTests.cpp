/*
 *  File:       XRegisterParseTests.cpp
 *  Summary:   	Entry point used to install all the Parsing unit tests.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRegisterParseTests.cpp,v $
 *		Revision 1.2  2000/11/09 12:48:57  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/06/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XRegisterParseTests.h>

#include <XDFATest.h>
#include <XLexerTest.h>
#include <XNDFATest.h>
#include <XParserTest.h>
#include <XRegularExprTest.h>

#if DEBUG
namespace Whisper {


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// RegisterParseTests
//
// MSVC and CodeWarrior on Windows don't seem to have any way to
// disable dead-stripping of static ctors so we'll require some
// external party to do the registration...
//
//---------------------------------------------------------------
void RegisterParseTests()
{
	static XDFAUnitTest sDFATest;
	static XLexerUnitTest sLexerTest;
	static XNDFAUnitTest sNDFATest;
	static XParserUnitTest sParserTest;
	static XRegExprUnitTest sRegExprTest;
}


}		// namespace Whisper
#endif	// DEBUG