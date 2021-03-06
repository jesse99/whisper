/*
 *  File:       XRect.h
 *  Summary:	Rectangle class
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XRect.h,v $
 *		Revision 1.6  2001/03/15 09:03:38  jesjones
 *		Exported the friend functions.
 *		
 *		Revision 1.5  2001/03/03 11:16:07  jesjones
 *		Instead of specializing selected algorithms for POD types, we use the new MSL mechanism to declare POD types.
 *		
 *		Revision 1.4  2001/03/02 11:25:05  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/12/10 08:54:54  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:48:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XPoint.h>
#include <XSize.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XRect;


//-----------------------------------
//	Constants
//
enum PointSelector {topLeft, botRight};

extern CORE_EXPORT const XRect kZeroRect;


// ===================================================================================
//	class XRect
//!		2D Rectangle class
// ===================================================================================
class CORE_EXPORT XRect {

//-----------------------------------
//	Initialization/Destruction
//
public:
						XRect()															{}
						XRect(int32 inLeft, int32 inTop, int32 inRight, int32 inBottom)	{left = inLeft; top = inTop; right = inRight; bottom = inBottom;}
						XRect(const XPoint& topLeftPt, const XPoint& botRightPt);
						XRect(const XPoint& topLeftPt, const XSize& size);
						
#if MAC
						XRect(const Rect& rect);
#elif WIN
						XRect(const RECT& rect);
#endif

			void 		Set(int32 inLeft, int32 inTop, int32 inRight, int32 inBottom)	{left = inLeft; top = inTop; right = inRight; bottom = inBottom;}

//-----------------------------------
//	API
//
public:
	//! @name Conversion Operators
	//@{
#if MAC
			OSRect 		GetOSRect() const;

#elif WIN
						operator RECT() const					{return *(RECT *) this;}
						operator const RECT*() const			{return (const RECT *) this;}
						operator RECT*()						{return (RECT *) this;}

			OSRect 		GetOSRect() const						{return *(RECT *) this;}
#endif
	//@}

	//! @name Dimensions
	//@{
			int32		GetWidth() const						{ASSERT(this->IsValid()); return right - left;}
			int32 		GetHeight() const						{ASSERT(this->IsValid()); return bottom - top;}
			XSize 		GetSize() const							{ASSERT(this->IsValid()); return XSize(right - left, bottom - top);}
			int32	 	GetArea() const							{ASSERT(this->IsValid()); return (right - left) * (bottom - top);}
	//@}

	//! @name Selectors
	//@{
			XPoint& 	  operator[](PointSelector sel)			{return sel == topLeft ? (*((XPoint *)&left)) : (*((XPoint *)&right));}			
			const XPoint& operator[](PointSelector sel) const	{return sel == topLeft ? (*((XPoint *)&left)) : (*((XPoint *)&right));}	
	//@}

	//! @name Operations
	//@{
			void 		MoveTo(const XPoint& pt);
			void 		MoveBy(const XPoint& pt);
			XRect 		operator+(const XPoint& rhs) const		{return XRect(left + rhs.x, top + rhs.y, right + rhs.x, bottom + rhs.y);}
			XRect 		operator+(const XSize& rhs) const		{return XRect(left + rhs.width, top + rhs.height, right + rhs.width, bottom + rhs.height);}
			XRect 		operator-(const XPoint& rhs) const		{return XRect(left - rhs.x, top - rhs.y, right - rhs.x, bottom - rhs.y);}
			XRect 		operator-(const XSize& rhs) const		{return XRect(left - rhs.width, top - rhs.height, right - rhs.width, bottom - rhs.height);}
					
			XRect 		operator&(const XRect& rhs) const;		// intersection
			XRect 		operator|(const XRect& rhs) const;		// union
		
			XRect& 		operator+=(const XPoint& rhs);
			XRect& 		operator+=(const XSize& rhs);
			XRect& 		operator-=(const XPoint& rhs);
			XRect& 		operator-=(const XSize& rhs);
			
			XRect& 		operator&=(const XRect& rhs);	
			XRect& 		operator|=(const XRect& rhs);
		
			void 		Inset(const XPoint& delta);
			void 		Inset(int32 dx, int32 dy);
		
			void 		SetWidth(int32 size)					{right = left + size;}
			void 		SetHeight(int32 size)					{bottom = top + size;}
			void 		SetSize(const XSize& size)				{right = left + size.width; bottom = top + size.height;}
	//@}
			
	//! @name Relational Operators
	//@{
			bool 		operator==(const XRect& rhs) const		{return left == rhs.left && right == rhs.right && top == rhs.top && bottom == rhs.bottom;}
			bool 		operator!=(const XRect& rhs) const		{return !(*this == rhs);}
	//@}

	//! @name Misc
	//@{
			bool 		Contains(const XPoint& pt) const		{ASSERT(this->IsValid()); return pt.y >= top && pt.y < bottom && pt.x >= left && pt.x < right;}
						/**< Returns true if pt is within the rectangle. */
		
			bool 		Contains(const XRect& rect) const		{ASSERT(this->IsValid()); return rect.top >= top && rect.bottom <= bottom && rect.left >= left && rect.right <= right;}
		
			bool 		Intersects(const XRect& rect) const		{ASSERT(this->IsValid()); return left <= rect.right && right > rect.left && top <= rect.bottom && bottom > rect.top;}
						/**< Return true if the rectangle intersects rect. */
						
			bool 		IsValid() const							{return right >= left && bottom >= top;}
		
			bool 		IsEmpty() const							{return left >= right || top >= bottom;}
						/**< Returns true if width or height are zero. */
		
			void 		MakeEmpty()								{left = right = top = bottom = 0;}
						/**< Empties the rectangle. */
		
			void 		CenterIn(const XRect& container);
						/**< Center the rectangle in the container. */
						
			XPoint 		GetCenter() const						{ASSERT(this->IsValid()); return XPoint((left + right)/2, (top + bottom)/2);}
				
			void 		MapTo(const XRect& container, double maxScaleFactor = 1.0);
						/**< Scales the rectangle so that it's within the container and
						its size is as close as possible to the container's size. 
						Note that this preserves the rectangle's aspect ratio and 
						won't blow up the rectangle by an amount larger than 
						maxScaleFactor. */
				
			XPoint 		Pin(const XPoint& pt) const;
						/**< Returns the point within the rectangle closest to pt. */
	
	friend 	CORE_EXPORT void 			FromStr(const std::wstring& str, XRect& value);
	friend 	CORE_EXPORT std::wstring	ToStr(const XRect& value);
	//@}

