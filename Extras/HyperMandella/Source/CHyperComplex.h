/*
 *  File:       CHyperComplex.h
 *  Summary:   	Hyper complex number class. 
 *  Written by: Jesse Jones
 *
 *	Abstract:	Complex numbers can be generalized into four dimensions using
 *				quaternions or hypercomplex numbers. Neither one of these have
 *				all the properties expected of a number system (or field). 
 *				Quaternion multiplication is not commutative (a*b != b*a) and
 *				hypercomplex do not always have an inverse (given a non-zero 
 *				hypercomplex number a, you cannot always find b such that
 *				a*b == 1. Quaternions are in general more useful than hypercomplex
 *				numbers (eg 3D graphics systems often use quaternions instead of
 *				matrices for rotations).
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CHyperComplex.h,v $
 *		Revision 1.4  2001/03/28 09:38:51  jesjones
 *		Added support for streaming.
 *		
 *		Revision 1.3  2001/03/03 11:15:20  jesjones
 *		Instead of specializing selected algorithms for POD types, we use the new MSL mechanism to declare POD types.
 *		
 *		Revision 1.2  2001/03/02 11:22:05  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.1  2000/12/28 02:49:45  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:55:13  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XNumbers.h>
#include <XTypes.h>

#include <X3DPrimitives.h>

#include "CComplex.h"


//-----------------------------------
//	Forward References
//
class CHyperComplex;
class CQuaternion;

namespace Whisper {
	class XInStream;
	class XOutStream;
}


//-----------------------------------
//	Constants
//
extern const CHyperComplex kZeroHyperComplex;


// ===================================================================================
//	struct SHyperComplex
//		Hypercomplex numbers are easily converted to and from pairs of regular complex
//		numbers. This seperability allows us to do the trickier operations (like trig
//		functions) in the complex domain and then convert back to the hypercomplex
//		domain.
// ===================================================================================
struct SHyperComplex {
	CComplex	a;
	CComplex	b;
	
		SHyperComplex(const CHyperComplex& rhs);
};


// ===================================================================================
//	class CHyperComplex
// ===================================================================================
class CHyperComplex {

//-----------------------------------
//	Initialization/Destruction
//
public:
							CHyperComplex() 								{}

							CHyperComplex(float inX, float inY, float inZ, float inW)	{x = inX; y = inY; z = inZ; w = inW;}

							CHyperComplex(const X3DPoint& pt, float inW)	{x = pt.x; y = pt.y; z = pt.z; w = inW;}

							CHyperComplex(const SHyperComplex& rhs);

	explicit				CHyperComplex(const CQuaternion& rhs);
						
//-----------------------------------
//	API
//
public:
	// ----- Assignment operators -----
			CHyperComplex& 	operator+=(const CHyperComplex& rhs)			{x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this;}
			CHyperComplex& 	operator-=(const CHyperComplex& rhs)			{x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this;}
			CHyperComplex& 	operator*=(const CHyperComplex& rhs);
			CHyperComplex& 	operator/=(const CHyperComplex& rhs);

			CHyperComplex& 	operator+=(double rhs)							{x = (float) (x + rhs); return *this;}
			CHyperComplex& 	operator-=(double rhs)							{x = (float) (x - rhs); return *this;}
			CHyperComplex& 	operator*=(double rhs)							{x = (float) (x * rhs); y = (float) (y * rhs); z = (float) (z * rhs); w = (float) (w * rhs); return *this;}
			CHyperComplex& 	operator/=(double rhs)							{x = (float) (x * rhs); y = (float) (y / rhs); z = (float) (z / rhs); w = (float) (w / rhs); return *this;}

	// ----- Unary minus -----
			CHyperComplex 	operator-() const								{return CHyperComplex((float) (-x), (float) (-y), (float) (-z), (float) (-w));}
	
	// ----- Arithmetic -----
			CHyperComplex 	operator+(const CHyperComplex& rhs) const		{return CHyperComplex(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);}			
			CHyperComplex 	operator-(const CHyperComplex& rhs) const		{return CHyperComplex(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);}			
			CHyperComplex 	operator*(const CHyperComplex& rhs) const		{CHyperComplex temp = *this; temp *= rhs; return temp;}
			CHyperComplex 	operator/(const CHyperComplex& rhs) const		{CHyperComplex temp = *this; temp /= rhs; return temp;}
	
			CHyperComplex 	operator+(double rhs) const						{return CHyperComplex((float) (x + rhs), y, z, w);}
			CHyperComplex 	operator-(double rhs) const						{return CHyperComplex((float) (x - rhs), y, z, w);}
			CHyperComplex 	operator*(double rhs) const						{return CHyperComplex((float) (x * rhs), (float) (y * rhs), (float) (z * rhs), (float) (w * rhs));}
			CHyperComplex 	operator/(double rhs) const						{return CHyperComplex((float) (x / rhs), (float) (y / rhs), (float) (z / rhs), (float) (w / rhs));}
	
	friend	CHyperComplex 	operator+(double lhs, const CHyperComplex& rhs)	{return CHyperComplex((float) (lhs + rhs.x), rhs.y, rhs.z, rhs.w);}
	friend	CHyperComplex 	operator-(double lhs, const CHyperComplex& rhs)	{return CHyperComplex((float) (lhs - rhs.x), rhs.y, rhs.z, rhs.w);}
	friend	CHyperComplex 	operator*(double lhs, const CHyperComplex& rhs)	{return CHyperComplex((float) (lhs * rhs.x), (float) (lhs * rhs.y), (float) (lhs * rhs.z), (float) (lhs * rhs.w));}
	
	// ----- Functions -----
	friend 	CHyperComplex 	cos(const CHyperComplex& z);
	friend 	CHyperComplex 	sin(const CHyperComplex& z);
	friend 	CHyperComplex 	tan(const CHyperComplex& z);

	friend 	CHyperComplex 	acos(const CHyperComplex& z);
	friend 	CHyperComplex 	asin(const CHyperComplex& z);
	friend 	CHyperComplex 	atan(const CHyperComplex& z);

	friend 	CHyperComplex 	cosh(const CHyperComplex& z);
	friend 	CHyperComplex 	sinh(const CHyperComplex& z);
	friend 	CHyperComplex 	tanh(const CHyperComplex& z);

	friend 	CHyperComplex 	acosh(const CHyperComplex& z);
	friend 	CHyperComplex 	asinh(const CHyperComplex& z);
	friend 	CHyperComplex 	atanh(const CHyperComplex& z);

	friend 	CHyperComplex 	exp(const CHyperComplex& z);
	friend 	CHyperComplex 	log(const CHyperComplex& z, int32 branch = 0);
	friend 	CHyperComplex 	log10(const CHyperComplex& z, int32 branch = 0);

	friend 	CHyperComplex 	powint(const CHyperComplex& z, int32 n);
	friend 	CHyperComplex 	sqr(const CHyperComplex& z);
	friend 	CHyperComplex 	sqrt(const CHyperComplex& z, int32 branch = 0);

	friend 	CHyperComplex 	inverse(const CHyperComplex& z);
							// Returns the reciprocal of z. Note that this can be infinite for finite z!

	friend 	double 			mag(const CHyperComplex& rhs)					{return sqrt((double) rhs.x*rhs.x + rhs.y*rhs.y + rhs.z*rhs.z + rhs.w*rhs.w);}
	friend 	double 			mag2(const CHyperComplex& rhs)					{return (double) rhs.x*rhs.x + rhs.y*rhs.y + rhs.z*rhs.z + rhs.w*rhs.w;}
							// Returns the square of the magnitude.
						
	// ----- Relational Operators -----
			bool 			operator==(const CHyperComplex& rhs) const		{return Equal(*this, rhs);}
			bool 			operator!=(const CHyperComplex& rhs) const		{return !Equal(*this, rhs);}

	friend	bool 			Equal(const CHyperComplex& lhs, const CHyperComplex& rhs, const double& tolerance = 1.0e-6f);

	// ----- Streaming -----
	friend 	void 		 FromStr(const std::wstring& str, CHyperComplex& value);
	friend 	std::wstring ToStr(const CHyperComplex& value);

	friend XInStream&  operator>>(XInStream& stream, CHyperComplex& value);
	friend XOutStream& operator<<(XOutStream& stream, const CHyperComplex& value);

//-----------------------------------
//	Member Data
//
public:
	float x, y, z, w;
};


DECLARE_POD(CHyperComplex)

