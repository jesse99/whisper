/*
 *  File:       MAEToken.cpp
 *  Summary:   	An AEDesc thats used when resolving scripting descriptors.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MAEToken.cpp,v $
 *		Revision 1.3  2001/04/21 03:23:42  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 09:09:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <MAEToken.h>

#include <AEObjects.h>

#include <XDebug.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class MAEToken
// ===================================================================================

//---------------------------------------------------------------
//
// MAEToken::~MAEToken
//
//---------------------------------------------------------------
MAEToken::~MAEToken()
{
	OSErr err = AEDisposeToken(this);
	ASSERT(err == noErr);				// don't throw from dtor!

	descriptorType = typeNull;
	dataHandle     = nil;
}


//---------------------------------------------------------------
//
// MAEToken::MAEToken ()
//
//---------------------------------------------------------------
MAEToken::MAEToken()
{
}


//---------------------------------------------------------------
//
// MAEToken::MAEToken (AEDesc, int16)
//
//---------------------------------------------------------------
MAEToken::MAEToken(const AEDesc& objectSpecifier, int16 callbackFlags) 
{
	PRECONDITION(dataHandle == nil);

	OSErr err = AEResolve(&objectSpecifier, callbackFlags, this);
	ThrowIfOSErr(err);
}


}	// namespace Whisper
