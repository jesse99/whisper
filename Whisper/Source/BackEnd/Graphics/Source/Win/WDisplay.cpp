/*
 *	File:		WDisplay.cpp
 *	Function:	Encapsulates a display device (typically a monitor).
 *	Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WDisplay.cpp,v $
 *		Revision 1.4  2001/04/21 03:31:02  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2000/12/10 04:06:38  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:06:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XDisplay.h>

#include <XExceptions.h>
#include <XNumbers.h>

namespace Whisper {


// ===================================================================================
//	class XDisplayState
// ===================================================================================

//---------------------------------------------------------------
//
// XDisplayState::XDisplayState ()
//
//---------------------------------------------------------------
XDisplayState::XDisplayState()
{
	mDisplay = 0;
	
	mResolution = kZeroSize;

	mDepth = 0;
	
	mRefreshRate = 0;
}


//---------------------------------------------------------------
//
// XDisplayState::XDisplayState (OSDisplay, DEVMODE)
//
//---------------------------------------------------------------
XDisplayState::XDisplayState(OSDisplay display, const DEVMODE& mode) : mState(mode)
{	
	mDisplay = display;
	
	mResolution.width  = numeric_cast<int32>(mode.dmPelsWidth);
	mResolution.height = numeric_cast<int32>(mode.dmPelsHeight);

	mDepth = numeric_cast<int32>(mode.dmBitsPerPel);
	
	mRefreshRate = mode.dmDisplayFrequency;
	ASSERT(mRefreshRate <= 128.0);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XDisplay::iterator
// ===================================================================================

//---------------------------------------------------------------
//
// XDisplay::iterator::~iterator
//
//---------------------------------------------------------------
XDisplay::iterator::~iterator()
{
}

				
//---------------------------------------------------------------
//
// XDisplay::iterator::iterator ()
//
//---------------------------------------------------------------
XDisplay::iterator::iterator()
{
	mDisplay = 0;
	mIndex = ULONG_MAX;
}

				
//---------------------------------------------------------------
//
// XDisplay::iterator::iterator (XDisplay*)
//
//---------------------------------------------------------------
XDisplay::iterator::iterator(const XDisplay* display)
{
	PRECONDITION(display != nil);
	
	mDisplay = *display;
	
	mIndex = 0;
	int32 succeeded = ::EnumDisplaySettings(nil, mIndex, &mMode);
	if (!succeeded)
		mIndex = ULONG_MAX;
}

	
//---------------------------------------------------------------
//
// XDisplay::iterator::iterator (iterator)
//
//---------------------------------------------------------------
XDisplay::iterator::iterator(const iterator& rhs) : mMode(rhs.mMode)
{
	mDisplay = rhs.mDisplay;
	mIndex = rhs.mIndex;
}
	

//---------------------------------------------------------------
//
// XDisplay::iterator::operator=
//
//---------------------------------------------------------------
XDisplay::iterator& XDisplay::iterator::operator=(const iterator& rhs)
{
	if (this != &rhs) {
		mDisplay = rhs.mDisplay;
		mIndex = rhs.mIndex;
		mMode = rhs.mMode;
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XDisplay::iterator::operator*
//
//---------------------------------------------------------------
XDisplayState XDisplay::iterator::operator*() const
{
	PRECONDITION(mIndex < ULONG_MAX);
	
	return XDisplayState(mDisplay, mMode);
}


//---------------------------------------------------------------
//
// XDisplay::iterator::operator++
//
//---------------------------------------------------------------
XDisplay::iterator& XDisplay::iterator::operator++()
{	
	if (mIndex < ULONG_MAX) {
		int32 succeeded = ::EnumDisplaySettings(nil, ++mIndex, &mMode);
		if (!succeeded)
			mIndex = ULONG_MAX;
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XDisplay::iterator::operator==
//
//---------------------------------------------------------------
bool XDisplay::iterator::operator==(const iterator& rhs) const		
{
	bool equal = false;
	
	if (mDisplay == 0 || rhs.mDisplay == 0) {
		equal = (mIndex < ULONG_MAX) == (rhs.mIndex < ULONG_MAX);
	
	} else {
		ASSERT(mDisplay == rhs.mDisplay); 
	
		equal = mIndex == rhs.mIndex;
	}
	
	return equal;
}	

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XDisplay
// ===================================================================================

//---------------------------------------------------------------
//
// XDisplay::XDisplay
//
//---------------------------------------------------------------
XDisplay::XDisplay(const OSDisplay& display)
{	
	mDisplay = display;

	// $$$ There doesn't appear to be a way to get the current
	// $$$ DEVMODE so we'll use this gross hack to signal the
	// $$$ default mode. 
	mCurrentState.dmPelsWidth  = ULONG_MAX;
	mCurrentState.dmPelsHeight = ULONG_MAX;
	mCurrentState.dmBitsPerPel = ULONG_MAX;
}


//---------------------------------------------------------------
//
// XDisplay::GetLocation
//
//---------------------------------------------------------------
XPoint XDisplay::GetLocation() const
{
	return kZeroPt;
}


//---------------------------------------------------------------
//
// XDisplay::GetResolution
//
//---------------------------------------------------------------
XSize XDisplay::GetResolution() const
{	
	HDC dc = ::CreateIC("DISPLAY", nil, nil, nil);
	ThrowIf(dc == nil);
	
	XSize size;										// don't cache this (setting may change)
	size.width  = ::GetDeviceCaps(dc, HORZRES); 
	size.height = ::GetDeviceCaps(dc, VERTRES); 
	
	(void) DeleteDC(dc);
	
	return size;
}


//---------------------------------------------------------------
//
// XDisplay::GetDepth
//
//---------------------------------------------------------------
int32 XDisplay::GetDepth() const
{
	HDC dc = ::CreateIC("DISPLAY", nil, nil, nil);
	ThrowIf(dc == nil);
	
	int32 depth = ::GetDeviceCaps(dc, BITSPIXEL); 
	
	(void) DeleteDC(dc);
	
	return depth;
}


//---------------------------------------------------------------
//
// XDisplay::IsColor
//
//---------------------------------------------------------------
bool XDisplay::IsColor() const
{
	int32 depth = this->GetDepth();
	
	bool isColor = depth > 2;			// $$$ there doesn't seem to be a way to find out if the monitor is gray scale...
	
	return isColor;
}


//---------------------------------------------------------------
//
// XDisplay::GetClientArea
//
//---------------------------------------------------------------
XRect XDisplay::GetClientArea() const
{
	XRect area;
	
//	if (mDisplay == ::GetMainDevice()) {		// $$$ needs to be updated for Win 98
		int32 succeeded = ::SystemParametersInfo(SPI_GETWORKAREA, 0, area, 0);	// $$$ isn't there a flag that toggles this on and off? 
		ThrowIf(!succeeded);

//	} else
//		area = this->GetFrame();
	
	return area;
}


//---------------------------------------------------------------
//
// XDisplay::GetState
//
//---------------------------------------------------------------
OSDisplayState XDisplay::GetState() const
{
	return mCurrentState;
}


//---------------------------------------------------------------
//
// XDisplay::SetState
//
//---------------------------------------------------------------
bool XDisplay::SetState(const OSDisplayState& state)
{
	LONG result;
	
	if (mCurrentState.dmPelsWidth == ULONG_MAX)
		result = ::ChangeDisplaySettings(nil, 0);	
	else
		result = ::ChangeDisplaySettings(const_cast<DEVMODE*>(&state), 0);
		
	bool succeeded = result == DISP_CHANGE_SUCCESSFUL;
	
	if (succeeded)
		mCurrentState = state;
		
	return succeeded;
}


//---------------------------------------------------------------
//
// XDisplay::begin
//
//---------------------------------------------------------------
XDisplay::iterator XDisplay::begin() const
{
	return iterator(this);
}


//---------------------------------------------------------------
//
// XDisplay::end
//
//---------------------------------------------------------------
XDisplay::iterator XDisplay::end() const
{
	return iterator();
}


}	// namespace Whisper
