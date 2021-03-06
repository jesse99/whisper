/*
 *  File:       X3DTransforms.h
 *  Summary:	QuickDraw 3D transform classes.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DMatrixTransform				- General matrix transform.
 *				X3DRotateTransform				- Rotates about the x, y, or z axis.
 *				X3DRotateAboutPointTransform	- Rotates about a point on the x, y, or z axis.
 *				X3DRotateAboutAxisTransform		- Rotates about a point on an arbitrary axis.
 *				X3DScaleTransform				- Scales along the x, y, and z axis.
 *				X3DTranslateTransform			- Moves an object along the x, y, and z axis.
 *				X3DQuaternionTransform			- Rotates and twists using a quaternion.
 *				X3DResetTransform				- $$$�not documented?
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DTransforms.h,v $
 *		Revision 1.5  2001/04/14 07:33:03  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.4  2000/12/15 09:07:49  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/13 08:55:15  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:22:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Quesa.h>
#include <QuesaTransform.h>

#include <X3DTransform.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DMatrixTransform
//!		General matrix transform.
// ===================================================================================
class QUESA_EXPORT X3DMatrixTransform : public X3DTransform {

	typedef X3DTransform Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DMatrixTransform();
						
						X3DMatrixTransform(const X3DMatrix& matrix);

						X3DMatrixTransform(TQ3TransformObject object);
						
	virtual X3DMatrixTransform* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Matrix
	//@{
			X3DMatrix 	Get() const									{return Inherited::GetMatrix();}
			
			void 		Set(const X3DMatrix& matrix);
	//@}

	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(const X3DMatrix& matrix, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DMatrixTransform. */
	//@}
};


// ===================================================================================
//	class X3DRotateTransform
//!		Rotates about the x, y, or z axis.
// ===================================================================================
class QUESA_EXPORT X3DRotateTransform : public X3DTransform {

	typedef X3DTransform Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DRotateTransform();
						
						X3DRotateTransform(const TQ3Axis& axis, float angle);
						// axis can be kQ3AxisX, kQ3AxisY, or kQ3AxisZ.

						X3DRotateTransform(const TQ3RotateTransformData& data);

						X3DRotateTransform(TQ3TransformObject object);
						
	virtual X3DRotateTransform* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Axis
	//@{
			TQ3Axis 	GetAxis() const;
			
			void 		SetAxis(const TQ3Axis& axis);
	//@}

	//! @name Angle
	//@{
			float 		GetAngle() const;
			
			void 		SetAngle(float angle);
	//@}

	//! @name Data
	//@{
			TQ3RotateTransformData GetData() const;
			
			void 		SetData(const TQ3RotateTransformData& data);
	//@}

	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(const TQ3Axis& axis, float angle, TQ3ViewObject view);

	static	void 		Submit(const TQ3RotateTransformData& data, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DRotateTransform. */
	//@}
};


// ===================================================================================
//	class X3DRotateAboutPointTransform
//!		Rotates about a point on the x, y, or z axis.
// ===================================================================================
class QUESA_EXPORT X3DRotateAboutPointTransform : public X3DTransform {

	typedef X3DTransform Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DRotateAboutPointTransform();
						
						X3DRotateAboutPointTransform(const TQ3Axis& axis, const X3DPoint& about, float angle);

						X3DRotateAboutPointTransform(const TQ3RotateAboutPointTransformData& data);

						X3DRotateAboutPointTransform(TQ3TransformObject object);
						
	virtual X3DRotateAboutPointTransform* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Axis
	//@{
			TQ3Axis 	GetAxis() const;
			
			void 		SetAxis(const TQ3Axis& axis);
	//@}

	//! @name Angle
	//@{
			float 		GetAngle() const;
			
			void 		SetAngle(float angle);
	//@}

	//! @name Point
	//@{
			X3DPoint 	GetAboutPoint() const;
			
			void 		SetAboutPoint(const X3DPoint& pt);
	//@}

	//! @name Data
	//@{
			TQ3RotateAboutPointTransformData GetData() const;
			
			void 		SetData(const TQ3RotateAboutPointTransformData& data);
	//@}

	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(const TQ3Axis& axis, const X3DPoint& about, float angle, TQ3ViewObject view);

