/*
 *  File:       CComplex.cpp
 *  Summary:   	Template complex number class. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-1998 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CComplex.cpp,v $
 *		Revision 1.1  2000/12/28 02:48:45  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:53:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 8/23/98	JDJ		Nuked xdouble.h include.
 *		 <3>	 1/30/98	JDJ		Fixed some minor syntax errors.
 *		 <2>	 9/26/97	JDJ		Added <T> to operator/.
 *		 <1>	 4/14/97	JDJ		Created
 */

#include "AppHeader.h"
#include "CComplex.h"

#include <cmath>

#include <XNumbers.h>


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// tolong
//
//---------------------------------------------------------------
inline int32 tolong(float x)				{return (int32) x;}
	
inline int32 tolong(double x)				{return (int32) x;}
		

//---------------------------------------------------------------
//
// sqr
//
//---------------------------------------------------------------
inline float  sqr(float x)					{return x*x;}
	
inline double sqr(double x)					{return x*x;}
	
	
//---------------------------------------------------------------
//
// powint
//
//---------------------------------------------------------------
inline float   powint(float x, int32 n)		{return (float) std::pow(x, (int) n);}
	
inline double  powint(double x, int32 n)	{return std::pow(x, (int) n);}
		

//---------------------------------------------------------------
//
// times2
//
//---------------------------------------------------------------
inline float   times2(float x)				{return (float) std::scalbn(x, 1);}
	
inline double  times2(double x)				{return std::scalbn(x, 1);}
		

//---------------------------------------------------------------
//
// half
//
//---------------------------------------------------------------
inline float   half(float x)				{return (float) std::scalbn(x, -1);}
	
inline double  half(double x)				{return std::scalbn(x, -1);}
		
#pragma mark -

// ===================================================================================
//	class CComplex
// ===================================================================================

//---------------------------------------------------------------
//
// CComplex::operator*
//
//---------------------------------------------------------------
CComplex CComplex::operator*(const CComplex& rhs) const
{
	CComplex result;
	
	if (rhs.i == 0.0) {
		result.r = r*rhs.r;
		result.i = i*rhs.r;		
		
	} else {
		result.r = r*rhs.r - i*rhs.i;
		result.i = r*rhs.i + i*rhs.r;
	}
	
	return result;
}


//---------------------------------------------------------------
//
// CComplex::operator/
//
//---------------------------------------------------------------
CComplex CComplex::operator/(const CComplex& rhs) const
{
	CComplex result;
	double ratio, denom;
	
	if (rhs.i == 0.0) {
		result.r = r/rhs.r;
		result.i = i/rhs.r;
	
	} else if (Abs(rhs.r) >= Abs(rhs.i)) {
		ratio = rhs.i/rhs.r; 
		denom = rhs.r + ratio*rhs.i; 
		result.r = (r + i*ratio)/denom; 
		result.i = (i - r*ratio)/denom; 
	
	} else {
		ratio = rhs.r/rhs.i; 
		denom = rhs.i + ratio*rhs.r; 
		result.r = (r*ratio + i)/denom; 
		result.i = (i*ratio - r)/denom;
	}

	return result;
}


//---------------------------------------------------------------
//
// Equal (CComplex, CComplex, double)
//
//---------------------------------------------------------------
bool Equal(const CComplex& lhs, const CComplex& rhs, float tolerance)
{
	bool equal = Whisper::Equal(lhs.r, rhs.r, tolerance) && Whisper::Equal(lhs.i, rhs.i, tolerance);
	
	return equal;
}


//---------------------------------------------------------------
//
// CComplex::theta
//
//---------------------------------------------------------------
double theta(const CComplex& z)
{
	double theta;
	
	if (z.i == 0.0) {
		if (z.r >= 0.0)
			theta = 0.0;				// positive real 
		else
			theta = kPi;				// negative real 
		
	} else if (z.r == 0.0) {
		if (z.i >= 0.0)
			theta = kHalfPi;			// positive imag 
		else
			theta = -kHalfPi;			// negative imag 
		
	} else {
		theta = atan(z.i/z.r);
		if (z.r < 0.0) {				// make sure theta is in the correct quadrant 
			if (z.i > 0.0)
				theta = kPi + theta;
			else
				theta = theta - kPi;
		}
	}
	
	return theta;
}


//---------------------------------------------------------------
//
// CComplex::mag2
//
//---------------------------------------------------------------
double mag2(const CComplex& z)
{
	double mag;
	
	if (z.i == 0.0) 
		mag = z.r*z.r;
	else
		mag = z.r*z.r + z.i*z.i;				
	
	return mag;
}


//---------------------------------------------------------------
//
// CComplex::sqr
//
//---------------------------------------------------------------
CComplex sqr(const CComplex& z)
{
	CComplex result;
	
	double temp = z.r*z.i;
	
	result.r = z.r*z.r - z.i*z.i;
	result.i = temp + temp;
	
	return result;
}


