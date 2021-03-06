/*
 *  File:       IGeometry.h
 *  Summary:   	Interface for objects that have a position and size.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IGeometry.h,v $
 *		Revision 1.4  2000/12/27 00:59:25  jesjones
 *		Fixed header comment.
 *		
 *		Revision 1.3  2000/12/14 08:22:06  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:02:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	  6/29/00	JDJ		Created
 */

#pragma once

#include <XConstants.h>
#include <XInterfacePtr.h>
#include <XRegion.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IGeometry
//!		Interface for objects that have a position and size.
// ===================================================================================
class IGeometry : public XUnknown {

//-----------------------------------
//	API
//
public:
	//! @name Location
	//@{
	virtual XPoint 		GetLocation() const = 0;
	
	virtual void 		SetLocation(const XPoint& newLoc, bool redraw = kRedraw) = 0;
	//@}
	
	//! @name Size
	//@{
	virtual int32 		GetWidth() const = 0;	
	virtual int32 		GetHeight() const = 0;
	virtual XSize 		GetSize() const = 0;
	
	virtual void 		SetSize(const XSize& newSize, bool redraw = kRedraw) = 0;
						/**< This refers to the client area for windows. */
	//@}

	//! @name Frame/Extent
	//@{
	virtual XRect 		GetFrame() const = 0;
						/**< Result is in parent coordinates. */
		
	virtual XRect 		GetExtent() const = 0;
						/**< Result is in 'this' coordinates. Normally topLeft will be
						kZeroPt, but for scrollable controls it may not be. */
		
	virtual XRegion 	GetVisibleExtent() const = 0;
						/**< Returns the extent clipped against the parent bounds. */

	virtual void 		SetFrame(const XRect& newFrame, bool redraw = kRedraw) = 0;
	//@}
		
	//! @name Min/Max size
	//@{
	virtual XSize 		GetMinSize() const = 0;
	virtual XSize 		GetMaxSize() const = 0;

	virtual void		SetMinSize(const XSize& newMin, bool redraw = kRedraw) = 0;
						/**< Generally defaults to (0, 0) but windows default to (16, 16). */
						
	virtual void		SetMaxSize(const XSize& newMax, bool redraw = kRedraw) = 0;
						/**< Defaults to (16000, 16000). */

	virtual void 		SetMinMaxSize(const XSize& newMin, const XSize& newMax, bool redraw = kRedraw) = 0;
	//@}
};

typedef XInterfacePtr<IGeometry> IGeometryPtr;
typedef XInterfacePtr<const IGeometry> IConstGeometryPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
