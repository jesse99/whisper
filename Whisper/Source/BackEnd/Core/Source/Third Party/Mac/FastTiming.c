// This is by David Lawrence and was posted to the Mac Games mailing list.

#include "FastTiming.h"

#if 0
enum {
	false						= 0,
	true						= 1
};
#endif

#include <Types.h>
#include <ConditionalMacros.h>
#include <CodeFragments.h>
#include <DriverServices.h>
#include <Gestalt.h>
#include <Timer.h>

#if !TARGET_CARBON

// #define's for PowerPC registers used in assembly
#define RTCU	4
#define RTCL	5
#define MQ		0


enum
{
	eFastTimingMode_None			= 0,
	
	eFastTimingMode_TickCount,
	eFastTimingMode_TimeManager,
	eFastTimingMode_Microseconds
#if GENERATINGPOWERPC
	,eFastTimingMode_601,
	eFastTimingMode_UpTime
#endif
};

static int		sgFastTimingMode = eFastTimingMode_None;



#pragma mark ---=== PowerPC 601 Timing ===---


//===========================================================================
// InitFastTiming_601
//===========================================================================

#if GENERATINGPOWERPC
static Boolean InitFastTiming_601( void )
{
	long					gestaltResult;
	OSErr					err;
	Boolean					initialized = false;
	
		// First, see if this PowerMac has a PowerPC 601.  If so, reading
		// the 601's Real Time Clock registers will be  the fastest method
		// getting elapsed time.
	
	err = Gestalt( gestaltNativeCPUtype, &gestaltResult );
	if ( err == noErr && gestaltResult == gestaltCPU601 )
	{
		initialized = true;
	}
	
	return initialized;
}
#endif


//===========================================================================
// GetMicroseconds_601
// Calculates the number of microseconds elapsed since an OS specified
// start time.  Returns the result as a 64 bit quantity.
// This function reads from the 601's MQ, RTCU and RTCL registers using the
// mfspr instruction.  These registers are not part of the PowerPC
// specification, and only exist on the 601.  Only call this function
// after you have determined that the computer has a 601 processor.
//===========================================================================

#if GENERATINGPOWERPC
static void asm GetMicroseconds_601( register unsigned long* /*r3_hiPtr*/, register unsigned long* /*r4_loPtr*/ )
{
	machine	601
	
	// Do as much as we can before reading the timing registers in order
	// to return the most accurate time.
	
	// r8 = (2^32 * 2^8) / 1000	= 1,099,511,627.78	= 0x4189374C (high by 0.00000002%)
	lis		r8,0x4189
	ori		r8,r8,0x374C
	
	// r9 = 1,000,000		= 0x000F4240
	lis		r9,0x000F
	ori		r9,r9,0x4240
	
	
	// Read the time from the 601's Real Time Clock registers: RTCU and RTCL.
	// RTCU contains the number of seconds that have elapsed since the
	//		time specified in software.
	// RTCL contains the number of nanoseconds that have elapsed since
	//		the begining of the current second in RTCU.
loop:
	mfspr	r5,RTCU	// Get the Real Time Clock Upper (RTCU) register (seconds).
	mfspr	r6,RTCL	// Get the Real Time Clock Lower (RTCL) register (nanoseconds).
	mfspr	r7,RTCU	// Get the Real Time Clock Upper (RTCU) register (seconds).
	cmpw	r5,r7	// Make sure that RTCU didn't get incremented before RTCL was read.
	bne		loop	// If RTCU has been incremented, read RTCU and RTCL again.
	
	
	// Convert the time from (seconds + nanoseconds) into
	// (microseconds-high + microseconds-low).
	
	// Multiply seconds * 1,000,000 to get microseconds.  Store low
	// part of product in r11, and high part in r10.  On 601, the mul
	// instruction returns the low part of the product, and stores the
	// high part in the MQ register, so we don't need to do a separate
	// mulhw instruction.
	
	mul		r5,r5,r9	// r5 *= 1,000,000 (high-word of product)
	mfspr	r7,MQ		// r7 = (seconds * 1,000,000) (low-word of product) (from MQ register)
	
	
	// Divide nanoseconds by 1000 to get microseconds.  Instead of using the div
	// instruction to divide (x / 1000) (which takes 36 cycles), we will multiply
	// x * ((2^40) / 1000) (which takes 5 or 9 cycles).  The high word of the
	// product will contain a close approximation to (x / 1000) * (2^8).  We will
	// then right-shift the high word by 8 to get a close approximation to microseconds.
	// Since nanoseconds will sometimes be less than 16 bits, we will make nanoseconds
	// be the second argument to mulhw so the multilply will sometimes take 5 cycles
	// instead of 9.  (We use (2^40) because it gives gives us the largest possible
	// ((2^n) / 1000) that will fit in a signed 32 bit value, to minimize rounding errors).
	
	mulhwu	r6,r8,r6		// r6 *= ((2^40) / 1000) (high-word of product)
	rlwinm	r6,r6,24,8,31	//  r6 /= (2^8)		srwi r6,r6,8
	
	// Add the result to the low-word of the microseconds result calculated
	// from seconds.  Add the carry bit (if any) to the high-word of the
	// microseconds result calculated from seconds.
	
	addc	r7,r7,r6		// Add to the low-word of previously calculated microseconds.
	addze	r5,r5			// If there is a carry, add 1 to the high-word of microseconds.
	
	
	// Now return the result
	
	stw		r5,0(r3)	// Store high-word of microseconds.
	stw		r7,0(r4)	// Store low-word of microseconds.

	blr
}
#endif


