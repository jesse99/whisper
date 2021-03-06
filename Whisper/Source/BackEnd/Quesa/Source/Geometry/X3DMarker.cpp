/*
 *  File:       X3DMarker.cpp
 *  Summary:	QuickDraw 3D one-bit marker.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DMarker.cpp,v $
 *		Revision 1.4  2001/04/21 03:36:21  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:29:33  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:17:08  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DMarker.h>

#include <X3DUtils.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateMarker (TQ3Object)
//
//---------------------------------------------------------------
static X3DObject* CreateMarker(TQ3Object inObject)
{
	X3DMarker* object = new X3DMarker(inObject);
	
	return object;
}


//---------------------------------------------------------------
//
// GetMarkerData
//
//---------------------------------------------------------------
static TQ3MarkerData GetMarkerData(const X3DPoint& location, const XPoint& offset, const SBitmap& bitmap, const X3DColorRGB& color)
{	
	TQ3MarkerData data;
	
	data.location            = location;
	data.xOffset             = offset.x;
	data.yOffset             = offset.y;
	data.bitmap              = bitmap;
	data.markerAttributeSet  = nil;
	
	if (color != kRGBBlack) {
		data.markerAttributeSet = Q3AttributeSet_New();
		ThrowIfNil(data.markerAttributeSet);
		
		TQ3Status status = Q3AttributeSet_Add(data.markerAttributeSet, kQ3AttributeTypeDiffuseColor, &color);
		ThrowIf3DError(status);
	}
		
	return data;
}


//---------------------------------------------------------------
//
// CreateMarker (X3DPoint, XPoint, SBitmap, X3DColorRGB)
//
//---------------------------------------------------------------
static TQ3GeometryObject CreateMarker(const X3DPoint& location, const XPoint& offset, const SBitmap& bitmap, const X3DColorRGB& color)
{
	TQ3MarkerData data = GetMarkerData(location, offset, bitmap, color);
	
	TQ3GeometryObject object = Q3Marker_New(&data);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DMarker
// ===================================================================================

//---------------------------------------------------------------
//
// X3DMarker::~X3DMarker
//
//---------------------------------------------------------------
X3DMarker::~X3DMarker()
{
}


//---------------------------------------------------------------
//
// X3DMarker::X3DMarker (X3DPoint, XPoint, SBitmap, X3DColorRGB)
//
//---------------------------------------------------------------
X3DMarker::X3DMarker(const X3DPoint& location, const XPoint& offset, const SBitmap& bitmap, const X3DColorRGB& color) : X3DGeometry(CreateMarker(location, offset, bitmap, color))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMarker::X3DMarker (TQ3MarkerData)
//
//---------------------------------------------------------------
X3DMarker::X3DMarker(const TQ3MarkerData& data) : X3DGeometry(Q3Marker_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMarker::X3DMarker (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DMarker::X3DMarker(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypeMarker);
}


//---------------------------------------------------------------
//
// X3DMarker::Clone
//
//---------------------------------------------------------------
X3DMarker* X3DMarker::Clone() const
{
	X3DMarker* object = new X3DMarker(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DMarker::GetPosition
//
//---------------------------------------------------------------
X3DPoint X3DMarker::GetPosition() const
{
	X3DPoint position;
	
	TQ3Status status = Q3Marker_GetPosition(mObject, position);
	ThrowIf3DError(status);
	
	return position;
}


//---------------------------------------------------------------
//
// X3DMarker::SetPosition
//
//---------------------------------------------------------------
void X3DMarker::SetPosition(const X3DPoint& position)
{
	TQ3Status status = Q3Marker_SetPosition(mObject, position);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMarker::GetXOffset
//
//---------------------------------------------------------------
long X3DMarker::GetXOffset() const
{
	long offset;
	
	TQ3Status status = Q3Marker_GetXOffset(mObject, &offset);
	ThrowIf3DError(status);
	
	return offset;
}


//---------------------------------------------------------------
//
// X3DMarker::SetXOffset
//
//---------------------------------------------------------------
void X3DMarker::SetXOffset(long offset)
{
	TQ3Status status = Q3Marker_SetXOffset(mObject, offset);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMarker::GetYOffset
//
//---------------------------------------------------------------
long X3DMarker::GetYOffset() const
{
	long offset;
	
	TQ3Status status = Q3Marker_GetYOffset(mObject, &offset);
	ThrowIf3DError(status);
	
	return offset;
}


//---------------------------------------------------------------
//
// X3DMarker::SetYOffset
//
//---------------------------------------------------------------
void X3DMarker::SetYOffset(long offset)
{
	TQ3Status status = Q3Marker_SetYOffset(mObject, offset);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMarker::GetBitmap
//
//---------------------------------------------------------------
SBitmap X3DMarker::GetBitmap() const
{
	SBitmap bitmap;
	
	TQ3Status status = Q3Marker_GetBitmap(mObject, &bitmap);
	ThrowIf3DError(status);
	
	return bitmap;
}


//---------------------------------------------------------------
//
// X3DMarker::SetBitmap
//
//---------------------------------------------------------------
void X3DMarker::SetBitmap(const SBitmap& bitmap)
{
	TQ3Status status = Q3Marker_SetBitmap(mObject, &bitmap);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMarker::GetData
//
//---------------------------------------------------------------
TQ3MarkerData X3DMarker::GetData() const
{
	TQ3MarkerData data;
	
	TQ3Status status = Q3Marker_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DMarker::SetData
//
//---------------------------------------------------------------
void X3DMarker::SetData(const TQ3MarkerData& data)
{
	TQ3Status status = Q3Marker_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMarker::Submit (X3DPoint, XPoint, SBitmap, TQ3ViewObject, X3DColorRGB)	[static]
//
//---------------------------------------------------------------
void X3DMarker::Submit(const X3DPoint& location, const XPoint& offset, const SBitmap& bitmap, TQ3ViewObject view, const X3DColorRGB& color)
{
	PRECONDITION(view != nil);
	
	TQ3MarkerData data = GetMarkerData(location, offset, bitmap, color);

	TQ3Status status = Q3Marker_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMarker::Submit (TQ3MarkerData, TQ3ViewObject)		[static]
//
//---------------------------------------------------------------
void X3DMarker::Submit(const TQ3MarkerData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Marker_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMarker::IsCompatible 								[static]
//	
//---------------------------------------------------------------
bool X3DMarker::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypeMarker);
	
	return compatible;
}


}	// namespace Whisper

