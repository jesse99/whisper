/*
 *  File:       MImageImporters.h
 *  Summary:	Classes that can be used to initialize XBaseImage's.
 *  Written by: Jesse Jones
 *
 *	Classes:	MPictImporter	- Data is taken from a Mac PICT.
 *
 *  Copyright � 1997-1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: MImageImporters.h,v $
 *		Revision 1.4  2001/03/05 05:39:20  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:48:40  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:15:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#if MAC
	#include <QuickDraw.h>
	#include <Resources.h>
#endif

#include <XImageImporter.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XResource;


// ===================================================================================
//	class MPictImporter
//!		Data is taken from a Mac PICT.
// ===================================================================================
class GRAPHICS_EXPORT MPictImporter : public XImageImporter {

	typedef XImageImporter Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~MPictImporter();
						
						MPictImporter(XResource* takeData);
						
	static 	XImageImporter* Create(XResource* takeData);

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Unpack(XBaseImage* sink, const XPoint& where = kZeroPt) const;

	virtual void 		Unpack(XBaseImage* sink, const XRect& srcRect, const XRect& dstRect) const;

protected:
	virtual void		Invariant() const;

//-----------------------------------
//	Member Data
//
protected:
	PicHandle	mPictH;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

