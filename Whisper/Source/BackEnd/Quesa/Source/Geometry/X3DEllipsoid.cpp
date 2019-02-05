/*
 *  File:       X3DEllipsoid.cpp
 *  Summary:	A generalized 3D sphere.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DEllipsoid.cpp,v $
 *		Revision 1.4  2001/04/21 03:35:47  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:28:18  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:15:29  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DEllipsoid.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetEllipsoidData
//
//---------------------------------------------------------------
static TQ3EllipsoidData GetEllipsoidData(const X3DPoint& origin, const X3DPoint& radius, TQ3EndCap caps, float uMin, float uMax, float vMin, float vMax)
{	
	TQ3EllipsoidData data;
	
	data.origin                = origin;
	data.orientation           = X3DVector(0.0f, radius.y, 0.0f);
	data.majorRadius           = X3DVector(0.0f, 0.0f, radius.z);
	data.minorRadius           = X3DVector(radius.x, 0.0f, 0.0f);
	data.uMin                  = uMin;
	data.uMax                  = uMax;
	data.vMin                  = vMin;
	data.vMax                  = vMax;
	data.caps                  = caps;
	data.interiorAttributeSet  = nil;
	data.ellipsoidAttributeSet = nil;
		
	return data;
}


//---------------------------------------------------------------
//
// CreateEllipsoid (X3DPoint, X3DPoint, TQ3EndCap, float, float, float, float)
//
//---------------------------------------------------------------
static TQ3GeometryObject CreateEllipsoid(const X3DPoint& origin, const X3DPoint& radius, TQ3EndCap caps, float uMin, float uMax, float vMin, float vMax)
{
	TQ3EllipsoidData data = GetEllipsoidData(origin, radius, caps, uMin, uMax, vMin, vMax);
	
	TQ3GeometryObject object = Q3Ellipsoid_New(&data);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DEllipsoid
// ===================================================================================

//---------------------------------------------------------------
//
// X3DEllipsoid::~X3DEllipsoid
//
//---------------------------------------------------------------
X3DEllipsoid::~X3DEllipsoid()
{
}


//---------------------------------------------------------------
//
// X3DEllipsoid::X3DEllipsoid (X3DPoint, X3DPoint, TQ3EndCap, float, float, float, float)
//
//---------------------------------------------------------------
X3DEllipsoid::X3DEllipsoid(const X3DPoint& origin, const X3DPoint& radius, TQ3EndCap caps, float uMin, float uMax, float vMin, float vMax) : X3DGeometry(CreateEllipsoid(origin, radius, caps, uMin, uMax, vMin, vMax))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipsoid::X3DEllipsoid (X3DRect, TQ3EndCap, float, float, float, float)
//
//---------------------------------------------------------------
X3DEllipsoid::X3DEllipsoid(const X3DRect& box, TQ3EndCap caps, float uMin, float uMax, float vMin, float vMax) : X3DGeometry(CreateEllipsoid(box.GetCenter(), box.GetSize(), caps, uMin, uMax, vMin, vMax))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipsoid::X3DEllipsoid (TQ3EllipsoidData)
//
//---------------------------------------------------------------
X3DEllipsoid::X3DEllipsoid(const TQ3EllipsoidData& data) : X3DGeometry(Q3Ellipsoid_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipsoid::X3DEllipsoid (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DEllipsoid::X3DEllipsoid(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypeEllipsoid);
}


//---------------------------------------------------------------
//
// X3DEllipsoid::Clone
//
//---------------------------------------------------------------
X3DEllipsoid* X3DEllipsoid::Clone() const
{
	X3DEllipsoid* object = new X3DEllipsoid(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DEllipsoid::GetOrigin
//
//---------------------------------------------------------------
X3DPoint X3DEllipsoid::GetOrigin() const
{
	X3DPoint origin;
	
	TQ3Status status = Q3Ellipsoid_GetOrigin(mObject, origin);
	ThrowIf3DError(status);
	
	return origin;
}


//---------------------------------------------------------------
//
// X3DEllipsoid::SetOrigin
//
//---------------------------------------------------------------
void X3DEllipsoid::SetOrigin(const X3DPoint& origin)
{
	TQ3Status status = Q3Ellipsoid_SetOrigin(mObject, origin);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipsoid::GetOrientation
//
//---------------------------------------------------------------
X3DVector X3DEllipsoid::GetOrientation() const
{
	X3DVector orientation;
	
	TQ3Status status = Q3Ellipsoid_GetOrientation(mObject, orientation);
	ThrowIf3DError(status);
	
	return orientation;
}


//---------------------------------------------------------------
//
// X3DEllipsoid::SetOrientation
//
//---------------------------------------------------------------
void X3DEllipsoid::SetOrientation(const X3DVector& orientation)
{
	TQ3Status status = Q3Ellipsoid_SetOrientation(mObject, orientation);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipsoid::GetMajorRadius
//
//---------------------------------------------------------------
X3DVector X3DEllipsoid::GetMajorRadius() const
{
	X3DVector radius;
	
	TQ3Status status = Q3Ellipsoid_GetMajorRadius(mObject, radius);
	ThrowIf3DError(status);
	
	return radius;
}


//---------------------------------------------------------------
//
// X3DEllipsoid::SetMajorRadius
//
//---------------------------------------------------------------
void X3DEllipsoid::SetMajorRadius(const X3DVector& radius)
{
	TQ3Status status = Q3Ellipsoid_SetMajorRadius(mObject, radius);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipsoid::GetMinorRadius
//
//---------------------------------------------------------------
X3DVector X3DEllipsoid::GetMinorRadius() const
{
	X3DVector radius;
	
	TQ3Status status = Q3Ellipsoid_GetMinorRadius(mObject, radius);
	ThrowIf3DError(status);
	
	return radius;
}


//---------------------------------------------------------------
//
// X3DEllipsoid::SetMinorRadius
//
//---------------------------------------------------------------
void X3DEllipsoid::SetMinorRadius(const X3DVector& radius)
{
	TQ3Status status = Q3Ellipsoid_SetMinorRadius(mObject, radius);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipsoid::GetData
//
//---------------------------------------------------------------
TQ3EllipsoidData X3DEllipsoid::GetData() const
{
	TQ3EllipsoidData data;
	
	TQ3Status status = Q3Ellipsoid_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DEllipsoid::SetData
//
//---------------------------------------------------------------
void X3DEllipsoid::SetData(const TQ3EllipsoidData& data)
{
	TQ3Status status = Q3Ellipsoid_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipsoid::Submit (X3DPoint, X3DPoint, TQ3ViewObject, TQ3EndCap, float, float, float, float)	[static]
//
//---------------------------------------------------------------
void X3DEllipsoid::Submit(const X3DPoint& origin, const X3DPoint& radius, TQ3ViewObject view, TQ3EndCap caps, float uMin, float uMax, float vMin, float vMax)
{
	PRECONDITION(view != nil);
	
	TQ3EllipsoidData data = Whisper::GetEllipsoidData(origin, radius, caps, uMin, uMax, vMin, vMax);

	TQ3Status status = Q3Ellipsoid_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipsoid::Submit (X3DRect, TQ3ViewObject, TQ3EndCap, float, float, float, float)	[static]
//
//---------------------------------------------------------------
void X3DEllipsoid::Submit(const X3DRect& box, TQ3ViewObject view, TQ3EndCap caps, float uMin, float uMax, float vMin, float vMax)
{
	PRECONDITION(view != nil);
	
	TQ3EllipsoidData data = Whisper::GetEllipsoidData(box.GetCenter(), box.GetSize(), caps, uMin, uMax, vMin, vMax);

	TQ3Status status = Q3Ellipsoid_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipsoid::Submit (TQ3EllipsoidData, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DEllipsoid::Submit(const TQ3EllipsoidData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Ellipsoid_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipsoid::IsCompatible 							[static]
//
//---------------------------------------------------------------
bool X3DEllipsoid::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypeEllipsoid);
	
	return compatible;
}


}	// namespace Whisper

