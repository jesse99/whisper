/*
 *  File:       X3DMeshIters.h
 *  Summary:	Classes that iterate over the differant parts of a mesh.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DMeshFaceIter			- Iterates over the faces in a mesh.
 *				X3DMeshComponentIter	- Iterates over the components in a mesh.
 *				X3DMeshEdgeIter			- Iterates over the edges in a mesh.
 *				X3DMeshVertexIter		- Iterates over the vertices in a mesh.
 * 
 *				X3DComponentVertexIter	- Iterates over the vertices in a component.
 *				X3DComponentEdgeIter	- Iterates over the edges in a component.
 *
 *				X3DFaceEdgeIter			- Iterates over the edges in a face.
 *				X3DFaceVertexIter		- Iterates over the vertices in a face.
 *				X3DFaceFaceIter			- Iterates over the faces adjacent to a face.
 *				X3DFaceContourIter		- Iterates over the contours adjacent to a face.
 *
 *				X3DContourEdgeIter		- Iterates over the edges in a contour.
 *				X3DContourVertexIter	- Iterates over the vertices in a contour.
 *				X3DContourFaceIter		- Iterates over the faces adjacent to a contour.
 *
 *				X3DVertexEdgeIter		- Iterates over the edges adjacent to a vertex.
 *				X3DVertexVertexIter		- Iterates over the vertices connected by an edge to the vertex.
 *				X3DVertexFaceIter		- Iterates over the faces adjacent to a vertex.
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DMeshIters.h,v $
 *		Revision 1.3  2000/12/13 08:52:38  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:17:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DMeshIters.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:52:38  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <->	  2/28/97	JDJ		Created.
 */

#pragma once

#include <X3DMesh.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DMeshComponentIter
//!		Iterates over the components in a mesh.
// ===================================================================================
class QUESA_EXPORT X3DMeshComponentIter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DMeshComponentIter(TQ3GeometryObject mesh);
						
//-----------------------------------
//	API
//
public:
						operator bool() const			{return mComponent != nil;}

	X3DMeshComponent 	operator*() const				{return X3DMeshComponent(mMesh, mComponent);}

	X3DMeshComponentIter& operator++();

	X3DMeshComponentIter operator++(int)				{X3DMeshComponentIter temp = *this; ++*this; return temp;}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshIterator		mIterator;
	TQ3MeshComponent	mComponent;
};


// ===================================================================================
//	class X3DComponentVertexIter
//!		Iterates over the vertices in a component.
// ===================================================================================
class QUESA_EXPORT X3DComponentVertexIter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DComponentVertexIter(const X3DMeshComponent& component);
						
//-----------------------------------
//	API
//
public:
						operator bool() const			{return mVertex != nil;}

	X3DMeshVertex 		operator*() const				{return X3DMeshVertex(mMesh, mVertex);}

	X3DComponentVertexIter& operator++();

	X3DComponentVertexIter 	operator++(int)				{X3DComponentVertexIter temp = *this; ++*this; return temp;}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshIterator		mIterator;
	TQ3MeshVertex		mVertex;
};


// ===================================================================================
//	class X3DComponentEdgeIter
//!		Iterates over the edges in a component.
// ===================================================================================
class QUESA_EXPORT X3DComponentEdgeIter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DComponentEdgeIter(const X3DMeshComponent& component);
						
//-----------------------------------
//	API
//
public:
						operator bool() const			{return mEdge != nil;}

	X3DMeshEdge 		operator*() const				{return X3DMeshEdge(mMesh, mEdge);}

	X3DComponentEdgeIter& operator++();

	X3DComponentEdgeIter operator++(int)				{X3DComponentEdgeIter temp = *this; ++*this; return temp;}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshIterator		mIterator;
	TQ3MeshEdge			mEdge;
};


// ===================================================================================
//	class X3DMeshVertexIter
//!		Iterates over the vertices in a mesh.
// ===================================================================================
class QUESA_EXPORT X3DMeshVertexIter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DMeshVertexIter(TQ3GeometryObject mesh);
						
//-----------------------------------
//	API
//
public:
						operator bool() const			{return mVertex != nil;}

	X3DMeshVertex 		operator*() const				{return X3DMeshVertex(mMesh, mVertex);}

	X3DMeshVertexIter& 	operator++();

	X3DMeshVertexIter 	operator++(int)					{X3DMeshVertexIter temp = *this; ++*this; return temp;}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshIterator		mIterator;
	TQ3MeshVertex		mVertex;
};


