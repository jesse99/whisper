/*
 *  File:       ICanClose.h
 *  Summary:   	Interface that is called to determine if a window can close.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ICanClose.h,v $
 *		Revision 1.1  2001/02/07 02:31:46  jesjones
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
//	class ICanClose
//!		Interface that is called to determine if a window can close.
// ===================================================================================
class ICanClose : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual bool 		OnCanClose(bool quitting) const = 0;
};

typedef XInterfacePtr<ICanClose> ICanClosePtr;
typedef XInterfacePtr<const ICanClose> IConstCanClosePtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