//---------------------------------------------------------------
//
// CComplex::sqrt
//
//---------------------------------------------------------------
CComplex sqrt(const CComplex& z, int32 branch)
{
	CComplex result;
	
	if (z.i == 0.0 && z.r > 0.0) {
		if (branch)
			result.r = -sqrt(z.r);
		else
			result.r = sqrt(z.r);

		result.i = 0.0;
		
	} else {
		double r = mag(z);
		
		result.r = half(r + z.r);
		result.i = half(r - z.r);
		result.r = sqrt(result.r);
		result.i = Sign(z.i)*sqrt(result.i);
	}
	
	return result;
}


//---------------------------------------------------------------
//
// CComplex::exp
//
// exp(z) = e^x*[cos(y) + i*sin(y)]
//
//---------------------------------------------------------------
CComplex exp(const CComplex& z)
{
	CComplex x;

	if (z.i == 0.0) {
		x.r = exp(z.r);
		x.i = 0.0;

	} else {
		double a = exp(z.r);
		x.r = a*cos(z.i);
		x.i = a*sin(z.i);
	}
	
	return x;
}


//---------------------------------------------------------------
//
// CComplex::log
//
// ln(z) = ln|z| + i(theta + 2*pi*branch)    
//
//---------------------------------------------------------------
CComplex log(const CComplex& z, int32 branch)
{
	CComplex x;

	if (z.i == 0.0 && z.r >= 0.0) {
		x.r = log(z.r);
		x.i = 0.0;
		
	} else {
		x.r = log(mag(z));
		x.i = theta(z) + times2(kPi*branch);
	}
	
	return x;
}


//---------------------------------------------------------------
//
// CComplex::log10
//
//---------------------------------------------------------------
CComplex log10(const CComplex& z, int32 branch)
{
	CComplex x;

	double ln10 = log(10.0);
	x = log(z, branch);
	x.r = x.r/ln10;
	x.i = x.i/ln10;
	
	return x;
}


//---------------------------------------------------------------
//
// CComplex::pow
//
// 
// z^c = exp(c*ln(z))
//
// Because ln(z) is a many valued function z^c does not, in general,
// have a unique value. In particular if we let j and k be any integer
// then if:
// 		c = k 
// 			there is one power
// 		c = 1/k 
// 			there are k distinct powers
// 		c = j/k, with j and k having no common factors
// 			there are k distinct powers
// If c is not an integer or a rational number there are an infinite 
// number of powers. This can result in some unexpected answers with 
// roots of negative reals. For example -8^(1/3) will come out as 
// (1+1.732i) the second value (n = 1) yields the expected result: -2.
//
//---------------------------------------------------------------
CComplex pow(const CComplex& z, const CComplex& c, int32 branch)
{
	CComplex x;

	if (z.r == 0.0 && z.i == 0.0) {
		if (c.r == 0.0 && c.i == 0.0) {
			x.r = NAN;
			x.i = NAN;
			
		} else {
			x.r = 0.0;
			x.i = 0.0;
		}
		
	} else if (c.r == 0.0 && c.i == 0.0) {
		x.r = 1.0;
		x.i = 0.0;
		
	} else if (c.r == 1.0 && c.i == 0.0) {
		x = z;
		
	} else if (c.i == 0.0) {
		if (rint(c.r) == c.r)
			x = powint(z, tolong(c.r));
			
		else {
			double r = pow(mag(z), c.r);
			double t = c.r*theta(z);

			x.r = r*cos(t);
			x.i = r*sin(t);
		}
		
	} else
		x = exp(c*log(z, branch));
	
	return x;
}


//---------------------------------------------------------------
//
// CComplex::powint
//
// z^n = r^n*[cos(n*theta) + i*sin(n*theta)]
//
//---------------------------------------------------------------
CComplex powint(const CComplex& z, int32 n)
{
	CComplex x;

	if (z.i == 0.0) {
		if (z.r == 0.0) {
			if (n == 0) {
				x.r = NAN;
				x.i = NAN;
				
			} else {
				x.r = 0.0;
				x.i = 0.0;
			}
			
		} else if (z.r == 1.0) {
			x.r = 1.0;
			x.i = 0.0;
			
		} else {
			x.r = powint(z.r, n);
			x.i = 0.0;
		}
		
	} else if (n == 0) {
		x.r = 1.0;
		x.i = 0.0;
		
	} else if (n == 1) {
		x = z;
		
	} else {
		double t = n*theta(z);
		double r = powint(mag(z), n);
		
		x.r = r*cos(t);
		x.i = r*sin(t);
	}
	
	return x;
}


//---------------------------------------------------------------
//
// CComplex::cos
//
//---------------------------------------------------------------
CComplex cos(const CComplex& z)
{
	CComplex x;

	if (z.i == 0.0) {
		x.r = cos(z.r);
		x.i = 0.0;

	} else {
		x.r = cos(z.r)*cosh(z.i);
		x.i = -sin(z.r)*sinh(z.i);
	}
	
	return x;
}


//---------------------------------------------------------------
//
// CComplex::sin
//
//---------------------------------------------------------------
CComplex  sin(const CComplex& z)
{
	CComplex x;

	if (z.i == 0.0) {
		x.r = sin(z.r);
		x.i = 0.0;

	} else {
		x.r = sin(z.r)*cosh(z.i);
		x.i = cos(z.r)*sinh(z.i);
	}
	
	return x;
}


