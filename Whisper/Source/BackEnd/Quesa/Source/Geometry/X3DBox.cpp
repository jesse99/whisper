/*
 *  File:       X3DBox.cpp
 *  Summary:	A generalized 3D box.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DBox.cpp,v $
 *		Revision 1.4  2001/04/21 03:35:09  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:27:35  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:13:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DBox.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetBoxData
//
//---------------------------------------------------------------
static TQ3BoxData GetBoxData(const X3DRect& box)
{	
	TQ3BoxData data;

	data.origin           = box.origin;
	data.orientation      = X3DVector(box.size.x, 0.0f, 0.0f);
	data.majorAxis        = X3DVector(0.0f, box.size.y, 0.0f);
	data.minorAxis        = X3DVector(0.0f, 0.0f, box.size.z);
	data.faceAttributeSet = nil;
	data.boxAttributeSet  = nil;
		
	return data;
}


//---------------------------------------------------------------
//
// CreateBox (X3DRect)
//
//---------------------------------------------------------------
static TQ3GeometryObject CreateBox(const X3DRect& box)
{
	TQ3BoxData data = GetBoxData(box);
	
	TQ3GeometryObject object = Q3Box_New(&data);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DBox
// ===================================================================================

//---------------------------------------------------------------
//
// X3DBox::~X3DBox
//
//---------------------------------------------------------------
X3DBox::~X3DBox()
{
}


//---------------------------------------------------------------
//
// X3DBox::X3DBox (X3DRect)
//
//---------------------------------------------------------------
X3DBox::X3DBox(const X3DRect& box) : X3DGeometry(CreateBox(box))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DBox::X3DBox (TQ3BoxData)
//
//---------------------------------------------------------------
X3DBox::X3DBox(const TQ3BoxData& data) : X3DGeometry(Q3Box_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DBox::X3DBox (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DBox::X3DBox(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypeBox);
}


//---------------------------------------------------------------
//
// X3DBox::Clone
//
//---------------------------------------------------------------
X3DBox* X3DBox::Clone() const
{
	X3DBox* object = new X3DBox(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DBox::GetOrigin
//
//---------------------------------------------------------------
X3DPoint X3DBox::GetOrigin() const
{
	X3DPoint origin;
	
	TQ3Status status = Q3Box_GetOrigin(mObject, origin);
	ThrowIf3DError(status);
	
	return origin;
}


//---------------------------------------------------------------
//
// X3DBox::SetOrigin
//
//---------------------------------------------------------------
void X3DBox::SetOrigin(const X3DPoint& origin)
{
	TQ3Status status = Q3Box_SetOrigin(mObject, origin);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DBox::GetOrientation
//
//---------------------------------------------------------------
X3DVector X3DBox::GetOrientation() const
{
	X3DVector orientation;
	
	TQ3Status status = Q3Box_GetOrientation(mObject, orientation);
	ThrowIf3DError(status);
	
	return orientation;
}


//---------------------------------------------------------------
//
// X3DBox::SetOrientation
//
//---------------------------------------------------------------
void X3DBox::SetOrientation(const X3DVector& orientation)
{
	TQ3Status status = Q3Box_SetOrientation(mObject, orientation);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DBox::GetMajorAxis
//
//---------------------------------------------------------------
X3DVector X3DBox::GetMajorAxis() const
{
	X3DVector axis;
	
	TQ3Status status = Q3Box_GetMajorAxis(mObject, axis);
	ThrowIf3DError(status);
	
	return axis;
}


//---------------------------------------------------------------
//
// X3DBox::SetMajorAxis
//
//---------------------------------------------------------------
void X3DBox::SetMajorAxis(const X3DVector& axis)
{
	TQ3Status status = Q3Box_SetMajorAxis(mObject, axis);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DBox::GetMinorAxis
//
//---------------------------------------------------------------
X3DVector X3DBox::GetMinorAxis() const
{
	X3DVector axis;
	
	TQ3Status status = Q3Box_GetMinorAxis(mObject, axis);
	ThrowIf3DError(status);
	
	return axis;
}


//---------------------------------------------------------------
//
// X3DBox::SetMinorAxis
//
//---------------------------------------------------------------
void X3DBox::SetMinorAxis(const X3DVector& axis)
{
	TQ3Status status = Q3Box_SetMinorAxis(mObject, axis);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DBox::GetFaceAttributeSet
//
//---------------------------------------------------------------
TQ3AttributeSet X3DBox::GetFaceAttributeSet(uint32 faceIndex) const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3Box_GetFaceAttributeSet(mObject, faceIndex, &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}


//---------------------------------------------------------------
//
// X3DBox::SetFaceAttributeSet
//
//---------------------------------------------------------------
void X3DBox::SetFaceAttributeSet(uint32 faceIndex, TQ3AttributeSet attributes)
{
	TQ3Status status = Q3Box_SetFaceAttributeSet(mObject, faceIndex, attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DBox::GetData
//
//---------------------------------------------------------------
TQ3BoxData X3DBox::GetData() const
{
	TQ3BoxData data;
	
	TQ3Status status = Q3Box_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DBox::SetData
//
//---------------------------------------------------------------
void X3DBox::SetData(const TQ3BoxData& data)
{
	TQ3Status status = Q3Box_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DBox::Submit (X3DRect, TQ3ViewObject)				[static]
//
//---------------------------------------------------------------
void X3DBox::Submit(const X3DRect& box, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3BoxData data = Whisper::GetBoxData(box);

	TQ3Status status = Q3Box_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DBox::Submit (TQ3BoxData, TQ3ViewObject)			[static]
//
//---------------------------------------------------------------
void X3DBox::Submit(const TQ3BoxData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Box_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DBox::IsCompatible 								[static]
//
//---------------------------------------------------------------
bool X3DBox::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypeBox);
	
	return compatible;
}


}	// namespace Whisper
