/*
 *  File:       X3DBitmaps.cpp
 *  Summary:	Wrappers for QuickDraw 3D bitmap structs.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DBitmaps.cpp,v $
 *		Revision 1.5  2001/04/21 03:35:04  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/04/14 07:27:26  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.3  2000/12/10 04:08:35  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:13:46  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DBitmaps.h>

#include <Quesa.h>
#include <QuesaStorage.h>

#include <X3DUtils.h>
#include <XConstants.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XNumbers.h>

#if MAC
	#include <MSystemInfo.h>
#endif

namespace Whisper {

#if WIN
	inline void ThrowIfOSErr(OSStatus err)	{ThrowIf(err != noErr);}	
#endif
	

// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// SetAlphaChannel
//
//---------------------------------------------------------------
static void SetAlphaChannel(void* image, uint32 width, uint32 height, uint32 pixelSize, uint32 rowBytes, float alpha)
{
	PRECONDITION(image != nil);
	PRECONDITION(pixelSize == 16 || pixelSize == 32);
	PRECONDITION(width < 16*1024L);
	PRECONDITION(height < 16*1024L);
	PRECONDITION(rowBytes < 16*1024L);
	PRECONDITION(alpha >= 0.0f);
	PRECONDITION(alpha <= 1.0f);
	
	int32 row, col;
	uint8* pixels = static_cast<uint8*>(image);
	
	if (pixelSize == 32) {		
		uint8 value = (uint8) (alpha*255);
		
		for (row = 0; row < height; row++) {
			for (col = 0; col < 4*width; col += 4)
				pixels[col] = value;
				
			pixels += rowBytes;
		}
		
	} else if (pixelSize == 16) {		
		bool transparent = alpha > 0.5f;
				
		for (row = 0; row < height; row++) {
			for (col = 0; col < 2*width; col += 2) {
				if (transparent)
					pixels[col] |= 0x80;
				else
					pixels[col] &= 0x7F;
			}
				
			pixels += rowBytes;
		}
	}
}

#pragma mark -

// ===================================================================================
//	struct SBitmap
// ===================================================================================

//---------------------------------------------------------------
//
// SBitmap::SBitmap ()
//
//---------------------------------------------------------------
SBitmap::SBitmap()
{
	image    = nil;
	width    = 0;
	height   = 0;
	rowBytes = 0;
	bitOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;
}


//---------------------------------------------------------------
//
// SBitmap::SBitmap (BitMap*)
//
//---------------------------------------------------------------
SBitmap::SBitmap(BitMap* bitmap)
{
	PRECONDITION(bitmap = nil);
	
	image    = (uint8*) bitmap->baseAddr;
	width    = numeric_cast<uint32>(bitmap->bounds.right - bitmap->bounds.left);
	height   = numeric_cast<uint32>(bitmap->bounds.bottom - bitmap->bounds.top);
	rowBytes = numeric_cast<uint32>(bitmap->rowBytes & 0x3FFF);
	bitOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;
}


//---------------------------------------------------------------
//
// SBitmap::SBitmap (PixMapHandle)
//
//---------------------------------------------------------------
SBitmap::SBitmap(PixMapHandle pixmap)
{
	PRECONDITION((**pixmap).pixelSize == 1);
	
	image    = (uint8*) (**pixmap).baseAddr;
	width    = numeric_cast<uint32>((**pixmap).bounds.right - (**pixmap).bounds.left);
	height   = numeric_cast<uint32>((**pixmap).bounds.bottom - (**pixmap).bounds.top);
	rowBytes = numeric_cast<uint32>((**pixmap).rowBytes & 0x3FFF);
	bitOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;
}


//---------------------------------------------------------------
//
// SBitmap::SBitmap (GWorldPtr)
//
//---------------------------------------------------------------
SBitmap::SBitmap(GWorldPtr world)
{
	PRECONDITION(world != nil);
	
	PixMapHandle pixmap = GetGWorldPixMap(world);
	PRECONDITION((GetPixelsState(pixmap) & pixelsLocked) == pixelsLocked);
	PRECONDITION((**pixmap).pixelSize == 1);
	
	image    = (uint8*) (**pixmap).baseAddr;
	width    = numeric_cast<uint32>((**pixmap).bounds.right - (**pixmap).bounds.left);
	height   = numeric_cast<uint32>((**pixmap).bounds.bottom - (**pixmap).bounds.top);
	rowBytes = numeric_cast<uint32>((**pixmap).rowBytes & 0x3FFF);
	bitOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;
}


//---------------------------------------------------------------
//
// SBitmap::SBitmap (TQ3Bitmap)
//
//---------------------------------------------------------------
SBitmap::SBitmap(TQ3Bitmap data)
{
	image    = data.image;
	width    = data.width;
	height   = data.height;
	rowBytes = data.rowBytes;
	bitOrder = data.bitOrder;
}

#pragma mark -

// ===================================================================================
//	struct SPixmap
// ===================================================================================

//---------------------------------------------------------------
//
// SPixmap::SPixmap (int16, bool)
//
//---------------------------------------------------------------
SPixmap::SPixmap(int16 depth, bool useAlpha)
{
	image     = nil;
	width     = 0;
	height    = 0;
	rowBytes  = 0;
	pixelSize = depth;
	bitOrder  = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;
	byteOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;

	if (pixelSize == 32)
		if (useAlpha)
			pixelType = kQ3PixelTypeARGB32;
		else
			pixelType = kQ3PixelTypeRGB32;
			
	else if (pixelSize == 16)
		if (useAlpha)
			pixelType = kQ3PixelTypeARGB16;
		else
			pixelType = kQ3PixelTypeRGB16;
			
	else
		DEBUGSTR("SPixmap::SPixmap (int16, bool) was passed a bogus pixel depth: ", pixelSize);
}


//---------------------------------------------------------------
//
// SPixmap::SPixmap (TQ3Pixmap)
//
//---------------------------------------------------------------
SPixmap::SPixmap(TQ3Pixmap data)
{
	image     = data.image;
	width     = data.width;
	height    = data.height;
	rowBytes  = data.rowBytes;
	pixelSize = data.pixelSize;
	pixelType = data.pixelType;
	bitOrder  = data.bitOrder;
	byteOrder = data.byteOrder;
}


//---------------------------------------------------------------
//
// SPixmap::SPixmap (GWorldPtr, bool)
//
//---------------------------------------------------------------
SPixmap::SPixmap(GWorldPtr world, bool useAlpha)
{
	PRECONDITION(world != nil);
	
	PixMapHandle pixmap = GetGWorldPixMap(world);
	PRECONDITION((GetPixelsState(pixmap) & pixelsLocked) == pixelsLocked);
	
	image     = (**pixmap).baseAddr;
	width     = numeric_cast<uint32>((**pixmap).bounds.right - (**pixmap).bounds.left);
	height    = numeric_cast<uint32>((**pixmap).bounds.bottom - (**pixmap).bounds.top);
	rowBytes  = numeric_cast<uint32>((**pixmap).rowBytes & 0x3FFF);
	pixelSize = (**pixmap).pixelSize;
	bitOrder  = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;
	byteOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;

	if (pixelSize == 32)
		if (useAlpha)
			pixelType = kQ3PixelTypeARGB32;
		else
			pixelType = kQ3PixelTypeRGB32;
			
	else if (pixelSize == 16)
		if (useAlpha)
			pixelType = kQ3PixelTypeARGB16;
		else
			pixelType = kQ3PixelTypeRGB16;
			
	else
		DEBUGSTR("SPixmap::SPixmap (GWorldPtr) was passed a bogus pixel depth: ", pixelSize);
}


//---------------------------------------------------------------
//
// SPixmap::SPixmap (PixMapHandle, bool)
//
//---------------------------------------------------------------
SPixmap::SPixmap(PixMapHandle pixmap, bool useAlpha)
{	
	image     = (**pixmap).baseAddr;
	width     = numeric_cast<uint32>((**pixmap).bounds.right - (**pixmap).bounds.left);
	height    = numeric_cast<uint32>((**pixmap).bounds.bottom - (**pixmap).bounds.top);
	rowBytes  = numeric_cast<uint32>((**pixmap).rowBytes & 0x3FFF);
	pixelSize = (**pixmap).pixelSize;
	bitOrder  = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;
	byteOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;

	if (pixelSize == 32)
		if (useAlpha)
			pixelType = kQ3PixelTypeARGB32;
		else
			pixelType = kQ3PixelTypeRGB32;
			
	else if (pixelSize == 16)
		if (useAlpha)
			pixelType = kQ3PixelTypeARGB16;
		else
			pixelType = kQ3PixelTypeRGB16;
			
	else
		DEBUGSTR("SPixmap::SPixmap (PixMapHandle) was passed a bogus pixel depth: ", pixelSize);
}


//---------------------------------------------------------------
//
// SPixmap::SetAlphaChannel
//
//---------------------------------------------------------------
void SPixmap::SetAlphaChannel(float alpha)
{
	PRECONDITION(pixelType == kQ3PixelTypeARGB32 || pixelType == kQ3PixelTypeARGB16);

	Whisper::SetAlphaChannel(image, width, height, pixelSize, rowBytes, alpha);
}

#pragma mark -

// ===================================================================================
//	struct SPixmapStorage
// ===================================================================================

//---------------------------------------------------------------
//
// SPixmapStorage::~SPixmapStorage
//
//---------------------------------------------------------------
SPixmapStorage::~SPixmapStorage()
{
	if (image != nil) {
		TQ3Status status = Q3Object_Dispose(image);
		ASSERT(status == kQ3Success);
	}
}


//---------------------------------------------------------------
//
// SPixmapStorage::SPixmapStorage (int16, bool)
//
//---------------------------------------------------------------
SPixmapStorage::SPixmapStorage(int16 depth, bool useAlpha)
{
	SPixmap pixmap(depth, useAlpha);
	
	this->DoInit(pixmap);
}


//---------------------------------------------------------------
//
// SPixmapStorage::SPixmapStorage (SPixmap)
//
//---------------------------------------------------------------
SPixmapStorage::SPixmapStorage(const SPixmap& pixmap)
{
	this->DoInit(pixmap);
}


//---------------------------------------------------------------
//
// SPixmapStorage::SPixmapStorage (PicHandle, int32, int32, int16, int16, bool)
//
//---------------------------------------------------------------
SPixmapStorage::SPixmapStorage(PicHandle pict, int32 width, int32 height, int16 depth, int16 dither, bool useAlpha)
{
	PRECONDITION(pict != nil);
	PRECONDITION(*pict != nil);
	PRECONDITION(width < 16*1024L);
	PRECONDITION(height < 16*1024L);
	PRECONDITION(depth == 16 || depth == 32);
	
	Rect dstRect;
	MacSetRect(&dstRect, 0, 0, numeric_cast<int16>(width), numeric_cast<int16>(height));

	// Create a temporary GWorld of the correct size.
	GWorldPtr world = nil;
	OSErr err = NewGWorld(&world, depth, &dstRect, nil, nil, 0);
	ThrowIfOSErr(err);
	ThrowIfNil(world);

	// Lock it down since we're going to be drawing into it.
	PixMapHandle pixels = GetGWorldPixMap(world);		
	if (!LockPixels(pixels))				
		ThrowIfOSErr(memFullErr);			// GWorld isn't purgeable so this should never happen

	// Save the old port and device.
	CGrafPtr oldPort;
	GDHandle oldDevice;
	GetGWorld(&oldPort, &oldDevice);

	// Switch to the GWorld port (and implicitly device).
	SetGWorld(world, nil);	

	// Draw the picture into the GWorld. If QuickTime is available
	// we'll use DrawTrimmedPicture so we can supress dithering.
#if MAC
	if (MSystemInfo::HasQuickTime())		// $$$ how do we check for QuickTime on Windows?
		err = ::DrawTrimmedPicture(pict, &dstRect, nil, dither, nil);		
	else
#else
	UNUSED(dither);
#endif
		::DrawPicture(pict, &dstRect);

	// Restore the port and device and throw if DrawTrimmedPicture 
	// failed.
	SetGWorld(oldPort, oldDevice);		
	ThrowIfOSErr(err);

	// Initialize our object.
	SPixmap pixmap(world, useAlpha);
	this->DoInit(pixmap);

	// Toss the GWorld.
	DisposeGWorld(world);
}


//---------------------------------------------------------------
//
// SPixmapStorage::SPixmapStorage (TQ3StoragePixmap)
//
//---------------------------------------------------------------
SPixmapStorage::SPixmapStorage(TQ3StoragePixmap pixmap)
{
	this->DoInit(pixmap);
}


//---------------------------------------------------------------
//
// SPixmapStorage::SPixmapStorage (SPixmapStorage)
//
//---------------------------------------------------------------
SPixmapStorage::SPixmapStorage(const SPixmapStorage& rhs)
{
	this->DoInit(rhs);
}


//---------------------------------------------------------------
//
// SPixmapStorage::operator=
//
//---------------------------------------------------------------
SPixmapStorage& SPixmapStorage::operator=(const SPixmapStorage& rhs)
{
	if (this != &rhs) {
		if (image != nil) {
			TQ3Status status = Q3Object_Dispose(image);
			ThrowIf3DError(status);
		}
		
		this->DoInit(rhs);
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// SPixmapStorage::SetAlphaChannel
//
//---------------------------------------------------------------
void SPixmapStorage::SetAlphaChannel(float alpha)
{
	PRECONDITION(image != nil);
	PRECONDITION(pixelType == kQ3PixelTypeARGB32 || pixelType == kQ3PixelTypeARGB16);

	uint8* buffer;
	uint32 validSize, bufferSize;

	TQ3Status status = Q3MemoryStorage_GetBuffer(image, &buffer, &validSize, &bufferSize);
	ThrowIf3DError(status);
	
	Whisper::SetAlphaChannel(buffer, width, height, pixelSize, rowBytes, alpha);
}


//---------------------------------------------------------------
//
// SPixmapStorage::DoInit (SPixmap)
//
//---------------------------------------------------------------
void SPixmapStorage::DoInit(const SPixmap& pixmap)
{
	width     = pixmap.width;
	height    = pixmap.height;
	rowBytes  = pixmap.rowBytes;
	pixelSize = pixmap.pixelSize;
	pixelType = pixmap.pixelType;
	bitOrder  = pixmap.bitOrder;
	byteOrder = pixmap.byteOrder;
	
	image = Q3MemoryStorage_New(reinterpret_cast<uint8*>(pixmap.image), rowBytes*height);
	ThrowIfNil(image);
}


//---------------------------------------------------------------
//
// SPixmapStorage::DoInit (TQ3StoragePixmap)
//
//---------------------------------------------------------------
void SPixmapStorage::DoInit(const TQ3StoragePixmap& pixmap)
{
	image     = pixmap.image;
	width     = pixmap.width;
	height    = pixmap.height;
	rowBytes  = pixmap.rowBytes;
	pixelSize = pixmap.pixelSize;
	pixelType = pixmap.pixelType;
	bitOrder  = pixmap.bitOrder;
	byteOrder = pixmap.byteOrder;

	if (image != nil)
		(void) Q3Shared_GetReference(image);
}


}	// namespace Whisper

