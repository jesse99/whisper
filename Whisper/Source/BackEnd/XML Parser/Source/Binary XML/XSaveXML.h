/*
 *  File:		XSaveXML.h
 *  Summary:	Streams an XML file into a binary XML file.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XSaveXML.h,v $
 *		Revision 1.3  2001/03/17 07:43:07  jesjones
 *		Used the right export macro.
 *		
 *		Revision 1.2  2000/11/09 12:50:17  jesjones
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
class XFileSpec;


// ===================================================================================
//	Global Functions
// ===================================================================================
XML_EXPORT bool 	Save(const XFileSpec& inSpec, const XFileSpec& outSpec, bool compress = true);
					// inSpec should point to an XML file. The file will be parsed and the 
					// resulting XXMLDoc will then be saved to outSpec using a binary stream. 
					// If compress is true (and the binary data isn't tiny) the binary data 
					// will be zipped before it's saved. Returns true if the XML was reparsed
					// (which will only happen if inSpec's CRC differs from the saved CRC).
					// The binary XML file can then be loaded using the Load function in
					// XLoadXML.h
			

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

