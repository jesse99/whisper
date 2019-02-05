/*
 *  File:       XNumbers.cpp
 *  Summary:   	Number related functions.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: KnuthRandom.cpp,v $
 *		Revision 1.4  2001/04/21 02:25:26  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2000/12/10 04:02:20  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:08:20  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
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

#include <climits>
#include <math.h>
#include <vector>

#include <XDebug.h>
#include <XMiscUtils.h>
#include <XTypes.h>
#include <XUnitTest.h>

#if WIN
	#include <mmsystem.h>
#endif

namespace Whisper {

// ===================================================================================
//	Internal Functions
// 		The random number algorithm  is from D. E. Knuth,  _The Stanford GraphBase_, 
//		Addison-Wesley, ISBN 0-201-54275-7. Note that the generator is much better 
//		than a simple linear congruential algorithm, but about 13% slower.
// ===================================================================================

int32	gb_flip_cycle();
int32	gb_init_rand(int32 seed);
int32	gb_unif_rand(int32 m);
int		gb_flip();

const uint32 kTwo_to_the_31 = 0x80000000UL;

static int32 A[56] = {-1};


#if MAC
	int32 sFlipSeed = gb_init_rand((int32) TickCount());		
#elif WIN
	int32 sFlipSeed = gb_init_rand((int32) timeGetTime());		
#endif


int32 *gb_fptr = A;

#define gb_next_rand() (*gb_fptr >= 0 ? *gb_fptr-- : gb_flip_cycle())

#define mod_diff(x,y) (((x) - (y)) & 0x7FFFFFFF)


//---------------------------------------------------------------
//
// gb_flip_cycle
//
//---------------------------------------------------------------
int32 gb_flip_cycle()
{
    register int32 *ii, *jj;
    
    for (ii = &A[1], jj = &A[32]; jj <= &A[55]; ii++, jj++)
           *ii = mod_diff(*ii, *jj);
    for (jj = &A[1]; ii<= &A[55]; ii++, jj++)
           *ii = mod_diff(*ii, *jj);
    gb_fptr = &A[54];

    return A[55];
}


//---------------------------------------------------------------
//
// gb_init_rand
//
//---------------------------------------------------------------
int32 gb_init_rand(int32 seed)
{
    register int32 i;
    register int32 prev = seed, next = 1;
    
    seed = prev = mod_diff(prev, 0);
    A[55] = prev;
    for (i = 21; i; i = (i + 21) % 55) {
        A[i] = next;
        /* Compute a new next value. */
        next = mod_diff(prev, next);
        if (seed & 1)
        	seed = 0x40000000 + (seed >> 1);
        else
        	seed >>= 1;
        next = mod_diff(next, seed);

        prev = A[i];
    }
    /* Get the array values ``warmed up''. */
    (void) gb_flip_cycle();
    (void) gb_flip_cycle();
    (void) gb_flip_cycle();
    (void) gb_flip_cycle();
    (void) gb_flip_cycle();
    
    if (seed == 0)
    	seed = 1;
    
    return seed;									// Hack to enable gb_init_rand to be called before main.
}


//---------------------------------------------------------------
//
// gb_unif_rand
//
//---------------------------------------------------------------
int32 gb_unif_rand(int32 m)
{
	PRECONDITION(m > 0);			
	
    register int32 r;
	register uint32 t = kTwo_to_the_31 - (kTwo_to_the_31 % m);
	
	if (m > 1) {
		do {
			r = gb_next_rand();
		} while (t <= (uint32) r);
		r = r % m;
		
	} else
		r = 0;
	
	return r;
}


