/*
 *  File:       X3DPolyLine.cpp
 *  Summary:	Encapsulates a 3D polygonal line.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DPolyLine.cpp,v $
 *		Revision 1.4  2001/04/21 03:37:15  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:30:48  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:19:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DPolyLine.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreatePolyLine
//
//---------------------------------------------------------------
static TQ3GeometryObject CreatePolyLine(uint32 count, const TQ3Vertex3D* vertices, const TQ3AttributeSet* segmentAttributes, TQ3AttributeSet polyLineAttributes)
{
	TQ3PolyLineData data;
	data.numVertices          = count;
	data.vertices             = const_cast<TQ3Vertex3D*>(vertices);
	data.segmentAttributeSet  = const_cast<TQ3AttributeSet*>(segmentAttributes);
	data.polyLineAttributeSet = polyLineAttributes;
	
	TQ3GeometryObject object = Q3PolyLine_New(&data);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DPolyLine
// ===================================================================================

//---------------------------------------------------------------
//
// X3DPolyLine::~X3DPolyLine
//
//---------------------------------------------------------------
X3DPolyLine::~X3DPolyLine()
{
}


//---------------------------------------------------------------
//
// X3DPolyLine::X3DPolyLine (TQ3PolyLineData)
//
//---------------------------------------------------------------
X3DPolyLine::X3DPolyLine(const TQ3PolyLineData& data) : X3DGeometry(Q3PolyLine_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolyLine::X3DPolyLine (uint32, const TQ3Vertex3D*, const TQ3AttributeSet*, TQ3AttributeSet)
//
//---------------------------------------------------------------
X3DPolyLine::X3DPolyLine(uint32 count, const TQ3Vertex3D* vertices, const TQ3AttributeSet* segmentAttributes, TQ3AttributeSet polyLineAttributes) : X3DGeometry(CreatePolyLine(count, vertices, segmentAttributes, polyLineAttributes))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolyLine::X3DPolyLine (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DPolyLine::X3DPolyLine(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypePolyLine);
}


//---------------------------------------------------------------
//
// X3DPolyLine::Clone
//
//---------------------------------------------------------------
X3DPolyLine* X3DPolyLine::Clone() const
{
	X3DPolyLine* object = new X3DPolyLine(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DPolyLine::GetVertexPosition
//
//---------------------------------------------------------------
X3DPoint X3DPolyLine::GetVertexPosition(uint32 index) const
{
	X3DPoint vertex;
	
	TQ3Status status = Q3PolyLine_GetVertexPosition(mObject, index, vertex);
	ThrowIf3DError(status);
	
	return vertex;
}

		
//---------------------------------------------------------------
//
// X3DPolyLine::SetVertexPosition
//
//---------------------------------------------------------------
void X3DPolyLine::SetVertexPosition(uint32 index, const X3DPoint& vertex)
{
	TQ3Status status = Q3PolyLine_SetVertexPosition(mObject, index, vertex);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolyLine::GetVertexAttributeSet
//
//---------------------------------------------------------------
TQ3AttributeSet X3DPolyLine::GetVertexAttributeSet(uint32 index) const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3PolyLine_GetVertexAttributeSet(mObject, index, &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}

		
//---------------------------------------------------------------
//
// X3DPolyLine::SetVertexAttributeSet
//
//---------------------------------------------------------------
void X3DPolyLine::SetVertexAttributeSet(uint32 index, TQ3AttributeSet attributes)
{
	TQ3Status status = Q3PolyLine_SetVertexAttributeSet(mObject, index, attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolyLine::GetSegmentAttributeSet
//
//---------------------------------------------------------------
TQ3AttributeSet X3DPolyLine::GetSegmentAttributeSet(uint32 index) const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3PolyLine_GetSegmentAttributeSet(mObject, index, &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}

		
//---------------------------------------------------------------
//
// X3DPolyLine::SetSegmentAttributeSet
//
//---------------------------------------------------------------
void X3DPolyLine::SetSegmentAttributeSet(uint32 index, TQ3AttributeSet attributes)
{
	TQ3Status status = Q3PolyLine_SetSegmentAttributeSet(mObject, index, attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolyLine::GetData
//
//---------------------------------------------------------------
TQ3PolyLineData X3DPolyLine::GetData() const
{
	TQ3PolyLineData data;
	
	TQ3Status status = Q3PolyLine_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DPolyLine::SetData
//
//---------------------------------------------------------------
void X3DPolyLine::SetData(const TQ3PolyLineData& data)
{
	TQ3Status status = Q3PolyLine_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolyLine::Submit (TQ3PolyLineData, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DPolyLine::Submit(const TQ3PolyLineData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3PolyLine_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPolyLine::IsCompatible 							[static]
//	
//---------------------------------------------------------------
bool X3DPolyLine::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypePolyLine);
	
	return compatible;
}


}	// namespace Whisper
