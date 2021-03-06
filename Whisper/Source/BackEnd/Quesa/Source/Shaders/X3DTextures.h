/*
 *  File:       X3DTextures.h
 *  Summary:	QuickDraw 3D texture map objects.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DPixmapTexture		   - Used to map a pixmap onto surfaces.
 *				X3DMipmapTexture		   - A pixmap shader that's optimized for large textures.
 *				X3DCompressedPixmapTexture - A pixmap shader that uses a QuickTime compressed texture.
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DTextures.h,v $
 *		Revision 1.4  2001/04/27 09:15:31  jesjones
 *		Compressed pixmap texture is Quicktime only.
 *		
 *		Revision 1.3  2000/12/13 08:55:02  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:22:13  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <X3DBitmaps.h>
#include <X3DTexture.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DPixmapTexture
//!		Used to map a pixmap onto surfaces.
// ===================================================================================
class QUESA_EXPORT X3DPixmapTexture : public X3DTexture {

	typedef X3DTexture Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DPixmapTexture();
						
						X3DPixmapTexture(const SPixmapStorage& pixmap);

						X3DPixmapTexture(const TQ3StoragePixmap& pixmap);

						X3DPixmapTexture(TQ3TextureObject object);
						
	virtual X3DPixmapTexture* Clone() const;
		
//-----------------------------------
//	New API
//
public:
			SPixmapStorage GetPixmap() const;
						
			void 		SetPixmap(const SPixmapStorage& pixmap);

			void 		SetPixmap(const TQ3StoragePixmap& pixmap);

	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DPixmapTexture. */
};


// ===================================================================================
//	class X3DMipmapTexture
//!		A pixmap shader that's optimized for large textures.
// ===================================================================================
class QUESA_EXPORT X3DMipmapTexture : public X3DTexture {

	typedef X3DTexture Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DMipmapTexture();
						
						X3DMipmapTexture(const TQ3Mipmap& mipmap);

						X3DMipmapTexture(TQ3TextureObject object);
						
	virtual X3DMipmapTexture* Clone() const;
		
//-----------------------------------
//	New API
//
public:
			TQ3Mipmap 	GetMipmap() const;
						
			void 		SetMipmap(const TQ3Mipmap& mipmap);

	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DMipmapTexture. */
};


// ===================================================================================
//	class X3DCompressedPixmapTexture
//!		A pixmap shader that uses a QuickTime compressed texture.
// ===================================================================================
#if QUESA_SUPPORT_QUICKTIME
class QUESA_EXPORT X3DCompressedPixmapTexture : public X3DTexture {

	typedef X3DTexture Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DCompressedPixmapTexture();
						
						X3DCompressedPixmapTexture(const TQ3CompressedPixmap& pixmap);
						/**< Requires QD3D 1.6 */

						X3DCompressedPixmapTexture(TQ3TextureObject object);
						
	virtual X3DCompressedPixmapTexture* Clone() const;
		
//-----------------------------------
//	New API
//
public:
			TQ3CompressedPixmap GetPixmap() const;
						
			void 		SetPixmap(const TQ3CompressedPixmap& pixmap);

	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DCompressedPixmapTexture. */
};
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
