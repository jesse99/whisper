/*
 *  File:       X3DPicks.h
 *  Summary:	QuickDraw 3D pick objects.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DWindowPointPick	- Works with points on a screen projection.
 *				X3DWindowRectPick	- Works with rectangles on a screen projection.
 *				X3DWorldPick		- Works with rays in world coordinates (useful for stuff
 *									  like collision detection).
 *
 *  Copyright � 1997-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DPicks.h,v $
 *		Revision 1.3  2000/12/13 08:53:01  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:18:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DPicks.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:53:01  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <2>	  4/19/00	JDJ		Added X3DWorldPick.
 *		 <1>	  2/18/97	JDJ		Created.
 */

#pragma once

#include <X2DPrimitives.h>
#include <X3DPick.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DWindowPointPick
//!		Used to hit test objects from a point in window coordinates.
// ===================================================================================
class QUESA_EXPORT X3DWindowPointPick : public X3DPick {

	typedef X3DPick Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DWindowPointPick();
						
						X3DWindowPointPick(TQ3PickDetail mask, uint32 numHits = 1, float tolerance = 2.0f, 
										   TQ3PickSort sort = kQ3PickSortNearToFar);
						/**< Mask can be kQ3PickDetailNone, kQ3PickDetailMaskPickID, kQ3PickDetailMaskPath, 
						kQ3PickDetailMaskObject, kQ3PickDetailMaskLocalToWorldMatrix, kQ3PickDetailMaskXYZ, 
						kQ3PickDetailMaskDistance, kQ3PickDetailMaskNormal, kQ3PickDetailMaskShapePart, 
						kQ3PickDetailMaskPickPart, or kQ3PickDetailMaskUV.
						Sort can be kQ3PickSortNone, kQ3PickSortNearToFar, or kQ3PickSortFarToNear. */

						X3DWindowPointPick(const TQ3WindowPointPickData& data);

						X3DWindowPointPick(TQ3PickObject object);
						
	virtual X3DWindowPointPick*	Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Point
	//@{
			X2DPoint 	GetPoint() const;
	
			void 		SetPoint(const X2DPoint& pt);
	//@}

	//! @name Data
	//@{
			TQ3WindowPointPickData GetData() const;
			
			void 		SetData(const TQ3WindowPointPickData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DWindowPointPick. */
	//@}
};


// ===================================================================================
//	class X3DWindowRectPick
//!		Used to hit test objects from a rectangle in window coordinates.
// ===================================================================================
class QUESA_EXPORT X3DWindowRectPick : public X3DPick {

	typedef X3DPick Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DWindowRectPick();
						
						X3DWindowRectPick(TQ3PickDetail mask, uint32 numHits = 1, TQ3PickSort sort = kQ3PickSortNearToFar);
						/**< Mask can be kQ3PickDetailNone, kQ3PickDetailMaskPickID, kQ3PickDetailMaskPath, 
						kQ3PickDetailMaskObject, kQ3PickDetailMaskLocalToWorldMatrix, kQ3PickDetailMaskXYZ, 
						kQ3PickDetailMaskDistance, kQ3PickDetailMaskNormal, kQ3PickDetailMaskShapePart, 
						kQ3PickDetailMaskPickPart, or kQ3PickDetailMaskUV.
						Sort can be kQ3PickSortNone, kQ3PickSortNearToFar, or kQ3PickSortFarToNear. */

						X3DWindowRectPick(const TQ3WindowRectPickData& data);

						X3DWindowRectPick(TQ3PickObject object);
						
	virtual X3DWindowRectPick* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Rect
	//@{
			X2DRect 	GetRect() const;
	
			void 		SetRect(const X2DRect& rect);
	//@}

	//! @name Data
	//@{
			TQ3WindowRectPickData GetData() const;
			
			void 		SetData(const TQ3WindowRectPickData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DWindowRectPick. */
	//@}
};


// ===================================================================================
//	class X3DWorldPick
//!		Used to hit test objects from a point in world coordinates (useful for stuff like collision detection).
// ===================================================================================
class QUESA_EXPORT X3DWorldPick : public X3DPick {

	typedef X3DPick Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DWorldPick();
						
						X3DWorldPick(const TQ3WorldRayPickData& data);
						/**< Requires QD3D 1.6 */

						X3DWorldPick(TQ3PickObject object);
						
	virtual X3DWorldPick* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Ray
	//@{
			TQ3Ray3D 	GetRay() const;
	
			void 		SetRay(const TQ3Ray3D& ray);
	//@}

	//! @name Data
	//@{
			TQ3WorldRayPickData GetData() const;
			
			void 		SetData(const TQ3WorldRayPickData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DWorldPick. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
