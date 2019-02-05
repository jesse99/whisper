/*
 *  File:       XImageExporter.h
 *  Summary:	Abstract base class that can be used to write XBaseImage's out.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first): 
 *
 *		$Log: XImageExporter.h,v $
 *		Revision 1.4  2001/03/05 05:41:55  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:53:13  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:38:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XBaseImage;
class XFileSpec;


// ===================================================================================
//	class XImageExporter
//!		Abstract base class that can be used to write XBaseImage's out.
// ===================================================================================
class GRAPHICS_EXPORT XImageExporter {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XImageExporter();
						
						XImageExporter();
						
private:
						XImageExporter(const XImageExporter& rhs);

			XImageExporter& operator=(const XImageExporter& rhs);
						
//-----------------------------------
//	API
//
public:
	virtual void 		Export(const XFileSpec& spec, const XBaseImage* image, OSType creator, OSType type) = 0;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

