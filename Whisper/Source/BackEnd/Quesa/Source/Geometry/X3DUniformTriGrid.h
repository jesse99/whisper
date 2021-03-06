/*
 *  File:       X3DUniformTriGrid.h
 *  Summary:	X3DTriMesh that works like a trigrid.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DUniformTriGrid.h,v $
 *		Revision 1.4  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/13 08:55:34  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:23:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	  1/26/98	JDJ		Removed operator< hacks (TSimpleVector outlines
 *									operator< to avoid having CW require operator<
 *									for every type used with TSimpleVector).
 *		 <1>	 10/02/97	JDJ		Created.
 */

#pragma once

#include <X3DColors.h>
#include <X3DTriMesh.h>
#include <XTinyVector.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DUniformTriGrid
//!		X3DTriMesh that works like a trigrid.
/*!		You can use this class when you have a 2D array of points that you wish to render 
 *		(eg a continuous surface). Note that this is a better choice than a trigrid because 
 *		a trigrid uses an attribute set for each vertex and face. A trimesh, on the other 
 *		hand, can store an array of colors which will be much more space efficent if there 
 *		are many different colors. */
// ===================================================================================
class QUESA_EXPORT X3DUniformTriGrid : public X3DTriMesh {

	typedef X3DTriMesh Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DUniformTriGrid();
						
						X3DUniformTriGrid(const XSize& size);
						
			void 		Reset();
						/**< After editing the mesh you must call this method to inform
						QuickDraw 3D of the changes. */
						
	virtual X3DUniformTriGrid* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Number of Vertices
	//@{
			XSize 		GetSize() const											{return mSize;}

			long 		GetWidth() const										{return mSize.width;}

			long 		GetHeight() const										{return mSize.height;}
						
			void 		SetSize(const XSize& size);
						/**< Wipes out old points. */
	//@}

	//! @name Vertex Position
	//@{
			X3DPoint 	GetVertexPosition(const XPoint& vertex) const;
						
			void 		SetVertexPosition(const XPoint& vertex, const X3DPoint& position);
	//@}

	//! @name Vertex Color
	//@{
			X3DColorRGB GetVertexColor(const XPoint& vertex) const;

			void 		SetVertexColor(const XPoint& vertex, const X3DColorRGB& color);
	//@}
			
	//! @name Misc
	//@{
			const XTinyVector<TQ3Point3D>& GetVertexBuffer()	const			{return mVertices;}

			XTinyVector<TQ3Point3D>& GetVertexBuffer()							{return mVertices;}

			void 		SetVertexBuffer(const XTinyVector<TQ3Point3D>& buffer);
	//@}
						
//-----------------------------------
//	Inherited API
//
public:
						operator TQ3Object() const								{ASSERT(!mDirty); return Inherited::operator TQ3Object();}

			TQ3TriMeshData GetData() const										{ASSERT(!mDirty); return Inherited::GetData();}

			void 		Submit(TQ3ViewObject view) const						{ASSERT(!mDirty); Inherited::Submit(view);}
									
	static	void 		Submit(const TQ3TriMeshData& data, TQ3ViewObject view)	{Inherited::Submit(data, view);}

//-----------------------------------
//	Member Data
//
protected:
	XSize									mSize;
	bool									mDirty;				// ie needs to be reset
	
	XTinyVector<TQ3Point3D>					mVertices;
	XTinyVector<TQ3TriMeshAttributeData>	mVertexData;
	XTinyVector<X3DColorRGB>				mVertexColors;
	XTinyVector<TQ3TriMeshTriangleData>		mTriangles;
	
	X3DRect									mBoundingBox;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
