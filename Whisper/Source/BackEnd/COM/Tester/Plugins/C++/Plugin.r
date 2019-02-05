/*
 *  File:       Plugin.r
 *  Summary:   	Mac plugin resources
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: Plugin.r,v $
 *		Revision 1.2  2000/11/09 09:22:59  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	12/02/99	JDJ		Created
 */

#include <Types.r>


// Note that resource ids below 256 are reserved for Whisper.

// ===================================================================================
//	XML Resources
// ===================================================================================
read 'bXML' (1024, "Bosses") "Bosses.bXML";
 