#pragma mark ---=== PowerPC UpTime() Timing ===---


#if GENERATINGPOWERPC
	typedef void (* GetTimeBaseInfoProcPtr)(
					UInt32 *minAbsoluteTimeDelta,
					UInt32 *theAbsoluteTimeToNanosecondNumerator,
					UInt32 *theAbsoluteTimeToNanosecondDenominator,
					UInt32 *theProcessorToAbsoluteTimeNumerator,
					UInt32 *theProcessorToAbsoluteTimeDenominator);
	
	typedef AbsoluteTime (* UpTimeFunctionPtr)(void);
	typedef Nanoseconds (* AbsoluteToNanosecondsPtr) (AbsoluteTime absoluteTime);

	static UpTimeFunctionPtr	sgUpTimePtr = nil;
	static CFragConnectionID	sgDSLConnection = 0;
	
	static UInt32				sgToMicrosecondsDenominatorMultiplier;
	static UInt32				sgToMicrosecondsNumerator;
#endif


//===========================================================================
// ReduceFraction
// A utility function to reduce a fraction. This may not reduce all
// fractions completely.  Used to make PowerPC multiplies faster.
//  Multiply instructions can be faster if the second arguement has 16 or
// fewer significant bits.
//===========================================================================

#if GENERATINGPOWERPC
static void ReduceFraction( UInt32 * ioNumerator, UInt32 * ioDenominator )
{
	UInt32			primeTable[] = { 2, 3, 5, 7, 11, 13, 17, 23, 29, 31 };
	UInt32			primeItr;
	UInt32			numerator;
	UInt32			denominator;
	
		// Work with local copies (in registers) for speed.
	
	numerator = *ioNumerator;
	denominator = *ioDenominator;
	
	for ( primeItr = 0; primeItr < sizeof(primeTable) / sizeof(primeTable[0]); primeItr++ )
	{
		UInt32			thisPrime;
		
		thisPrime = primeTable[ primeItr ];
		while ( ( (numerator % thisPrime) == 0) && ((denominator % thisPrime) == 0 ) )
		{
			numerator /= thisPrime;
			denominator /= thisPrime;
		}
	}
	
		// Copy the local copies back out.
	
	*ioNumerator = numerator;
	*ioDenominator = denominator;
}
#endif


//===========================================================================
// InitFastTiming_UpTime
//===========================================================================

