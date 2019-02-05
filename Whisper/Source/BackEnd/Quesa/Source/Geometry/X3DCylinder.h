/*
 *  File:       X3DCylinder.h
 *  Summary:	A generalized 3D cylinder.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DCylinder.h,v $
 *		Revision 1.3  2000/12/13 08:51:08  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:14:46  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DCylinder.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:51:08  jesjones
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


// ===================================================================================
//	class X3DCylinder 
//!		A generalized 3D cylinder.
// ===================================================================================
class QUESA_EXPORT X3DCylinder : public X3DGeometry {

	typedef X3DGeometry Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DCylinder();
						
						X3DCylinder(const TQ3CylinderData& data);

						X3DCylinder(TQ3GeometryObject object);
						
	virtual X3DCylinder* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Origin
	//@{
			X3DPoint 	GetOrigin() const;
						
			void 		SetOrigin(const X3DPoint& origin);
	//@}

	//! @name Orientation
	//@{
			X3DVector 	GetOrientation() const;
						
			void 		SetOrientation(const X3DVector& orientation);
	//@}

	//! @name Major Radius
	//@{
			X3DVector 	GetMajorRadius() const;
						
			void 		SetMajorRadius(const X3DVector& radius);
	//@}

	//! @name Minor Radius
	//@{
			X3DVector 	GetMinorRadius() const;
						
			void 		SetMinorRadius(const X3DVector& radius);
	//@}

	//! @name Data
	//@{
			TQ3CylinderData GetData() const;
						
			void 		SetData(const TQ3CylinderData& data);
	//@}

	//! @name Submitting (retained mode)
	//@{
			void 		Submit(TQ3ViewObject view) const				{Inherited::Submit(view);}
	//@}
									
	//! @name Submitting (immediate mode)
	//@{
	static	void 		Submit(const TQ3CylinderData& data, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DCylinder. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

