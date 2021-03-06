/*
 *  File:       Z2DPrimitives.h
 *  Summary:	2D geometry primitives for QuickDraw 3D.
 *  Written by: Jesse Jones
 *
 *	Classes:	X2DSize		- 2D size
 *				X2DPoint	- 2D point
 *				X2DRect		- 2D box
 *
 *  Copyright � 1996-1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X2DPrimitives.h,v $
 *		Revision 1.6  2001/04/14 07:27:13  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.5  2001/03/03 11:15:43  jesjones
 *		Instead of specializing selected algorithms for POD types, we use the new MSL mechanism to declare POD types.
 *		
 *		Revision 1.4  2001/03/02 11:23:17  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/12/13 08:50:16  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:13:32  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	  1/31/98	JDJ		Added X2DSize.
 *		 <4>	  1/30/98	JDJ		Added streaming operators.
 *		 <3>	 10/21/97	JDJ		Scalar ops use doubles instead of floats.
 *		 <2>	  4/15/97	JDJ		Made Equal a friend function.
 *		 <1>	  2/14/97	JDJ		Created.
 */

#pragma once

#include <Quesa.h>

#include <XDebug.h>
#include <XRect.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class X2DPoint;
class X2DSize;
class X2DRect;
class XInStream;
class XOutStream;
	
	
//-----------------------------------
//	Constants
//
extern QUESA_EXPORT const X2DSize	kZero2DSize;
extern QUESA_EXPORT const X2DPoint	kZero2DPt;
extern QUESA_EXPORT const X2DRect 	kZero2DRect;


// ===================================================================================
//	class X2DSize
//!		2D size object.
// ===================================================================================
class QUESA_EXPORT X2DSize {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X2DSize()									{}
						X2DSize(float inWidth, float inHeight)		{width = inWidth; height = inHeight;}
	explicit			X2DSize(const XPoint& rhs)					{width = rhs.x; height = rhs.y;}
	explicit			X2DSize(const TQ3Point2D& rhs)				{width = rhs.x; height = rhs.y;}

//-----------------------------------
//	API
//
public:
	//! @name Conversion operators
	//@{
						operator TQ3Point2D() const					{return *reinterpret_cast<const TQ3Point2D*>(this);}
						operator TQ3Point2D*()						{return reinterpret_cast<TQ3Point2D*>(this);}
						operator const TQ3Point2D*() const			{return reinterpret_cast<const TQ3Point2D*>(this);}
	//@}

	//! @name Arithmetic operators
	//@{
			X2DSize 	operator-() const							{return X2DSize(-width, -height);}
	
			X2DSize 	operator+(const X2DSize& rhs) const			{return X2DSize(width + rhs.width, height + rhs.height);}
			X2DSize 	operator-(const X2DSize& rhs) const			{return X2DSize(width - rhs.width, height - rhs.height);}	
		
			X2DSize 	operator*(double num) const					{return X2DSize((float) (width*num), (float) (height*num));}
	friend 	X2DSize 	operator*(double num, const X2DSize& rhs)	{return X2DSize((float) (rhs.width*num), (float) (rhs.height*num));}
			X2DSize 	operator/(double num) const					{ASSERT(num != 0); return X2DSize((float) (width/num), (float) (height/num));}

			X2DSize& 	operator+=(const X2DSize& rhs)				{width += rhs.width; height += rhs.height; return *this;}	
			X2DSize& 	operator-=(const X2DSize& rhs)				{width -= rhs.width; height -= rhs.height; return *this;}	
	//@}
		
	//! @name Relational Operators
	//@{
			bool 		operator==(const X2DSize& rhs) const		{return Equal(*this, rhs);}
			bool 		operator!=(const X2DSize& rhs) const		{return !Equal(*this, rhs);}

	friend	bool 		Equal(const X2DSize& lhs, const X2DSize& rhs, double tolerance = 1.0e-6);
	//@}

	//! @name Streaming
	//@{
	friend 	XInStream& 	operator>>(XInStream& stream, X2DSize& rhs);
	friend	XOutStream& operator<<(XOutStream& stream, const X2DSize& rhs);
	
	friend 	void 			FromStr(const std::wstring& str, X2DSize& value);
	friend 	std::wstring	ToStr(const X2DSize& value);
	//@}

	//! @name Misc
	//@{
			double 		GetArea() const								{return (double) width * height;}
	//@}

//-----------------------------------
//	Member Data
//
public:
	float width;
	float height;
};


