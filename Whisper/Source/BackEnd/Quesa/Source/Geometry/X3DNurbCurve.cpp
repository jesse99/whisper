/*
 *  File:       X3DNurbCurve.cpp
 *  Summary:	A 3D curve defined by a NURB.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DNurbCurve.cpp,v $
 *		Revision 1.4  2001/04/21 03:36:42  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:29:56  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:17:52  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DNurbCurve.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	class X3DNurbCurve
// ===================================================================================

//---------------------------------------------------------------
//
// X3DNurbCurve::~X3DNurbCurve
//
//---------------------------------------------------------------
X3DNurbCurve::~X3DNurbCurve()
{
}


//---------------------------------------------------------------
//
// X3DNurbCurve::X3DNurbCurve (TQ3NURBCurveData)
//
//---------------------------------------------------------------
X3DNurbCurve::X3DNurbCurve(const TQ3NURBCurveData& data) : X3DGeometry(Q3NURBCurve_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DNurbCurve::X3DNurbCurve (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DNurbCurve::X3DNurbCurve(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypeNURBCurve);
}


//---------------------------------------------------------------
//
// X3DNurbCurve::Clone
//
//---------------------------------------------------------------
X3DNurbCurve* X3DNurbCurve::Clone() const
{
	X3DNurbCurve* object = new X3DNurbCurve(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DNurbCurve::GetControlPoint
//
//---------------------------------------------------------------
TQ3RationalPoint4D X3DNurbCurve::GetControlPoint(uint32 index) const
{
	TQ3RationalPoint4D pt;
	
	TQ3Status status = Q3NURBCurve_GetControlPoint(mObject, index, &pt);
	ThrowIf3DError(status);
	
	return pt;
}

			
//---------------------------------------------------------------
//
// X3DNurbCurve::SetControlPoint
//
//---------------------------------------------------------------
void X3DNurbCurve::SetControlPoint(uint32 index, const TQ3RationalPoint4D& pt)
{
	TQ3Status status = Q3NURBCurve_SetControlPoint(mObject, index, &pt);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DNurbCurve::GetKnot
//
//---------------------------------------------------------------
float X3DNurbCurve::GetKnot(uint32 index) const
{
	float value;
	
	TQ3Status status = Q3NURBCurve_GetKnot(mObject, index, &value);
	ThrowIf3DError(status);
	
	return value;
}

			
//---------------------------------------------------------------
//
// X3DNurbCurve::SetKnot
//
//---------------------------------------------------------------
void X3DNurbCurve::SetKnot(uint32 index, float value)
{
	TQ3Status status = Q3NURBCurve_SetKnot(mObject, index, value);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DNurbCurve::GetData
//
//---------------------------------------------------------------
TQ3NURBCurveData X3DNurbCurve::GetData() const
{
	TQ3NURBCurveData data;
	
	TQ3Status status = Q3NURBCurve_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DNurbCurve::SetData
//
//---------------------------------------------------------------
void X3DNurbCurve::SetData(const TQ3NURBCurveData& data)
{
	TQ3Status status = Q3NURBCurve_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DNurbCurve::Submit									[static]
//
//---------------------------------------------------------------
void X3DNurbCurve::Submit(const TQ3NURBCurveData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3NURBCurve_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DNurbCurve::IsCompatible 							[static]
//	
//---------------------------------------------------------------
bool X3DNurbCurve::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypeNURBCurve);
	
	return compatible;
}


}	// namespace Whisper

