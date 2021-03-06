/*
 *  File:       CHyperComplex.cpp
 *  Summary:   	Hyper complex number class. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CHyperComplex.cpp,v $
 *		Revision 1.3  2001/03/28 09:38:47  jesjones
 *		Added support for streaming.
 *		
 *		Revision 1.2  2001/03/02 11:22:01  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.1  2000/12/28 02:49:41  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:55:08  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include "CHyperComplex.h"

#include <XNumbers.h>
#include <XStreaming.h>
#include <XTextConversions.h>

#include "CQuaternion.h"


//-----------------------------------
//	Constants
//
const CHyperComplex kZeroHyperComplex(0.0f, 0.0f, 0.0f, 0.0f);

inline double sqr(double x) 	{return x*x;}


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// operator>> (CHyperComplex)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, CHyperComplex& value)
{
	stream 	>> value.x		
			>> value.y		
			>> value.z
			>> value.w;
		
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (CHyperComplex)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const CHyperComplex& value)
{
	stream 	<< value.x		
			<< value.y		
			<< value.z
			<< value.w;
				
	return stream;
}


//---------------------------------------------------------------
//
// FromStr (wstring, CHyperComplex)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& inStr, CHyperComplex& value)
{
	Whisper::FromStr(inStr, value.x);
	
	std::wstring str = After(inStr, L" ");
	Whisper::FromStr(str, value.y);
	
	str = After(str, L" ");
	Whisper::FromStr(str, value.z);
	
	str = After(str, L" ");
	Whisper::FromStr(str, value.w);
}


//---------------------------------------------------------------
//
// ToStr (CHyperComplex)
//
//---------------------------------------------------------------
std::wstring ToStr(const CHyperComplex& value)
{
	return Whisper::ToStr(value.x) + L" " + Whisper::ToStr(value.y) + L" " + Whisper::ToStr(value.z) + L" " + Whisper::ToStr(value.w);
}

// ===================================================================================
//	struct SHyperComplex
// ===================================================================================

//---------------------------------------------------------------
//
// SHyperComplex::SHyperComplex
//
//---------------------------------------------------------------
SHyperComplex::SHyperComplex(const CHyperComplex& rhs)
{
	a.r = rhs.x - rhs.w;
	a.i = rhs.y + rhs.z;
	
	b.r = rhs.x + rhs.w;
	b.i = rhs.y - rhs.z;
}

#pragma mark -

// ===================================================================================
//	class CHyperComplex
// ===================================================================================

//---------------------------------------------------------------
//
// CHyperComplex::CHyperComplex (SHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex::CHyperComplex(const SHyperComplex& rhs)
{
	x = (float) (0.5*(rhs.a.r + rhs.b.r));
	y = (float) (0.5*(rhs.a.i + rhs.b.i));
	z = (float) (0.5*(rhs.a.i - rhs.b.i));
	w = (float) (0.5*(rhs.b.r - rhs.a.r));
}


//---------------------------------------------------------------
//
// CHyperComplex::CHyperComplex (CQuaternion)
//
//---------------------------------------------------------------
CHyperComplex::CHyperComplex(const CQuaternion& rhs)
{
	x = (float) rhs.x; 
	y = (float) rhs.y; 
	z = (float) rhs.z; 
	w = (float) rhs.w;
}


//---------------------------------------------------------------
//
// CHyperComplex::operator*=
//
//---------------------------------------------------------------
CHyperComplex& CHyperComplex::operator*=(const CHyperComplex& rhs)
{	
	double xx = (double) x*rhs.x - y*rhs.y - z*rhs.z + w*rhs.w;
	double yy = (double) y*rhs.x + x*rhs.y - w*rhs.z - z*rhs.w;
	double zz = (double) z*rhs.x - w*rhs.y + x*rhs.z - y*rhs.w;
	double ww = (double) w*rhs.x + z*rhs.y + y*rhs.z + x*rhs.w;

	x = (float) xx;
	y = (float) yy;
	z = (float) zz;
	w = (float) ww;
	
	return *this;
}


//---------------------------------------------------------------
//
// CHyperComplex::operator/=
//
//---------------------------------------------------------------
CHyperComplex& CHyperComplex::operator/=(const CHyperComplex& rhs)
{
	CHyperComplex a = inverse(rhs);
	
	return this->operator*=(a);
}

#pragma mark ~

//---------------------------------------------------------------
//
// Equal (CHyperComplex, CHyperComplex, double)
//
//---------------------------------------------------------------
bool Equal(const CHyperComplex& lhs, const CHyperComplex& rhs, const double& tolerance)
{
	bool equal = false;
	
	if (Equal(lhs.x, rhs.x, tolerance))
		if (Equal(lhs.y, rhs.y, tolerance))
			if (Equal(lhs.z, rhs.z, tolerance))
				if (Equal(lhs.w, rhs.w, tolerance))
					equal = true;
					
	return equal;
}


//---------------------------------------------------------------
//
// sqr (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex sqr(const CHyperComplex& rhs)
{
	CHyperComplex result;
	
	double xx = (double) rhs.x*rhs.x - rhs.y*rhs.y - rhs.z*rhs.z + rhs.w*rhs.w;
	double yy = (double) rhs.x*rhs.y - rhs.z*rhs.w;
	double zz = (double) rhs.x*rhs.z - rhs.y*rhs.w;
	double ww = (double) rhs.x*rhs.w + rhs.y*rhs.z;

	result.x = (float) xx;
	result.y = (float) (yy + yy);
	result.z = (float) (zz + zz);
	result.w = (float) (ww + ww);
		
	return result;
}


//---------------------------------------------------------------
//
// inverse (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex inverse(const CHyperComplex& rhs)
{
	CHyperComplex result;

	double det = (sqr(rhs.x - rhs.w) + sqr(rhs.y + rhs.z))*(sqr(rhs.x + rhs.w) + sqr(rhs.y - rhs.z));
	
	double xx = rhs.x*rhs.x;
	double yy = rhs.y*rhs.y;
	double zz = rhs.z*rhs.z;
	double ww = rhs.w*rhs.w;
	
	double t = 2.0*(rhs.x*rhs.w - rhs.y*rhs.z);

	result.x = (float) (( rhs.x*(xx + yy + zz + ww) - rhs.w*t)/det);
	result.y = (float) ((-rhs.y*(xx + yy + zz + ww) - rhs.z*t)/det);
	result.z = (float) ((-rhs.z*(xx + yy + zz + ww) - rhs.y*t)/det);
	result.w = (float) (( rhs.w*(xx + yy + zz + ww) - rhs.x*t)/det);
	
	return result;
}


//---------------------------------------------------------------
//
// sqrt (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex sqrt(const CHyperComplex& z, int32 branch)
{
	SHyperComplex result(z);
	
	result.a = sqrt(result.a, branch);
	result.b = sqrt(result.b, branch);

	return result;
}


//---------------------------------------------------------------
//
// exp (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex exp(const CHyperComplex& z)
{
	SHyperComplex result(z);
	
	result.a = exp(result.a);
	result.b = exp(result.b);

	return result;
}


//---------------------------------------------------------------
//
// log (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex log(const CHyperComplex& z, int32 branch)
{
	SHyperComplex result(z);
	
	result.a = log(result.a, branch);
	result.b = log(result.b, branch);

	return result;
}


//---------------------------------------------------------------
//
// log10 (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex log10(const CHyperComplex& z, int32 branch)
{
	SHyperComplex result(z);
	
	result.a = log10(result.a, branch);
	result.b = log10(result.b, branch);

	return result;
}


//---------------------------------------------------------------
//
// powint (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex powint(const CHyperComplex& z, int32 n)
{
	SHyperComplex result(z);
	
	result.a = powint(result.a, n);
	result.b = powint(result.b, n);

	return result;
}


//---------------------------------------------------------------
//
// cos (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex cos(const CHyperComplex& z)
{
	SHyperComplex result(z);
	
	result.a = cos(result.a);
	result.b = cos(result.b);

	return result;
}


//---------------------------------------------------------------
//
// sin (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex sin(const CHyperComplex& z)
{
	SHyperComplex result(z);
	
	result.a = sin(result.a);
	result.b = sin(result.b);

	return result;
}


//---------------------------------------------------------------
//
// tan (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex tan(const CHyperComplex& z)
{
	SHyperComplex result(z);
	
	result.a = tan(result.a);
	result.b = tan(result.b);

	return result;
}


//---------------------------------------------------------------
//
// acos (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex acos(const CHyperComplex& z)
{
	SHyperComplex result(z);
	
	result.a = acos(result.a);
	result.b = acos(result.b);

	return result;
}


//---------------------------------------------------------------
//
// asin (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex asin(const CHyperComplex& z)
{
	SHyperComplex result(z);
	
	result.a = asin(result.a);
	result.b = asin(result.b);

	return result;
}


//---------------------------------------------------------------
//
// atan (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex atan(const CHyperComplex& z)
{
	SHyperComplex result(z);
	
	result.a = atan(result.a);
	result.b = atan(result.b);

	return result;
}


//---------------------------------------------------------------
//
// cosh (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex cosh(const CHyperComplex& z)
{
	SHyperComplex result(z);
	
	result.a = cosh(result.a);
	result.b = cosh(result.b);

	return result;
}


//---------------------------------------------------------------
//
// sinh (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex sinh(const CHyperComplex& z)
{
	SHyperComplex result(z);
	
	result.a = sinh(result.a);
	result.b = sinh(result.b);

	return result;
}


//---------------------------------------------------------------
//
// tanh (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex tanh(const CHyperComplex& z)
{
	SHyperComplex result(z);
	
	result.a = tanh(result.a);
	result.b = tanh(result.b);

	return result;
}


//---------------------------------------------------------------
//
// acosh (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex acosh(const CHyperComplex& z)
{
	SHyperComplex result(z);
	
	result.a = acosh(result.a);
	result.b = acosh(result.b);

	return result;
}


//---------------------------------------------------------------
//
// asinh (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex asinh(const CHyperComplex& z)
{
	SHyperComplex result(z);
	
	result.a = asinh(result.a);
	result.b = asinh(result.b);

	return result;
}


//---------------------------------------------------------------
//
// atanh (CHyperComplex)
//
//---------------------------------------------------------------
CHyperComplex atanh(const CHyperComplex& z)
{
	SHyperComplex result(z);
	
	result.a = atanh(result.a);
	result.b = atanh(result.b);

	return result;
}



