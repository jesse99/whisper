/*
 *  File:       X3DPt.cpp
 *  Summary:	Encapsulates a 3D point geometry.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DPt.cpp,v $
 *		Revision 1.4  2001/04/21 03:37:25  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:31:04  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:19:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DPt.h>

#include <X3DColors.h>
#include <X3DUtils.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetPointData
//
//---------------------------------------------------------------
static TQ3PointData GetPointData(const X3DPoint& pt, TQ3AttributeSet attributes)
{	
	TQ3PointData data;
	
	data.point             = pt;
	data.pointAttributeSet = attributes;
		
	return data;
}


//---------------------------------------------------------------
//
// CreatePoint (X3DPoint, X3DColorRGB)
//
//---------------------------------------------------------------
static TQ3GeometryObject CreatePoint(const X3DPoint& pt, const X3DColorRGB& color)
{
	TQ3AttributeSet attributes = Q3AttributeSet_New();
	ThrowIfNil(attributes);
		
	TQ3Status status = Q3AttributeSet_Add(attributes, kQ3AttributeTypeDiffuseColor, &color);
	ThrowIf3DError(status);
		
	TQ3PointData data = GetPointData(pt, attributes);
	
	TQ3GeometryObject object = Q3Point_New(&data);
	
	status = Q3Object_Dispose(attributes);
	ASSERT(status == kQ3Success);

	return object;
}


//---------------------------------------------------------------
//
// CreatePoint (X3DPoint, TQ3AttributeSet)
//
//---------------------------------------------------------------
static TQ3GeometryObject CreatePoint(const X3DPoint& pt, TQ3AttributeSet attributes)
{
	TQ3PointData data = GetPointData(pt, attributes);
	
	TQ3GeometryObject object = Q3Point_New(&data);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DPt
// ===================================================================================

//---------------------------------------------------------------
//
// X3DPt::~X3DPt
//
//---------------------------------------------------------------
X3DPt::~X3DPt()
{
}


//---------------------------------------------------------------
//
// X3DPt::X3DPt (X3DPoint, X3DColorRGB)
//
//---------------------------------------------------------------
X3DPt::X3DPt(const X3DPoint& pt, const X3DColorRGB& color) : X3DGeometry(CreatePoint(pt, color))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPt::X3DPt (X3DPoint, TQ3AttributeSet)
//
//---------------------------------------------------------------
X3DPt::X3DPt(const X3DPoint& pt, TQ3AttributeSet attributes) : X3DGeometry(CreatePoint(pt, attributes))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPt::X3DPt (TQ3PointData)
//
//---------------------------------------------------------------
X3DPt::X3DPt(const TQ3PointData& data) : X3DGeometry(Q3Point_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPt::X3DPt (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DPt::X3DPt(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypePoint);
}


//---------------------------------------------------------------
//
// X3DPt::Clone
//
//---------------------------------------------------------------
X3DPt* X3DPt::Clone() const
{
	X3DPt* object = new X3DPt(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DPt::GetPosition
//
//---------------------------------------------------------------
X3DPoint X3DPt::GetPosition() const
{
	X3DPoint pt;
	
	TQ3Status status = Q3Point_GetPosition(mObject, pt);
	ThrowIf3DError(status);
	
	return pt;
}

		
//---------------------------------------------------------------
//
// X3DPt::SetPosition
//
//---------------------------------------------------------------
void X3DPt::SetPosition(const X3DPoint& pt)
{
	TQ3Status status = Q3Point_SetPosition(mObject, pt);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPt::GetData
//
//---------------------------------------------------------------
TQ3PointData X3DPt::GetData() const
{
	TQ3PointData data;
	
	TQ3Status status = Q3Point_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DPt::SetData
//
//---------------------------------------------------------------
void X3DPt::SetData(const TQ3PointData& data)
{
	TQ3Status status = Q3Point_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPt::Submit (X3DPoint, TQ3ViewObject, TQ3AttributeSet)	[static]
//
//---------------------------------------------------------------
void X3DPt::Submit(const X3DPoint& pt, TQ3ViewObject view, const X3DColorRGB& color)
{
	PRECONDITION(view != nil);
	
	TQ3AttributeSet attributes = Q3AttributeSet_New();
	ThrowIfNil(attributes);
		
	TQ3Status status = Q3AttributeSet_Add(attributes, kQ3AttributeTypeDiffuseColor, &color);
	ThrowIf3DError(status);
		
	TQ3PointData data = Whisper::GetPointData(pt, attributes);

	status = Q3Point_Submit(&data, view);

	VERIFY(Q3Object_Dispose(attributes) == kQ3Success);

	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPt::Submit (X3DPoint, TQ3ViewObject, X3DColorRGB)	[static]
//
//---------------------------------------------------------------
void X3DPt::Submit(const X3DPoint& pt, TQ3ViewObject view, TQ3AttributeSet attributes)
{
	PRECONDITION(view != nil);
	
	TQ3PointData data = Whisper::GetPointData(pt, attributes);

	TQ3Status status = Q3Point_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPt::Submit (TQ3PointData, TQ3ViewObject)			[static]
//
//---------------------------------------------------------------
void X3DPt::Submit(const TQ3PointData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Point_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPt::IsCompatible 									[static]
//	
//---------------------------------------------------------------
bool X3DPt::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypePoint);
	
	return compatible;
}


}	// namespace Whisper

