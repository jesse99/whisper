/*
 *  File:       X3DPick.h
 *  Summary:	Base class for QuickDraw 3D pick objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DPick.h,v $
 *		Revision 1.4  2001/04/14 07:30:23  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.3  2000/12/13 08:52:56  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:18:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Quesa.h>
#include <QuesaPick.h>

#include <X3DGroup.h>
#include <X3DMatrix.h>
#include <X3DObject.h>
#include <X3DPrimitives.h>
#include <X3DShape.h>
#include <XQueue.h>
#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DHitPath
//!		Path through a model hierarchy to a specific picked object.
// ===================================================================================
class QUESA_EXPORT X3DHitPath {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~X3DHitPath();
						
						X3DHitPath(TQ3HitPath path);
						
//-----------------------------------
//	API
//
public:
			X3DGroup 	GetRootGroup() const			{return mRootGroup;}
			
			X3DGroup 	GetLeafGroup() const			{return mLeafGroup;}
						
			uint32 		GetNumGroups() const			{return mGroups.size();}

			X3DGroup 	GetGroup(uint32 index) const;
						/**< First group is the root group. Last group is the leaf's group. */

//-----------------------------------
//	Member Data
//
protected:
	X3DGroup			mRootGroup;
	X3DGroup			mLeafGroup;
	XQueue<X3DGroup>	mGroups;
};


// ===================================================================================
//	class X3DPickEntry
//!		Wrapper for the object picked by X3DPick.
// ===================================================================================
class QUESA_EXPORT X3DPickEntry {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~X3DPickEntry();
						
						X3DPickEntry(TQ3PickObject object, uint32 index);
						
//-----------------------------------
//	API
//
public:
	//! @name Detail Mask
	//@{
			bool 		HasDetail(TQ3PickDetail detail) const		{return (mMask & detail) == detail;}
	//@}
			
	//! @name Details
	//@{
			uint32 		GetPickID() const;
			
			X3DHitPath 	GetHitPath() const;
						/**< Returns the groups containing the object. */
			
			X3DSharedObject GetObject() const;
			
			X3DMatrix 	GetLocalToWorldMatrix() const;
			
			X3DPoint 	GetXYZPoint() const;
			
			float 		GetDistance() const;
			
			X3DVector 	GetNormal() const;
			
			X3DShape 	GetShapePart() const;
			
			TQ3PickParts GetPickParts() const;
			
			TQ3Param2D 	GetUV() const;
	//@}

//-----------------------------------
//	Member Data
//
protected:
	TQ3PickObject	mObject;
	uint32			mIndex;
	TQ3PickDetail	mMask;
};


// ===================================================================================
//	class X3DPick
//!		Used to hit test objects in the scene.
// ===================================================================================
class QUESA_EXPORT X3DPick : public X3DObject {

	typedef X3DObject Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DPick();
						
						X3DPick(TQ3PickObject object);
						
	virtual X3DPick* 	Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3PickObject() const			{return mObject;}
	//@}

	//! @name Vertex Tolerance
	//@{
			float 		GetVertexTolerance() const;
	
			void 		SetVertexTolerance(float toler);
	//@}

	//! @name Edge Tolerance
	//@{
			float 		GetEdgeTolerance() const;
	
			void 		SetEdgeTolerance(float toler);
	//@}

	//! @name Number of Hits
	//@{
			uint32 		GetNumHits() const;
			
			void 		EmptyHitList();
	//@}

	//! @name Pick Details
	//@{
			bool 		HasDetail(uint32 index, TQ3PickDetail detail) const	{return (this->GetPickDetailValidMask(index) & detail) == detail;}

			TQ3PickDetail GetPickDetailValidMask(uint32 index) const;
						/**< Mask can be kQ3PickDetailNone, kQ3PickDetailMaskPickID, kQ3PickDetailMaskPath, 
						kQ3PickDetailMaskObject, kQ3PickDetailMaskLocalToWorldMatrix, kQ3PickDetailMaskXYZ, 
						kQ3PickDetailMaskDistance, kQ3PickDetailMaskNormal, kQ3PickDetailMaskShapePart, 
						kQ3PickDetailMaskPickPart, or kQ3PickDetailMaskUV. */
			
			void 		GetPickDetailData(uint32 index, TQ3PickDetail detail, void* data) const;
			
			X3DPickEntry GetEntry(uint32 index) const;
	//@}

	//! @name Data
	//@{
			TQ3PickData GetData() const;
			
			void 		SetData(TQ3PickSort sort, TQ3PickDetail mask, uint32 numHitsToReturn);
						/**< Sort can be kQ3PickSortNone, kQ3PickSortNearToFar, or kQ3PickSortFarToNear. */
						
			void 		SetData(const TQ3PickData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DPick. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

