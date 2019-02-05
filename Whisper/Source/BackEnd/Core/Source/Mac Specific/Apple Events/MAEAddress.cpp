/*
 *  File:       MAEAddress.cpp
 *  Summary:   	An AEDesc that contains the target of an AppleEvent.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MAEAddress.cpp,v $
 *		Revision 1.4  2001/04/21 02:25:34  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/13 07:48:20  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.2  2000/11/09 09:08:39  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <MAEAddress.h>

#include <EPPC.h>

#include <XDebug.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class MAEAddress
// ===================================================================================

//---------------------------------------------------------------
//
// MAEAddress::~MAEAddress
//
//---------------------------------------------------------------
MAEAddress::~MAEAddress()
{
}


//---------------------------------------------------------------
//
// MAEAddress::MAEAddress ()
//
//---------------------------------------------------------------
MAEAddress::MAEAddress()
{
}


//---------------------------------------------------------------
//
// MAEAddress::MAEAddress (ProcessSerialNumber)
//
//---------------------------------------------------------------
MAEAddress::MAEAddress(ProcessSerialNumber psn) 
{
	PRECONDITION(dataHandle == nil);
	
	OSErr err = AECreateDesc(typeProcessSerialNumber, &psn, sizeof(ProcessSerialNumber), this);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAEAddress::MAEAddress (OSType)
//
//---------------------------------------------------------------
MAEAddress::MAEAddress(OSType signature) 
{
	PRECONDITION(dataHandle == nil);
	
	OSErr err = AECreateDesc(typeApplSignature, &signature, sizeof(signature), this);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAEAddress::MAEAddress (AEAddressDesc)
//
//---------------------------------------------------------------
MAEAddress::MAEAddress(const AEAddressDesc& desc, bool copyDesc) : MAEDesc(desc, copyDesc)
{
}
	

}	// namespace Whisper
