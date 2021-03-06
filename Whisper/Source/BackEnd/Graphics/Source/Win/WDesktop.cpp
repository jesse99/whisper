/*
 *	File:		WDesktop.cpp
 *	Function:	Encapsulates the notion of a desktop (ie an object containing one or more displays).
 *	Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WDesktop.cpp,v $
 *		Revision 1.3  2001/04/21 03:30:57  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 12:06:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XDesktop.h>

namespace Whisper {


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
	mDisplay = 1;
}
				

//---------------------------------------------------------------
//
// XDesktop::iterator::iterator (OSDisplay)
//
//---------------------------------------------------------------
XDesktop::iterator::iterator(OSDisplay display)
{
	PRECONDITION(display == 0);
	
	mDisplay = display;
}

	
//---------------------------------------------------------------
//
// XDesktop::iterator::operator++
//
//---------------------------------------------------------------
XDesktop::iterator& XDesktop::iterator::operator++()
{
	if (mDisplay != 1)
		++mDisplay;
	
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

#if __MWERKS__
#pragma mark -
#endif

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
	return 0;
}


//---------------------------------------------------------------
//
// XDesktop::GetMaxDisplay								[static]
//
//---------------------------------------------------------------
XDisplay XDesktop::GetMaxDisplay(const XRect& globalRect)
{
	XDisplay display(0);
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
	XDisplay display(0);
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
	XDisplay display(0);
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
	return iterator(0);
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
