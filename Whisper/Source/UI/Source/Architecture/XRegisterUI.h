/*
 *  File:       XRegisterUI.h
 *  Summary:   	Provides functions to register the UI interfaces and implementations.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRegisterUI.h,v $
 *		Revision 1.4  2001/02/07 04:59:01  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/19 07:58:44  jesjones
 *		Added ITextBox and some range interfaces.
 *		
 *		Revision 1.2  2000/11/09 12:49:11  jesjones
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
UI_EXPORT 	void 	RegisterUIInterfaces();
					// Registers all of the UI interfaces with the object model. Note 
					// that this won't cause any code bloat.

UI_EXPORT 	void 	RegisterUIImplementations();
					// Registers all the UI implementation classes with the object 
					// model. Note that this means that the linker won't be able to
					// strip out any of these classes: if you care about code bloat
					// you should register only the implementation's that your app uses.


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