//-----------------------------------
//	Member Data
//
public:
    int32    left;		// Windows ordering...
    int32    top;
    int32    right;
    int32    bottom;
};


// ===================================================================================
//	Inlines
// ===================================================================================
inline XRect::XRect(const XPoint& topLeftPt, const XPoint& botRightPt)
{
	left	= topLeftPt.x;
	top		= topLeftPt.y;
	right	= botRightPt.x;
	bottom	= botRightPt.y;
}

inline XRect::XRect(const XPoint& topLeftPt, const XSize& size)
{
	left	= topLeftPt.x;
	top		= topLeftPt.y;
	right	= topLeftPt.x + size.width;
	bottom	= topLeftPt.y + size.height;
}

#if MAC
	inline XRect::XRect(const Rect& rect)
	{
		left	= rect.left;
		top		= rect.top;
		right	= rect.right;
		bottom	= rect.bottom;
	}
#elif WIN
	inline XRect::XRect(const RECT& rect)
	{
		left	= rect.left;
		top		= rect.top;
		right	= rect.right;
		bottom	= rect.bottom;
	}
#endif

inline void XRect::MoveBy(const XPoint& pt)
{
	left 	+= pt.x; 
	top 	+= pt.y; 
	right 	+= pt.x; 
	bottom 	+= pt.y; 
}

inline void XRect::MoveTo(const XPoint& pt)				
{
	this->MoveBy(XPoint(pt.x - left, pt.y - top));
}

inline void XRect::Inset(int32 dx, int32 dy)
{
	top 	+= dy; 
	left 	+= dx; 
	bottom 	-= dy; 
	right 	-= dx;
}
 
inline void XRect::Inset(const XPoint& delta)				
{
	this->Inset(delta.x, delta.y);
}

inline XRect& XRect::operator&=(const XRect& rhs)
{
	*this = *this & rhs; 
	return *this;
}
	
inline XRect& XRect::operator|=(const XRect& rhs)
{
	*this = *this | rhs; 
	return *this;
}

inline XRect& XRect::operator+=(const XPoint& rhs)
{
	this->MoveBy(rhs); 
	return *this;
}

inline XRect& XRect::operator-=(const XPoint& rhs)
{
	this->MoveBy(-rhs); 
	return *this;
}

inline XRect& XRect::operator+=(const XSize& rhs)
{
	this->MoveBy(XPoint(rhs.width, rhs.height)); 
	return *this;
}

inline XRect& XRect::operator-=(const XSize& rhs)
{
	this->MoveBy(XPoint(-rhs.width, -rhs.height)); 
	return *this;
}

inline void XRect::CenterIn(const XRect& container)		
{
	ASSERT(this->IsValid()); 

	this->MoveTo(container[topLeft] + (container.GetSize() - GetSize()) / 2);
}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper


DECLARE_POD(Whisper::XRect)


