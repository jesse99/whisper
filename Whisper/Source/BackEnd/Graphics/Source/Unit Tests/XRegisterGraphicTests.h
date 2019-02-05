/*
 *  File:       XRegisterGraphicTests.h
 *  Summary:   	Entry point used to install all the Graphic unit tests.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRegisterGraphicTests.h,v $
 *		Revision 1.3  2001/03/15 09:09:31  jesjones
 *		Uses the right export macro.
 *		
 *		Revision 1.2  2000/11/09 12:48:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTypes.h>

#if DEBUG
namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	Global Functions
// ===================================================================================
GRAPHICS_EXPORT void 	RegisterGraphicTests();


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}		// namespace Whisper
#endif	// DEBUG