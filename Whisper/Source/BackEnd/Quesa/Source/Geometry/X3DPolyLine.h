/*
 *  File:       X3DPolyLine.h
 *  Summary:	Encapsulates a 3D polygonal line.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DPolyLine.h,v $
 *		Revision 1.3  2000/12/13 08:53:19  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:19:09  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DPolyLine.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:53:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <2>	  4/16/00	JDJ		Added X3DPolyLineData.
 *		 <1>	  3/06/97	JDJ		Created.
 */

#pragma once

#include <X3DGeometry.h>
#include <X3DPrimitives.h>
#include <X3DVertexList.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DPolyLineData
//!		Used by X3DPolyLine.
// ===================================================================================
template <int N>
class X3DPolyLineData : public X3DVertexList<N> {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~X3DPolyLineData()										{this->DoDestroyAttributes(mSegmentAttributes, N-1);}
						
						X3DPolyLineData()										{std::memset(mSegmentAttributes, 0, sizeof(mSegmentAttributes)); mHasSegmentAttributes = false;}

//-----------------------------------
//	New API
//
public:
	// ----- Segment Attributes -----
			X3DAttributeSet GetSegmentAttribute(uint32 index) const				{return this->DoGetAttribute(mSegmentAttributes, N-1, index); mHasSegmentAttributes = true;}
						
			void 		SetSegmentAttribute(uint32 index, TQ3AttributeSet attr)	{this->DoSetAttribute(mSegmentAttributes, N-1, index, attr); if (attr != nil) mHasSegmentAttributes = true;}

			TQ3AttributeSet* GetSegmentAttributes() 							{return mHasSegmentAttributes ? mSegmentAttributes : nil;}

//-----------------------------------
//	Member Data
//
private:
	TQ3AttributeSet	mSegmentAttributes[N-1];
	bool			mHasSegmentAttributes;
};


// ===================================================================================
//	class X3DPolyLine
//!		A shape consisting of a list of linked line segments.
// ===================================================================================
class QUESA_EXPORT X3DPolyLine : public X3DGeometry {

	typedef X3DGeometry Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DPolyLine();
						
						X3DPolyLine(const TQ3PolyLineData& data);

						X3DPolyLine(uint32 count, const TQ3Vertex3D* vertices, const TQ3AttributeSet* segmentAttributes = nil, TQ3AttributeSet polyLineAttributes = nil);

						X3DPolyLine(TQ3GeometryObject object);
						
	virtual X3DPolyLine* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Vertex Position
	//@{
			X3DPoint 	GetVertexPosition(uint32 index) const;
						
			void 		SetVertexPosition(uint32 index, const X3DPoint& vertex);
	//@}

	//! @name Vertex Attributes
	//@{
			TQ3AttributeSet GetVertexAttributeSet(uint32 index) const;
						
			void 		SetVertexAttributeSet(uint32 index, TQ3AttributeSet attributes);
	//@}

	//! @name Segment Attributes
	//@{
			TQ3AttributeSet GetSegmentAttributeSet(uint32 index) const;
						
			void 		SetSegmentAttributeSet(uint32 index, TQ3AttributeSet attributes);
	//@}

	//! @name Data
	//@{
			TQ3PolyLineData GetData() const;
						
			void 		SetData(const TQ3PolyLineData& data);
	//@}

	//! @name Submitting (retained mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}
									
	//! @name Submitting (immediate mode)
	//@{
	static	void 		Submit(const TQ3PolyLineData& data, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DPolyLine. */
	//@}
};



#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