#if GENERATINGPOWERPC
static Boolean InitFastTiming_UpTime( void )
{
	Str255					errMsg;
	Ptr						mainAddress;
	Ptr						upTimeAddress;
	Ptr						getTimeBaseInfoAddress;
	CFragSymbolClass		symClass;
	Boolean					initialized = false;
	OSErr					err;
	
		// Try loading the Driver Services Library
	
	err = GetSharedLibrary(	"\pDriverServicesLib",
							kPowerPCCFragArch,
							kLoadCFrag,
							&sgDSLConnection,
							&mainAddress,
							errMsg	);
	
		// Try finding the UpTime symbol.
	
	if ( err == noErr )
	{
		err = FindSymbol(	sgDSLConnection,
							"\pUpTime",
							&upTimeAddress,
							&symClass	);
	}
	
		// Try finding the GetTimeBaseInfo symbol.
	
	if ( err == noErr )
	{
		err = FindSymbol(	sgDSLConnection,
							"\pGetTimeBaseInfo",
							&getTimeBaseInfoAddress,
							&symClass	);
	}
	
		// If the Driver Services Library was loaded, and the UpTime and GetTimeBaseInfo
		// function symbols were located, we will use UpTime() to measure elapsed time.
	
	if ( err == noErr )
	{
		GetTimeBaseInfoProcPtr	getTimeBaseInfoProc;
		UInt32					minAbsoluteTimeDelta;
		UInt32					theAbsoluteTimeToNanosecondNumerator;
		UInt32					theAbsoluteTimeToNanosecondDenominator;
		UInt32					theProcessorToAbsoluteTimeNumerator;
		UInt32					theProcessorToAbsoluteTimeDenominator;
		const UInt32			kTwoToTheThirtyOne = 2147483648;
		
		getTimeBaseInfoProc = (GetTimeBaseInfoProcPtr) getTimeBaseInfoAddress;
		
		(*getTimeBaseInfoProc)(	&minAbsoluteTimeDelta,
								&theAbsoluteTimeToNanosecondNumerator,
								&theAbsoluteTimeToNanosecondDenominator,
								&theProcessorToAbsoluteTimeNumerator,
								&theProcessorToAbsoluteTimeDenominator	);
		
			// Make sure that multiplying theAbsoluteTimeToNanosecondDenominator by 1000
			// won't overflow the 32-bit value.
		
		if ( theAbsoluteTimeToNanosecondDenominator <= 0xFFFFFFFF / 1000 )
		{
				// Multiply the denominator by 1000 so the fraction converts AbsoluteTime to
				// microseconds instead of nanoseconds.
			
			theAbsoluteTimeToNanosecondDenominator *= 1000;
			
				// Reduce the fraction.  Multiply instructions can be faster if the second arguement
				// has 16 or fewer significant bits.
			
			ReduceFraction( &theAbsoluteTimeToNanosecondNumerator, &theAbsoluteTimeToNanosecondDenominator );
			
				// For the two to the thirty one trick to work we need the denominator to be
				// a multiple of 2.  We insure that here.
			
			if ( (theAbsoluteTimeToNanosecondDenominator & 1) != 0 )
			{
				theAbsoluteTimeToNanosecondNumerator *= 2;
				theAbsoluteTimeToNanosecondDenominator *= 2;
			}
			
			sgToMicrosecondsDenominatorMultiplier = kTwoToTheThirtyOne / (theAbsoluteTimeToNanosecondDenominator / 2);
			sgToMicrosecondsNumerator = theAbsoluteTimeToNanosecondNumerator;
			
			sgUpTimePtr = (UpTimeFunctionPtr) upTimeAddress;
			
			initialized = true;
		}
		else
		{
			err = -1;
		}
	}
	
		// If we had a problem starting up, close the Driver Services Library.
	
	if ( err != noErr && sgDSLConnection != 0 )
	{
		err = CloseConnection(&sgDSLConnection);
		sgDSLConnection = 0;
	}
	
	return initialized;
}
#endif


#if GENERATINGPOWERPC
static asm void UpTimeToMicroseconds( UInt32 /*inHi*/, UInt32 /*inLo*/, UInt32 /*inNumerator*/, UInt32 /*inDenomMultiplier*/, UInt32* /*outHi*/, UInt32* /*outLo*/ )
{
	// r3 = inhi
	// r4 = inlo
	// r5 = inNumerator
	// r6 = inDenomMultiplier
	// r7 = outhi
	// r8 = outlo
	
		// We first multiply the 64-bit value (r3,r4) by the numerator (r5) to get a 96-bit result
		// (result-hi, result-med, result-lo) where:
		//		result-lo =		(value-lo * numerator)-lo
		//		result-med =	(value-hi * numerator)-lo + (value-lo * numerator)-hi
		//		result-hi =		(value-hi * numerator)-hi + (carry from result-med sum)
	
	
	// Arrange the multiplies in an order so that we can interleave the adds
	// after multiplies, where adds don't use the results of the previous multiply.
	
	mulhwu	r9,r4,r5		// 				(value-lo * numerator)-hi
	mullw	r10,r3,r5		//				(value-hi * numerator)-lo
	mulhwu	r3,r3,r5		// 				(value-hi * numerator)-hi
	addc	r9,r10,r9		// result-med =	(value-hi * numerator)-lo + (value-lo * numerator)-hi
	mullw	r4,r4,r5		// result-lo =	(value-lo * numerator)-lo
	addze	r3,r3			// result-hi =	(value-hi * numerator)-hi + (carry from result-med sum)
	
	// The 96-bit result (result-hi, result-med, result-lo) is now in (r3, r9, r4).
	// We will now multiply the 96-bit result by the denominator-multiplier (r6).
	// This final result in a 128-bit value (final-hi-hi, final-hi, final-lo, final-lo-lo)
	// but we ignore the highest 32 bits (final-hi-hi) and the lowest 32 bits (final-lo-lo),
	// so we don't need to calculate them.
	//		final-lo =		(result-lo * multiplier)-hi + (result-med * multiplier)-lo
	//		final-hi =		(result-med * multiplier)-hi + (result-hi * multiplier)-lo + (carry from final-lo sum)
	
	mullw	r10,r9,r6		//				(value-hi * numerator)-lo
	mulhwu	r4,r4,r6		// 				(result-lo * multiplier)-hi
	mulhwu	r9,r9,r6		//				(result-med * multiplier)-hi
	addc	r4,r10,r4		// final-lo =	(result-lo * multiplier)-hi + (result-med * multiplier)-lo
	addze	r9,r9			//				(result-med * multiplier)-hi + (carry from final-lo sum)
	mullw	r3,r3,r6		//				(result-hi * multiplier)-lo
	add		r3,r9,r3		// final-hi =	(result-med * multiplier)-hi + (result-hi * multiplier)-lo + (carry from final-lo sum)

	stw		r3,0(r7)		// store upper
	stw		r4,0(r8)		// store lower

	blr
}
#endif


