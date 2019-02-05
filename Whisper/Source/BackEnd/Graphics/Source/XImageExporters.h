/*
 *  File:       XImageExporters.h
 *  Summary:	Classes that can be used to write out XBaseImage's.
 *  Written by: Jesse Jones
 *
 *	Classes:	XPNGExporter	- Writes out a PNG file.
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XImageExporters.h,v $
 *		Revision 1.4  2001/03/05 05:41:59  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:53:17  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:38:40  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XImageExporter.h>

struct png_info_struct;
struct png_struct_def;

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XPNGExporter
//!		Writes out a PNG file.
// ===================================================================================
class GRAPHICS_EXPORT XPNGExporter : public XImageExporter {

	typedef XImageExporter Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XPNGExporter();
						
						XPNGExporter(double gamma = 0.0, bool saveAlphaChannel = false);
						/**< Two values for gamma are special cases: 0.0 means use the
						computer's default gamma correction, 1.0 means do no gamma
						correction. */

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Export(const XFileSpec& spec, const XBaseImage* image, OSType creator, OSType type);

//-----------------------------------
//	Member Data
//
protected:
	png_struct_def*		mPngPtr;
	png_info_struct* 	mInfoPtr;
	
	double				mGamma;
	bool 				mSaveAlpha;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

