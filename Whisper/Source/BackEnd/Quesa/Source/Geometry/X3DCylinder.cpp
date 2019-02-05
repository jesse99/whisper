/*
 *  File:       X3DCylinder.cpp
 *  Summary:	A generalized 3D cylinder.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DCylinder.cpp,v $
 *		Revision 1.4  2001/04/21 03:35:27  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:28:00  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:14:42  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DCylinder.h>

#include <X3DUtils.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class X3DCylinder
// ===================================================================================

//---------------------------------------------------------------
//
// X3DCylinder::~X3DCylinder
//
//---------------------------------------------------------------
X3DCylinder::~X3DCylinder()
{
}


//---------------------------------------------------------------
//
// X3DCylinder::X3DCylinder (TQ3CylinderData)
//
//---------------------------------------------------------------
X3DCylinder::X3DCylinder(const TQ3CylinderData& data) : X3DGeometry(Q3Cylinder_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCylinder::X3DCylinder (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DCylinder::X3DCylinder(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypeCylinder);
}


//---------------------------------------------------------------
//
// X3DCylinder::Clone
//
//---------------------------------------------------------------
X3DCylinder* X3DCylinder::Clone() const
{
	X3DCylinder* object = new X3DCylinder(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DCylinder::GetOrigin
//
//---------------------------------------------------------------
X3DPoint X3DCylinder::GetOrigin() const
{
	X3DPoint origin;
	
	TQ3Status status = Q3Cylinder_GetOrigin(mObject, origin);
	ThrowIf3DError(status);
	
	return origin;
}


//---------------------------------------------------------------
//
// X3DCylinder::SetOrigin
//
//---------------------------------------------------------------
void X3DCylinder::SetOrigin(const X3DPoint& origin)
{
	TQ3Status status = Q3Cylinder_SetOrigin(mObject, origin);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCylinder::GetOrientation
//
//---------------------------------------------------------------
X3DVector X3DCylinder::GetOrientation() const
{
	X3DVector orientation;
	
	TQ3Status status = Q3Cylinder_GetOrientation(mObject, orientation);
	ThrowIf3DError(status);
	
	return orientation;
}


//---------------------------------------------------------------
//
// X3DCylinder::SetOrientation
//
//---------------------------------------------------------------
void X3DCylinder::SetOrientation(const X3DVector& orientation)
{
	TQ3Status status = Q3Cylinder_SetOrientation(mObject, orientation);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCylinder::GetMajorRadius
//
//---------------------------------------------------------------
X3DVector X3DCylinder::GetMajorRadius() const
{
	X3DVector radius;
	
	TQ3Status status = Q3Cylinder_GetMajorRadius(mObject, radius);
	ThrowIf3DError(status);
	
	return radius;
}


//---------------------------------------------------------------
//
// X3DCylinder::SetMajorRadius
//
//---------------------------------------------------------------
void X3DCylinder::SetMajorRadius(const X3DVector& radius)
{
	TQ3Status status = Q3Cylinder_SetMajorRadius(mObject, radius);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCylinder::GetMinorRadius
//
//---------------------------------------------------------------
X3DVector X3DCylinder::GetMinorRadius() const
{
	X3DVector radius;
	
	TQ3Status status = Q3Cylinder_GetMinorRadius(mObject, radius);
	ThrowIf3DError(status);
	
	return radius;
}


//---------------------------------------------------------------
//
// X3DCylinder::SetMinorRadius
//
//---------------------------------------------------------------
void X3DCylinder::SetMinorRadius(const X3DVector& radius)
{
	TQ3Status status = Q3Cylinder_SetMinorRadius(mObject, radius);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCylinder::GetData
//
//---------------------------------------------------------------
TQ3CylinderData X3DCylinder::GetData() const
{
	TQ3CylinderData data;
	
	TQ3Status status = Q3Cylinder_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DCylinder::SetData
//
//---------------------------------------------------------------
void X3DCylinder::SetData(const TQ3CylinderData& data)
{
	TQ3Status status = Q3Cylinder_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCylinder::Submit (TQ3CylinderData, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DCylinder::Submit(const TQ3CylinderData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Cylinder_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCylinder::IsCompatible 							[static]
//
//---------------------------------------------------------------
bool X3DCylinder::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypeCylinder);
	
	return compatible;
}


}	// namespace Whisper

