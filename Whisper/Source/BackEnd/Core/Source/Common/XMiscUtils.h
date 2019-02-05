/*
 *  File:       XMiscUtils.h
 *  Summary:   	Misc utilities
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XMiscUtils.h,v $
 *		Revision 1.3  2001/03/01 11:45:22  jesjones
 *		Disabled copying for a few classes.
 *		
 *		Revision 1.2  2000/11/09 12:43:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTypes.h>

#if MAC
	#include <Timer.h>
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	Byte Swapping
// ===================================================================================
#if WIN
	 CORE_EXPORT void	byteswap16(int16& data);
	 CORE_EXPORT void	byteswap32(int32& data);

#elif MAC
	#if __POWERPC__
		inline 	void 	byteswap16(int16& data)	{data = (int16) __lhbrx(&data, 0);}	
		inline 	void 	byteswap32(int32& data)	{data = (int32) __lwbrx(&data, 0);}	
	#else
		#pragma parameter __D0 byteswap16(__D0)
		short	byteswap16(int16) = 0xE058;      					// ROL.W #8,D0

		#pragma parameter __D0 byteswap32(__D0)
		long	byteswap32(int32)   = { 0xE058, 0x4840, 0xE058 };	// ROL.W #8,D0  SWAP D0  ROL.W #8,D0
	#endif
#endif

inline 	void	ByteSwap(int8&)					{}					// for convenience when swapping fields of a structure
inline 	void	ByteSwap(int16& data)			{byteswap16(data);}			
inline 	void	ByteSwap(int32& data)			{byteswap32(data);}			
CORE_EXPORT  void	ByteSwap(int64& data);
inline	void	ByteSwap(float& data)			{byteswap32((int32&) data);}
inline	void	ByteSwap(double& data)			{ByteSwap((int64&) data);}

#if MSVC < 1100					// MSVC 5 incorrectly doesn't treat wchar_t as a distinct type
inline 	void	ByteSwap(wchar_t& data)			{byteswap16((int16&)data);}			
#endif

inline 	void	ByteSwap(uint8&)				{}			
inline 	void	ByteSwap(uint16& data)			{ByteSwap((int16&) data);}			
inline 	void	ByteSwap(uint32& data)			{ByteSwap((int32&) data);}				
inline 	void	ByteSwap(uint64& data)			{ByteSwap((int64&) data);}	


// ===================================================================================
//	Events
// ===================================================================================
CORE_EXPORT bool IsKeyDown(int16 keyCode);
				// Returns asynchronous state of the keyboard.

CORE_EXPORT bool UserAborted();
				// Returns true if the user pressed command-period (Mac) or control-break 
				// (Win). Note that this function eats key down and up events.

#if MAC
	CORE_EXPORT wchar_t ExtractChar(const EventRecord& event);
				// Returns the ASCII character corresponding to the keyDown or keyUp event
				// (eg if the user pressed option-P you'll get P instead of ¹).

	CORE_EXPORT bool IsCommandPeriod(const EventRecord& event);
				// International friendly check for command-period.
#endif


// ===================================================================================
//	Misc
// ===================================================================================
inline int16	MakeInt16(int8 hi, uint8 lo)		{return (int16) ((((int16) hi) << 8) + lo);}
inline uint16	MakeUint16(uint8 hi, uint8 lo)		{return (uint16) ((((uint16) hi) << 8) + lo);}

inline int32	MakeInt32(int16 hi, uint16 lo)		{return (((int32) hi) << 16) + lo;}
inline uint32	MakeUint32(uint16 hi, uint16 lo)	{return (((uint32) hi) << 16) + lo;}

inline int64	MakeInt64(int32 hi, uint32 lo)		{return (((int64) hi) << 32) + lo;}
inline uint64	MakeUint64(uint32 hi, uint32 lo)	{return (((uint64) hi) << 32) + lo;}

CORE_EXPORT MilliSecond GetMilliSeconds();

CORE_EXPORT void MilliSecondDelay(MilliSecond delay);

CORE_EXPORT void Beep();

CORE_EXPORT uint32 ComputeCRC(const void* buffer, uint32 bytes, uint32 crc = 0xFFFFFFFFL);
				// Computes a fancy checksum for buffer which is able to detect all errors in 
				// 32 or fewer consectutive bits for any buffer sizeCRCs can be computed in 
				// chunks by passing in the CRC for the previous chunk. If two CRCs differ
				// then the source data also differs. If two CRCs are the same the source
				// data may or may not be identical.
			
			
// ========================================================================================
//	class XAverageTimer
//		This is used to find the average time required to execute a section of code. You'd
//		typically use this by defining a static object, calling StartTiming and StopTiming,
//		and wait for the object to be destroyed (which will use DEBUGSTR to print the
//		average time). Note that these times will be accurate to at least micro-seconds,
//		except on Windows 95 where they are inaccurate to the milli-second (since there's
//		no way to avoid timing any threads that preempt our thread).
// ========================================================================================
#if !RELEASE
class CORE_EXPORT XAverageTimer {

//-----------------------------------
//	Initialization/Destruction
//
public:
				   		~XAverageTimer();
				   
						XAverageTimer(const wchar_t* description = (const wchar_t*) nil);
						
private:
						XAverageTimer(const XAverageTimer& rhs);
			XAverageTimer& operator=(const XAverageTimer& rhs);

//-----------------------------------
//	API
//
public:
			void 		StartTiming();
			
			void 		StopTiming();

//-----------------------------------
//	Member Data
//
private:
	double			mElapsedTime;			// times are in msecs, but use doubles since Mac and NT have better than milli-second precision
	double			mMaxTime;	
	double			mMinTime;	
	long			mNumTimings;

	bool			mTiming;	
	const wchar_t*	mDescription;		

#if WIN
	HANDLE			mThreadH;
	int64			mKernelStartTime;		// NT: 100's of nano-seconds, 95: milli-seconds
	int64			mUserStartTime;

#elif MAC
	int64			mStartTime;			
#endif
};
#endif	// !RELEASE


// ========================================================================================
//	class XCodeTimer
//		This is used to time the amount of time required to execute a section of code. On
//		the Mac and NT the time will be accurate to the microsecond (or even better). On
//		Windows 95 the time will be to the millisecond and unreliable (since the time will
//		include any threads that happen to preempt our thread $$$Êmight want to optionally
//		set thread priority to real time $$$). 
// ========================================================================================
#if !RELEASE
class CORE_EXPORT XCodeTimer : private XAverageTimer 
{

//-----------------------------------
//	Initialization/Destruction
//
public:
				  		~XCodeTimer();

						XCodeTimer(const wchar_t* description = (const wchar_t*) nil);
};
#endif	// !RELEASE


// ========================================================================================
//	class XStartAverageTimer
// ========================================================================================
#if !RELEASE
class CORE_EXPORT XStartAverageTimer {

public:
				   		~XStartAverageTimer()										{mTimer.StopTiming();}
				   
						XStartAverageTimer(XAverageTimer& timer) : mTimer(timer)	{mTimer.StartTiming();}
						
private:
						XStartAverageTimer(const XStartAverageTimer& rhs);
			XStartAverageTimer& operator=(const XStartAverageTimer& rhs);

private:
	XAverageTimer&	mTimer;				
};
#endif	// !RELEASE


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
