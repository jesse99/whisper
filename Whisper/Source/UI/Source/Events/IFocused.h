/*
 *  File:       IFocused.h
 *  Summary:   	Hook for implementations that want to be notified of key focus changes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IFocused.h,v $
 *		Revision 1.2  2001/02/21 08:07:50  jesjones
 *		Removed some extra whitespace.
 *		
 *		Revision 1.1  2001/01/30 07:27:28  jesjones
 *		Checked in	
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IFocused
//!		Hook for implementations that want to be notified of key focus changes.
// ===================================================================================
class IFocused : public XUnknown {

//-----------------------------------
//	API
//
public:					
	virtual void 		OnAcquiredFocus() = 0;
	virtual void 		OnLostFocus() = 0;
};

typedef XInterfacePtr<IFocused> IFocusedPtr;
typedef XInterfacePtr<const IFocused> IConstIFocusedPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
