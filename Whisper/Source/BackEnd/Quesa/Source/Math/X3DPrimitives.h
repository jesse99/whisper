/*
 *  File:       X3DPrimitives.h
 *  Summary:	3D geometry primitives for QuickDraw 3D.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DPoint	- 3D point
 *				X3DRect		- 3D box
 *				X3DSphere	- 3D sphere
 *
 *  Copyright � 1997-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DPrimitives.h,v $
 *		Revision 1.6  2001/04/14 07:31:00  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.5  2001/03/03 11:15:48  jesjones
 *		Instead of specializing selected algorithms for POD types, we use the new MSL mechanism to declare POD types.
 *		
 *		Revision 1.4  2001/03/02 11:23:40  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/12/13 08:53:25  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:19:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <6>	  4/16/00	JDJ		Added X3DPoint(TQ3Vector3D) ctor. Added friend functions
 *									to add and subtract X3DVector's from X3DPoint's.
 *		 <5>	  1/30/98	JDJ		Added streaming operators.
 *		 <4>	  1/26/98	JDJ		X3DPoint no longer supports < or > operators.
 *		 <3>	 10/21/97	JDJ		Scalars use doubles instead of floats.
 *		 <2>	  4/15/97	JDJ		Made Equal methods friend functions.
 *		 <1>	  2/14/97	JDJ		Created.
 */

#pragma once

#include <Quesa.h>

#include <X3DVectors.h>
#include <XDebug.h>
#include <XGeneric.h>
#include <XRect.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class X3DPoint;
class X3DRect;
class X3DSphere;
class XInStream;
class XOutStream;
	
	
//-----------------------------------
//	Constants
//
extern QUESA_EXPORT const X3DPoint 	kZero3DPt;
extern QUESA_EXPORT const X3DRect   kZero3DRect;
extern QUESA_EXPORT const X3DSphere	kZero3DSphere;


// ===================================================================================
//	class X3DPoint
//!		3D point class.
// ===================================================================================
class QUESA_EXPORT X3DPoint {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DPoint()												{}
						X3DPoint(float inX, float inY, float inZ)				{x = inX; y = inY; z = inZ;}
	explicit			X3DPoint(const TQ3Point2D& rhs)							{x = rhs.x; y = rhs.y; z = 0.0f;}
						X3DPoint(const TQ3Point3D& rhs)							{x = rhs.x; y = rhs.y; z = rhs.z;}
	explicit			X3DPoint(const TQ3Vector3D& rhs)						{x = rhs.x; y = rhs.y; z = rhs.z;}
						
			void 		Set(float inX, float inY, float inZ)					{x = inX; y = inY; z = inZ;}

//-----------------------------------
//	 API
//
public:
	//! @name Conversion operators
	//@{
						operator TQ3Point3D() const								{return *reinterpret_cast<const TQ3Point3D*>(this);}
						operator TQ3Point3D*()									{return reinterpret_cast<TQ3Point3D*>(this);}
						operator const TQ3Point3D*() const						{return reinterpret_cast<const TQ3Point3D*>(this);}
	//@}

	//! @name Arithmetic operators
	//@{	
			X3DPoint 	operator-() const										{return X3DPoint(-x, -y, -z);}
	
			X3DPoint 	operator+(const X3DPoint& rhs) const					{return X3DPoint(x + rhs.x, y + rhs.y, z + rhs.z);}
			X3DPoint 	operator+(const X3DVector& rhs) const					{return X3DPoint(x + rhs.x, y + rhs.y, z + rhs.z);}
	friend 	X3DPoint 	operator+(const X3DVector& lhs, const X3DPoint& rhs)	{return X3DPoint(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);}

			X3DPoint 	operator-(const X3DPoint& rhs) const					{return X3DPoint(x - rhs.x, y - rhs.y, z - rhs.z);}
			X3DPoint 	operator-(const X3DVector& rhs) const					{return X3DPoint(x - rhs.x, y - rhs.y, z - rhs.z);}
	friend 	X3DPoint 	operator-(const X3DVector& lhs, const X3DPoint& rhs)	{return X3DPoint(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);}

			X3DPoint 	operator*(double num) const								{return X3DPoint((float) (x*num), (float) (y*num), (float) (z*num));}
	friend 	X3DPoint 	operator*(double num, const X3DPoint& rhs);
			X3DPoint 	operator/(double num) const								{ASSERT(num != 0.0f); return X3DPoint((float) (x/num), (float) (y/num), (float) (z/num));}