//---------------------------------------------------------------
//
// gb_flip
//
//---------------------------------------------------------------
int gb_flip() 
{
    int j;
    
    gb_init_rand(-314159L);
    if (gb_next_rand() != 119318998) {
    	DEBUGSTR("Failure on the first try.\n");
    	return -1;
    }
    for (j = 1; j <= 133; j++) 
    	gb_next_rand();
	if (gb_unif_rand(0x55555555L) != 748103812) {
    	DEBUGSTR("Failure on the second try.\n");
        return -2;
	}
    /* DEBUGSTR("OK, the gb_flip_routines seem to work."); */
    return 0;
}


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// SetRandomSeed
//
//---------------------------------------------------------------
void SetRandomSeed(int32 seed)
{
	sFlipSeed = gb_init_rand(seed);		
}


//---------------------------------------------------------------
//
// RandomInt32 (int32)
//
//---------------------------------------------------------------
int32 RandomInt32(int32 max)
{
	return (int32) gb_unif_rand(max); 						
}


//---------------------------------------------------------------
//
// RandomInt32 (int32, int32)
//
//---------------------------------------------------------------
int32 RandomInt32(int32 min, int32 max)
{
	int32 range = max - min;

	return min + gb_unif_rand(range);
}


//---------------------------------------------------------------
//
// Random (float)
//
//---------------------------------------------------------------
template<> float Random(float max)
{
	int32 n = gb_unif_rand(LONG_MAX); 

#if MAC || MSVC
	float x = (float) (max*scalb(n, -31));
#else
	float x = (float) (max*n*pow(2.0, -31));
#endif
	
	return x; 						
}


//---------------------------------------------------------------
//
// Random (double)
//
//---------------------------------------------------------------
template<> double Random(double max)
{
	int32 n = gb_unif_rand(LONG_MAX); 
	
#if MAC || MSVC
	double x = max*scalb(n, -31);
#else
	double x = max*n*pow(2.0, -31);
#endif
		
	return x; 						
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZNumbersTest
// ===================================================================================
#if DEBUG
class ZNumbersTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~ZNumbersTest();
	
						ZNumbersTest();
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnTest();

//-----------------------------------
//	Internal API
//
protected:
			void 		DoConstantTest();
			
			void 		DoRandomTest();
			
			void 		DoTestSpread(int16 max);
			bool 		DoPassesChiSqr(int16 max);
			void 		DoTestForCycles(int16 max);
			bool 		DoHasACycle(int16 values[], int16 numValues, int16 cycleLen, int16 startIndex, int16 max);

			void 		DoPlotUniform();
			void 		DoPlotNormal();

			int16 		DoGetRandom(int16 max);
			
			void 		DoSpeedTest();
			void 		DoFlushCache(const char* buffer, uint32 count);
			
private:
	uint32	mSum;
};

static ZNumbersTest sNumbersTest;

//---------------------------------------------------------------
//
// ZNumbersTest::~ZNumbersTest
//
//---------------------------------------------------------------
ZNumbersTest::~ZNumbersTest()
{
}

	
//---------------------------------------------------------------
//
// ZNumbersTest::ZNumbersTest
//
//---------------------------------------------------------------
ZNumbersTest::ZNumbersTest() : XUnitTest(L"Core", L"Numbers")
{
}
						

//---------------------------------------------------------------
//
// ZNumbersTest::OnTest
//
//---------------------------------------------------------------
void ZNumbersTest::OnTest()
{
	this->DoConstantTest();
	this->DoRandomTest();
	this->DoSpeedTest();
	
	TRACE("Completed numbers test.\n\n");
}

			
//---------------------------------------------------------------
//
// ZNumbersTest::DoConstantTest
//										
//---------------------------------------------------------------
void ZNumbersTest::DoConstantTest()
{
	const double kCRCPi = 3.14159265358979323846;	// per CRC handbook, 14th. ed.
	ASSERT(Abs(kPi - kCRCPi) < 1e-13);				// 64-bit IEEE numbers have 15-16 decimal digits of precision
	ASSERT(Equal(kPi, kCRCPi, 1e-13));
	
	ASSERT(Equal(kHalfPi, kCRCPi*0.5, 1e-13));
	ASSERT(Equal(kTwoPi, kCRCPi*2.0, 1e-13));
	
	ASSERT(Equal(180.0*kDegreesToRadians, kPi, 1e-13));
	ASSERT(Equal(kHalfPi*kRadiansToDegrees, 90.0, 1e-13));
}


