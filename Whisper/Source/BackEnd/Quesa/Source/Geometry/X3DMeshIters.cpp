/*
 *  File:       X3DMeshIters.cpp
 *  Summary:	Classes that iterate over the differant parts of a mesh.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DMeshIters.cpp,v $
 *		Revision 1.4  2001/04/21 03:36:37  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:29:52  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:17:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DMeshIters.h>

namespace Whisper {


// ===================================================================================
//	class X3DMeshComponentIter
// ===================================================================================

//---------------------------------------------------------------
//
// X3DMeshComponentIter::X3DMeshComponentIter
//
//---------------------------------------------------------------
X3DMeshComponentIter::X3DMeshComponentIter(TQ3GeometryObject mesh)
{
	PRECONDITION(mesh != nil);
	
	mMesh = mesh;
	
	mComponent = Q3Mesh_FirstMeshComponent(mMesh, &mIterator);
}


//---------------------------------------------------------------
//
// X3DMeshComponentIter::operator++
//
//---------------------------------------------------------------
X3DMeshComponentIter& X3DMeshComponentIter::operator++()
{
	mComponent = Q3Mesh_NextMeshComponent(&mIterator);
	
	return *this;
}

#pragma mark -

// ===================================================================================
//	class X3DComponentVertexIter
// ===================================================================================

//---------------------------------------------------------------
//
// X3DComponentVertexIter::X3DComponentVertexIter
//
//---------------------------------------------------------------
X3DComponentVertexIter::X3DComponentVertexIter(const X3DMeshComponent& component)
{
	mMesh = component.GetMesh();
	
	mVertex = Q3Mesh_FirstComponentVertex(component, &mIterator);
}


//---------------------------------------------------------------
//
// X3DComponentVertexIter::operator++
//
//---------------------------------------------------------------
X3DComponentVertexIter& X3DComponentVertexIter::operator++()
{
	mVertex = Q3Mesh_NextComponentVertex(&mIterator);
	
	return *this;
}

#pragma mark -

// ===================================================================================
//	class X3DComponentEdgeIter
// ===================================================================================

//---------------------------------------------------------------
//
// X3DComponentEdgeIter::X3DComponentEdgeIter
//
//---------------------------------------------------------------
X3DComponentEdgeIter::X3DComponentEdgeIter(const X3DMeshComponent& component)
{
	mMesh = component.GetMesh();
	
	mEdge = Q3Mesh_FirstComponentEdge(component, &mIterator);
}


//---------------------------------------------------------------
//
// X3DComponentEdgeIter::operator++
//
//---------------------------------------------------------------
X3DComponentEdgeIter& X3DComponentEdgeIter::operator++()
{
	mEdge = Q3Mesh_NextComponentEdge(&mIterator);
	
	return *this;
}

#pragma mark -

// ===================================================================================
//	class X3DMeshVertexIter
// ===================================================================================

//---------------------------------------------------------------
//
// X3DMeshVertexIter::X3DMeshVertexIter
//
//---------------------------------------------------------------
X3DMeshVertexIter::X3DMeshVertexIter(TQ3GeometryObject mesh)
{
	PRECONDITION(mesh != nil);
	
	mMesh = mesh;
	
	mVertex = Q3Mesh_FirstMeshVertex(mesh, &mIterator);
}


//---------------------------------------------------------------
//
// X3DMeshVertexIter::operator++
//
//---------------------------------------------------------------
X3DMeshVertexIter& X3DMeshVertexIter::operator++()
{
	mVertex = Q3Mesh_NextMeshVertex(&mIterator);
	
	return *this;
}

#pragma mark -

// ===================================================================================
//	class X3DMeshFaceIter
// ===================================================================================

//---------------------------------------------------------------
//
// X3DMeshFaceIter::X3DMeshFaceIter
//
//---------------------------------------------------------------
X3DMeshFaceIter::X3DMeshFaceIter(TQ3GeometryObject mesh)
{
	PRECONDITION(mesh != nil);
	
	mMesh = mesh;
	
	mFace = Q3Mesh_FirstMeshFace(mesh, &mIterator);
}


//---------------------------------------------------------------
//
// X3DMeshFaceIter::operator++
//
//---------------------------------------------------------------
X3DMeshFaceIter& X3DMeshFaceIter::operator++()
{
	mFace = Q3Mesh_NextMeshFace(&mIterator);
	
	return *this;
}

#pragma mark -

// ===================================================================================
//	class X3DMeshEdgeIter
// ===================================================================================

//---------------------------------------------------------------
//
// X3DMeshEdgeIter::X3DMeshEdgeIter
//
//---------------------------------------------------------------
X3DMeshEdgeIter::X3DMeshEdgeIter(TQ3GeometryObject mesh)
{
	PRECONDITION(mesh != nil);
	
	mMesh = mesh;
	
	mEdge = Q3Mesh_FirstMeshEdge(mesh, &mIterator);
}


//---------------------------------------------------------------
//
// X3DMeshEdgeIter::operator++
//
//---------------------------------------------------------------
X3DMeshEdgeIter& X3DMeshEdgeIter::operator++()
{
	mEdge = Q3Mesh_NextMeshEdge(&mIterator);
	
	return *this;
}

#pragma mark -

// ===================================================================================
//	class X3DVertexEdgeIter
// ===================================================================================

//---------------------------------------------------------------
//
// X3DVertexEdgeIter::X3DVertexEdgeIter
//
//---------------------------------------------------------------
X3DVertexEdgeIter::X3DVertexEdgeIter(const X3DMeshVertex& vertex)
{
	mMesh = vertex.GetMesh();
	
	mEdge = Q3Mesh_FirstVertexEdge(vertex, &mIterator);
}


//---------------------------------------------------------------
//
// X3DVertexEdgeIter::operator++
//
//---------------------------------------------------------------
X3DVertexEdgeIter& X3DVertexEdgeIter::operator++()
{
	mEdge = Q3Mesh_NextVertexEdge(&mIterator);
	
	return *this;
}

#pragma mark -

// ===================================================================================
//	class X3DVertexVertexIter
// ===================================================================================

//---------------------------------------------------------------
//
// X3DVertexVertexIter::X3DVertexVertexIter
//
//---------------------------------------------------------------
X3DVertexVertexIter::X3DVertexVertexIter(const X3DMeshVertex& vertex)
{
	mMesh = vertex.GetMesh();
	
	mVertex = Q3Mesh_FirstVertexVertex(vertex, &mIterator);
}


//---------------------------------------------------------------
//
// X3DVertexVertexIter::operator++
//
//---------------------------------------------------------------
X3DVertexVertexIter& X3DVertexVertexIter::operator++()
{
	mVertex = Q3Mesh_NextVertexVertex(&mIterator);
	
	return *this;
}

#pragma mark -

// ===================================================================================
//	class X3DVertexFaceIter
// ===================================================================================

//---------------------------------------------------------------
//
// X3DVertexFaceIter::X3DVertexFaceIter
//
//---------------------------------------------------------------
X3DVertexFaceIter::X3DVertexFaceIter(const X3DMeshVertex& vertex)
{
	mMesh = vertex.GetMesh();
	
	mFace = Q3Mesh_FirstVertexFace(vertex, &mIterator);
}


//---------------------------------------------------------------
//
// X3DVertexFaceIter::operator++
//
//---------------------------------------------------------------
X3DVertexFaceIter& X3DVertexFaceIter::operator++()
{
	mFace = Q3Mesh_NextVertexFace(&mIterator);
	
	return *this;
}

#pragma mark -

// ===================================================================================
//	class X3DFaceEdgeIter
// ===================================================================================

//---------------------------------------------------------------
//
// X3DFaceEdgeIter::X3DFaceEdgeIter
//
//---------------------------------------------------------------
X3DFaceEdgeIter::X3DFaceEdgeIter(const X3DMeshFace& face)
{
	mMesh = face.GetMesh();
	
	mEdge = Q3Mesh_FirstFaceEdge(face, &mIterator);
}


//---------------------------------------------------------------
//
// X3DFaceEdgeIter::operator++
//
//---------------------------------------------------------------
X3DFaceEdgeIter& X3DFaceEdgeIter::operator++()
{
	mEdge = Q3Mesh_NextFaceEdge(&mIterator);
	
	return *this;
}

#pragma mark -

// ===================================================================================
//	class X3DFaceVertexIter
// ===================================================================================

//---------------------------------------------------------------
//
// X3DFaceVertexIter::X3DFaceVertexIter
//
//---------------------------------------------------------------
X3DFaceVertexIter::X3DFaceVertexIter(const X3DMeshFace& face)
{
	mMesh = face.GetMesh();
	
	mVertex = Q3Mesh_FirstFaceVertex(face, &mIterator);
}


//---------------------------------------------------------------
//
// X3DFaceVertexIter::operator++
//
//---------------------------------------------------------------
X3DFaceVertexIter& X3DFaceVertexIter::operator++()
{
	mVertex = Q3Mesh_NextFaceVertex(&mIterator);
	
	return *this;
}

#pragma mark -

// ===================================================================================
//	class X3DFaceFaceIter
// ===================================================================================

//---------------------------------------------------------------
//
// X3DFaceFaceIter::X3DFaceFaceIter
//
//---------------------------------------------------------------
X3DFaceFaceIter::X3DFaceFaceIter(const X3DMeshFace& face)
{
	mMesh = face.GetMesh();
	
	mFace = Q3Mesh_FirstFaceFace(face, &mIterator);
}


//---------------------------------------------------------------
//
// X3DFaceFaceIter::operator++
//
//---------------------------------------------------------------
X3DFaceFaceIter& X3DFaceFaceIter::operator++()
{
	mFace = Q3Mesh_NextFaceFace(&mIterator);
	
	return *this;
}

#pragma mark -

// ===================================================================================
//	class X3DFaceContourIter
// ===================================================================================

//---------------------------------------------------------------
//
// X3DFaceContourIter::X3DFaceContourIter
//
//---------------------------------------------------------------
X3DFaceContourIter::X3DFaceContourIter(const X3DMeshFace& face)
{
	mMesh = face.GetMesh();
	
	mContour = Q3Mesh_FirstFaceContour(face, &mIterator);
}


//---------------------------------------------------------------
//
// X3DFaceContourIter::operator++
//
//---------------------------------------------------------------
X3DFaceContourIter& X3DFaceContourIter::operator++()
{
	mContour = Q3Mesh_NextFaceContour(&mIterator);
	
	return *this;
}

#pragma mark -

// ===================================================================================
//	class X3DContourEdgeIter
// ===================================================================================

//---------------------------------------------------------------
//
// X3DContourEdgeIter::X3DContourEdgeIter
//
//---------------------------------------------------------------
X3DContourEdgeIter::X3DContourEdgeIter(const X3DMeshContour& contour)
{
	mMesh = contour.GetMesh();
	
	mEdge = Q3Mesh_FirstContourEdge(contour, &mIterator);
}


//---------------------------------------------------------------
//
// X3DContourEdgeIter::operator++
//
//---------------------------------------------------------------
X3DContourEdgeIter& X3DContourEdgeIter::operator++()
{
	mEdge = Q3Mesh_NextContourEdge(&mIterator);
	
	return *this;
}

#pragma mark -

// ===================================================================================
//	class X3DContourVertexIter
// ===================================================================================

//---------------------------------------------------------------
//
// X3DContourVertexIter::X3DContourVertexIter
//
//---------------------------------------------------------------
X3DContourVertexIter::X3DContourVertexIter(const X3DMeshContour& contour)
{
	mMesh = contour.GetMesh();
	
	mVertex = Q3Mesh_FirstContourVertex(contour, &mIterator);
}


//---------------------------------------------------------------
//
// X3DContourVertexIter::operator++
//
//---------------------------------------------------------------
X3DContourVertexIter& X3DContourVertexIter::operator++()
{
	mVertex = Q3Mesh_NextContourVertex(&mIterator);
	
	return *this;
}

#pragma mark -

// ===================================================================================
//	class X3DContourFaceIter
// ===================================================================================

//---------------------------------------------------------------
//
// X3DContourFaceIter::X3DContourFaceIter
//
//---------------------------------------------------------------
X3DContourFaceIter::X3DContourFaceIter(const X3DMeshContour& contour)
{
	mMesh = contour.GetMesh();
	
	mFace = Q3Mesh_FirstContourFace(contour, &mIterator);
}


//---------------------------------------------------------------
//
// X3DContourFaceIter::operator++
//
//---------------------------------------------------------------
X3DContourFaceIter& X3DContourFaceIter::operator++()
{
	mFace = Q3Mesh_NextContourFace(&mIterator);
	
	return *this;
}


}	// namespace Whisper