// ===================================================================================
//	class X2DPoint
//@!	2D point object.
// ===================================================================================
class QUESA_EXPORT X2DPoint {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X2DPoint()									{}
						X2DPoint(float inX, float inY)				{x = inX; y = inY;}
						X2DPoint(const TQ3Point2D& rhs)				{x = rhs.x; y = rhs.y;}
	explicit			X2DPoint(const XPoint& rhs)					{x = rhs.x; y = rhs.y;}
	explicit			X2DPoint(const X2DSize& rhs)				{x = rhs.width; y = rhs.height;}
						
//-----------------------------------
//	API
//
public:
	//! @name Conversion operators
	//@{
						operator TQ3Point2D() const					{return *reinterpret_cast<const TQ3Point2D*>(this);}
						operator TQ3Point2D*()						{return reinterpret_cast<TQ3Point2D*>(this);}
						operator const TQ3Point2D*() const			{return reinterpret_cast<const TQ3Point2D*>(this);}
	//@}

	//! @name Arithmetic operators
	//@{
			X2DPoint 	operator-() const							{return X2DPoint(-x, -y);}
	
			X2DPoint 	operator+(const X2DPoint& rhs) const		{return X2DPoint(x + rhs.x, y + rhs.y);}
			X2DPoint 	operator+(const X2DSize& rhs) const			{return X2DPoint(x + rhs.width, y + rhs.height);}
			X2DPoint 	operator-(const X2DPoint& rhs) const		{return X2DPoint(x - rhs.x, y - rhs.y);}
			X2DPoint 	operator-(const X2DSize& rhs) const			{return X2DPoint(x - rhs.width, y - rhs.height);}

			X2DPoint 	operator*(double num) const					{return X2DPoint((float) (x*num), (float) (y*num));}
	friend 	X2DPoint 	operator*(double num, const X2DPoint& rhs)	{return X2DPoint((float) (rhs.x*num), (float) (rhs.y*num));}
			X2DPoint 	operator/(double num) const					{ASSERT(num != 0.0f); return X2DPoint((float) (x/num), (float) (y/num));}

			X2DPoint& 	operator+=(const X2DPoint& rhs)				{x += rhs.x; y += rhs.y; return *this;}
			X2DPoint& 	operator+=(const X2DSize& rhs)				{x += rhs.width; y += rhs.height; return *this;}
			X2DPoint& 	operator-=(const X2DPoint& rhs)				{x -= rhs.x; y -= rhs.y; return *this;}
			X2DPoint& 	operator-=(const X2DSize& rhs)				{x -= rhs.width; y -= rhs.height; return *this;}
	//@}
				
	//! @name Comparisons
	//@{
			bool 		operator==(const X2DPoint& rhs) const		{return Equal(*this, rhs);}
			bool 		operator!=(const X2DPoint& rhs) const		{return !Equal(*this, rhs);}

	friend	bool 		Equal(const X2DPoint& lhs, const X2DPoint& rhs, double tolerance = 1.0e-6);

	friend	double 		Distance(const X2DPoint& lhs, const X2DPoint& rhs);
	friend	double 		DistanceSquared(const X2DPoint& lhs, const X2DPoint& rhs);
	//@}

	//! @name Streaming
	//@{
	friend 	XInStream& 	operator>>(XInStream& stream, X2DPoint& rhs);
	friend	XOutStream& operator<<(XOutStream& stream, const X2DPoint& rhs);
	
	friend 	void 			FromStr(const std::wstring& str, X2DPoint& value);
	friend 	std::wstring	ToStr(const X2DPoint& value);
	//@}

//-----------------------------------
//	Member Data
//
public:
	float x;
	float y;
};


// ===================================================================================
//	class X2DRect
//!		2D rect object.
// ===================================================================================
class QUESA_EXPORT X2DRect {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X2DRect()									{}
						X2DRect(float inLeft, float inTop, float inRight, float inBottom)	{left = inLeft; top = inTop; right = inRight; bottom = inBottom;}
						X2DRect(const X2DPoint& topLeftPt, const X2DPoint& botRightPt);
						X2DRect(const X2DPoint& topLeftPt, const X2DSize& size)				{left = topLeftPt.x; top = topLeftPt.y; right = left + size.width; bottom = top + size.height;}
	explicit			X2DRect(const XRect& rect)					{left = rect.left; top = rect.top; right = rect.right; bottom = rect.bottom;}

//-----------------------------------
//	 API
//
public:
	//! @name Dimensions
	//@{
			double		GetWidth() const							{return right - left;}
			double 		GetHeight() const							{return bottom - top;}
			X2DSize	 	GetSize() const								{return X2DSize(right - left, bottom - top);}
			double	 	GetArea() const								{return (double) (right - left)*(bottom - top);}
	//@}

	//! @name Selectors
	//@{
			X2DPoint& 	operator[](PointSelector sel)				{return (sel == topLeft) ? (*((X2DPoint *)&left)) : (*((X2DPoint *)&right));}		
			const X2DPoint& operator[](PointSelector sel) const		{return (sel == topLeft) ? (*((const X2DPoint *)&left)) : (*((const X2DPoint *)&right));}
	//@}

