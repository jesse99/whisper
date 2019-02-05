/*
 *  File:       X3DNurbCurve.h
 *  Summary:	A 3D curve defined by a NURB.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DNurbCurve.h,v $
 *		Revision 1.3  2000/12/13 08:52:44  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:17:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DNurbCurve.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:52:44  jesjones
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
//	class X3DNurbCurve
//!		A 3D curve defined by a NURB.
// ===================================================================================
class QUESA_EXPORT X3DNurbCurve : public X3DGeometry {

	typedef X3DGeometry Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DNurbCurve();
						
						X3DNurbCurve(const TQ3NURBCurveData& data);

						X3DNurbCurve(TQ3GeometryObject object);
						
	virtual X3DNurbCurve* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Control Point
	//@{
			TQ3RationalPoint4D GetControlPoint(uint32 index) const;
						
			void 		SetControlPoint(uint32 index, const TQ3RationalPoint4D& pt);
	//@}

	//! @name Knot
	//@{
			float 		GetKnot(uint32 index) const;
						
			void 		SetKnot(uint32 index, float value);
	//@}

	//! @name Data
	//@{
			TQ3NURBCurveData GetData() const;
						
			void 		SetData(const TQ3NURBCurveData& data);
	//@}

	//! @name Submitting (retained mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}
									
	//! @name Submitting (immediate mode)
	//@{
	static	void 		Submit(const TQ3NURBCurveData& data, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DNurbCurve. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

