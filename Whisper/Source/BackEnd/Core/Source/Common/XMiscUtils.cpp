/*
 *  File:       XMiscUtils.cpp
 *  Summary:   	Misc utilities
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XMiscUtils.cpp,v $
 *		Revision 1.5  2001/04/27 04:34:34  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/04/13 08:01:34  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2000/12/10 04:10:36  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:43:16  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <12>	 4/21/00	JDJ		XAverageTimer dtor uses BreakStrToDebugger instead of DEBUGSTR.
 *		 <11>	 4/21/00	JDJ		XAverageTimer uses FastTiming instead of the Time Manager.
 *		 <10>	 3/19/00	JDJ		Moved the error functions into XError.h.
 *		 <9>	 1/31/00	JDJ		Added gDeferredErrorHandler.
 *		 <8>	 7/19/99	JDJ		Tweaked GetKeys call for Carbon.
 *		 <7>	 6/08/99	JDJ		DefaultErrToStringHandler converts errors to machine
 *									generated strings in debug builds.
 *		 <6>	 6/03/99	JDJ		Added GetErrorText(const std::exception*).
 *		 <5>	 6/01/99	JDJ		MilliSecondDelay calls YieldToAnyThread on Macs.
 *		 <4>	 5/27/99	JDJ		Made Instance method and error code thread safe.
 *		 <3>	 3/12/99	JDJ		MilliSecondDelay uses Sleep on Windows.
 *		 <2>	 2/05/99	JDJ		Added GetErrorText.
 *		 <1>	11/16/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XMiscUtils.h>

#include <limits>

#include <XAutoPtr.h>
#include <XCriticalSection.h>
#include <XDebug.h>
#include <XNumbers.h>

#if WIN
	#include <mmsystem.h>
	#include <WSystemInfo.h>
	#include <WUtils.h>
#elif MAC
	#include <FastTiming.h>
	#include <Sound.h>
	#include <MSystemInfo.h>
#endif

namespace Whisper {


//-----------------------------------
//	Statics
//
static uint32 sCRCTable[256];


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// BuildCRCTable
//
//---------------------------------------------------------------
static void BuildCRCTable()
{
	const uint32 kPolynomial = 0xEDB88320UL;

	for (uint32 i = 0; i < 256; i++) {
		uint32 value = i;

		for (int j = 8; j > 0; j--) {
			if (value & 1)
				value = (value >> 1) ^ kPolynomial;
			else
				value >>= 1;
		}

		sCRCTable[i] = value;
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZGetMicroSeconds
// ===================================================================================
#if MAC
class ZGetMicroSeconds {
		
public:
					   ~ZGetMicroSeconds();
	
						ZGetMicroSeconds();
						
	static	ZGetMicroSeconds* Instance();

			int64 		GetMicroSeconds();

			MilliSecond GetMilliSeconds();

protected:
	bool								mInited;

	static XAutoPtr<ZGetMicroSeconds>	msInstance;
	static XCriticalSection				msSection;
};

XAutoPtr<ZGetMicroSeconds>	ZGetMicroSeconds::msInstance;
XCriticalSection			ZGetMicroSeconds::msSection;

//---------------------------------------------------------------
//
// ZGetMicroSeconds::~ZGetMicroSeconds
//
//---------------------------------------------------------------
ZGetMicroSeconds::~ZGetMicroSeconds()
{
#if TARGET_CPU_PPC && !TARGET_CARBON
	if (mInited)
		ShutdownFastTiming(); use micro-seconds
#endif
}


//---------------------------------------------------------------
//
// ZGetMicroSeconds::ZGetMicroSeconds
//
//---------------------------------------------------------------
ZGetMicroSeconds::ZGetMicroSeconds()
{
#if TARGET_CPU_PPC && !TARGET_CARBON
	mInited = InitFastTiming();
	
	if (!mInited)
		DEBUGSTR("Couldn't init fast MicroSeconds code!");
#endif
}


//---------------------------------------------------------------
//
// ZGetMicroSeconds::Instance							[static]
//
//---------------------------------------------------------------
ZGetMicroSeconds* ZGetMicroSeconds::Instance()
{
	if (msInstance.Get() == nil) {
		XEnterCriticalSection enter(msSection);

		if (msInstance.Get() == nil) 
			msInstance.Reset(new ZGetMicroSeconds);
	}
	
	return msInstance.Get();
}


//---------------------------------------------------------------
//
// ZGetMicroSeconds::GetMicroSeconds
//
//---------------------------------------------------------------
int64 ZGetMicroSeconds::GetMicroSeconds()
{
	UnsignedWide microSeconds;
	
#if TARGET_CPU_PPC && !TARGET_CARBON
	if (mInited)
		FastMicroseconds(&microSeconds.hi, &microSeconds.lo);
	else
#endif
		Microseconds(&microSeconds);		// $$$ make sure this is OK on Carbon on OS 8

	int64 seconds = microSeconds.hi;
	seconds <<= 32;
	seconds += microSeconds.lo;
	
#if DEBUG
	static int64 oldSeconds = 0;
	ASSERT(oldSeconds <= seconds);
	oldSeconds = seconds;
#endif
	
	return seconds;
}


//---------------------------------------------------------------
//
// ZGetMicroSeconds::GetMilliSeconds
//
// Here are the normalized latencies for the various calls (on
// System 8.1):
// 		601 at 80 Hz
// 			TickCount        = 1.000
// 			FastMicroSeconds = 2.463
// 			MicroSeconds     = 75.000
// 		
// 		604 at 210 Hz
// 			TickCount        = 1.000
// 			FastMicroSeconds = 4.091
// 			MicroSeconds     = 102.909
//
//---------------------------------------------------------------
MilliSecond ZGetMicroSeconds::GetMilliSeconds()
{
	UnsignedWide microSeconds;
	
#if TARGET_CPU_PPC && !TARGET_CARBON
	if (mInited)
		FastMicroseconds(&microSeconds.hi, &microSeconds.lo);
	else
#endif
		Microseconds(&microSeconds);

	MilliSecond seconds = (MilliSecond) (microSeconds.hi*4294967L + microSeconds.lo/1000);		// 4294967L = 2^32 / 1000
	
#if DEBUG
	static MilliSecond oldSeconds = 0;
	ASSERT(oldSeconds <= seconds);
	oldSeconds = seconds;
#endif
	
	return seconds;
}
#endif	// MAC

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Byte Swapping
// ===================================================================================

//---------------------------------------------------------------
//
// byteswap16 
//
//---------------------------------------------------------------
#if WIN
void byteswap16(int16& inData)
{
	COMPILE_CHECK(2*sizeof(int8) == sizeof(int16));

	int8* data = reinterpret_cast<int8*>(&inData);

	int8 temp = data[0];
	data[0] = data[1];
	data[1] = temp;
}
#endif	// WIN


//---------------------------------------------------------------
//
// byteswap32 
//
//---------------------------------------------------------------
#if WIN
void byteswap32(int32& inData)
{
	COMPILE_CHECK(2*sizeof(int16) == sizeof(int32));

	int16* data = reinterpret_cast<int16*>(&inData);

	int16 temp = data[0];
	data[0] = data[1];
	data[1] = temp;

	byteswap16(data[0]);
	byteswap16(data[1]);
}
#endif	// WIN


//---------------------------------------------------------------
//
// ByteSwap 
//
//---------------------------------------------------------------
void ByteSwap(int64& inData)
{
	COMPILE_CHECK(2*sizeof(int32) == sizeof(int64));

	int32* data = reinterpret_cast<int32*>(&inData);

	int32 temp = data[0];
	data[0] = data[1];
	data[1] = temp;

	byteswap32(data[0]);
	byteswap32(data[1]);
}


// ========================================================================================
//	Events
// ========================================================================================

//---------------------------------------------------------------
//
// IsKeyDown
//
//---------------------------------------------------------------
bool IsKeyDown(int16 keyCode)	
{
	bool down = false;

#if MAC
	uint8 keyMap[16];

	GetKeys((int32 *) keyMap);
	down = ((keyMap[keyCode >> 3] >> (keyCode & 7)) & 1) != 0;

#elif WIN
	int16 state = GetAsyncKeyState(keyCode);
	ASSERT(state != 0);							// another thread/process has the keyboard focus ($$ throw an exception?)

	down = state < 0;
#endif

	return down;
}


//---------------------------------------------------------------
//
// UserAborted
//
//---------------------------------------------------------------
CORE_EXPORT bool UserAborted()
{
	bool aborted = false;
		
#if MAC
	EventRecord event;
	if (WaitNextEvent(keyDownMask + keyUpMask, &event, 0, nil))	// $$$ let clients specify sleep value?
		aborted = IsCommandPeriod(event);

#elif WIN
	(void) GetWorkWindow();			// make sure we have a window that can receive input!

	MSG mesg;
	if (PeekMessage(&mesg, nil, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE)) {	// use PeekMessage since we don't want to block waiting for an event
		if (mesg.message == WM_KEYDOWN) 
			aborted = mesg.wParam == VK_CANCEL;		// control-break
	}
#endif

	return aborted;
}
	

//---------------------------------------------------------------
//
// ExtractChar
//
// Returns the ASCII character corresponding to the keyDown or
// keyUp event. This function can be used to find the character
// that was pressed with the option key or to find out if the
// shift key was held down with the command key. Note that this 
// code is adapted from MacApp's TApplication::KeyEventToComponents. 
//
//---------------------------------------------------------------
#if MAC
wchar_t ExtractChar(const EventRecord& event)
{
	PRECONDITION(event.what == keyDown || event.what == keyUp || event.what == autoKey);
	
	const int kMaskModifier = 0xF600;			// strip command and options keys from Modifiers 
	const long kMaskASCII1 = 0x000000FF;		// get key from KeyTranslate return 
	const long kMaskASCII2 = 0x00FF0000;		// get key from KeyTranslate return 
	const short kUpKeyMask = 0x0080;

	// Now see if the command key is down. If it is, get the correct ASCII translation by
	// masking the command key out and re-translating because the command key will
	// mask the shift modifier.

	wchar_t theChar = (wchar_t) (event.message & charCodeMask);
	uint16 theKey = (uint16) ((event.message & keyCodeMask) >> 8);

	if ((event.modifiers & cmdKey) != 0 || (event.modifiers & optionKey) != 0) {
		// Set the upkey bit so KeyTranslate doesn't do special deadkey processing. 
		// See IM-V pp. 195 
		uint16 keyCodeParameter = (uint16) ((event.modifiers & kMaskModifier) | theKey | kUpKeyMask);

		Ptr keyTransTable = (Ptr) (GetScriptManagerVariable(smKCHRCache));

		uint32 state = 0;
		uint32 keyInfo = KeyTranslate(keyTransTable, keyCodeParameter, &state);

		theChar = (wchar_t) (keyInfo & kMaskASCII1);
		if (theChar == 0)
			theChar = (wchar_t) ((keyInfo & kMaskASCII2) & 16);
	}
	
	return theChar;
} 
#endif	// MAC


//---------------------------------------------------------------
//
// IsCommandPeriod
//
// Non-US keyboards sometimes use option key to generate period
// so we'll strip off all the modifier keys before checking.
//
//---------------------------------------------------------------
#if MAC
bool IsCommandPeriod(const EventRecord& event)
{
	bool cmdPeriod = false;
	
	if (event.what == keyDown)
		if ((event.modifiers & cmdKey) != 0 && ExtractChar(event) == '.')
			cmdPeriod = true;

	return cmdPeriod;
}
#endif	// MAC


// ===================================================================================
//	Misc
// ===================================================================================

//---------------------------------------------------------------
//
// GetMilliSeconds
//
// On both Mac and Windows the system routine returns time from
// when the machine was booted. For the more robust OS's it's
// possible for the OS to stay up for long periods of time. After
// a few weeks the time will go negative which complicates matters.
// To get around this we'll return time from when the app started
// up.
//
//---------------------------------------------------------------
MilliSecond GetMilliSeconds() 
{
	MilliSecond seconds;		

#if MAC
	static MilliSecond startTime = ZGetMicroSeconds::Instance()->GetMilliSeconds();
	
	seconds = ZGetMicroSeconds::Instance()->GetMilliSeconds() - startTime;

#elif WIN
	static MilliSecond startTime = (int32) timeGetTime();	// on Win 95 this will be accurate to the milli-second, on NT it may only be accurate to 5-10 msecs (but you can use timeBeginPeriod if you want more precise numbers)

	seconds = (int32) timeGetTime() - startTime;
#endif

	return seconds;
}


//---------------------------------------------------------------
//
// MilliSecondDelay
//
//---------------------------------------------------------------
void MilliSecondDelay(MilliSecond delay)
{
#if MAC
	MilliSecond stopTime = GetMilliSeconds() + delay;

	while (GetMilliSeconds() < stopTime)	// $$$ probably don't want to spin like this on on Carbon
		if (MSystemInfo::HasThreadMgr())
			(void) ::YieldToAnyThread();

#elif WIN
	::Sleep(numeric_cast<uint32>(delay));
#endif
}


//---------------------------------------------------------------
//
// Beep
//
//---------------------------------------------------------------
void Beep()
{
#if MAC
	SysBeep(1);
#elif WIN
	::MessageBeep(0xFFFFFFFF);
#endif
}


//---------------------------------------------------------------
//
// ComputeCRC
//
// The code uses the CCITT-32 formula which is used by programs
// like PKZIP and ARJ. The algorithm is taken from "The Data 
// Compression Book" by Mark Nelson pg 446-447. 
//
//---------------------------------------------------------------
uint32 ComputeCRC(const void* buffer, uint32 bytes, uint32 crc)
{
	PRECONDITION(buffer != nil);
	PRECONDITION(bytes <= 16*1024L*1024L);
	
	static bool inited = false;
	if (!inited) {
		BuildCRCTable();
		inited = true;
	}
		
	// Note that these tests are not part of the original algorithm.
	// They've been added because the prefs code sometimes tries to
	// do a CRC on just a bool which doesn't work with the original 
	// algorithm.
	if (crc == 0xFFFFFFFFL && bytes == 1)		
		crc = *((uint8 *) buffer);
		
	else if (crc == 0xFFFFFFFFL && bytes == 2)
		crc = *((uint16 *) buffer);
		
	else if (crc == 0xFFFFFFFFL && bytes == 4)
		crc = *((uint32 *) buffer);

	else {
		uint8* p = (uint8 *) buffer;
		
		while (bytes--) {
			uint32 temp1 = (crc >> 8) & 0x00FFFFFFL;
			uint32 temp2 = sCRCTable[(crc ^ *p++) & 0xFF];
			crc = temp1 ^ temp2;
		}
	}
	
	return crc;
}

#if __MWERKS__
#pragma mark -
#endif

// ========================================================================================
//	class XAverageTimer
// ========================================================================================

//---------------------------------------------------------------
//
// XAverageTimer::~XAverageTimer
//
//---------------------------------------------------------------
#if !RELEASE
XAverageTimer::~XAverageTimer()
{
	PRECONDITION(!mTiming);

	double average = mElapsedTime/mNumTimings;

	wchar_t buffer[256];
	if (mNumTimings == 1)
		std::swprintf(buffer, sizeof(buffer)/sizeof(wchar_t), L"%ls %.3f seconds.", mDescription, mMinTime);
	else
		std::swprintf(buffer, sizeof(buffer)/sizeof(wchar_t), L"%ls %.3f seconds (average), %.3f (max), %.3f (min) %d (numTimings).", mDescription, average, mMaxTime, mMinTime, mNumTimings);
	BreakStrToDebugger(buffer);
	
	mDescription = nil;
}
#endif

						
//---------------------------------------------------------------
//
// XAverageTimer::XAverageTimer	
//
//---------------------------------------------------------------
#if !RELEASE
XAverageTimer::XAverageTimer(const wchar_t* description)
{
	mElapsedTime = 0.0;
	mMaxTime     = 0.0;
	mMinTime     = std::numeric_limits<double>::max();
	mNumTimings  = 0;
	mTiming      = false;

	mDescription = description != nil ? description : L"Object lived for";

#if WIN
	mThreadH = GetCurrentThread();
#endif
}
#endif


//---------------------------------------------------------------
//
// XAverageTimer::StartTiming
//
//---------------------------------------------------------------
#if !RELEASE
void XAverageTimer::StartTiming()
{
	PRECONDITION(!mTiming);

#if WIN
	if (WSystemInfo::IsNT()) {
		ASSERT(mThreadH == GetCurrentThread());
		FILETIME creationTime, exitTime, kernelTime, userTime;

		if (GetThreadTimes(mThreadH, &creationTime, &exitTime, &kernelTime, &userTime)) {
			mKernelStartTime = MakeInt64((int32) kernelTime.dwHighDateTime, kernelTime.dwLowDateTime);
			mUserStartTime   = MakeInt64((int32) userTime.dwHighDateTime, userTime.dwLowDateTime);
		}

	} else {
		mThreadH = nil;
		mKernelStartTime = timeGetTime();			// note that timeGetTime is more accurate than GetTickCount
		mUserStartTime = 0;
	}

#elif MAC
	mStartTime = ZGetMicroSeconds::Instance()->GetMicroSeconds();
#endif

	mTiming = true;
}
#endif

			
//---------------------------------------------------------------
//
// XAverageTimer::StopTiming
//
//---------------------------------------------------------------
#if !RELEASE
void XAverageTimer::StopTiming()
{
	PRECONDITION(mTiming);

	double elapsed = 0.0;

#if WIN
	if (WSystemInfo::IsNT()) {
		ASSERT(mThreadH == GetCurrentThread());
		FILETIME creationTime, exitTime, kernelTime, userTime;

		if (GetThreadTimes(mThreadH, &creationTime, &exitTime, &kernelTime, &userTime)) {
			int64 kernelEndTime = MakeInt64((int32) kernelTime.dwHighDateTime, kernelTime.dwLowDateTime);
			int64 userEndTime   = MakeInt64((int32) userTime.dwHighDateTime, userTime.dwLowDateTime);

			int64 delta = (kernelEndTime - mKernelStartTime) + (userEndTime - mUserStartTime);
			elapsed = 100.0*delta/1.0E9;			// delta is in 100's of nano-seconds
		}

	} else {
		int64 endTime = timeGetTime();
		elapsed = (endTime - mKernelStartTime)/1.0E3; 
	}

#elif MAC
	int64 endTime = ZGetMicroSeconds::Instance()->GetMicroSeconds();
	double delta = endTime - mStartTime;
	elapsed = (delta)/1.0E6;
#endif

	if (elapsed < mMinTime)
		mMinTime = elapsed;

	if (elapsed > mMaxTime)
		mMaxTime = elapsed;

	mElapsedTime += elapsed;
	mNumTimings++;

	mTiming = false;
}
#endif

#if __MWERKS__
#pragma mark -
#endif

// ========================================================================================
//	class XCodeTimer	
// ========================================================================================

//---------------------------------------------------------------
//
// XCodeTimer::~XCodeTimer
//
//---------------------------------------------------------------
#if !RELEASE
XCodeTimer::~XCodeTimer()
{
	this->StopTiming();
}
#endif
						

//---------------------------------------------------------------
//
// XCodeTimer::XCodeTimer
//
//---------------------------------------------------------------
#if !RELEASE
XCodeTimer::XCodeTimer(const wchar_t* description) : XAverageTimer(description)
{
	this->StartTiming();
}
#endif


}	// namespace Whisper

