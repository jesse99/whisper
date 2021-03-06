/*
 *  File:       X3DTriangle.h
 *  Summary:	Encapsulates a 3D filled triangle.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DTriangle.h,v $
 *		Revision 1.3  2000/12/13 08:55:19  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:22:57  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DTriangle.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:55:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <->	  3/06/97	JDJ		Created.
 */

#pragma once

#include <X3DGeometry.h>
#include <X3DPrimitives.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class X3DColorRGB;


// ===================================================================================
//	class X3DTriangle
//!		A 3D triangle shape.
// ===================================================================================
class QUESA_EXPORT X3DTriangle : public X3DGeometry {

	typedef X3DGeometry Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DTriangle();
						
						X3DTriangle(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3, 
									const X3DColorRGB& color);
						/**< Triangle is a solid color. */

						X3DTriangle(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3, 
									const X3DColorRGB& color1, const X3DColorRGB& color2, const X3DColorRGB& color3);
						/**< Triangle uses a color ramp (if interpolation style is kQ3InterpolationStyleVertex
						or kQ3InterpolationStylePixel). */

						X3DTriangle(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3, 
									TQ3AttributeSet attributes);

						X3DTriangle(const TQ3TriangleData& data);

						X3DTriangle(TQ3GeometryObject object);
						
	virtual X3DTriangle* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Vertex Position
	//@{
			X3DPoint 	GetVertexPosition(uint32 index) const;
						
			void 		SetVertexPosition(uint32 index, const X3DPoint& vertex);
	//@}

	//! @name Vertex Attributes
	//@{
			TQ3AttributeSet GetVertexAttributeSet(uint32 index) const;
						
			void 		SetVertexAttributeSet(uint32 index, TQ3AttributeSet attributes);
	//@}

	//! @name Data
	//@{
			TQ3TriangleData GetData() const;
						
			void 		SetData(const TQ3TriangleData& data);
	//@}

	//! @name Submitting (retained mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}
									
	//! @name Submitting (immediate mode)
	//@{
	static	void 		Submit(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3, 
							   TQ3ViewObject view, const X3DColorRGB& color);

	static	void 		Submit(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3, 
							   TQ3ViewObject view, const X3DColorRGB& color1, const X3DColorRGB& color2, 
							   const X3DColorRGB& color3);

	static	void 		Submit(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3, 
							   TQ3ViewObject view, TQ3AttributeSet attributes);

	static	void 		Submit(const TQ3TriangleData& data, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DTriangle. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
