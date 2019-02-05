/*
 *  File:       XImageImporters.h
 *  Summary:	Classes that can be used to initialize XBaseImage's.
 *  Written by: Jesse Jones
 *
 *	Classes:	XPNGImporter		- Unpacks a PNG resource.
 *				XQuickTimeImporter	- Unpacks multiple file formats.
 *
 *  Copyright © 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XImageImporters.h,v $
 *		Revision 1.5  2001/06/03 10:05:16  jesjones
 *		Added a QuickTime image importer.
 *		
 *		Revision 1.4  2001/03/05 05:42:06  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:53:27  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:38:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XHandle.h>
#include <XImageImporter.h>

#if MAC
	#include <ImageCompression.h>
#endif

struct png_info_struct;
struct png_struct_def;

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif

#ifndef HAS_QUICKTIME
	#if MAC
		#define HAS_QUICKTIME	1
	#endif
#endif


//-----------------------------------
//	Forward References
//
class XResource;


// ===================================================================================
//	class XPNGImporter
//!		Unpacks a PNG resource.
// ===================================================================================
class GRAPHICS_EXPORT XPNGImporter : public XImageImporter {

	typedef XImageImporter Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XPNGImporter();
						
						XPNGImporter(XResource* takeData, double gamma = 0.0);
						/**< Two values for gamma are special cases: 0.0 means use the
						computer's default gamma correction, 1.0 means do no gamma
						correction. */
						
	static 	XImageImporter* Create(XResource* takeData);

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Unpack(XBaseImage* sink, const XPoint& where = kZeroPt) const;

//-----------------------------------
//	Internal Types
//
public:
	struct SReadBuffer {
		const XResource*	resource;
		uint32			 	offset;
		
			SReadBuffer(const XResource* r) : resource(r), offset(0) {}
	};

//-----------------------------------
//	Member Data
//
protected:
	XResource*			mData;	
	png_struct_def*		mPngPtr;
	png_info_struct* 	mInfoPtr;
	
	SReadBuffer			mBuffer;
};


// ===================================================================================
//	class XQuickTimeImporter
//!		Unpacks multiple file formats.
// ===================================================================================
#if HAS_QUICKTIME
class GRAPHICS_EXPORT XQuickTimeImporter : public XImageImporter {

	typedef XImageImporter Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XQuickTimeImporter();
						
						XQuickTimeImporter(XResource* takeData);
						/**< QuickTime 2.5 supports PICT, Photoshop, GIF, and JPEG.
						QuickTime 3 supports BMP, PNG, TGA, and TIFF. QuickTime 4
						supports FlashPix. */
						
	static 	XImageImporter* Create(XResource* takeData);

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Unpack(XBaseImage* sink, const XPoint& where = kZeroPt) const;

//-----------------------------------
//	Internal API
//
protected:
			XHandle 	DoGetDataRef(const XResource* data) const;

//-----------------------------------
//	Member Data
//
protected:
	XResource*				mData;	
	XHandle					mRef;
	GraphicsImportComponent	mImporter;
};
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

