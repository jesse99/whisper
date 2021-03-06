/*
 *  File:       XKeyEvents.cpp
 *  Summary:   	Cross platform wrapper around keystroke events.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XKeyEvents.cpp,v $
 *		Revision 1.4  2001/04/27 04:33:45  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2000/12/10 04:10:11  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:40:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 4/05/99	JDJ		Added XKeyDownEvent::ComputeChar and XCharEvent::ComputeKey.
 *		 <2>	 4/03/99	JDJ		Added GetRepeatDelay. Implemented GetRepeatCount on Windows.
 *		 <1>	10/08/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XKeyEvents.h>

#include <XExceptions.h>
#include <XNumbers.h>

#if MAC
	#include <LowMem.h>
#elif WIN
	#include <WSystemInfo.h>
#endif

namespace Whisper {


// ========================================================================================
//	class XKeyDownEvent
// ========================================================================================

//---------------------------------------------------------------
//
// XKeyDownEvent::~XKeyDownEvent
//
//---------------------------------------------------------------
XKeyDownEvent::~XKeyDownEvent()
{
}


//---------------------------------------------------------------
//
// XKeyDownEvent::XKeyDownEvent ()
//
//---------------------------------------------------------------
XKeyDownEvent::XKeyDownEvent()
{
	mChar = (wchar_t) -1;
}


//---------------------------------------------------------------
//
// XKeyDownEvent::XKeyDownEvent (OSEvent)
//
//---------------------------------------------------------------
XKeyDownEvent::XKeyDownEvent(const OSEvent& event) : XEvent(event)
{
#if MAC
	PRECONDITION(event.what == keyDown || event.what == autoKey);
#elif WIN
	PRECONDITION(event.message == WM_KEYDOWN);
#endif

	mChar = (wchar_t) -1;
}


//---------------------------------------------------------------
//
// XKeyDownEvent::ComputeChar		
//
//---------------------------------------------------------------
wchar_t XKeyDownEvent::ComputeChar() const				
{	
	if (mChar == -1) {
#if MAC
		mChar = (wchar_t) (mEvent.message & charCodeMask);
#elif WIN
		if (WSystemInfo::HasUnicode())
			mChar = MapVirtualKeyW(mEvent.wParam, 2) && 0xFFFF;
		else
			mChar = MapVirtualKeyA(mEvent.wParam, 2) && 0xFF;
#endif
	}
	
	return mChar;
}


//---------------------------------------------------------------
//
// XKeyDownEvent::GetRepeatDelay						[static]
//
//---------------------------------------------------------------
MilliSecond XKeyDownEvent::GetRepeatDelay()
{
	static MilliSecond time = 0;

	if (time == 0) {
#if MAC
		int32 ticks = Min(LMGetKeyThresh(), (short) 128);
		time = ticks*1000L/60;

#elif WIN
		int32 delay;
		int32 succeeded = SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &delay, 0);
		ThrowIf(!succeeded);
		ASSERT(delay >= 0);

		time = 250 + 250*delay;
#endif
	}

	return time;
}


//---------------------------------------------------------------
//
// XKeyDownEvent::GetRepeatFreq							[static]
//
//---------------------------------------------------------------
MilliSecond XKeyDownEvent::GetRepeatFreq()
{
	static MilliSecond time = 0;

	if (time == 0) {
#if MAC
		// A note on LMSetKeyRepThresh: the key repeat rate typically 
		// ranges from 12 ticks (short) to 40 ticks (int32).  However, 
		// if key repeat is turned off, LMGetKeyThresh returns SHRT_MAX.  
		// To prevent an fTimeOutInterval that high, we bound it at 128 
		// ticks (about 2 seconds).
		int32 ticks = Min(LMGetKeyRepThresh(), (short) 128);
		time = ticks*1000L/60;

#elif WIN
		int32 freq;
		int32 succeeded = SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &freq, 0);
		ThrowIf(!succeeded);
		ASSERT(freq >= 0);

		time = 33 + 12*freq;
#endif
	}

	return time;
}

#if __MWERKS__
#pragma mark -
#endif

// ========================================================================================
//	class XCharEvent
// ========================================================================================

//---------------------------------------------------------------
//
// XCharEvent::~XCharEvent
//
//---------------------------------------------------------------
XCharEvent::~XCharEvent()
{
}


//---------------------------------------------------------------
//
// XCharEvent::XCharEvent ()
//
//---------------------------------------------------------------
XCharEvent::XCharEvent()
{
	mKey = 0;
}


//---------------------------------------------------------------
//
// XCharEvent::XCharEvent (OSEvent)
//
//---------------------------------------------------------------
XCharEvent::XCharEvent(const OSEvent& event) : XEvent(event)
{
#if MAC
	PRECONDITION(event.what == keyDown || event.what == autoKey);
#elif WIN
	PRECONDITION(event.message == WM_CHAR || event.message == WM_SYSCHAR);
#endif

	mKey = 0;
}


//---------------------------------------------------------------
//
// XCharEvent::ComputeKey
//
//---------------------------------------------------------------
int16 XCharEvent::ComputeKey() const				
{	
	if (mKey == 0) {
#if MAC
		mKey = (int16) ((mEvent.message & keyCodeMask) >> 8);
#elif WIN
		if (WSystemInfo::HasUnicode())
			mKey = numeric_cast<int16>(VkKeyScanW((wchar_t) mEvent.wParam) & 0xFF);
		else
			mKey = numeric_cast<int16>(VkKeyScanA((char) mEvent.wParam) & 0xFF);
#endif
	}
	
	return mKey;
}

#if __MWERKS__
#pragma mark -
#endif

// ========================================================================================
//	class XKeyUpEvent
// ========================================================================================

//---------------------------------------------------------------
//
// XKeyUpEvent::~XKeyUpEvent
//
//---------------------------------------------------------------
XKeyUpEvent::~XKeyUpEvent()
{
}


//---------------------------------------------------------------
//
// XKeyUpEvent::XKeyUpEvent ()
//
//---------------------------------------------------------------
XKeyUpEvent::XKeyUpEvent()
{
}


//---------------------------------------------------------------
//
// XKeyUpEvent::XKeyUpEvent (OSEvent)
//
//---------------------------------------------------------------
XKeyUpEvent::XKeyUpEvent(const OSEvent& event) : XEvent(event)
{
#if MAC
	PRECONDITION(event.what == keyUp);
#elif WIN
	PRECONDITION(event.message == WM_KEYUP);
#endif
}


}	// namespace Whisper

