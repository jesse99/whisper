/*
 *  File:       X3DPixmapMarker.h
 *  Summary:	QuickDraw 3D multi-bit marker.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DPixmapMarker.h,v $
 *		Revision 1.3  2000/12/13 08:53:05  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:18:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DPixmapMarker.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:53:05  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <->	  3/02/97	JDJ		Created.
 */

#pragma once

#include <X3DBitmaps.h>
#include <X3DGeometry.h>
#include <X3DPrimitives.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DPixmapMarker
//!		A color pixmap that always faces the camera.
// ===================================================================================
class QUESA_EXPORT X3DPixmapMarker : public X3DGeometry {

	typedef X3DGeometry Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DPixmapMarker();
						
						X3DPixmapMarker(const X3DPoint& origin, const XPoint& offset, const SPixmap& pixmap);
						/**< Offset is the number of pixels by which the topLeft corner of the
						marker is offset. Note that this copies the the image data. */

						X3DPixmapMarker(const TQ3PixmapMarkerData& data);

						X3DPixmapMarker(TQ3GeometryObject object);
						
	virtual X3DPixmapMarker* Clone() const;
		
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

	//! @name Pixmap
	//@{
			TQ3StoragePixmap GetPixmap() const;
						
			void 		SetPixmap(TQ3StoragePixmap pixmap);
	//@}

	//! @name Data
	//@{
			TQ3PixmapMarkerData GetData() const;
						
			void 		SetData(const TQ3PixmapMarkerData& data);
	//@}

	//! @name Submitting (retained mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}
									
	//! @name Submitting (immediate mode)
	//@{
	static	void 		Submit(const X3DPoint& origin, const XPoint& offset, const SPixmap& pixmap, TQ3ViewObject view);

	static	void 		Submit(const TQ3PixmapMarkerData& data, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DPixmapMarker. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
