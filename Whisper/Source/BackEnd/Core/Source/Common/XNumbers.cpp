/*
 *  File:       XNumbers.cpp
 *  Summary:   	Number related functions.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XNumbers.cpp,v $
 *		Revision 1.4  2001/05/06 10:44:00  jesjones
 *		Changed the floating point Equal function to use an algorithm recommended by Knuth. This also slightly simplifies the API.
 *		
 *		Revision 1.3  2001/04/27 04:34:50  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:44:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <8>	 2/04/00	JDJ		Instead of making Random a template function a few
 *									overloads are used. Random uses the Mersenne Twister
 *									instead of the code from Knuth.
 *		 <7>	 7/31/99	JDJ		kPi is now computed using acos(-1.0).
 *		 <6>	 7/15/99	JDJ		Made double constants extern.
 *		 <5>	 6/11/99	JDJ		Tweaked for CW Pro 5fc3.
 *		 <4>	12/11/98	JDJ		Added PlotUniform and PlotNormal.
 *		 <3>	12/11/98	JDJ		GaussianRandom() now returns number on real number
 *									line and allows variance to be specified.
 *		 <2>	12/09/98	JDJ		Added GaussianRandom (double, double)
 *		 <1>	11/16/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XNumbers.h>

#include <cmath>

#if WIN
	#include <mmsystem.h>
#endif

void sgenrand(unsigned long seed);	
void seedMT(uint32 seed);
void sgenrandInt(unsigned long seed);

namespace Whisper {


//-----------------------------------
//	Constants
//
const double kPi     = std::acos(-1.0);				// rather than hard-coding this we'll attempt to use the same value the math library uses	
const double kHalfPi = 0.5*kPi;
const double kTwoPi  = 2.0*kPi;

const double kDegreesToRadians	= kTwoPi/360.0;
const double kRadiansToDegrees	= 360.0/kTwoPi;


//-----------------------------------
//	Static Globals
//
static int32 DoSetRandomSeed(uint32 inSeed)
{		
	uint32 seed = inSeed == 0 ? 1 : inSeed;

	sgenrand(seed);
	seedMT(seed);
	
	return (int32) seed;
}

#if MAC
	int32 sFlipSeed = DoSetRandomSeed((uint32) TickCount());		
#elif WIN
	int32 sFlipSeed = DoSetRandomSeed((uint32) timeGetTime());		
#endif


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// FlipCoin
//
// The algorithm is from the second edition of _Numerical Recipies 
// in C_ by Press, Teukolsky, Vetterling, and Flannery and is based
// on "primitive polynomials modulo 2". The polynomial used here
// is x**18 + x**5 + x**2 + x + 1. This is guaranteed to produce 
// all possible sequences of 18 bits before repeating.
//
// Numerical Recipies has a table with larger polynomials that
// will yield a larger period but they'll also yield (occasionally)
// longer runs returning the same result.
//
//---------------------------------------------------------------
bool FlipCoin()
{
    const uint32 kBit1  = 1;
    const uint32 kBit2  = 2;
    const uint32 kBit5  = 16;
    const uint32 kBit18 = 131072;
    
    const uint32 kMask = kBit1 + kBit2 + kBit5;
    
    ASSERT(sFlipSeed != 0);
    
    if (sFlipSeed & kBit18) {
    	sFlipSeed = (int32) (((sFlipSeed ^ kMask) << 1) | kBit1);
    	return true;
    	
    } else {
    	sFlipSeed <<= 1;
    	if (sFlipSeed == 0)			
    		sFlipSeed = 1;
    	return false;
    }
}


//---------------------------------------------------------------
//
// Equal
//
// Comparing floating point numbers for equality is tricky because
// the limited precision of the hardware introduces small errors
// so that two numbers that should compare equal don't. So what 
// we do is consider the numbers equal if their difference is less 
// than some epsilon value. But choosing this epsilon is also tricky 
// because if the numbers are large epsilon should also be large, 
// and if the numbers are very small the epsilon must be even smaller. 
// To get around this we'll use a technique from Knuth's "Seminumerical
// Algorithms" section 4.2.2 and scale epsilon by the exponent of 
// one of the specified numbers. 
//
//---------------------------------------------------------------
bool Equal(double x, double y, double epsilon)
{
	PRECONDITION(epsilon >= 0.0);
	
	// Get the exponent of the smaller of the two numbers (using the
	// smaller of the two gives us a tighter epsilon value).
	int exponent;
	(void) std::frexp(std::fabs(x) < std::fabs(y) ? x : y, &exponent);	

	// Scale epsilon by the exponent.
	double delta = std::ldexp(epsilon, exponent); 

	// If the difference between the numbers is smaller than the
	// scaled epsilon we'll consider the numbers to be equal.
	double difference = std::fabs(x - y);
	bool equal = difference <= delta;

	return equal;
}


//---------------------------------------------------------------
//
// SetRandomSeed
//
//---------------------------------------------------------------
void SetRandomSeed(int32 seed)
{
	sFlipSeed = seed == 0 ? 1 : seed;
		
	sgenrand((uint32) seed);
	seedMT((uint32) seed);
}


//---------------------------------------------------------------
//
// GaussianRandom (double)
//
// From _Numerical Recipies in C_, 2cnd ed. Note that this isn't
// thread safe, but I think the worst that can happen is that
// the same number can be returned twice to two different threads.
//
//---------------------------------------------------------------
double GaussianRandom(double variance)
{
	PRECONDITION(variance > 0.0);
	
	static bool computed = false;		
	static double nextValue = 0.0;
	
	double value;
	int32 count = 0;
	
	if (computed) {
		computed = false;
		value = nextValue;
	
	} else {
		double rsq, v1, v2;
		do {
			v1 = 2.0*Random(1.0) - 1.0;
			v2 = 2.0*Random(1.0) - 1.0;
			rsq = v1*v1 + v2*v2;
		} while (rsq >= 1.0 || rsq == 0.0);
		
		double fac = std::sqrt(-2.0*std::log(rsq)/rsq);
		nextValue = v1*fac*variance;
		computed = true;
		
		value = v2*fac*variance;
	}
		
	return value;
}


//---------------------------------------------------------------
//
// GaussianRandom (double, double, double, double)
//
//---------------------------------------------------------------
double GaussianRandom(double min, double max, double variance, double sigma)				
{
	PRECONDITION(sigma >= 1.0);		// sigma needs to be fairly large so that we can quickly find a value
	PRECONDITION(min < max);
	
	sigma *= variance;
	
	uint32 index;
	double x = 0.0;
	for (index = 0; index < 100; ++index) {		// for a sigma of 2.0 this should very rarely iterate more than once, for a sigma of 1.0 it should iterate on average 3 times
		double y = GaussianRandom(variance);
		if (y >= -sigma && y <= sigma) {
			x = y;
			break;
		}
	}
	
	double range = max - min;
	
	range *= 0.5;											
	
	double value = min + range + range*x/sigma;
	POSTCONDITION(value >= min);
	POSTCONDITION(value <= max);
	
	return value;
}


}		// namespace Whisper


