/*
 *  File:       X3DPick.cpp
 *  Summary:	Base class for QuickDraw 3D pick objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DPick.cpp,v $
 *		Revision 1.4  2001/04/21 03:36:54  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:30:18  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:18:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DPick.h>

#include <QuesaGroup.h>

#include <X3DUtils.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XTypes.h>

namespace Whisper {


// ===================================================================================
//	class X3DHitPath
// ===================================================================================

//---------------------------------------------------------------
//
// X3DHitPath::~X3DHitPath
//
//---------------------------------------------------------------
X3DHitPath::~X3DHitPath()
{
}


//---------------------------------------------------------------
//
// X3DHitPath::X3DHitPath
//
//---------------------------------------------------------------
X3DHitPath::X3DHitPath(TQ3HitPath path) : mRootGroup(path.rootGroup)
{
	PRECONDITION(path.rootGroup != nil);
	PRECONDITION(path.positions != nil);
	
	mGroups.push_back(mRootGroup);

	if (path.depth > 1) {
		TQ3GroupObject group = path.rootGroup;
		
		for (long index = 0; index < path.depth - 1; index++) {
			TQ3GroupObject subGroup;
			TQ3Status status = Q3Group_GetPositionObject(group, path.positions[index], &subGroup);

			mGroups.push_back(subGroup);

			group = subGroup;
		}

		mLeafGroup = mGroups.back();

	} else
		mLeafGroup = mRootGroup;
}


//---------------------------------------------------------------
//
// X3DHitPath::GetGroup
//
//---------------------------------------------------------------
X3DGroup X3DHitPath::GetGroup(uint32 index) const
{
	PRECONDITION(index < 16*1024L*1024L);
	PRECONDITION(index < mGroups.size());
	
	XQueue<X3DGroup>::const_iterator iter = mGroups.begin();
		
	return iter[index];
}

#pragma mark -

// ===================================================================================
//	class X3DPickEntry
// ===================================================================================

//---------------------------------------------------------------
//
// X3DPickEntry::~X3DPickEntry
//
//---------------------------------------------------------------
X3DPickEntry::~X3DPickEntry()
{
}


//---------------------------------------------------------------
//
// X3DPickEntry::X3DPickEntry
//
//---------------------------------------------------------------
X3DPickEntry::X3DPickEntry(TQ3PickObject object, uint32 index)
{
	PRECONDITION(object != nil);
	PRECONDITION(index < 16*1024L*1024L);
	
	mObject = object;
	mIndex  = index;
	
	TQ3Status status = Q3Pick_GetPickDetailValidMask(mObject, mIndex, &mMask);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPickEntry::GetPickID
//
//---------------------------------------------------------------
uint32 X3DPickEntry::GetPickID() const
{
	uint32 id;
	
	TQ3Status status = Q3Pick_GetPickDetailData(mObject, mIndex, kQ3PickDetailMaskPickID, &id);
	ThrowIf3DError(status);
	
	return id;
}


//---------------------------------------------------------------
//
// X3DPickEntry::GetHitPath
//
//---------------------------------------------------------------
X3DHitPath X3DPickEntry::GetHitPath() const
{
	TQ3HitPath path;
	
	TQ3Status status = Q3Pick_GetPickDetailData(mObject, mIndex, kQ3PickDetailMaskPath, &path);
	ThrowIf3DError(status);
	
	X3DHitPath hitPath(path);
	
	status = Q3HitPath_EmptyData(&path);
	ASSERT(status == kQ3Success);	

	return hitPath;
}


//---------------------------------------------------------------
//
// X3DPickEntry::GetObject
//
//---------------------------------------------------------------
X3DSharedObject X3DPickEntry::GetObject() const
{
	TQ3Object object;
	
	TQ3Status status = Q3Pick_GetPickDetailData(mObject, mIndex, kQ3PickDetailMaskObject, &object);
	ThrowIf3DError(status);
		
	X3DSharedObject obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DPickEntry::GetLocalToWorldMatrix
//
//---------------------------------------------------------------
X3DMatrix X3DPickEntry::GetLocalToWorldMatrix() const
{
	X3DMatrix matrix;
	
	TQ3Status status = Q3Pick_GetPickDetailData(mObject, mIndex, kQ3PickDetailMaskLocalToWorldMatrix, matrix);
	ThrowIf3DError(status);
	
	return matrix;
}


//---------------------------------------------------------------
//
// X3DPickEntry::GetXYZPoint
//
//---------------------------------------------------------------
X3DPoint X3DPickEntry::GetXYZPoint() const
{
	X3DPoint pt;
	
	TQ3Status status = Q3Pick_GetPickDetailData(mObject, mIndex, kQ3PickDetailMaskXYZ, pt);
	ThrowIf3DError(status);
	
	return pt;
}


//---------------------------------------------------------------
//
// X3DPickEntry::GetDistance
//
//---------------------------------------------------------------
float X3DPickEntry::GetDistance() const
{
	float distance;
	
	TQ3Status status = Q3Pick_GetPickDetailData(mObject, mIndex, kQ3PickDetailMaskDistance, &distance);
	ThrowIf3DError(status);
	
	return distance;
}


//---------------------------------------------------------------
//
// X3DPickEntry::GetNormal
//
//---------------------------------------------------------------
X3DVector X3DPickEntry::GetNormal() const
{
	X3DVector normal;
	
	TQ3Status status = Q3Pick_GetPickDetailData(mObject, mIndex, kQ3PickDetailMaskNormal, normal);
	ThrowIf3DError(status);
	
	return normal;
}


//---------------------------------------------------------------
//
// X3DPickEntry::GetShapePart
//
//---------------------------------------------------------------
X3DShape X3DPickEntry::GetShapePart() const
{
	TQ3Object object;
	
	TQ3Status status = Q3Pick_GetPickDetailData(mObject, mIndex, kQ3PickDetailMaskShapePart, &object);
	ThrowIf3DError(status);
	
	X3DShape obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DPickEntry::GetPickParts
//
//---------------------------------------------------------------
TQ3PickParts X3DPickEntry::GetPickParts() const
{
	TQ3PickParts parts;
	
	TQ3Status status = Q3Pick_GetPickDetailData(mObject, mIndex, kQ3PickDetailMaskPickPart, &parts);
	ThrowIf3DError(status);
	
	return parts;
}


//---------------------------------------------------------------
//
// X3DPickEntry::GetUV
//
//---------------------------------------------------------------
TQ3Param2D X3DPickEntry::GetUV() const
{
	TQ3Param2D uv;
	
	TQ3Status status = Q3Pick_GetPickDetailData(mObject, mIndex, kQ3PickDetailMaskUV, &uv);
	ThrowIf3DError(status);
	
	return uv;
}

#pragma mark -

// ===================================================================================
//	class X3DPick
// ===================================================================================

//---------------------------------------------------------------
//
// X3DPick::~X3DPick
//
//---------------------------------------------------------------
X3DPick::~X3DPick()
{
}


//---------------------------------------------------------------
//
// X3DPick::X3DPick
//
//---------------------------------------------------------------
X3DPick::X3DPick(TQ3PickObject object) : X3DObject(object)
{
	this->ConfirmType(kQ3ObjectTypePick);
}


//---------------------------------------------------------------
//
// X3DPick::Clone
//
//---------------------------------------------------------------
X3DPick* X3DPick::Clone() const
{
	TQ3Object newObject = Q3Object_Duplicate(mObject);
	ThrowIfNil(newObject);
	
	X3DPick* object = new X3DPick(newObject);
	
	return object;
}


//---------------------------------------------------------------
//
// X3DPick::GetVertexTolerance
//
//---------------------------------------------------------------
float X3DPick::GetVertexTolerance() const
{
	float tolerance;
	
	TQ3Status status = Q3Pick_GetVertexTolerance(mObject, &tolerance);
	ThrowIf3DError(status);
	
	return tolerance;
}


//---------------------------------------------------------------
//
// X3DPick::SetVertexTolerance
//
//---------------------------------------------------------------
void X3DPick::SetVertexTolerance(float tolerance)
{
	PRECONDITION(tolerance >= 0.0f);
	
	TQ3Status status = Q3Pick_SetVertexTolerance(mObject, tolerance);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPick::GetEdgeTolerance
//
//---------------------------------------------------------------
float X3DPick::GetEdgeTolerance() const
{
	float tolerance;
	
	TQ3Status status = Q3Pick_GetEdgeTolerance(mObject, &tolerance);
	ThrowIf3DError(status);
	
	return tolerance;
}


//---------------------------------------------------------------
//
// X3DPick::SetEdgeTolerance
//
//---------------------------------------------------------------
void X3DPick::SetEdgeTolerance(float tolerance)
{
	PRECONDITION(tolerance >= 0.0f);
	
	TQ3Status status = Q3Pick_SetEdgeTolerance(mObject, tolerance);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPick::GetNumHits
//
//---------------------------------------------------------------
uint32 X3DPick::GetNumHits() const
{
	uint32 numHits;
	
	TQ3Status status = Q3Pick_GetNumHits(mObject, &numHits);
	ThrowIf3DError(status);
	
	return numHits;
}


//---------------------------------------------------------------
//
// X3DPick::EmptyHitList
//
//---------------------------------------------------------------
void X3DPick::EmptyHitList()
{
	TQ3Status status = Q3Pick_EmptyHitList(mObject);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPick::GetPickDetailValidMask
//
//---------------------------------------------------------------
TQ3PickDetail X3DPick::GetPickDetailValidMask(uint32 index) const
{
	PRECONDITION(index < 16*1024L*1024L);
	
	TQ3PickDetail mask;
	
	TQ3Status status = Q3Pick_GetPickDetailValidMask(mObject, index, &mask);
	ThrowIf3DError(status);
	
	return mask;
}


//---------------------------------------------------------------
//
// X3DPick::GetPickDetailData
//
//---------------------------------------------------------------
void X3DPick::GetPickDetailData(uint32 index, TQ3PickDetail detail, void* data) const
{
	PRECONDITION(index < 16*1024L*1024L);
	PRECONDITION(data != nil);
	
	TQ3Status status = Q3Pick_GetPickDetailData(mObject, index, detail, data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPick::GetEntry
//
//---------------------------------------------------------------
X3DPickEntry X3DPick::GetEntry(uint32 index) const
{
	X3DPickEntry entry(mObject, index);
	
	return entry;
}


//---------------------------------------------------------------
//
// X3DPick::GetData
//
//---------------------------------------------------------------
TQ3PickData X3DPick::GetData() const
{
	TQ3PickData data;
	
	TQ3Status status = Q3Pick_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DPick::SetData (TQ3PickSort, TQ3PickDetail, uint32)
//
//---------------------------------------------------------------
void X3DPick::SetData(TQ3PickSort sort, TQ3PickDetail mask, uint32 numHitsToReturn)
{
	PRECONDITION(numHitsToReturn < 16*1024L*1024L);
	
	TQ3PickData data;
	data.sort = sort;
	data.mask = mask;
	data.numHitsToReturn = numHitsToReturn;
	
	TQ3Status status = Q3Pick_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPick::SetData (TQ3PickData)
//
//---------------------------------------------------------------
void X3DPick::SetData(const TQ3PickData& data)
{
	TQ3Status status = Q3Pick_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPick::IsCompatible 								[static]
//	
//---------------------------------------------------------------
bool X3DPick::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3ObjectTypePick);
	
	return compatible;
}


}	// namespace Whisper
