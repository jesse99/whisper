/*
 *  File:       CComplex.h
 *  Summary:   	Template complex number class. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-1998 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CComplex.h,v $
 *		Revision 1.2  2001/03/03 11:15:16  jesjones
 *		Instead of specializing selected algorithms for POD types, we use the new MSL mechanism to declare POD types.
 *		
 *		Revision 1.1  2000/12/28 02:48:49  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:53:40  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <6>	 5/18/98	JDJ		Declared Equal outside the class (Pro 3 linker bug).
 *		 <5>	 6/20/97	JDJ		Mag() uses math libraries hypot function.
 *		 <4>	 4/20/97	JDJ		Added element_type typedef.
 *		 <3>	 4/19/97	JDJ		Added scalar operators. Added implicit conversion from T.
 *		 <2>	 4/15/97	JDJ		Added STL specializations.
 *		 <1>	 4/14/97	JDJ		Created
 */

#pragma once

#include <XNumbers.h>
#include <XTypes.h>

using namespace Whisper;
using namespace std;


//-----------------------------------
//	Library Functions
//
#if __dest_os == __mac_os && __MC68K__
	extern "C" double_t hypot(double_t x, double_t y);		// returns sqrt(x*x + y*y)
#else	
	extern "C" double hypot(double x, double y);
#endif


// ===================================================================================
//	class CComplex
//		We use this instead of the standard's complex class because the standard class
//		encapsulates the components which makes it rather painful to use.
// ===================================================================================
class CComplex {

//-----------------------------------
//	Initialization/Destruction
//
public:
						CComplex() 										{}

						CComplex(double real, double imag) : r(real), i(imag)	{}
						
//-----------------------------------
//	API
//
public:
	// ----- Assignment operators -----
			CComplex& 	operator+=(const CComplex& rhs)					{r += rhs.r; i += rhs.i; return *this;}
			CComplex& 	operator-=(const CComplex& rhs)					{r -= rhs.r; i -= rhs.i; return *this;}
			CComplex& 	operator*=(const CComplex& rhs)					{*this = *this*rhs; return *this;}
			CComplex& 	operator/=(const CComplex& rhs)					{*this = *this/rhs; return *this;}

			CComplex& 	operator+=(double rhs)						{r += rhs; return *this;}
			CComplex& 	operator-=(double rhs)						{r -= rhs; return *this;}
			CComplex& 	operator*=(double rhs)						{r *= rhs; i *= rhs; return *this;}
			CComplex& 	operator/=(double rhs)						{r /= rhs; i /= rhs; return *this;}

	// ----- Unary minus -----
			CComplex 	operator-() const								{return CComplex(-r, -i);}
	
	// ----- Arithmetic -----
			CComplex 	operator+(const CComplex& rhs) const			{return CComplex(r + rhs.r, i + rhs.i);}			
			CComplex 	operator-(const CComplex& rhs) const			{return CComplex(r - rhs.r, i - rhs.i);}			
			CComplex 	operator*(const CComplex& rhs) const;			
			CComplex 	operator/(const CComplex& rhs) const;
	
			CComplex 	operator+(double rhs) const					{return CComplex(r + rhs, i);}
			CComplex 	operator-(double rhs) const					{return CComplex(r - rhs, i);}
			CComplex 	operator*(double rhs) const					{return CComplex(r*rhs, i*rhs);}
			CComplex 	operator/(double rhs) const					{return CComplex(r/rhs, i/rhs);}
	
	friend	CComplex 	operator+(double lhs, const CComplex& rhs);
	friend	CComplex 	operator-(double lhs, const CComplex& rhs);
	friend	CComplex 	operator*(double lhs, const CComplex& rhs);
	
	// ----- Functions -----
	friend 	CComplex 	cos(const CComplex& z);
	friend 	CComplex 	sin(const CComplex& z);
	friend 	CComplex 	tan(const CComplex& z);

	friend 	CComplex 	acos(const CComplex& z);
	friend 	CComplex 	asin(const CComplex& z);
	friend 	CComplex 	atan(const CComplex& z);

	friend 	CComplex 	cosh(const CComplex& z);
	friend 	CComplex 	sinh(const CComplex& z);
	friend 	CComplex 	tanh(const CComplex& z);

	friend 	CComplex 	acosh(const CComplex& z);
	friend 	CComplex 	asinh(const CComplex& z);
	friend 	CComplex 	atanh(const CComplex& z);

	friend 	CComplex 	exp(const CComplex& z);
	friend 	CComplex 	log(const CComplex& z, int32 branch = 0);
	friend 	CComplex 	log10(const CComplex& z, int32 branch = 0);

	friend 	CComplex 	pow(const CComplex& z, const CComplex& c, int32 branch = 0);
	friend 	CComplex 	powint(const CComplex& z, int32 n);
	friend 	CComplex 	sqr(const CComplex& z);
	friend 	CComplex 	sqrt(const CComplex& z, int32 branch = 0);

	friend 	double 		theta(const CComplex& z);

	friend 	double		mag(const CComplex& z);
	friend 	double 		mag2(const CComplex& z);
						// Returns the square of the magnitude.
						
	// ----- Relational Operators -----
//	friend	bool 		Equal(const CComplex& lhs, const CComplex& rhs, float tolerance = 1.0e-6f);	
						// ��� Declared below due to Pro 3 linking problems...
						
			bool 		operator==(const CComplex& rhs) const;// 				{return Equal(*this, rhs);}
			bool 		operator!=(const CComplex& rhs) const;//				{return !this->operator==(rhs);}

//-----------------------------------
//	Member Data
//
public:
	double	r;
	double	i;
};


bool Equal(const CComplex& lhs, const CComplex& rhs, float tolerance);	

inline bool CComplex::operator==(const CComplex& rhs) const 			{return Equal(*this, rhs, 1.0e-6f);}
inline bool CComplex::operator!=(const CComplex& rhs) const				{return !this->operator==(rhs);}

// ===================================================================================
//	Friend Functions
// ===================================================================================
inline CComplex operator+(double lhs, const CComplex& rhs)	
{
	return CComplex(lhs + rhs.r, rhs.i);
}

inline CComplex operator-(double lhs, const CComplex& rhs)	
{
	return CComplex(lhs - rhs.r, rhs.i);
}

inline CComplex operator*(double lhs, const CComplex& rhs)	
{
	return CComplex(lhs*rhs.r, rhs.i*lhs);
}

inline double mag(const CComplex& z)							
{
	return std::hypot(z.r, z.i);
}


DECLARE_POD(CComplex)