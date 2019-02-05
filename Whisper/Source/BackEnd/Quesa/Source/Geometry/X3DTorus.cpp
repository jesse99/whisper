/*
 *  File:       X3DTorus.cpp
 *  Summary:	A generalized 3D doughnut.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DTorus.cpp,v $
 *		Revision 1.4  2001/04/21 03:38:24  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:32:47  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:22:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DTorus.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	class X3DTorus
// ===================================================================================

//---------------------------------------------------------------
//
// X3DTorus::~X3DTorus
//
//---------------------------------------------------------------
X3DTorus::~X3DTorus()
{
}


//---------------------------------------------------------------
//
// X3DTorus::X3DTorus (TQ3TorusData)
//
//---------------------------------------------------------------
X3DTorus::X3DTorus(const TQ3TorusData& data) : X3DGeometry(Q3Torus_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTorus::X3DTorus (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DTorus::X3DTorus(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypeTorus);
}


//---------------------------------------------------------------
//
// X3DTorus::Clone
//
//---------------------------------------------------------------
X3DTorus* X3DTorus::Clone() const
{
	X3DTorus* object = new X3DTorus(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DTorus::GetOrigin
//
//---------------------------------------------------------------
X3DPoint X3DTorus::GetOrigin() const
{
	X3DPoint origin;
	
	TQ3Status status = Q3Torus_GetOrigin(mObject, origin);
	ThrowIf3DError(status);
	
	return origin;
}


//---------------------------------------------------------------
//
// X3DTorus::SetOrigin
//
//---------------------------------------------------------------
void X3DTorus::SetOrigin(const X3DPoint& origin)
{
	TQ3Status status = Q3Torus_SetOrigin(mObject, origin);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTorus::GetOrientation
//
//---------------------------------------------------------------
X3DVector X3DTorus::GetOrientation() const
{
	X3DVector orientation;
	
	TQ3Status status = Q3Torus_GetOrientation(mObject, orientation);
	ThrowIf3DError(status);
	
	return orientation;
}


//---------------------------------------------------------------
//
// X3DTorus::SetOrientation
//
//---------------------------------------------------------------
void X3DTorus::SetOrientation(const X3DVector& orientation)
{
	TQ3Status status = Q3Torus_SetOrientation(mObject, orientation);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTorus::GetMajorRadius
//
//---------------------------------------------------------------
X3DVector X3DTorus::GetMajorRadius() const
{
	X3DVector radius;
	
	TQ3Status status = Q3Torus_GetMajorRadius(mObject, radius);
	ThrowIf3DError(status);
	
	return radius;
}


//---------------------------------------------------------------
//
// X3DTorus::SetMajorRadius
//
//---------------------------------------------------------------
void X3DTorus::SetMajorRadius(const X3DVector& radius)
{
	TQ3Status status = Q3Torus_SetMajorRadius(mObject, radius);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTorus::GetMinorRadius
//
//---------------------------------------------------------------
X3DVector X3DTorus::GetMinorRadius() const
{
	X3DVector radius;
	
	TQ3Status status = Q3Torus_GetMinorRadius(mObject, radius);
	ThrowIf3DError(status);
	
	return radius;
}


//---------------------------------------------------------------
//
// X3DTorus::SetMinorRadius
//
//---------------------------------------------------------------
void X3DTorus::SetMinorRadius(const X3DVector& radius)
{
	TQ3Status status = Q3Torus_SetMinorRadius(mObject, radius);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTorus::GetRatio
//
//---------------------------------------------------------------
float X3DTorus::GetRatio() const
{
	float ratio;
	
	TQ3Status status = Q3Torus_GetRatio(mObject, &ratio);
	ThrowIf3DError(status);
	
	return ratio;
}

			
//---------------------------------------------------------------
//
// X3DTorus::SetRatio
//
//---------------------------------------------------------------
void X3DTorus::SetRatio(float ratio)
{
	TQ3Status status = Q3Torus_SetRatio(mObject, ratio);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTorus::GetData
//
//---------------------------------------------------------------
TQ3TorusData X3DTorus::GetData() const
{
	TQ3TorusData data;
	
	TQ3Status status = Q3Torus_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DTorus::SetData
//
//---------------------------------------------------------------
void X3DTorus::SetData(const TQ3TorusData& data)
{
	TQ3Status status = Q3Torus_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTorus::Submit (TQ3TorusData, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DTorus::Submit(const TQ3TorusData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Torus_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTorus::IsCompatible					 			[static]
//	
//---------------------------------------------------------------
bool X3DTorus::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypeTorus);
	
	return compatible;
}


}	// namespace Whisper
