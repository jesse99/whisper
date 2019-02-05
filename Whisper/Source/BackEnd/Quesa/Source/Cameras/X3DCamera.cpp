/*
 *  File:       X3DCamera.h
 *  Summary:	Base class for QuickDraw 3D cameras.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DCamera.cpp,v $
 *		Revision 1.3  2001/04/14 07:27:39  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:14:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <X3DCamera.h>

#include <QuesaLight.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	class X3DCamera
// ===================================================================================

//---------------------------------------------------------------
//
// X3DCamera::~X3DCamera
//
//---------------------------------------------------------------
X3DCamera::~X3DCamera()
{
}


//---------------------------------------------------------------
//
// X3DCamera::X3DCamera
//
//---------------------------------------------------------------
X3DCamera::X3DCamera(TQ3CameraObject object) : X3DShape(object)
{
	this->ConfirmType(kQ3ShapeTypeCamera);
}


//---------------------------------------------------------------
//
// X3DCamera::Clone
//
//---------------------------------------------------------------
X3DCamera* X3DCamera::Clone() const
{
	X3DCamera* object = new X3DCamera(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DCamera::GetPlacement
//
//---------------------------------------------------------------
TQ3CameraPlacement X3DCamera::GetPlacement() const
{
	TQ3CameraPlacement placement;
	
	TQ3Status status = Q3Camera_GetPlacement(mObject, &placement);
	ThrowIf3DError(status);
	
	return placement;
}


//---------------------------------------------------------------
//
// X3DCamera::SetPlacement (TQ3CameraPlacement)
//
//---------------------------------------------------------------
void X3DCamera::SetPlacement(const TQ3CameraPlacement& placement)
{
	TQ3Status status = Q3Camera_SetPlacement(mObject, &placement);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCamera::SetPlacement (X3DPoint, X3DPoint, X3DVector)
//
//---------------------------------------------------------------
void X3DCamera::SetPlacement(const X3DPoint& loc, const X3DPoint& pointOfInterest, const X3DVector& upVector)
{
	TQ3CameraPlacement placement;
	placement.cameraLocation  = loc;				// set placement data 
	placement.pointOfInterest = pointOfInterest;
	placement.upVector        = upVector;

	TQ3Status status = Q3Camera_SetPlacement(mObject, &placement);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCamera::GetRange
//
//---------------------------------------------------------------
TQ3CameraRange X3DCamera::GetRange() const
{
	TQ3CameraRange range;
	
	TQ3Status status = Q3Camera_GetRange(mObject, &range);
	ThrowIf3DError(status);
	
	return range;
}


//---------------------------------------------------------------
//
// X3DCamera::SetRange
//
//---------------------------------------------------------------
void X3DCamera::SetRange(const TQ3CameraRange& range)
{
	TQ3Status status = Q3Camera_SetRange(mObject, &range);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCamera::GetViewPort
//
//---------------------------------------------------------------
TQ3CameraViewPort X3DCamera::GetViewPort() const
{
	TQ3CameraViewPort port;
	
	TQ3Status status = Q3Camera_GetViewPort(mObject, &port);
	ThrowIf3DError(status);
	
	return port;
}


//---------------------------------------------------------------
//
// X3DCamera::SetViewPort
//
//---------------------------------------------------------------
void X3DCamera::SetViewPort(const TQ3CameraViewPort& viewPort)
{
	TQ3Status status = Q3Camera_SetViewPort(mObject, &viewPort);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCamera::GetData
//
//---------------------------------------------------------------
TQ3CameraData X3DCamera::GetData() const
{
	TQ3CameraData data;
	
	TQ3Status status = Q3Camera_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DCamera::SetData
//
//---------------------------------------------------------------
void X3DCamera::SetData(const TQ3CameraData& data)
{
	TQ3Status status = Q3Camera_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCamera::GetWorldToView
//
//---------------------------------------------------------------
X3DMatrix X3DCamera::GetWorldToView() const
{
	X3DMatrix transform;
	
	TQ3Status status = Q3Camera_GetWorldToView(mObject, transform);
	ThrowIf3DError(status);
	
	return transform;
}


//---------------------------------------------------------------
//
// X3DCamera::GetWorldToFrustum
//
//---------------------------------------------------------------
X3DMatrix X3DCamera::GetWorldToFrustum() const
{
	X3DMatrix transform;
	
	TQ3Status status = Q3Camera_GetWorldToFrustum(mObject, transform);
	ThrowIf3DError(status);
	
	return transform;
}


//---------------------------------------------------------------
//
// X3DCamera::GetViewToFrustum
//
//---------------------------------------------------------------
X3DMatrix X3DCamera::GetViewToFrustum() const
{
	X3DMatrix transform;
	
	TQ3Status status = Q3Camera_GetViewToFrustum(mObject, transform);
	ThrowIf3DError(status);
	
	return transform;
}


//---------------------------------------------------------------
//
// X3DCamera::IsCompatible 								[static]
//
//---------------------------------------------------------------
bool X3DCamera::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3ShapeTypeCamera);
	
	return compatible;
}


}	// namespace Whisper
