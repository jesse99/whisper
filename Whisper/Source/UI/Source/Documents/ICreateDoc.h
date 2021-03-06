/*
 *  File:       ICreateDoc.h
 *  Summary:   	Interface used by XDocuments to create documents. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: ICreateDoc.h,v $
 *		Revision 1.1  2001/02/07 02:31:49  jesjones
 *		Checked in
 *		
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

class IHierarchy;
typedef XInterfacePtr<IHierarchy> IHierarchyPtr;


// ===================================================================================
//	class ICreateDoc
//!		Interface used by XDocuments to create documents. 
// ===================================================================================
class ICreateDoc : public XUnknown {
	
//-----------------------------------
//	API
//
public:
	virtual IDocumentPtr OnCreate(IHierarchyPtr& parent) = 0;
						/**< You should also create a doc window here. */
};

typedef XInterfacePtr<ICreateDoc> ICreateDocPtr;
typedef XInterfacePtr<const ICreateDoc> IConstCreateDocPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
