/*
 *  File:       X3DMarker.h
 *  Summary:	QuickDraw 3D one-bit marker.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DMarker.h,v $
 *		Revision 1.3  2000/12/13 08:52:23  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:17:12  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DMarker.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:52:23  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <->	  3/01/97	JDJ		Created.
 */

#pragma once

#include <X3DBitmaps.h>
#include <X3DColors.h>
#include <X3DGeometry.h>
#include <X3DPrimitives.h>
#include <XRGBColor.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DMarker
//!		A black and white bitmap that always faces the camera.
// ===================================================================================
class QUESA_EXPORT X3DMarker : public X3DGeometry {

	typedef X3DGeometry Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DMarker();
						
						X3DMarker(const X3DPoint& origin, const XPoint& offset, const SBitmap& bitmap, 
								  const X3DColorRGB& color = kRGBBlack);
						/**< Offset is the number of pixels by which the topLeft corner of the
						marker is offset. Note that this saves a pointer to the image data
						so you can't delete the bitmap while the marker exists. */

						X3DMarker(const TQ3MarkerData& data);

						X3DMarker(TQ3GeometryObject object);
						
	virtual X3DMarker* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Position
	//@{
			X3DPoint 	GetPosition() const;
						
			void 		SetPosition(const X3DPoint& position);
	//@}

	//! @name X Offset
	//@{
			long 		GetXOffset() const;
						
			void 		SetXOffset(long offset);
	//@}

	//! @name Y Offset
	//@{
			long 		GetYOffset() const;
						
			void 		SetYOffset(long offset);
	//@}

	//! @name Bitmap
	//@{
			SBitmap 	GetBitmap() const;
						
			void 		SetBitmap(const SBitmap& bitmap);
	//@}

	//! @name Data
	//@{
			TQ3MarkerData GetData() const;
						
			void 		SetData(const TQ3MarkerData& data);
	//@}

	//! @name Submitting (retained mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}
									
	//! @name Submitting (immediate mode)
	//@{
	static	void 		Submit(const X3DPoint& location, const XPoint& offset, const SBitmap& bitmap, TQ3ViewObject view, const X3DColorRGB& color = kRGBBlack);

	static	void 		Submit(const TQ3MarkerData& data, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DMarker. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

