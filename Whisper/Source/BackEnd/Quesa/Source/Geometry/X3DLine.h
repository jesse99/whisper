/*
 *  File:       X3DLine.h
 *  Summary:	Encapsulates a 3D line.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DLine.h,v $
 *		Revision 1.3  2000/12/13 08:52:20  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:17:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DLine.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:52:20  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <->	  2/28/97	JDJ		Created.
 */

#pragma once

#include <X3DGeometry.h>
#include <X3DPrimitives.h>
#include <X3DVectors.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class X3DColorRGB;


// ===================================================================================
//	class X3DLine
//!		A 3D line segment shape.
// ===================================================================================
class QUESA_EXPORT X3DLine : public X3DGeometry {

	typedef X3DGeometry Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DLine();
						
						X3DLine(const X3DPoint& start, const X3DPoint& stop, const X3DColorRGB& color);

						X3DLine(const X3DPoint& start, const X3DPoint& stop, TQ3AttributeSet attributes = nil);

						X3DLine(const TQ3LineData& data);

						X3DLine(TQ3GeometryObject object);
						
	virtual X3DLine* Clone() const;
		
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
			TQ3LineData GetData() const;
						
			void 		SetData(const TQ3LineData& data);
	//@}

	//! @name Submitting (retained mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}
									
	//! @name Submitting (immediate mode)
	//@{
	static	void 		Submit(const X3DPoint& start, const X3DPoint& stop, TQ3ViewObject view, const X3DColorRGB& color);

	static	void 		Submit(const X3DPoint& start, const X3DPoint& stop, TQ3ViewObject view, TQ3AttributeSet attributes = nil);

	static	void 		Submit(const TQ3LineData& data, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DLine. */
	//@}
};



#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
