/*
 *  File:       X3DTextures.cpp
 *  Summary:	QuickDraw 3D texture map objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DTextures.cpp,v $
 *		Revision 1.4  2001/04/27 09:15:22  jesjones
 *		Compressed pixmap texture is Quicktime only.
 *		
 *		Revision 1.3  2001/04/14 07:32:44  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:22:09  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DTextures.h>

#include <QuesaShader.h>
#include <QuesaStorage.h>

#include <X3DUtils.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class X3DPixmapTexture
// ===================================================================================

//---------------------------------------------------------------
//
// X3DPixmapTexture::~X3DPixmapTexture
//
//---------------------------------------------------------------
X3DPixmapTexture::~X3DPixmapTexture()
{
}


//---------------------------------------------------------------
//
// X3DPixmapTexture::X3DPixmapTexture (SPixmapStorage)
//
//---------------------------------------------------------------
X3DPixmapTexture::X3DPixmapTexture(const SPixmapStorage& pixmap) : X3DTexture(Q3PixmapTexture_New(&pixmap))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPixmapTexture::X3DPixmapTexture (TQ3StoragePixmap)
//
//---------------------------------------------------------------
X3DPixmapTexture::X3DPixmapTexture(const TQ3StoragePixmap& pixmap) : X3DTexture(Q3PixmapTexture_New(&pixmap))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPixmapTexture::X3DPixmapTexture (TQ3TextureObject)
//
//---------------------------------------------------------------
X3DPixmapTexture::X3DPixmapTexture(TQ3TextureObject object) : X3DTexture(object)
{
	this->ConfirmType(kQ3TextureTypePixmap);
}


//---------------------------------------------------------------
//
// X3DPixmapTexture::Clone
//
//---------------------------------------------------------------
X3DPixmapTexture* X3DPixmapTexture::Clone() const
{
	X3DPixmapTexture* object = new X3DPixmapTexture(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DPixmapTexture::GetPixmap
//
//---------------------------------------------------------------
SPixmapStorage X3DPixmapTexture::GetPixmap() const
{
	TQ3StoragePixmap pixels;
	
	TQ3Status status = Q3PixmapTexture_GetPixmap(mObject, &pixels);
	ThrowIf3DError(status);
	
	SPixmapStorage pixmap(pixels);				// increments ref count

	status = Q3Object_Dispose(pixels.image);	// decrement ref count so image is deleted when last SPixmapStorage is destructed
	ThrowIf3DError(status);

	return pixmap;
}


//---------------------------------------------------------------
//
// X3DPixmapTexture::SetPixmap (SPixmapStorage)
//
//---------------------------------------------------------------
void X3DPixmapTexture::SetPixmap(const SPixmapStorage& pixmap)
{	
	TQ3Status status = Q3PixmapTexture_SetPixmap(mObject, &pixmap);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPixmapTexture::SetPixmap (TQ3StoragePixmap)
//
//---------------------------------------------------------------
void X3DPixmapTexture::SetPixmap(const TQ3StoragePixmap& pixmap)
{	
	TQ3Status status = Q3PixmapTexture_SetPixmap(mObject, &pixmap);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPixmapTexture::IsCompatible			 			[static]
//	
//---------------------------------------------------------------
bool X3DPixmapTexture::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3TextureTypePixmap);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DMipmapTexture
// ===================================================================================

//---------------------------------------------------------------
//
// X3DMipmapTexture::~X3DMipmapTexture
//
//---------------------------------------------------------------
X3DMipmapTexture::~X3DMipmapTexture()
{
}


//---------------------------------------------------------------
//
// X3DMipmapTexture::X3DMipmapTexture (TQ3Mipmap)
//
//---------------------------------------------------------------
X3DMipmapTexture::X3DMipmapTexture(const TQ3Mipmap& mipmap): X3DTexture(Q3MipmapTexture_New(&mipmap))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMipmapTexture::X3DMipmapTexture (TQ3TextureObject)
//
//---------------------------------------------------------------
X3DMipmapTexture::X3DMipmapTexture(TQ3TextureObject object) : X3DTexture(object)
{
	this->ConfirmType(kQ3TextureTypeMipmap);
}


//---------------------------------------------------------------
//
// X3DMipmapTexture::Clone
//
//---------------------------------------------------------------
X3DMipmapTexture* X3DMipmapTexture::Clone() const
{
	X3DMipmapTexture* object = new X3DMipmapTexture(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DMipmapTexture::GetMipmap
//
//---------------------------------------------------------------
TQ3Mipmap X3DMipmapTexture::GetMipmap() const
{
	TQ3Mipmap mipmap;
	
	TQ3Status status = Q3MipmapTexture_GetMipmap(mObject, &mipmap);
	ThrowIf3DError(status);
	
	return mipmap;
}


//---------------------------------------------------------------
//
// X3DMipmapTexture::SetMipmap
//
//---------------------------------------------------------------
void X3DMipmapTexture::SetMipmap(const TQ3Mipmap& mipmap)
{
	TQ3Status status = Q3MipmapTexture_SetMipmap(mObject, &mipmap);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMipmapTexture::IsCompatible			 			[static]
//	
//---------------------------------------------------------------
bool X3DMipmapTexture::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3TextureTypeMipmap);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DCompressedPixmapTexture
// ===================================================================================

#if QUESA_SUPPORT_QUICKTIME

//---------------------------------------------------------------
//
// X3DCompressedPixmapTexture::~X3DCompressedPixmapTexture
//
//---------------------------------------------------------------
X3DCompressedPixmapTexture::~X3DCompressedPixmapTexture()
{
}


//---------------------------------------------------------------
//
// X3DCompressedPixmapTexture::X3DCompressedPixmapTexture (TQ3CompressedPixmap)
//
//---------------------------------------------------------------
X3DCompressedPixmapTexture::X3DCompressedPixmapTexture(const TQ3CompressedPixmap& pixmap) : X3DTexture(Q3CompressedPixmapTexture_New(&pixmap))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCompressedPixmapTexture::X3DCompressedPixmapTexture (TQ3TextureObject)
//
//---------------------------------------------------------------
X3DCompressedPixmapTexture::X3DCompressedPixmapTexture(TQ3TextureObject object) : X3DTexture(object)
{
	this->ConfirmType(kQ3TextureTypeCompressedPixmap);
}


//---------------------------------------------------------------
//
// X3DCompressedPixmapTexture::Clone
//
//---------------------------------------------------------------
X3DCompressedPixmapTexture* X3DCompressedPixmapTexture::Clone() const
{
	X3DCompressedPixmapTexture* object = new X3DCompressedPixmapTexture(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DCompressedPixmapTexture::GetPixmap
//
//---------------------------------------------------------------
TQ3CompressedPixmap X3DCompressedPixmapTexture::GetPixmap() const
{
	TQ3CompressedPixmap pixels;
	
	TQ3Status status = Q3CompressedPixmapTexture_GetCompressedPixmap(mObject, &pixels);
	ThrowIf3DError(status);
	
	return pixels;
}


//---------------------------------------------------------------
//
// X3DCompressedPixmapTexture::SetPixmap
//
//---------------------------------------------------------------
void X3DCompressedPixmapTexture::SetPixmap(const TQ3CompressedPixmap& pixmap)
{	
	TQ3Status status = Q3CompressedPixmapTexture_SetCompressedPixmap(mObject, &pixmap);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCompressedPixmapTexture::IsCompatible			 			[static]
//	
//---------------------------------------------------------------
bool X3DCompressedPixmapTexture::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3TextureTypeCompressedPixmap);
	
	return compatible;
}
#endif	// QUESA_SUPPORT_QUICKTIME

}		// namespace Whisper
