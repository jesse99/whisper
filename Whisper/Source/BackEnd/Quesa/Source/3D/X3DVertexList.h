/*
 *  File:       X3DVertexList.h
 *  Summary:	Encapsulates a TQ3Vertex3D vector.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DVertexList.h,v $
 *		Revision 1.3  2000/12/13 08:55:49  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:24:18  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DVertexList.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:55:49  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	  4/16/00	JDJ		Created.
 */

#pragma once

#include <X3DAttributeSet.h>
#include <X3DPrimitives.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class ZBase3DVertexList
//!		Used by X3DVertexList.
// ===================================================================================
class QUESA_EXPORT ZBase3DVertexList {

protected:
			void 		DoDestroy(TQ3Vertex3D* vertices, uint32 count);
			X3DAttributeSet DoGetAttribute(const TQ3Vertex3D* vertices, uint32 count, uint32 index) const;
			void 		DoSetAttribute(TQ3Vertex3D* vertices, uint32 count, uint32 index, TQ3AttributeSet attr);

			void 		DoDestroyAttributes(TQ3AttributeSet* attributes, uint32 count);
			X3DAttributeSet DoGetAttribute(const TQ3AttributeSet* attributes, uint32 count, uint32 index) const;
			void 		DoSetAttribute(TQ3AttributeSet* attributes, uint32 count, uint32 index, TQ3AttributeSet attr);
						// These are here to reduce code bloat.
};


// ===================================================================================
//	class X3DVertexList
//!		TQ3Vertex3D wrapper.
// ===================================================================================
template <int N>
class X3DVertexList : private ZBase3DVertexList {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~X3DVertexList()									{this->DoDestroy(mVertices, N);}
						
						X3DVertexList()										{std::memset(mVertices, 0, sizeof(mVertices));}

private:
						X3DVertexList(const X3DVertexList& rhs);
						
			X3DVertexList& operator=(const X3DVertexList& rhs);
		
//-----------------------------------
//	API
//
public:
	//! @name Vertice
	//@{
			uint32 		GetNumVertices() const								{return N;}
			
			const TQ3Vertex3D* GetVertices() const							{return mVertices;}
						
			const TQ3Vertex3D& operator[](uint32 index) const				{ASSERT(index < N); return mVertices[index];}
	//@}

	//! @name Points
	//@{
			X3DPoint 	GetPoint(uint32 index) const						{ASSERT(index < N); return mVertices[index].point;}
						
			void 		SetPoint(uint32 index, const X3DPoint& pt)			{ASSERT(index < N); mVertices[index].point = pt;}
			void 		SetPoint(uint32 index, float x, float y, float z)	{ASSERT(index < N); mVertices[index].point.x = x; mVertices[index].point.y = y; mVertices[index].point.z = z;}
	//@}

	//! @name Attributes
	//@{
			X3DAttributeSet GetAttribute(uint32 index) const				{return this->DoGetAttribute(mVertices, N, index);}
						
			void 		SetAttribute(uint32 index, TQ3AttributeSet attr)	{this->DoSetAttribute(mVertices, N, index, attr);}
	//@}

//-----------------------------------
//	Member Data
//
private:
	TQ3Vertex3D		mVertices[N];
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
