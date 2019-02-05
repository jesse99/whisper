/*
 *  File:       WGDIUtils.cpp
 *  Summary:	Misc GDI utilities.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: WGDIUtils.cpp,v $
 *		Revision 1.3  2001/04/21 03:32:09  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 12:08:08  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XWhisperHeader.h>
#include <WGDIUtils.h>

#include <WSystemInfo.h>
#include <XMemUtils.h>
#include <XNumbers.h>

namespace Whisper {


// ===================================================================================
//	DIBs
// ===================================================================================

//---------------------------------------------------------------
//
// CalcRowBytes
//
// DIBs are aligned to 4-byte boundaries.
//
//---------------------------------------------------------------
uint32 CalcRowBytes(const BITMAPINFOHEADER* header)
{
	PRECONDITION(header != nil);
	PRECONDITION(header->biSize >= sizeof(BITMAPINFOHEADER));	// do some simple sanity checking
	PRECONDITION(header->biWidth >= 0);
	PRECONDITION(header->biBitCount == 1 || header->biBitCount == 4 || header->biBitCount == 8 || header->biBitCount == 16 || header->biBitCount == 24 || header->biBitCount == 32);
	PRECONDITION(header->biCompression == BI_RGB || header->biCompression == BI_BITFIELDS);	// compressed DIBs have variable width rows

	uint32 rowBytes = ((((uint32) header->biBitCount * header->biWidth + 7) >> 3) + 3) & ~3L;
	
	return rowBytes;
}


//---------------------------------------------------------------
//
// GetColors
//
//---------------------------------------------------------------
const RGBQUAD* GetColors(const BITMAPINFOHEADER* header)
{
	PRECONDITION(header != nil);
	PRECONDITION(header->biSize >= sizeof(BITMAPINFOHEADER));	// do some simple sanity checking
	PRECONDITION(header->biWidth >= 0);
	PRECONDITION(header->biBitCount == 1 || header->biBitCount == 4 || header->biBitCount == 8 || header->biBitCount == 16 || header->biBitCount == 24 || header->biBitCount == 32);

	const RGBQUAD* colors = nil;
	if (GetNumColorEntries(header) > 0)
		colors = reinterpret_cast<const RGBQUAD*>((uint8*) header + header->biSize);
		
	return colors;
}


//---------------------------------------------------------------
//
// GetNumColorEntries
//
//---------------------------------------------------------------
uint32 GetNumColorEntries(const BITMAPINFOHEADER* header)
{
	PRECONDITION(header != nil);
	PRECONDITION(header->biSize >= sizeof(BITMAPINFOHEADER));	// do some simple sanity checking
	PRECONDITION(header->biWidth >= 0);
	PRECONDITION(header->biBitCount == 1 || header->biBitCount == 4 || header->biBitCount == 8 || header->biBitCount == 16 || header->biBitCount == 24 || header->biBitCount == 32);

	uint32 count = 0;										
	if (header->biBitCount <= 8)
		count = header->biClrUsed > 0 ? header->biClrUsed : (1UL << header->biBitCount);
		
	return count;
}

		
//---------------------------------------------------------------
//
// GetPixels
//
//---------------------------------------------------------------
const uint8* GetPixels(const BITMAPINFOHEADER* header)
{
	PRECONDITION(header != nil);
	PRECONDITION(header->biSize >= sizeof(BITMAPINFOHEADER));	// do some simple sanity checking
	PRECONDITION(header->biWidth >= 0);
	PRECONDITION(header->biBitCount == 1 || header->biBitCount == 4 || header->biBitCount == 8 || header->biBitCount == 16 || header->biBitCount == 24 || header->biBitCount == 32);
	
	uint32 numColors = GetNumColorEntries(header);
	
	const uint8* pixels = (uint8*) header + header->biSize + numColors*sizeof(RGBQUAD);
	
	return pixels;
}


//---------------------------------------------------------------
//
// GetPixelBytes
//
//---------------------------------------------------------------
uint32 GetPixelBytes(const BITMAPINFOHEADER* header)
{
	PRECONDITION(header != nil);
	PRECONDITION(header->biSize >= sizeof(BITMAPINFOHEADER));	// do some simple sanity checking
	PRECONDITION(header->biWidth >= 0);
	PRECONDITION(header->biBitCount == 1 || header->biBitCount == 4 || header->biBitCount == 8 || header->biBitCount == 16 || header->biBitCount == 24 || header->biBitCount == 32);
	
	uint32 rowBytes = CalcRowBytes(header);
	uint32 bytes = rowBytes*Abs(header->biHeight);
	
	return bytes;
}

			
//---------------------------------------------------------------
//
// IsCompressed
//
//---------------------------------------------------------------
bool IsCompressed(const BITMAPINFOHEADER* header)
{
	PRECONDITION(header != nil);
	PRECONDITION(header->biSize >= sizeof(BITMAPINFOHEADER));	// do some simple sanity checking
	PRECONDITION(header->biWidth >= 0);
	PRECONDITION(header->biBitCount == 1 || header->biBitCount == 4 || header->biBitCount == 8 || header->biBitCount == 16 || header->biBitCount == 24 || header->biBitCount == 32);
	
	bool compressed = header->biCompression == BI_RLE8 || header->biCompression == BI_RLE4;
	
	return compressed;
}


//---------------------------------------------------------------
//
// EqualHeader
//
//---------------------------------------------------------------
bool EqualHeader(const BITMAPINFOHEADER* lhs, const BITMAPINFOHEADER* rhs)
{
	bool equal = lhs == rhs;
	
	if (!equal && lhs != nil && rhs != nil) {
		ASSERT(lhs->biSize >= sizeof(BITMAPINFOHEADER));	// do some simple sanity checking
		ASSERT(lhs->biWidth >= 0);
		ASSERT(lhs->biBitCount == 1 || lhs->biBitCount == 4 || lhs->biBitCount == 8 || lhs->biBitCount == 16 || lhs->biBitCount == 24 || lhs->biBitCount == 32);
		ASSERT(rhs->biSize >= sizeof(BITMAPINFOHEADER));	
		ASSERT(rhs->biWidth >= 0);
		ASSERT(rhs->biBitCount == 1 || rhs->biBitCount == 4 || rhs->biBitCount == 8 || rhs->biBitCount == 16 || rhs->biBitCount == 24 || rhs->biBitCount == 32);
		
		equal = lhs->biSize == rhs->biSize &&
				lhs->biWidth == rhs->biWidth &&
				Abs(lhs->biHeight) == Abs(rhs->biHeight) &&
				lhs->biPlanes == rhs->biPlanes &&
				lhs->biBitCount == rhs->biBitCount &&
				lhs->biCompression == rhs->biCompression;
//				lhs->biSizeImage == rhs->biSizeImage &&
//				lhs->biXPelsPerMeter == rhs->biXPelsPerMeter &&
//				lhs->biYPelsPerMeter == rhs->biYPelsPerMeter &&
//				lhs->biClrUsed == rhs->biClrUsed &&
//				lhs->biClrImportant == rhs->biClrImportant
		
		if (equal) {
			uint32 count1 = GetNumColorEntries(lhs);
			uint32 count2 = GetNumColorEntries(rhs);
			equal = count1 == count2;
				
			if (equal && count1 > 0) {
				const RGBQUAD* colors1 = GetColors(lhs);
				const RGBQUAD* colors2 = GetColors(rhs);
				
				for (uint32 i = 0; i < count1 && equal; ++i)
					equal = colors1[i] == colors2[i];
			}
		}
	}
	
	return equal;		
}


//---------------------------------------------------------------
//
// EqualPixels
//
//---------------------------------------------------------------
bool EqualPixels(const BITMAPINFOHEADER* lhs, const BITMAPINFOHEADER* rhs)
{
	bool equal = lhs == rhs;

	if (!equal && lhs != nil && rhs != nil) {
		ASSERT(lhs->biSize >= sizeof(BITMAPINFOHEADER));	// do some simple sanity checking
		ASSERT(lhs->biWidth >= 0);
		ASSERT(lhs->biBitCount == 1 || lhs->biBitCount == 4 || lhs->biBitCount == 8 || lhs->biBitCount == 16 || lhs->biBitCount == 24 || lhs->biBitCount == 32);
		ASSERT(rhs->biSize >= sizeof(BITMAPINFOHEADER));	
		ASSERT(rhs->biWidth >= 0);
		ASSERT(rhs->biBitCount == 1 || rhs->biBitCount == 4 || rhs->biBitCount == 8 || rhs->biBitCount == 16 || rhs->biBitCount == 24 || rhs->biBitCount == 32);
		
		const uint8* pixels1 = GetPixels(lhs);
		const uint8* pixels2 = GetPixels(rhs);

		uint32 bytes1 = GetPixelBytes(lhs);
		uint32 bytes2 = GetPixelBytes(rhs);

		equal = bytes1 == bytes2 && EqualMemory(pixels1, pixels2, bytes1);
	}
	
	return equal;		
}


//---------------------------------------------------------------
//
// CreatePackedDIB
//
//---------------------------------------------------------------
XPointer CreatePackedDIB(const BITMAPINFOHEADER* header, const void* bits)
{
	PRECONDITION(header != nil);
	PRECONDITION(header->biSize >= sizeof(BITMAPINFOHEADER));	// do some simple sanity checking
	PRECONDITION(header->biWidth >= 0);
	PRECONDITION(header->biBitCount == 1 || header->biBitCount == 4 || header->biBitCount == 8 || header->biBitCount == 16 || header->biBitCount == 24 || header->biBitCount == 32);
	PRECONDITION(bits != nil);
		
	uint32 numColors = GetNumColorEntries(header);

	uint32 headerBytes = header->biSize + numColors*sizeof(RGBQUAD);
	uint32 bufferBytes = GetPixelBytes(header);

	XPointer dib(headerBytes + bufferBytes);
	
	if (WSystemInfo::IsNT() || header->biHeight > 0) {
		BlockMoveData(header, dib.GetPtr(), headerBytes);
		BlockMoveData(bits, dib.GetPtr() + headerBytes, bufferBytes); 
	
	// Top down dibs seem to have problems on Windows. For example,
	// CreateDIBPatternBrushPt will return a ERROR_INVALID_PARAMETER
	// error. Of course the April 2000 MSDN doesn't mention this
	// restriction...
	} else { 
		BlockMoveData(header, dib.GetPtr(), headerBytes);
		BITMAPINFOHEADER* dibHdr = reinterpret_cast<BITMAPINFOHEADER*>(dib.GetPtr());
		dibHdr->biHeight = -dibHdr->biHeight;
		
		uint32 rowBytes = CalcRowBytes(header);
		uint32 srcOffset = 0;
		uint32 dstOffset = headerBytes + bufferBytes - rowBytes;
		
		const uint8* src = static_cast<const uint8*>(bits);
		for (uint32 i = 0; i < dibHdr->biHeight; ++i) {
			BlockMoveData(src + srcOffset, dib.GetPtr() + dstOffset, rowBytes);
			srcOffset += rowBytes;
			dstOffset -= rowBytes;
		}
	}
	
	return dib;
}


}	// namespace Whisper

