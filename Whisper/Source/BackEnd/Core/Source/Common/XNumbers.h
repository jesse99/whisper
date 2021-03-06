/*
 *  File:       XNumbers.h
 *  Summary:   	Number related functions.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XNumbers.h,v $
 *		Revision 1.5  2001/05/06 10:44:15  jesjones
 *		Changed the floating point Equal function to use an algorithm recommended by Knuth. This also slightly simplifies the API.
 *		
 *		Revision 1.4  2001/03/30 04:15:08  jesjones
 *		Rewrote numeric_cast so casting a double to a float no longer can assert and 64-bit integer types are treated correctly.
 *		
 *		Revision 1.3  2000/12/10 04:10:43  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:44:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <limits>

#include <XDebug.h>
#include <XTypes.h>

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif

CORE_EXPORT uint32 	randomMT();		// Mersenne Twister exports
CORE_EXPORT double 	genrand();
CORE_EXPORT uint32 	genrandInt();

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

		
namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


#if MSVC >= 1100
	#define isnan		_isnan
	#define isfinite	_finite
	#define scalb		_scalb
	#define NAN			sqrt(-4.0)
//	#define NAN			std::numeric_limits<double>::quiet_NaN()
	#define INFINITY	HUGE_VAL
#endif


// ===================================================================================
//	Constants
// ===================================================================================
extern CORE_EXPORT const double kPi;
extern CORE_EXPORT const double kHalfPi;
extern CORE_EXPORT const double kTwoPi;

extern CORE_EXPORT const double kDegreesToRadians;
extern CORE_EXPORT const double kRadiansToDegrees;


// ===================================================================================
//	Min and Max
// ===================================================================================
template <class T> 
inline T		Min(const T& x, const T& y)							{return x < y ? x : y;}

template <class T> 
inline T		Max(const T& x, const T& y)							{return x > y ? x : y;}

template <class T> 
inline T		MinMax(const T& lo, const T& x, const T& hi)		{return Min(Max(x, lo), hi);}
				// Returns x pinned to lo and high.


#if 1			// $$ MSVC 5.0 get's confused when these are enabled
template <class T> 
inline T		Min(const T& x, const T& y, const T& z)				{return Min(Min(x, y), z);}
				// Versions taking more than two arguments.

template <class T> 
inline T		Min(const T& x, const T& y, const T& z, const T& w)	{return Min(Min(Min(x, y), z), w);}

template <class T> 
inline T		Max(const T& x, const T& y, const T& z)				{return Max(Max(x, y), z);}

template <class T> 
inline T		Max(const T& x, const T& y, const T& z, const T& w)	{return Max(Max(Max(x, y), z), w);}
#endif


// ===================================================================================
//	Sign
// ===================================================================================
template <class T> 
inline int32	Sign(T value)										{return value > 0 ? 1 : ((value < 0) ? -1 : 0);}
				// Returns -1, 0, or +1.
	

// ===================================================================================
//	Random
// ===================================================================================
inline int32 	Random(int32 max)									{ASSERT(max >= 0); return (int32) (randomMT() % max);} 
inline uint32 	Random(uint32 max)									{return randomMT() % max;}
inline float 	Random(float max)									{return (float) (max*genrand());}
inline double 	Random(double max)									{return max*genrand();}
				// Returns a random number in [0, max). Note that std:: rand is 3.8x
				// faster than Random(int32) and 1.3x faster than Random(double) (if
				// scalb is used when converting std::rand's result to a double).

inline int32 	Random(int32 min, int32 max)						{ASSERT(max >= min); return min + Random(max - min);}
inline uint32 	Random(uint32 min, uint32 max)						{ASSERT(max >= min); return min + Random(max - min);}
inline float 	Random(float min, float max)						{ASSERT(max >= min); return min + Random(max - min);}
inline double 	Random(double min, double max)						{ASSERT(max >= min); return min + Random(max - min);}
				// Returns a random number in [min, max).
	
CORE_EXPORT double GaussianRandom(double variance = 1.0);
				// Returns a random number with a normal distribution centered on 0.0.
				// Note that this may return any real number (although it will almost
				// always return a number in [-3*variance.0, 3.0*variance]). Larger
				// variances result in a shorter and wider probability curve.

CORE_EXPORT double GaussianRandom(double min, double max, double variance = 1.0, double sigma = 2.0);
				// Returns a random number in [min, max]. Sigma controls how much of the
				// probability curve is used. The default uses 97% of the curve. Note that
				// sigma will be scaled for non-unitary variances.

CORE_EXPORT bool FlipCoin();
				// Very fast heads or tails random number generator.
			
CORE_EXPORT void SetRandomSeed(int32 seed);
				// Note that the seed is inited to a psuedo-random value.


// ===================================================================================
//	Numeric Cast
// ===================================================================================
template <typename DST, typename SRC>	// can't use specializations with functions so we use a struct
struct SCheckCast {						// use doubles to avoid signed <--> unsigned problems
	inline void Check(SRC n)									{ASSERT((double) n >= std::numeric_limits<DST>::min()); ASSERT((double) n <= std::numeric_limits<DST>::max());}
};

template <typename SRC>					// can't go wrong converting to floating point
struct SCheckCast<float, SRC> {
	inline void Check(SRC)										{}
};

template <typename SRC>					
struct SCheckCast<double, SRC> {
	inline void Check(SRC)										{}
};

template <typename SRC>					// need to special case 64-bit types because they have more significant digits than double			
struct SCheckCast<int64, SRC> {
	inline void Check(SRC n)									{ASSERT(n >= std::numeric_limits<int64>::min()); ASSERT(n <= std::numeric_limits<int64>::max());}
};

template <typename SRC>					
struct SCheckCast<uint64, SRC> {
	inline void Check(SRC n)									{ASSERT(n >= 0); ASSERT(n <= std::numeric_limits<uint64>::max());}
};

template <>					
struct SCheckCast<int64, uint64> {
	inline void Check(uint64 n)									{ASSERT(n <= std::numeric_limits<int64>::max());}
};

template <>					
struct SCheckCast<uint64, int64> {
	inline void Check(int64 n)									{ASSERT(n >= 0);}
};

template <typename DST, typename SRC>
inline DST 		numeric_cast(SRC n)								{SCheckCast<DST, SRC>().Check(n); return static_cast<DST>(n);}
				

// ===================================================================================
//	Misc
// ===================================================================================
template <class T> 
inline bool 	InBounds(T lo, T value, T hi)					{return lo <= value && value <= hi;}
				// Returns true if lo <= value <= hi.

inline int16 	MulDiv(int16 value, int16 num, int16 denom)		{return (int16) ((int32) value*num/denom);}
inline uint16 	MulDiv(uint16 value, uint16 num, uint16 denom)	{return (uint16) ((uint32) value*num/denom);}
inline int32 	MulDiv(int32 value, int32 num, int32 denom)		{return (int32) ((int64) value*num/denom);}
inline uint32 	MulDiv(uint32 value, uint32 num, uint32 denom)	{return (uint32) ((uint64) value*num/denom);}
				// Returns value*num/denom. Uses longs to avoid overflows.	
	
template <class T> 
inline T		Abs(T value)									{return value < 0 ? -value : value;}

CORE_EXPORT bool Equal(double x, double y, double epsilon = 1.0e-6);
				// Returns true if the difference between the numbers is less
				// than epsilon. Note that epsilon is scaled by the exponent
				// of the smaller of the two numbers.


// ===================================================================================
//	Intrinsics
// ===================================================================================
#if __POWERPC__
	// Use faster intrinsic functions on PowerPC's
	template<> inline int16	 Abs(int16 value)			{return (int16) __abs(value);}
	template<> inline int32	 Abs(int32 value)			{return (int32) __abs(value);}
	template<> inline float	 Abs(float value)			{return (float) __fabs(value);}
	template<> inline double Abs(double value)			{return __fabs(value);}

#elif MSVC >= 1100
	// MSVC optimizer automatically uses intrinsics for abs and fabs
	template<> inline int16	 Abs(int16 value)			{return (int16) abs(value);}
	template<> inline int32	 Abs(int32 value)			{return (int32) abs(value);}
	template<> inline float	 Abs(float value)			{return (float) fabs(value);}
	template<> inline double Abs(double value)			{return (double) fabs(value);}
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

