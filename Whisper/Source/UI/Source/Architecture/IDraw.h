/*
 *  File:       IDraw.h
 *  Summary:   	The interface used to draw windows and user controls.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IDraw.h,v $
 *		Revision 1.4  2001/03/05 05:37:21  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:21:21  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:01:30  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
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
class XDrawContext;
class XRegion;


// ===================================================================================
//	class IDraw
//!		The interface used to draw windows and user controls.
// ===================================================================================
class IDraw : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		HandleDraw(XDrawContext& canvas, const XRegion& dirtyRgn, bool isActive) = 0;
};

typedef XInterfacePtr<IDraw> IDrawPtr;
typedef XInterfacePtr<const IDraw> IConstDrawPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
