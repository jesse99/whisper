/*
 *  File:       MAEAddress.h
 *  Summary:   	An AEDesc that contains the target of an AppleEvent.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MAEAddress.h,v $
 *		Revision 1.4  2001/04/13 07:48:23  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2000/12/10 08:47:58  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:08:42  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <MAEDesc.h>

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
struct TargetID;

namespace Whisper {


// ===================================================================================
//	class MAEAddress
//!		An AEDesc that contains the target of an AppleEvent.
// ===================================================================================
class CORE_EXPORT MAEAddress : public MAEDesc {

	typedef MAEDesc Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~MAEAddress();
						
						MAEAddress();
					
						MAEAddress(ProcessSerialNumber psn);
						/**< Note that you can use kCurrentProcess to target your app (if
						you do this the event will be sent directly to your Apple
						Event handler bypassing the event queue). */
						
						MAEAddress(OSType signature);
						/**< Targets a running app with the specified signature. */

						MAEAddress(const AEAddressDesc& desc, bool copyDesc = true);
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
