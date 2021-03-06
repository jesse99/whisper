/*
 *  File:       X3DPixmapMarker.cpp
 *  Summary:	QuickDraw 3D multi-bit marker.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DPixmapMarker.cpp,v $
 *		Revision 1.4  2001/04/21 03:37:04  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:30:35  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:18:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DPixmapMarker.h>

#include <QuesaStorage.h>

#include <X3DUtils.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetPixmapMarkerData
//
//---------------------------------------------------------------
static TQ3PixmapMarkerData GetPixmapMarkerData(const X3DPoint& location, const XPoint& offset, const SPixmap& pixmap)
{	
	TQ3PixmapMarkerData data;
	
	data.position                 = location;
	data.xOffset                  = offset.x;
	data.yOffset                  = offset.y;
	data.pixmapMarkerAttributeSet = nil;
	
	data.pixmap.width     = pixmap.width;
	data.pixmap.height    = pixmap.height;
	data.pixmap.rowBytes  = pixmap.rowBytes;
	data.pixmap.pixelSize = pixmap.pixelSize;
	data.pixmap.pixelType = pixmap.pixelType;
	data.pixmap.bitOrder  = pixmap.bitOrder;
	data.pixmap.byteOrder = pixmap.byteOrder;
	
	data.pixmap.image = Q3MemoryStorage_New((Byte*) pixmap.image, pixmap.rowBytes*pixmap.height);
	ThrowIfNil(data.pixmap.image);
		
	return data;
}


//---------------------------------------------------------------
//
// CreatePixmapMarker (X3DPoint, XPoint, SPixmap)
//
//---------------------------------------------------------------
static TQ3GeometryObject CreatePixmapMarker(const X3DPoint& location, const XPoint& offset, const SPixmap& pixmap)
{
	TQ3PixmapMarkerData data = GetPixmapMarkerData(location, offset, pixmap);
	
	TQ3GeometryObject object = Q3PixmapMarker_New(&data);
	
	TQ3Status status = Q3Object_Dispose(data.pixmap.image);
	ASSERT(status == kQ3Success);

	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DPixmapMarker
// ===================================================================================

//---------------------------------------------------------------
//
// X3DPixmapMarker::~X3DPixmapMarker
//
//---------------------------------------------------------------
X3DPixmapMarker::~X3DPixmapMarker()
{
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::X3DPixmapMarker (X3DPoint, XPoint, SPixmap)
//
//---------------------------------------------------------------
X3DPixmapMarker::X3DPixmapMarker(const X3DPoint& location, const XPoint& offset, const SPixmap& pixmap) : X3DGeometry(CreatePixmapMarker(location, offset, pixmap))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::X3DPixmapMarker (TQ3PixmapMarkerData)
//
//---------------------------------------------------------------
X3DPixmapMarker::X3DPixmapMarker(const TQ3PixmapMarkerData& data) : X3DGeometry(Q3PixmapMarker_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::X3DPixmapMarker (TQ3GeometryObject)
//
//---------------------------------------------------------------
X3DPixmapMarker::X3DPixmapMarker(TQ3GeometryObject object) : X3DGeometry(object)
{
	this->ConfirmType(kQ3GeometryTypePixmapMarker);
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::Clone
//
//---------------------------------------------------------------
X3DPixmapMarker* X3DPixmapMarker::Clone() const
{
	X3DPixmapMarker* object = new X3DPixmapMarker(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::GetPosition
//
//---------------------------------------------------------------
X3DPoint X3DPixmapMarker::GetPosition() const
{
	X3DPoint position;
	
	TQ3Status status = Q3PixmapMarker_GetPosition(mObject, position);
	ThrowIf3DError(status);
	
	return position;
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::SetPosition
//
//---------------------------------------------------------------
void X3DPixmapMarker::SetPosition(const X3DPoint& position)
{
	TQ3Status status = Q3PixmapMarker_SetPosition(mObject, position);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::GetXOffset
//
//---------------------------------------------------------------
long X3DPixmapMarker::GetXOffset() const
{
	long offset;
	
	TQ3Status status = Q3PixmapMarker_GetXOffset(mObject, &offset);
	ThrowIf3DError(status);
	
	return offset;
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::SetXOffset
//
//---------------------------------------------------------------
void X3DPixmapMarker::SetXOffset(long offset)
{
	TQ3Status status = Q3PixmapMarker_SetXOffset(mObject, offset);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::GetYOffset
//
//---------------------------------------------------------------
long X3DPixmapMarker::GetYOffset() const
{
	long offset;
	
	TQ3Status status = Q3PixmapMarker_GetYOffset(mObject, &offset);
	ThrowIf3DError(status);
	
	return offset;
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::SetYOffset
//
//---------------------------------------------------------------
void X3DPixmapMarker::SetYOffset(long offset)
{
	TQ3Status status = Q3PixmapMarker_SetYOffset(mObject, offset);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::GetPixmap
//
//---------------------------------------------------------------
TQ3StoragePixmap X3DPixmapMarker::GetPixmap() const
{
	TQ3StoragePixmap pixmap;
	
	TQ3Status status = Q3PixmapMarker_GetPixmap(mObject, &pixmap);
	ThrowIf3DError(status);
	
	return pixmap;
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::SetPixmap
//
//---------------------------------------------------------------
void X3DPixmapMarker::SetPixmap(TQ3StoragePixmap pixmap)
{
	TQ3Status status = Q3PixmapMarker_SetPixmap(mObject, &pixmap);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::GetData
//
//---------------------------------------------------------------
TQ3PixmapMarkerData X3DPixmapMarker::GetData() const
{
	TQ3PixmapMarkerData data;
	
	TQ3Status status = Q3PixmapMarker_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::SetData
//
//---------------------------------------------------------------
void X3DPixmapMarker::SetData(const TQ3PixmapMarkerData& data)
{
	TQ3Status status = Q3PixmapMarker_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::Submit (X3DPoint, XPoint, SPixmap, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DPixmapMarker::Submit(const X3DPoint& location, const XPoint& offset, const SPixmap& pixmap, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3PixmapMarkerData data = GetPixmapMarkerData(location, offset, pixmap);

	TQ3Status status = Q3PixmapMarker_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::Submit (TQ3PixmapMarkerData, TQ3ViewObject)	[static]
//
//---------------------------------------------------------------
void X3DPixmapMarker::Submit(const TQ3PixmapMarkerData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3PixmapMarker_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPixmapMarker::IsCompatible 						[static]
//	
//---------------------------------------------------------------
bool X3DPixmapMarker::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3GeometryTypePixmapMarker);
	
	return compatible;
}


}	// namespace Whisper
