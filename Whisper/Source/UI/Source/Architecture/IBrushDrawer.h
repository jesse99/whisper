/*
 *  File:       IBrushDrawer.h
 *  Summary:   	Interface used with IDraw for drawers using brushes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IBrushDrawer.h,v $
 *		Revision 1.4  2001/02/07 04:55:43  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:19:56  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 08:59:55  jesjones
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
class XBrush;
class XRGBColor;


// ===================================================================================
//	class IBrushDrawer
//!		Interface used with IDraw for drawers using brushes.
// ===================================================================================
class IBrushDrawer : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		SetBrushes(const XBrush& active, const XBrush& inactive) = 0;

	virtual void 		SetColors(const XRGBColor& active, const XRGBColor& inactive) = 0;
						/**< Provided for implementations that may be able to draw more
						efficiently with a color instead of a brush. */
};

typedef XInterfacePtr<IBrushDrawer> IBrushDrawerPtr;
typedef XInterfacePtr<const IBrushDrawer> IConstBrushDrawerPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
