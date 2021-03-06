/*
 *  File:       X3DTriGrid.cpp
 *  Summary:	Efficient mesh with a fixed topology.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first): 
 *
 *		$Log: X3DTriGrid.cpp,v $
 *		Revision 1.4  2001/04/21 03:38:45  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2000/12/10 04:08:49  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:23:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DTriGrid.h>

#include <X3DUtils.h>
#include <XExceptions.h>
#include <XNumbers.h>

namespace Whisper {


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateTriGrid (XSize)
//
//---------------------------------------------------------------
static TQ3GeometryObject CreateTriGrid(const XSize& size)
{
	PRECONDITION(size.width >= 2);		// 1.5d22 asserts if size < 2
	PRECONDITION(size.height >= 2);
	
	TQ3TriGridData data;
	
	TQ3Vertex3D* vertices = new TQ3Vertex3D[size.width*size.height];
	for (int v = 0; v < size.height; v++) {
		for (int h = 0; h < size.width; h++) {
			vertices[h + v*size.width].point        = kZero3DPt;
			vertices[h + v*size.width].attributeSet = nil;
		}
	}

	data.numRows             = numeric_cast<uint32>(size.height);
	data.numColumns          = numeric_cast<uint32>(size.width);
	data.vertices            = vertices;			// can't pass nil
	data.facetAttributeSet   = nil;
	data.triGridAttributeSet = nil;	
	
	TQ3GeometryObject object = Q3TriGrid_New(&data);
	
	delete [] vertices;
	
	ThrowIfNil(object);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateTriGrid (uint32, uint32, const TQ3Vertex3D*, const TQ3AttributeSet*, TQ3AttributeSet)
//
//---------------------------------------------------------------
static TQ3GeometryObject CreateTriGrid(uint32 numCols, uint32 numRows, const TQ3Vertex3D* vertices, const TQ3AttributeSet* facetAttributes, TQ3AttributeSet gridAttributes)
{
	TQ3TriGridData data;
	data.numRows             = numRows;
	data.numColumns          = numCols;
	data.vertices            = const_cast<TQ3Vertex3D*>(vertices);
	data.facetAttributeSet   = const_cast<TQ3AttributeSet*>(facetAttributes);
	data.triGridAttributeSet = gridAttributes;	
	
	TQ3GeometryObject object = Q3TriGrid_New(&data);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DTriGrid
// ===================================================================================

//---------------------------------------------------------------
//
// X3DTriGrid::~X3DTriGrid
//
//---------------------------------------------------------------
X3DTriGrid::~X3DTriGrid()
{
}


//---------------------------------------------------------------
//
// X3DTriGrid::X3DTriGrid (XSize)
//
//---------------------------------------------------------------
X3DTriGrid::X3DTriGrid(const XSize& size) : X3DGeometry(CreateTriGrid(size))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriGrid::X3DTriGrid (TQ3TriGridData)
//
//---------------------------------------------------------------
X3DTriGrid::X3DTriGrid(const TQ3TriGridData& data) : X3DGeometry(Q3TriGrid_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriGrid::X3DTriGrid (uint32, uint32, const TQ3Vertex3D*, const TQ3AttributeSet*, TQ3AttributeSet)
//
//---------------------------------------------------------------
X3DTriGrid::X3DTriGrid(uint32 numCols, uint32 numRows, const TQ3Vertex3D* vertices, const TQ3AttributeSet* facetAttributes, TQ3AttributeSet gridAttributes) : X3DGeometry(CreateTriGrid(numCols, numRows, vertices, facetAttributes, gridAttributes))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriGrid::X3DTriGrid (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DTriGrid::X3DTriGrid(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypeTriGrid);
}


//---------------------------------------------------------------
//
// X3DTriGrid::Clone
//
//---------------------------------------------------------------
X3DTriGrid* X3DTriGrid::Clone() const
{
	X3DTriGrid* object = new X3DTriGrid(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DTriGrid::GetVertexPosition
//
//---------------------------------------------------------------
X3DPoint X3DTriGrid::GetVertexPosition(const XPoint& vertex) const
{
	X3DPoint position;
	
	TQ3Status status = Q3TriGrid_GetVertexPosition(mObject, numeric_cast<uint32>(vertex.y), numeric_cast<uint32>(vertex.x), position);
	ThrowIf3DError(status);
	
	return position;
}

		
//---------------------------------------------------------------
//
// X3DTriGrid::SetVertexPosition
//
//---------------------------------------------------------------
void X3DTriGrid::SetVertexPosition(const XPoint& vertex, const X3DPoint& position)
{
	TQ3Status status = Q3TriGrid_SetVertexPosition(mObject, numeric_cast<uint32>(vertex.y), numeric_cast<uint32>(vertex.x), position);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriGrid::GetVertexAttributeSet
//
//---------------------------------------------------------------
TQ3AttributeSet X3DTriGrid::GetVertexAttributeSet(const XPoint& vertex) const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3TriGrid_GetVertexAttributeSet(mObject, numeric_cast<uint32>(vertex.y), numeric_cast<uint32>(vertex.x), &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}

		
//---------------------------------------------------------------
//
// X3DTriGrid::SetVertexAttributeSet
//
//---------------------------------------------------------------
void X3DTriGrid::SetVertexAttributeSet(const XPoint& vertex, TQ3AttributeSet attributes)
{
	TQ3Status status = Q3TriGrid_SetVertexAttributeSet(mObject, numeric_cast<uint32>(vertex.y), numeric_cast<uint32>(vertex.x), attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriGrid::GetFaceAttributeSet
//
//---------------------------------------------------------------
TQ3AttributeSet X3DTriGrid::GetFaceAttributeSet(uint32 index) const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3TriGrid_GetFacetAttributeSet(mObject, index, &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}

		
//---------------------------------------------------------------
//
// X3DTriGrid::SetFaceAttributeSet
//
//---------------------------------------------------------------
void X3DTriGrid::SetFaceAttributeSet(uint32 index, TQ3AttributeSet attributes)
{
	TQ3Status status = Q3TriGrid_SetFacetAttributeSet(mObject, index, attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriGrid::GetSize
//
//---------------------------------------------------------------
XSize X3DTriGrid::GetSize() const
{
	TQ3TriGridData data;
	
	TQ3Status status = Q3TriGrid_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	XSize size(numeric_cast<int16>(data.numColumns), numeric_cast<int16>(data.numRows));
	
	status = Q3TriGrid_EmptyData(&data);
	ASSERT(status == kQ3Success);

	return size;
}

			
//---------------------------------------------------------------
//
// X3DTriGrid::SetSize
//
//---------------------------------------------------------------
void X3DTriGrid::SetSize(const XSize& size)
{
	PRECONDITION(size.width >= 2);		// 1.5d22 asserts if size < 2
	PRECONDITION(size.height >= 2);
		
	TQ3TriGridData data;
	TQ3Status status = Q3TriGrid_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	TQ3Vertex3D* vertices = new TQ3Vertex3D[size.width*size.height];
	for (int h = 0; h < size.width; h++) {
		for (int v = 0; v < size.height; v++) {
			vertices[h + v*size.width].point        = kZero3DPt;
			vertices[h + v*size.width].attributeSet = nil;
		}
	}
	
	if (data.facetAttributeSet != nil && *data.facetAttributeSet != nil) {
		status = Q3Object_Dispose(*data.facetAttributeSet);
		ASSERT(status == kQ3Success);
	}
	
	data.numRows            = numeric_cast<uint32>(size.height);
	data.numColumns         = numeric_cast<uint32>(size.width);
	data.vertices           = vertices;
	data.facetAttributeSet  = nil;
	
	status = Q3TriGrid_SetData(mObject, &data);
	
	delete [] vertices;

	data.vertices = nil;
	VERIFY(Q3TriGrid_EmptyData(&data) == kQ3Success);
	
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriGrid::GetData
//
//---------------------------------------------------------------
TQ3TriGridData X3DTriGrid::GetData() const
{
	TQ3TriGridData data;
	
	TQ3Status status = Q3TriGrid_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DTriGrid::SetData
//
//---------------------------------------------------------------
void X3DTriGrid::SetData(const TQ3TriGridData& data)
{
	TQ3Status status = Q3TriGrid_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriGrid::Submit (TQ3TriGridData, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DTriGrid::Submit(const TQ3TriGridData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3TriGrid_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTriGrid::IsCompatible								[static]
//	
//---------------------------------------------------------------
bool X3DTriGrid::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypeTriGrid);
	
	return compatible;
}


}	// namespace Whisper

