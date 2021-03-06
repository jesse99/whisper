/*
 *  File:       IWindowClick.h
 *  Summary:   	Mac interface used to respond to window clicks.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IWindowClick.h,v $
 *		Revision 1.1  2001/02/07 02:32:07  jesjones
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


//-----------------------------------
//	Forward References
//
class XMouseDownEvent;


// ===================================================================================
//	class IWindowClick
//!		Mac interface used to respond to window clicks.
// ===================================================================================
class IWindowClick : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual bool 		OnClick(const XMouseDownEvent& globalEvent, int16 partCode) = 0;
						/**< Return true if the click was handled. */
};

typedef XInterfacePtr<IWindowClick> IWindowClickPtr;
typedef XInterfacePtr<const IWindowClick> IConstWindowClickPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
