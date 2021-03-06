/*
 *  File:       X3DVectors.cpp
 *  Summary:   	QuickDraw 3D vector classes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DVectors.cpp,v $
 *		Revision 1.6  2001/05/06 10:41:19  jesjones
 *		Normalize asserts that the norm is positive.
 *		
 *		Revision 1.5  2001/04/21 03:38:57  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/03/02 11:23:56  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/11/20 05:43:47  jesjones
 *		Added std:: to a few function calls.
 *		
 *		Revision 1.2  2000/11/09 12:24:05  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <7>	 4/16/00	JDJ		Added X3DVector::CrossTriangleProduct.
 *		 <6>	 4/15/00	JDJ		Added an X3DVector taking two 3D points.
 *		 <5>	 1/30/98	JDJ		Added streaming operators.
 *		 <4>	 1/26/98	JDJ		Instead of ASSERTing that dot product is in [-1, +1]
 *									the Angle methods use MinMax to force it into the valid range.
 *		 <3>	10/21/97	JDJ		Scalar ops are doubles instead of floats.
 *		 <2>	 4/15/97	JDJ		Made Equal a friend function.
 *		 <1>	 2/14/97	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <X3DVectors.h>

#include <X3DPrimitives.h>
#include <XDebug.h>
#include <XNumbers.h>
#include <XStreaming.h>
#include <XTextConversions.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const X2DVector kZero2DVector(0.0f, 0.0f);
const X3DVector kZero3DVector(0.0f, 0.0f, 0.0f);

const X3DVector kXVector(1.0f, 0.0f, 0.0f);
const X3DVector kYVector(0.0f, 1.0f, 0.0f);
const X3DVector kZVector(0.0f, 0.0f, 1.0f);


// ===================================================================================
//	class X2DVector
// ===================================================================================

//---------------------------------------------------------------
//
// operator*
//
//---------------------------------------------------------------
X2DVector operator*(double alpha, const X2DVector& rhs)
{
	X2DVector result;
	
	result.x = (float) (alpha*rhs.x);
	result.y = (float) (alpha*rhs.y);
	
	return result;
}


//---------------------------------------------------------------
//
// X2DVector::operator*
//
//---------------------------------------------------------------
X2DVector X2DVector::operator*(double alpha) const
{
	X2DVector result;
	
	result.x = (float) (alpha*x);
	result.y = (float) (alpha*y);
	
	return result;
}


//---------------------------------------------------------------
//
// X2DVector::operator/
//
//---------------------------------------------------------------
X2DVector X2DVector::operator/(double alpha) const
{
	PRECONDITION(alpha != 0.0);
	
	X2DVector result;
	
	result.x = (float) (x/alpha);
	result.y = (float) (y/alpha);
	
	return result;
}


//---------------------------------------------------------------
//
// X2DVector::operator+
//
//---------------------------------------------------------------
X2DVector X2DVector::operator+(const X2DVector& rhs) const
{
	X2DVector result;
	
	result.x = x + rhs.x;
	result.y = y + rhs.y;
	
	return result;
}


//---------------------------------------------------------------
//
// X2DVector::operator-
//
//---------------------------------------------------------------
X2DVector X2DVector::operator-(const X2DVector& rhs) const
{
	X2DVector result;
	
	result.x = x - rhs.x;
	result.y = y - rhs.y;
	
	return result;
}


//---------------------------------------------------------------
//
// Normalize (X2DVector)
//
//---------------------------------------------------------------
X2DVector Normalize(const X2DVector& rhs)
{
	X2DVector result;
	
	double norm = rhs.Length();
	
	result.x = (float) (rhs.x/norm);
	result.y = (float) (rhs.y/norm);
	
	return result;
}


//---------------------------------------------------------------
//
// CrossProduct (X2DVector, X2DVector)							
//
//---------------------------------------------------------------
X3DVector CrossProduct(const X2DVector& lhs, const X2DVector& rhs)	
{
	X3DVector product = CrossProduct(X3DVector(lhs), X3DVector(rhs));
	
	return product;
}


//---------------------------------------------------------------
//
// Angle (X2DVector, X2DVector)							
//
//---------------------------------------------------------------
double Angle(const X2DVector& lhs, const X2DVector& rhs)
{
	// Adding numbers with large magnitude differences can cause
	// a loss of precision so we'll normalize the vectors before
	// taking the dot product.
	const X2DVector nlhs = Normalize(lhs);
	const X2DVector nrhs = Normalize(rhs);

	double dp = MinMax(-1.0, DotProduct(nlhs, nrhs), 1.0);
	double angle = std::acos(dp);
	
	return angle;
}


//---------------------------------------------------------------
//
// Equal (X3DVector, X3DVector, double)
//
//---------------------------------------------------------------
bool Equal(const X3DVector& lhs, const X3DVector& rhs, double tolerance)
{	
	bool equal = Equal(lhs.x, rhs.x, tolerance) && Equal(lhs.y, rhs.y, tolerance);
	
	return equal;
}


//---------------------------------------------------------------
//
// operator>> (X2DVector)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, X2DVector& rhs)
{
	stream >> rhs.x >> rhs.y;
	
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (X2DVector)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const X2DVector& rhs)
{
	stream << rhs.x << rhs.y;
	
	return stream;
}


//---------------------------------------------------------------
//
// FromStr (wstring, X2DVector)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& inStr, X2DVector& value)
{
	FromStr(inStr, value.x);
	
	std::wstring str = After(inStr, L" ");
	FromStr(str, value.y);
}


//---------------------------------------------------------------
//
// ToStr (X2DVector)
//
//---------------------------------------------------------------
std::wstring ToStr(const X2DVector& value)
{
	return ToStr(value.x) + L" " + ToStr(value.y);
}

#pragma mark -

// ===================================================================================
//	class X3DVector
// ===================================================================================

//---------------------------------------------------------------
//
// X3DVector::X3DVector (TQ3Point3D, TQ3Point3D)
//
//---------------------------------------------------------------
X3DVector::X3DVector(const TQ3Point3D& start, const TQ3Point3D& end)
{
	x = end.x - start.x;
	y = end.y - start.y;
	z = end.z - start.z;
}


//---------------------------------------------------------------
//
// operator*
//
//---------------------------------------------------------------
X3DVector operator*(double alpha, const X3DVector& rhs)
{
	X3DVector result;
	
	result.x = (float) (alpha*rhs.x);
	result.y = (float) (alpha*rhs.y);
	result.z = (float) (alpha*rhs.z);
	
	return result;
}


//---------------------------------------------------------------
//
// X3DVector::operator*
//
//---------------------------------------------------------------
X3DVector X3DVector::operator*(double alpha) const
{
	X3DVector result;
	
	result.x = (float) (alpha*x);
	result.y = (float) (alpha*y);
	result.z = (float) (alpha*z);
	
	return result;
}


//---------------------------------------------------------------
//
// X3DVector::operator/
//
//---------------------------------------------------------------
X3DVector X3DVector::operator/(double alpha) const
{
	PRECONDITION(alpha != 0.0);
	
	X3DVector result;
	
	result.x = (float) (x/alpha);
	result.y = (float) (y/alpha);
	result.z = (float) (z/alpha);
	
	return result;
}


//---------------------------------------------------------------
//
// X3DVector::operator+
//
//---------------------------------------------------------------
X3DVector X3DVector::operator+(const X3DVector& rhs) const
{
	X3DVector result;
	
	result.x = x + rhs.x;
	result.y = y + rhs.y;
	result.z = z + rhs.z;
	
	return result;
}


//---------------------------------------------------------------
//
// X3DVector::operator-
//
//---------------------------------------------------------------
X3DVector X3DVector::operator-(const X3DVector& rhs) const
{
	X3DVector result;
	
	result.x = x - rhs.x;
	result.y = y - rhs.y;
	result.z = z - rhs.z;
	
	return result;
}


//---------------------------------------------------------------
//
// Normalize (X3DVector)
//
//---------------------------------------------------------------
X3DVector Normalize(const X3DVector& rhs)
{
	X3DVector result;
	
	double norm = rhs.Length();
	ASSERT(norm > 0.0);
	
	result.x = (float) (rhs.x/norm);
	result.y = (float) (rhs.y/norm);
	result.z = (float) (rhs.z/norm);
	
	return result;
}


//---------------------------------------------------------------
//
// CrossProduct (X3DVector, X3DVector)									
//
//---------------------------------------------------------------
X3DVector CrossProduct(const X3DVector& lhs, const X3DVector& rhs)
{
	X3DVector product;
	
	product.x = lhs.y*rhs.z - lhs.z*rhs.y;
	product.y = lhs.z*rhs.x - lhs.x*rhs.z;
	product.z = lhs.x*rhs.y - lhs.y*rhs.x;
	
	return product;
}


//---------------------------------------------------------------
//
// CrossTriangleProduct								
//
//---------------------------------------------------------------
X3DVector CrossTriangleProduct(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3)
{
	X3DVector v1(pt2, pt1);
	X3DVector v2(pt3, pt1);

	X3DVector product = CrossProduct(v1, v2);
	
	return product;
}


//---------------------------------------------------------------
//
// Angle (X3DVector, X3DVector)								
//
//---------------------------------------------------------------
double Angle(const X3DVector& lhs, const X3DVector& rhs)
{
	// Adding numbers with large magnitude differences can cause
	// a loss of precision so we'll normalize the vectors before
	// taking the dot product.
	const X3DVector nlhs = Normalize(lhs);
	const X3DVector nrhs = Normalize(rhs);

	double dp = MinMax(-1.0, DotProduct(nlhs, nrhs), 1.0);

	double angle = std::acos(dp);
	
	return angle;
}


//---------------------------------------------------------------
//
// Equal (X2DVector, X2DVector, double)
//
//---------------------------------------------------------------
bool Equal(const X2DVector& lhs, const X2DVector& rhs, double tolerance)
{	
	bool equal = Equal(lhs.x, rhs.x, tolerance) && Equal(lhs.y, rhs.y, tolerance);
	
	return equal;
}


//---------------------------------------------------------------
//
// operator>> (X3DVector)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, X3DVector& rhs)
{
	stream >> rhs.x >> rhs.y >> rhs.z;
	
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (X3DVector)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const X3DVector& rhs)
{
	stream << rhs.x << rhs.y << rhs.z;
	
	return stream;
}


//---------------------------------------------------------------
//
// FromStr (wstring, X3DVector)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& inStr, X3DVector& value)
{
	FromStr(inStr, value.x);
	
	std::wstring str = After(inStr, L" ");
	FromStr(str, value.y);
	
	str = After(str, L" ");
	FromStr(str, value.z);
}


//---------------------------------------------------------------
//
// ToStr (X3DVector)
//
//---------------------------------------------------------------
std::wstring ToStr(const X3DVector& value)
{
	return ToStr(value.x) + L" " + ToStr(value.y) + L" " + ToStr(value.z);
}


}	// namespace Whisper
