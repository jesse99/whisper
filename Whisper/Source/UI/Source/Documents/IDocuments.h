/*
 *  File:       IDocuments.h
 *  Summary:   	Interface an the app that creates new documents and handles stuff like save all. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: IDocuments.h,v $
 *		Revision 1.5  2001/04/05 04:01:11  jesjones
 *		New returns the new document.
 *		
 *		Revision 1.4  2001/02/07 02:22:44  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:21:00  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:01:17  jesjones
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
class XFileDialogOptions;
class XFileFilter;
class XFileSpec;
typedef XInterfacePtr<IDocument> IDocumentPtr;


//-----------------------------------
//	Constants
//
enum EStartupAction {kNewDocOnStartup, kOpenDocOnStartup, kDoNothingOnStartup};


// ===================================================================================
//	class IDocuments
//!		Interface an the app that creates new documents and handles stuff like save all. 
// ===================================================================================
class IDocuments : public XUnknown {
	
//-----------------------------------
//	API
//
public:
	virtual void 		Init(EStartupAction action) = 0;
	
	virtual IDocumentPtr New() = 0;
						/**< Called when the user wants to create a brand new document. */
						
	virtual void 		Open() = 0;
						/**< Pops up a dialog allowing the user to select one or more
						files to open. */

	virtual void 		Open(const XFileSpec& file) = 0;
						/**< Called when the user wants to open an existing file. */

	virtual void 		CloseAll() = 0;
	virtual void 		ForceCloseAll() = 0;
	virtual void 		SaveAll() = 0;
	
	virtual XFileDialogOptions& GetFileOptions() = 0;
						/**< This is used when the user pops up the open file dialog.
						You should, at a minimum, call AddType. */
						
	virtual void 		SetFileFilter(XFileFilter* takeFilter) = 0;
						/**< Optional file filter used when the user pops up the open 
						file dialog. */
};

typedef XInterfacePtr<IDocuments> IDocumentsPtr;
typedef XInterfacePtr<const IDocuments> IConstDocumentsPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
