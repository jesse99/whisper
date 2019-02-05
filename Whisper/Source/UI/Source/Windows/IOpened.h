/*
 *  File:       IOpened.h
 *  Summary:   	Interface that is called when a window is opened and closed.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IOpened.h,v $
 *		Revision 1.1  2001/02/07 02:31:56  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IOpened
//!		Interface that is called when a window is opened and closed.
// ===================================================================================
class IOpened : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		OnOpened() = 0;
	virtual void 		OnClosing() = 0;
};

typedef XInterfacePtr<IOpened> IOpenedPtr;
typedef XInterfacePtr<const IOpened> IConstOpenedPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
