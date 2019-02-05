/*
 *  File:       X3DLine.cpp
 *  Summary:	Encapsulates a 3D line.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DLine.cpp,v $
 *		Revision 1.4  2001/04/21 03:36:17  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:29:29  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:16:59  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DLine.h>

#include <X3DColors.h>
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
static TQ3LineData GetLineData(const X3DPoint& start, const X3DPoint& stop, TQ3AttributeSet attributes)
{	
	TQ3LineData data;
	
	data.vertices[0].point        = start;
	data.vertices[0].attributeSet = nil;

	data.vertices[1].point        = stop;
	data.vertices[1].attributeSet = nil;

	data.lineAttributeSet = attributes;
		
	return data;
}


//---------------------------------------------------------------
//
// CreateLine (X3DPoint, X3DPoint, TQ3AttributeSet)
//
//---------------------------------------------------------------
static TQ3GeometryObject CreateLine(const X3DPoint& start, const X3DPoint& stop, TQ3AttributeSet attributes)
{
	TQ3LineData data = GetLineData(start, stop, attributes);
	
	TQ3GeometryObject object = Q3Line_New(&data);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateLine (X3DPoint, X3DPoint, X3DColorRGB)
//
//---------------------------------------------------------------
static TQ3GeometryObject CreateLine(const X3DPoint& start, const X3DPoint& stop, const X3DColorRGB& color)
{
	TQ3AttributeSet attributes = Q3AttributeSet_New();
	ThrowIfNil(attributes);
		
	TQ3Status status = Q3AttributeSet_Add(attributes, kQ3AttributeTypeDiffuseColor, &color);
	ThrowIf3DError(status);
		
	TQ3LineData data = GetLineData(start, stop, attributes);
	
	TQ3GeometryObject object = Q3Line_New(&data);
	
	status = Q3Object_Dispose(attributes);
	ASSERT(status == kQ3Success);

	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DLine
// ===================================================================================

//---------------------------------------------------------------
//
// X3DLine::~X3DLine
//
//---------------------------------------------------------------
X3DLine::~X3DLine()
{
}


//---------------------------------------------------------------
//
// X3DLine::X3DLine (X3DPoint, X3DPoint, X3DColorRGB)
//
//---------------------------------------------------------------
X3DLine::X3DLine(const X3DPoint& start, const X3DPoint& stop, const X3DColorRGB& color) : X3DGeometry(CreateLine(start, stop, color))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DLine::X3DLine (X3DPoint, X3DPoint, TQ3AttributeSet)
//
//---------------------------------------------------------------
X3DLine::X3DLine(const X3DPoint& start, const X3DPoint& stop, TQ3AttributeSet attributes) : X3DGeometry(CreateLine(start, stop, attributes))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DLine::X3DLine (TQ3LineData)
//
//---------------------------------------------------------------
X3DLine::X3DLine(const TQ3LineData& data) : X3DGeometry(Q3Line_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DLine::X3DLine (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DLine::X3DLine(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypeLine);
}


//---------------------------------------------------------------
//
// X3DLine::Clone
//
//---------------------------------------------------------------
X3DLine* X3DLine::Clone() const
{
	X3DLine* object = new X3DLine(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DLine::GetVertexPosition
//
//---------------------------------------------------------------
X3DPoint X3DLine::GetVertexPosition(uint32 index) const
{
	X3DPoint vertex;
	
	TQ3Status status = Q3Line_GetVertexPosition(mObject, index, vertex);
	ThrowIf3DError(status);
	
	return vertex;
}

		
//---------------------------------------------------------------
//
// X3DLine::SetVertexPosition
//
//---------------------------------------------------------------
void X3DLine::SetVertexPosition(uint32 index, const X3DPoint& vertex)
{
	TQ3Status status = Q3Line_SetVertexPosition(mObject, index, vertex);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DLine::GetVertexAttributeSet
//
//---------------------------------------------------------------
TQ3AttributeSet X3DLine::GetVertexAttributeSet(uint32 index) const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3Line_GetVertexAttributeSet(mObject, index, &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}

		
//---------------------------------------------------------------
//
// X3DLine::SetVertexAttributeSet
//
//---------------------------------------------------------------
void X3DLine::SetVertexAttributeSet(uint32 index, TQ3AttributeSet attributes)
{
	TQ3Status status = Q3Line_SetVertexAttributeSet(mObject, index, attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DLine::GetData
//
//---------------------------------------------------------------
TQ3LineData X3DLine::GetData() const
{
	TQ3LineData data;
	
	TQ3Status status = Q3Line_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DLine::SetData
//
//---------------------------------------------------------------
void X3DLine::SetData(const TQ3LineData& data)
{
	TQ3Status status = Q3Line_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DLine::Submit (X3DPoint, X3DPoint, TQ3ViewObject, X3DColorRGB)	[static]
//
//---------------------------------------------------------------
void X3DLine::Submit(const X3DPoint& start, const X3DPoint& stop, TQ3ViewObject view, const X3DColorRGB& color)
{
	PRECONDITION(view != nil);
	
	TQ3AttributeSet attributes = Q3AttributeSet_New();
	ThrowIfNil(attributes);
		
	TQ3Status status = Q3AttributeSet_Add(attributes, kQ3AttributeTypeDiffuseColor, &color);
	ThrowIf3DError(status);
		
	TQ3LineData data = Whisper::GetLineData(start, stop, attributes);

	status = Q3Line_Submit(&data, view);

	VERIFY(Q3Object_Dispose(attributes) == kQ3Success);
	
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DLine::Submit (X3DPoint, X3DPoint, TQ3ViewObject, TQ3AttributeSet)	[static]
//
//---------------------------------------------------------------
void X3DLine::Submit(const X3DPoint& start, const X3DPoint& stop, TQ3ViewObject view, TQ3AttributeSet attributes)
{
	PRECONDITION(view != nil);
	
	TQ3LineData data = Whisper::GetLineData(start, stop, attributes);

	TQ3Status status = Q3Line_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DLine::Submit (TQ3LineData, TQ3ViewObject)			[static]
//
//---------------------------------------------------------------
void X3DLine::Submit(const TQ3LineData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Line_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DLine::IsCompatible 								[static]
//	
//---------------------------------------------------------------
bool X3DLine::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypeLine);
	
	return compatible;
}


}	// namespace Whisper
