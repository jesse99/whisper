/*
 *  File:       WGDIUtils.h
 *  Summary:	Misc GDI utilities.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: WGDIUtils.h,v $
 *		Revision 1.4  2001/03/05 05:40:04  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:49:38  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:08:12  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XDebug.h>
#include <XPointer.h>

namespace Whisper {


// ===================================================================================
//	class WAcquireScreenDC
//!		Stack based class used to get an release an HDC for the screen.
// ===================================================================================
class GRAPHICS_EXPORT WAcquireScreenDC {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~WAcquireScreenDC()					{::ReleaseDC(nil, mDC);}
						
						WAcquireScreenDC()					{mDC = ::GetDC(nil);}
						
						operator HDC() const				{return mDC;}
						
//-----------------------------------
//	Member Data
//
protected:
	HDC		mDC;
};


// ===================================================================================
//	DIBs
// ===================================================================================
uint32 		CalcRowBytes(const BITMAPINFOHEADER* header);

const RGBQUAD* GetColors(const BITMAPINFOHEADER* header);
uint32 		GetNumColorEntries(const BITMAPINFOHEADER* header);
			// GetColors returns nil if the DIB doesn't have a color table.
			// GetNumColorEntries returns the number of colors in the color table (will 
			// be zero if DIB depth is greater than eight).
			
const uint8* GetPixels(const BITMAPINFOHEADER* header);
uint32 		GetPixelBytes(const BITMAPINFOHEADER* header);
			// GetPixels returns a pointer to the pixels in a packed DIB. (GetPixelBytes
			// will also work for unpacked DIBs).
			
bool 		IsCompressed(const BITMAPINFOHEADER* header);

bool 		EqualHeader(const BITMAPINFOHEADER* lhs, const BITMAPINFOHEADER* rhs);
bool 		EqualPixels(const BITMAPINFOHEADER* lhs, const BITMAPINFOHEADER* rhs);
			// EqualHeader returns true if the header and color tables match.
			// EqualPixels returns true if the pixels in packed DIBs match (this can
			// be slow for large DIBs).
			
XPointer 	CreatePackedDIB(const BITMAPINFO* header, const void* bits);
XPointer 	CreatePackedDIB(const BITMAPINFOHEADER* header, const void* bits);
			// Converts an unpacked DIB (eg a DIB-section) to a DIB where the bits
			// immediately follow the color table.


// ===================================================================================
//	Misc
// ===================================================================================
bool 		operator==(const RGBQUAD& lhs, const RGBQUAD& rhs);


// ===================================================================================
//	Inlines
// ===================================================================================
inline bool operator==(const RGBQUAD& lhs, const RGBQUAD& rhs)
{
	bool equal = lhs.rgbBlue == rhs.rgbBlue && lhs.rgbGreen == rhs.rgbGreen && lhs.rgbRed == rhs.rgbRed;
	
	return equal;
}

inline XPointer CreatePackedDIB(const BITMAPINFO* header, const void* bits)
{
	ASSERT(header != nil);
			
	return CreatePackedDIB(&header->bmiHeader, bits);
}


}	// namespace Whisper

