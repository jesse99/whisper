/*
 *  File:       XImageExporters.cpp
 *  Summary:	Classes that can be used to write out XBaseImage's.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XImageExporters.cpp,v $
 *		Revision 1.9  2001/04/27 09:20:48  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.8  2001/04/27 04:33:20  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.7  2001/03/16 10:36:16  jesjones
 *		Updated the deallocate function for png 1.0.9
 *		
 *		Revision 1.6  2001/02/25 10:22:24  jesjones
 *		PMG exporter no longer swaps alpha byte.
 *		
 *		Revision 1.5  2000/12/10 04:10:00  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.4  2000/11/27 07:17:56  jesjones
 *		Tweaked for Pro 6.
 *		
 *		Revision 1.3  2000/11/20 05:40:43  jesjones
 *		Added a hack to always use the alpha channel on Windows.
 *		
 *		Revision 1.2  2000/11/09 12:38:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XImageExporters.h>

#include <png.h>
#include <XColorTable.h>
#include <XExceptions.h>
#include <XBaseImage.h>
#include <XFileSpec.h>
#include <XNumbers.h>
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
// PngError
//
//---------------------------------------------------------------
static void PngError(png_structp info, png_const_charp message)
{
	DEBUGSTR(L"libpng error: ", message ? message : "?");

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


//---------------------------------------------------------------
//
// FSp_fopen
//
// This is copied from MSL_C:MSL_MacOS:Src:FSp_fopen.c which isn't
// part of the MSL C library (you're supposed to insert the file
// into your project). $$ Unfortunately it doesn't compile with 
// the implicit arithmetic conversion warning on and there's some
// weird problem with the file_io.h include. So for now I just
// include a tweaked version of the file here. $$
//
//---------------------------------------------------------------
#if MAC
extern "C" int	__get_file_modes(const char * mode, std::__file_modes * modes);
static FILE* FSp_fopen(ConstFSSpecPtr spec, const char* open_mode, OSType creator, OSType type)
{
	std::__file_modes	mode;
	char				permission;
	OSErr				ioResult;
	short				refNum;
	FILE *				file;
	
	if (!__get_file_modes(open_mode, &mode))
		return(NULL);
	
	permission = (mode.io_mode == std::__read) ? fsRdPerm : fsRdWrPerm;
	
	ioResult = FSpOpenDF(spec, permission, &refNum);
	
	if (ioResult)
	{
		if (ioResult != fnfErr || mode.open_mode == std::__must_exist)
			return(NULL);
				
		if (!(ioResult = FSpCreate(spec, creator, type, smSystemScript)))
			ioResult = FSpOpenDF(spec, permission, &refNum);
		
		if (ioResult)
			return(NULL);
	}
	else if (mode.open_mode == std::__create_or_truncate)
	{
		ioResult = SetEOF(refNum, 0);
		
		if (ioResult)
		{
			FSClose(refNum);
			return(NULL);
		}
	}
	
	file = std::__handle_open(refNum, open_mode);
	
	if (!file)
		FSClose(refNum);
	
	return(file);
}
#endif

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XPNGExporter
// ===================================================================================

//---------------------------------------------------------------
//
// XPNGExporter::~XPNGExporter
//
//---------------------------------------------------------------
XPNGExporter::~XPNGExporter()
{	
	png_destroy_write_struct(&mPngPtr, &mInfoPtr);
}


//---------------------------------------------------------------
//
// XPNGExporter::XPNGExporter
//
//---------------------------------------------------------------
XPNGExporter::XPNGExporter(double gamma, bool saveAlpha) 
{	
	PRECONDITION(gamma >= 0.0);

	mPngPtr  = nil;	
	mInfoPtr = nil;
	
	mGamma     = gamma;
	mSaveAlpha = saveAlpha;
	
	// Allocate the private png struct.
	mPngPtr = png_create_write_struct_2(PNG_LIBPNG_VER_STRING, 
									   nil, PngError, PngWarning,
									   nil, PngAllocate, PngFree);
	ThrowIfNil(mPngPtr);
				
	// Create a struct that will be filled with info about the 
	// image.
	mInfoPtr = png_create_info_struct(mPngPtr);
	if (mInfoPtr == nil) {
		png_destroy_write_struct(&mPngPtr, nil);
		ThrowIfNil(mInfoPtr);
	}
	
	// lpng expects to use longjmp when it has a fatal error so 
	// we'll call setjmp to register the location to longjmp to
	// (it would be possible to have PngError throw an exception
	// instead, but throwing an exception across C files is very
	// iffy).
	if (setjmp(mPngPtr->jmpbuf))
		throw std::runtime_error(ToUTF8Str((LoadWhisperString(L"Couldn't write out the PNG file."))));
}


//---------------------------------------------------------------
//
// XPNGExporter::Export
//
//---------------------------------------------------------------
void XPNGExporter::Export(const XFileSpec& spec, const XBaseImage* image, OSType creator, OSType type)
{	
	PRECONDITION(image != nil);
	UNUSED(creator);
	UNUSED(type);
		
#if WIN
	mSaveAlpha = true;		// $$$
#endif

#if MAC
	FILE* file = FSp_fopen(&spec.GetOSSpec(), "wb", creator, type);
#else
	FILE* file = std::fopen(ToPlatformStr(spec.GetOSSpec()).c_str(), "wb");
#endif
	if (file == nil)
		throw std::runtime_error(ToUTF8Str((LoadWhisperString(L"Couldn't open '#1'.", spec.GetName()))));
		
	try {
		int32 depth = image->GetDepth();
		
		// Tell the png_struct_def which file we're writing to.
		png_init_io(mPngPtr, file);
	
  		// Initialize the mandatory fields in png_info_struct. 
  		int colorType = PNG_COLOR_TYPE_PALETTE;
  		if (depth == 32)
  			colorType = mSaveAlpha ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB;
  			
		png_set_IHDR(mPngPtr, mInfoPtr, 
					 (uint32) image->GetWidth(), 
					 (uint32) image->GetHeight(),
					 depth == 32 ? 8 : depth, 
					 colorType, 		
					 PNG_INTERLACE_NONE,
					 PNG_COMPRESSION_TYPE_DEFAULT, 
					 PNG_FILTER_TYPE_DEFAULT);
				
		png_color palette[256];							// PNG writes this out in png_write_info so we need to define it outside the if block
		if (depth <= 8) {
			const XColorTable* colors = image->GetColors();
			uint32 numColors = colors->GetSize();
			
			for (uint32 i = 0; i < numColors; ++i) {
				XRGBColor color = colors->GetColor(i);
				
				palette[i].red   = numeric_cast<uint8>(255.0*color.red);
				palette[i].green = numeric_cast<uint8>(255.0*color.green);
				palette[i].blue  = numeric_cast<uint8>(255.0*color.blue);
			}
			
			png_set_PLTE(mPngPtr, mInfoPtr, palette, (int) numColors);
		}

  		// Initialize some optional fields in png_info_struct. 
		if (!Equal(mGamma, 0.0)) {
			if (Equal(mGamma, 0.0)) {
#if WIN
				const char* str = std::getenv("SCREEN_GAMMA");
				if (str != nil)
					mGamma = std::atof(str);
				else
					mGamma = 2.2; 
#elif MAC
				mGamma = 1.7;							// $$$ presumbably there's a way to get this using ColorSync, but the documentation isn't much help...
#endif		
			}
			png_set_gAMA(mPngPtr, mInfoPtr, mGamma);
		}
			
		png_time time;
		png_convert_from_time_t(&time, std::time(nil));
   		png_set_tIME(mPngPtr, mInfoPtr, &time);
   		
   		// Write out the header.
    	png_write_info(mPngPtr, mInfoPtr);	
    	
		// Tell the PNG library about our pixels
#if !BIG_ENDIAN
		if (depth > 8)
			png_set_swap(mPngPtr);
#endif
//		png_set_swap_alpha(mPngPtr);					// Win and Mac have alpha in the high byte
		
		if (!mSaveAlpha)
			png_set_filler(mPngPtr, 0, PNG_FILLER_BEFORE);	// get rid of the alpha byte

#if WIN
		png_set_bgr(mPngPtr);							// Windows uses blue, green, red ordering
#endif

		// Write the pixels out
		XLocker lock(image);
		
		for (int32 row = 0; row < image->GetHeight(); ++row) {
			const uint8* pixels = image->GetBufferAt(0, row);
			png_write_row(mPngPtr, const_cast<uint8*>(pixels));
		}

		// Clean up
		png_write_end(mPngPtr, mInfoPtr);

		std::fclose(file);

	} catch (...) {
		std::fclose(file);
		throw;
	}
}


}	// namespace Whisper

