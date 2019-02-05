/*
 *  File:       X3DCone.cpp
 *  Summary:	A generalized 3D cone.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DCone.cpp,v $
 *		Revision 1.4  2001/04/21 03:35:24  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:27:57  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:14:34  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DCone.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	class X3DCone
// ===================================================================================

//---------------------------------------------------------------
//
// X3DCone::~X3DCone
//
//---------------------------------------------------------------
X3DCone::~X3DCone()
{
}


//---------------------------------------------------------------
//
// X3DCone::X3DCone (TQ3ConeData)
//
//---------------------------------------------------------------
X3DCone::X3DCone(const TQ3ConeData& data) : X3DGeometry(Q3Cone_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCone::X3DCone (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DCone::X3DCone(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypeCone);
}


//---------------------------------------------------------------
//
// X3DCone::Clone
//
//---------------------------------------------------------------
X3DCone* X3DCone::Clone() const
{
	X3DCone* object = new X3DCone(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DCone::GetOrigin
//
//---------------------------------------------------------------
X3DPoint X3DCone::GetOrigin() const
{
	X3DPoint origin;
	
	TQ3Status status = Q3Cone_GetOrigin(mObject, origin);
	ThrowIf3DError(status);
	
	return origin;
}


//---------------------------------------------------------------
//
// X3DCone::SetOrigin
//
//---------------------------------------------------------------
void X3DCone::SetOrigin(const X3DPoint& origin)
{
	TQ3Status status = Q3Cone_SetOrigin(mObject, origin);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCone::GetOrientation
//
//---------------------------------------------------------------
X3DVector X3DCone::GetOrientation() const
{
	X3DVector orientation;
	
	TQ3Status status = Q3Cone_GetOrientation(mObject, orientation);
	ThrowIf3DError(status);
	
	return orientation;
}


//---------------------------------------------------------------
//
// X3DCone::SetOrientation
//
//---------------------------------------------------------------
void X3DCone::SetOrientation(const X3DVector& orientation)
{
	TQ3Status status = Q3Cone_SetOrientation(mObject, orientation);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCone::GetMajorRadius
//
//---------------------------------------------------------------
X3DVector X3DCone::GetMajorRadius() const
{
	X3DVector radius;
	
	TQ3Status status = Q3Cone_GetMajorRadius(mObject, radius);
	ThrowIf3DError(status);
	
	return radius;
}


//---------------------------------------------------------------
//
// X3DCone::SetMajorRadius
//
//---------------------------------------------------------------
void X3DCone::SetMajorRadius(const X3DVector& radius)
{
	TQ3Status status = Q3Cone_SetMajorRadius(mObject, radius);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCone::GetMinorRadius
//
//---------------------------------------------------------------
X3DVector X3DCone::GetMinorRadius() const
{
	X3DVector radius;
	
	TQ3Status status = Q3Cone_GetMinorRadius(mObject, radius);
	ThrowIf3DError(status);
	
	return radius;
}


//---------------------------------------------------------------
//
// X3DCone::SetMinorRadius
//
//---------------------------------------------------------------
void X3DCone::SetMinorRadius(const X3DVector& radius)
{
	TQ3Status status = Q3Cone_SetMinorRadius(mObject, radius);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCone::GetData
//
//---------------------------------------------------------------
TQ3ConeData X3DCone::GetData() const
{
	TQ3ConeData data;
	
	TQ3Status status = Q3Cone_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DCone::SetData
//
//---------------------------------------------------------------
void X3DCone::SetData(const TQ3ConeData& data)
{
	TQ3Status status = Q3Cone_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCone::Submit (TQ3ConeData, TQ3ViewObject)			[static]
//
//---------------------------------------------------------------
void X3DCone::Submit(const TQ3ConeData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Cone_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCone::IsCompatible 								[static]
//
//---------------------------------------------------------------
bool X3DCone::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypeCone);
	
	return compatible;
}


}	// namespace Whisper
