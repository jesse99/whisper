/*
 *  File:       WRegion.cpp
 *  Summary:   	Cross platform region class.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WRegion.cpp,v $
 *		Revision 1.3  2001/04/21 03:33:25  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 12:11:15  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XRegion.h>

#include <XDebug.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	Friend Functions
// ===================================================================================

//---------------------------------------------------------------
//
// operator+ (XRegion, XRegion)
//
//---------------------------------------------------------------
XRegion operator+(const XRegion& lhs, const XRegion& rhs)
{
	XRegion newRgn;
	
	int type = ::CombineRgn(newRgn.mRgn, lhs.mRgn, rhs.mRgn, RGN_OR);
	ThrowIf(type == ERROR);

	return newRgn;
}


//---------------------------------------------------------------
//
// XRegion::operator+ (XRegion, XPoint)
//
//---------------------------------------------------------------
XRegion operator+(const XRegion& rgn, const XPoint& offset)
{
	XRegion newRgn = rgn;
	
	int type = ::OffsetRgn(newRgn.mRgn, offset.x, offset.y);
	ThrowIf(type == ERROR);

	return newRgn;
}


//---------------------------------------------------------------
//
// operator- (XRegion, XRegion)
//
//---------------------------------------------------------------
XRegion operator-(const XRegion& lhs, const XRegion& rhs)
{
	XRegion newRgn;
	
	int type = ::CombineRgn(newRgn.mRgn, lhs.mRgn, rhs.mRgn, RGN_DIFF);
	ThrowIf(type == ERROR);

	return newRgn;
}


//---------------------------------------------------------------
//
// XRegion::operator- (XRegion, XPoint)
//
//---------------------------------------------------------------
XRegion operator-(const XRegion& rgn, const XPoint& offset)
{
	XRegion newRgn = rgn;
	
	int type = ::OffsetRgn(newRgn.mRgn, -offset.x, -offset.y);
	ThrowIf(type == ERROR);

	return newRgn;
}


//---------------------------------------------------------------
//
// operator& (XRegion, XRegion)
//
//---------------------------------------------------------------
XRegion operator&(const XRegion& lhs, const XRegion& rhs)
{
	XRegion newRgn;
	
	int type = ::CombineRgn(newRgn.mRgn, lhs.mRgn, rhs.mRgn, RGN_AND);
	ThrowIf(type == ERROR);

	return newRgn;
}


//---------------------------------------------------------------
//
// operator^ (XRegion, XRegion)	SIMPLEREGION
//
//---------------------------------------------------------------
XRegion operator^(const XRegion& lhs, const XRegion& rhs)
{
	XRegion newRgn;
	
	int type = ::CombineRgn(newRgn.mRgn, lhs.mRgn, rhs.mRgn, RGN_XOR);
	ThrowIf(type == ERROR);

	return newRgn;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XRegion
// ===================================================================================

//---------------------------------------------------------------
//
// Destructor
//
//---------------------------------------------------------------
XRegion::~XRegion()
{	
	VERIFY(::DeleteObject(mRgn));
	mRgn = nil;
}
	
	
//---------------------------------------------------------------
//
// XRegion::XRegion ()
//
//---------------------------------------------------------------
XRegion::XRegion()
{
	mRgn = ::CreateRectRgn(0, 0, 0, 0);
	ThrowIfBadHandle(mRgn);
}


//---------------------------------------------------------------
//
// XRegion::XRegion (XRect)
//
//---------------------------------------------------------------
XRegion::XRegion(const XRect& rect)
{
	mRgn = ::CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
	ThrowIfBadHandle(mRgn);
}


//---------------------------------------------------------------
//
// XRegion::XRegion (XRect*, uint32)
//
//---------------------------------------------------------------
XRegion::XRegion(const XRect* rects, uint32 count)
{
	PRECONDITION(rects != nil);
	PRECONDITION(count < 100*1024L);
	
	mRgn = ::CreateRectRgn(0, 0, 0, 0);
	ThrowIfBadHandle(mRgn);

	HRGN temp = ::CreateRectRgn(0, 0, 0, 0);
	ThrowIfBadHandle(temp);

	for (uint32 index = 0; index < count; ++index) {
		XRect rect = rects[index];
		int32 succeeded = ::SetRectRgn(temp, rect.left, rect.top, rect.right, rect.bottom);
		ThrowIf(!succeeded);

		int type = ::CombineRgn(mRgn, mRgn, temp, RGN_OR);
		ThrowIf(type == ERROR);
	}
		
	VERIFY(::DeleteObject(temp));
}


//---------------------------------------------------------------
//
// XRegion::XRegion (XRect, int32, int32)
//
//---------------------------------------------------------------
XRegion::XRegion(const XRect& rect, int32 ovalWidth, int32 ovalHeight)
{
	PRECONDITION(ovalWidth >= 0);
	PRECONDITION(ovalHeight >= 0);
		
	mRgn = ::CreateRoundRectRgn(rect.left, rect.top, rect.right, rect.bottom, ovalWidth, ovalHeight);
	ThrowIfBadHandle(mRgn);
}


//---------------------------------------------------------------
//
// XRegion::XRegion (XPoint, int32, int32)
//
//---------------------------------------------------------------
XRegion::XRegion(const XPoint& center, int32 width, int32 height)
{
	PRECONDITION(width >= 0);
	PRECONDITION(height >= 0);
	
	XRect rect;
	rect.left   = center.x - width/2;
	rect.top    = center.y - height/2;
	rect.right  = rect.left + width;
	rect.bottom = rect.top + height;
	
	mRgn = ::CreateEllipticRgn(rect.left, rect.top, rect.right, rect.bottom);
	ThrowIfBadHandle(mRgn);
}


//---------------------------------------------------------------
//
// XRegion::XRegion (XPoint*, uint32)
//
//---------------------------------------------------------------
XRegion::XRegion(const XPoint* vertices, uint32 count)
{
	PRECONDITION(vertices != nil);
	PRECONDITION(count < 100*1024L);
	
	mRgn = ::CreatePolygonRgn((POINT*) vertices, (int32) count, WINDING);
	ThrowIfBadHandle(mRgn);
}


//---------------------------------------------------------------
//
// XRegion::XRegion (HRGN)
//
//---------------------------------------------------------------
XRegion::XRegion(HRGN rgn)
{
	PRECONDITION(rgn != nil);
	
	mRgn = ::CreateRectRgn(0, 0, 0, 0);
	ThrowIfBadHandle(mRgn);

	int type = ::CombineRgn(mRgn, mRgn, rgn, RGN_OR);
	ThrowIf(type == ERROR);
}


//---------------------------------------------------------------
//
// XRegion::XRegion (XRegion)
//
//---------------------------------------------------------------
XRegion::XRegion(const XRegion& rhs)
{
	mRgn = ::CreateRectRgn(0, 0, 0, 0);
	ThrowIfBadHandle(mRgn);

	int type = ::CombineRgn(mRgn, mRgn, rhs.mRgn, RGN_OR);
	ThrowIf(type == ERROR);
}


//---------------------------------------------------------------
//
// XRegion::operator=
//
//---------------------------------------------------------------
XRegion& XRegion::operator=(const XRegion& rhs)
{
	if (this != &rhs && *this != rhs) {
		HRGN temp = ::CreateRectRgn(0, 0, 0, 0);
		ThrowIfBadHandle(temp);

		int type = ::CombineRgn(temp, temp, rhs.mRgn, RGN_OR);
		ThrowIf(type == ERROR);
	
		VERIFY(::DeleteObject(mRgn));
		mRgn = temp;
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XRegion::operator+= (XRegion)
//
//---------------------------------------------------------------
XRegion& XRegion::operator+=(const XRegion& rhs)
{	
	int type = ::CombineRgn(mRgn, mRgn, rhs.mRgn, RGN_OR);
	ThrowIf(type == ERROR);

	return *this;
}


//---------------------------------------------------------------
//
// XRegion::operator+= (XPoint)
//
//---------------------------------------------------------------
XRegion& XRegion::operator+=(const XPoint& offset)
{	
	int type = ::OffsetRgn(mRgn, offset.x, offset.y);
	ThrowIf(type == ERROR);

	return *this;
}


//---------------------------------------------------------------
//
// XRegion::operator-= (XRegion)
//
//---------------------------------------------------------------
XRegion& XRegion::operator-=(const XRegion& rhs)
{
	int type = ::CombineRgn(mRgn, mRgn, rhs.mRgn, RGN_DIFF);
	ThrowIf(type == ERROR);

	return *this;
}


//---------------------------------------------------------------
//
// XRegion::operator-= (XPoint)
//
//---------------------------------------------------------------
XRegion& XRegion::operator-=(const XPoint& offset)
{
	int type = ::OffsetRgn(mRgn, -offset.x, -offset.y);
	ThrowIf(type == ERROR);

	return *this;
}


//---------------------------------------------------------------
//
// XRegion::operator&=
//
//---------------------------------------------------------------
XRegion& XRegion::operator&=(const XRegion& rhs)
{
	int type = ::CombineRgn(mRgn, mRgn, rhs.mRgn, RGN_AND);
	ThrowIf(type == ERROR);

	return *this;
}


//---------------------------------------------------------------
//
// XRegion::operator^=
//
//---------------------------------------------------------------
XRegion& XRegion::operator^=(const XRegion& rhs)
{
	int type = ::CombineRgn(mRgn, mRgn, rhs.mRgn, RGN_XOR);
	ThrowIf(type == ERROR);

	return *this;
}


//---------------------------------------------------------------
//
// XRegion::GetEnclosingRect
//
//---------------------------------------------------------------
XRect XRegion::GetEnclosingRect() const						
{
	XRect rect;
	
	int type = ::GetRgnBox(mRgn, rect);
	ThrowIf(type == ERROR);
	
	return rect;
}

					
//---------------------------------------------------------------
//
// XRegion::MakeEmpty
//
//---------------------------------------------------------------
void XRegion::MakeEmpty()
{
	int32 succeeded = ::SetRectRgn(mRgn, 0, 0, 0, 0);
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XRegion::MoveTo
//
//---------------------------------------------------------------
void XRegion::MoveTo(const XPoint& pt)
{
	XRect bounds = this->GetEnclosingRect();
	XPoint offset(pt - bounds[topLeft]);
	
	int type = ::OffsetRgn(mRgn, offset.x, offset.y);
	ThrowIf(type == ERROR);
}


//---------------------------------------------------------------
//
// XRegion::Contains (XRect)
//
//---------------------------------------------------------------
bool XRegion::Contains(const XRect& rect) const				
{
	XRegion rhs(rect);
	
	XRegion intersection = *this & rhs;
	
	bool contains = rhs == intersection;
	
	return contains;
}


}	// namespace Whisper


