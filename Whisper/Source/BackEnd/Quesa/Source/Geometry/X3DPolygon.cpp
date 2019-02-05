/*
 *  File:       X3DPolygon.cpp
 *  Summary:	Encapsulates a QuickDraw 3D polygon.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DPolygon.cpp,v $
 *		Revision 1.4  2001/04/21 03:37:08  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:30:39  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:18:45  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DPolygon.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreatePolygon
//
//---------------------------------------------------------------
static TQ3GeometryObject CreatePolygon(uint32 count, const TQ3Vertex3D* vertices, TQ3AttributeSet attr)
{
	TQ3PolygonData data;
	data.numVertices         = count;
	data.vertices            = const_cast<TQ3Vertex3D*>(vertices);
	data.polygonAttributeSet = attr;
	
	TQ3GeometryObject object = Q3Polygon_New(&data);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DPolygon
// ===================================================================================

//---------------------------------------------------------------
//
// X3DPolygon::~X3DPolygon
//
//---------------------------------------------------------------
X3DPolygon::~X3DPolygon()
{
}


//---------------------------------------------------------------
//
// X3DPolygon::X3DPolygon (TQ3PolygonData)
//
//---------------------------------------------------------------
X3DPolygon::X3DPolygon(const TQ3PolygonData& data) : X3DGeometry(Q3Polygon_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolygon::X3DPolygon (count, const TQ3Vertex3D*, TQ3AttributeSet)
//
//---------------------------------------------------------------
X3DPolygon::X3DPolygon(uint32 count, const TQ3Vertex3D* vertices, TQ3AttributeSet attr) : X3DGeometry(CreatePolygon(count, vertices, attr))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolygon::X3DPolygon (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DPolygon::X3DPolygon(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypePolygon);
}


//---------------------------------------------------------------
//
// X3DPolygon::Clone
//
//---------------------------------------------------------------
X3DPolygon* X3DPolygon::Clone() const
{
	X3DPolygon* object = new X3DPolygon(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DPolygon::GetVertexPosition
//
//---------------------------------------------------------------
X3DPoint X3DPolygon::GetVertexPosition(uint32 index) const
{
	X3DPoint vertex;
	
	TQ3Status status = Q3Polygon_GetVertexPosition(mObject, index, vertex);
	ThrowIf3DError(status);
	
	return vertex;
}

		
//---------------------------------------------------------------
//
// X3DPolygon::SetVertexPosition
//
//---------------------------------------------------------------
void X3DPolygon::SetVertexPosition(uint32 index, const X3DPoint& vertex)
{
	TQ3Status status = Q3Polygon_SetVertexPosition(mObject, index, vertex);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolygon::GetVertexAttributeSet
//
//---------------------------------------------------------------
TQ3AttributeSet X3DPolygon::GetVertexAttributeSet(uint32 index) const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3Polygon_GetVertexAttributeSet(mObject, index, &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}

		
//---------------------------------------------------------------
//
// X3DPolygon::SetVertexAttributeSet
//
//---------------------------------------------------------------
void X3DPolygon::SetVertexAttributeSet(uint32 index, TQ3AttributeSet attributes)
{
	TQ3Status status = Q3Polygon_SetVertexAttributeSet(mObject, index, attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolygon::GetData
//
//---------------------------------------------------------------
TQ3PolygonData X3DPolygon::GetData() const
{
	TQ3PolygonData data;
	
	TQ3Status status = Q3Polygon_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DPolygon::SetData
//
//---------------------------------------------------------------
void X3DPolygon::SetData(const TQ3PolygonData& data)
{
	TQ3Status status = Q3Polygon_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolygon::Submit (TQ3PolygonData, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DPolygon::Submit(const TQ3PolygonData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Polygon_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolygon::IsCompatible 							[static]
//	
//---------------------------------------------------------------
bool X3DPolygon::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypePolygon);
	
	return compatible;
}


}	// namespace Whisper
