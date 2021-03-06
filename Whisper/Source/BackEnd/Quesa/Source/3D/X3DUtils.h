/*
 *  File:       X3DUtils.h
 *  Summary:   	Misc QuickDraw 3D utilities.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DUtils.h,v $
 *		Revision 1.3  2001/04/14 07:33:30  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:24:01  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Quesa.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	Global Functions
// ===================================================================================
QUESA_EXPORT bool Init3D();
			// Returns false if there was a problem initializing QuickDraw 3D (if this
			// happens MSystemInfo::hasQuickDraw3D is set to false).

QUESA_EXPORT void Terminate3D();
			// Call this when you're done with QuickDraw 3D.

QUESA_EXPORT void ThrowIf3DError(TQ3Status status);


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
