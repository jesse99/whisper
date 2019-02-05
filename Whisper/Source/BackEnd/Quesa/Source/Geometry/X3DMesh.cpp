/*
 *  File:       X3DMesh.cpp
 *  Summary:	A very flexible 3D mesh that's well suited for interactive editing.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DMesh.cpp,v $
 *		Revision 1.4  2001/04/21 03:36:30  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:29:47  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:17:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DMesh.h>

#include <X3DUtils.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateMesh
//
//---------------------------------------------------------------
static X3DObject* CreateMesh(TQ3Object inObject)
{
	X3DMesh* object = new X3DMesh(inObject);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DMeshComponent
// ===================================================================================

//---------------------------------------------------------------
//
// X3DMeshComponent::X3DMeshComponent
//
//---------------------------------------------------------------
X3DMeshComponent::X3DMeshComponent(TQ3GeometryObject mesh, TQ3MeshComponent component)
{
	PRECONDITION(mesh != nil);
	PRECONDITION(component != nil);
	
	mMesh = mesh;
	mComponent = component;
}


//---------------------------------------------------------------
//
// X3DMeshComponent::GetNumVertices
//
//---------------------------------------------------------------
uint32 X3DMeshComponent::GetNumVertices() const
{
	uint32 count = 0;
	
	TQ3Status status = Q3Mesh_GetComponentNumVertices(mMesh, mComponent, &count);
	ThrowIf3DError(status);
	
	return count;
}


//---------------------------------------------------------------
//
// X3DMeshComponent::GetNumEdges
//
//---------------------------------------------------------------
uint32 X3DMeshComponent::GetNumEdges() const
{
	uint32 count = 0;
	
	TQ3Status status = Q3Mesh_GetComponentNumEdges(mMesh, mComponent, &count);
	ThrowIf3DError(status);
	
	return count;
}


//---------------------------------------------------------------
//
// X3DMeshComponent::GetBoundingBox
//
//---------------------------------------------------------------
X3DRect X3DMeshComponent::GetBoundingBox() const
{
	TQ3BoundingBox box;
	
	TQ3Status status = Q3Mesh_GetComponentBoundingBox(mMesh, mComponent, &box);
	ThrowIf3DError(status);
	
	return box.isEmpty ? kZero3DRect : X3DRect(box.min, box.max);
}


//---------------------------------------------------------------
//
// X3DMeshComponent::IsOrientable
//
//---------------------------------------------------------------
bool X3DMeshComponent::IsOrientable() const
{
	TQ3Boolean orientable;
	
	TQ3Status status = Q3Mesh_GetComponentOrientable(mMesh, mComponent, &orientable);
	ThrowIf3DError(status);
	
	return orientable;
}

#pragma mark -

// ===================================================================================
//	class X3DMeshVertex
// ===================================================================================

//---------------------------------------------------------------
//
// X3DMeshVertex::X3DMeshVertex
//
//---------------------------------------------------------------
X3DMeshVertex::X3DMeshVertex(TQ3GeometryObject mesh, TQ3MeshVertex vertex)
{
	PRECONDITION(mesh != nil);
	PRECONDITION(vertex != nil);
	
	mMesh = mesh;
	mVertex = vertex;
}


//---------------------------------------------------------------
//
// X3DMeshVertex::GetCoordinates
//
//---------------------------------------------------------------
X3DPoint X3DMeshVertex::GetCoordinates() const
{
	TQ3Point3D pt;
	
	TQ3Status status = Q3Mesh_GetVertexCoordinates(mMesh, mVertex, &pt);
	ThrowIf3DError(status);
	
	return pt;
}


//---------------------------------------------------------------
//
// X3DMeshVertex::SetCoordinates
//
//---------------------------------------------------------------
void X3DMeshVertex::SetCoordinates(const X3DPoint& pt)
{
	TQ3Status status = Q3Mesh_SetVertexCoordinates(mMesh, mVertex, pt);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMeshVertex::GetAttributes
//
//---------------------------------------------------------------
TQ3AttributeSet X3DMeshVertex::GetAttributes() const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3Mesh_GetVertexAttributeSet(mMesh, mVertex, &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}


//---------------------------------------------------------------
//
// X3DMeshVertex::SetAttributes
//
//---------------------------------------------------------------
void X3DMeshVertex::SetAttributes(TQ3AttributeSet attributes)
{
	TQ3Status status = Q3Mesh_SetVertexAttributeSet(mMesh, mVertex, attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMeshVertex::GetIndex
//
//---------------------------------------------------------------
uint32 X3DMeshVertex::GetIndex() const
{
	uint32 index = 0;
	
	TQ3Status status = Q3Mesh_GetVertexIndex(mMesh, mVertex, &index);
	ThrowIf3DError(status);
	
	return index;
}


//---------------------------------------------------------------
//
// X3DMeshVertex::IsOnBoundary
//
//---------------------------------------------------------------
bool X3DMeshVertex::IsOnBoundary() const
{
	TQ3Boolean onBoundary;
	
	TQ3Status status = Q3Mesh_GetVertexOnBoundary(mMesh, mVertex, &onBoundary);
	ThrowIf3DError(status);
	
	return onBoundary;
}


//---------------------------------------------------------------
//
// X3DMeshVertex::GetComponent
//
//---------------------------------------------------------------
X3DMeshComponent X3DMeshVertex::GetComponent() const
{
	TQ3MeshComponent component;
	
	TQ3Status status = Q3Mesh_GetVertexComponent(mMesh, mVertex, &component);
	ThrowIf3DError(status);
	
	return X3DMeshComponent(mMesh, component);
}

#pragma mark -

// ===================================================================================
//	class X3DMeshFace
// ===================================================================================

//---------------------------------------------------------------
//
// X3DMeshFace::X3DMeshFace
//
//---------------------------------------------------------------
X3DMeshFace::X3DMeshFace(TQ3GeometryObject mesh, TQ3MeshFace face)
{
	PRECONDITION(mesh != nil);
	PRECONDITION(face != nil);
	
	mMesh = mesh;
	mFace = face;
}


//---------------------------------------------------------------
//
// X3DMeshFace::GetComponent
//
//---------------------------------------------------------------
X3DMeshComponent X3DMeshFace::GetComponent() const
{
	TQ3MeshComponent component;
	
	TQ3Status status = Q3Mesh_GetFaceComponent(mMesh, mFace, &component);
	ThrowIf3DError(status);
	
	return X3DMeshComponent(mMesh, component);
}


//---------------------------------------------------------------
//
// X3DMeshFace::GetAttributes
//
//---------------------------------------------------------------
TQ3AttributeSet X3DMeshFace::GetAttributes() const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3Mesh_GetFaceAttributeSet(mMesh, mFace, &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}


//---------------------------------------------------------------
//
// X3DMeshFace::SetAttributes
//
//---------------------------------------------------------------
void X3DMeshFace::SetAttributes(TQ3AttributeSet attributes)
{
	TQ3Status status = Q3Mesh_SetFaceAttributeSet(mMesh, mFace, attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMeshFace::GetNumVertices
//
//---------------------------------------------------------------
uint32 X3DMeshFace::GetNumVertices() const
{
	uint32 count = 0;
	
	TQ3Status status = Q3Mesh_GetFaceNumVertices(mMesh, mFace, &count);
	ThrowIf3DError(status);
	
	return count;
}


//---------------------------------------------------------------
//
// X3DMeshFace::GetNumContours
//
//---------------------------------------------------------------
uint32 X3DMeshFace::GetNumContours() const
{
	uint32 count = 0;
	
	TQ3Status status = Q3Mesh_GetFaceNumContours(mMesh, mFace, &count);
	ThrowIf3DError(status);
	
	return count;
}


//---------------------------------------------------------------
//
// X3DMeshFace::GetPlaneEquation
//
//---------------------------------------------------------------
TQ3PlaneEquation X3DMeshFace::GetPlaneEquation() const
{
	TQ3PlaneEquation equation;
	
	TQ3Status status = Q3Mesh_GetFacePlaneEquation(mMesh, mFace, &equation);
	ThrowIf3DError(status);
	
	return equation;
}


//---------------------------------------------------------------
//
// X3DMeshFace::GetIndex
//
//---------------------------------------------------------------
uint32 X3DMeshFace::GetIndex() const
{
	uint32 index = 0;
	
	TQ3Status status = Q3Mesh_GetFaceIndex(mMesh, mFace, &index);
	ThrowIf3DError(status);
	
	return index;
}

#pragma mark -

// ===================================================================================
//	class X3DMeshEdge
// ===================================================================================

//---------------------------------------------------------------
//
// X3DMeshEdge::X3DMeshEdge
//
//---------------------------------------------------------------
X3DMeshEdge::X3DMeshEdge(TQ3GeometryObject mesh, TQ3MeshEdge edge)
{
	PRECONDITION(mesh != nil);
	PRECONDITION(edge != nil);
	
	mMesh = mesh;
	mEdge = edge;
}


//---------------------------------------------------------------
//
// X3DMeshEdge::GetVertex1
//
//---------------------------------------------------------------
X3DMeshVertex X3DMeshEdge::GetVertex1() const
{
	TQ3MeshVertex vertex1, vertex2;
	
	TQ3Status status = Q3Mesh_GetEdgeVertices(mMesh, mEdge, &vertex1, &vertex2);
	ThrowIf3DError(status);
	
	return X3DMeshVertex(mMesh, vertex1);
}


//---------------------------------------------------------------
//
// X3DMeshEdge::GetVertex2
//
//---------------------------------------------------------------
X3DMeshVertex X3DMeshEdge::GetVertex2() const
{
	TQ3MeshVertex vertex1, vertex2;
	
	TQ3Status status = Q3Mesh_GetEdgeVertices(mMesh, mEdge, &vertex1, &vertex2);
	ThrowIf3DError(status);
	
	return X3DMeshVertex(mMesh, vertex2);
}


//---------------------------------------------------------------
//
// X3DMeshEdge::GetFace1
//
//---------------------------------------------------------------
X3DMeshFace X3DMeshEdge::GetFace1() const
{
	TQ3MeshFace face1, face2;
	
	TQ3Status status = Q3Mesh_GetEdgeFaces(mMesh, mEdge, &face1, &face2);
	ThrowIf3DError(status);
	
	return X3DMeshFace(mMesh, face1);
}


//---------------------------------------------------------------
//
// X3DMeshEdge::GetFace2
//
//---------------------------------------------------------------
X3DMeshFace X3DMeshEdge::GetFace2() const
{
	TQ3MeshFace face1, face2;
	
	TQ3Status status = Q3Mesh_GetEdgeFaces(mMesh, mEdge, &face1, &face2);
	ThrowIf3DError(status);
	
	return X3DMeshFace(mMesh, face2);
}


//---------------------------------------------------------------
//
// X3DMeshEdge::GetComponent
//
//---------------------------------------------------------------
X3DMeshComponent X3DMeshEdge::GetComponent() const
{
	TQ3MeshComponent component;
	
	TQ3Status status = Q3Mesh_GetEdgeComponent(mMesh, mEdge, &component);
	ThrowIf3DError(status);
	
	return X3DMeshComponent(mMesh, component);
}


//---------------------------------------------------------------
//
// X3DMeshEdge::GetAttributes
//
//---------------------------------------------------------------
TQ3AttributeSet X3DMeshEdge::GetAttributes() const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3Mesh_GetEdgeAttributeSet(mMesh, mEdge, &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}


//---------------------------------------------------------------
//
// X3DMeshEdge::SetAttributes
//
//---------------------------------------------------------------
void X3DMeshEdge::SetAttributes(TQ3AttributeSet attributes)
{
	TQ3Status status = Q3Mesh_SetEdgeAttributeSet(mMesh, mEdge, attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMeshEdge::IsOnBoundary
//
//---------------------------------------------------------------
bool X3DMeshEdge::IsOnBoundary() const
{
	TQ3Boolean onBoundary;
	
	TQ3Status status = Q3Mesh_GetEdgeOnBoundary(mMesh, mEdge, &onBoundary);
	ThrowIf3DError(status);
	
	return onBoundary;
}

#pragma mark -

// ===================================================================================
//	class X3DMeshContour
// ===================================================================================

//---------------------------------------------------------------
//
// X3DMeshContour::X3DMeshContour
//
//---------------------------------------------------------------
X3DMeshContour::X3DMeshContour(TQ3GeometryObject mesh, TQ3MeshContour contour)
{
	PRECONDITION(mesh != nil);
	PRECONDITION(contour != nil);
	
	mMesh = mesh;
	mContour = contour;
}


//---------------------------------------------------------------
//
// X3DMeshContour::GetNumVertices
//
//---------------------------------------------------------------
uint32 X3DMeshContour::GetNumVertices() const
{
	uint32 count = 0;
	
	TQ3Status status = Q3Mesh_GetContourNumVertices(mMesh, mContour, &count);
	ThrowIf3DError(status);
	
	return count;
}


//---------------------------------------------------------------
//
// X3DMeshContour::GetFace
//
//---------------------------------------------------------------
X3DMeshFace X3DMeshContour::GetFace() const
{
	TQ3MeshFace face;
	
	TQ3Status status = Q3Mesh_GetContourFace(mMesh, mContour, &face);
	ThrowIf3DError(status);
	
	return X3DMeshFace(mMesh, face);
}

#pragma mark -

// ===================================================================================
//	class X3DMesh
// ===================================================================================

//---------------------------------------------------------------
//
// X3DMesh::~X3DMesh
//
//---------------------------------------------------------------
X3DMesh::~X3DMesh()
{
}


//---------------------------------------------------------------
//
// X3DMesh::X3DMesh
//
//---------------------------------------------------------------
X3DMesh::X3DMesh() : X3DGeometry(Q3Mesh_New())
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMesh::X3DMesh
//
//---------------------------------------------------------------
X3DMesh::X3DMesh(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypeMesh);
}


//---------------------------------------------------------------
//
// X3DMesh::Clone
//
//---------------------------------------------------------------
X3DMesh* X3DMesh::Clone() const
{
	X3DMesh* object = new X3DMesh(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DMesh::AddVertex (X3DPoint, TQ3AttributeSet)
//
//---------------------------------------------------------------
X3DMeshVertex X3DMesh::AddVertex(const X3DPoint& pt, TQ3AttributeSet attributes)
{
	TQ3Vertex3D vertexData;
	vertexData.point = pt;
	vertexData.attributeSet = attributes;
	
	TQ3MeshVertex vertex = Q3Mesh_VertexNew(mObject, &vertexData);
	ThrowIfNil(vertex);
	
	return X3DMeshVertex(mObject, vertex);
}


//---------------------------------------------------------------
//
// X3DMesh::AddVertex (TQ3Vertex3D)
//
//---------------------------------------------------------------
X3DMeshVertex X3DMesh::AddVertex(const TQ3Vertex3D& vertex)
{
	TQ3MeshVertex v = Q3Mesh_VertexNew(mObject, &vertex);
	ThrowIfNil(v);
	
	return X3DMeshVertex(mObject, v);
}


//---------------------------------------------------------------
//
// X3DMesh::DeleteVertex
//
//---------------------------------------------------------------
void X3DMesh::DeleteVertex(const X3DMeshVertex& vertex)
{
	TQ3Status status = Q3Mesh_VertexDelete(mObject, vertex);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMesh::AddFace
//
//---------------------------------------------------------------
X3DMeshFace X3DMesh::AddFace(uint32 numVertices, const TQ3MeshVertex* vertices, TQ3AttributeSet attributes)
{
	TQ3MeshFace face = Q3Mesh_FaceNew(mObject, numVertices, vertices, attributes);
	ThrowIfNil(face);
	
	return X3DMeshFace(mObject, face);
}


//---------------------------------------------------------------
//
// X3DMesh::DeleteFace
//
//---------------------------------------------------------------
void X3DMesh::DeleteFace(const X3DMeshFace& face)
{
	TQ3Status status = Q3Mesh_FaceDelete(mObject, face);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMesh::DelayUpdates
//
//---------------------------------------------------------------
void X3DMesh::DelayUpdates()
{
	TQ3Status status = Q3Mesh_DelayUpdates(mObject);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMesh::ResumeUpdates
//
//---------------------------------------------------------------
void X3DMesh::ResumeUpdates()
{
	TQ3Status status = Q3Mesh_ResumeUpdates(mObject);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMesh::FaceToContour
//
//---------------------------------------------------------------
X3DMeshContour X3DMesh::FaceToContour(const X3DMeshFace& containerFace, const X3DMeshFace& face)
{
	TQ3MeshContour contour = Q3Mesh_FaceToContour(mObject, containerFace, face);
	ThrowIfNil(contour);
	
	return X3DMeshContour(mObject, contour);
}


//---------------------------------------------------------------
//
// X3DMesh::ContourToFace
//
//---------------------------------------------------------------
X3DMeshFace X3DMesh::ContourToFace(const X3DMeshContour& contour)
{
	TQ3MeshFace face = Q3Mesh_ContourToFace(mObject, contour);
	ThrowIfNil(face);
	
	return X3DMeshFace(mObject, face);
}


//---------------------------------------------------------------
//
// X3DMesh::GetNumComponents
//
//---------------------------------------------------------------
uint32 X3DMesh::GetNumComponents() const
{
	uint32 count = 0;
	
	TQ3Status status = Q3Mesh_GetNumComponents(mObject, &count);
	ThrowIf3DError(status);
	
	return count;
}


//---------------------------------------------------------------
//
// X3DMesh::GetNumEdges
//
//---------------------------------------------------------------
uint32 X3DMesh::GetNumEdges() const
{
	uint32 count = 0;
	
	TQ3Status status = Q3Mesh_GetNumEdges(mObject, &count);
	ThrowIf3DError(status);
	
	return count;
}


//---------------------------------------------------------------
//
// X3DMesh::GetNumVertices
//
//---------------------------------------------------------------
uint32 X3DMesh::GetNumVertices() const
{
	uint32 count = 0;
	
	TQ3Status status = Q3Mesh_GetNumVertices(mObject, &count);
	ThrowIf3DError(status);
	
	return count;
}


//---------------------------------------------------------------
//
// X3DMesh::GetNumFaces
//
//---------------------------------------------------------------
uint32 X3DMesh::GetNumFaces() const
{
	uint32 count = 0;
	
	TQ3Status status = Q3Mesh_GetNumFaces(mObject, &count);
	ThrowIf3DError(status);
	
	return count;
}


//---------------------------------------------------------------
//
// X3DMesh::GetNumCorners
//
//---------------------------------------------------------------
uint32 X3DMesh::GetNumCorners() const
{
	uint32 count = 0;
	
	TQ3Status status = Q3Mesh_GetNumCorners(mObject, &count);
	ThrowIf3DError(status);
	
	return count;
}


//---------------------------------------------------------------
//
// X3DMesh::IsOrientable
//
//---------------------------------------------------------------
bool X3DMesh::IsOrientable() const
{
	TQ3Boolean orientable;
	
	TQ3Status status = Q3Mesh_GetOrientable(mObject, &orientable);
	ThrowIf3DError(status);
	
	return orientable;
}


//---------------------------------------------------------------
//
// X3DMesh::GetCornerAttributes
//
//---------------------------------------------------------------
TQ3AttributeSet X3DMesh::GetCornerAttributes(const TQ3MeshVertex& vertex, const TQ3MeshFace& face) const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3Mesh_GetCornerAttributeSet(mObject, vertex, face, &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}


//---------------------------------------------------------------
//
// X3DMesh::SetCornerAttributes
//
//---------------------------------------------------------------
void X3DMesh::SetCornerAttributes(const TQ3MeshVertex& vertex, const TQ3MeshFace& face, TQ3AttributeSet attributes)
{
	TQ3Status status = Q3Mesh_SetCornerAttributeSet(mObject, vertex, face, attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMesh::IsCompatible 								[static]
//	
//---------------------------------------------------------------
bool X3DMesh::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypeMesh);
	
	return compatible;
}


}	// namespace Whisper
