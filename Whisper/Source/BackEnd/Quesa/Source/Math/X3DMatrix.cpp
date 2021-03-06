/*
 *  File:       X3DMatrix.cpp
 *  Summary:   	TQ3Matrix4x4 wrapper class.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DMatrix.cpp,v $
 *		Revision 1.4  2001/04/21 03:36:26  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:29:37  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:17:18  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 4/16/00	JDJ		X3DPoint and X3DVector operator* functions use the QD3D routines
 *									(old code had rows/cols swapped i think).
 *		 <3>	 8/08/98	JDJ		X3DMatrix (TQ3Matrix4x4) doesn't use BlockMove on Win.
 *		 <2>	 4/15/97	JDJ		Made Equal a friend function.
 *		 <1>	 2/26/97	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <X3DMatrix.h>

#include <XNumbers.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const X3DMatrix kZero3DMatrix(0.0f, 0.0f, 0.0f, 0.0f,
							  0.0f, 0.0f, 0.0f, 0.0f,
							  0.0f, 0.0f, 0.0f, 0.0f,
							  0.0f, 0.0f, 0.0f, 0.0f);
								  
const X3DMatrix kIdentity3DMatrix(1.0f, 0.0f, 0.0f, 0.0f,
								  0.0f, 1.0f, 0.0f, 0.0f,
								  0.0f, 0.0f, 1.0f, 0.0f,
								  0.0f, 0.0f, 0.0f, 1.0f);

// ===================================================================================
//	class X3DMatrix
// ===================================================================================

//---------------------------------------------------------------
//
// X3DMatrix::X3DMatrix (float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float)
//
//---------------------------------------------------------------
X3DMatrix::X3DMatrix(float a00, float a01, float a02, float a03, float a10, float a11, float a12, float a13, float a20, float a21, float a22, float a23, float a30, float a31, float a32, float a33)
{
	value[0][0] = a00; value[0][1] = a01; value[0][2] = a02; value[0][3] = a03;
	value[1][0] = a10; value[1][1] = a11; value[1][2] = a12; value[1][3] = a13;
	value[2][0] = a20; value[2][1] = a21; value[2][2] = a22; value[2][3] = a23;
	value[3][0] = a30; value[3][1] = a31; value[3][2] = a32; value[3][3] = a33;
}


//---------------------------------------------------------------
//
// X3DMatrix::X3DMatrix (TQ3Matrix4x4)
//
//---------------------------------------------------------------
X3DMatrix::X3DMatrix(const TQ3Matrix4x4& rhs)
{
#if TARGET_OS_MAC
	BlockMoveData(rhs.value, value, sizeof(TQ3Matrix4x4));
#else
	for (int i = 0; i < 4; i++)			// $$ BlockMoveData isn't working with QTML?
		for (int j = 0; j < 4; j++)
			value[i][j] = rhs.value[i][j];
#endif
}


//---------------------------------------------------------------
//
// X3DMatrix::operator*
//
//---------------------------------------------------------------
X3DMatrix X3DMatrix::operator*(float alpha) const
{
	X3DMatrix temp;
	
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			temp.value[i][j] = alpha*value[i][j];
			
	return temp;
}


//---------------------------------------------------------------
//
// X3DMatrix::operator/
//
//---------------------------------------------------------------
X3DMatrix X3DMatrix::operator/(float alpha) const
{
	PRECONDITION(alpha != 0.0f);
	
	X3DMatrix temp;
	
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			temp.value[i][j] = value[i][j]/alpha;
			
	return temp;
}


//---------------------------------------------------------------
//
// X3DMatrix::operator*=
//
//---------------------------------------------------------------
X3DMatrix& X3DMatrix::operator*=(float alpha)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			value[i][j] *= alpha;
			
	return *this;
}


//---------------------------------------------------------------
//
// X3DMatrix::operator/=
//
//---------------------------------------------------------------
X3DMatrix& X3DMatrix::operator/=(float alpha)
{
	PRECONDITION(alpha != 0.0f);
	
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			value[i][j] /= alpha;
			
	return *this;
}


//---------------------------------------------------------------
//
// X3DMatrix::operator-
//
//---------------------------------------------------------------
X3DMatrix X3DMatrix::operator-() const
{
	X3DMatrix temp;
	
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			temp.value[i][j] = -value[i][j];
			
	return temp;
}


//---------------------------------------------------------------
//
// X3DMatrix::operator+
//
//---------------------------------------------------------------
X3DMatrix X3DMatrix::operator+(const X3DMatrix& rhs) const
{
	X3DMatrix temp;
	
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			temp.value[i][j] = value[i][j] + rhs.value[i][j];
			
	return temp;
}


//---------------------------------------------------------------
//
// X3DMatrix::operator-
//
//---------------------------------------------------------------
X3DMatrix X3DMatrix::operator-(const X3DMatrix& rhs) const
{
	X3DMatrix temp;
	
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			temp.value[i][j] = value[i][j] - rhs.value[i][j];
			
	return temp;
}


//---------------------------------------------------------------
//
// X3DMatrix::operator*
//
//---------------------------------------------------------------
X3DMatrix X3DMatrix::operator*(const X3DMatrix& rhs) const
{
	X3DMatrix temp;
	
	::Q3Matrix4x4_Multiply(*this, rhs, temp);
			
	return temp;
}


//---------------------------------------------------------------
//
// X3DMatrix::operator+=
//
//---------------------------------------------------------------
X3DMatrix& X3DMatrix::operator+=(const X3DMatrix& rhs)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			value[i][j] += rhs.value[i][j];
			
	return *this;
}


//---------------------------------------------------------------
//
// X3DMatrix::operator-=
//
//---------------------------------------------------------------
X3DMatrix& X3DMatrix::operator-=(const X3DMatrix& rhs)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			value[i][j] -= rhs.value[i][j];
			
	return *this;
}


//---------------------------------------------------------------
//
// X3DMatrix::operator*=
//
//---------------------------------------------------------------
X3DMatrix& X3DMatrix::operator*=(const X3DMatrix& rhs)
{
	::Q3Matrix4x4_Multiply(*this, rhs, *this);
			
	return *this;
}

#pragma mark  

//---------------------------------------------------------------
//
// operator* (float, X3DMatrix)
//
//---------------------------------------------------------------
X3DMatrix operator*(float alpha, const X3DMatrix& rhs)
{
	X3DMatrix temp;
	
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			temp.value[i][j] = alpha*rhs.value[i][j];
			
	return temp;
}


//---------------------------------------------------------------
//
// operator* (X3DVector, X3DMatrix)
//
//---------------------------------------------------------------
X3DVector operator*(const X3DVector& lhs, const X3DMatrix& rhs)
{
	X3DVector temp;
	
	::Q3Vector3D_Transform(lhs, rhs, temp);

	return temp;
}


//---------------------------------------------------------------
//
// operator* (X3DPoint, X3DMatrix)
//
//---------------------------------------------------------------
X3DPoint operator*(const X3DPoint& lhs, const X3DMatrix& rhs)
{
	X3DPoint temp;
	
	::Q3Point3D_Transform(lhs, rhs, temp);

	return temp;
}


//---------------------------------------------------------------
//
// Transpose
//
//---------------------------------------------------------------
X3DMatrix Transpose(const X3DMatrix& rhs)
{
	X3DMatrix temp;
	
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			temp.value[i][j] = rhs.value[j][i];
			
	return temp;
}


//---------------------------------------------------------------
//
// Invert
//
//---------------------------------------------------------------
X3DMatrix Invert(const X3DMatrix& rhs)
{
	PRECONDITION(Abs(Determinant(rhs)) > 1.0e-6);		// matrices are only invertible if determinant is non-zero
	
	X3DMatrix temp;
	
	Q3Matrix4x4_Invert(rhs, temp);
	
	return temp;
}


//---------------------------------------------------------------
//
// Determinant
//
//---------------------------------------------------------------
float Determinant(const X3DMatrix& rhs)
{
	float det = Q3Matrix4x4_Determinant(rhs);
	
	return det;
}


//---------------------------------------------------------------
//
// Equal (X3DMatrix, X3DMatrix, double)
//
//---------------------------------------------------------------
bool Equal(const X3DMatrix& lhs, const X3DMatrix& rhs, double tolerance)
{
	bool equal = true;
	
	for (int i = 0; i < 4 && equal; i++)
		for (int j = 0; j < 4 && equal; j++)
			equal = Equal(lhs.value[i][j], rhs.value[i][j], tolerance);
			
	return equal;
}


}	// namespace Whisper
