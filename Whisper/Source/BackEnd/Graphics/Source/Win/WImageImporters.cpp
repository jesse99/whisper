/*
 *  File:       WImageImporters.cpp
 *  Summary:	Classes that can be used to initialize pixel maps.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: WImageImporters.cpp,v $
 *		Revision 1.3  2001/04/21 03:32:22  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 12:09:05  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <WImageImporters.h>

#include <XColorTable.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XNumbers.h>
#include <XPixMap.h>
#include <XResource.h>
#include <XStringUtils.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const uint32 kRGBFormat       = 0;	// uncompressed
const uint32 kRLE8Format      = 1;	// RLE encoding with 8 bits per pixel
const uint32 kRLE4Format      = 2;	// RLE encoding with 4 bits per pixel
const uint32 kBITFIELDSFormat = 3;	// uncompressed color table entries are 32-bit triplets (only valid with 16 and 32 bit images)


// ===================================================================================
//	struct SRGBQuad
// ===================================================================================
struct SRGBQuad { 
	uint8	blue;
	uint8	green;
	uint8	red;
	uint8	reserved;
}; 


// ===================================================================================
//	struct SBitMapFileHeader
// ===================================================================================
#if MAC
	#pragma options align = mac68k	// BMP's on disk are misaligned...
#elif WIN
	#include <pshpack2.h>
#endif

struct SBitMapFileHeader { 
	uint16	type;				// should be 'BM'
	uint32	fileBytes;			// size of the entire file
	uint16	reserved1;			// should be zero
	uint16	reserved2;			// should be zero
	uint32	imageOffset;		// offset from the start of the file to the image bits
	
		void 	ByteSwap();
}; 

#if MAC
	#pragma options align = native
#elif WIN
	#include <poppack.h>
#endif


//---------------------------------------------------------------
//
// SBitMapFileHeader::ByteSwap
//
//---------------------------------------------------------------
void SBitMapFileHeader::ByteSwap()
{
#if BIG_ENDIAN
	Whisper::ByteSwap(type);
	Whisper::ByteSwap(fileBytes);
	Whisper::ByteSwap(reserved1);
	Whisper::ByteSwap(reserved2);
	Whisper::ByteSwap(imageOffset);
#endif
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	struct SBitMapInfoHeader
// ===================================================================================
struct SBitMapInfoHeader { 
	uint32		structBytes;		// size of this struct
	int32		width;				// width of the image in pixels 			
	int32		height; 			// height of the image in pixels (if negative the scan lines are top down)
	uint16		numPlanes; 			// number of bit planes (must be one)
	uint16		bitsPerPixel;		// image depth, can be 1, 4, 8, 16, 24, or 32 
	uint32		compressionType; 	// can be kRGBFormat, kRLE8Format, kRLE4Format, or kBITFIELDSFormat
	uint32		imageBytes; 		// size of the image data (may be zero)
	int32		horzResolution; 	// horizontal resolution in pixels per meter
	int32		vertResolution; 	// vertical resolution in pixels per meter
	uint32		numColors;			// number of colors in the color table (if zero and bitsPerPixel is less than 16 the maximum number of colors are used, if bitsPerPixel is >= 16 numColors specifies the size of the optimal color table)
	uint32		numImportantColors;	// number of "important" indices in the color table (zero means all the colors are important)
	
		void 	ByteSwap();
}; 


//---------------------------------------------------------------
//
// SBitMapInfoHeader::ByteSwap
//
//---------------------------------------------------------------
void SBitMapInfoHeader::ByteSwap()
{
#if BIG_ENDIAN
	Whisper::ByteSwap(structBytes);
	Whisper::ByteSwap(width);
	Whisper::ByteSwap(height);
	Whisper::ByteSwap(numPlanes);
	Whisper::ByteSwap(bitsPerPixel);
	Whisper::ByteSwap(compressionType);
	Whisper::ByteSwap(imageBytes);
	Whisper::ByteSwap(horzResolution);
	Whisper::ByteSwap(vertResolution);
	Whisper::ByteSwap(numColors);
	Whisper::ByteSwap(numImportantColors);
#endif
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class WBMPImporter
// ===================================================================================

//---------------------------------------------------------------
//
// WBMPImporter::~WBMPImporter
//
//---------------------------------------------------------------
WBMPImporter::~WBMPImporter()
{
	CALL_INVARIANT;
	
	mData->RemoveReference();
	mData = nil;
}


//---------------------------------------------------------------
//
// WBMPImporter::WBMPImporter
//
//---------------------------------------------------------------
WBMPImporter::WBMPImporter(XResource* data) 
{	
	PRECONDITION(data != nil);
	PRECONDITION(mColors == nil);

	try {
		XLocker lock(data);
		
		uint8* dataPtr = const_cast<uint8*>(data->GetPtr());	// cast away const so we can byte swap (note that these changes may not remain once this function exits) 
		uint32 bytes = data->GetSize();
		
		// Get a pointer to the info header
		SBitMapInfoHeader* info = nil;
	
		if (data->IsFromFile()) {
			SBitMapFileHeader* header = reinterpret_cast<SBitMapFileHeader*>(dataPtr);
			header->ByteSwap();
			
			if (header->type != 0x4D42)		// 'BM'
				throw std::domain_error(ToUTF8Str(LoadWhisperString(L"#1 has a bad file header type.", data->GetURI().GetAddress())));
	
			info = reinterpret_cast<SBitMapInfoHeader*>(dataPtr + sizeof(SBitMapFileHeader));
		
		} else
			info = reinterpret_cast<SBitMapInfoHeader*>(dataPtr);
			
		info->ByteSwap();
	
		// Do some sanity checks
		if (info->structBytes < 40)
			throw std::domain_error(ToUTF8Str(LoadWhisperString(L"#1 info header is too small.", data->GetURI().GetAddress())));
		
		if (info->width < 0)
			throw std::domain_error(ToUTF8Str(LoadWhisperString(L"#1 has a negative width.", data->GetURI().GetAddress())));
		
		if (info->height < 0 && (info->compressionType == kRLE8Format || info->compressionType == kRLE4Format))
			throw std::domain_error(ToUTF8Str(LoadWhisperString(L"#1 has an invalid compression type.", data->GetURI().GetAddress())));
		
		if (info->numPlanes != 1)
			throw std::domain_error(ToUTF8Str(LoadWhisperString(L"#1 has more than one bit plane.", data->GetURI().GetAddress())));	// docs say this must be one
		
		if (info->bitsPerPixel != 1 && info->bitsPerPixel != 4 && info->bitsPerPixel != 8 && info->bitsPerPixel != 16 && info->bitsPerPixel != 24 && info->bitsPerPixel != 32)
			throw std::domain_error(ToUTF8Str(LoadWhisperString(L"#1 has a bad depth.", data->GetURI().GetAddress())));
		
		if (info->compressionType != kRGBFormat && info->compressionType != kRLE8Format && info->compressionType != kRLE4Format && info->compressionType != kBITFIELDSFormat)
			throw std::domain_error(ToUTF8Str(LoadWhisperString(L"#1 has a bad compression type.", data->GetURI().GetAddress())));
		
		if (info->imageBytes >= bytes)
			throw std::domain_error(ToUTF8Str(LoadWhisperString(L"#1 has a bad image size.", data->GetURI().GetAddress())));
		
		uint32 rowBytes = ((((uint32) info->bitsPerPixel * info->width + 7) >> 3) + 3) & ~3L;
		if (info->imageBytes != 0 && info->imageBytes < info->height*rowBytes)
			throw std::domain_error(ToUTF8Str(LoadWhisperString(L"#1 has a bad image size.", data->GetURI().GetAddress())));
		
		// $$$ supporting compressed DIB's is a bit sticky since we can't guarantee that the
		// $$$ sink is a compressed DIB (however this wouldn't be too difficult to add: DIBs
		// $$$ are compressed using a simple RLE scheme).
		if (info->compressionType != kRGBFormat)
			throw std::domain_error(ToUTF8Str(LoadWhisperString(L"#1 is compressed.", data->GetURI().GetAddress())));

		// Initialize some member variables
		mData    = data;
		mWidth   = info->width;				
		mHeight  = Abs(info->height);
		mDepth   = info->bitsPerPixel;
		mTopDown = info->height < 0;
		
		// Initialize the color table and the image offset
		if (mDepth < 16) {
			uint32 numColors = info->numColors;
			if (numColors == 0)
				numColors = 1UL << mDepth;
	
			const SRGBQuad* colors = reinterpret_cast<const SRGBQuad*>((uint8*) info + info->structBytes);
			
#if MAC
			mColors = new XColorTable;
			mColors->SetSize(numColors);
			
			for (uint32 index = 0; index < numColors; ++index) {
				const SRGBQuad& color = colors[index];
				
				mColors->SetColor(index, XRGBColor(color.red/255.0, color.green/255.0, color.blue/255.0));
			}
#elif WIN
			mColors = new XColorTable((OSColor*) colors, numColors);
#endif

			mImageOffset = info->structBytes + numColors*sizeof(SRGBQuad);
			if (data->IsFromFile()) 
				mImageOffset += sizeof(SBitMapFileHeader);
		
		} else if (mDepth >= 16) {
			mImageOffset = info->structBytes;
		
			if (data->IsFromFile()) 
				mImageOffset += sizeof(SBitMapFileHeader);
		}

	} catch (...) {
		data->RemoveReference();
		throw;
	}		
	
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// WBMPImporter::Create								[static]
//
//---------------------------------------------------------------
XImageImporter* WBMPImporter::Create(XResource* takeData)
{
	XImageImporter* image = new WBMPImporter(takeData);
	
	return image;
}


//---------------------------------------------------------------
//
// WBMPImporter::Unpack
//
//---------------------------------------------------------------
void WBMPImporter::Unpack(XBaseImage* sink, const XPoint& where) const
{	
	PRECONDITION(sink != nil);
	PRECONDITION(sink->GetExtent().Contains(XRect(where, XSize(mWidth, mHeight))));
			
	XLocker lock1(sink);
	XLocker lock2(mData);
		
	uint8* dstStart = sink->GetBufferAt(where.x, where.y);
	const uint8* srcStart = mData->GetPtr() + mImageOffset;
	
	int32 rowBytes = (int32) ((((mDepth * mWidth + 7) >> 3) + 3) & ~3L);
	
	if (where == kZeroPt && sink->GetSize() == this->GetSize() && sink->GetRowBytes() == rowBytes) {
		BlockMoveData(srcStart, dstStart, (uint32) mHeight*rowBytes);
	
	} else {
		int32 offset = rowBytes;
		
		if (!mTopDown) {
			offset = -rowBytes;
			srcStart += (mHeight - 1)*rowBytes;
		}
		
		for (uint32 row = 0; row < mHeight; ++row) {
			BlockMoveData(srcStart, dstStart, (uint32) mWidth);	// this code will execute only if the sink isn't a DIB
			
			dstStart += sink->GetRowBytes();
			srcStart += offset;
		}
	}
}


}	// namespace Whisper

