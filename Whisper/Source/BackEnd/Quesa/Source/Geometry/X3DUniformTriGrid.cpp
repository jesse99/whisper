/*
 *  File:       X3DUniformTriGrid.cpp
 *  Summary:	X3DTriMesh that works like a trigrid.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DUniformTriGrid.cpp,v $
 *		Revision 1.4  2001/04/21 03:38:52  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2000/12/10 04:08:57  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:23:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DUniformTriGrid.h>

#include <X3DUtils.h>
#include <XExceptions.h>
#include <XNumbers.h>

#if DEBUG
#include <XIntConversions.h>
#include <XStringUtils.h>
#endif

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateTriMesh
//
//---------------------------------------------------------------
static TQ3GeometryObject CreateTriMesh()
{
	TQ3TriMeshData data;
	
	static TQ3Point3D vertices[4];
	static TQ3TriMeshTriangleData triangles[2] = {{0, 1, 2}, {2, 3, 1}};
	
	data.triMeshAttributeSet       = nil;
	data.numTriangles              = 2;				// QD 3D 1.5.3 won't create an empty mesh
	data.triangles				   = triangles;
	data.numTriangleAttributeTypes = 0;
	data.triangleAttributeTypes    = nil;
	data.numEdges                  = 0;
	data.edges                     = nil;
	data.numEdgeAttributeTypes     = 0;
	data.edgeAttributeTypes        = nil;
	data.numPoints                 = 4;
	data.points                    = vertices;
	data.numVertexAttributeTypes   = 0;
	data.vertexAttributeTypes      = nil;
	data.bBox.min                  = kZero3DPt;
	data.bBox.max                  = kZero3DPt;
	data.bBox.isEmpty              = kQ3True;
	
	TQ3GeometryObject object = Q3TriMesh_New(&data);	
	ThrowIfNil(object);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DUniformTriGrid
// ===================================================================================

//---------------------------------------------------------------
//
// X3DUniformTriGrid::~X3DUniformTriGrid
//
//---------------------------------------------------------------
X3DUniformTriGrid::~X3DUniformTriGrid()
{
}


//---------------------------------------------------------------
//
// X3DUniformTriGrid::X3DUniformTriGrid
//
//---------------------------------------------------------------
X3DUniformTriGrid::X3DUniformTriGrid(const XSize& size) : X3DTriMesh(CreateTriMesh())
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
		
	mSize  = kZeroSize;
	mDirty = false;
	
	mBoundingBox = X3DRect(kZeroRect.GetOSRect());
	
	this->SetSize(size);
}


//---------------------------------------------------------------
//
// X3DUniformTriGrid::Clone
//
//---------------------------------------------------------------
X3DUniformTriGrid* X3DUniformTriGrid::Clone() const
{	
	X3DUniformTriGrid* object = new X3DUniformTriGrid(mSize);
	object->Detach();
	
	object->mDirty        = mDirty;
	object->mVertices     = mVertices;
	object->mVertexData   = mVertexData;
	object->mVertexColors = mVertexColors;
	object->mTriangles    = mTriangles;
	object->mBoundingBox  = mBoundingBox;

	return object;
}


//---------------------------------------------------------------
//
// X3DUniformTriGrid::Reset
//
//---------------------------------------------------------------
void X3DUniformTriGrid::Reset()
{
	if (mDirty) {
		TQ3TriMeshData data;	
		
		data.triMeshAttributeSet       = nil;
		data.numTriangles              = mTriangles.size();
		data.triangles				   = mTriangles.buffer();
		data.numTriangleAttributeTypes = 0;
		data.triangleAttributeTypes    = nil;
		data.numEdges				   = 0;
		data.edges                     = nil;
		data.numEdgeAttributeTypes     = 0;
		data.edgeAttributeTypes        = nil;
		data.numPoints                 = mVertices.size();
		data.points                    = mVertices.buffer();
		data.numVertexAttributeTypes   = 1;							// diffuse color only
		data.vertexAttributeTypes      = mVertexData.buffer();
		data.bBox.min                  = mBoundingBox.origin;
		data.bBox.max                  = mBoundingBox.origin + mBoundingBox.size;
		data.bBox.isEmpty              = (TQ3Boolean) mBoundingBox.IsEmpty();
		
		TQ3Status status = Q3TriMesh_SetData(mObject, &data);
		ThrowIf3DError(status);
		
		mDirty = false;
	}
}


//---------------------------------------------------------------
//
// X3DUniformTriGrid::GetVertexPosition
//
//---------------------------------------------------------------
X3DPoint X3DUniformTriGrid::GetVertexPosition(const XPoint& pt) const
{
	uint32 vertex = numeric_cast<uint32>(pt.x + pt.y*mSize.width);

	X3DPoint position = mVertices[vertex];
	
	return position;
}

		
//---------------------------------------------------------------
//
// X3DUniformTriGrid::SetVertexPosition
//
//---------------------------------------------------------------
void X3DUniformTriGrid::SetVertexPosition(const XPoint& pt, const X3DPoint& position)
{
	uint32 vertex = numeric_cast<uint32>(pt.x + pt.y*mSize.width);
	
	if (position != mVertices[vertex]) {
		mVertices[vertex] = position;
		
		mBoundingBox |= X3DRect(position, position);
		
		mDirty = true;
	}
}


//---------------------------------------------------------------
//
// X3DUniformTriGrid::GetVertexColor
//
//---------------------------------------------------------------
X3DColorRGB X3DUniformTriGrid::GetVertexColor(const XPoint& pt) const
{
	uint32 vertex = numeric_cast<uint32>(pt.x + pt.y*mSize.width);

	X3DColorRGB color = mVertexColors[vertex];
	
	return color;
}

		
//---------------------------------------------------------------
//
// X3DUniformTriGrid::SetVertexColor
//
//---------------------------------------------------------------
void X3DUniformTriGrid::SetVertexColor(const XPoint& pt, const X3DColorRGB& color)
{
	uint32 vertex = numeric_cast<uint32>(pt.x + pt.y*mSize.width);
	
	if (color != mVertexColors[vertex]) {
		mVertexColors[vertex] = color;
				
		mDirty = true;
	}
}


//---------------------------------------------------------------
//
// X3DUniformTriGrid::SetSize
//
//---------------------------------------------------------------
void X3DUniformTriGrid::SetSize(const XSize& size)
{
	PRECONDITION(size.width >= 2);		// QD 3D 1.5.3 won't create a zero sized trimesh
	PRECONDITION(size.height >= 2);
	
	if (size != mSize) {
		uint32 numVertices = numeric_cast<uint32>(size.width*size.height);
		uint32 numFaces = size != kZeroSize ? 2*(size.width - 1UL)*(size.height - 1UL) : 0UL;
		uint32 numEdges = size != kZeroSize ? 2*(size.width - 1UL) + 2*(size.height - 1UL) : 0UL;
		
		mVertices     = XTinyVector<TQ3Point3D>(numVertices);
		mVertexData   = XTinyVector<TQ3TriMeshAttributeData>(numVertices);
		mVertexColors = XTinyVector<X3DColorRGB>(numVertices);
		mTriangles    = XTinyVector<TQ3TriMeshTriangleData>(numFaces);

		uint32 vertex = 0;
#if DEBUG
		// Initialize the vertex array with some values QD 3D will
		// complain about.
		for (vertex = 0; vertex < mVertices.size(); ++vertex) {
			mVertices[vertex].x = (float) NAN;
			mVertices[vertex].y = (float) NAN;
			mVertices[vertex].z = (float) NAN;
		}
#endif

		// Initialize the vertex colors and the vertex data arrays.
		for (vertex = 0; vertex < mVertices.size(); ++vertex) {
			mVertexColors[vertex] = X3DColorRGB(0.5f, 0.5f, 0.5f);
			
			mVertexData[vertex].attributeType     = kQ3AttributeTypeDiffuseColor;
			mVertexData[vertex].data              = mVertexColors.buffer() + vertex;
			mVertexData[vertex].attributeUseArray = nil;
		}

		// Initialize the triangles array.
		uint32 face = 0;
		vertex = 0;
		for (long v = 0; v < size.height - 1; ++v) {				// use longs in case width or height is zero
			bool even = (v & 1) == 0;
			for (long h = 0; h < size.width - 1; ++h) {
				if (even) {
					mTriangles[face].pointIndices[0] = vertex + size.width;			// .---.
					mTriangles[face].pointIndices[1] = vertex + size.width + 1;		// | / |
					mTriangles[face].pointIndices[2] = vertex;						// *---.  where * is vertex
					face++;

					mTriangles[face].pointIndices[0] = vertex;
					mTriangles[face].pointIndices[1] = vertex + size.width + 1;
					mTriangles[face].pointIndices[2] = vertex + 1;
					face++; vertex++;
				
				} else {
					mTriangles[face].pointIndices[0] = vertex;						// .---.
					mTriangles[face].pointIndices[1] = vertex + size.width;			// | \ |
					mTriangles[face].pointIndices[2] = vertex + 1;					// *---.  where * is vertex
					face++;

					mTriangles[face].pointIndices[0] = vertex + size.width;
					mTriangles[face].pointIndices[1] = vertex + size.width + 1;
					mTriangles[face].pointIndices[2] = vertex + 1;
					face++; vertex++;
				}
				
				ASSERT(vertex + size.width < numVertices);
				
				even = !even;
			}
			++vertex;
		}
		ASSERT(vertex + size.width == numVertices);			// didn't traverse the topmost vertices
		ASSERT(face == numFaces);
		
		mBoundingBox = X3DRect(kZeroRect.GetOSRect());

		mSize = size;
		mDirty = true;
	}
}


//---------------------------------------------------------------
//
// X3DUniformTriGrid::SetVertexBuffer
//
//---------------------------------------------------------------
void X3DUniformTriGrid::SetVertexBuffer(const XTinyVector<TQ3Point3D>& buffer)
{
	PRECONDITION(buffer.size() == mVertices.size());
	
	mVertices = buffer;
				
	mDirty = true;
}


}	// namespace Whisper
