/*
 *  File:       X3DMatrix.h
 *  Summary:   	TQ3Matrix4x4 wrapper class.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DMatrix.h,v $
 *		Revision 1.4  2001/04/14 07:29:42  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.3  2000/12/13 08:52:27  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:17:24  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Quesa.h>
#include <QuesaMath.h>

#include <X3DPrimitives.h>
#include <X3DVectors.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class X3DMatrix;


//-----------------------------------
//	Constants
//
extern QUESA_EXPORT const X3DMatrix kZero3DMatrix;
extern QUESA_EXPORT const X3DMatrix kIdentity3DMatrix;


// ===================================================================================
//	class X3DMatrix
//!		4x4 matrix class.
//		Note that these matrices use homogeneous coordinates: an extra dimension has
//		been added so that translations can be expressed in matrix form. When multiplying
//		a 3D vector or point to a matrix a fourth coordinate with value 1.0 is assumed.
// ===================================================================================
class QUESA_EXPORT X3DMatrix : public TQ3Matrix4x4 {

	typedef TQ3Matrix4x4 Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DMatrix()													{}
								
						X3DMatrix(float a00, float a01, float a02, float a03,
								  float a10, float a11, float a12, float a13,
								  float a20, float a21, float a22, float a23,
								  float a30, float a31, float a32, float a33);

						X3DMatrix(const TQ3Matrix4x4& rhs);

//-----------------------------------
//	API
//
public:
	//! @name Conversion operators
	//@{
						operator TQ3Matrix4x4() const								{return *reinterpret_cast<const TQ3Matrix4x4*>(this);}
						operator TQ3Matrix4x4*()									{return reinterpret_cast<TQ3Matrix4x4*>(this);}
						operator const TQ3Matrix4x4*() const						{return reinterpret_cast<const TQ3Matrix4x4*>(this);}
	//@}

	//! @name Scalar Arithmetic
	//@{
	friend	X3DMatrix 	operator*(float alpha, const X3DMatrix& rhs);
			X3DMatrix 	operator*(float alpha) const;
			X3DMatrix 	operator/(float alpha) const;

			X3DMatrix& 	operator*=(float alpha);
			X3DMatrix& 	operator/=(float alpha);
	//@}

	//! @name Vector Arithmetic
	//@{
	friend	X3DVector	operator*(const X3DVector& lhs, const X3DMatrix& rhs);
	friend	X3DPoint	operator*(const X3DPoint& lhs, const X3DMatrix& rhs);
						/**< Transforms the vector or point by the given matrix. */
	//@}

	//! @name Matrix Arithmetic
	//@{
			X3DMatrix 	operator-() const;

			X3DMatrix 	operator+(const X3DMatrix& rhs) const;
			X3DMatrix 	operator-(const X3DMatrix& rhs) const;
			X3DMatrix 	operator*(const X3DMatrix& rhs) const;

			X3DMatrix& 	operator+=(const X3DMatrix& rhs);
			X3DMatrix& 	operator-=(const X3DMatrix& rhs);
			X3DMatrix& 	operator*=(const X3DMatrix& rhs);
	//@}

	//! @name Matrix Operations
	//@{
			void 		MakeIdentity()													{*this = kIdentity3DMatrix;}
			
			void 		MakeZero()														{*this = kZero3DMatrix;}
			
	friend 	X3DMatrix 	Transpose(const X3DMatrix& rhs);

	friend 	X3DMatrix 	Invert(const X3DMatrix& rhs);
	
	friend	float 		Determinant(const X3DMatrix& rhs);
	//@}

	//! @name Transforms
	//@{
			void 		SetTranslate(float dx, float dy, float dz)						{Q3Matrix4x4_SetTranslate(*this, dx, dy, dz);}

			void 		SetScale(float dx, float dy, float dz)							{Q3Matrix4x4_SetScale(*this, dx, dy, dz);}

			void 		SetRotateAboutPoint(const X3DPoint& origin, float xAngle, float yAngle, float zAngle)	{Q3Matrix4x4_SetRotateAboutPoint(*this, origin, xAngle, yAngle, zAngle);}
	
			void 		SetRotateAboutAxis(const X3DPoint& origin, const X3DVector& orientation, float angle)	{Q3Matrix4x4_SetRotateAboutAxis(*this, origin, orientation, angle);}
						/**< Sets 'this' so that it rotates around an arbitrary axis (in orientation). */

			void 		SetRotateX(float angle)											{Q3Matrix4x4_SetRotate_X(*this, angle);}

			void 		SetRotateY(float angle)											{Q3Matrix4x4_SetRotate_Y(*this, angle);}

			void 		SetRotateZ(float angle)											{Q3Matrix4x4_SetRotate_Z(*this, angle);}

			void 		SetRotateXYZ(float xAngle, float yAngle, float zAngle)			{Q3Matrix4x4_SetRotate_XYZ(*this, xAngle, yAngle, zAngle);}

			void 		SetRotateVectorToVector(const X3DVector& v1, const X3DVector& v2)	{Q3Matrix4x4_SetRotateVectorToVector(*this, v1, v2);}
						/**< Sets 'this' so that v1*this == v2. */

			void 		SetQuaternion(const TQ3Quaternion& quaternion)					{Q3Matrix4x4_SetQuaternion(*this, &quaternion);}
	//@}

	//! @name Comparisons
	//@{
			bool 		IsIdentity() const												{return *this == kIdentity3DMatrix;}

			bool 		IsZero() const													{return *this == kZero3DMatrix;}

			bool 		operator==(const X3DMatrix& rhs) const							{return Equal(*this, rhs);}
			bool 		operator!=(const X3DMatrix& rhs) const							{return !Equal(*this, rhs);}
			
	friend	bool 		Equal(const X3DMatrix& lhs, const X3DMatrix& rhs, double tolerance = 1.0e-6);
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

