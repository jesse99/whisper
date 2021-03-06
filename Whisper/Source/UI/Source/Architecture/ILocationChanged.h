/*
 *  File:       ILocationChanged.h
 *  Summary:   	Called by XControlGeometry and XWindowGeometry when the location changes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ILocationChanged.h,v $
 *		Revision 1.1  2001/02/07 02:31:53  jesjones
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
class XPoint;


// ===================================================================================
//	class ILocationChanged
//!		Called by XControlGeometry and XWindowGeometry when the location changes.
// ===================================================================================
class ILocationChanged : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		OnLocationChanged(const XPoint& newLoc) = 0;
};

typedef XInterfacePtr<ILocationChanged> ILocationChangedPtr;
typedef XInterfacePtr<const ILocationChanged> IConstLocationChangedPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
