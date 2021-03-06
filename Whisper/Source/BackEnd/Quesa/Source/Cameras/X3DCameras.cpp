/*
 *  File:       X3DCameras.cpp
 *  Summary:	QuickDraw 3D camera objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DCameras.cpp,v $
 *		Revision 1.4  2001/04/21 03:35:14  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:27:48  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:14:15  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DCameras.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateViewAngleAspectCamera
//
//---------------------------------------------------------------
static X3DObject* CreateViewAngleAspectCamera(TQ3Object inObject)
{
	X3DViewAngleAspectCamera* object = new X3DViewAngleAspectCamera(inObject);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateViewPlaneCamera
//
//---------------------------------------------------------------
static X3DObject* CreateViewPlaneCamera(TQ3Object inObject)
{
	X3DViewPlaneCamera* object = new X3DViewPlaneCamera(inObject);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateOrthographicCamera
//
//---------------------------------------------------------------
static X3DObject* CreateOrthographicCamera(TQ3Object inObject)
{
	X3DOrthographicCamera* object = new X3DOrthographicCamera(inObject);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DViewAngleAspectCamera
// ===================================================================================

//---------------------------------------------------------------
//
// X3DViewAngleAspectCamera::~X3DViewAngleAspectCamera
//
//---------------------------------------------------------------
X3DViewAngleAspectCamera::~X3DViewAngleAspectCamera()
{
}


//---------------------------------------------------------------
//
// X3DViewAngleAspectCamera::X3DViewAngleAspectCamera (TQ3ViewAngleAspectCameraData)
//
//---------------------------------------------------------------
X3DViewAngleAspectCamera::X3DViewAngleAspectCamera(const TQ3ViewAngleAspectCameraData& data) : X3DCamera(Q3ViewAngleAspectCamera_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewAngleAspectCamera::X3DViewAngleAspectCamera (TQ3CameraObject)
//
//---------------------------------------------------------------
X3DViewAngleAspectCamera::X3DViewAngleAspectCamera(TQ3CameraObject object) : X3DCamera(object)
{
	this->ConfirmType(kQ3CameraTypeViewAngleAspect);
}


//---------------------------------------------------------------
//
// X3DViewAngleAspectCamera::Clone
//
//---------------------------------------------------------------
X3DViewAngleAspectCamera* X3DViewAngleAspectCamera::Clone() const
{
	X3DViewAngleAspectCamera* object = new X3DViewAngleAspectCamera(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DViewAngleAspectCamera::GetFOV
//
//---------------------------------------------------------------
float X3DViewAngleAspectCamera::GetFOV() const
{
	float fov;
	
	TQ3Status status = Q3ViewAngleAspectCamera_GetFOV(mObject, &fov);
	ThrowIf3DError(status);
	
	return fov;
}


//---------------------------------------------------------------
//
// X3DViewAngleAspectCamera::SetFOV
//
//---------------------------------------------------------------
void X3DViewAngleAspectCamera::SetFOV(float fov)
{
	PRECONDITION(fov > 0.0);
	
	TQ3Status status = Q3ViewAngleAspectCamera_SetFOV(mObject, fov);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewAngleAspectCamera::GetAspectRatio
//
//---------------------------------------------------------------
float X3DViewAngleAspectCamera::GetAspectRatio() const
{
	float ratio;
	
	TQ3Status status = Q3ViewAngleAspectCamera_GetAspectRatio(mObject, &ratio);
	ThrowIf3DError(status);
	
	return ratio;
}


//---------------------------------------------------------------
//
// X3DViewAngleAspectCamera::SetAspectRatio
//
//---------------------------------------------------------------
void X3DViewAngleAspectCamera::SetAspectRatio(float ratio)
{
	PRECONDITION(ratio > 0.0);
	
	TQ3Status status = Q3ViewAngleAspectCamera_SetAspectRatio(mObject, ratio);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewAngleAspectCamera::GetData
//
//---------------------------------------------------------------
TQ3ViewAngleAspectCameraData X3DViewAngleAspectCamera::GetData() const
{
	TQ3ViewAngleAspectCameraData data;
	
	TQ3Status status = Q3ViewAngleAspectCamera_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DViewAngleAspectCamera::SetData
//
//---------------------------------------------------------------
void X3DViewAngleAspectCamera::SetData(const TQ3ViewAngleAspectCameraData& data)
{
	TQ3Status status = Q3ViewAngleAspectCamera_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewAngleAspectCamera::IsCompatible 				[static]
//
//---------------------------------------------------------------
bool X3DViewAngleAspectCamera::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3CameraTypeViewAngleAspect);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DViewPlaneCamera
// ===================================================================================

//---------------------------------------------------------------
//
// X3DViewPlaneCamera::~X3DViewPlaneCamera
//
//---------------------------------------------------------------
X3DViewPlaneCamera::~X3DViewPlaneCamera()
{
}


//---------------------------------------------------------------
//
// X3DViewPlaneCamera::X3DViewPlaneCamera (TQ3ViewPlaneCameraData)
//
//---------------------------------------------------------------
X3DViewPlaneCamera::X3DViewPlaneCamera(const TQ3ViewPlaneCameraData& data) : X3DCamera(Q3ViewPlaneCamera_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewPlaneCamera::X3DViewPlaneCamera (TQ3CameraObject)
//
//---------------------------------------------------------------
X3DViewPlaneCamera::X3DViewPlaneCamera(TQ3CameraObject object) : X3DCamera(object)
{
	this->ConfirmType(kQ3CameraTypeViewPlane);
}


//---------------------------------------------------------------
//
// X3DViewPlaneCamera::Clone
//
//---------------------------------------------------------------
X3DViewPlaneCamera* X3DViewPlaneCamera::Clone() const
{
	X3DViewPlaneCamera* object = new X3DViewPlaneCamera(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DViewPlaneCamera::GetViewPlane
//
//---------------------------------------------------------------
float X3DViewPlaneCamera::GetViewPlane() const
{
	float plane;
	
	TQ3Status status = Q3ViewPlaneCamera_GetViewPlane(mObject, &plane);
	ThrowIf3DError(status);
	
	return plane;
}

	
//---------------------------------------------------------------
//
// X3DViewPlaneCamera::SetViewPlane
//
//---------------------------------------------------------------
void X3DViewPlaneCamera::SetViewPlane(float plane)
{
	TQ3Status status = Q3ViewPlaneCamera_SetViewPlane(mObject, plane);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewPlaneCamera::GetHalfSize
//
//---------------------------------------------------------------
X2DPoint X3DViewPlaneCamera::GetHalfSize() const
{
	X2DPoint size;
	
	TQ3Status status = Q3ViewPlaneCamera_GetHalfWidth(mObject, &size.x);
	ThrowIf3DError(status);
	
	status = Q3ViewPlaneCamera_GetHalfHeight(mObject, &size.y);
	ThrowIf3DError(status);
	
	return size;
}


//---------------------------------------------------------------
//
// X3DViewPlaneCamera::SetHalfSize
//
//---------------------------------------------------------------
void X3DViewPlaneCamera::SetHalfSize(const X2DPoint& size)
{
	PRECONDITION(size.x > 0.0);
	PRECONDITION(size.y > 0.0);
	
	TQ3Status status = Q3ViewPlaneCamera_SetHalfWidth(mObject, size.x);
	ThrowIf3DError(status);

	status = Q3ViewPlaneCamera_SetHalfHeight(mObject, size.y);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewPlaneCamera::GetCenter
//
//---------------------------------------------------------------
X2DPoint X3DViewPlaneCamera::GetCenter() const
{
	X2DPoint center;
	
	TQ3Status status = Q3ViewPlaneCamera_GetCenterX(mObject, &center.x);
	ThrowIf3DError(status);
	
	status = Q3ViewPlaneCamera_GetCenterY(mObject, &center.y);
	ThrowIf3DError(status);
	
	return center;
}

	
//---------------------------------------------------------------
//
// X3DViewPlaneCamera::SetCenter
//
//---------------------------------------------------------------
void X3DViewPlaneCamera::SetCenter(const X2DPoint& center)
{
	TQ3Status status = Q3ViewPlaneCamera_SetCenterX(mObject, center.x);
	ThrowIf3DError(status);

	status = Q3ViewPlaneCamera_SetCenterY(mObject, center.y);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewPlaneCamera::GetData
//
//---------------------------------------------------------------
TQ3ViewPlaneCameraData X3DViewPlaneCamera::GetData() const
{
	TQ3ViewPlaneCameraData data;
	
	TQ3Status status = Q3ViewPlaneCamera_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DViewPlaneCamera::SetData
//
//---------------------------------------------------------------
void X3DViewPlaneCamera::SetData(const TQ3ViewPlaneCameraData& data)
{
	TQ3Status status = Q3ViewPlaneCamera_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewPlaneCamera::IsCompatible 					[static]
//
//---------------------------------------------------------------
bool X3DViewPlaneCamera::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3CameraTypeViewPlane);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DOrthographicCamera
// ===================================================================================

//---------------------------------------------------------------
//
// X3DOrthographicCamera::~X3DOrthographicCamera
//
//---------------------------------------------------------------
X3DOrthographicCamera::~X3DOrthographicCamera()
{
}


//---------------------------------------------------------------
//
// X3DOrthographicCamera::X3DOrthographicCamera (TQ3OrthographicCameraData)
//
//---------------------------------------------------------------
X3DOrthographicCamera::X3DOrthographicCamera(const TQ3OrthographicCameraData& data) : X3DCamera(Q3OrthographicCamera_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DOrthographicCamera::X3DOrthographicCamera (TQ3CameraObject)
//
//---------------------------------------------------------------
X3DOrthographicCamera::X3DOrthographicCamera(TQ3CameraObject object) : X3DCamera(object)
{
	this->ConfirmType(kQ3CameraTypeOrthographic);
}


//---------------------------------------------------------------
//
// X3DOrthographicCamera::Clone
//
//---------------------------------------------------------------
X3DOrthographicCamera* X3DOrthographicCamera::Clone() const
{
	X3DOrthographicCamera* object = new X3DOrthographicCamera(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DOrthographicCamera::GetBox
//
//---------------------------------------------------------------
X2DRect X3DOrthographicCamera::GetBox() const
{
	X2DRect box;
	
	TQ3Status status = Q3OrthographicCamera_GetLeft(mObject, &box.left);
	ThrowIf3DError(status);
	
	status = Q3OrthographicCamera_GetRight(mObject, &box.right);
	ThrowIf3DError(status);
	
	status = Q3OrthographicCamera_GetTop(mObject, &box.top);
	ThrowIf3DError(status);
	
	status = Q3OrthographicCamera_GetBottom(mObject, &box.bottom);
	ThrowIf3DError(status);
	
	return box;
}


//---------------------------------------------------------------
//
// X3DOrthographicCamera::SetBox
//
//---------------------------------------------------------------
void X3DOrthographicCamera::SetBox(const X2DRect& box)
{
	TQ3Status status = Q3OrthographicCamera_SetLeft(mObject, box.left);
	ThrowIf3DError(status);
	
	status = Q3OrthographicCamera_SetRight(mObject, box.right);
	ThrowIf3DError(status);
	
	status = Q3OrthographicCamera_SetTop(mObject, box.top);
	ThrowIf3DError(status);
	
	status = Q3OrthographicCamera_SetBottom(mObject, box.bottom);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DOrthographicCamera::GetData
//
//---------------------------------------------------------------
TQ3OrthographicCameraData X3DOrthographicCamera::GetData() const
{
	TQ3OrthographicCameraData data;
	
	TQ3Status status = Q3OrthographicCamera_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DOrthographicCamera::SetData
//
//---------------------------------------------------------------
void X3DOrthographicCamera::SetData(const TQ3OrthographicCameraData& data)
{
	TQ3Status status = Q3OrthographicCamera_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DOrthographicCamera::IsCompatible 					[static]
//
//---------------------------------------------------------------
bool X3DOrthographicCamera::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3CameraTypeOrthographic);
	
	return compatible;
}


}	// namespace Whisper
