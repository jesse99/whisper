/*
 *  File:       IDocReader.h
 *  Summary:   	XPersistDoc will iterate over the document boss to find a reader it can use. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: IDocReader.h,v $
 *		Revision 1.4  2001/03/05 05:37:15  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:20:51  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:01:09  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFileSpec;


// ===================================================================================
//	class IDocReader
//!		XPersistDoc will iterate over the document boss to find a reader it can use. 
// ===================================================================================
class IDocReader : public XUnknown {
	
//-----------------------------------
//	API
//
public:
	virtual bool 		CanRead(OSType type, const std::wstring& extension) const = 0;
						/**< Windows will ignore type. */
			
	virtual void 		Read(const XFileSpec& spec) = 0;	
};

typedef XInterfacePtr<IDocReader> IDocReaderPtr;
typedef XInterfacePtr<const IDocReader> IConstDocReaderPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
