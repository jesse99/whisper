/*
 *  File:       XNumbers.h
 *  Summary:   	Number related functions.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: KnuthRandom.h,v $
 *		Revision 1.2  2000/11/09 09:08:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <4>	 7/15/99	JDJ		Made double constants extern.
 *		 <3>	12/11/98	JDJ		GaussianRandom() now returns number on real number
 *									line and allows variance to be specified.
 *		 <2>	12/09/98	JDJ		Added GaussianRandom (double, double)
 *		 <1>	11/16/97	JDJ		Created (from Raven)
 */

#pragma once

#include <limits>

#include <XDebug.h>
#include <XTypes.h>

namespace Whisper {


#if MSVC >= 1100
	#define isnan		_isnan
	#define isfinite	_finite
	#define scalb		_scalb
	#define NAN			sqrt(-4.0)
//	#define NAN			std::numeric_limits<double>::quiet_NaN()
	#define INFINITY	HUGE_VAL
#endif


// ===================================================================================
//	Random
// ===================================================================================
template <class T> 
T				Random(T max);
				// Returns a random number in [0, max).

template <class T> 
T				Random(T min, T max);
				// Returns a random number in [min, max).
	
EXPORT double 	GaussianRandom(double variance = 1.0);
				// Returns a random number with a normal distribution centered on 0.0.
				// Note that this may return any real number (although it will almost
				// always return a number in [-3*variance.0, 3.0*variance]). Larger
				// variances result in a shorter and wider probability curve.

EXPORT double 	GaussianRandom(double min, double max, double variance = 1.0, double sigma = 2.0);
				// Returns a random number in [min, max]. Sigma controls how much of the
				// probability curve is used. The default uses 97% of the curve. Note that
				// sigma is scaled for non-unitary variances.

EXPORT bool 	FlipCoin();
				// Very fast heads or tails random number generator.
			
EXPORT void 	SetRandomSeed(int32 seed);
				// Note that the seed is inited to a psuedo-random value.


// ===================================================================================
//	Inlines
// ===================================================================================
EXPORT int32 	RandomInt32(int32 max);
EXPORT int32 	RandomInt32(int32 min, int32 max);

template <class T> 
inline T		Random(T max)							{return (T) RandomInt32((int32) max);}

template <class T> 
inline T		Random(T min, T max)					{return (T) RandomInt32((int32) min, (int32) max);}

template<> double Random(double max);
template<> inline double Random(double min, double max)	{return min + Random(max - min);}

template<> float Random(float max);
template<> inline float Random(float min, float max)	{return min + Random(max - min);}



}	// namespace Whisper

