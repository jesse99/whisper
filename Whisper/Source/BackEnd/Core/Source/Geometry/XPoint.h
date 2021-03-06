/*
 *  File:       XPoint.h
 *  Summary:	Point class
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997--2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XPoint.h,v $
 *		Revision 1.6  2001/03/15 09:03:32  jesjones
 *		Exported the friend functions.
 *		
 *		Revision 1.5  2001/03/03 11:16:02  jesjones
 *		Instead of specializing selected algorithms for POD types, we use the new MSL mechanism to declare POD types.
 *		
 *		Revision 1.4  2001/03/02 11:24:52  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/12/10 08:54:27  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:47:00  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XSize.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Constants
//
extern CORE_EXPORT const XPoint kZeroPt;


// ===================================================================================
//	class XPoint
//!		2D point class
// ===================================================================================
class CORE_EXPORT XPoint {

//-----------------------------------
//	Initialization/Destruction
//
public:
						XPoint()								{}
						XPoint(int32 inX, int32 inY)			{x = inX; y = inY;}
	explicit			XPoint(const XSize& size)				{x = size.width; y = size.height;}

#if MAC
						XPoint(const Point& rhs)				{x = rhs.h; y = rhs.v;}
#elif WIN
						XPoint(const POINT& rhs)				{x = rhs.x; y = rhs.y;}
#endif
						
			void 		Set(int32 inX, int32 inY)				{x = inX; y = inY;}

//-----------------------------------
//	API
//
public:
	//! @name Conversion Operators
	//@{
#if MAC
			OSPoint 	GetOSPoint() const;
#elif WIN
						operator POINT() const					{return *(const POINT *) this;}
						operator POINT*()						{return (POINT *) this;}

			OSPoint 	GetOSPoint() const						{return *(POINT *) this;}
#endif
	//@}

	//! @name Arithmetic operators
	//@{
			XPoint 		operator-() const						{return XPoint(-x, -y);}
	
			XPoint 		operator+(const XPoint& rhs) const		{return XPoint(x + rhs.x, y + rhs.y);}
			XPoint 		operator+(const XSize& rhs) const		{return XPoint(x + rhs.width, y + rhs.height);}
			XPoint 		operator-(const XPoint& rhs) const		{return XPoint(x - rhs.x, y - rhs.y);}
			XPoint 		operator-(const XSize& rhs) const		{return XPoint(x - rhs.width, y - rhs.height);}

			XPoint 		operator*(int32 num) const				{return XPoint(x*num, y*num);}
	friend 	XPoint 		operator*(int32 num, const XPoint& rhs)	{return XPoint(rhs.x*num, rhs.y*num);}
			XPoint 		operator/(int32 num) const				{ASSERT(num != 0); return XPoint(x/num, y/num);}

			XPoint& 	operator+=(const XPoint& rhs);	
			XPoint&	 	operator+=(const XSize& rhs);	
			XPoint& 	operator-=(const XPoint& rhs);	
			XPoint& 	operator-=(const XSize& rhs);	
					
			XPoint& 	operator*=(int32 num)					{x *= num; y *= num; return *this;}
			XPoint&	 	operator/=(int32 num)					{ASSERT(num != 0); x /= num; y /= num; return *this;}
	//@}

	//! @name Relational Operators
	//@{
			bool 		operator==(const XPoint& rhs) const		{return x == rhs.x && y == rhs.y;}
			bool 		operator!=(const XPoint& rhs) const		{return x != rhs.x || y != rhs.y;}

			bool 		operator<(const XPoint& rhs) const		{return y < rhs.y || (y == rhs.y && x <  rhs.x);}
			bool 		operator<=(const XPoint& rhs) const		{return y < rhs.y || (y == rhs.y && x <= rhs.x);}

			bool 		operator>(const XPoint& rhs) const		{return y > rhs.y || (y == rhs.y && x >  rhs.x);}
			bool 		operator>=(const XPoint& rhs) const		{return y > rhs.y || (y == rhs.y && x >= rhs.x);}
	//@}

	//! @name Misc
	//@{
	friend	int32 		Distance(const XPoint& pt1, const XPoint& pt2)			{return (int32) STD::sqrt((double) (pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y));}
	friend	int32 		DistanceSquared(const XPoint& pt1, const XPoint& pt2)	{return (pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y);}
	
	friend 	CORE_EXPORT void 		 FromStr(const std::wstring& str, XPoint& value);
	friend 	CORE_EXPORT std::wstring ToStr(const XPoint& value);
	//@}

//-----------------------------------
//	Member Data
//
public:
	int32 x;
	int32 y;
};


// ===================================================================================
//	Inlines
// ===================================================================================
inline XPoint& XPoint::operator+=(const XPoint& rhs)	
{
	x += rhs.x; 
	y += rhs.y; 
	
	return *this;
}

inline XPoint& XPoint::operator-=(const XPoint& rhs)	
{
	x -= rhs.x; 
	y -= rhs.y; 
	
	return *this;
}

inline XPoint& XPoint::operator+=(const XSize& rhs)
{
	x += rhs.width; 
	y += rhs.height; 
	
	return *this;
}
	
inline XPoint& XPoint::operator-=(const XSize& rhs)
{
	x -= rhs.width; 
	y -= rhs.height; 
	
	return *this;
}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper


DECLARE_POD(Whisper::XPoint)

