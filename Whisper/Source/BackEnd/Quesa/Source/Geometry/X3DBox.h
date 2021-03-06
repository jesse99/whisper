/*
 *  File:       X3DBox.h
 *  Summary:	A generalized 3D box.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DBox.h,v $
 *		Revision 1.3  2000/12/13 08:50:30  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:14:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DBox.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:50:30  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <->	  2/27/97	JDJ		Created.
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
//	class X3DBox
//!		A generalized 3D box.
// ===================================================================================
class QUESA_EXPORT X3DBox : public X3DGeometry {

	typedef X3DGeometry Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DBox();
						
						X3DBox(const X3DRect& box);

						X3DBox(const TQ3BoxData& data);

						X3DBox(TQ3GeometryObject object);
						
	virtual X3DBox* Clone() const;
		
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

	//! @name Major Axis
	//@{
			X3DVector 	GetMajorAxis() const;
						
			void 		SetMajorAxis(const X3DVector& axis);
	//@}

	//! @name Minor Axis
	//@{
			X3DVector 	GetMinorAxis() const;
						
			void 		SetMinorAxis(const X3DVector& axis);
	//@}

	//! @name Face Attributes
	//@{
			TQ3AttributeSet GetFaceAttributeSet(uint32 faceIndex) const;
						
			void 		SetFaceAttributeSet(uint32 faceIndex, TQ3AttributeSet attributes);
	//@}

	//! @name Data
	//@{
			TQ3BoxData 	GetData() const;
						
			void 		SetData(const TQ3BoxData& data);
	//@}

	//! @name Submitting (retained mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}
									
	//! @name Submitting (immediate mode)
	//@{
	static	void 		Submit(const X3DRect& box, TQ3ViewObject view);

	static	void 		Submit(const TQ3BoxData& data, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DBox. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