//---------------------------------------------------------------
//
// ZNumbersTest::DoRandomTest
//
//---------------------------------------------------------------
void ZNumbersTest::DoRandomTest()
{
	while (gb_flip() < 0)	
		;
		
	this->DoTestSpread(2); 			
	this->DoTestSpread(10); 
	this->DoTestSpread(105);
	 
	this->DoTestForCycles(10); 
	this->DoTestForCycles(23); 
	this->DoTestForCycles(100); 
	
	this->DoPlotUniform();
	this->DoPlotNormal();
	
	// $$$ need to test FlipCoin as well
}


//---------------------------------------------------------------
//
// this->DoTestSpread
//
// Here we check to see if the values returned by the random
// number generator are reasonably spread out. We do this by
// using the chi-squared test. This can fail about once in ten
// times, so we'll write an error message if it fails more than
// twice.
//
//---------------------------------------------------------------
void ZNumbersTest::DoTestSpread(int16 max)
{
	int16 failCount = 0;
	
	for (int16 i = 0; i < 10; i++)
		if (!this->DoPassesChiSqr(max))
			failCount++;
			
	if (failCount > 2) {
		TRACE("Random(", max, ") failed the chi-squared test ", failCount, " times.\n");
		TRACE("Note that failing the test once is perfectly normal.\n\n");
	}
}


//---------------------------------------------------------------
//
// DoPassesChiSqr
// 
// See Algorithms by Sedgewick.
//
//---------------------------------------------------------------
bool ZNumbersTest::DoPassesChiSqr(int16 max)
{
	int32 i;
	
	const int32 kNoIterations = 10L*max;
	int32* count = new int32[max];
	
	for (i = 0; i < max; i++)
		count[i] = 0;
	
	for (i = 0; i < kNoIterations; i++) {
		int16 x = this->DoGetRandom(max);
		if (x >= 0 && x < max)
			count[x] += 1;
		else
			TRACE("Random(", max, ") returned ", x, ".\n");
	}
	
	int32 t = 0;
	for (i = 0; i < max; i++) 
		t += count[i]*count[i];
		
	int32 chiSqr = (max*t/kNoIterations) - kNoIterations;
	int32 toler = (int32) (2*sqrt((float) max));
	
#if 0
	TRACE("Chi-square for Random(", max, ") = ", chiSqr, "\n");
	TRACE("It should be within ", toler, " of ", max, "\n");
#endif
	
	delete [] count;
	
	return Abs(chiSqr - max) <= toler;
}


//---------------------------------------------------------------
//
// ZNumbersTest::DoTestForCycles
//
// A good random number generator should generate numbers that
// are independant of the numbers it cranked out before. In this
// test we check to see if the random number generator degenerates
// into cyclic behavior. For example, a bad random number generator
// might fall into a cycle of length 4 for Random(3): 011201120112.
//
// The code below checks for cycles of length max and up. (If the
// random number generator falls into a cycle with length smaller
// than max it will fail the frequency test above). 
//
//---------------------------------------------------------------
void ZNumbersTest::DoTestForCycles(int16 max)
{
	const int16 kNoIterations = 1000;
	int16 values[kNoIterations];
	
	for (int16 i = 0; i < kNoIterations; i++)
		values[i] = this->DoGetRandom(max);
		
	const int16 kMaxCycle = kNoIterations/3;
	for (int16 cycleLen = max; cycleLen < kMaxCycle; cycleLen++)
		for (int16 startIndex = 0; startIndex < kNoIterations - cycleLen; startIndex++)
			if (this->DoHasACycle(values, kNoIterations, cycleLen, startIndex, max))
				break;
}