// ===================================================================================
//	class X3DMeshFaceIter
//!		Iterates over the faces in a mesh.
// ===================================================================================
class QUESA_EXPORT X3DMeshFaceIter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DMeshFaceIter(TQ3GeometryObject mesh);
						
//-----------------------------------
//	API
//
public:
						operator bool() const			{return mFace != nil;}

	X3DMeshFace 		operator*() const				{return X3DMeshFace(mMesh, mFace);}

	X3DMeshFaceIter& 	operator++();

	X3DMeshFaceIter 	operator++(int)					{X3DMeshFaceIter temp = *this; ++*this; return temp;}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshIterator		mIterator;
	TQ3MeshFace			mFace;
};


// ===================================================================================
//	class X3DMeshEdgeIter
//!		Iterates over the edges in a mesh.
// ===================================================================================
class QUESA_EXPORT X3DMeshEdgeIter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DMeshEdgeIter(TQ3GeometryObject mesh);
						
//-----------------------------------
//	API
//
public:
						operator bool() const			{return mEdge != nil;}

	X3DMeshEdge 		operator*() const				{return X3DMeshEdge(mMesh, mEdge);}

	X3DMeshEdgeIter& 	operator++();

	X3DMeshEdgeIter 	operator++(int)					{X3DMeshEdgeIter temp = *this; ++*this; return temp;}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshIterator		mIterator;
	TQ3MeshEdge			mEdge;
};


// ===================================================================================
//	class X3DVertexEdgeIter
//!		Iterates over the edges adjacent to a vertex.
// ===================================================================================
class QUESA_EXPORT X3DVertexEdgeIter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DVertexEdgeIter(const X3DMeshVertex& vertex);
						
//-----------------------------------
//	API
//
public:
						operator bool() const			{return mEdge != nil;}

	X3DMeshEdge 		operator*() const				{return X3DMeshEdge(mMesh, mEdge);}

	X3DVertexEdgeIter& 	operator++();

	X3DVertexEdgeIter 	operator++(int)					{X3DVertexEdgeIter temp = *this; ++*this; return temp;}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshIterator		mIterator;
	TQ3MeshEdge			mEdge;
};


// ===================================================================================
//	class X3DVertexVertexIter
//!		Iterates over the vertices connected by an edge to the vertex.
// ===================================================================================
class QUESA_EXPORT X3DVertexVertexIter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DVertexVertexIter(const X3DMeshVertex& vertex);
						
//-----------------------------------
//	API
//
public:
						operator bool() const			{return mVertex != nil;}

	X3DMeshVertex 		operator*() const				{return X3DMeshVertex(mMesh, mVertex);}

	X3DVertexVertexIter& operator++();

	X3DVertexVertexIter operator++(int)					{X3DVertexVertexIter temp = *this; ++*this; return temp;}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshIterator		mIterator;
	TQ3MeshVertex		mVertex;
};


// ===================================================================================
//	class X3DVertexFaceIter
//!		Iterates over the faces adjacent to a vertex.
// ===================================================================================
class QUESA_EXPORT X3DVertexFaceIter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DVertexFaceIter(const X3DMeshVertex& vertex);
						
//-----------------------------------
//	API
//
public:
						operator bool() const			{return mFace != nil;}

	X3DMeshFace 		operator*() const				{return X3DMeshFace(mMesh, mFace);}

	X3DVertexFaceIter& 	operator++();

	X3DVertexFaceIter 	operator++(int)					{X3DVertexFaceIter temp = *this; ++*this; return temp;}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshIterator		mIterator;
	TQ3MeshFace			mFace;
};


// ===================================================================================
//	class X3DFaceEdgeIter
//!		Iterates over the edges in a face.
// ===================================================================================
class QUESA_EXPORT X3DFaceEdgeIter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DFaceEdgeIter(const X3DMeshFace& face);
						
//-----------------------------------
//	API
//
public:
						operator bool() const			{return mEdge != nil;}

	X3DMeshEdge 		operator*() const				{return X3DMeshEdge(mMesh, mEdge);}

	X3DFaceEdgeIter& 	operator++();

	X3DFaceEdgeIter 	operator++(int)					{X3DFaceEdgeIter temp = *this; ++*this; return temp;}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshIterator		mIterator;
	TQ3MeshEdge			mEdge;
};


