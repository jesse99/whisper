/*
 *  File:       IStringTableLoader.h
 *  Summary:   	Interface used to load strings into a StringTable.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IStringTableLoader.h,v $
 *		Revision 1.4  2001/02/17 10:23:42  jesjones
 *		IStringTableLoader::Load takes an XResource instead of an XURI.
 *		
 *		Revision 1.3  2000/12/14 08:25:18  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:07:20  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XCallbacks.h>
#include <XInterfacePtr.h>
#include <XStringUtils.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XResource;


// ===================================================================================
//	class IStringTableLoader
//!		Interface used to load strings into a StringTable.
// ===================================================================================
class IStringTableLoader : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		Load(const XResource& data, const XLocale& locale, StringTable& table) = 0;
						/**< Uses data and locale to append strings into the table. */
};

typedef XInterfacePtr<IStringTableLoader> IStringTableLoaderPtr;
typedef XInterfacePtr<const IStringTableLoader> IConstStringTableLoaderPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