//---------------------------------------------------------------
//
// ZNumbersTest::DoHasACycle	
//
// Writes a message if cycleLen values starting at startIndex
// form a cycle (ie the next set of cycleLen values is the same
// as the first). 
//
//---------------------------------------------------------------
bool ZNumbersTest::DoHasACycle(int16 values[], int16 numValues, int16 cycleLen, int16 startIndex, int16 max)
{
	int16 entry = 0;					// index into cycle
	int16 repeatCount = 0;				// number of times cycle has been repeated
	int16 index = numeric_cast<int16>(startIndex + cycleLen);// index into next value to be checked
	bool cycling = true;				// turns false when cycle is broken

	while (index < numValues && cycling) {
		if (values[startIndex + entry++] != values[index++])
			cycling = false;
		else if (entry == cycleLen) {
			entry = 0;
			repeatCount++;
		}
	}
	
	if (repeatCount > 0) {
		TRACE("A cycle of length ", cycleLen, " was repeated ", repeatCount);
		TRACE(" times with Random(", max, ").\n");
	}
	
	return repeatCount > 0;
}


//---------------------------------------------------------------
//
// ZNumbersTest::DoPlotUniform
//
//---------------------------------------------------------------
void ZNumbersTest::DoPlotUniform()
{
	std::vector<int32> buckets(32);
	
	const double kMinValue = -10;
	const double kMaxValue =  10;
	
	uint32 index;
	for (index = 0; index < 32; ++index)
		buckets[index] = 0;
		
	for (index = 0; index < 30000; ++index) {
		double value = Random(kMinValue, kMaxValue);
		ASSERT(value >= kMinValue);
		ASSERT(value <= kMaxValue);
		
		value -= kMinValue;
		value /= kMaxValue - kMinValue;
		value *= 32;
		
		uint32 i = (uint32) value;
		ASSERT(i >= 0);
		ASSERT(i < 32);
		
		buckets[i] += 1;
	} 
	
	double maxCount = 0.0;
	for (index = 0; index < 32; ++index)
		if (buckets[index] > maxCount)
			maxCount = buckets[index];

	TRACE("\nUniform Distribution:\n");
	for (index = 0; index < 32; ++index) {
		TRACE("%.2d: ", index); $$$
		int32 count = numeric_cast<int32>(30.0*buckets[index]/maxCount);
		for (int32 j = 0; j < count; ++j)
			TRACE("*");
		TRACE("\n");
	}
}


//---------------------------------------------------------------
//
// ZNumbersTest::DoPlotNormal
//
//---------------------------------------------------------------
void ZNumbersTest::DoPlotNormal()		
{
	std::vector<int32> buckets(32);
	
	const double kMinValue = -10;
	const double kMaxValue =  10;
	
	uint32 index;
	for (index = 0; index < 32; ++index)
		buckets[index] = 0;
		
	for (index = 0; index < 30000; ++index) {
		double value = GaussianRandom(kMinValue, kMaxValue, 1.0, 3.0);
		ASSERT(value >= kMinValue);
		ASSERT(value <= kMaxValue);
		
		value -= kMinValue;
		value /= kMaxValue - kMinValue;
		value *= 32;
		
		uint32 i = (uint32) value;
		ASSERT(i >= 0);
		ASSERT(i < 32);
		
		buckets[i] += 1;
	}
	
	double maxCount = 0.0;
	for (index = 0; index < 32; ++index)
		if (buckets[index] > maxCount)
			maxCount = buckets[index];

	TRACE("\nNormal Distribution:\n");
	for (index = 0; index < 32; ++index) {
		TRACE("%.2d: ", index); $$$
		int32 count = numeric_cast<int32>(30.0*buckets[index]/maxCount);
		for (int32 j = 0; j < count; ++j)
			TRACE("*");
		TRACE("\n");
	}
}


