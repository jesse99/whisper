/*
 *  File:       WImageImporters.h
 *  Summary:	Classes that can be used to initialize pixel maps.
 *  Written by: Jesse Jones
 *
 *	Classes:	WBMPImporter	- Windows BMP/DIB resource.
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: WImageImporters.h,v $
 *		Revision 1.4  2001/03/05 05:40:07  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:49:42  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:09:11  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XImageImporter.h>

namespace Whisper {


//-----------------------------------
//	Forward References
//
class GRAPHICS_EXPORT XResource;


// ===================================================================================
//	class WBMPImporter
//!		Windows BMP/DIB resource.
// ===================================================================================
class GRAPHICS_EXPORT WBMPImporter : public XImageImporter {

	typedef XImageImporter Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~WBMPImporter();
						
						WBMPImporter(XResource* takeData);
						
	static 	XImageImporter* Create(XResource* takeData);

private:
						WBMPImporter(const WBMPImporter& rhs);

			WBMPImporter& operator=(const WBMPImporter& rhs);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Unpack(XBaseImage* sink, const XPoint& where = kZeroPt) const;

//-----------------------------------
//	Member Data
//
protected:
	XResource*	mData;
	uint32		mImageOffset;
	bool		mTopDown;
};


}	// namespace Whisper