	static	void 		Submit(const TQ3RotateAboutPointTransformData& data, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DRotateAboutPointTransform. */
	//@}
};


// ===================================================================================
//	class X3DRotateAboutAxisTransform
//!		Rotates about a point on an arbitrary axis.
// ===================================================================================
class QUESA_EXPORT X3DRotateAboutAxisTransform : public X3DTransform {

	typedef X3DTransform Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DRotateAboutAxisTransform();
						
						X3DRotateAboutAxisTransform(const TQ3Point3D& origin, const X3DVector& orientation, float angle);

						X3DRotateAboutAxisTransform(const TQ3RotateAboutAxisTransformData& data);

						X3DRotateAboutAxisTransform(TQ3TransformObject object);
						
	virtual X3DRotateAboutAxisTransform* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Orientation
	//@{
			X3DVector 	GetOrientation() const;
			
			void 		SetOrientation(const X3DVector& axis);
	//@}

	//! @name Angle
	//@{
			float 		GetAngle() const;
			
			void 		SetAngle(float angle);
	//@}

	//! @name Origin
	//@{
			X3DPoint 	GetOrigin() const;
			
			void 		SetOrigin(const X3DPoint& pt);
	//@}

	//! @name Data
	//@{
			TQ3RotateAboutAxisTransformData GetData() const;
			
			void 		SetData(const TQ3RotateAboutAxisTransformData& data);
	//@}

	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(const TQ3Point3D& origin, const X3DVector& orientation, float angle, TQ3ViewObject view);

	static	void 		Submit(const TQ3RotateAboutAxisTransformData& data, TQ3ViewObject view);
	//@}

	//! @name Misc -----
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DRotateAboutAxisTransform. */
	//@}
};


// ===================================================================================
//	class X3DScaleTransform
//!		Scales along the x, y, and z axis.
// ===================================================================================
class QUESA_EXPORT X3DScaleTransform : public X3DTransform {

	typedef X3DTransform Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DScaleTransform();
						
						X3DScaleTransform(float dx, float dy, float dz);

						X3DScaleTransform(const X3DVector& scale);

						X3DScaleTransform(TQ3TransformObject object);
						
	virtual X3DScaleTransform* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Data
	//@{
			X3DVector 	Get() const;
			
			void 		Set(const X3DVector& scale);
	//@}

	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(float dx, float dy, float dz, TQ3ViewObject view);

	static	void 		Submit(const X3DVector& scale, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DScaleTransform. */
	//@}
};


// ===================================================================================
//	class X3DTranslateTransform
//!		Moves an object along the x, y, and z axis.
// ===================================================================================
class QUESA_EXPORT X3DTranslateTransform : public X3DTransform {

	typedef X3DTransform Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DTranslateTransform();
						
						X3DTranslateTransform(float dx, float dy, float dz);

						X3DTranslateTransform(const X3DVector& translate);

						X3DTranslateTransform(TQ3TransformObject object);
						
	virtual X3DTranslateTransform* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Data
	//@{
			X3DVector 	Get() const;
			
			void 		Set(const X3DVector& translate);
	//@}

	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(float dx, float dy, float dz, TQ3ViewObject view);

	static	void 		Submit(const X3DVector& translate, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DTranslateTransform. */
	//@}
};


// ===================================================================================
//	class X3DQuaternionTransform
//!		Rotates and twists using a quaternion.
// ===================================================================================
class QUESA_EXPORT X3DQuaternionTransform : public X3DTransform {

	typedef X3DTransform Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DQuaternionTransform();
						
						X3DQuaternionTransform(const TQ3Quaternion& quaternion);

						X3DQuaternionTransform(TQ3TransformObject object);
						
	virtual X3DQuaternionTransform* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Data
	//@{
			TQ3Quaternion Get() const;
			
			void 		Set(const TQ3Quaternion& quaternion);
	//@}

	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(const TQ3Quaternion& quaternion, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DQuaternionTransform. */
	//@}
};


// ===================================================================================
//	class X3DResetTransform
//!		$$$�not documented?
// ===================================================================================
class QUESA_EXPORT X3DResetTransform : public X3DTransform {

	typedef X3DTransform Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DResetTransform();
						
						X3DResetTransform();

						X3DResetTransform(TQ3TransformObject object);
						
	virtual X3DResetTransform* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DResetTransform. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
