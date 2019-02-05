/*
 *  File:       XNumbersTest.cpp
 *  Summary:   	Unit test for the stuff in XNumbers.h
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XNumbersTest.cpp,v $
 *		Revision 1.4  2001/03/02 11:24:46  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/12/10 04:10:51  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:44:46  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XNumbersTest.h>

#include <iomanip>
#include <vector>

#include <XMiscUtils.h>
#include <Xmt19937-2.h>
#include <Xmt19937int.h>
#include <XNumbers.h>

#if WIN
	#include <mmsystem.h>
#endif

void sgenrand(unsigned long seed);	
void seedMT(uint32 seed);

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class XNumbersTest
// ===================================================================================

//---------------------------------------------------------------
//
// XNumbersTest::~XNumbersTest
//
//---------------------------------------------------------------
XNumbersTest::~XNumbersTest()
{
}

	
//---------------------------------------------------------------
//
// XNumbersTest::XNumbersTest
//
//---------------------------------------------------------------
XNumbersTest::XNumbersTest() : XUnitTest(L"Backend", L"Numbers")
{
}
						

//---------------------------------------------------------------
//
// XNumbersTest::OnTest
//
//---------------------------------------------------------------
void XNumbersTest::OnTest()
{
	this->DoTestRandomInt();
	this->DoTestFloatInt();
			
//	this->DoPlotUniform();
//	this->DoPlotNormal();
	
	// $$$ print flip coin

	this->DoConstantTest();
//	this->DoSpeedTest();
	
	TRACE("Completed numbers test.\n\n");
}

			
//---------------------------------------------------------------
//
// XNumbersTest::DoConstantTest
//										
//---------------------------------------------------------------
void XNumbersTest::DoConstantTest()
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
// XNumbersTest::DoTestRandomInt
//
//---------------------------------------------------------------
void XNumbersTest::DoTestRandomInt()
{
	seedMT(4357);

	uint32 count = sizeof(sIntTable)/sizeof(sIntTable[0]);
	for (uint32 i = 0; i < count; ++i) {
		uint32 r = randomMT();
		uint32 t = sIntTable[i];

		ASSERT(r == t);
	}	
}


//---------------------------------------------------------------
//
// XNumbersTest::DoTestFloatInt
//										
//---------------------------------------------------------------
void XNumbersTest::DoTestFloatInt()
{
	sgenrand(4357);

	uint32 count = sizeof(sFloatTable)/sizeof(sFloatTable[0]);
	for (uint32 i = 0; i < count; ++i) {
		double r = genrand();
		double t = sFloatTable[i];

		double delta = Abs(r - t);
		ASSERT(delta < 1.0e-8);
	}	
}


//---------------------------------------------------------------
//
// XNumbersTest::DoPlotUniform
//
//---------------------------------------------------------------
void XNumbersTest::DoPlotUniform()
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
		TRACE(ToStr(index, 2), ":"); 
		int32 count = numeric_cast<int32>(30.0*buckets[index]/maxCount);
		for (int32 j = 0; j < count; ++j)
			TRACE("*");
		TRACE("\n");
	}
}


//---------------------------------------------------------------
//
// XNumbersTest::DoPlotNormal
//
//---------------------------------------------------------------
void XNumbersTest::DoPlotNormal()		
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
		TRACE(ToStr(index, 2), ":"); 
		int32 count = numeric_cast<int32>(30.0*buckets[index]/maxCount);
		for (int32 j = 0; j < count; ++j)
			TRACE("*");
		TRACE("\n");
	}
}


//---------------------------------------------------------------
//
// XNumbersTest::DoSpeedTest
//
//---------------------------------------------------------------
void XNumbersTest::DoSpeedTest()
{
	int32 n;
	double x;
	
	char* buffer = new char[64*1024L];

	// No cache flushing
#if 0
	{
	XCodeTimer timer(L"std::rand");
	
	for (uint32 index = 0; index < 10000; ++index)
		n = std::rand() % 1024;
		ASSERT(n >= 0);
		ASSERT(n < 1024);
	}

	{
	XCodeTimer timer(L"std::rand(1.0)");
	
	for (uint32 index = 0; index < 10000; ++index)
		x = (RAND_MAX*scalb(std::rand(), -31));
		ASSERT(x >= 0.0);
		ASSERT(x <= 1.0);
	}
#endif

	{
	XCodeTimer timer(L"Random(1024)");
	
	for (uint32 index = 0; index < 10000; ++index)
		n = Random(1024L);
		ASSERT(n >= 0);
		ASSERT(n < RAND_MAX);
	}

	{
	XCodeTimer timer(L"Random(1.0)");
	
	for (uint32 index = 0; index < 10000; ++index)
		x = Random(1.0);
		ASSERT(x >= 0.0);
		ASSERT(x <= 1.0);
	}

	// Flushes the cache
#if 0
	{
	XCodeTimer timer(L"std::rand");
	
	for (uint32 index = 0; index < 10000; ++index)
		n = std::rand() % 1024;
		ASSERT(n >= 0);
		ASSERT(n < 1024);
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
#endif

	{
	XCodeTimer timer(L"Random(1024)");
	
	for (uint32 index = 0; index < 10000; ++index)
		n = Random(1024L);
		ASSERT(n >= 0);
		ASSERT(n < 1024);
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
// XNumbersTest::DoFlushCache
//
//---------------------------------------------------------------
void XNumbersTest::DoFlushCache(const char* buffer, uint32 count)
{
	for (uint32 i = 0; i < count; ++i)
		mSum += buffer[i];							// do something with the buffer so the compiler doesn't optimize away our loop
}

#endif	// DEBUG
}		// namespace Whisper


