/*
 *  File:       X3DTriGrid.h
 *  Summary:	Efficient mesh with a fixed topology.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DTriGrid.h,v $
 *		Revision 1.4  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/13 08:55:25  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:23:07  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	  4/16/00	JDJ		Added X3DTriGridData.
 *		 <1>	  3/06/97	JDJ		Created.
 */

#pragma once

#include <X3DColors.h>
#include <X3DGeometry.h>
#include <X3DPrimitives.h>
#include <X3DVertexList.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DTriGridData
//!		Used by X3DTriGrid.
// ===================================================================================
template <int NUMCOLS, int NUMROWS>
class X3DTriGridData : public X3DVertexList<NUMCOLS*NUMROWS> {

//-----------------------------------
//	Constants
//
public:
	enum {kNumFacets = 2*(NUMCOLS - 1)*(NUMROWS - 1)};

//-----------------------------------
//	Initialization/Destruction
//
public:
						~X3DTriGridData()										{this->DoDestroyAttributes(mFacetAttributes, kNumFacets);}
						
						X3DTriGridData()										{std::memset(mFacetAttributes, 0, sizeof(mFacetAttributes)); mHasFacetAttributes = false;}

//-----------------------------------
//	New API
//
public:
	// ----- Facet Attributes -----
			X3DAttributeSet GetFacetAttribute(uint32 index) const				{return this->DoGetAttribute(mFacetAttributes, kNumFacets, index); mHasFacetAttributes = true;}
						
			void 		SetFacetAttribute(uint32 index, TQ3AttributeSet attr)	{this->DoSetAttribute(mFacetAttributes, kNumFacets, index, attr); if (attr != nil) mHasFacetAttributes = true;}

			TQ3AttributeSet* GetFacetAttributes() 								{return mHasFacetAttributes ? mFacetAttributes : nil;}

//-----------------------------------
//	Member Data
//
private:
	TQ3AttributeSet	mFacetAttributes[kNumFacets];
	bool			mHasFacetAttributes;
};


// ===================================================================================
//	class X3DTriGrid
//!		Efficient mesh with a fixed topology.
/*!		Because trigrids use attribute sets to contain things like colors they can 
 *		consume an awful lot of memory when many different colors are used. If you plan 
 *		on using lots of colors you may want to look at X3DUniformTriGrid. */
// ===================================================================================
class QUESA_EXPORT X3DTriGrid : public X3DGeometry {

	typedef X3DGeometry Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DTriGrid();
						
						X3DTriGrid(const XSize& size);

						X3DTriGrid(const TQ3TriGridData& data);

						X3DTriGrid(uint32 numCols, uint32 numRows, const TQ3Vertex3D* vertices, const TQ3AttributeSet* facetAttributes = nil, TQ3AttributeSet gridAttributes = nil);

						X3DTriGrid(TQ3GeometryObject object);
						
	virtual X3DTriGrid* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Number of Vertices
	//@{
			XSize 		GetSize() const;
						
			void 		SetSize(const XSize& size);
						/**< Wipes out old points. */
	//@}

	//! @name Vertex Position
	//@{
			X3DPoint 	GetVertexPosition(const XPoint& vertex) const;
						
			void 		SetVertexPosition(const XPoint& vertex, const X3DPoint& position);
	//@}

	//! @name Vertex Attributes
	//@{
			TQ3AttributeSet GetVertexAttributeSet(const XPoint& vertex) const;
						/**< Note that this can return nil. */
						
			void 		SetVertexAttributeSet(const XPoint& vertex, TQ3AttributeSet attributes);
	//@}

	//! @name Face Attributes
	//@{
			TQ3AttributeSet GetFaceAttributeSet(uint32 index) const;
						/**< Note that this can return nil. */
						
			void 		SetFaceAttributeSet(uint32 index, TQ3AttributeSet attributes);
	//@}

	//! @name Data
	//@{
			TQ3TriGridData GetData() const;
						
			void 		SetData(const TQ3TriGridData& data);
	//@}

	//! @name Submitting (retained mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}
									
	//! @name Submitting (immediate mode)
	//@{
	static	void 		Submit(const TQ3TriGridData& data, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DTriGrid. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
