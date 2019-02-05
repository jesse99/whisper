/*
 *  File:       ISizeChanged.h
 *  Summary:   	Called by XControlGeometry and XWindowGeometry when the size changes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ISizeChanged.h,v $
 *		Revision 1.1  2001/02/07 02:32:00  jesjones
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
class XSize;


// ===================================================================================
//	class ISizeChanged
//!		Called by XControlGeometry and XWindowGeometry when the size changes.
// ===================================================================================
class ISizeChanged : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		OnSizeChanged(const XSize& oldSize, const XSize& newSize) = 0;
};

typedef XInterfacePtr<ISizeChanged> ISizeChangedPtr;
typedef XInterfacePtr<const ISizeChanged> IConstSizeChangedPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