#pragma mark ---=== Public API Functions ===---


//===========================================================================
// InitFastTiming
//===========================================================================

Boolean InitFastTiming( void )
{
		// Don't reinitialize if InitFastTiming() has already been called.
	
	if ( sgFastTimingMode == eFastTimingMode_None )
	{
#if GENERATINGPOWERPC
	#if 1
		if ( InitFastTiming_601() )
		{
			sgFastTimingMode = eFastTimingMode_601;
		}
		else
	#endif
		
	#if 1
		if ( InitFastTiming_UpTime() )
		{
			sgFastTimingMode = eFastTimingMode_UpTime;
		}
		else
	#endif
#endif
		
	#if 1
		// if ( HasMicroseconds() )	// Check for trap?  Always available since System 7 ???
		if ( true )
		{
			sgFastTimingMode = eFastTimingMode_Microseconds;
		}
	#endif
		
		// TODO - ??? Install a Time Manager task for 68K and non-PCI, non-601 PowerMacs ???
		
		// TODO - ??? Use TickCount as a last resort ???
	}
	
	return ( sgFastTimingMode != eFastTimingMode_None );
}


//===========================================================================
// ShutdownFastTiming
//===========================================================================

void ShutdownFastTiming( void )
{
		// Only shut down if FastTiming is initialized.
	
	if ( sgFastTimingMode != eFastTimingMode_None )
	{
			// Close the Driver Services Library if it was opened.
		
		if ( sgDSLConnection != 0 )
		{
			OSErr	err;
			
			err = CloseConnection(&sgDSLConnection);
			sgDSLConnection = 0;
		}
		
		sgUpTimePtr = nil;
		sgToMicrosecondsDenominatorMultiplier = 0;
		sgToMicrosecondsNumerator = 0;
		sgFastTimingMode = eFastTimingMode_None;
	}
}


//===========================================================================
// FastMicroseconds
//===========================================================================

void FastMicroseconds( register unsigned long * microseconds_hi, register unsigned long * microseconds_lo )
{
	int				fastTimingMode;
	
	fastTimingMode = sgFastTimingMode;
	
	switch ( fastTimingMode )
	{
		case eFastTimingMode_Microseconds:
			{
				UnsignedWide	microTickCount;
				
				Microseconds( &microTickCount );
				
				*microseconds_hi = microTickCount.hi;
				*microseconds_lo = microTickCount.lo;
			}
			break;
		
#if GENERATINGPOWERPC
		case eFastTimingMode_601:
			{
				GetMicroseconds_601( microseconds_hi, microseconds_lo );
			}
			break;
		
		case eFastTimingMode_UpTime:
			{
				AbsoluteTime	absoluteTime;
				
				absoluteTime = sgUpTimePtr();
				UpTimeToMicroseconds(	absoluteTime.hi, absoluteTime.lo,
										sgToMicrosecondsNumerator, sgToMicrosecondsDenominatorMultiplier,
										microseconds_hi, microseconds_lo );
			}
			break;
#endif
	}
}


#endif // !TARGET_CARBON