	//! @name Operations
	//@{
			void 		MoveTo(const X2DPoint& pt);
			void 		MoveBy(const X2DPoint& pt);
			X2DRect 	operator+(const X2DPoint& rhs) const		{return X2DRect(left + rhs.x, top + rhs.y, right + rhs.x, bottom + rhs.y);}
			X2DRect 	operator-(const X2DPoint& rhs) const		{return X2DRect(left - rhs.x, top - rhs.y, right - rhs.x, bottom - rhs.y);}
					
			X2DRect 	operator&(const X2DRect& rhs) const;
			X2DRect 	operator|(const X2DRect& rhs) const;
		
			X2DRect& 	operator+=(const X2DPoint& rhs);
			X2DRect& 	operator-=(const X2DPoint& rhs);
			
			X2DRect& 	operator&=(const X2DRect& rhs);	
			X2DRect& 	operator|=(const X2DRect& rhs);
		
			void 		Inset(const X2DPoint& delta);
			void 		Inset(double dx, double dy);
		
			void 		SetSize(const X2DSize& size)				{right = left + size.width; bottom = top + size.height;}
	//@}
			
	//! @name Comparisons
	//@{
			bool 		operator==(const X2DRect& rhs) const		{return Equal(*this, rhs);}
			bool 		operator!=(const X2DRect& rhs) const		{return !Equal(*this, rhs);}

	friend	bool 		Equal(const X2DRect& lhs, const X2DRect& rhs, double tolerance = 1.0e-6);
	//@}

	//! @name Streaming
	//@{
	friend 	XInStream& 	operator>>(XInStream& stream, X2DRect& rhs);
	friend	XOutStream& operator<<(XOutStream& stream, const X2DRect& rhs);
	
	friend 	void 			FromStr(const std::wstring& str, X2DRect& value);
	friend 	std::wstring	ToStr(const X2DRect& value);
	//@}

	//! @name Misc
	//@{
			bool 		Contains(const X2DPoint& pt) const			{return pt.y >= top && pt.y < bottom && pt.x >= left && pt.x < right;}
						/**< Returns true if pt is within the rectangle. */
		
			bool 		Contains(const X2DRect& rect) const			{return rect.top >= top && rect.bottom <= bottom && rect.left >= left && rect.right <= right;}
		
			bool 		Intersects(const X2DRect& rect) const		{return left <= rect.right && right >= rect.left && top <= rect.bottom && bottom >= rect.top;}
						/**< Return true if the rectangle intersects rect. */
		
			bool 		IsEmpty() const								{return left >= right || top >= bottom;}
						/**< Returns true if width or height are zero. */
		
			void 		MakeEmpty()									{left = right = top = bottom = 0.0f;}
						/**< Empties the rectangle. */
		
			void 		CenterIn(const X2DRect& container);
						/**< Center the rectangle in the container. */
				
			void 		MapTo(const X2DRect& container, double maxScaleFactor = 1.0);
						/**< Scales the rectangle so that it's within the container and
						its size is as close as possible to the container's size. 
						Note that this preserves the rectangle's aspect ratio and 
						won't blow up the rectangle by an amount larger than 
						maxScaleFactor. */
				
			X2DPoint 	Pin(const X2DPoint& pt);
						/**< Returns the point within the rectangle closest to pt. */
	//@}

//-----------------------------------
//	Member Data
//
public:
	float left;
	float top;
	float right;
	float bottom;
};


// ===================================================================================
//	Inlines
// ===================================================================================
inline X2DRect::X2DRect(const X2DPoint& topLeftPt, const X2DPoint& botRightPt)
{
	left	= topLeftPt.x;
	top		= topLeftPt.y;
	right	= botRightPt.x;
	bottom	= botRightPt.y;
}

inline void X2DRect::MoveBy(const X2DPoint& pt)
{
	left 	+= pt.x; 
	top 	+= pt.y; 
	right 	+= pt.x; 
	bottom 	+= pt.y; 
}

inline void X2DRect::MoveTo(const X2DPoint& pt)				
{
	this->MoveBy(X2DPoint(pt.x - left, pt.y - top));
}

inline void X2DRect::Inset(double dx, double dy)
{
	top 	= (float) (top + dy); 
	left 	= (float) (left + dx); 
	bottom 	= (float) (bottom - dy); 
	right 	= (float) (right - dx);
}
 
inline void X2DRect::Inset(const X2DPoint& delta)				
{
	this->Inset(delta.x, delta.y);
}

inline X2DRect& X2DRect::operator&=(const X2DRect& rhs)
{
	*this = *this & rhs; 
	return *this;
}
	
inline X2DRect& X2DRect::operator|=(const X2DRect& rhs)
{
	*this = *this | rhs; 
	return *this;
}

inline X2DRect& X2DRect::operator+=(const X2DPoint& rhs)
{
	this->MoveBy(rhs); 
	return *this;
}

inline X2DRect& X2DRect::operator-=(const X2DPoint& rhs)
{
	this->MoveBy(-rhs); 
	return *this;
}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper


DECLARE_POD(Whisper::X2DPoint)
DECLARE_POD(Whisper::X2DRect)

