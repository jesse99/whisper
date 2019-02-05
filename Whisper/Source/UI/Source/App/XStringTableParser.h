/*
 *  File:       XStringTableParser.h
 *  Summary:   	Builds a string table used for localization from an XML resource.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XStringTableParser.h,v $
 *		Revision 1.2  2000/11/09 12:53:40  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <3>	 7/14/99	JDJ		Redesigned so that the XML parser doesn't need to
 *									be invoked in release builds.
 *		 <2>	 3/19/99	JDJ		ParseXMLStringTable takes a URI instead of a name.
 *		 <1>	 2/15/99	JDJ		Created
 */

#pragma once

#include <set>

#include <XStringUtils.h>

namespace Whisper {


//-----------------------------------
//	Forward References
//
class XURI;


// ===================================================================================
//	Global Functions
// ===================================================================================
void 	ReadXMLStringTable(const XURI& uri, const XLocale& locale, StringTable& table, std::set<XLocale>* locales);
		// Streams in an XXMLDoc using the specified uri and uses it and the specified
		// locale to initialize table and locales (which may be nil). In debug builds
		// the uri will typically point to the outSpec passed into XXMLDoc::Save. In 
		// release builds the uri will typically point to a resource created using the
		// outSpec.


}	// namespace Whisper
