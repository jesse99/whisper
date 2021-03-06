/*
 *  File:       X3DPicks.cpp
 *  Summary:	QuickDraw 3D pick objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DPicks.cpp,v $
 *		Revision 1.4  2001/04/21 03:36:59  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:30:30  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:18:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DPicks.h>

#include <X3DUtils.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateWindowPointPick (TQ3PickDetail, uint32, float, TQ3PickSort)
//
//---------------------------------------------------------------
static TQ3PickObject CreateWindowPointPick(TQ3PickDetail mask, uint32 numHits, float tolerance, TQ3PickSort sort)
{
	PRECONDITION(numHits < 16*1024L*1024L);
	
	TQ3WindowPointPickData data;
	
	data.data.sort = sort;
	data.data.mask = mask;
	data.data.numHitsToReturn = numHits;
	
	data.point = kZero2DPt;
	data.vertexTolerance = tolerance;
	data.edgeTolerance = tolerance;
	
	return Q3WindowPointPick_New(&data);
}


//---------------------------------------------------------------
//
// CreateWindowPointPick (TQ3Object)
//
//---------------------------------------------------------------
static X3DObject* CreateWindowPointPick(TQ3Object inObject)
{
	X3DWindowPointPick* object = new X3DWindowPointPick(inObject);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateWindowRectPick (TQ3PickDetail, uint32, TQ3PickSort)
//
//---------------------------------------------------------------
static TQ3PickObject CreateWindowRectPick(TQ3PickDetail mask, uint32 numHits, TQ3PickSort sort)
{
	PRECONDITION(numHits < 16*1024L*1024L);
	
	TQ3WindowRectPickData data;
	
	data.data.sort = sort;
	data.data.mask = mask;
	data.data.numHitsToReturn = numHits;
	
	data.rect.min = kZero2DPt;
	data.rect.max = kZero2DPt;
	
	return Q3WindowRectPick_New(&data);
}


//---------------------------------------------------------------
//
// CreateWindowRectPick (TQ3Object)
//
//---------------------------------------------------------------
static X3DObject* CreateWindowRectPick(TQ3Object inObject)
{
	X3DWindowRectPick* object = new X3DWindowRectPick(inObject);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DWindowPointPick
// ===================================================================================

//---------------------------------------------------------------
//
// X3DWindowPointPick::~X3DWindowPointPick
//
//---------------------------------------------------------------
X3DWindowPointPick::~X3DWindowPointPick()
{
}


//---------------------------------------------------------------
//
// X3DWindowPointPick::X3DWindowPointPick (TQ3PickDetail, uint32, float, TQ3PickSort)
//
//---------------------------------------------------------------
X3DWindowPointPick::X3DWindowPointPick(TQ3PickDetail mask, uint32 numHits, float tolerance, TQ3PickSort sort) : X3DPick(CreateWindowPointPick(mask, numHits, tolerance, sort))
{
}


//---------------------------------------------------------------
//
// X3DWindowPointPick::X3DWindowPointPick (TQ3PickObject)
//
//---------------------------------------------------------------
X3DWindowPointPick::X3DWindowPointPick(TQ3PickObject object) : X3DPick(object)
{
	this->ConfirmType(kQ3PickTypeWindowPoint);
}


//---------------------------------------------------------------
//
// X3DWindowPointPick::Clone
//
//---------------------------------------------------------------
X3DWindowPointPick* X3DWindowPointPick::Clone() const
{
	TQ3Object newObject = Q3Object_Duplicate(mObject);
	ThrowIfNil(newObject);
	
	X3DWindowPointPick* object = new X3DWindowPointPick(newObject);
	
	return object;
}


//---------------------------------------------------------------
//
// X3DWindowPointPick::GetPoint
//
//---------------------------------------------------------------
X2DPoint X3DWindowPointPick::GetPoint() const
{
	X2DPoint pt;
	
	TQ3Status status = Q3WindowPointPick_GetPoint(mObject, pt);
	ThrowIf3DError(status);
	
	return pt;
}


//---------------------------------------------------------------
//
// X3DWindowPointPick::SetPoint
//
//---------------------------------------------------------------
void X3DWindowPointPick::SetPoint(const X2DPoint& pt)
{
	TQ3Status status = Q3WindowPointPick_SetPoint(mObject, pt);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DWindowPointPick::GetData
//
//---------------------------------------------------------------
TQ3WindowPointPickData X3DWindowPointPick::GetData() const
{
	TQ3WindowPointPickData data;
	
	TQ3Status status = Q3WindowPointPick_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DWindowPointPick::SetData
//
//---------------------------------------------------------------
void X3DWindowPointPick::SetData(const TQ3WindowPointPickData& data)
{
	TQ3Status status = Q3WindowPointPick_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DWindowPointPick::IsCompatible 					[static]
//	
//---------------------------------------------------------------
bool X3DWindowPointPick::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3PickTypeWindowPoint);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DWindowRectPick
// ===================================================================================

//---------------------------------------------------------------
//
// X3DWindowRectPick::~X3DWindowRectPick
//
//---------------------------------------------------------------
X3DWindowRectPick::~X3DWindowRectPick()
{
}


//---------------------------------------------------------------
//
// X3DWindowRectPick::X3DWindowRectPick (TQ3PickDetail, uint32, TQ3PickSort)
//
//---------------------------------------------------------------
X3DWindowRectPick::X3DWindowRectPick(TQ3PickDetail mask, uint32 numHits, TQ3PickSort sort) : X3DPick(CreateWindowRectPick(mask, numHits, sort))
{
}


//---------------------------------------------------------------
//
// X3DWindowRectPick::X3DWindowRectPick (TQ3WindowRectPickData)
//
//---------------------------------------------------------------
X3DWindowRectPick::X3DWindowRectPick(const TQ3WindowRectPickData& data) : X3DPick(Q3WindowRectPick_New(&data))
{
}


//---------------------------------------------------------------
//
// X3DWindowRectPick::X3DWindowRectPick (TQ3PickObject)
//
//---------------------------------------------------------------
X3DWindowRectPick::X3DWindowRectPick(TQ3PickObject object) : X3DPick(object)
{
	this->ConfirmType(kQ3PickTypeWindowRect);
}


//---------------------------------------------------------------
//
// X3DWindowRectPick::Clone
//
//---------------------------------------------------------------
X3DWindowRectPick* X3DWindowRectPick::Clone() const
{
	TQ3Object newObject = Q3Object_Duplicate(mObject);
	ThrowIfNil(newObject);
	
	X3DWindowRectPick* object = new X3DWindowRectPick(newObject);
	
	return object;
}


//---------------------------------------------------------------
//
// X3DWindowRectPick::GetRect
//
//---------------------------------------------------------------
X2DRect X3DWindowRectPick::GetRect() const
{
	TQ3Area area;
	
	TQ3Status status = Q3WindowRectPick_GetRect(mObject, &area);
	ThrowIf3DError(status);
	
	return X2DRect(area.min, area.max);
}


//---------------------------------------------------------------
//
// X3DWindowRectPick::SetRect
//
//---------------------------------------------------------------
void X3DWindowRectPick::SetRect(const X2DRect& rect)
{
	TQ3Area area;
	area.min = rect[topLeft];
	area.max = rect[botRight];
	
	TQ3Status status = Q3WindowRectPick_SetRect(mObject, &area);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DWindowRectPick::GetData
//
//---------------------------------------------------------------
TQ3WindowRectPickData X3DWindowRectPick::GetData() const
{
	TQ3WindowRectPickData data;
	
	TQ3Status status = Q3WindowRectPick_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DWindowRectPick::SetData
//
//---------------------------------------------------------------
void X3DWindowRectPick::SetData(const TQ3WindowRectPickData& data)
{
	TQ3Status status = Q3WindowRectPick_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DWindowRectPick::IsCompatible 						[static]
//	
//---------------------------------------------------------------
bool X3DWindowRectPick::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3PickTypeWindowRect);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DWorldPick
// ===================================================================================

//---------------------------------------------------------------
//
// X3DWorldPick::~X3DWorldPick
//
//---------------------------------------------------------------
X3DWorldPick::~X3DWorldPick()
{
}


//---------------------------------------------------------------
//
// X3DWorldPick::X3DWorldPick (TQ3WorldRayPickData)
//
//---------------------------------------------------------------
X3DWorldPick::X3DWorldPick(const TQ3WorldRayPickData& data) : X3DPick(Q3WorldRayPick_New(&data))
{
}


//---------------------------------------------------------------
//
// X3DWorldPick::X3DWorldPick (TQ3PickObject)
//
//---------------------------------------------------------------
X3DWorldPick::X3DWorldPick(TQ3PickObject object) : X3DPick(object)
{
	this->ConfirmType(kQ3PickTypeWorldRay);
}


//---------------------------------------------------------------
//
// X3DWorldPick::Clone
//
//---------------------------------------------------------------
X3DWorldPick* X3DWorldPick::Clone() const
{
	TQ3Object newObject = Q3Object_Duplicate(mObject);
	ThrowIfNil(newObject);
	
	X3DWorldPick* object = new X3DWorldPick(newObject);
	
	return object;
}


//---------------------------------------------------------------
//
// X3DWorldPick::GetRay
//
//---------------------------------------------------------------
TQ3Ray3D X3DWorldPick::GetRay() const
{
	TQ3Ray3D ray;
	
	TQ3Status status = Q3WorldRayPick_GetRay(mObject, &ray);
	ThrowIf3DError(status);
	
	return ray;
}


//---------------------------------------------------------------
//
// X3DWorldPick::SetRay
//
//---------------------------------------------------------------
void X3DWorldPick::SetRay(const TQ3Ray3D& ray)
{
	TQ3Status status = Q3WorldRayPick_SetRay(mObject, &ray);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DWorldPick::GetData
//
//---------------------------------------------------------------
TQ3WorldRayPickData X3DWorldPick::GetData() const
{
	TQ3WorldRayPickData data;
	
	TQ3Status status = Q3WorldRayPick_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DWorldPick::SetData
//
//---------------------------------------------------------------
void X3DWorldPick::SetData(const TQ3WorldRayPickData& data)
{
	TQ3Status status = Q3WorldRayPick_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DWorldPick::IsCompatible 						[static]
//	
//---------------------------------------------------------------
bool X3DWorldPick::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3PickTypeWorldRay);
	
	return compatible;
}


}	// namespace Whisper