//---------------------------------------------------------------
//
// CComplex::tan
//
//---------------------------------------------------------------
CComplex tan(const CComplex& z)
{
	CComplex x;

	if (z.i == 0.0) {
		x.r = tan(z.r);
		x.i = 0.0;

	} else
		x = sin(z)/cos(z);
	
	return x;
}


//---------------------------------------------------------------
//
// CComplex::acos
//
// acos(z) = -i*ln[z + i*sqrt(1 - z*z)]
//
//---------------------------------------------------------------
CComplex acos(const CComplex& z)
{
	CComplex x, y;

	if (z.i == 0.0) {
		x.r = acos(z.r);
		x.i = 0.0;

	} else {
		y = sqr(z);
		y.r = 1.0 - y.r;
		y.i = -y.i;
		y = sqrt(y, 0);
		x.r = -y.i;
		x.i = y.r;
		y = log(z + x, 0);
		x.r = y.i;
		x.i = -y.r;
	}
	
	return y;
}


//---------------------------------------------------------------
//
// CComplex::asin
//
// asin(z) = -i*ln[i*z + sqrt(1 - z*z)]
//
//---------------------------------------------------------------
CComplex asin(const CComplex& z)
{
	CComplex x, y;

	if (z.i == 0.0) {
		x.r = asin(z.r);
		x.i = 0.0;

	} else {
		x.r = -z.i;
		x.i = z.r;
		y = sqr(z);
		y.r = 1.0 - y.r;
		y.i = -y.i;
		y = sqrt(y, 0);
		x = log(x + y, 0);
		y.r = x.i;
		y.i = -x.r;
	}
	
	return y;
}


//---------------------------------------------------------------
//
// CComplex::atan
//
// atan(z) = i*ln[(i + z)/(i - z)]/2
//
//---------------------------------------------------------------
CComplex atan(const CComplex& z)
{
	CComplex x, y;

	if (z.i == 0.0) {
		x.r = atan(z.r);
		x.i = 0.0;

	} else {
		x.r = z.r;
		x.i = 1.0 + z.i;
		y.r = -z.r;
		y.i = 1.0 - z.i;
		x = log(x/y, 0);
		y.r = -half(x.i);		
		y.i = half(x.r);
	}
	
	return y;
}


//---------------------------------------------------------------
//
// CComplex::cosh
//
//---------------------------------------------------------------
CComplex cosh(const CComplex& z)
{
	CComplex x;

	if (z.i == 0.0) {
		x.r = cosh(z.r);
		x.i = 0.0;

	} else {
		x.r = cosh(z.r)*cos(z.i);
		x.i = sinh(z.r)*sin(z.i);
	}
	
	return x;
}


//---------------------------------------------------------------
//
// CComplex::sinh
//
//---------------------------------------------------------------
CComplex sinh(const CComplex& z)
{
	CComplex x;

	if (z.i == 0.0) {
		x.r = sinh(z.r);
		x.i = 0.0;

	} else {
		x.r = sinh(z.r)*cos(z.i);
		x.i = cosh(z.r)*sin(z.i);
	}
	
	return x;
}


//---------------------------------------------------------------
//
// CComplex::tanh
//
//---------------------------------------------------------------
CComplex tanh(const CComplex& z)
{
	CComplex x;

	if (z.i == 0.0) {
		x.r = tanh(z.r);
		x.i = 0.0;

	} else
		x = sinh(z)/cosh(z);
	
	return x;
}


//---------------------------------------------------------------
//
// CComplex::acosh
//
// acosh(z) = ln[z + sqrt(z*z - 1)]
//
//---------------------------------------------------------------
CComplex acosh(const CComplex& z)
{
	CComplex x;

	if (z.i == 0.0) {
		x.r = acosh(z.r);
		x.i = 0.0;

	} else {
		x = sqr(z);
		x.r = x.r - 1.0;
		x = log(z + sqrt(x, 0), 0);
	}
	
	return x;
}


//---------------------------------------------------------------
//
// CComplex::asinh
//
// asinh(z) = ln[z + sqrt(z*z + 1)]
//
//---------------------------------------------------------------
CComplex asinh(const CComplex& z)
{
	CComplex x;

	if (z.i == 0.0) {
		x.r = asinh(z.r);
		x.i = 0.0;

	} else {
		x = sqr(z);
		x.r = 1.0 + x.r;
		x = log(z + sqrt(x, 0), 0);
	}
	
	return x;
}


//---------------------------------------------------------------
//
// CComplex::atanh
//
// atanh(z) = ln[(1 + z)/(1 - z)]/2
//
//---------------------------------------------------------------
CComplex atanh(const CComplex& z)
{
	CComplex x, y;

	if (z.i == 0.0) {
		x.r = atanh(z.r);
		x.i = 0.0;

	} else {
		x.r = 1.0 + z.r;
		x.i = z.i;
		y.r = 1.0 - z.r;
		y.i = -z.i;
		x = log(x/y, 0);
		x.r = half(x.r);
		x.i = half(x.i);
	}
	
	return y;
}

