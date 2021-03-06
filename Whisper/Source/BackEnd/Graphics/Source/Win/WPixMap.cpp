/*
 *  File:       WPixMap.cpp
 *  Summary:   	Wrapper around a DIB-section.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WPixMap.cpp,v $
 *		Revision 1.4  2001/04/21 03:33:08  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2000/12/10 04:07:35  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:10:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 4/08/00	JDJ		Tweaked SBitmapInfo so that it creates a top-down bitmap.
 *		 <2>	 1/13/99	JDJ		Added GetAlphaMask, GetRedMask, etc.
 *		 <1>	 1/07/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XPixMap.h>

#include <XColorTable.h>
#include <XExceptions.h>
#include <XImageImporter.h>
#include <XMemUtils.h>
#include <XNumbers.h>

namespace Whisper {


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CalcRowBytes
//
// Calculates bytes per row for given width and pixel size.
//
//---------------------------------------------------------------
inline uint32 CalcRowBytes(int32 width, int32 pixelSize)
{
	PRECONDITION(width >= 0);
	PRECONDITION(pixelSize == 1 || pixelSize == 4 || pixelSize == 8 || pixelSize == 16 || pixelSize == 24 || pixelSize == 32);

	uint32 rowBytes = ((((uint32) pixelSize * width + 7) >> 3) + 3) & ~3L;
	
	return rowBytes;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	struct XPixMap::SBitmapInfo
// ===================================================================================

//---------------------------------------------------------------
//
// XPixMap::SBitmapInfo::SBitmapInfo
//
//---------------------------------------------------------------
XPixMap::SBitmapInfo::SBitmapInfo(const XSize& size, int32 depth, const XColorTable* colors)
{
	PRECONDITION(size.width >= 0);
	PRECONDITION(size.height >= 0);
	PRECONDITION(depth == 1 || depth == 4 || depth == 8 || depth == 16 || depth == 24 || depth == 32);	
	PRECONDITION(colors == nil || depth < 16);	

	uint32 numColors = depth < 16 ? (1UL << depth) : 0UL;
	
	if (colors != nil)
		numColors = Min(numColors, colors->GetSize());

	bmiHeader.biSize			= sizeof(bmiHeader);
	bmiHeader.biWidth			= size.width;
	bmiHeader.biHeight			= -size.height;				// negative so that we get a top-down bitmap
	bmiHeader.biPlanes			= 1;
	bmiHeader.biBitCount		= numeric_cast<uint16>(depth);
	bmiHeader.biCompression		= BI_RGB;
	bmiHeader.biSizeImage		= 0;
	bmiHeader.biXPelsPerMeter	= 1;
	bmiHeader.biYPelsPerMeter	= 1;
	bmiHeader.biClrUsed			= numColors;
	bmiHeader.biClrImportant	= 0;
	
	if (colors != nil)
		BlockMoveData((const RGBQUAD*) *colors, bmiColors, numColors*sizeof(RGBQUAD));
}


//---------------------------------------------------------------
//
// XPixMap::SBitmapInfo::operator new					[static]
//
// BITMAPINFO is defined to only have one color so we'll be a bit
// sleazy and always allocate enough for 256 colors.
//
//---------------------------------------------------------------
void* XPixMap::SBitmapInfo::operator new(size_t size)
{
	UNUSED(size);
	COMPILE_CHECK(sizeof(XPixMap::SBitmapInfo) == sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD));

	size_t bytes = sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD);

	void* ptr = PrimitiveNewPtr(bytes);
	ThrowIfNil(ptr);
	
	return ptr;
}


//---------------------------------------------------------------
//
// XPixMap::SBitmapInfo::operator delete				[static]
//
//---------------------------------------------------------------
void XPixMap::SBitmapInfo::operator delete(void* ptr)
{
	if (ptr != nil)
		PrimitiveFreePtr(ptr);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XPixMap
// ===================================================================================

//---------------------------------------------------------------
//
// XPixMap::~XPixMap
//
//---------------------------------------------------------------
XPixMap::~XPixMap()
{
	VERIFY(::DeleteObject(mDIB));
	VERIFY(::DeleteDC(mDC));

	delete mInfo;
	mInfo = nil;

	mDIB = nil;
	mBits = nil;
	mDC = nil;
}


//---------------------------------------------------------------
//
// XPixMap::XPixMap (XSize, XColorTable*, int32)
//
//---------------------------------------------------------------
XPixMap::XPixMap(const XSize& size, const XColorTable* colors, int32 depth)
{	
	mDIB  = nil;
	mDC   = nil;
	mInfo = nil;

	this->DoInit(size, colors, depth);
	
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XPixMap::XPixMap (XImageImporter)
//
//---------------------------------------------------------------
XPixMap::XPixMap(const XImageImporter& image)
{
	mDIB  = nil;
	mDC   = nil;
	mInfo = nil;

	this->DoInit(image.GetSize(), image.GetColors(), image.GetDepth());
	
	image.Unpack(this);
	
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XPixMap::XPixMap (XImageImporter, XColorTable*, int32)
//
//---------------------------------------------------------------
XPixMap::XPixMap(const XImageImporter& image, const XColorTable* colors, int32 depth)
{
	mDIB  = nil;
	mDC   = nil;
	mInfo = nil;

	this->DoInit(image.GetSize(), colors, depth);
	
	image.Unpack(this);
	
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XPixMap::Clone
//
//---------------------------------------------------------------
XPixMap* XPixMap::Clone() const
{
	XPixMap* pixmap = new XPixMap(mSize, mColors, mDepth);
	
	const uint8* oldData = this->GetUnsafeBuffer();
	uint8* newData = pixmap->GetUnsafeBuffer();
	
	uint32 bytes = this->GetBufferSize();
	
	BlockMoveData(oldData, newData, bytes);
	
	return pixmap;
}


//---------------------------------------------------------------
//
// XPixMap::SetSize
//
//---------------------------------------------------------------
void XPixMap::SetSize(const XSize& size)
{
	PRECONDITION(size.width > 0);
	PRECONDITION(size.height > 0);
	PRECONDITION(!this->IsLocked());			// mBits may not be valid when this method returns							
	CHECK_INVARIANT;
		
	if (mSize != size) {		
		SBitmapInfo* newInfo = new SBitmapInfo(size, mDepth, mColors);
		
		uint8* bits = nil;
		HBITMAP dib = ::CreateDIBSection(nil,				// device context (only required if DIB_PAL_COLORS is used)
								  		 newInfo,			// bitmap info
								  		 DIB_RGB_COLORS,	// colors are specified using RGBColor's instead of palette indices
								  		 (void**) &bits,	// pointer that is set to point to DIB bits
								  		 nil,				// optional file mapping offset
								  		 0);				// file mapping offset
		ThrowIfBadHandle(dib);
		ThrowIfNil(bits);
		
		VERIFY(::DeleteObject(mDIB));
		delete mInfo;
		
		mDIB  = dib;
		mBits = bits;
		mInfo = newInfo;

		mSize = size;
		mRowBytes = CalcRowBytes(size.width, mDepth);
		
		(void) ::SelectObject(mDC, mDIB);
	}

	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// XPixMap::GetUnsafeBuffer
//
//---------------------------------------------------------------
uint8* XPixMap::GetUnsafeBuffer()
{
	(void) GdiFlush();
	
	return mBits;
}
		

//---------------------------------------------------------------
//
// XPixMap::SetColors
//
//---------------------------------------------------------------
void XPixMap::SetColors(const XColorTable* newColors)
{
	PRECONDITION(newColors != nil);
	PRECONDITION(mDepth < 16);
	PRECONDITION(!this->IsLocked());			// mBits may not be valid when this method returns							
	CHECK_INVARIANT;
	
	if (mColors == nil || *mColors != *newColors) {
		SBitmapInfo* newInfo = new SBitmapInfo(mSize, mDepth, newColors);

		XColorTable* newPalette = new XColorTable(*newColors);

		uint8* bits = nil;
		HBITMAP dib = ::CreateDIBSection(nil,				// device context (only required if DIB_PAL_COLORS is used)
								  		 newInfo,			// bitmap info
								  		 DIB_RGB_COLORS,	// colors are specified using RGBColor's instead of palette indices
								  		 (void**) &bits,	// pointer that is set to point to DIB bits
								  		 nil,				// optional file mapping offset
								  		 0);				// file mapping offset
		ThrowIfBadHandle(dib);
		ThrowIfNil(bits);
		
		BlockMoveData(mBits, bits, this->GetBufferSize());
		
		VERIFY(::DeleteObject(mDIB));
		delete mInfo;
		
		mDIB  = dib;
		mBits = bits;
		mInfo = newInfo;

		delete mColors;
		mColors = newPalette;
		
		(void) ::SelectObject(mDC, mDIB);
	}
	
	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// XPixMap::GetAlphaMask
//
//---------------------------------------------------------------
uint32 XPixMap::GetAlphaMask() const
{
	PRECONDITION(mDepth == 16 || mDepth == 32);

	uint32 mask = 0;

	if (mDepth == 16) 
		mask = 0x8000;
	else if (mDepth == 32)
		mask = 0xFF000000;
	else
		DEBUGSTR("Bad depth in XPixMap::GetAlphaMask");

	return mask;
}


//---------------------------------------------------------------
//
// XPixMap::GetRedMask
//
//---------------------------------------------------------------
uint32 XPixMap::GetRedMask() const
{
	PRECONDITION(mDepth > 8);

	uint32 mask = 0;

	if (mDepth == 16)			// unlike DDB's uncompressed DIBs are always 1-5-5-5
		mask = 0x7C00;
	else if (mDepth == 24)
		mask = 0x0000FF;
	else if (mDepth == 32)
		mask = 0x000000FF;
	else
		DEBUGSTR("Bad depth in XPixMap::GetRedMask");

	return mask;
}


//---------------------------------------------------------------
//
// XPixMap::GetGreenMask
//
//---------------------------------------------------------------
uint32 XPixMap::GetGreenMask() const 
{
	PRECONDITION(mDepth > 8);

	uint32 mask = 0;

	if (mDepth == 16) 
		mask = 0x03E0;
	else if (mDepth == 24)
		mask = 0x00FF00;
	else if (mDepth == 32)
		mask = 0x0000FF00;
	else
		DEBUGSTR("Bad depth in XPixMap::GetGreenMask");

	return mask;
}


//---------------------------------------------------------------
//
// XPixMap::GetBlueMask
//
//---------------------------------------------------------------
uint32 XPixMap::GetBlueMask() const
{
	PRECONDITION(mDepth > 8);

	uint32 mask = 0;

	if (mDepth == 16) 
		mask = 0x001F;
	else if (mDepth == 24)
		mask = 0xFF0000;
	else if (mDepth == 32)
		mask = 0x00FF0000;
	else
		DEBUGSTR("Bad depth in XPixMap::GetBlueMask");

	return mask;
}


//---------------------------------------------------------------
//
// XPixMap::CreateCompatibleBitmap 
//
//---------------------------------------------------------------
HBITMAP	XPixMap::CreateCompatibleBitmap(HDC dc) const
{
	HDC memoryDC = nil;
	HBITMAP bitmap = nil;

	try {
		memoryDC = ::CreateCompatibleDC(dc);
		ThrowIfBadHandle(memoryDC);
  
		bitmap = ::CreateCompatibleBitmap(dc, this->GetWidth(), this->GetHeight());
		ThrowIfBadHandle(bitmap);

		HANDLE oldBitmap = ::SelectObject(memoryDC, bitmap);

		{
		XLocker lock(this);
			const BITMAPINFO* info = this->GetInfo();

			int32 result = ::StretchDIBits(memoryDC,			// device context
										   0,					// destination rect
										   0,
										   this->GetWidth(),
										   this->GetHeight(),
										   0,					// source rect
										   0,
										   this->GetWidth(),
										   this->GetHeight(),
										   this->GetBuffer(),	// bitmap bits
										   info,				// bitmap data
										   DIB_RGB_COLORS,		// usage
										   SRCCOPY);			// raster operation code
			ThrowIf(result == GDI_ERROR);	
		}

		(void) ::SelectObject(memoryDC, oldBitmap);

		::DeleteDC(memoryDC);

	} catch (...) {
		::DeleteDC(memoryDC);
		throw;
	}

	return bitmap;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XPixMap::Invariant 
//
//---------------------------------------------------------------
void XPixMap::Invariant() const
{
	Inherited::Invariant();
	
	uint32 rowBytes = CalcRowBytes(mSize.width, mDepth);	
	ASSERT(mRowBytes >= rowBytes);
	ASSERT(mRowBytes % 4 == 0);
	
	ASSERT(mDIB != nil);
	ASSERT(mDC != nil);
	ASSERT(mBits != nil);
	ASSERT(mInfo != nil);
}


//---------------------------------------------------------------
//
// XPixMap::OnLock
//
//---------------------------------------------------------------
void XPixMap::OnLock(bool moveHigh)
{
	UNUSED(moveHigh);								
	
	// no-op on Windows
}		
	
	
//---------------------------------------------------------------
//
// XPixMap::OnUnlock
//
//---------------------------------------------------------------
void XPixMap::OnUnlock()
{
	// no-op on Windows
}	

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XPixMap::DoInit
//
// $$$ This differs from the Mac implementation if colors is nil
// $$$ and depth < 16. On the Mac this will cause the appropiate
// $$$ system palette to be used. Unfortunately Windows doesn't
// $$$ appear to have a good analogue for this (there is a
// $$$ GetSystemPaletteEntries function but it returns the
// $$$ realized colors for a specified device which isn't really
// $$$ the same). The best match I could find was the DEFAULT_PALETTE
// $$$ stock object, but this only returns the static colors which
// $$$ won't work too well if the depth is 8...
//
//---------------------------------------------------------------
void XPixMap::DoInit(const XSize& size, const XColorTable* colors, int32 depth)
{
	PRECONDITION(size.width >= 0);
	PRECONDITION(size.height >= 0);
	PRECONDITION(colors == nil || depth < 16);	
	PRECONDITION(mDC == nil);
	PRECONDITION(mInfo == nil);

	mDepth = depth;
	mSize = size;
		
	if (mDepth < 16) {
		if (colors == nil) {
			if (mDepth == 1) {
				OSColor palette[2] = {{0, 0, 0, 0}, {255, 255, 255, 0}};
				mColors = new XColorTable(palette, 2);

			} else {
				HPALETTE palette = (HPALETTE) ::GetStockObject(DEFAULT_PALETTE);
				ThrowIfBadHandle(palette);
				
				mColors = new XColorTable(palette);
			}
			
		} else
			mColors = new XColorTable(*colors);
	
	} else
		mColors = nil;
			
	try {
		mDC = ::CreateCompatibleDC(nil);
		ThrowIfBadHandle(mDC);
	
		mInfo = new SBitmapInfo(size, depth, mColors);
		
		mDIB = ::CreateDIBSection(nil,				// device context (only required if DIB_PAL_COLORS is used)
								  mInfo,			// bitmap info
								  DIB_RGB_COLORS,	// colors are specified using RGBColor's instead of palette indices
								  (void**) &mBits,	// pointer that is set to point to DIB bits
								  nil,				// optional file mapping offset
								  0);				// file mapping offset
		ThrowIfBadHandle(mDIB);
		ThrowIfNil(mBits);
		
		mRowBytes = CalcRowBytes(size.width, mDepth);
		
		(void) ::SelectObject(mDC, mDIB);
		
	} catch (...) {
		if (mDC != nil)
			DeleteDC(mDC);
		
		delete mColors;
		delete mInfo;
		throw;
	}
}


}	// namespace Whisper

