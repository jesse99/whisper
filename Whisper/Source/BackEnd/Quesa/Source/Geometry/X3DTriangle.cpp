/*
 *  File:       X3DTriangle.cpp
 *  Summary:	Encapsulates a 3D filled triangle.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DTriangle.cpp,v $
 *		Revision 1.4  2001/04/21 03:38:40  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:33:08  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:22:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DTriangle.h>

#include <X3DAttributeSet.h>
#include <X3DUtils.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetLineData
//
//---------------------------------------------------------------
static TQ3TriangleData GetLineData(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3, TQ3AttributeSet attributes)
{	
	TQ3TriangleData data;
	
	data.vertices[0].point        = pt1;
	data.vertices[0].attributeSet = nil;

	data.vertices[1].point        = pt2;
	data.vertices[1].attributeSet = nil;

	data.vertices[2].point        = pt3;
	data.vertices[2].attributeSet = nil;

	data.triangleAttributeSet     = attributes;
		
	return data;
}


//---------------------------------------------------------------
//
// CreateTriangle (X3DPoint, X3DPoint, X3DPoint, X3DColorRGB)
//
//---------------------------------------------------------------
static TQ3GeometryObject CreateTriangle(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3, const X3DColorRGB& color)
{
	X3DAttributeSet attributes;
	attributes.AddDiffuseColor(color);
	
	TQ3TriangleData data = GetLineData(pt1, pt2, pt3, attributes);
	
	TQ3GeometryObject object = Q3Triangle_New(&data);

	return object;
}


//---------------------------------------------------------------
//
// CreateTriangle (X3DPoint, X3DPoint, X3DPoint, X3DColorRGB, X3DColorRGB, X3DColorRGB)
//
//---------------------------------------------------------------
static TQ3GeometryObject CreateTriangle(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3, const X3DColorRGB& color1, const X3DColorRGB& color2, const X3DColorRGB& color3)
{
	X3DAttributeSet attributes0;
	attributes0.AddDiffuseColor(color1);
	
	X3DAttributeSet attributes1;
	attributes1.AddDiffuseColor(color2);
	
	X3DAttributeSet attributes2;
	attributes2.AddDiffuseColor(color3);
	
	TQ3TriangleData data = GetLineData(pt1, pt2, pt3, nil);
	
	data.vertices[0].attributeSet = attributes0;
	data.vertices[1].attributeSet = attributes1;
	data.vertices[2].attributeSet = attributes2;

	TQ3GeometryObject object = Q3Triangle_New(&data);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateTriangle (X3DPoint, X3DPoint, X3DPoint, TQ3AttributeSet)
//
//---------------------------------------------------------------
static TQ3GeometryObject CreateTriangle(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3, TQ3AttributeSet attributes)
{
	TQ3TriangleData data = GetLineData(pt1, pt2, pt3, attributes);
	
	TQ3GeometryObject object = Q3Triangle_New(&data);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DTriangle
// ===================================================================================

//---------------------------------------------------------------
//
// X3DTriangle::~X3DTriangle
//
//---------------------------------------------------------------
X3DTriangle::~X3DTriangle()
{
}


//---------------------------------------------------------------
//
// X3DTriangle::X3DTriangle (X3DPoint, X3DPoint, X3DPoint, X3DColorRGB)
//
//---------------------------------------------------------------
X3DTriangle::X3DTriangle(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3, const X3DColorRGB& color) : X3DGeometry(CreateTriangle(pt1, pt2, pt3, color))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriangle::X3DTriangle (X3DPoint, X3DPoint, X3DPoint, X3DColorRGB, X3DColorRGB, X3DColorRGB)
//
//---------------------------------------------------------------
X3DTriangle::X3DTriangle(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3, const X3DColorRGB& color1, const X3DColorRGB& color2, const X3DColorRGB& color3) : X3DGeometry(CreateTriangle(pt1, pt2, pt3, color1, color2, color3))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriangle::X3DTriangle (X3DPoint, X3DPoint, X3DPoint, TQ3AttributeSet)
//
//---------------------------------------------------------------
X3DTriangle::X3DTriangle(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3, TQ3AttributeSet attributes) : X3DGeometry(CreateTriangle(pt1, pt2, pt3, attributes))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriangle::X3DTriangle (TQ3TriangleData)
//
//---------------------------------------------------------------
X3DTriangle::X3DTriangle(const TQ3TriangleData& data) : X3DGeometry(Q3Triangle_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriangle::X3DTriangle (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DTriangle::X3DTriangle(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypeTriangle);
}


//---------------------------------------------------------------
//
// X3DTriangle::Clone
//
//---------------------------------------------------------------
X3DTriangle* X3DTriangle::Clone() const
{
	X3DTriangle* object = new X3DTriangle(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DTriangle::GetVertexPosition
//
//---------------------------------------------------------------
X3DPoint X3DTriangle::GetVertexPosition(uint32 index) const
{
	X3DPoint vertex;
	
	TQ3Status status = Q3Triangle_GetVertexPosition(mObject, index, vertex);
	ThrowIf3DError(status);
	
	return vertex;
}

		
//---------------------------------------------------------------
//
// X3DTriangle::SetVertexPosition
//
//---------------------------------------------------------------
void X3DTriangle::SetVertexPosition(uint32 index, const X3DPoint& vertex)
{
	TQ3Status status = Q3Triangle_SetVertexPosition(mObject, index, vertex);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriangle::GetVertexAttributeSet
//
//---------------------------------------------------------------
TQ3AttributeSet X3DTriangle::GetVertexAttributeSet(uint32 index) const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3Triangle_GetVertexAttributeSet(mObject, index, &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}

		
//---------------------------------------------------------------
//
// X3DTriangle::SetVertexAttributeSet
//
//---------------------------------------------------------------
void X3DTriangle::SetVertexAttributeSet(uint32 index, TQ3AttributeSet attributes)
{
	TQ3Status status = Q3Triangle_SetVertexAttributeSet(mObject, index, attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriangle::GetData
//
//---------------------------------------------------------------
TQ3TriangleData X3DTriangle::GetData() const
{
	TQ3TriangleData data;
	
	TQ3Status status = Q3Triangle_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DTriangle::SetData
//
//---------------------------------------------------------------
void X3DTriangle::SetData(const TQ3TriangleData& data)
{
	TQ3Status status = Q3Triangle_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriangle::Submit (X3DPoint, X3DPoint, X3DPoint, TQ3ViewObject, X3DColorRGB)	[static]
//
//---------------------------------------------------------------
void X3DTriangle::Submit(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3, TQ3ViewObject view, const X3DColorRGB& color)
{
	PRECONDITION(view != nil);
	
	X3DAttributeSet attributes;
	attributes.AddDiffuseColor(color);

	TQ3TriangleData data = GetLineData(pt1, pt2, pt3, attributes);
	
	TQ3Status status = Q3Triangle_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriangle::Submit (X3DPoint, X3DPoint, X3DPoint, TQ3ViewObject, X3DColorRGB, X3DColorRGB, X3DColorRGB)	[static]
//
//---------------------------------------------------------------
void X3DTriangle::Submit(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3, TQ3ViewObject view, const X3DColorRGB& color1, const X3DColorRGB& color2, const X3DColorRGB& color3)
{
	PRECONDITION(view != nil);
	
	X3DAttributeSet attributes0;
	attributes0.AddDiffuseColor(color1);
	
	X3DAttributeSet attributes1;
	attributes1.AddDiffuseColor(color2);
	
	X3DAttributeSet attributes2;
	attributes2.AddDiffuseColor(color3);
	
	TQ3TriangleData data = GetLineData(pt1, pt2, pt3, nil);
	
	data.vertices[0].attributeSet = attributes0;
	data.vertices[1].attributeSet = attributes1;
	data.vertices[2].attributeSet = attributes2;
	
	TQ3Status status = Q3Triangle_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriangle::Submit (X3DPoint, X3DPoint, X3DPoint, TQ3ViewObject, TQ3AttributeSet)	[static]
//
//---------------------------------------------------------------
void X3DTriangle::Submit(const X3DPoint& pt1, const X3DPoint& pt2, const X3DPoint& pt3, TQ3ViewObject view, TQ3AttributeSet attributes)
{
	PRECONDITION(view != nil);
	
	TQ3TriangleData data = Whisper::GetLineData(pt1, pt2, pt3, attributes);

	TQ3Status status = Q3Triangle_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriangle::Submit (TQ3TriangleData, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DTriangle::Submit(const TQ3TriangleData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Triangle_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriangle::IsCompatible							[static]
//	
//---------------------------------------------------------------
bool X3DTriangle::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypeTriangle);
	
	return compatible;
}


}	// namespace Whisper

