/*
 *  File:       XRect.cpp
 *  Summary:	Rectangle class
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XRect.cpp,v $
 *		Revision 1.5  2001/04/27 04:35:35  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/03/02 11:25:01  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/12/10 04:11:09  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:48:12  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XRect.h>

#include <XNumbers.h>
#include <XTextConversions.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const XRect kZeroRect(0, 0, 0, 0);


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// FromStr (wstring, XRect)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& inStr, XRect& value)
{
	FromStr(inStr, value.left);
	
	std::wstring str = After(inStr, L" ");
	FromStr(str, value.top);
	
	str = After(str, L" ");
	FromStr(str, value.right);
	
	str = After(str, L" ");
	FromStr(str, value.bottom);
}


//---------------------------------------------------------------
//
// ToStr (XRect)
//
//---------------------------------------------------------------
std::wstring ToStr(const XRect& value)
{
	return ToStr(value.left) + L" " + ToStr(value.top) + L" " + ToStr(value.right) + L" " + ToStr(value.bottom);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XRect
// ===================================================================================

//---------------------------------------------------------------
//
// XRect::GetOSRect
//
//---------------------------------------------------------------
#if MAC
Rect XRect::GetOSRect() const
{
	Rect rect;

	PRECONDITION(this->IsValid()); 

	rect.left   = numeric_cast<int16>(left);
	rect.right  = numeric_cast<int16>(right);
	rect.top    = numeric_cast<int16>(top);
	rect.bottom = numeric_cast<int16>(bottom);

	return rect;
}
#endif


//---------------------------------------------------------------
//
// XRect::operator|
//
//---------------------------------------------------------------
XRect XRect::operator|(const XRect& rect) const
{
	PRECONDITION(this->IsValid()); 

	XRect returnRect;

	if (this->IsEmpty())
		returnRect = rect;
		
	else if (rect.IsEmpty())
		returnRect = *this;
		
	else {
		returnRect.top    = Min(top, rect.top);
		returnRect.left   = Min(left, rect.left);
		returnRect.bottom = Max(bottom, rect.bottom);
		returnRect.right  = Max(right, rect.right);
	}
	
	return returnRect; 
}


//---------------------------------------------------------------
//
// XRect::operator&
//
//---------------------------------------------------------------
XRect XRect::operator&(const XRect& rect) const
{
	PRECONDITION(this->IsValid()); 

	XRect returnRect;
	
	returnRect.top    = Max(top, rect.top);
	returnRect.left   = Max(left, rect.left);
	returnRect.bottom = Min(bottom, rect.bottom);
	returnRect.right  = Min(right, rect.right);
	
	if (returnRect.left > returnRect.right || returnRect.top > returnRect.bottom)
		returnRect = kZeroRect;
	
	return returnRect;
} 


//---------------------------------------------------------------
//
// XRect::MapTo
//
//---------------------------------------------------------------
void XRect::MapTo(const XRect& container, double maxScaleFactor)
{	
	PRECONDITION(maxScaleFactor > 0.0);
	PRECONDITION(this->IsValid()); 
	
	int32 srcHeight = this->GetHeight();
	int32 srcWidth  = this->GetWidth();

	int32 destHeight = container.GetHeight();
	int32 destWidth  = container.GetWidth();

	double	heightScale	= (double) destHeight/srcHeight;
	double	widthScale	= (double) destWidth/srcWidth;
	double	scale		= (widthScale < heightScale) ? widthScale : heightScale;
	double	maxScale	= STD::sqrt(maxScaleFactor);

	if (scale > maxScale)
		scale = maxScale;

	int32 aspectHeight = (int32) (srcHeight*scale + 0.5);
	int32 aspectWidth  = (int32) (srcWidth*scale + 0.5);

	ASSERT(aspectHeight <= destHeight);
	ASSERT(aspectWidth <= destWidth);

	top	 = (container.top + container.bottom - aspectHeight)/2;
	left = (container.left + container.right - aspectWidth)/2;

	bottom = top + aspectHeight;
	right  = left + aspectWidth;
}


//---------------------------------------------------------------
//
// XRect::Pin
//
//---------------------------------------------------------------
XPoint XRect::Pin(const XPoint& pt) const
{
	PRECONDITION(this->IsValid()); 

	XPoint result = pt;
	
	if (result.x < left)
		result.x = left;
	else if (result.x > right)
		result.x = right;
	
	if (result.y < top)
		result.y = top;
	else if (result.y > bottom)
		result.y = bottom;
	
	return result;
}


}	// namespace Whisper