// ===================================================================================
//	class X3DFaceVertexIter
//!		Iterates over the vertices in a face.
// ===================================================================================
class QUESA_EXPORT X3DFaceVertexIter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DFaceVertexIter(const X3DMeshFace& face);
						
//-----------------------------------
//	API
//
public:
						operator bool() const			{return mVertex != nil;}

	X3DMeshVertex 		operator*() const				{return X3DMeshVertex(mMesh, mVertex);}

	X3DFaceVertexIter& 	operator++();

	X3DFaceVertexIter 	operator++(int)					{X3DFaceVertexIter temp = *this; ++*this; return temp;}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshIterator		mIterator;
	TQ3MeshVertex		mVertex;
};


// ===================================================================================
//	class X3DFaceFaceIter
//!		Iterates over the faces adjacent to a face.
// ===================================================================================
class QUESA_EXPORT X3DFaceFaceIter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DFaceFaceIter(const X3DMeshFace& face);
						
//-----------------------------------
//	API
//
public:
						operator bool() const			{return mFace != nil;}

	X3DMeshFace 		operator*() const				{return X3DMeshFace(mMesh, mFace);}

	X3DFaceFaceIter& 	operator++();

	X3DFaceFaceIter 	operator++(int)					{X3DFaceFaceIter temp = *this; ++*this; return temp;}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshIterator		mIterator;
	TQ3MeshFace			mFace;
};


// ===================================================================================
//	class X3DFaceContourIter
//!		Iterates over the contours adjacent to a face.
// ===================================================================================
class QUESA_EXPORT X3DFaceContourIter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DFaceContourIter(const X3DMeshFace& face);
						
//-----------------------------------
//	API
//
public:
						operator bool() const			{return mContour != nil;}

	X3DMeshContour 		operator*() const				{return X3DMeshContour(mMesh, mContour);}

	X3DFaceContourIter& operator++();

	X3DFaceContourIter 	operator++(int)					{X3DFaceContourIter temp = *this; ++*this; return temp;}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshIterator		mIterator;
	TQ3MeshContour		mContour;
};


// ===================================================================================
//	class X3DContourEdgeIter
//!		Iterates over the edges in a contour.
// ===================================================================================
class QUESA_EXPORT X3DContourEdgeIter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DContourEdgeIter(const X3DMeshContour& contour);
						
//-----------------------------------
//	API
//
public:
						operator bool() const			{return mEdge != nil;}

	X3DMeshEdge 		operator*() const				{return X3DMeshEdge(mMesh, mEdge);}

	X3DContourEdgeIter& operator++();

	X3DContourEdgeIter 	operator++(int)					{X3DContourEdgeIter temp = *this; ++*this; return temp;}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshIterator		mIterator;
	TQ3MeshEdge			mEdge;
};


// ===================================================================================
//	class X3DContourVertexIter
//!		Iterates over the vertices in a contour.
// ===================================================================================
class QUESA_EXPORT X3DContourVertexIter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DContourVertexIter(const X3DMeshContour& contour);
						
//-----------------------------------
//	API
//
public:
						operator bool() const			{return mVertex != nil;}

	X3DMeshVertex 		operator*() const				{return X3DMeshVertex(mMesh, mVertex);}

	X3DContourVertexIter& operator++();

	X3DContourVertexIter operator++(int)				{X3DContourVertexIter temp = *this; ++*this; return temp;}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshIterator		mIterator;
	TQ3MeshVertex		mVertex;
};


// ===================================================================================
//	class X3DContourFaceIter
//!		Iterates over the faces adjacent to a contour.
// ===================================================================================
class QUESA_EXPORT X3DContourFaceIter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DContourFaceIter(const X3DMeshContour& contour);
						
//-----------------------------------
//	API
//
public:
						operator bool() const			{return mFace != nil;}

	X3DMeshFace 		operator*() const				{return X3DMeshFace(mMesh, mFace);}

	X3DContourFaceIter& operator++();

	X3DContourFaceIter 	operator++(int)					{X3DContourFaceIter temp = *this; ++*this; return temp;}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshIterator		mIterator;
	TQ3MeshFace			mFace;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