			X3DPoint& 	operator+=(const X3DPoint& rhs);
			X3DPoint& 	operator+=(const X3DVector& rhs);

			X3DPoint& 	operator-=(const X3DPoint& rhs);	
			X3DPoint& 	operator-=(const X3DVector& rhs);	
	//@}
					
	//! @name Comparisons
	//@{
			bool 		operator==(const X3DPoint& rhs) const					{return Equal(*this, rhs);}
			bool 		operator!=(const X3DPoint& rhs) const					{return !Equal(*this, rhs);}

	friend	bool 		Equal(const X3DPoint& lhs, const X3DPoint& rhs, double tolerance = 1.0e-6);

	friend	double 		Distance(const X3DPoint& lhs, const X3DPoint& rhs);
	friend	double 		DistanceSquared(const X3DPoint& lhs, const X3DPoint& rhs);
	//@}

	//! @name Streaming
	//@{
	friend 	XInStream& 	operator>>(XInStream& stream, X3DPoint& rhs);	
	friend	XOutStream& operator<<(XOutStream& stream, const X3DPoint& rhs);
	
	friend 	void 			FromStr(const std::wstring& str, X3DPoint& value);
	friend 	std::wstring	ToStr(const X3DPoint& value);
	//@}

//-----------------------------------
//	Member data
//
public:
	float x;
	float y;
	float z;
};


// ===================================================================================
//	class X3DRect
//!		3D rect class.
// ===================================================================================
class QUESA_EXPORT X3DRect {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DRect()									{}
						X3DRect(const X3DPoint& origin, const X3DVector& offset);
						X3DRect(const X3DPoint& min, const X3DPoint& max);
	explicit			X3DRect(const XRect& rect);

//-----------------------------------
//	 API
//
public:
	//! @name Getters
	//@{
			X3DPoint 	GetCenter() const;
			X3DPoint 	GetSize() const								{return size;}
			double	 	GetVolume() const;
	//@}

	//! @name Operations
	//@{
			void 		MoveTo(const X3DPoint& pt)					{origin = pt;}
			void 		MoveBy(const X3DPoint& offset);
			void 		MoveBy(const X3DVector& offset);

			X3DRect 	operator+(const X3DPoint& rhs) const;
			X3DRect 	operator-(const X3DPoint& rhs) const;
			X3DRect 	operator+(const X3DVector& rhs) const;
			X3DRect 	operator-(const X3DVector& rhs) const;
					
			X3DRect 	operator&(const X3DRect& rhs) const;
			X3DRect 	operator|(const X3DRect& rhs) const;
		
			X3DRect& 	operator+=(const X3DPoint& rhs);
			X3DRect& 	operator-=(const X3DPoint& rhs);
			X3DRect& 	operator+=(const X3DVector& rhs);
			X3DRect& 	operator-=(const X3DVector& rhs);
			
			X3DRect& 	operator&=(const X3DRect& rhs);	
			X3DRect& 	operator|=(const X3DRect& rhs);
		
			void 		Inset(const X3DPoint& delta);
			void 		Inset(double dx, double dy, double dz)		{this->Inset(X3DPoint((float) dx, (float) dy, (float) dz));}
		
			void 		SetSize(const X3DPoint& newSize)			{ASSERT(size.x >= 0.0f && size.y >= 0.0f && size.z >= 0.0f); size = newSize;}
	//@}
			
	//! @name Comparisons
	//@{
			bool 		operator==(const X3DRect& rhs) const		{return Equal(*this, rhs);}
			bool 		operator!=(const X3DRect& rhs) const		{return !Equal(*this, rhs);}

	friend	bool 		Equal(const X3DRect& lhs, const X3DRect& rhs, double tolerance = 1.0e-6);
	//@}

	//! @name Streaming
	//@{
	friend 	XInStream& 	operator>>(XInStream& stream, X3DRect& rhs);	
	friend	XOutStream& operator<<(XOutStream& stream, const X3DRect& rhs);
	
	friend 	void 			FromStr(const std::wstring& str, X3DRect& value);
	friend 	std::wstring	ToStr(const X3DRect& value);
	//@}

	//! @name Misc
	//@{
			bool 		Contains(const X3DPoint& pt) const;
						/**< Returns true if pt is within the rectangle.*/
		
			bool 		Contains(const X3DRect& rect) const;
		
			bool 		Intersects(const X3DRect& rect) const;
						/**< Return true if the rectangle intersects rect. */
		
