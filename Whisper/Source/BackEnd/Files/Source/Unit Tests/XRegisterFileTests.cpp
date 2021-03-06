/*
 *  File:       XRegisterFileTests.cpp
 *  Summary:   	Entry point used to install all the Files unit tests.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRegisterFileTests.cpp,v $
 *		Revision 1.2  2000/11/09 12:48:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/02/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XRegisterFileTests.h>

#include <XFileIteratorTest.h>

#if DEBUG
namespace Whisper {


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// RegisterFileTests
//
// MSVC and CodeWarrior on Windows don't seem to have any way to
// disable dead-stripping of static ctors so we'll require some
// external party to do the registration...
//
//---------------------------------------------------------------
void RegisterFileTests()
{
	static XFileIteratorTest sFileIteratorTest;
}


}		// namespace Whisper
#endif	// DEBUG