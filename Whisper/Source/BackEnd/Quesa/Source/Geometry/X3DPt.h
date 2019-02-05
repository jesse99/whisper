/*
 *  File:       X3DPt.h
 *  Summary:	Encapsulates a 3D point geometry.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DPt.h,v $
 *		Revision 1.3  2000/12/13 08:53:31  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:19:30  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DPt.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:53:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <->	  3/06/97	JDJ		Created.
 */

#pragma once

#include <X3DGeometry.h>
#include <X3DPrimitives.h>
#include <X3DVectors.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class X3DColorRGB;


// ===================================================================================
//	class X3DPt
//!		A 3D point shape.
// ===================================================================================
class QUESA_EXPORT X3DPt : public X3DGeometry {

	typedef X3DGeometry Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DPt();
						
						X3DPt(const X3DPoint& pt, const X3DColorRGB& color);

						X3DPt(const X3DPoint& pt, TQ3AttributeSet attributes = nil);

						X3DPt(const TQ3PointData& data);

						X3DPt(TQ3GeometryObject object);
						
	virtual X3DPt* 		Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Position
	//@{
			X3DPoint 	GetPosition() const;
						
			void 		SetPosition(const X3DPoint& pt);
	//@}

	//! @name Data
	//@{
			TQ3PointData GetData() const;
						
			void 		SetData(const TQ3PointData& data);
	//@}

	//! @name Submitting (retained mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}
									
	//! @name Submitting (immediate mode)
	//@{
	static	void 		Submit(const X3DPoint& start, TQ3ViewObject view, const X3DColorRGB& color);

	static	void 		Submit(const X3DPoint& start, TQ3ViewObject view, TQ3AttributeSet attributes = nil);

	static	void 		Submit(const TQ3PointData& data, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DPt. */
	//@}
};



#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

