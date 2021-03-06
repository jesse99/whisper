/*
 *  File:       Z2DPrimitives.cpp
 *  Summary:	2D geometry primitives for QuickDraw 3D.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X2DPrimitives.cpp,v $
 *		Revision 1.5  2001/04/21 03:34:55  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/03/02 11:23:13  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/11/20 05:43:47  jesjones
 *		Added std:: to a few function calls.
 *		
 *		Revision 1.2  2000/11/09 12:13:24  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	  1/31/98	JDJ		Added X2DSize.
 *		 <4>	  1/30/98	JDJ		Added streaming operators.
 *		 <3>	 10/21/97	JDJ		Scalar ops use doubles instead of floats.
 *		 <2>	  4/15/97	JDJ		Made Equal a friend function.
 *		 <1>	  2/14/97	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <X2DPrimitives.h>

#include <cmath>

#include <XNumbers.h>
#include <XStreaming.h>
#include <XTextConversions.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const X2DSize	kZero2DSize(0.0f, 0.0f);
const X2DPoint	kZero2DPt(0.0f, 0.0f);
const X2DRect 	kZero2DRect(0.0f, 0.0f, 0.0f, 0.0f);


// ===================================================================================
//	class X2DSize
// ===================================================================================

//---------------------------------------------------------------
//
// Equal (X2DSize, X2DSize, double)
//
//---------------------------------------------------------------
bool Equal(const X2DSize& lhs, const X2DSize& rhs, double tolerance)
{	
	bool equal = Equal(lhs.width, rhs.width, tolerance) && Equal(lhs.height, rhs.height, tolerance);
	
	return equal;
}


//---------------------------------------------------------------
//
// operator>> (X2DSize)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, X2DSize& rhs)
{
	stream >> rhs.width >> rhs.height;
	
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (X2DSize)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const X2DSize& rhs)
{
	stream << rhs.width << rhs.height;
	
	return stream;
}


//---------------------------------------------------------------
//
// FromStr (wstring, X2DSize)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, X2DSize& value)
{
	FromStr(str, value.width);
	FromStr(After(str, L" "), value.height);
}


//---------------------------------------------------------------
//
// ToStr (X2DSize)
//
//---------------------------------------------------------------
std::wstring ToStr(const X2DSize& value)
{
	return ToStr(value.width) + L" " + ToStr(value.height);
}

#pragma mark -

// ===================================================================================
//	class X2DPoint
// ===================================================================================

//---------------------------------------------------------------
//
// Distance (X2DPoint, X2DPoint)
//
//---------------------------------------------------------------
double Distance(const X2DPoint& lhs, const X2DPoint& rhs)
{
	double dx = (double) lhs.x - rhs.x;
	double dy = (double) lhs.y - rhs.y;
	
	double dist = std::sqrt(dx*dx + dy*dy);
	
	return dist;
}


//---------------------------------------------------------------
//
// DistanceSquared (X2DPoint, X2DPoint)
//
//---------------------------------------------------------------
double DistanceSquared(const X2DPoint& lhs, const X2DPoint& rhs)
{
	double dx = (double) lhs.x - rhs.x;
	double dy = (double) lhs.y - rhs.y;
	
	double dist = dx*dx + dy*dy;
	
	return dist;
}


//---------------------------------------------------------------
//
// Equal (X2DPoint, X2DPoint, double)
//
//---------------------------------------------------------------
bool Equal(const X2DPoint& lhs, const X2DPoint& rhs, double tolerance)
{	
	bool equal = Equal(lhs.x, rhs.x, tolerance) && Equal(lhs.y, rhs.y, tolerance);
	
	return equal;
}


//---------------------------------------------------------------
//
// operator>> (X2DPoint)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, X2DPoint& rhs)
{
	stream >> rhs.x >> rhs.y;
	
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (X2DPoint)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const X2DPoint& rhs)
{
	stream << rhs.x << rhs.y;
	
	return stream;
}


//---------------------------------------------------------------
//
// FromStr (wstring, X2DPoint)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, X2DPoint& value)
{
	FromStr(str, value.x);
	FromStr(After(str, L" "), value.y);
}


//---------------------------------------------------------------
//
// ToStr (X2DPoint)
//
//---------------------------------------------------------------
std::wstring ToStr(const X2DPoint& value)
{
	return ToStr(value.x) + L" " + ToStr(value.y);
}

#pragma mark -

// ===================================================================================
//	class X2DRect
// ===================================================================================

//---------------------------------------------------------------
//
// X2DRect::operator|
//
//---------------------------------------------------------------
X2DRect X2DRect::operator|(const X2DRect& rect) const
{
	X2DRect returnRect;

	if (this->IsEmpty())
		returnRect = rect;
		
	else if (rect.IsEmpty())
		returnRect = *this;
		
	else {
		returnRect.top = Min(top, rect.top);
		returnRect.left = Min(left, rect.left);
		returnRect.bottom = Max(bottom, rect.bottom);
		returnRect.right = Max(right, rect.right);
	}
	
	return returnRect; 
}


//---------------------------------------------------------------
//
// X2DRect::operator&
//
//---------------------------------------------------------------
X2DRect X2DRect::operator&(const X2DRect& rect) const
{
	X2DRect returnRect;
	
	returnRect.top = Max(top, rect.top);
	returnRect.left = Max(left, rect.left);
	returnRect.bottom = Min(bottom, rect.bottom);
	returnRect.right = Min(right, rect.right);
	
	if (left > right || top > bottom)
		returnRect = kZero2DRect;
	
	return returnRect;
} 


//---------------------------------------------------------------
//
// X2DRect::CenterIn
//
//---------------------------------------------------------------
void X2DRect::CenterIn(const X2DRect& container)		
{
	X2DPoint delta;
	delta.x = (float) (0.5*(container.GetWidth() - this->GetWidth()));
	delta.y = (float) (0.5*(container.GetHeight() - this->GetHeight()));

	this->MoveTo(container[topLeft] + delta);
}


//---------------------------------------------------------------
//
// X2DRect::MapTo
//
//---------------------------------------------------------------
void X2DRect::MapTo(const X2DRect& container, double maxScaleFactor)
{	
	PRECONDITION(maxScaleFactor > 0.0);
	
	double srcHeight = this->GetHeight();
	double srcWidth  = this->GetWidth();

	double destHeight = container.GetHeight();
	double destWidth  = container.GetWidth();

	double	heightScale	= (double) destHeight/srcHeight;
	double	widthScale	= (double) destWidth/srcWidth;
	double	scale		= (widthScale < heightScale) ? widthScale : heightScale;
	double	maxScale	= std::sqrt(maxScaleFactor);

	if (scale > maxScale)
		scale = maxScale;

	double aspectHeight = srcHeight*scale + 0.5;
	double aspectWidth  = srcWidth*scale + 0.5;

	ASSERT(aspectHeight <= destHeight);
	ASSERT(aspectWidth <= destWidth);

	top	 = (float) (0.5f*(container.top + container.bottom - aspectHeight));
	left = (float) (0.5f*(container.left + container.right - aspectWidth));

	bottom = (float) (top + aspectHeight);
	right  = (float) (left + aspectWidth);
}


//---------------------------------------------------------------
//
// X2DRect::Pin
//
//---------------------------------------------------------------
X2DPoint X2DRect::Pin(const X2DPoint& pt)
{
	X2DPoint result = pt;
	
	if (result.x < left)
		result.x = left;
	else if (result.x >= right)
		result.x = right - 1;
	
	if (result.y < top)
		result.y = top;
	else if (result.y >= bottom)
		result.y = bottom - 1;
	
	return result;
}


//---------------------------------------------------------------
//
// Equal (X2DRect, X2DRect, double)
//
//---------------------------------------------------------------
bool Equal(const X2DRect& lhs, const X2DRect& rhs, double tolerance)
{
	bool equal = Equal(lhs.left, rhs.left, tolerance) && Equal(lhs.top, rhs.top, tolerance) && Equal(lhs.right, rhs.right, tolerance) && Equal(lhs.bottom, rhs.bottom, tolerance);
	
	return equal;
}


//---------------------------------------------------------------
//
// operator>> (X2DRect)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, X2DRect& rhs)
{
	stream >> rhs.left >> rhs.right >> rhs.top >> rhs.bottom;
	
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (X2DRect)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const X2DRect& rhs)
{
	stream << rhs.left << rhs.right << rhs.top << rhs.bottom;
	
	return stream;
}


//---------------------------------------------------------------
//
// FromStr (wstring, X2DRect)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& inStr, X2DRect& value)
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
// ToStr (X2DRect)
//
//---------------------------------------------------------------
std::wstring ToStr(const X2DRect& value)
{
	return ToStr(value.left) + L" " + ToStr(value.top) + L" " + ToStr(value.right) + L" " + ToStr(value.bottom);
}


}	// namespace Whisper
