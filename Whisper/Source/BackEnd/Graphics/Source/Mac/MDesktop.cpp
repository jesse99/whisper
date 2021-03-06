/*
 *	File:		MDesktop.cpp
 *	Function:	Encapsulates the notion of a desktop (ie an object containing one or more displays).
 *	Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MDesktop.cpp,v $
 *		Revision 1.3  2001/04/21 03:24:56  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 09:12:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XDesktop.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// IsActiveScreenDevice
//
//---------------------------------------------------------------
static bool IsActiveScreenDevice(GDHandle device)
{
	PRECONDITION(device != nil);
	
	bool isActive = TestDeviceAttribute(device, screenDevice) && TestDeviceAttribute(device, screenActive);
	
	return isActive;
}

#pragma mark -

// ===================================================================================
//	class XDesktop::iterator
// ===================================================================================

//---------------------------------------------------------------
//
// XDesktop::iterator::iterator ()
//
//---------------------------------------------------------------
XDesktop::iterator::iterator()
{
	mDisplay = nil;
}
				

//---------------------------------------------------------------
//
// XDesktop::iterator::iterator (OSDisplay)
//
//---------------------------------------------------------------
XDesktop::iterator::iterator(OSDisplay display)
{
	PRECONDITION(display != nil);
	
	mDisplay = display;

	while (mDisplay != nil && !IsActiveScreenDevice(mDisplay))
		mDisplay = ::GetNextDevice(mDisplay);
}

	
//---------------------------------------------------------------
//
// XDesktop::iterator::operator++
//
//---------------------------------------------------------------
XDesktop::iterator& XDesktop::iterator::operator++()
{
	if (mDisplay != nil)
		mDisplay = ::GetNextDevice(mDisplay);

	while (mDisplay != nil && !IsActiveScreenDevice(mDisplay))
		mDisplay = ::GetNextDevice(mDisplay);
	
	return *this;
}


//---------------------------------------------------------------
//
// XDesktop::iterator::operator==
//
//---------------------------------------------------------------
bool XDesktop::iterator::operator==(const iterator& rhs) const
{	
	bool equal = mDisplay == rhs.mDisplay;
	
	return equal;
}	

#pragma mark -

// ===================================================================================
//	class XDesktop
// ===================================================================================

//---------------------------------------------------------------
//
// XDesktop::GetMainDisplay								[static]
//
//---------------------------------------------------------------
XDisplay XDesktop::GetMainDisplay()
{
	XDisplay display(::GetMainDevice());
	
	return display;
}


//---------------------------------------------------------------
//
// XDesktop::GetMaxDisplay								[static]
//
//---------------------------------------------------------------
XDisplay XDesktop::GetMaxDisplay(const XRect& globalRect)
{
	XDisplay display(::GetMainDevice());
	int32 greatestArea = 0;

	iterator iter = XDesktop::begin();
	while (iter != XDesktop::end()) {
		XDisplay candidate = *iter;
		++iter;
		
		XRect frame = candidate.GetFrame();
		XRect sect  = frame & globalRect;

		int32 area = sect.GetArea();
		if (area > greatestArea) {
			greatestArea = area;
			display = candidate;
		}
	}
	
	ASSERT(greatestArea > 0);

	return display;
}


//---------------------------------------------------------------
//
// XDesktop::GetShallowDisplay							[static]
//
//---------------------------------------------------------------
XDisplay XDesktop::GetShallowDisplay(const XRect& globalRect)
{
	XDisplay display(::GetMainDevice());
	int32 leastDepth = 64;

	iterator iter = XDesktop::begin();
	while (iter != XDesktop::end()) {
		XDisplay candidate = *iter;
		++iter;
		
		XRect frame = candidate.GetFrame();
		if (frame.Intersects(globalRect)) {
			int32 depth = candidate.GetDepth();
			if (depth < leastDepth) {
				leastDepth = depth;
				display = candidate;
			}
		}
	}

	ASSERT(leastDepth <= 32);

	return display;
}


//---------------------------------------------------------------
//
// XDesktop::GetDeepestDisplay							[static]
//
//---------------------------------------------------------------
XDisplay XDesktop::GetDeepestDisplay(const XRect& globalRect)
{
	XDisplay display(::GetMainDevice());
	int32 greatestDepth = 0;

	iterator iter = XDesktop::begin();
	while (iter != XDesktop::end()) {
		XDisplay candidate = *iter;
		++iter;
		
		XRect frame = candidate.GetFrame();
		if (frame.Intersects(globalRect)) {
			int32 depth = candidate.GetDepth();
			if (depth > greatestDepth) {
				greatestDepth = depth;
				display = candidate;
			}
		}
	}

	ASSERT(greatestDepth > 0);

	return display;
}


//---------------------------------------------------------------
//
// XDesktop::GetClientArea								[static]
//
//---------------------------------------------------------------
XRegion XDesktop::GetClientArea()
{
	XRegion desktop;
	
	XDesktop::iterator iter = XDesktop::begin();
	while (iter != XDesktop::end()) {
		XDisplay display = *iter;
		++iter;
		
		XRect bounds = display.GetClientArea();
		desktop += bounds;
	}
	
	return desktop;
}


//---------------------------------------------------------------
//
// XDesktop::begin										[static]
//
//---------------------------------------------------------------
XDesktop::iterator XDesktop::begin()
{
	return iterator(::GetDeviceList());
}


//---------------------------------------------------------------
//
// XDesktop::end										[static]
//
//---------------------------------------------------------------
XDesktop::iterator XDesktop::end()
{
	return iterator();
}


}	// namespace Whisper
