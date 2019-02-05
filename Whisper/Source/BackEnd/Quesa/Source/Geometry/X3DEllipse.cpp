/*
 *  File:       X3DEllipse.cpp
 *  Summary:	A generalized 3D elliptical curve.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DEllipse.cpp,v $
 *		Revision 1.4  2001/04/21 03:35:43  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:28:15  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:15:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DEllipse.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetDiskData
//
//---------------------------------------------------------------
static TQ3EllipseData GetDiskData(const X3DPoint& origin, const X3DVector& majorRadius, const X3DVector& minorRadius, float uMin, float uMax)
{	
	TQ3EllipseData data;
	
	data.origin              = origin;
	data.majorRadius         = majorRadius;
	data.minorRadius         = minorRadius;
	data.uMin                = uMin;
	data.uMax                = uMax;
	data.ellipseAttributeSet = nil;
		
	return data;
}


//---------------------------------------------------------------
//
// CreateEllipse (X3DPoint, X3DPoint, float, float, float, float)
//
//---------------------------------------------------------------
static TQ3GeometryObject CreateEllipse(const X3DPoint& origin, const X3DVector& majorRadius, const X3DVector& minorRadius, float uMin, float uMax)
{
	TQ3EllipseData data = GetDiskData(origin, majorRadius, minorRadius, uMin, uMax);
	
	TQ3GeometryObject object = Q3Ellipse_New(&data);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DEllipse
// ===================================================================================

//---------------------------------------------------------------
//
// X3DEllipse::~X3DEllipse
//
//---------------------------------------------------------------
X3DEllipse::~X3DEllipse()
{
}


//---------------------------------------------------------------
//
// X3DEllipse::X3DEllipse (X3DPoint, X3DPoint, float, float, float, float)
//
//---------------------------------------------------------------
X3DEllipse::X3DEllipse(const X3DPoint& origin, const X3DVector& majorRadius, const X3DVector& minorRadius, float uMin, float uMax) : X3DGeometry(CreateEllipse(origin, majorRadius, minorRadius, uMin, uMax))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipse::X3DEllipse (TQ3EllipseData)
//
//---------------------------------------------------------------
X3DEllipse::X3DEllipse(const TQ3EllipseData& data) : X3DGeometry(Q3Ellipse_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipse::X3DEllipse (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DEllipse::X3DEllipse(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypeEllipse);
}


//---------------------------------------------------------------
//
// X3DEllipse::Clone
//
//---------------------------------------------------------------
X3DEllipse* X3DEllipse::Clone() const
{
	X3DEllipse* object = new X3DEllipse(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DEllipse::GetOrigin
//
//---------------------------------------------------------------
X3DPoint X3DEllipse::GetOrigin() const
{
	X3DPoint origin;
	
	TQ3Status status = Q3Ellipse_GetOrigin(mObject, origin);
	ThrowIf3DError(status);
	
	return origin;
}


//---------------------------------------------------------------
//
// X3DEllipse::SetOrigin
//
//---------------------------------------------------------------
void X3DEllipse::SetOrigin(const X3DPoint& origin)
{
	TQ3Status status = Q3Ellipse_SetOrigin(mObject, origin);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipse::GetMajorRadius
//
//---------------------------------------------------------------
X3DVector X3DEllipse::GetMajorRadius() const
{
	X3DVector radius;
	
	TQ3Status status = Q3Ellipse_GetMajorRadius(mObject, radius);
	ThrowIf3DError(status);
	
	return radius;
}


//---------------------------------------------------------------
//
// X3DEllipse::SetMajorRadius
//
//---------------------------------------------------------------
void X3DEllipse::SetMajorRadius(const X3DVector& radius)
{
	TQ3Status status = Q3Ellipse_SetMajorRadius(mObject, radius);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipse::GetMinorRadius
//
//---------------------------------------------------------------
X3DVector X3DEllipse::GetMinorRadius() const
{
	X3DVector radius;
	
	TQ3Status status = Q3Ellipse_GetMinorRadius(mObject, radius);
	ThrowIf3DError(status);
	
	return radius;
}


//---------------------------------------------------------------
//
// X3DEllipse::SetMinorRadius
//
//---------------------------------------------------------------
void X3DEllipse::SetMinorRadius(const X3DVector& radius)
{
	TQ3Status status = Q3Ellipse_SetMinorRadius(mObject, radius);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipse::GetData
//
//---------------------------------------------------------------
TQ3EllipseData X3DEllipse::GetData() const
{
	TQ3EllipseData data;
	
	TQ3Status status = Q3Ellipse_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DEllipse::SetData
//
//---------------------------------------------------------------
void X3DEllipse::SetData(const TQ3EllipseData& data)
{
	TQ3Status status = Q3Ellipse_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipse::Submit (X3DPoint, X3DPoint, TQ3ViewObject, float, float, float, float)	[static]
//
//---------------------------------------------------------------
void X3DEllipse::Submit(const X3DPoint& origin, const X3DVector& majorRadius, const X3DVector& minorRadius, TQ3ViewObject view, float uMin, float uMax)
{
	PRECONDITION(view != nil);
	
	TQ3EllipseData data = Whisper::GetDiskData(origin, majorRadius, minorRadius, uMin, uMax);

	TQ3Status status = Q3Ellipse_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipse::Submit (TQ3EllipseData, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DEllipse::Submit(const TQ3EllipseData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Ellipse_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DEllipse::IsCompatible 							[static]
//
//---------------------------------------------------------------
bool X3DEllipse::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypeEllipse);
	
	return compatible;
}


}	// namespace Whisper
