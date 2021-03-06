/*
 *  File:       X3DBitmaps.h
 *  Summary:	Wrappers for QuickDraw 3D bitmap structs.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DBitmaps.h,v $
 *		Revision 1.4  2001/04/14 07:27:30  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.3  2000/12/13 08:50:26  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:13:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <ImageCompression.h>			// for defaultDither constant
#include <Quesa.h>

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	struct SBitmap
//!		Quesa wrapper for a black and white bitmap (you are responsible for deleting the pixmap when it's no longer in use).
// ===================================================================================
struct QUESA_EXPORT SBitmap : public TQ3Bitmap {

public:						
						SBitmap();

						SBitmap(BitMap* bitmap);

						SBitmap(PixMapHandle pixmap);
						/**< Depth must be 1. */

						SBitmap(GWorldPtr world);

						SBitmap(TQ3Bitmap bitmap);
};


// ===================================================================================
//	struct SPixmap
//!		Quesa wrapper around an existing color pixmap (you are responsible for deleting the pixmap when it's no longer in use).
// ===================================================================================
struct QUESA_EXPORT SPixmap : public TQ3Pixmap {

public:						
						SPixmap(int16 depth = 16, bool useAlpha = false);

						SPixmap(PixMapHandle pixmap, bool useAlpha = false);
						/**< Depth must be 16 or 32. */

						SPixmap(GWorldPtr world, bool useAlpha = false);

						SPixmap(TQ3Pixmap pixmap);
						
			void 		SetAlphaChannel(float value);
						/**< 0.0 == transparent, 1.0 == opaque */
};


// ===================================================================================
//	struct SPixmapStorage
//!		Quesa wrapper for a color pixmap where Quesa is responsible for deleting the memory.
// ===================================================================================
struct QUESA_EXPORT SPixmapStorage : public TQ3StoragePixmap {

public:						
						~SPixmapStorage();
						
						SPixmapStorage(int16 depth = 16, bool useAlpha = false);

						SPixmapStorage(const SPixmap& pixmap);

						SPixmapStorage(PicHandle pict, int32 width, int32 height, int16 depth, int16 dither = defaultDither, bool useAlpha = false);
						/**< Creates a pixmap of the specified size and copies pict into it. 
						dither can be defaultDither, forceDither, or suppressDither. */
				
						SPixmapStorage(TQ3StoragePixmap pixmap);
						
						SPixmapStorage(const SPixmapStorage& rhs);

			SPixmapStorage& operator=(const SPixmapStorage& rhs);

			void 		SetAlphaChannel(float value);
						/**< 0.0 == transparent, 1.0 == opaque */
						
protected:
			void 		DoInit(const SPixmap& pixmap);

			void 		DoInit(const TQ3StoragePixmap& pixmap);
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
