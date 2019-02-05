/*
 *  File:		XLoadXML.h
 *  Summary:	Streams in an XXMLDoc from an XURI.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XLoadXML.h,v $
 *		Revision 1.3  2001/02/17 10:24:27  jesjones
 *		Added an overload that takes an XResource.
 *		
 *		Revision 1.2  2000/11/09 12:41:57  jesjones
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
class XResource;
class XURI;
class XXMLDoc;


// ===================================================================================
//	Global Functions
// ===================================================================================
FILES_EXPORT void 	Load(XXMLDoc& xml, const XURI& uri);
FILES_EXPORT void 	Load(XXMLDoc& xml, const XResource& data);
					// Streams in an XXMLDoc that was saved with the Save function in
					// XSaveXML.h 


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