			bool 		IsEmpty() const;
						/**< Returns true if any size component is zero. */
		
			void 		MakeEmpty();
						/**< Empties the rectangle. */
	//@}
	
//-----------------------------------
//	Member data
//
public:
	X3DPoint	origin;			//!< a corner of the box
	X3DPoint 	size;			//!< offset to the opposite corner (should be positive)
};


// ===================================================================================
//	class X3DSphere
//!		3D sphere class.
// ===================================================================================
class QUESA_EXPORT X3DSphere {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DSphere()									{}
						X3DSphere(const X3DPoint& origin, float radius);

//-----------------------------------
//	 API
//
public:
	//! @name Getters
	//@{
			X3DPoint 	GetOrigin() const							{return origin;}
			double 		GetRadius() const							{return radius;}
			double	 	GetVolume() const;
	//@}

	//! @name Operations
	//@{
			void 		MoveTo(const X3DPoint& pt)					{origin = pt;}
			void 		MoveBy(const X3DPoint& offset);
			void 		MoveBy(const X3DVector& offset);

			X3DSphere 	operator+(const X3DPoint& rhs) const;
			X3DSphere 	operator-(const X3DPoint& rhs) const;
			X3DSphere 	operator+(const X3DVector& rhs) const;
			X3DSphere 	operator-(const X3DVector& rhs) const;
							
			X3DSphere& 	operator+=(const X3DPoint& rhs);
			X3DSphere& 	operator-=(const X3DPoint& rhs);
			X3DSphere& 	operator+=(const X3DVector& rhs);
			X3DSphere& 	operator-=(const X3DVector& rhs);
			
			void 		Inset(double delta);
		
			void 		SetRadius(float newRadius)					{ASSERT(newRadius >= 0.0f); radius = newRadius;}
	//@}
			
	//! @name Comparisons
	//@{
			bool 		operator==(const X3DSphere& rhs) const		{return Equal(*this, rhs);}
			bool 		operator!=(const X3DSphere& rhs) const		{return !Equal(*this, rhs);}

	friend	bool 		Equal(const X3DSphere& lhs, const X3DSphere& rhs, double tolerance = 1.0e-6);
	//@}

	//! @name Streaming
	//@{
	friend 	XInStream& 	operator>>(XInStream& stream, X3DSphere& rhs);	
	friend	XOutStream& operator<<(XOutStream& stream, const X3DSphere& rhs);
	
	friend 	void 			FromStr(const std::wstring& str, X3DSphere& value);
	friend 	std::wstring	ToStr(const X3DSphere& value);
	//@}

	//! @name Misc
	//@{
			bool 		Contains(const X3DPoint& pt) const;
						/**< Returns true if pt is within the sphere. */
		
			bool 		IsEmpty() const;
						/**< Returns true if radius is zero. */
		
			void 		MakeEmpty()									{radius = 0.0f;}
	//@}
		
//-----------------------------------
//	Member data
//
public:
	X3DPoint	origin;			
	float		radius;			
};


// ===================================================================================
//	Inlines
// ===================================================================================
inline X3DPoint& X3DPoint::operator+=(const X3DPoint& rhs)	
{
	x += rhs.x; 
	y += rhs.y; 
	z += rhs.z; 
	
	return *this;
}

inline X3DPoint& X3DPoint::operator+=(const X3DVector& rhs)	
{
	x += rhs.x; 
	y += rhs.y; 
	z += rhs.z; 
	
	return *this;
}

inline X3DPoint& X3DPoint::operator-=(const X3DPoint& rhs)	
{
	x -= rhs.x; 
	y -= rhs.y; 
	z -= rhs.z; 
	
	return *this;
}

inline X3DPoint& X3DPoint::operator-=(const X3DVector& rhs)	
{
	x -= rhs.x; 
	y -= rhs.y; 
	z -= rhs.z; 
	
	return *this;
}

#pragma mark -

inline void X3DRect::MoveBy(const X3DPoint& pt)					
{
	origin += pt;
}


inline void X3DRect::MoveBy(const X3DVector& pt)					
{
	origin += pt;
}

#pragma mark -

inline void X3DSphere::MoveBy(const X3DPoint& pt)					
{
	origin += pt;
}

inline void X3DSphere::MoveBy(const X3DVector& pt)					
{
	origin += pt;
}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper


DECLARE_POD(Whisper::X3DPoint)
DECLARE_POD(Whisper::X3DRect)
DECLARE_POD(Whisper::X3DSphere)


