/*
 *  File:       XBaseImage.cpp
 *  Summary:   	Abstract base class for objects that can be initialized using an XImageSource.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XBaseImage.cpp,v $
 *		Revision 1.7  2001/05/30 01:17:13  jesjones
 *		Pixel values are now uint32's (instead of int32's).
 *		
 *		Revision 1.6  2001/04/21 03:39:33  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/17 01:42:54  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/02/28 10:00:26  jesjones
 *		GetPixelAt and SetPixelAt handle 24-bit images (I hope).
 *		
 *		Revision 1.3  2000/12/10 04:09:10  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:26:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 5/01/99	JDJ		Fixed the second ASSERT in GetBits.
 *		 <2>	 4/10/99	JDJ		Fixed ColorToPixel.
 *		 <1>	11/27/97	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XBaseImage.h>

#include <XColorTable.h>
#include <XExceptions.h>
#include <XMemUtils.h>
#include <XNumbers.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetShift
//
// Returns the number of right shifts needed to shift mask so
// that the mask occupies the low bits.
//
//---------------------------------------------------------------
static uint32 GetShift(uint32 mask)
{
	PRECONDITION(mask != 0);

	uint32 count = 0;

	while ((mask & 1) == 0) {
		mask >>= 1;
		++count;
	}

	return count;
}


//---------------------------------------------------------------
//
// GetBits
//
// Returns the number of consectutive 1 bits.
//
//---------------------------------------------------------------
static uint32 GetBits(uint32 mask)
{
	PRECONDITION(mask != 0);

	uint32 count = 0;
	uint32 bit = 0x80000000;
	
	while (bit != 0 && (mask & bit) == 0)
		bit >>= 1;
	ASSERT(bit != 0);
	
	while (bit != 0 && (mask & bit) != 0) {
		++count;
		bit >>= 1;
	}
	ASSERT(count > 0);

	return count;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XBaseImage
// ===================================================================================

//---------------------------------------------------------------
//
// XBaseImage::~XBaseImage
//
//---------------------------------------------------------------
XBaseImage::~XBaseImage()
{
	delete mColors;
}


//---------------------------------------------------------------
//
// XBaseImage::XBaseImage
//
//---------------------------------------------------------------
XBaseImage::XBaseImage()
{
	mDepth = 0;
	mSize  = kZeroSize;
	mRowBytes = 0;

	mColors = nil;
	
//	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XBaseImage::GetUnsafeBufferAt
//
// Returns a pointer to the pixel at loc in the buffer. If the depth 
// is less than 8 the caller will need to do some work to get the 
// correct bit.
//
//---------------------------------------------------------------
uint8* XBaseImage::GetUnsafeBufferAt(int32 h, int32 v)
{
	PRECONDITION(h >= 0 && h < mSize.width);
	PRECONDITION(v >= 0 && v < mSize.height);
	CHECK_INVARIANT;
	
	uint32 offset = v*mRowBytes + h*mDepth/8;
	uint8* buffer = this->GetUnsafeBuffer() + offset;
	
	POSTCONDITION(buffer != nil);

	return buffer;
}


//---------------------------------------------------------------
//
// XBaseImage::GetUnsafeBufferAt const
//
// Returns a pointer to the pixel at loc in the buffer. If the depth 
// is less than 8 the caller will need to do some work to get the 
// correct bit.
//
//---------------------------------------------------------------
const uint8* XBaseImage::GetUnsafeBufferAt(int32 h, int32 v) const
{
	PRECONDITION(h >= 0 && h < mSize.width);
	PRECONDITION(v >= 0 && v < mSize.height);
	CHECK_INVARIANT;
	
	uint32 offset = v*mRowBytes + h*mDepth/8;
	const uint8* buffer = this->GetUnsafeBuffer() + offset;
	
	POSTCONDITION(buffer != nil);

	return buffer;
}


//---------------------------------------------------------------
//
// XBaseImage::PixelToColor
//
//---------------------------------------------------------------
OSColor XBaseImage::PixelToColor(uint32 pixel) const
{
	PRECONDITION(mDepth >= 8);							
	CHECK_INVARIANT;
	
	OSColor color;

	if (mDepth == 8) {
		ASSERT(mColors != nil);
		color = mColors->GetColor((uint32) pixel).GetOSColor();

	} else {
#if WIN
		uint32 mask  = this->GetRedMask();
		uint32 shift = GetShift(mask);
		color.rgbRed = numeric_cast<uint8>((pixel & mask) >> shift);	

		mask  = this->GetGreenMask();
		shift = GetShift(mask);
		color.rgbGreen = numeric_cast<uint8>((pixel & mask) >> shift);

		mask  = this->GetBlueMask();
		shift = GetShift(mask);
		color.rgbBlue = numeric_cast<uint8>((pixel & mask) >> shift);

		if (mDepth == 16) {
			color.rgbRed   <<= 3;
			color.rgbGreen <<= 3;
			color.rgbBlue  <<= 3;
		}

#elif MAC
		uint32 mask  = this->GetRedMask();
		uint32 shift = GetShift(mask);
		color.red = numeric_cast<uint16>((pixel & mask) >> shift);	

		mask  = this->GetGreenMask();
		shift = GetShift(mask);
		color.green = numeric_cast<uint16>((pixel & mask) >> shift);

		mask  = this->GetBlueMask();
		shift = GetShift(mask);
		color.blue = numeric_cast<uint16>((pixel & mask) >> shift);

		if (mDepth == 16) {
			color.red   <<= 3;
			color.green <<= 3;
			color.blue  <<= 3;
		}

		color.red   = numeric_cast<uint16>((color.red << 8) + color.red);		// duplicate color byte so white maps to white
		color.green = numeric_cast<uint16>((color.green << 8) + color.green);
		color.blue  = numeric_cast<uint16>((color.blue << 8) + color.blue);
#endif
	}

	POSTCONDITION(true);

	return color;
}


//---------------------------------------------------------------
//
// XBaseImage::ColorToPixel
//
//---------------------------------------------------------------
uint32 XBaseImage::ColorToPixel(const OSColor& color) const
{
	PRECONDITION(mDepth >= 8);							
	CHECK_INVARIANT;
	
	uint32 pixel = 0;
	
	if (mDepth == 8) {
		DEBUGSTR("not implemented");						// $$$ use Graphics Gems inverse color lookup
		
	} else {
#if WIN
		uint32 mask  = this->GetRedMask();
		uint32 shift = GetShift(mask);
		uint32 bits  = GetBits(mask);
		int32 temp = (uint32) color.rgbRed >> (8 - bits);
		pixel = temp << shift;

		mask  = this->GetGreenMask();
		shift = GetShift(mask);
		bits  = GetBits(mask);
		temp = (uint32) color.rgbGreen >> (8 - bits);
		pixel |= temp << shift;

		mask  = this->GetBlueMask();
		shift = GetShift(mask);
		bits  = GetBits(mask);
		temp = (uint32) color.rgbBlue >> (8 - bits);
		pixel |= temp << shift;

#elif MAC
		uint32 mask  = this->GetRedMask();
		uint32 shift = GetShift(mask);
		uint32 bits  = GetBits(mask);
		uint32 temp = (uint32) color.red >> (16 - bits);
		pixel = temp << shift;

		mask  = this->GetGreenMask();
		shift = GetShift(mask);
		bits  = GetBits(mask);
		temp = (uint32) color.green >> (16 - bits);
		pixel |= temp << shift;

		mask  = this->GetBlueMask();
		shift = GetShift(mask);
		bits  = GetBits(mask);
		temp = (uint32) color.blue >> (16 - bits);
		pixel |= temp << shift;
#endif
	}
	
	POSTCONDITION(true);
	
	return pixel;
}


//---------------------------------------------------------------
//
// XBaseImage::GetPixelAt
//
//---------------------------------------------------------------
uint32 XBaseImage::GetPixelAt(int32 h, int32 v) const
{
	PRECONDITION(mDepth >= 8);
	CHECK_INVARIANT;
	
	const uint8* buffer = this->GetUnsafeBufferAt(h, v);
	
	uint32 result = 0;

	if (mDepth == 8)
		result = *buffer;
	
	else if (mDepth == 16)
		result = *((uint16 *) buffer);
	
	else if (mDepth == 24)
		result = (uint32) ((*((uint32 *) buffer)) >> 8);
	
	else if (mDepth == 32)
		result = *((uint32 *) buffer);	
	
	else
		DEBUGSTR("Bad depth in XBaseImage::GetPixelAt");
	
	POSTCONDITION(true);

	return result;
}


//---------------------------------------------------------------
//
// XBaseImage::SetPixelAt
//
//---------------------------------------------------------------
void XBaseImage::SetPixelAt(int32 h, int32 v, uint32 value)
{
	PRECONDITION(mDepth >= 8);
	CHECK_INVARIANT;
		
	if (mDepth == 8) {
		uint8* buffer = this->GetUnsafeBufferAt(h, v);
		*buffer = (uint8) value;

	} else if (mDepth == 16) {
		uint16* buffer = (uint16 *) this->GetUnsafeBufferAt(h, v);
		*buffer = (uint16) value;

	} else if (mDepth == 24) {
		uint32* buffer = (uint32 *) this->GetUnsafeBufferAt(h, v);
		*buffer &= 0x000F;
		*buffer |= value << 8;

	} else if (mDepth == 32) {
		uint32* buffer = (uint32 *) this->GetUnsafeBufferAt(h, v);
		*buffer = value;
	}	
	
	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// XBaseImage::Erase
//
//---------------------------------------------------------------
void XBaseImage::Erase(uint8 value)
{
	PRECONDITION(value <= 255);
	if (value != 0 && value != 255)
		PRECONDITION(mDepth <= 8);

	{
	XLocker lock(this);
		uint32 size = this->GetBufferSize();
		uint8* start = this->GetBuffer();
		SetMemory(start, value, size);
	}
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XBaseImage::Invariant 
//
//---------------------------------------------------------------
void XBaseImage::Invariant() const
{		
	ASSERT(mSize.width >= 0);
	ASSERT(mSize.height >= 0);

	ASSERT(mDepth == 1 || mDepth == 2 || mDepth == 4 || mDepth == 8 || mDepth == 16 || mDepth == 24 || mDepth == 32);
	ASSERT(mRowBytes >= mDepth*mSize.width/8);
	
	ASSERT(mLockCount >= 0);
	
	if (mDepth < 16)
		ASSERT(mColors != nil);
	else
		ASSERT(mColors == nil);
}


}	// namespace Whisper

