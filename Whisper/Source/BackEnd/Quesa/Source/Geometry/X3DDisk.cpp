/*
 *  File:       X3DDisk.cpp
 *  Summary:	A generalized 3D filled elliptical curve.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DDisk.cpp,v $
 *		Revision 1.4  2001/04/21 03:35:31  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:28:04  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:14:50  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DDisk.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetDiskData
//
//---------------------------------------------------------------
static TQ3DiskData GetDiskData(const X3DPoint& origin, const X3DVector& majorRadius, const X3DVector& minorRadius, float uMin, float uMax, float vMin, float vMax)
{	
	TQ3DiskData data;
	
	data.origin            = origin;
	data.majorRadius       = majorRadius;
	data.minorRadius       = minorRadius;
	data.uMin              = uMin;
	data.uMax              = uMax;
	data.vMin              = vMin;
	data.vMax              = vMax;
	data.diskAttributeSet  = nil;
		
	return data;
}


//---------------------------------------------------------------
//
// CreateDisk (X3DPoint, X3DPoint, float, float, float, float)
//
//---------------------------------------------------------------
static TQ3GeometryObject CreateDisk(const X3DPoint& origin, const X3DVector& majorRadius, const X3DVector& minorRadius, float uMin, float uMax, float vMin, float vMax)
{
	TQ3DiskData data = GetDiskData(origin, majorRadius, minorRadius, uMin, uMax, vMin, vMax);
	
	TQ3GeometryObject object = Q3Disk_New(&data);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DDisk
// ===================================================================================

//---------------------------------------------------------------
//
// X3DDisk::~X3DDisk
//
//---------------------------------------------------------------
X3DDisk::~X3DDisk()
{
}


//---------------------------------------------------------------
//
// X3DDisk::X3DDisk (X3DPoint, X3DPoint, float, float, float, float)
//
//---------------------------------------------------------------
X3DDisk::X3DDisk(const X3DPoint& origin, const X3DVector& majorRadius, const X3DVector& minorRadius, float uMin, float uMax, float vMin, float vMax) : X3DGeometry(CreateDisk(origin, majorRadius, minorRadius, uMin, uMax, vMin, vMax))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDisk::X3DDisk (TQ3DiskData)
//
//---------------------------------------------------------------
X3DDisk::X3DDisk(const TQ3DiskData& data) : X3DGeometry(Q3Disk_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDisk::X3DDisk (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DDisk::X3DDisk(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypeDisk);
}


//---------------------------------------------------------------
//
// X3DDisk::Clone
//
//---------------------------------------------------------------
X3DDisk* X3DDisk::Clone() const
{
	X3DDisk* object = new X3DDisk(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DDisk::GetOrigin
//
//---------------------------------------------------------------
X3DPoint X3DDisk::GetOrigin() const
{
	X3DPoint origin;
	
	TQ3Status status = Q3Disk_GetOrigin(mObject, origin);
	ThrowIf3DError(status);
	
	return origin;
}


//---------------------------------------------------------------
//
// X3DDisk::SetOrigin
//
//---------------------------------------------------------------
void X3DDisk::SetOrigin(const X3DPoint& origin)
{
	TQ3Status status = Q3Disk_SetOrigin(mObject, origin);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDisk::GetMajorRadius
//
//---------------------------------------------------------------
X3DVector X3DDisk::GetMajorRadius() const
{
	X3DVector radius;
	
	TQ3Status status = Q3Disk_GetMajorRadius(mObject, radius);
	ThrowIf3DError(status);
	
	return radius;
}


//---------------------------------------------------------------
//
// X3DDisk::SetMajorRadius
//
//---------------------------------------------------------------
void X3DDisk::SetMajorRadius(const X3DVector& radius)
{
	TQ3Status status = Q3Disk_SetMajorRadius(mObject, radius);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDisk::GetMinorRadius
//
//---------------------------------------------------------------
X3DVector X3DDisk::GetMinorRadius() const
{
	X3DVector radius;
	
	TQ3Status status = Q3Disk_GetMinorRadius(mObject, radius);
	ThrowIf3DError(status);
	
	return radius;
}


//---------------------------------------------------------------
//
// X3DDisk::SetMinorRadius
//
//---------------------------------------------------------------
void X3DDisk::SetMinorRadius(const X3DVector& radius)
{
	TQ3Status status = Q3Disk_SetMinorRadius(mObject, radius);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDisk::GetData
//
//---------------------------------------------------------------
TQ3DiskData X3DDisk::GetData() const
{
	TQ3DiskData data;
	
	TQ3Status status = Q3Disk_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DDisk::SetData
//
//---------------------------------------------------------------
void X3DDisk::SetData(const TQ3DiskData& data)
{
	TQ3Status status = Q3Disk_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDisk::Submit (X3DPoint, X3DPoint, TQ3ViewObject, float, float, float, float)	[static]
//
//---------------------------------------------------------------
void X3DDisk::Submit(const X3DPoint& origin, const X3DVector& majorRadius, const X3DVector& minorRadius, TQ3ViewObject view, float uMin, float uMax, float vMin, float vMax)
{
	PRECONDITION(view != nil);
	
	TQ3DiskData data = Whisper::GetDiskData(origin, majorRadius, minorRadius, uMin, uMax, vMin, vMax);

	TQ3Status status = Q3Disk_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDisk::Submit (TQ3DiskData, TQ3ViewObject)			[static]
//
//---------------------------------------------------------------
void X3DDisk::Submit(const TQ3DiskData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Disk_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDisk::IsCompatible 								[static]
//
//---------------------------------------------------------------
bool X3DDisk::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypeDisk);
	
	return compatible;
}


}	// namespace Whisper

