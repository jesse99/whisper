/*
 *  File:       X3DVectors.h
 *  Summary:   	QuickDraw 3D vector classes.
 *  Written by: Jesse Jones
 *
 *	Classes:	X2DVector	- 2D vector class
 *				X3DVector	- 3D vector class
 *
 *  Copyright � 1997-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DVectors.h,v $
 *		Revision 1.5  2001/04/14 07:33:35  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.4  2001/03/02 11:24:00  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/12/13 08:55:44  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:24:09  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <9>	 4/16/00	JDJ		Added X3DVector::CrossTriangleProduct.
 *		 <8>	 4/15/00	JDJ		Added an X3DVector taking two 3D points.
 *		 <7>	 2/01/98	JDJ		Inlined some assignment operators.
 *		 <6>	 1/30/98	JDJ		Added streaming operators.
 *		 <5>	12/31/97	JDJ		Fixed previous change (wasn't using rhs.z).
 *		 <4>	12/10/97	JDJ		Added explicit X3DVector(TQ3Point3D) ctor.
 *		 <3>	10/21/97	JDJ		Scalar ops are doubles instead of floats.
 *		 <2>	 4/15/97	JDJ		Made Equal a friend function.
 *		 <1>	 2/14/97	JDJ		Created
 */

#pragma once

#include <cmath>
#include <Quesa.h>

#include <XDebug.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class X3DPoint;
class X2DVector;
class X3DVector;
class XInStream;
class XOutStream;


//-----------------------------------
//	Constants
//
extern QUESA_EXPORT const X2DVector kZero2DVector;
extern QUESA_EXPORT const X3DVector kZero3DVector;

extern QUESA_EXPORT const X3DVector kXVector;		// unit vectors
extern QUESA_EXPORT const X3DVector kYVector;
extern QUESA_EXPORT const X3DVector kZVector;


// ===================================================================================
//	class X3DVector
//!		3D vector class.
// ===================================================================================
class QUESA_EXPORT X3DVector {

//-----------------------------------	
//	Initialization/Destruction
//
public:
						X3DVector()													{}
						
						X3DVector(float inX, float inY, float inZ)					{x = inX; y = inY; z = inZ;}
		
	explicit			X3DVector(const TQ3Vector2D& rhs)							{x = rhs.x; y = rhs.y; z = 0.0f;}

	explicit			X3DVector(const TQ3Point3D& rhs)							{x = rhs.x; y = rhs.y; z = rhs.z;}

						X3DVector(const TQ3Vector3D& rhs)							{x = rhs.x; y = rhs.y; z = rhs.z;}

						X3DVector(const TQ3Point3D& start, const TQ3Point3D& end);

//-----------------------------------
//	API
//
public:
	//! @name Conversion operators
	//@{
						operator TQ3Vector3D() const								{return *reinterpret_cast<const TQ3Vector3D*>(this);}
						
						operator TQ3Vector3D*()										{return reinterpret_cast<TQ3Vector3D*>(this);}
						
						operator const TQ3Vector3D*() const							{return reinterpret_cast<const TQ3Vector3D*>(this);}
	//@}

	//! @name Arithmetic by a scalar
	//@{
	friend	X3DVector 	operator*(double alpha, const X3DVector& rhs);
			X3DVector 	operator*(double alpha) const;
			X3DVector 	operator/(double alpha) const;

			X3DVector& 	operator*=(double alpha);
			X3DVector& 	operator/=(double alpha);
	//@}

	//! @name Vector Arithmetic
	//@{
			X3DVector 	operator-() const											{return X3DVector(-x, -y, -z);}

			X3DVector 	operator+(const X3DVector& rhs) const;
			X3DVector 	operator-(const X3DVector& rhs) const;

			X3DVector& 	operator+=(const X3DVector& rhs);
			X3DVector& 	operator-=(const X3DVector& rhs);
	//@}

	//! @name Vector Operations
	//@{
			double 		Length() const												{return std::sqrt((double) x*x + y*y + z*z);}			
			double 		LengthSquared() const										{return (double) x*x + y*y + z*z;}

	friend 	X3DVector 	Normalize(const X3DVector& rhs);

	friend	double 		DotProduct(const X3DVector& lhs, const X3DVector& rhs)		{return (double) lhs.x*rhs.x + (double) lhs.y*rhs.y + (double) lhs.z*rhs.z;}

	friend	X3DVector 	CrossProduct(const X3DVector& lhs, const X3DVector& rhs);
						/**< Returns the a vector normal to both lhs and rhs. */

	friend	X3DVector 	CrossTriangleProduct(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3);
						/**< Returns the vector normal to the triangle defined by the specified points. */

	friend	double 		Angle(const X3DVector& lhs, const X3DVector& rhs);
						/**< Returns the angle between the two vectors. */
	//@}

	//! @name Comparisons
	//@{
			bool 		operator==(const X3DVector& rhs) const						{return Equal(*this, rhs);}
			bool 		operator!=(const X3DVector& rhs) const						{return !Equal(*this, rhs);}
			