//---------------------------------------------------------------
//
// ZNumbersTest::DoGetRandom
//
// A function to help test the random number testers. It does this
// by providing some low quality random number generators that will
// hopefully do poorly on our tests.
//
//---------------------------------------------------------------
int16 ZNumbersTest::DoGetRandom(int16 max)
{
#if 1
	// Use the real random number generator.
	int16 x = Random(max);
	
#elif 0
	// Use a very bad random number generator.
	gRandomSeed = Abs(7*gRandomSeed) % 211;	
	int32 y = Abs(gRandomSeed) % 100;					// y is in [0, 99]				
	int16 x = numeric_cast<int16>(max*y/100);						

#elif 0
	// Return the next integer in the range.
	int16 x = numeric_cast<int16>(Abs(gRandomSeed++) % max);
#endif

	return x; 						
}


//---------------------------------------------------------------
//
// ZNumbersTest::DoSpeedTest
//
//---------------------------------------------------------------
void ZNumbersTest::DoSpeedTest()
{
	int32 n;
	double x;
	
	char* buffer = new char[64*1024L];

	// No cache flushing
	{
	XCodeTimer timer(L"std::rand");
	
	for (uint32 index = 0; index < 10000; ++index)
		n = std::rand();
		ASSERT(n >= 0);
		ASSERT(n <= RAND_MAX);
	}

	{
	XCodeTimer timer(L"RandomInt32");
	
	for (uint32 index = 0; index < 10000; ++index)
		n = RandomInt32(RAND_MAX);
		ASSERT(n >= 0);
		ASSERT(n < RAND_MAX);
	}

	{
	XCodeTimer timer(L"std::rand(1.0)");
	
	for (uint32 index = 0; index < 10000; ++index)
		x = (RAND_MAX*scalb(std::rand(), -31));
		ASSERT(x >= 0.0);
		ASSERT(x <= 1.0);
	}

	{
	XCodeTimer timer(L"Random(1.0)");
	
	for (uint32 index = 0; index < 10000; ++index)
		x = Random(1.0);
		ASSERT(x >= 0.0);
		ASSERT(x <= 1.0);
	}

	// Flushes the cache
	{
	XCodeTimer timer(L"std::rand");
	
	for (uint32 index = 0; index < 10000; ++index)
		n = std::rand();
		ASSERT(n >= 0);
		ASSERT(n <= RAND_MAX);
		this->DoFlushCache(buffer, sizeof(buffer));
	}

	{
	XCodeTimer timer(L"RandomInt32");
	
	for (uint32 index = 0; index < 10000; ++index)
		n = RandomInt32(RAND_MAX);
		ASSERT(n >= 0);
		ASSERT(n < RAND_MAX);
		this->DoFlushCache(buffer, sizeof(buffer));
	}

	{
	XCodeTimer timer(L"std::rand(1.0)");
	
	for (uint32 index = 0; index < 10000; ++index)
		x = (RAND_MAX*scalb(std::rand(), -31));
		ASSERT(x >= 0.0);
		ASSERT(x <= 1.0);
		this->DoFlushCache(buffer, sizeof(buffer));
	}

	{
	XCodeTimer timer(L"Random(1.0)");
	
	for (uint32 index = 0; index < 10000; ++index)
		x = Random(1.0);
		ASSERT(x >= 0.0);
		ASSERT(x <= 1.0);
		this->DoFlushCache(buffer, sizeof(buffer));
	}

	delete [] buffer;
}


//---------------------------------------------------------------
//
// ZNumbersTest::DoFlushCache
//
//---------------------------------------------------------------
void ZNumbersTest::DoFlushCache(const char* buffer, uint32 count)
{
	for (uint32 i = 0; i < count; ++i)
		mSum += buffer[i];							// do something with the buffer so the compiler doesn't optimize away our loop
}

#endif	// DEBUG


}		// namespace Whisper


