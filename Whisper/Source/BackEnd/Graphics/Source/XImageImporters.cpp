/*
 *  File:       XImageImporters.cpp
 *  Summary:	Classes that can be used to initialize XBaseImage's.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XImageImporters.cpp,v $
 *		Revision 1.12  2001/07/13 22:10:11  jesjones
 *		Tweaked gamma code.
 *		
 *		Revision 1.11  2001/06/03 10:05:11  jesjones
 *		Added a QuickTime image importer.
 *		
 *		Revision 1.10  2001/04/27 04:33:27  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.9  2001/03/16 10:36:20  jesjones
 *		Updated the deallocate function for png 1.0.9
 *		
 *		Revision 1.8  2001/02/25 10:49:35  jesjones
 *		Tweaked the PNG RGBA code for the Mac.
 *		
 *		Revision 1.7  2001/02/25 10:23:37  jesjones
 *		Tweaked the RGB and RGBA PNG import code for Windows.
 *		
 *		Revision 1.6  2001/02/25 08:22:27  jesjones
 *		Png importer no longer flips rows.
 *		
 *		Revision 1.5  2001/02/12 10:36:14  jesjones
 *		PNG files are no longer imported upside down on Windows (fix from Thomas Powers).
 *		
 *		Revision 1.4  2000/12/10 04:10:06  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.3  2000/11/20 05:40:14  jesjones
 *		Added std:: to a few function calls.
 *		
 *		Revision 1.2  2000/11/09 12:38:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XImageImporters.h>

#include <new>

#if HAS_QUICKTIME
	#include <QuickTimeComponents.h>
#endif

#include <png.h>
#include <XColorTable.h>
#include <XExceptions.h>
#include <XBaseImage.h>
#include <XLocker.h>
#include <XMiscUtils.h>
#include <XNumbers.h>
#include <XResource.h>
#include <XStringUtils.h>
#include <XTinyVector.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// PngAllocate
//
//---------------------------------------------------------------
static void* PngAllocate(png_structp info, png_size_t bytes)
{	
	void* ptr = new (std::nothrow) uint8[bytes];
	if (ptr == nil)
         png_error(info, ToUTF8Str(LoadWhisperString(L"Out of Memory.")).c_str());
	
	return ptr;
}


//---------------------------------------------------------------
//
// PngFree
//
//---------------------------------------------------------------
static void PngFree(png_structp info, png_voidp adr)
{
	UNUSED(info);

	delete [] reinterpret_cast<uint8*>(adr);
}


//---------------------------------------------------------------
//
// PngRead
//
//---------------------------------------------------------------
static void PngRead(png_structp info, png_bytep dst, png_size_t bytes)
{	
	PRECONDITION(dst != nil);
	
	XPNGImporter::SReadBuffer* src = reinterpret_cast<XPNGImporter::SReadBuffer*>(info->io_ptr);
	PRECONDITION(src != nil);
	PRECONDITION(src->offset + bytes <= src->resource->GetSize());
	
	BlockMoveData(src->resource->GetPtr() + src->offset, dst, (uint32) bytes);
	src->offset += bytes;
}


//---------------------------------------------------------------
//
// PngError
//
//---------------------------------------------------------------
static void PngError(png_structp info, png_const_charp message)
{
	DEBUGSTR("libpng error: ", message ? message : "?");

	longjmp(info->jmpbuf, 1);
}


//---------------------------------------------------------------
//
// PngWarning
//
//---------------------------------------------------------------
static void PngWarning(png_structp info, png_const_charp message)
{
	UNUSED(info);

	if (message != nil)
		TRACEFLOW("XWarning", "libpng warning: ", message, "\n");
	else
		TRACEFLOW("XWarning", "bogus libpng warning\n");
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XPNGImporter
// ===================================================================================

//---------------------------------------------------------------
//
// XPNGImporter::~XPNGImporter
//
//---------------------------------------------------------------
XPNGImporter::~XPNGImporter()
{
	CALL_INVARIANT;
	
	png_destroy_read_struct(&mPngPtr, &mInfoPtr, nil);
	mData->RemoveReference();

	mData = nil;
	mPngPtr = nil;
	mInfoPtr = nil;
}


//---------------------------------------------------------------
//
// XPNGImporter::XPNGImporter
//
//---------------------------------------------------------------
XPNGImporter::XPNGImporter(XResource* data, double gamma) : mBuffer(data)
{	
	PRECONDITION(data != nil);
	PRECONDITION(gamma >= 0.0);
	PRECONDITION(mColors == nil);
	
	mData    = data;
	mPngPtr  = nil;	
	mInfoPtr = nil;
	
	try {
		XLocker lock(mData);
		
		// Allocate the private png struct.
		mPngPtr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, 
										   nil, PngError, PngWarning,
										   nil, PngAllocate, PngFree);
		ThrowIfNil(mPngPtr);
			
		// We read from a memory buffer instead of using fread.
		png_set_read_fn(mPngPtr, &mBuffer, PngRead);
		
		// Create a struct that will be filled with info about the 
		// image.
		mInfoPtr = png_create_info_struct(mPngPtr);
		ThrowIfNil(mInfoPtr);
		
		// lpng expects to use longjmp when it has a fatal error so 
		// we'll call setjmp to register the location to longjmp to
		// (it would be possible to have PngError throw an exception
		// instead, but throwing an exception across C files is very
		// iffy).
		volatile XResource*	temp = mData;
		if (setjmp(mPngPtr->jmpbuf)) {
			XResource* temp2 = const_cast<XResource*>(temp);	// $$ required by MSVC 5
			throw std::runtime_error(ToUTF8Str((LoadWhisperString(L"<#1> info couldn't be read.", temp2->GetURI().GetAddress()))));
		}

		// Initialize the info pointer.
	    png_read_info(mPngPtr, mInfoPtr);

		// Find out what the gamma correction should be.
		double screenGamma = 1.0;
		if (gamma > 1.0) {
			screenGamma = gamma;
			
		} else if (Equal(gamma, 0.0)) {
#if WIN
			const char* str = std::getenv("SCREEN_GAMMA");
			if (str != nil)
				screenGamma = std::atof(str);
			else
				screenGamma = 2.2; 
#elif MAC
			screenGamma = 1.8;				// $$$ presumbably there's a way to get this using ColorSync, but the documentation isn't much help...
#endif
		}
		
		// Tell lpng how to handle gamma correction.
		if (!Equal(screenGamma, 1.0)) {
			int intent;
			double imageGamma;
			
			if (png_get_sRGB(mPngPtr, mInfoPtr, &intent))
				png_set_sRGB(mPngPtr, mInfoPtr, intent);
			else 
				if (png_get_gAMA(mPngPtr, mInfoPtr, &imageGamma))
					png_set_gamma(mPngPtr, screenGamma, imageGamma);
				else
					png_set_gamma(mPngPtr, screenGamma, 0.45455);
		}	

		// Initialize our member variables using the info pointer.
		png_uint_32 width, height;
		int bitDepth, colorType;
		png_get_IHDR(mPngPtr, mInfoPtr, &width, &height, &bitDepth, &colorType, nil, nil, nil);
					 
		mWidth  = numeric_cast<int32>(width);
		mHeight = numeric_cast<int32>(height);
		
		if (bitDepth == 16) {
			bitDepth /= 2;
			png_set_strip_16(mPngPtr);			// neither Mac nor Windows support 16-bit channels
			
#if !BIG_ENDIAN
			png_set_swap(mPngPtr);				// png files are always big endian
#endif
		}
		
		switch (colorType) {
			case PNG_COLOR_TYPE_GRAY:
			case PNG_COLOR_TYPE_GRAY_ALPHA:
				{
				mDepth = bitDepth;

				uint32 numColors = 1UL << bitDepth;
				XTinyVector<OSColor> colors(numColors);
				
				uint8 value = 0;
				uint8 delta = numeric_cast<uint8>(255/(numColors - 1));
				for (uint32 index = 0; index < numColors; ++index) {
#if MAC
					colors[index].red   = numeric_cast<uint16>((value << 8) + value);
					colors[index].green = numeric_cast<uint16>((value << 8) + value);
					colors[index].blue  = numeric_cast<uint16>((value << 8) + value);
#elif WIN
					colors[index].rgbRed   = value;
					colors[index].rgbGreen = value;
					colors[index].rgbBlue  = value;
					
					colors[index].rgbReserved  = 0;
#endif

					value += delta;
				}
				
				mColors = new XColorTable(colors.buffer(), numColors);
				}
				break;
			
			case PNG_COLOR_TYPE_PALETTE:
				{
				mDepth = bitDepth;

				uint32 numColors = 0;
				png_color* palette = nil;
				png_uint_32 found = png_get_PLTE(mPngPtr, mInfoPtr, &palette, (int*) &numColors);
				if (found) {
//					uint16* histogram;
//					png_get_hIST(mPngPtr, mInfoPtr, &histogram);
					
//					png_set_dither(mPngPtr, palette, (int) numColors, (int) numColors, histogram, 0);

					XTinyVector<OSColor> colors(numColors);
					for (uint32 index = 0; index < numColors; ++index) {
#if MAC
						colors[index].red   = numeric_cast<uint16>((palette[index].red << 8) + palette[index].red);
						colors[index].green = numeric_cast<uint16>((palette[index].green << 8) + palette[index].green);
						colors[index].blue  = numeric_cast<uint16>((palette[index].blue << 8) + palette[index].blue);
#elif WIN
						colors[index].rgbRed   = palette[index].red;
						colors[index].rgbGreen = palette[index].green;
						colors[index].rgbBlue  = palette[index].blue;
						
						colors[index].rgbReserved  = 0;
#endif
					}
					
					mColors = new XColorTable(colors.buffer(), numColors);
				
				} else
					throw std::runtime_error(ToUTF8Str((LoadWhisperString(L"<#1> doesn't have a palette.", mData->GetURI().GetAddress()))));
				}
				break;
			
			case PNG_COLOR_TYPE_RGB:
				mDepth = 4*bitDepth;		
				mColors = nil;			
#if WIN
				png_set_bgr(mPngPtr);							// Windows uses blue, green, red ordering
				png_set_filler(mPngPtr, 0, PNG_FILLER_AFTER);	// unpack pixels into long words
#else
				png_set_filler(mPngPtr, 0, PNG_FILLER_BEFORE);	// unpack pixels into long words
#endif
				break;
			
			case PNG_COLOR_TYPE_RGB_ALPHA:
				mDepth = 4*bitDepth;
				mColors = nil;			
#if WIN
				png_set_bgr(mPngPtr);							// Windows uses blue, green, red ordering
#else
				png_set_swap_alpha(mPngPtr);					// Win and Mac want alpha in the high byte
#endif
				break;
		}
		
	} catch (...) {
		if (mPngPtr != nil)
			if (mInfoPtr != nil)
				png_destroy_read_struct(&mPngPtr, &mInfoPtr, nil);
			else
				png_destroy_read_struct(&mPngPtr, nil, nil);
		mData->RemoveReference();
		throw;
	}		
	
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XPNGImporter::Create									[static]
//
//---------------------------------------------------------------
XImageImporter* XPNGImporter::Create(XResource* takeData)
{
	XImageImporter* image = new XPNGImporter(takeData);
	
	return image;
}


//---------------------------------------------------------------
//
// XPNGImporter::Unpack (XBaseImage*, XPoint)
//
//---------------------------------------------------------------
void XPNGImporter::Unpack(XBaseImage* sink, const XPoint& where) const
{	
	PRECONDITION(sink != nil);
	PRECONDITION(sink->GetExtent().Contains(XRect(where, XSize(mWidth, mHeight))));
	PRECONDITION(sink->GetDepth() == this->GetDepth());
							
	XLocker lock1(sink);
	XLocker lock2(mData);
		
	volatile XResource*	temp = mData;
	if (setjmp(mPngPtr->jmpbuf)) {
		XResource* temp2 = const_cast<XResource*>(temp);	// $$ required by MSVC 5
		throw std::runtime_error(ToUTF8Str((LoadWhisperString(L"<#1> couldn't be read.", temp2->GetURI().GetAddress()))));
	}

	XTinyVector<uint8*> rowPointers((uint32) mHeight);
	
	uint8* rowStart = sink->GetBufferAt(where.x, where.y);
	for (uint32 index = 0; index < (uint32) mHeight; ++index) {
		rowPointers[index] = rowStart;
		rowStart += sink->GetRowBytes();
	}
	
	png_read_image(mPngPtr, rowPointers.buffer());
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XQuickTimeImporter
// ===================================================================================

#if HAS_QUICKTIME

//---------------------------------------------------------------
//
// XQuickTimeImporter::~XQuickTimeImporter
//
//---------------------------------------------------------------
XQuickTimeImporter::~XQuickTimeImporter()
{
	CALL_INVARIANT;

	CloseComponent(mImporter);
}


//---------------------------------------------------------------
//
// XQuickTimeImporter::XQuickTimeImporter
//
//---------------------------------------------------------------
XQuickTimeImporter::XQuickTimeImporter(XResource* data)
{	
	PRECONDITION(data != nil);
	PRECONDITION(mColors == nil);
	
	mData = data;
	mRef = this->DoGetDataRef(data);
	mImporter = nil;

	ImageDescriptionHandle description = nil;

	try {
		// get the QuickTime importer
		OSStatus err = GetGraphicsImporterForDataRef(mRef.GetHandle(), HandleDataHandlerSubType, &mImporter);
		ThrowIfOSErr(err);
		ThrowIfNil(mImporter);

		// get the image info
		err = GraphicsImportGetImageDescription(mImporter, &description);
		ThrowIfOSErr(err);

		mWidth  = (**description).width;
		mHeight = (**description).height;
		mDepth  = (**description).depth;
		
		if (mDepth < 16) {
			CTabHandle colors = nil;
			err = GetImageDescriptionCTable(description, &colors);
			ThrowIfOSErr(err);
			mColors = new XColorTable(colors);
		}
	
	} catch (...) {
		if (description != nil)
			DisposeHandle((Handle) description);

		if (mImporter != nil)
			CloseComponent(mImporter);
			
		throw;
	}
		
	DisposeHandle((Handle) description);
	
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XQuickTimeImporter::Create							[static]
//
//---------------------------------------------------------------
XImageImporter* XQuickTimeImporter::Create(XResource* takeData)
{
	XImageImporter* image = new XQuickTimeImporter(takeData);
	
	return image;
}


//---------------------------------------------------------------
//
// XQuickTimeImporter::Unpack (XBaseImage*, XPoint)
//
//---------------------------------------------------------------
void XQuickTimeImporter::Unpack(XBaseImage* sink, const XPoint& where) const
{	
	PRECONDITION(sink != nil);
	PRECONDITION(sink->GetExtent().Contains(XRect(where, XSize(mWidth, mHeight))));
							
	XLocker lock1(sink);

	// point the importer at the sink's port
	OSStatus err = GraphicsImportSetGWorld(mImporter, sink->GetOSContext(), nil);
	ThrowIfOSErr(err);
	
	// set the dest rect
	Rect dstRect = {(int16) where.y, (int16) where.x, (int16) (where.y + mHeight), (int16) (where.x + mWidth)};
	err = GraphicsImportSetDestRect(mImporter, &dstRect);
	ThrowIfOSErr(err);
	
	// tell the importer to draw the image
	err = GraphicsImportDraw(mImporter);
	ThrowIfOSErr(err);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XQuickTimeImporter::DoGetDataRef
//
//---------------------------------------------------------------
XHandle XQuickTimeImporter::DoGetDataRef(const XResource* data) const
{
	ASSERT(data != nil);
	
	Handle result = nil;
	
	// add a refrence to the data handle
	Handle temp = data->GetHandle().GetHandle();
	OSErr err = PtrToHand(&temp, &result, sizeof(Handle));
	ThrowIfOSErr(err);
	
	// add a tag for the file name
	const XURI& uri = data->GetURI();
	std::wstring name = uri.GetFile();
	if (!name.empty()) {
		std::string temp = ToPlatformStr(name);
		
		err = PtrAndHand(temp.c_str(), result, (int32) temp.length()+1L);
		ThrowIfOSErr(err);
	}
 
 	return result;
}
#endif	// HAS_QUICKTIME


}		// namespace Whisper