	friend bool 		Equal(const X3DVector& lhs, const X3DVector& rhs, double tolerance = 1.0e-6);
	//@}

	//! @name Streaming
	//@{
	friend 	XInStream& 	operator>>(XInStream& stream, X3DVector& rhs);
	friend	XOutStream& operator<<(XOutStream& stream, const X3DVector& rhs);
	
	friend 	void 			FromStr(const std::wstring& str, X3DVector& value);
	friend 	std::wstring	ToStr(const X3DVector& value);
	//@}

//-----------------------------------
//	Member data
//
public:
	float	x;
	float	y;
	float	z;
};


// ===================================================================================
//	class X2DVector
//!		2D vector class.
// ===================================================================================
class QUESA_EXPORT X2DVector {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X2DVector()													{}
						
						X2DVector(float inX, float inY)								{x = inX; y = inY;}
		
						X2DVector(const TQ3Vector2D& rhs)							{x = rhs.x; y = rhs.y;}

//-----------------------------------
//	API
//
public:
	//! @name Conversion operators
	//@{
						operator TQ3Vector2D() const								{return *reinterpret_cast<const TQ3Vector2D*>(this);}
						
						operator TQ3Vector2D*()										{return reinterpret_cast<TQ3Vector2D*>(this);}

						operator const TQ3Vector2D*() const							{return reinterpret_cast<const TQ3Vector2D*>(this);}
	//@}

	//! @name Arithmetic by a scalar
	//@{
	friend	X2DVector 	operator*(double alpha, const X2DVector& rhs);
			X2DVector 	operator*(double alpha) const;
			X2DVector 	operator/(double alpha) const;

			X2DVector& 	operator*=(double alpha);
			X2DVector& 	operator/=(double alpha);
	//@}

	//! @name Vector Arithmetic
	//@{
			X2DVector 	operator-() const											{return X2DVector(-x, -y);}

			X2DVector 	operator+(const X2DVector& rhs) const;
			X2DVector 	operator-(const X2DVector& rhs) const;

			X2DVector& 	operator+=(const X2DVector& rhs);
			X2DVector& 	operator-=(const X2DVector& rhs);
	//@}

	//! @name Vector Operations
	//@{
			double 		Length() const												{return std::sqrt((double) x*x + y*y);}
			double 		LengthSquared() const										{return (double) x*x + y*y;}

	friend	X2DVector 	Normalize(const X2DVector& rhs);

	friend	double 		DotProduct(const X2DVector& lhs, const X2DVector& rhs)		{return (double) lhs.x*rhs.x + (double) lhs.y*rhs.y;}

	friend	X3DVector 	CrossProduct(const X2DVector& lhs, const X2DVector& rhs);

	friend	double 		Angle(const X2DVector& lhs, const X2DVector& rhs);
						/**< Returns the angle between the two vectors. */
	//@}

	//! @name Comparisons
	//@{
			bool 		operator==(const X2DVector& rhs) const						{return Equal(*this, rhs);}
			bool 		operator!=(const X2DVector& rhs) const						{return !Equal(*this, rhs);}
			
	friend	bool 		Equal(const X2DVector& lhs, const X2DVector& rhs, double tolerance = 1.0e-6);
	//@}

	//! @name Streaming
	//@{
	friend 	XInStream& 	operator>>(XInStream& stream, X2DVector& rhs);
	friend	XOutStream& operator<<(XOutStream& stream, const X2DVector& rhs);
	
	friend 	void 			FromStr(const std::wstring& str, X2DVector& value);
	friend 	std::wstring	ToStr(const X2DVector& value);
	//@}

//-----------------------------------
//	Member data
//
public:
	float	x;
	float	y;
};


// ===================================================================================
//	Inlines
// ===================================================================================

inline X2DVector& X2DVector::operator*=(double alpha)
{
	x = (float) (x*alpha);
	y = (float) (y*alpha);
	
	return *this;
}

inline X2DVector& X2DVector::operator/=(double alpha)
{
	ASSERT(alpha != 0.0);
	
	x = (float) (x/alpha);
	y = (float) (y/alpha);
	
	return *this;
}

inline X2DVector& X2DVector::operator+=(const X2DVector& rhs)
{
	x += rhs.x;
	y += rhs.y;
		
	return *this;
}

inline X2DVector& X2DVector::operator-=(const X2DVector& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
		
	return *this;
}

#pragma mark -

inline X3DVector& X3DVector::operator*=(double alpha)
{
	x = (float) (x*alpha);
	y = (float) (y*alpha);
	z = (float) (z*alpha);
	
	return *this;
}

inline X3DVector& X3DVector::operator/=(double alpha)
{
	ASSERT(alpha != 0.0);
	
	x = (float) (x/alpha);
	y = (float) (y/alpha);
	z = (float) (z/alpha);
	
	return *this;
}

inline X3DVector& X3DVector::operator+=(const X3DVector& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
		
	return *this;
}

inline X3DVector& X3DVector::operator-=(const X3DVector& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
		
	return *this;
}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
