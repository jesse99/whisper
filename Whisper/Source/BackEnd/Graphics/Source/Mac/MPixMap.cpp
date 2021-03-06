/*
 *  File:       MPixMap.cpp
 *  Summary:   	Wrapper around an offscreen whose pixels are accessible.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MPixMap.cpp,v $
 *		Revision 1.5  2001/05/30 01:15:41  jesjones
 *		DoInit no longer asserts that number of colors is a power of two.
 *		
 *		Revision 1.4  2001/04/21 03:27:34  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2000/12/10 04:04:55  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:17:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 3/04/00	JDJ		DoInit uses the system palettes for depths < 16.
 *		 <4>	 6/11/99	JDJ		Tweaked for CW Pro 5fc3.
 *		 <3>	 1/13/99	JDJ		Added GetAlphaMask, GetRedMask, etc.
 *		 <2>	 1/06/99	JDJ		SetSize ASSERTs if the pixmap is locked and being shrinking.
 *									SetColors compares color tables instead of seeds.
 *		 <1>	11/27/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XPixMap.h>

#include <XColorTable.h>
#include <XExceptions.h>
#include <XImageImporter.h>
#include <XMemUtils.h>
#include <XNumbers.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const uint32 kMaxRowBytes = 0x3FFE;				// largest legit rowBytes


// ===================================================================================
//	Internal Functions
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
	PRECONDITION(pixelSize == 1 || pixelSize == 2 || pixelSize == 4 || pixelSize == 8 || pixelSize == 16 || pixelSize == 32);

	uint32 rowBytes = ((((uint32) pixelSize * width + 7) >> 3) + 3) & ~3L;
	
	return rowBytes;
}

#pragma mark -

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
	DisposeGWorld(mWorld);
}


//---------------------------------------------------------------
//
// XPixMap::XPixMap (XSize, XColorTable*, int32)
//
//---------------------------------------------------------------
XPixMap::XPixMap(const XSize& size, const XColorTable* colors, int32 depth)
{
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
	int32 depth = image.GetDepth();
	if (depth == 24)
		depth = 32;
		
	this->DoInit(image.GetSize(), image.GetColors(), depth);
	
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
	if (depth == 24)
		depth = 32;
		
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
	XPixMap* world = new XPixMap(mSize, mColors, mDepth);
	
	const uint8* oldData = this->GetUnsafeBuffer();
	uint8* newData = world->GetUnsafeBuffer();
	
	uint32 bytes = this->GetBufferSize();
	
	BlockMoveData(oldData, newData, bytes);
	
	return world;
}


//---------------------------------------------------------------
//
// XPixMap::SetSize
//
//---------------------------------------------------------------
void XPixMap::SetSize(const XSize& size)
{
	PRECONDITION(size.width > 0 && size.height > 0);
	PRECONDITION(!this->IsLocked());							// it's probably OK to shrink a locked pixmap on the Mac, but on Windows the buffer pointer will change...
	CHECK_INVARIANT;
	
	if (mSize != size) {		
		uint32 newRowBytes = CalcRowBytes(size.width, mDepth);				
		if (newRowBytes > kMaxRowBytes)				
			ThrowErr(memFullErr);

		mCachedBuffer = nil;

		GWorldFlags flags;
		Rect bounds = XRect(kZeroPt, size).GetOSRect();
		if (size == kZeroSize)							// QuickDraw doesn't like an empty rectangle
			bounds = XRect(0, 0, 1, 1).GetOSRect();
		flags = UpdateGWorld(&mWorld, numeric_cast<int16>(mDepth), &bounds, mDepth < 16 ? (CTabHandle) *mColors : nil, nil, 0);
		QDErr err = (QDErr) ((flags & gwFlagErr) ? flags : noErr);	
		ThrowIfOSErr(err);
		ThrowIfNil(mWorld);

		PixMapHandle pixels = this->GetOSPixMap();
		mRowBytes = (uint32) ((**pixels).rowBytes & 0x3FFF);
		mSize = size;
	}

	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// XPixMap::GetUnsafeBuffer
//
// Note that the offscreen was created with the keepLocal flag so 
// SwapMMUMode does not need to be called.
//
//---------------------------------------------------------------
uint8* XPixMap::GetUnsafeBuffer()
{
	CHECK_INVARIANT;
		
	uint8* buffer = mCachedBuffer;

	if (buffer == nil) {
		CGrafPtr context = mWorld;
		PixMapHandle pixels = GetGWorldPixMap(mWorld);
		buffer = (uint8 *) GetPixBaseAddr(pixels);
		if (buffer == nil) {
			buffer = (uint8 *) GetPixBaseAddr(pixels);
			ThrowIfNil(buffer);								// should never happen
		}
	}
		
	POSTCONDITION(buffer != nil);

	return buffer;
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
	PRECONDITION(!this->IsLocked());					// required on Windows							
	CHECK_INVARIANT;
	
	if (mColors == nil || *mColors != *newColors) {
		mCachedBuffer = nil;

		XColorTable* newPalette = new XColorTable(*newColors);

		GWorldFlags flags;
		Rect bounds = XRect(kZeroPt, mSize).GetOSRect();
		if (mSize == kZeroSize)					// QuickDraw doesn't like an empty rectangle
			bounds = XRect(0, 0, 1, 1).GetOSRect();
		flags = UpdateGWorld(&mWorld, numeric_cast<int16>(mDepth), &bounds, *newColors, nil, 0);
		QDErr err = (QDErr) ((flags & gwFlagErr) ? flags : noErr);	
		ThrowIfOSErr(err);
		ThrowIfNil(mWorld);

		delete mColors;
		mColors = newPalette;
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
	PRECONDITION(mDepth > 8);

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

	if (mDepth == 16) 
		mask = 0x7C00;
	else if (mDepth == 32)
		mask = 0x00FF0000;
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
	else if (mDepth == 32)
		mask = 0x000000FF;
	else
		DEBUGSTR("Bad depth in XPixMap::GetBlueMask");

	return mask;
}

#pragma mark ~

//---------------------------------------------------------------
//
// XPixMap::Invariant 
//
//---------------------------------------------------------------
void XPixMap::Invariant() const
{
	Inherited::Invariant();
	
	uint32 rowBytes = CalcRowBytes(mSize.width, mDepth);	
	ASSERT(rowBytes <= kMaxRowBytes);
	ASSERT(mRowBytes >= rowBytes);
	ASSERT(mRowBytes % 4 == 0);
	
	ASSERT(mWorld != nil);
}


//---------------------------------------------------------------
//
// XPixMap::OnLock
//
//---------------------------------------------------------------
void XPixMap::OnLock(bool moveHigh)
{
	UNUSED(moveHigh);								// LockPixels always moves high!
	
	PixMapHandle pixels = this->GetOSPixMap();		
	if (!LockPixels(pixels))				
		ThrowErr(memFullErr);						// We don't support purgeable GWorld's so this should never happen.

	if (mCachedBuffer == nil)
		mCachedBuffer = this->GetUnsafeBuffer();
}		
	
	
//---------------------------------------------------------------
//
// XPixMap::OnUnlock
//
//---------------------------------------------------------------
void XPixMap::OnUnlock()
{
	PixMapHandle pixels = this->GetOSPixMap();
	UnlockPixels(pixels);
	
	mCachedBuffer = nil;
}	

#pragma mark ~

//---------------------------------------------------------------
//
// XPixMap::DoInit
//
//---------------------------------------------------------------
void XPixMap::DoInit(const XSize& size, const XColorTable* colors, int32 depth)
{
	PRECONDITION(size.width >= 0 && size.height >= 0);
	PRECONDITION(depth == 1 || depth == 2 || depth == 4 || depth == 8 || depth == 16 || depth == 32);	
	PRECONDITION(colors == nil || depth < 16);	
	
	mDepth = depth;
	mSize = size;
	
	mCachedBuffer = nil;
	
	if (mDepth == 1)
		if (colors == nil)
			mColors = new XColorTable(33);					// gray scale palettes are at depth plus 32
		else
			mColors = new XColorTable(*colors);
	else if (mDepth < 16)
		if (colors == nil)
			mColors = new XColorTable((ResID) (mDepth + 64)); // color palettes are at depth plus 64
		else
			mColors = new XColorTable(*colors);
	else
		mColors = nil;
	
	try {
		mRowBytes = CalcRowBytes(mSize.width, mDepth);					
		if (mRowBytes > kMaxRowBytes)
			ThrowErr(memFullErr);
	
		GWorldFlags flags = 0;							
		if (GetMMUMode() == false32b)
			flags += keepLocal;								// Otherwise GetPixBaseAddr returns a 32-bit address and SwapMMUMode must be called :-(
		
		Rect bounds = XRect(kZeroPt, mSize).GetOSRect();
		if (size == kZeroSize)								// QuickDraw doesn't like an empty rectangle
			bounds = XRect(0, 0, 1, 1).GetOSRect();
		QDErr err = NewGWorld(&mWorld, numeric_cast<int16>(mDepth), &bounds, mDepth < 16 ? (CTabHandle) *mColors : nil, nil, flags);
		ThrowIfOSErr(err);
		ThrowIfNil(mWorld);
		
		PixMapHandle pixels = this->GetOSPixMap();
		mRowBytes = (uint32) ((**pixels).rowBytes & 0x3FFF);
		
	} catch (...) {
		delete mColors;
		throw;
	}
}


}	// namespace Whisper

