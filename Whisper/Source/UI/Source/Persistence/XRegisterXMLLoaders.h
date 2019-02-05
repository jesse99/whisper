/*
 *  File:       XRegisterXMLLoaders.h
 *  Summary:   	Registers XML load functions for the Whisper classes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRegisterXMLLoaders.h,v $
 *		Revision 1.3  2001/02/07 04:59:04  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.2  2000/11/09 12:49:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	Global Functions
// ===================================================================================
UI_EXPORT void 		RegisterWhisperLoaders();
					// Note that calling this function means that the linker will not
					// be able to dead strip unused classes.


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
