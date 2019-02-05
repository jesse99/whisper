/*
 *  File:       IDocView.h
 *  Summary:   	Interface that allows us to distinguish between doc windows and
 *				other windows in the document layer. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: IDocView.h,v $
 *		Revision 1.4  2001/02/09 04:31:33  jesjones
 *		Tweaked a comment.
 *		
 *		Revision 1.3  2000/12/14 08:21:07  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:01:21  jesjones
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
class IDocument;
typedef XInterfacePtr<IDocument> IDocumentPtr;


// ===================================================================================
//	class IDocView
//!		Interface that allows us to distinguish between doc windows and other windows in the document layer. 
// ===================================================================================
class IDocView : public XUnknown {
	
//-----------------------------------
//	API
//
public:
	virtual IDocumentPtr GetDoc() const = 0;
};

typedef XInterfacePtr<IDocView> IDocViewPtr;
typedef XInterfacePtr<const IDocView> IConstDocViewPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
