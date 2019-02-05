/*
 *  File:       X3DNurbPatch.h
 *  Summary:	A 3D surface defined by a NURB.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DNurbPatch.h,v $
 *		Revision 1.3  2000/12/13 08:52:48  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:18:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DNurbPatch.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:52:48  jesjones
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
//	class X3DNurbPatch
//!		A 3D surface defined by a NURB.
// ===================================================================================
class QUESA_EXPORT X3DNurbPatch : public X3DGeometry {

	typedef X3DGeometry Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DNurbPatch();
						
						X3DNurbPatch(const TQ3NURBPatchData& data);

						X3DNurbPatch(TQ3GeometryObject object);
						
	virtual X3DNurbPatch* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Control Point
	//@{
			TQ3RationalPoint4D GetControlPoint(uint32 row, uint32 col) const;
						
			void 		SetControlPoint(uint32 row, uint32 col, const TQ3RationalPoint4D& pt);
	//@}

	//! @name U Knot
	//@{
			float 		GetUKnot(uint32 index) const;
						
			void 		SetUKnot(uint32 index, float value);
	//@}

	//! @name V Knot
	//@{
			float 		GetVKnot(uint32 index) const;
						
			void 		SetVKnot(uint32 index, float value);
	//@}

	//! @name Data
	//@{
			TQ3NURBPatchData GetData() const;
						
			void 		SetData(const TQ3NURBPatchData& data);
	//@}

	//! @name Submitting (retained mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}
									
	//! @name Submitting (immediate mode)
	//@{
	static	void 		Submit(const TQ3NURBPatchData& data, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DNurbPatch. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

