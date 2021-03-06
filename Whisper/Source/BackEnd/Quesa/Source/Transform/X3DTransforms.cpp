/*
 *  File:       X3DTransforms.cpp
 *  Summary:	QuickDraw 3D transform classes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DTransforms.cpp,v $
 *		Revision 1.4  2001/04/21 03:38:34  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:32:56  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:22:39  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DTransforms.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateRotateTransform 
//
//---------------------------------------------------------------
static TQ3TransformObject CreateRotateTransform(const TQ3Axis& axis, float angle)
{
	TQ3RotateTransformData data;
	data.axis    = axis;
	data.radians = angle;
	
	TQ3TransformObject object = Q3RotateTransform_New(&data);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateRotateAboutPointTransform
//
//---------------------------------------------------------------
static TQ3TransformObject CreateRotateAboutPointTransform(const TQ3Axis& axis, const X3DPoint& about, float angle)
{
	TQ3RotateAboutPointTransformData data;
	data.axis    = axis;
	data.radians = angle;
	data.about   = about;
	
	TQ3TransformObject object = Q3RotateAboutPointTransform_New(&data);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateRotateAboutAxisTransform
//
//---------------------------------------------------------------
static TQ3TransformObject CreateRotateAboutAxisTransform(const TQ3Point3D& origin, const X3DVector& orientation, float angle)
{
	TQ3RotateAboutAxisTransformData data;
	data.origin      = origin;
	data.orientation = orientation;
	data.radians     = angle;
	
	TQ3TransformObject object = Q3RotateAboutAxisTransform_New(&data);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateScaleTransform 
//
//---------------------------------------------------------------
static TQ3TransformObject CreateScaleTransform(float dx, float dy, float dz)
{
	TQ3Vector3D data;
	data.x = dx;
	data.y = dy;
	data.z = dz;
	
	TQ3TransformObject object = Q3ScaleTransform_New(&data);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateTranslateTransform 
//
//---------------------------------------------------------------
static TQ3TransformObject CreateTranslateTransform(float dx, float dy, float dz)
{
	TQ3Vector3D data;
	data.x = dx;
	data.y = dy;
	data.z = dz;
	
	TQ3TransformObject object = Q3TranslateTransform_New(&data);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DMatrixTransform
// ===================================================================================

//---------------------------------------------------------------
//
// X3DMatrixTransform::~X3DMatrixTransform
//
//---------------------------------------------------------------
X3DMatrixTransform::~X3DMatrixTransform()
{
}


//---------------------------------------------------------------
//
// X3DMatrixTransform::X3DMatrixTransform (X3DMatrix)
//
//---------------------------------------------------------------
X3DMatrixTransform::X3DMatrixTransform(const X3DMatrix& matrix) : X3DTransform(Q3MatrixTransform_New(matrix))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMatrixTransform::X3DMatrixTransform (TQ3TransformObject)
//
//---------------------------------------------------------------
X3DMatrixTransform::X3DMatrixTransform(TQ3TransformObject object) : X3DTransform(object)
{
	this->ConfirmType(kQ3TransformTypeMatrix);
}


//---------------------------------------------------------------
//
// X3DMatrixTransform::Clone
//
//---------------------------------------------------------------
X3DMatrixTransform* X3DMatrixTransform::Clone() const
{
	X3DMatrixTransform* object = new X3DMatrixTransform(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DMatrixTransform::Set
//
//---------------------------------------------------------------
void X3DMatrixTransform::Set(const X3DMatrix& matrix)
{
	TQ3Status status = Q3MatrixTransform_Set(mObject, matrix);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMatrixTransform::Submit							[static]
//
//---------------------------------------------------------------
void X3DMatrixTransform::Submit(const X3DMatrix& matrix, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3MatrixTransform_Submit(matrix, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMatrixTransform::IsCompatible				 		[static]
//	
//---------------------------------------------------------------
bool X3DMatrixTransform::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3TransformTypeMatrix);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DRotateTransform
// ===================================================================================

//---------------------------------------------------------------
//
// X3DRotateTransform::~X3DRotateTransform
//
//---------------------------------------------------------------
X3DRotateTransform::~X3DRotateTransform()
{
}


//---------------------------------------------------------------
//
// X3DRotateTransform::X3DRotateTransform (TQ3Axis, float)
//
//---------------------------------------------------------------
X3DRotateTransform::X3DRotateTransform(const TQ3Axis& axis, float angle) : X3DTransform(CreateRotateTransform(axis, angle))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateTransform::X3DRotateTransform (TQ3RotateTransformData)
//
//---------------------------------------------------------------
X3DRotateTransform::X3DRotateTransform(const TQ3RotateTransformData& data) : X3DTransform(Q3RotateTransform_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateTransform::X3DRotateTransform (TQ3TransformObject)
//
//---------------------------------------------------------------
X3DRotateTransform::X3DRotateTransform(TQ3TransformObject object) : X3DTransform(object)
{
	this->ConfirmType(kQ3TransformTypeRotate);
}


//---------------------------------------------------------------
//
// X3DRotateTransform::Clone
//
//---------------------------------------------------------------
X3DRotateTransform* X3DRotateTransform::Clone() const
{
	X3DRotateTransform* object = new X3DRotateTransform(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DRotateTransform::GetAxis
//
//---------------------------------------------------------------
TQ3Axis X3DRotateTransform::GetAxis() const
{
	TQ3Axis axis;
	
	TQ3Status status = Q3RotateTransform_GetAxis(mObject, &axis);
	ThrowIf3DError(status);
	
	return axis;
}


//---------------------------------------------------------------
//
// X3DRotateTransform::SetAxis
//
//---------------------------------------------------------------
void X3DRotateTransform::SetAxis(const TQ3Axis& axis)
{
	TQ3Status status = Q3RotateTransform_SetAxis(mObject, axis);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateTransform::GetAngle
//
//---------------------------------------------------------------
float X3DRotateTransform::GetAngle() const
{
	float angle;
	
	TQ3Status status = Q3RotateTransform_GetAngle(mObject, &angle);
	ThrowIf3DError(status);
	
	return angle;
}


//---------------------------------------------------------------
//
// X3DRotateTransform::SetAngle
//
//---------------------------------------------------------------
void X3DRotateTransform::SetAngle(float angle)
{
	TQ3Status status = Q3RotateTransform_SetAngle(mObject, angle);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateTransform::GetData
//
//---------------------------------------------------------------
TQ3RotateTransformData X3DRotateTransform::GetData() const
{
	TQ3RotateTransformData data;
	
	TQ3Status status = Q3RotateTransform_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DRotateTransform::SetData
//
//---------------------------------------------------------------
void X3DRotateTransform::SetData(const TQ3RotateTransformData& data)
{
	TQ3Status status = Q3RotateTransform_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateTransform::Submit (TQ3Axis, float, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DRotateTransform::Submit(const TQ3Axis& axis, float angle, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3RotateTransformData data;
	data.axis    = axis;
	data.radians = angle;

	TQ3Status status = Q3RotateTransform_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateTransform::Submit (TQ3RotateTransformData, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DRotateTransform::Submit(const TQ3RotateTransformData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3RotateTransform_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateTransform::IsCompatible				 		[static]
//	
//---------------------------------------------------------------
bool X3DRotateTransform::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3TransformTypeRotate);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DRotateAboutPointTransform
// ===================================================================================

//---------------------------------------------------------------
//
// X3DRotateAboutPointTransform::~X3DRotateAboutPointTransform
//
//---------------------------------------------------------------
X3DRotateAboutPointTransform::~X3DRotateAboutPointTransform()
{
}


//---------------------------------------------------------------
//
// X3DRotateAboutPointTransform::X3DRotateAboutPointTransform (TQ3Axis, X3DPoint, float)
//
//---------------------------------------------------------------
X3DRotateAboutPointTransform::X3DRotateAboutPointTransform(const TQ3Axis& axis, const X3DPoint& about, float angle) : X3DTransform(CreateRotateAboutPointTransform(axis, about, angle))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateAboutPointTransform::X3DRotateAboutPointTransform (TQ3RotateAboutPointTransformData)
//
//---------------------------------------------------------------
X3DRotateAboutPointTransform::X3DRotateAboutPointTransform(const TQ3RotateAboutPointTransformData& data) : X3DTransform(Q3RotateAboutPointTransform_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateAboutPointTransform::X3DRotateAboutPointTransform (TQ3TransformObject)
//
//---------------------------------------------------------------
X3DRotateAboutPointTransform::X3DRotateAboutPointTransform(TQ3TransformObject object) : X3DTransform(object)
{
	this->ConfirmType(kQ3TransformTypeRotateAboutPoint);
}


//---------------------------------------------------------------
//
// X3DRotateAboutPointTransform::Clone
//
//---------------------------------------------------------------
X3DRotateAboutPointTransform* X3DRotateAboutPointTransform::Clone() const
{
	X3DRotateAboutPointTransform* object = new X3DRotateAboutPointTransform(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DRotateAboutPointTransform::GetAxis
//
//---------------------------------------------------------------
TQ3Axis X3DRotateAboutPointTransform::GetAxis() const
{
	TQ3Axis axis;
	
	TQ3Status status = Q3RotateAboutPointTransform_GetAxis(mObject, &axis);
	ThrowIf3DError(status);
	
	return axis;
}


//---------------------------------------------------------------
//
// X3DRotateAboutPointTransform::SetAxis
//
//---------------------------------------------------------------
void X3DRotateAboutPointTransform::SetAxis(const TQ3Axis& axis)
{
	TQ3Status status = Q3RotateAboutPointTransform_SetAxis(mObject, axis);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateAboutPointTransform::GetAngle
//
//---------------------------------------------------------------
float X3DRotateAboutPointTransform::GetAngle() const
{
	float angle;
	
	TQ3Status status = Q3RotateAboutPointTransform_GetAngle(mObject, &angle);
	ThrowIf3DError(status);
	
	return angle;
}


//---------------------------------------------------------------
//
// X3DRotateAboutPointTransform::SetAngle
//
//---------------------------------------------------------------
void X3DRotateAboutPointTransform::SetAngle(float angle)
{
	TQ3Status status = Q3RotateAboutPointTransform_SetAngle(mObject, angle);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateAboutPointTransform::GetAboutPoint
//
//---------------------------------------------------------------
X3DPoint X3DRotateAboutPointTransform::GetAboutPoint() const
{
	X3DPoint pt;
	
	TQ3Status status = Q3RotateAboutPointTransform_GetAboutPoint(mObject, pt);
	ThrowIf3DError(status);
	
	return pt;
}


//---------------------------------------------------------------
//
// X3DRotateAboutPointTransform::SetAboutPoint
//
//---------------------------------------------------------------
void X3DRotateAboutPointTransform::SetAboutPoint(const X3DPoint& pt)
{
	TQ3Status status = Q3RotateAboutPointTransform_SetAboutPoint(mObject, pt);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateAboutPointTransform::GetData
//
//---------------------------------------------------------------
TQ3RotateAboutPointTransformData X3DRotateAboutPointTransform::GetData() const
{
	TQ3RotateAboutPointTransformData data;
	
	TQ3Status status = Q3RotateAboutPointTransform_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DRotateAboutPointTransform::SetData
//
//---------------------------------------------------------------
void X3DRotateAboutPointTransform::SetData(const TQ3RotateAboutPointTransformData& data)
{
	TQ3Status status = Q3RotateAboutPointTransform_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateAboutPointTransform::Submit (TQ3Axis, X3DPoint, float, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DRotateAboutPointTransform::Submit(const TQ3Axis& axis, const X3DPoint& about, float angle, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3RotateAboutPointTransformData data;
	data.axis    = axis;
	data.radians = angle;
	data.about   = about;
	
	TQ3Status status = Q3RotateAboutPointTransform_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateAboutPointTransform::Submit (TQ3RotateAboutPointTransformData, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DRotateAboutPointTransform::Submit(const TQ3RotateAboutPointTransformData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3RotateAboutPointTransform_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateAboutPointTransform::IsCompatible			[static]
//	
//---------------------------------------------------------------
bool X3DRotateAboutPointTransform::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3TransformTypeRotateAboutPoint);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DRotateAboutAxisTransform
// ===================================================================================

//---------------------------------------------------------------
//
// X3DRotateAboutAxisTransform::~X3DRotateAboutAxisTransform
//
//---------------------------------------------------------------
X3DRotateAboutAxisTransform::~X3DRotateAboutAxisTransform()
{
}


//---------------------------------------------------------------
//
// X3DRotateAboutAxisTransform::X3DRotateAboutAxisTransform (TQ3Point3D, X3DVector, float)
//
//---------------------------------------------------------------
X3DRotateAboutAxisTransform::X3DRotateAboutAxisTransform(const TQ3Point3D& origin, const X3DVector& orientation, float angle) : X3DTransform(CreateRotateAboutAxisTransform(origin, orientation, angle))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateAboutAxisTransform::X3DRotateAboutAxisTransform (TQ3RotateAboutAxisTransformData)
//
//---------------------------------------------------------------
X3DRotateAboutAxisTransform::X3DRotateAboutAxisTransform(const TQ3RotateAboutAxisTransformData& data) : X3DTransform(Q3RotateAboutAxisTransform_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateAboutAxisTransform::X3DRotateAboutAxisTransform (TQ3TransformObject)
//
//---------------------------------------------------------------
X3DRotateAboutAxisTransform::X3DRotateAboutAxisTransform(TQ3TransformObject object) : X3DTransform(object)
{
	this->ConfirmType(kQ3TransformTypeRotateAboutAxis);
}


//---------------------------------------------------------------
//
// X3DRotateAboutAxisTransform::Clone
//
//---------------------------------------------------------------
X3DRotateAboutAxisTransform* X3DRotateAboutAxisTransform::Clone() const
{
	X3DRotateAboutAxisTransform* object = new X3DRotateAboutAxisTransform(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DRotateAboutAxisTransform::GetOrientation
//
//---------------------------------------------------------------
X3DVector X3DRotateAboutAxisTransform::GetOrientation() const
{
	X3DVector axis;
	
	TQ3Status status = Q3RotateAboutAxisTransform_GetOrientation(mObject, axis);
	ThrowIf3DError(status);
	
	return axis;
}


//---------------------------------------------------------------
//
// X3DRotateAboutAxisTransform::SetOrientation
//
//---------------------------------------------------------------
void X3DRotateAboutAxisTransform::SetOrientation(const X3DVector& axis)
{
	TQ3Status status = Q3RotateAboutAxisTransform_SetOrientation(mObject, axis);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateAboutAxisTransform::GetAngle
//
//---------------------------------------------------------------
float X3DRotateAboutAxisTransform::GetAngle() const
{
	float angle;
	
	TQ3Status status = Q3RotateAboutAxisTransform_GetAngle(mObject, &angle);
	ThrowIf3DError(status);
	
	return angle;
}


//---------------------------------------------------------------
//
// X3DRotateAboutAxisTransform::SetAngle
//
//---------------------------------------------------------------
void X3DRotateAboutAxisTransform::SetAngle(float angle)
{
	TQ3Status status = Q3RotateAboutAxisTransform_SetAngle(mObject, angle);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateAboutAxisTransform::GetOrigin
//
//---------------------------------------------------------------
X3DPoint X3DRotateAboutAxisTransform::GetOrigin() const
{
	X3DPoint origin;
	
	TQ3Status status = Q3RotateAboutAxisTransform_GetOrigin(mObject, origin);
	ThrowIf3DError(status);
	
	return origin;
}


//---------------------------------------------------------------
//
// X3DRotateAboutAxisTransform::SetOrigin
//
//---------------------------------------------------------------
void X3DRotateAboutAxisTransform::SetOrigin(const X3DPoint& origin)
{
	TQ3Status status = Q3RotateAboutAxisTransform_SetOrigin(mObject, origin);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateAboutAxisTransform::GetData
//
//---------------------------------------------------------------
TQ3RotateAboutAxisTransformData X3DRotateAboutAxisTransform::GetData() const
{
	TQ3RotateAboutAxisTransformData data;
	
	TQ3Status status = Q3RotateAboutAxisTransform_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DRotateAboutAxisTransform::SetData
//
//---------------------------------------------------------------
void X3DRotateAboutAxisTransform::SetData(const TQ3RotateAboutAxisTransformData& data)
{
	TQ3Status status = Q3RotateAboutAxisTransform_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateAboutAxisTransform::Submit (TQ3Point3D, X3DVector, float, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DRotateAboutAxisTransform::Submit(const TQ3Point3D& origin, const X3DVector& orientation, float angle, TQ3ViewObject view)
{	
	PRECONDITION(view != nil);
	
	TQ3RotateAboutAxisTransformData data;
	data.origin      = origin;
	data.orientation = orientation;
	data.radians     = angle;
	
	TQ3Status status = Q3RotateAboutAxisTransform_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateAboutAxisTransform::Submit (TQ3RotateAboutAxisTransformData, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DRotateAboutAxisTransform::Submit(const TQ3RotateAboutAxisTransformData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3RotateAboutAxisTransform_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRotateAboutAxisTransform::IsCompatible			[static]
//	
//---------------------------------------------------------------
bool X3DRotateAboutAxisTransform::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3TransformTypeRotateAboutAxis);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DScaleTransform
// ===================================================================================

//---------------------------------------------------------------
//
// X3DScaleTransform::~X3DScaleTransform
//
//---------------------------------------------------------------
X3DScaleTransform::~X3DScaleTransform()
{
}


//---------------------------------------------------------------
//
// X3DScaleTransform::X3DScaleTransform (float, float, float)
//
//---------------------------------------------------------------
X3DScaleTransform::X3DScaleTransform(float dx, float dy, float dz) : X3DTransform(CreateScaleTransform(dx, dy, dz))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DScaleTransform::X3DScaleTransform (X3DVector)
//
//---------------------------------------------------------------
X3DScaleTransform::X3DScaleTransform(const X3DVector& scale) : X3DTransform(Q3ScaleTransform_New(scale))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DScaleTransform::X3DScaleTransform (TQ3TransformObject)
//
//---------------------------------------------------------------
X3DScaleTransform::X3DScaleTransform(TQ3TransformObject object) : X3DTransform(object)
{
	this->ConfirmType(kQ3TransformTypeScale);
}


//---------------------------------------------------------------
//
// X3DScaleTransform::Clone
//
//---------------------------------------------------------------
X3DScaleTransform* X3DScaleTransform::Clone() const
{
	X3DScaleTransform* object = new X3DScaleTransform(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DScaleTransform::Get
//
//---------------------------------------------------------------
X3DVector X3DScaleTransform::Get() const
{
	X3DVector scale;
	
	TQ3Status status = Q3ScaleTransform_Get(mObject, scale);
	ThrowIf3DError(status);
	
	return scale;
}


//---------------------------------------------------------------
//
// X3DScaleTransform::Set
//
//---------------------------------------------------------------
void X3DScaleTransform::Set(const X3DVector& scale)
{
	TQ3Status status = Q3ScaleTransform_Set(mObject, scale);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DScaleTransform::Submit (float, float, float, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DScaleTransform::Submit(float dx, float dy, float dz, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	X3DVector scale(dx, dy, dz);
	
	TQ3Status status = Q3ScaleTransform_Submit(scale, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DScaleTransform::Submit (X3DVector, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DScaleTransform::Submit(const X3DVector& scale, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3ScaleTransform_Submit(scale, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DScaleTransform::IsCompatible						[static]
//	
//---------------------------------------------------------------
bool X3DScaleTransform::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3TransformTypeScale);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DTranslateTransform
// ===================================================================================

//---------------------------------------------------------------
//
// X3DTranslateTransform::~X3DTranslateTransform
//
//---------------------------------------------------------------
X3DTranslateTransform::~X3DTranslateTransform()
{
}


//---------------------------------------------------------------
//
// X3DTranslateTransform::X3DTranslateTransform (float, float, float)
//
//---------------------------------------------------------------
X3DTranslateTransform::X3DTranslateTransform(float dx, float dy, float dz) : X3DTransform(CreateTranslateTransform(dx, dy, dz))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTranslateTransform::X3DTranslateTransform (X3DVector)
//
//---------------------------------------------------------------
X3DTranslateTransform::X3DTranslateTransform(const X3DVector& translate) : X3DTransform(Q3TranslateTransform_New(translate))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTranslateTransform::X3DTranslateTransform (TQ3TransformObject)
//
//---------------------------------------------------------------
X3DTranslateTransform::X3DTranslateTransform(TQ3TransformObject object) : X3DTransform(object)
{
	this->ConfirmType(kQ3TransformTypeTranslate);
}


//---------------------------------------------------------------
//
// X3DTranslateTransform::Clone
//
//---------------------------------------------------------------
X3DTranslateTransform* X3DTranslateTransform::Clone() const
{
	X3DTranslateTransform* object = new X3DTranslateTransform(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DTranslateTransform::Get
//
//---------------------------------------------------------------
X3DVector X3DTranslateTransform::Get() const
{
	X3DVector translate;
	
	TQ3Status status = Q3TranslateTransform_Get(mObject, translate);
	ThrowIf3DError(status);
	
	return translate;
}


//---------------------------------------------------------------
//
// X3DTranslateTransform::Set
//
//---------------------------------------------------------------
void X3DTranslateTransform::Set(const X3DVector& translate)
{
	TQ3Status status = Q3TranslateTransform_Set(mObject, translate);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTranslateTransform::Submit (float, float, float, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DTranslateTransform::Submit(float dx, float dy, float dz, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	X3DVector translate(dx, dy, dz);
	
	TQ3Status status = Q3TranslateTransform_Submit(translate, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTranslateTransform::Submit (X3DVector, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DTranslateTransform::Submit(const X3DVector& translate, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3TranslateTransform_Submit(translate, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTranslateTransform::IsCompatible					[static]
//	
//---------------------------------------------------------------
bool X3DTranslateTransform::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3TransformTypeTranslate);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DQuaternionTransform
// ===================================================================================

//---------------------------------------------------------------
//
// X3DQuaternionTransform::~X3DQuaternionTransform
//
//---------------------------------------------------------------
X3DQuaternionTransform::~X3DQuaternionTransform()
{
}


//---------------------------------------------------------------
//
// X3DQuaternionTransform::X3DQuaternionTransform (TQ3Quaternion)
//
//---------------------------------------------------------------
X3DQuaternionTransform::X3DQuaternionTransform(const TQ3Quaternion& quaternion) : X3DTransform(Q3QuaternionTransform_New(&quaternion))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DQuaternionTransform::X3DQuaternionTransform (TQ3TransformObject)
//
//---------------------------------------------------------------
X3DQuaternionTransform::X3DQuaternionTransform(TQ3TransformObject object) : X3DTransform(object)
{
	this->ConfirmType(kQ3TransformTypeQuaternion);
}


//---------------------------------------------------------------
//
// X3DQuaternionTransform::Clone
//
//---------------------------------------------------------------
X3DQuaternionTransform* X3DQuaternionTransform::Clone() const
{
	X3DQuaternionTransform* object = new X3DQuaternionTransform(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DQuaternionTransform::Get
//
//---------------------------------------------------------------
TQ3Quaternion X3DQuaternionTransform::Get() const
{
	TQ3Quaternion quaternion;
	
	TQ3Status status = Q3QuaternionTransform_Get(mObject, &quaternion);
	ThrowIf3DError(status);
	
	return quaternion;
}


//---------------------------------------------------------------
//
// X3DQuaternionTransform::Set
//
//---------------------------------------------------------------
void X3DQuaternionTransform::Set(const TQ3Quaternion& quaternion)
{
	TQ3Status status = Q3QuaternionTransform_Set(mObject, &quaternion);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DQuaternionTransform::Submit						[static]
//
//---------------------------------------------------------------
void X3DQuaternionTransform::Submit(const TQ3Quaternion& quaternion, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3QuaternionTransform_Submit(&quaternion, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DQuaternionTransform::IsCompatible					[static]
//	
//---------------------------------------------------------------
bool X3DQuaternionTransform::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3TransformTypeQuaternion);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DResetTransform
// ===================================================================================

//---------------------------------------------------------------
//
// X3DResetTransform::~X3DResetTransform
//
//---------------------------------------------------------------
X3DResetTransform::~X3DResetTransform()
{
}


//---------------------------------------------------------------
//
// X3DResetTransform::X3DResetTransform ()
//
//---------------------------------------------------------------
X3DResetTransform::X3DResetTransform() : X3DTransform(Q3ResetTransform_New())
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DResetTransform::X3DResetTransform (TQ3TransformObject)
//
//---------------------------------------------------------------
X3DResetTransform::X3DResetTransform(TQ3TransformObject object) : X3DTransform(object)
{
	this->ConfirmType(kQ3TransformTypeReset);
}


//---------------------------------------------------------------
//
// X3DResetTransform::Clone
//
//---------------------------------------------------------------
X3DResetTransform* X3DResetTransform::Clone() const
{
	X3DResetTransform* object = new X3DResetTransform(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DResetTransform::Submit							[static]
//
//---------------------------------------------------------------
void X3DResetTransform::Submit(TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3ResetTransform_Submit(view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DResetTransform::IsCompatible						[static]
//	
//---------------------------------------------------------------
bool X3DResetTransform::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3TransformTypeReset);
	
	return compatible;
}


}	// namespace Whisper
