/*
 *  File:       X3DPrimitives.cpp
 *  Summary:	3D geometry primitives for QuickDraw 3D.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DPrimitives.cpp,v $
 *		Revision 1.6  2001/04/21 03:37:20  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/14 07:30:53  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.4  2001/03/02 11:23:31  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/11/20 05:43:47  jesjones
 *		Added std:: to a few function calls.
 *		
 *		Revision 1.2  2000/11/09 12:19:15  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <6>	  8/08/98	JDJ		Uses kQ3Pi instead of kPi.
 *		 <5>	  1/30/98	JDJ		Added streaming operators.
 *		 <4>	  1/26/98	JDJ		X3DPoint no longer supports < or > operators.
 *									X3DRect::operator&= correctly returns kZero3DRect if
 *									the rectangles don't intersect.
 *		 <3>	 10/21/97	JDJ		Scalars use doubles instead of floats.
 *		 <2>	  4/15/97	JDJ		Made Equal methods friend functions.
 *		 <1>	  2/14/97	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <X3DPrimitives.h>

#include <cmath>
#include <QuesaMath.h> 

#include <XMiscUtils.h>
#include <XNumbers.h>
#include <XStreaming.h>
#include <XTextConversions.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
extern const double_t pi;

const X3DPoint	kZero3DPt(0.0f, 0.0f, 0.0f);
const X3DRect 	kZero3DRect(kZero3DPt, kZero3DPt);
const X3DSphere	kZero3DSphere(kZero3DPt, 0.0f);


// ===================================================================================
//	class X3DPoint
// ===================================================================================

//---------------------------------------------------------------
//
// operator* (double, X3DPoint)
//
//---------------------------------------------------------------
X3DPoint operator*(double num, const X3DPoint& pt)
{
	return X3DPoint((float) (pt.x*num), (float) (pt.y*num), (float) (pt.z*num));
}


//---------------------------------------------------------------
//
// Distance (X3DPoint, X3DPoint)
//
//---------------------------------------------------------------
double Distance(const X3DPoint& lhs, const X3DPoint& rhs)
{
	double dx = (double) lhs.x - rhs.x;
	double dy = (double) lhs.y - rhs.y;
	double dz = (double) lhs.z - rhs.z;
	
	double dist = std::sqrt(dx*dx + dy*dy + dz*dz);
	
	return dist;
}


//---------------------------------------------------------------
//
// DistanceSquared (X3DPoint, X3DPoint)
//
//---------------------------------------------------------------
double DistanceSquared(const X3DPoint& lhs, const X3DPoint& rhs)
{
	double dx = (double) lhs.x - rhs.x;
	double dy = (double) lhs.y - rhs.y;
	double dz = (double) lhs.z - rhs.z;
	
	double dist = dx*dx + dy*dy + dz*dz;
	
	return dist;
}


//---------------------------------------------------------------
//
// Equal
//
//---------------------------------------------------------------
bool Equal(const X3DPoint& lhs, const X3DPoint& rhs, double tolerance)
{
	bool equal = false;
	
	if (Equal(lhs.x, rhs.x, tolerance))
		if (Equal(lhs.y, rhs.y, tolerance))
			if (Equal(lhs.z, rhs.z, tolerance))
				equal = true;

	return equal;
}


//---------------------------------------------------------------
//
// operator>> (X3DPoint)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, X3DPoint& rhs)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == '3DPt');
				
	stream.ReadBytes(&rhs.x, sizeof(rhs.x));
	stream.ReadBytes(&rhs.y, sizeof(rhs.y));
	stream.ReadBytes(&rhs.z, sizeof(rhs.z));
	
	if (stream.NeedsByteSwap()) {
		ByteSwap(rhs.x);
		ByteSwap(rhs.y);
		ByteSwap(rhs.z);
	}
	
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (X3DPoint)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const X3DPoint& rhs)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('3DPt');
#endif

	stream.WriteBytes(&rhs.x, sizeof(rhs.x));
	stream.WriteBytes(&rhs.y, sizeof(rhs.y));
	stream.WriteBytes(&rhs.z, sizeof(rhs.z));

	return stream;
}


//---------------------------------------------------------------
//
// FromStr (wstring, X3DPoint)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& inStr, X3DPoint& value)
{
	FromStr(inStr, value.x);
	
	std::wstring str = After(inStr, L" ");
	FromStr(str, value.y);
	
	str = After(str, L" ");
	FromStr(str, value.z);
}


//---------------------------------------------------------------
//
// ToStr (X3DPoint)
//
//---------------------------------------------------------------
std::wstring ToStr(const X3DPoint& value)
{
	return ToStr(value.x) + L" " + ToStr(value.y) + L" " + ToStr(value.z);
}

#pragma mark -

// ===================================================================================
//	class X3DRect
// ===================================================================================

//---------------------------------------------------------------
//
// X3DRect::X3DRect (X3DPoint, X3DVector)
//
//---------------------------------------------------------------
X3DRect::X3DRect(const X3DPoint& startPt, const X3DVector& offset)
{
	PRECONDITION(offset.x >= 0.0f);
	PRECONDITION(offset.y >= 0.0f);
	PRECONDITION(offset.z >= 0.0f);

	origin = startPt;

	size.x = offset.x;
	size.y = offset.y;
	size.z = offset.z;
}


//---------------------------------------------------------------
//
// X3DRect::X3DRect (X3DPoint, X3DPoint)
//
//---------------------------------------------------------------
X3DRect::X3DRect(const X3DPoint& min, const X3DPoint& max)
{
	if (DistanceSquared(kZero3DPt, min) < DistanceSquared(kZero3DPt, max)) {
		origin = min;

		size.x = max.x - min.x;
		size.y = max.y - min.y;
		size.z = max.z - min.z;

	} else {
		origin = max;

		size.x = min.x - max.x;
		size.y = min.y - max.y;
		size.z = min.z - max.z;
	}
}


//---------------------------------------------------------------
//
// X3DRect::X3DRect (XRect)
//
//---------------------------------------------------------------
X3DRect::X3DRect(const XRect& rect)
{
	origin = X3DPoint(rect.left, rect.top, 0.0f);

	size.x = rect.right - rect.left;
	size.y = rect.bottom - rect.top;
	size.z = 0.0f;
}


//---------------------------------------------------------------
//
// X3DRect::GetCenter
//
//---------------------------------------------------------------
X3DPoint X3DRect::GetCenter() const
{
	X3DPoint center;
	
	center.x = origin.x + 0.5f*size.x;
	center.y = origin.y + 0.5f*size.y;
	center.z = origin.z + 0.5f*size.z;
	
	return center;
}


//---------------------------------------------------------------
//
// X3DRect::GetVolume
//
//---------------------------------------------------------------
double X3DRect::GetVolume() const
{
	double volume = (double) size.x*size.y*size.z;
	
	return volume;
}


//---------------------------------------------------------------
//
// X3DRect::operator+
//
//---------------------------------------------------------------
X3DRect X3DRect::operator+(const X3DPoint& rhs) const
{
	X3DRect box = *this;
	
	box.origin += rhs;
	
	return box;
}


//---------------------------------------------------------------
//
// X3DRect::operator+
//
//---------------------------------------------------------------
X3DRect X3DRect::operator+(const X3DVector& rhs) const
{
	X3DRect box = *this;
	
	box.origin += rhs;
	
	return box;
}


//---------------------------------------------------------------
//
// X3DRect::operator-
//
//---------------------------------------------------------------
X3DRect X3DRect::operator-(const X3DPoint& rhs) const
{
	X3DRect box = *this;
	
	box.origin -= rhs;
	
	return box;
}


//---------------------------------------------------------------
//
// X3DRect::operator-
//
//---------------------------------------------------------------
X3DRect X3DRect::operator-(const X3DVector& rhs) const
{
	X3DRect box = *this;
	
	box.origin -= rhs;
	
	return box;
}


//---------------------------------------------------------------
//
// X3DRect::operator&
//
//---------------------------------------------------------------
X3DRect X3DRect::operator&(const X3DRect& rhs) const
{
	X3DRect box = *this;

	box &= rhs;
	
	return box;
}


//---------------------------------------------------------------
//
// X3DRect::operator|
//
//---------------------------------------------------------------
X3DRect X3DRect::operator|(const X3DRect& rhs) const
{
	X3DRect box = *this;

	box |= rhs;
	
	return box;
}


//---------------------------------------------------------------
//
// X3DRect::operator+=
//
//---------------------------------------------------------------
X3DRect& X3DRect::operator+=(const X3DPoint& rhs)
{
	origin += rhs;
	
	return *this;
}


//---------------------------------------------------------------
//
// X3DRect::operator+=
//
//---------------------------------------------------------------
X3DRect& X3DRect::operator+=(const X3DVector& rhs)
{
	origin += rhs;
	
	return *this;
}


//---------------------------------------------------------------
//
// X3DRect::operator-=
//
//---------------------------------------------------------------
X3DRect& X3DRect::operator-=(const X3DPoint& rhs)
{
	origin -= rhs;
	
	return *this;
}


//---------------------------------------------------------------
//
// X3DRect::operator-=
//
//---------------------------------------------------------------
X3DRect& X3DRect::operator-=(const X3DVector& rhs)
{
	origin -= rhs;
	
	return *this;
}


//---------------------------------------------------------------
//
// X3DRect::operator&=
//
//---------------------------------------------------------------
X3DRect& X3DRect::operator&=(const X3DRect& rhs)
{
	X3DPoint lhsMin = origin;
	X3DPoint lhsMax = origin + size;

	X3DPoint rhsMin = rhs.origin;
	X3DPoint rhsMax = rhs.origin + rhs.size;
	
	X3DPoint min;
	min.x = Max(lhsMin.x, rhsMin.x);
	min.y = Max(lhsMin.y, rhsMin.y);
	min.z = Max(lhsMin.z, rhsMin.z);
	
	X3DPoint max;
	max.x = Min(lhsMax.x, rhsMax.x);
	max.y = Min(lhsMax.y, rhsMax.y);
	max.z = Min(lhsMax.z, rhsMax.z);
	
	if (DistanceSquared(kZero3DPt, min) < DistanceSquared(kZero3DPt, max))
		*this = X3DRect(min, max);
	else
		size = kZero3DPt;
	
	return *this;
}


//---------------------------------------------------------------
//
// X3DRect::operator|=
//
//---------------------------------------------------------------
X3DRect& X3DRect::operator|=(const X3DRect& rhs)
{
	if (this->IsEmpty())
		*this = rhs;
		
	else if (!rhs.IsEmpty()) {
		X3DPoint lhsMin = origin;
		X3DPoint lhsMax = origin + size;

		X3DPoint rhsMin = rhs.origin;
		X3DPoint rhsMax = rhs.origin + rhs.size;
		
		X3DPoint min;
		min.x = Min(lhsMin.x, rhsMin.x);
		min.y = Min(lhsMin.y, rhsMin.y);
		min.z = Min(lhsMin.z, rhsMin.z);
		
		X3DPoint max;
		max.x = Max(lhsMax.x, rhsMax.x);
		max.y = Max(lhsMax.y, rhsMax.y);
		max.z = Max(lhsMax.z, rhsMax.z);
		
		*this = X3DRect(min, max);
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// X3DRect::Inset (X3DPoint)
//
//---------------------------------------------------------------
void X3DRect::Inset(const X3DPoint& inDelta)
{
	X3DPoint delta = 0.5f*inDelta;
	
	origin += delta;
	size   -= delta;
	
	if (size.x < 0.0 || size.y < 0.0 || size.z < 0.0)
		size = kZero3DPt;
}


//---------------------------------------------------------------
//
// X3DRect::Contains (X3DPoint)
//
//---------------------------------------------------------------
bool X3DRect::Contains(const X3DPoint& pt) const
{
	X3DPoint min = origin;
	X3DPoint max = origin + size;
	
	bool contains = min.x <= pt.x && pt.x <= max.x &&
					min.y <= pt.y && pt.y <= max.y &&
					min.z <= pt.z && pt.z <= max.z;
					
	return contains;
}


//---------------------------------------------------------------
//
// X3DRect::Contains (X3DRect)
//
//---------------------------------------------------------------
bool X3DRect::Contains(const X3DRect& rect) const
{
	bool contains = this->Contains(rect.origin) && this->Contains(rect.origin + rect.size);
					
	return contains;
}


//---------------------------------------------------------------
//
// X3DRect::Intersects
//
//---------------------------------------------------------------
bool X3DRect::Intersects(const X3DRect& rhs) const
{
	X3DPoint lhsMin = origin;
	X3DPoint lhsMax = origin + size;

	X3DPoint rhsMin = rhs.origin;
	X3DPoint rhsMax = rhs.origin + rhs.size;
	
	bool intersects = lhsMin.x <= rhsMax.x && lhsMax.x >= rhsMin.x && 
					  lhsMin.y <= rhsMax.y && lhsMax.y >= rhsMin.y && 
					  lhsMin.z <= rhsMax.z && lhsMax.z >= rhsMin.z;
					  
	return intersects;
}


//---------------------------------------------------------------
//
// X3DRect::IsEmpty
//
//---------------------------------------------------------------
bool X3DRect::IsEmpty() const
{
	const float kTolerance = 1.0e-6f;
	
	return Equal(size.x, 0.0, kTolerance) || Equal(size.y, 0.0, kTolerance) || Equal(size.z, 0.0, kTolerance);
}


//---------------------------------------------------------------
//
// X3DRect::MakeEmpty
//
//---------------------------------------------------------------
void X3DRect::MakeEmpty()
{
	origin = kZero3DPt;
	size   = kZero3DPt;
}


//---------------------------------------------------------------
//
// Equal (X3DRect, X3DRect, double)
//
//---------------------------------------------------------------
bool Equal(const X3DRect& lhs, const X3DRect& rhs, double tolerance)
{
	bool equal = Equal(lhs.origin, rhs.origin, tolerance) && Equal(lhs.size, rhs.size, tolerance);
	
	return equal;
}


//---------------------------------------------------------------
//
// operator>> (X3DRect)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, X3DRect& rhs)
{
	stream >> rhs.origin >> rhs.size;
	
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (X3DRect)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const X3DRect& rhs)
{
	stream << rhs.origin << rhs.size;
	
	return stream;
}


//---------------------------------------------------------------
//
// FromStr (wstring, X3DRect)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, X3DRect& value)
{
	FromStr(str, value.origin);
	FromStr(After(str, L" "), value.size);
}


//---------------------------------------------------------------
//
// ToStr (X3DRect)
//
//---------------------------------------------------------------
std::wstring ToStr(const X3DRect& value)
{
	return ToStr(value.origin) + L" " + ToStr(value.size);
}

#pragma mark -

// ===================================================================================
//	class X3DSphere
// ===================================================================================

//---------------------------------------------------------------
//
// X3DSphere::X3DSphere
//
//---------------------------------------------------------------
X3DSphere::X3DSphere(const X3DPoint& center, float dim)
{
	PRECONDITION(dim >= 0.0f);
	
	origin = center;
	radius = dim;
}


//---------------------------------------------------------------
//
// X3DSphere::GetVolume
//
//---------------------------------------------------------------
double X3DSphere::GetVolume() const
{
	const double kScaleFactor = 4.0/3.0;
	
	double volume = kScaleFactor*kQ3Pi*radius*radius*radius;
	
	return volume; 
}


//---------------------------------------------------------------
//
// X3DSphere::operator+
//
//---------------------------------------------------------------
X3DSphere X3DSphere::operator+(const X3DPoint& rhs) const
{
	X3DSphere box = *this;
	
	box.origin += rhs;
	
	return box;
}


//---------------------------------------------------------------
//
// X3DSphere::operator+
//
//---------------------------------------------------------------
X3DSphere X3DSphere::operator+(const X3DVector& rhs) const
{
	X3DSphere box = *this;
	
	box.origin += rhs;
	
	return box;
}


//---------------------------------------------------------------
//
// X3DSphere::operator-
//
//---------------------------------------------------------------
X3DSphere X3DSphere::operator-(const X3DPoint& rhs) const
{
	X3DSphere box = *this;
	
	box.origin -= rhs;
	
	return box;
}

	
//---------------------------------------------------------------
//
// X3DSphere::operator-
//
//---------------------------------------------------------------
X3DSphere X3DSphere::operator-(const X3DVector& rhs) const
{
	X3DSphere box = *this;
	
	box.origin -= rhs;
	
	return box;
}

	
//---------------------------------------------------------------
//
// X3DSphere::operator+=
//
//---------------------------------------------------------------
X3DSphere& X3DSphere::operator+=(const X3DPoint& rhs)
{
	origin += rhs;
	
	return *this;
}


//---------------------------------------------------------------
//
// X3DSphere::operator+=
//
//---------------------------------------------------------------
X3DSphere& X3DSphere::operator+=(const X3DVector& rhs)
{
	origin += rhs;
	
	return *this;
}


//---------------------------------------------------------------
//
// X3DSphere::operator-=
//
//---------------------------------------------------------------
X3DSphere& X3DSphere::operator-=(const X3DPoint& rhs)
{
	origin -= rhs;
	
	return *this;
}


//---------------------------------------------------------------
//
// X3DSphere::operator-=
//
//---------------------------------------------------------------
X3DSphere& X3DSphere::operator-=(const X3DVector& rhs)
{
	origin -= rhs;
	
	return *this;
}


//---------------------------------------------------------------
//
// X3DSphere::Inset
//
//---------------------------------------------------------------
void X3DSphere::Inset(double delta)
{
	radius = (float) (radius - delta);
	
	if (radius < 0.0f)
		radius = 0.0f;
}


//---------------------------------------------------------------
//
// X3DSphere::Contains
//
//---------------------------------------------------------------
bool X3DSphere::Contains(const X3DPoint& pt) const
{
	double distance = Distance(origin, pt);
	
	bool contains = distance < radius;
	
	return contains;
}


//---------------------------------------------------------------
//
// X3DSphere::IsEmpty
//
//---------------------------------------------------------------
bool X3DSphere::IsEmpty() const
{
	bool empty = radius <= 0.0;
	
	return empty;
}


//---------------------------------------------------------------
//
// Equal (X3DSphere, X3DSphere, double)
//
//---------------------------------------------------------------
bool Equal(const X3DSphere& lhs, const X3DSphere& rhs, double tolerance)
{
	bool equal = Equal(lhs.origin, rhs.origin, tolerance) && Equal(lhs.radius, rhs.radius, tolerance);

	return equal;
}


//---------------------------------------------------------------
//
// operator>> (X3DSphere)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, X3DSphere& rhs)
{
	stream >> rhs.origin >> rhs.radius;
	
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (X3DSphere)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const X3DSphere& rhs)
{
	stream << rhs.origin << rhs.radius;
	
	return stream;
}


//---------------------------------------------------------------
//
// FromStr (wstring, X3DSphere)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, X3DSphere& value)
{
	FromStr(str, value.origin);
	FromStr(After(str, L" "), value.radius);
}


//---------------------------------------------------------------
//
// ToStr (X3DSphere)
//
//---------------------------------------------------------------
std::wstring ToStr(const X3DSphere& value)
{
	return ToStr(value.origin) + L" " + ToStr(value.radius);
}


}	// namespace Whisper
