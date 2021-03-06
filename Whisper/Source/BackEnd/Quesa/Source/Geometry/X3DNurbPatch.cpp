/*
 *  File:       X3DNurbPatch.cpp
 *  Summary:	A 3D surface defined by a NURB.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DNurbPatch.cpp,v $
 *		Revision 1.4  2001/04/21 03:36:46  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:30:04  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:18:00  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DNurbPatch.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	class X3DNurbPatch
// ===================================================================================

//---------------------------------------------------------------
//
// X3DNurbPatch::~X3DNurbPatch
//
//---------------------------------------------------------------
X3DNurbPatch::~X3DNurbPatch()
{
}


//---------------------------------------------------------------
//
// X3DNurbPatch::X3DNurbPatch (TQ3NURBPatchData)
//
//---------------------------------------------------------------
X3DNurbPatch::X3DNurbPatch(const TQ3NURBPatchData& data) : X3DGeometry(Q3NURBPatch_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DNurbPatch::X3DNurbPatch (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DNurbPatch::X3DNurbPatch(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypeNURBPatch);
}


//---------------------------------------------------------------
//
// X3DNurbPatch::Clone
//
//---------------------------------------------------------------
X3DNurbPatch* X3DNurbPatch::Clone() const
{
	X3DNurbPatch* object = new X3DNurbPatch(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DNurbPatch::GetControlPoint
//
//---------------------------------------------------------------
TQ3RationalPoint4D X3DNurbPatch::GetControlPoint(uint32 row, uint32 col) const
{
	TQ3RationalPoint4D pt;
	
	TQ3Status status = Q3NURBPatch_GetControlPoint(mObject, row, col, &pt);
	ThrowIf3DError(status);
	
	return pt;
}

			
//---------------------------------------------------------------
//
// X3DNurbPatch::SetControlPoint
//
//---------------------------------------------------------------
void X3DNurbPatch::SetControlPoint(uint32 row, uint32 col, const TQ3RationalPoint4D& pt)
{
	TQ3Status status = Q3NURBPatch_SetControlPoint(mObject, row, col, &pt);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DNurbPatch::GetUKnot
//
//---------------------------------------------------------------
float X3DNurbPatch::GetUKnot(uint32 index) const
{
	float value;
	
	TQ3Status status = Q3NURBPatch_GetUKnot(mObject, index, &value);
	ThrowIf3DError(status);
	
	return value;
}

			
//---------------------------------------------------------------
//
// X3DNurbPatch::SetUKnot
//
//---------------------------------------------------------------
void X3DNurbPatch::SetUKnot(uint32 index, float value)
{
	TQ3Status status = Q3NURBPatch_SetUKnot(mObject, index, value);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DNurbPatch::GetVKnot
//
//---------------------------------------------------------------
float X3DNurbPatch::GetVKnot(uint32 index) const
{
	float value;
	
	TQ3Status status = Q3NURBPatch_GetVKnot(mObject, index, &value);
	ThrowIf3DError(status);
	
	return value;
}

			
//---------------------------------------------------------------
//
// X3DNurbPatch::SetVKnot
//
//---------------------------------------------------------------
void X3DNurbPatch::SetVKnot(uint32 index, float value)
{
	TQ3Status status = Q3NURBPatch_SetVKnot(mObject, index, value);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DNurbPatch::GetData
//
//---------------------------------------------------------------
TQ3NURBPatchData X3DNurbPatch::GetData() const
{
	TQ3NURBPatchData data;
	
	TQ3Status status = Q3NURBPatch_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DNurbPatch::SetData
//
//---------------------------------------------------------------
void X3DNurbPatch::SetData(const TQ3NURBPatchData& data)
{
	TQ3Status status = Q3NURBPatch_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DNurbPatch::Submit									[static]
//
//---------------------------------------------------------------
void X3DNurbPatch::Submit(const TQ3NURBPatchData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3NURBPatch_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DNurbPatch::IsCompatible 							[static]
//	
//---------------------------------------------------------------
bool X3DNurbPatch::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypeNURBPatch);
	
	return compatible;
}


}	// namespace Whisper

