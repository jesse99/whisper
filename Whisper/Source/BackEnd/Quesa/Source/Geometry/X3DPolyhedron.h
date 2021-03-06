/*
 *  File:       X3DPolyhedron.h
 *  Summary:	A fast mesh geometry that can be edited.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DPolyhedron.h,v $
 *		Revision 1.3  2000/12/13 08:53:14  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:18:57  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DPolyhedron.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:53:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <->	  3/06/97	JDJ		Created.
 */

#pragma once

#include <X3DGeometry.h>
#include <X3DPrimitives.h>
#include <X3DVectors.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DPolyhedron
//!		A fast mesh geometry that can be edited.
// ===================================================================================
class QUESA_EXPORT X3DPolyhedron : public X3DGeometry {

	typedef X3DGeometry Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DPolyhedron();
						
						X3DPolyhedron(const TQ3PolyhedronData& data);

						X3DPolyhedron(TQ3GeometryObject object);
						
	virtual X3DPolyhedron* Clone() const;
		
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

	//! @name Triangle Data
	//@{
			TQ3PolyhedronTriangleData GetTriangleData(uint32 index) const;
						
			void 		SetTriangleData(uint32 index, const TQ3PolyhedronTriangleData& data);
	//@}

	//! @name Edge Data
	//@{
			TQ3PolyhedronEdgeData GetEdgeData(uint32 index) const;
						
			void 		SetEdgeData(uint32 index, const TQ3PolyhedronEdgeData& data);
	//@}

	//! @name Data
	//@{
			TQ3PolyhedronData GetData() const;
						
			void 		SetData(const TQ3PolyhedronData& data);
	//@}

	//! @name Submitting (retained mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}
									
	//! @name Submitting (immediate mode)
	//@{
	static	void 		Submit(const TQ3PolyhedronData& data, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DPolyhedron. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

