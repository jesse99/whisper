/*
 *  File:       CQuaternion.h
 *  Summary:   	Quaternion number class. 
 *  Written by: Jesse Jones
 *
 *	Abstract:	Complex numbers can be generalized into four dimensions using
 *				quaternions or hypercomplex numbers. Neither one of these have
 *				all the properties expected of a number system (or field). 
 *				Quaternion multiplication is not commutative (a*b != b*a) and
 *				hypercomplex do not always have an inverse (given a non-zero 
 *				hypercomplex number a, you cannot always find b such that
 *				a*b == 1. Quaternions are in general more useful than
 *				hypercomplex numbers (eg 3D graphics systems often use quaternions
 *				instead of matrices for rotations).
 *
 *  Copyright © 1998 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CQuaternion.h,v $
 *		Revision 1.2  2001/03/03 11:15:24  jesjones
 *		Instead of specializing selected algorithms for POD types, we use the new MSL mechanism to declare POD types.
 *		
 *		Revision 1.1  2000/12/28 02:50:44  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:57:07  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include "CHyperComplex.h"


//-----------------------------------
//	Constants
//
extern const CQuaternion kZeroQuaternion;


// ===================================================================================
//	class CQuaternion
// ===================================================================================
class CQuaternion {

//-----------------------------------
//	Initialization/Destruction
//
public:
							CQuaternion() 									{}

							CQuaternion(float inX, float inY, float inZ, float inW)	{x = inX; y = inY; z = inZ; w = inW;}

							CQuaternion(const X3DPoint& pt, float inW)		{x = pt.x; y = pt.y; z = pt.z; w = inW;}

	explicit				CQuaternion(const CHyperComplex& rhs)			{x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w;}
						
//-----------------------------------
//	API
//
public:
	// ----- Assignment operators -----
			CQuaternion& 	operator+=(const CQuaternion& rhs)				{x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this;}
			CQuaternion& 	operator-=(const CQuaternion& rhs)				{x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this;}
			CQuaternion& 	operator*=(const CQuaternion& rhs);
			CQuaternion& 	operator/=(const CQuaternion& rhs);

			CQuaternion& 	operator+=(double rhs)							{x = (float) (x + rhs); return *this;}
			CQuaternion& 	operator-=(double rhs)							{x = (float) (x - rhs); return *this;}
			CQuaternion& 	operator*=(double rhs)							{x = (float) (x * rhs); y = (float) (y * rhs); z = (float) (z * rhs); w = (float) (w * rhs); return *this;}
			CQuaternion& 	operator/=(double rhs)							{x = (float) (x * rhs); y = (float) (y / rhs); z = (float) (z / rhs); w = (float) (w / rhs); return *this;}
							// Note that multiplication is not commutative.
							
	// ----- Unary minus -----
			CQuaternion 	operator-() const								{return CQuaternion((float) (-x), (float) (-y), (float) (-z), (float) (-w));}
	
	// ----- Arithmetic -----
			CQuaternion 	operator+(const CQuaternion& rhs) const			{return CQuaternion(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);}			
			CQuaternion 	operator-(const CQuaternion& rhs) const			{return CQuaternion(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);}			
			CQuaternion 	operator*(const CQuaternion& rhs) const			{CQuaternion temp = *this; temp *= rhs; return temp;}
			CQuaternion 	operator/(const CQuaternion& rhs) const			{CQuaternion temp = *this; temp /= rhs; return temp;}
	
			CQuaternion 	operator+(double rhs) const						{return CQuaternion((float) (x + rhs), y, z, w);}
			CQuaternion 	operator-(double rhs) const						{return CQuaternion((float) (x - rhs), y, z, w);}
			CQuaternion 	operator*(double rhs) const						{return CQuaternion((float) (x * rhs), (float) (y * rhs), (float) (z * rhs), (float) (w * rhs));}
			CQuaternion 	operator/(double rhs) const						{return CQuaternion((float) (x / rhs), (float) (y / rhs), (float) (z / rhs), (float) (w / rhs));}
	
	friend	CQuaternion 	operator+(double lhs, const CQuaternion& rhs)	{return CQuaternion((float) (lhs + rhs.x), rhs.y, rhs.z, rhs.w);}
	friend	CQuaternion 	operator-(double lhs, const CQuaternion& rhs)	{return CQuaternion((float) (lhs - rhs.x), rhs.y, rhs.z, rhs.w);}
	friend	CQuaternion 	operator*(double lhs, const CQuaternion& rhs)	{return CQuaternion((float) (lhs * rhs.x), (float) (lhs * rhs.y), (float) (lhs * rhs.z), (float) (lhs * rhs.w));}
	
	// ----- Functions -----
	friend 	CQuaternion 	sqr(const CQuaternion& z);

	friend 	CQuaternion 	inverse(const CQuaternion& z);
							// Returns the reciprocal of z. 

	friend 	double 			mag(const CQuaternion& rhs)						{return sqrt((double) rhs.x*rhs.x + rhs.y*rhs.y + rhs.z*rhs.z + rhs.w*rhs.w);}
	friend 	double 			mag2(const CQuaternion& rhs)					{return (double) rhs.x*rhs.x + rhs.y*rhs.y + rhs.z*rhs.z + rhs.w*rhs.w;}
							// Returns the square of the magnitude.
						
	// ----- Relational Operators -----
			bool 			operator==(const CQuaternion& rhs) const		{return Equal(*this, rhs);}
			bool 			operator!=(const CQuaternion& rhs) const		{return !Equal(*this, rhs);}

	friend	bool 			Equal(const CQuaternion& lhs, const CQuaternion& rhs, const double& tolerance = 1.0e-6f);

//-----------------------------------
//	Member Data
//
public:
	float x, y, z, w;
};


DECLARE_POD(CQuaternion)

