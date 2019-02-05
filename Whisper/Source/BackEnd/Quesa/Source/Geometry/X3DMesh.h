/*
 *  File:       X3DMesh.h
 *  Summary:	A very flexible 3D mesh that's well suited for interactive editing.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DMeshComponent - A collection of connected vertices. Meshes will have zero or more of these.
 *				X3DMeshVertex	 - A mesh vertex with optional attributes.
 *				X3DMeshFace		 - A polygonal figure defined by a list of vertices.
 *				X3DMeshEdge		 - The border between two vertices.
 *				X3DMeshContour	 - A hole in the mesh.
 *				X3DMesh			 - A collection of faces, countours, and vertices.
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DMesh.h,v $
 *		Revision 1.3  2000/12/13 08:52:32  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:17:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DMesh.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:52:32  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	  2/28/97	JDJ		Created.
 */

#pragma once

#include <X3DAttributeSet.h>
#include <X3DGeometry.h>
#include <X3DPrimitives.h>
#include <X3DVectors.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DMeshComponent
//!		A collection of connected vertices. Meshes will have zero or more of these.
// ===================================================================================
class QUESA_EXPORT X3DMeshComponent {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						X3DMeshComponent(TQ3GeometryObject mesh, TQ3MeshComponent component);

//-----------------------------------
//	API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3MeshComponent() const			{return mComponent;}
	//@}

	//! @name Getters
	//@{
			uint32 		GetNumVertices() const;
	
			uint32 		GetNumEdges() const;

			X3DRect 	GetBoundingBox() const;

			bool 		IsOrientable() const;

			TQ3GeometryObject GetMesh() const						{return mMesh;}
	//@}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshComponent	mComponent;
};


// ===================================================================================
//	class X3DMeshVertex
//!		A mesh vertex with optional attributes.
// ===================================================================================
class X3DMeshVertex {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						X3DMeshVertex(TQ3GeometryObject mesh, TQ3MeshVertex vertex);

//-----------------------------------
//	API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3MeshVertex() const			{return mVertex;}
	//@}

	//! @name Coordinates
	//@{
			X3DPoint 	GetCoordinates() const;

			void 		SetCoordinates(const X3DPoint& pt);
	//@}

	//! @name Attributes
	//@{
			TQ3AttributeSet GetAttributes() const;

			void 		SetAttributes(TQ3AttributeSet attributes);
	//@}

	//! @name Misc
	//@{
			uint32 		GetIndex() const;
			
			bool 		IsOnBoundary() const;
			
			X3DMeshComponent GetComponent() const;

			TQ3GeometryObject GetMesh() const					{return mMesh;}
	//@}

//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshVertex		mVertex;
};


// ===================================================================================
//	class X3DMeshFace
//!		A polygonal figure defined by a list of vertices.
// ===================================================================================
class X3DMeshFace {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						X3DMeshFace(TQ3GeometryObject mesh, TQ3MeshFace face);
						
//-----------------------------------
//	API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3MeshFace() const			{return mFace;}
	//@}

	//! @name Components
	//@{
			X3DMeshComponent GetComponent() const;
	//@}

	//! @name Attributes
	//@{
			TQ3AttributeSet GetAttributes() const;

			void 		SetAttributes(TQ3AttributeSet attributes);
	//@}

	//! @name Misc
	//@{
			uint32 		GetNumVertices() const;
			
			uint32 		GetNumContours() const;

			TQ3PlaneEquation GetPlaneEquation() const;

			uint32 		GetIndex() const;

			TQ3GeometryObject GetMesh() const					{return mMesh;}
	//@}
	
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshFace			mFace;
};


// ===================================================================================
//	class X3DMeshEdge
//!		The border between two vertices.
// ===================================================================================
class X3DMeshEdge {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						X3DMeshEdge(TQ3GeometryObject mesh, TQ3MeshEdge edge);
						
//-----------------------------------
//	API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3MeshEdge() const			{return mEdge;}
	//@}
			
	//! @name Vertices
	//@{
			X3DMeshVertex GetVertex1() const;

			X3DMeshVertex GetVertex2() const;
	//@}

	//! @name Faces
	//@{
			X3DMeshFace GetFace1() const;

			X3DMeshFace GetFace2() const;
	//@}

	//! @name Components
	//@{
			X3DMeshComponent GetComponent() const;
	//@}

	//! @name Attributes
	//@{
			TQ3AttributeSet GetAttributes() const;

			void 		SetAttributes(TQ3AttributeSet attributes);
	//@}

	//! @name Misc
	//@{
			bool 		IsOnBoundary() const;

			TQ3GeometryObject GetMesh() const					{return mMesh;}
	//@}
		
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshEdge			mEdge;
};


// ===================================================================================
//	class X3DMeshContour
//!		A hole in the mesh.
// ===================================================================================
class X3DMeshContour {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						X3DMeshContour(TQ3GeometryObject mesh, TQ3MeshContour contour);
						
//-----------------------------------
//	API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3MeshContour() const			{return mContour;}
	//@}

	//! @name Misc
	//@{
			uint32 		GetNumVertices() const;

			X3DMeshFace GetFace() const;

			TQ3GeometryObject GetMesh() const					{return mMesh;}
	//@}
		
//-----------------------------------
//	Member Data
//
protected:
	TQ3GeometryObject	mMesh;
	TQ3MeshContour		mContour;
};


// ===================================================================================
//	class X3DMesh
//!		A collection of faces, countours, and vertices.
// ===================================================================================
class X3DMesh : public X3DGeometry {

	typedef X3DGeometry Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DMesh();
						
						X3DMesh();

						X3DMesh(TQ3GeometryObject object);
						
	virtual X3DMesh* 	Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Vertices
	//@{
			X3DMeshVertex AddVertex(const X3DPoint& pt, TQ3AttributeSet attributes = nil);
			X3DMeshVertex AddVertex(const TQ3Vertex3D& vertex);
						/**< Returns the new vertex. */
						
			void 		DeleteVertex(const X3DMeshVertex& vertex);	
	//@}
					
	//! @name Faces
	//@{
			X3DMeshFace AddFace(uint32 numVertices, const TQ3MeshVertex* vertices, TQ3AttributeSet attributes = nil);
						/**< Returns the new face. */
						
			void 		DeleteFace(const X3DMeshFace& face);
	//@}

	//! @name Methods
	//@{
			void 		DelayUpdates();
						/**< Call this before adding vertices and faces. */
						
			void 		ResumeUpdates();
						/**< Call this when you're done adding vertices and faces. */
						
			X3DMeshContour FaceToContour(const X3DMeshFace& containerFace, const X3DMeshFace& face);
			
			X3DMeshFace ContourToFace(const X3DMeshContour& contour);
	//@}

	//! @name Mesh
	//@{
			uint32 		GetNumComponents() const;

			uint32 		GetNumEdges() const;

			uint32 		GetNumVertices() const;

			uint32 		GetNumFaces() const;

			uint32 		GetNumCorners() const;

			bool 		IsOrientable() const;
	//@}

	//! @name Corner
	//@{
			TQ3AttributeSet GetCornerAttributes(const TQ3MeshVertex& vertex, const TQ3MeshFace& face) const;

			void 		SetCornerAttributes(const TQ3MeshVertex& vertex, const TQ3MeshFace& face, TQ3AttributeSet attributes);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DMesh. */
	//@}
};



#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
