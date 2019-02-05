/*
 *  File:       X3DPolyhedron.cpp
 *  Summary:	A fast mesh geometry that can be edited.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DPolyhedron.cpp,v $
 *		Revision 1.4  2001/04/21 03:37:12  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:30:44  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:18:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DPolyhedron.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	class X3DPolyhedron
// ===================================================================================

//---------------------------------------------------------------
//
// X3DPolyhedron::~X3DPolyhedron
//
//---------------------------------------------------------------
X3DPolyhedron::~X3DPolyhedron()
{
}


//---------------------------------------------------------------
//
// X3DPolyhedron::X3DPolyhedron (TQ3PolyhedronData)
//
//---------------------------------------------------------------
X3DPolyhedron::X3DPolyhedron(const TQ3PolyhedronData& data) : X3DGeometry(Q3Polyhedron_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolyhedron::X3DPolyhedron (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DPolyhedron::X3DPolyhedron(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypePolyhedron);
}


//---------------------------------------------------------------
//
// X3DPolyhedron::Clone
//
//---------------------------------------------------------------
X3DPolyhedron* X3DPolyhedron::Clone() const
{
	X3DPolyhedron* object = new X3DPolyhedron(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DPolyhedron::GetVertexPosition
//
//---------------------------------------------------------------
X3DPoint X3DPolyhedron::GetVertexPosition(uint32 index) const
{
	X3DPoint vertex;
	
	TQ3Status status = Q3Polyhedron_GetVertexPosition(mObject, index, vertex);
	ThrowIf3DError(status);
	
	return vertex;
}

		
//---------------------------------------------------------------
//
// X3DPolyhedron::SetVertexPosition
//
//---------------------------------------------------------------
void X3DPolyhedron::SetVertexPosition(uint32 index, const X3DPoint& vertex)
{
	TQ3Status status = Q3Polyhedron_SetVertexPosition(mObject, index, vertex);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolyhedron::GetVertexAttributeSet
//
//---------------------------------------------------------------
TQ3AttributeSet X3DPolyhedron::GetVertexAttributeSet(uint32 index) const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3Polyhedron_GetVertexAttributeSet(mObject, index, &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}

		
//---------------------------------------------------------------
//
// X3DPolyhedron::SetVertexAttributeSet
//
//---------------------------------------------------------------
void X3DPolyhedron::SetVertexAttributeSet(uint32 index, TQ3AttributeSet attributes)
{
	TQ3Status status = Q3Polyhedron_SetVertexAttributeSet(mObject, index, attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolyhedron::GetTriangleData
//
//---------------------------------------------------------------
TQ3PolyhedronTriangleData X3DPolyhedron::GetTriangleData(uint32 index) const
{
	TQ3PolyhedronTriangleData data;
	
	TQ3Status status = Q3Polyhedron_GetTriangleData(mObject, index, &data);
	ThrowIf3DError(status);
	
	return data;
}

			
//---------------------------------------------------------------
//
// X3DPolyhedron::SetTriangleData
//
//---------------------------------------------------------------
void  X3DPolyhedron::SetTriangleData(uint32 index, const TQ3PolyhedronTriangleData& data)
{
	TQ3Status status = Q3Polyhedron_SetTriangleData(mObject, index, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolyhedron::GetEdgeData
//
//---------------------------------------------------------------
TQ3PolyhedronEdgeData X3DPolyhedron::GetEdgeData(uint32 index) const
{
	TQ3PolyhedronEdgeData data;
	
	TQ3Status status = Q3Polyhedron_GetEdgeData(mObject, index, &data);
	ThrowIf3DError(status);
	
	return data;
}

			
//---------------------------------------------------------------
//
// X3DPolyhedron::SetEdgeData
//
//---------------------------------------------------------------
void  X3DPolyhedron::SetEdgeData(uint32 index, const TQ3PolyhedronEdgeData& data)
{
	TQ3Status status = Q3Polyhedron_SetEdgeData(mObject, index, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolyhedron::GetData
//
//---------------------------------------------------------------
TQ3PolyhedronData X3DPolyhedron::GetData() const
{
	TQ3PolyhedronData data;
	
	TQ3Status status = Q3Polyhedron_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DPolyhedron::SetData
//
//---------------------------------------------------------------
void X3DPolyhedron::SetData(const TQ3PolyhedronData& data)
{
	TQ3Status status = Q3Polyhedron_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolyhedron::Submit (TQ3PolyhedronData, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DPolyhedron::Submit(const TQ3PolyhedronData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Polyhedron_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolyhedron::IsCompatible 							[static]
//	
//---------------------------------------------------------------
bool X3DPolyhedron::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypePolyhedron);
	
	return compatible;
}


}	// namespace Whisper

