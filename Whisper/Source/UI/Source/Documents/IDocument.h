/*
 *  File:       IDocument.h
 *  Summary:   	An object containing the data for an application. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: IDocument.h,v $
 *		Revision 1.5  2001/03/25 06:12:28  jesjones
 *		kReverted message now includes the file spec.
 *		
 *		Revision 1.4  2001/03/05 05:37:18  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:20:55  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:01:13  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XAnyValue.h>
#include <XCallbacks.h>
#include <XInterfacePtr.h>
#include <XFileDialog.h>
#include <XFileSpec.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class IDocument;


//-----------------------------------
//	Constants
//
const uint32 kOpenedDocument     	  = 'OpnD';	//!< data will be an XFileSpec or empty if the doc was opened without a file
const uint32 kForceCloseDocument  	  = 'CloD';	
const uint32 kChangedDocumentName 	  = 'CgDn';	
const uint32 kChangedDocument     	  = 'CngD';	//!< defaults to empty, but clients can specify anything when calling SetDirty
const uint32 kSavedDocument       	  = 'SavD';	//!< data will be an XFileSpec
const uint32 kRevertedDocument    	  = 'RvtD';	//!< data will be an XFileSpec, kChangedDocument will also be broadcast
const uint32 kDocumentDirtyChanged    = 'DrtD';	//!< clients will normally listen to kChangedDocument instead of this
const uint32 kChangedDocumentFileSpec = 'FspD';


// ===================================================================================
//	struct SDocumentMessage
//!		IDocument calls callbacks with this struct when its state changes.
// ===================================================================================
struct UI_EXPORT SDocumentMessage {					
	const IDocument*	doc;
	uint32				change;
	XAnyValue			data;
	
		SDocumentMessage(const IDocument* d, uint32 c)						{doc = d; change = c;}
		SDocumentMessage(const IDocument* d, uint32 c, const XAnyValue& v)	{doc = d; change = c; data = v;}
};


// ===================================================================================
//	class IDocument
//!		An object containing the data for an application. 
// ===================================================================================
class IDocument : public XUnknown {

//-----------------------------------
//	Types
//
public:
	typedef XCallback1<void, const SDocumentMessage&> Callback;
	
//-----------------------------------
//	API
//
public:
	//! @name Opening and Closing
	//@{
	virtual void 		Open() = 0;
						/**< Called when opening a brand new document. */
						
	virtual void 		Open(const XFileSpec& file) = 0;
						/**< Called when opening an existing document. */
						
	virtual	bool 		CanClose(ESaveChanges reason) = 0;
						/**< Puts up the save changes alert. Returns true if it's OK to 
						close. */
						
	virtual void		ForceClose() = 0;
						/**< Closes the document without asking the user if he wants to
						save his changes. (More typically you'll close the doc
						window(s)). */
	//@}
		
	//! @name Saving
	//@{
	virtual bool		Save() = 0;
	virtual bool		SaveAs() = 0;
						/**< Returns false if user canceled or there was an error writing the 
						file out (if there was an error an alert will have been displayed). */
	
	virtual void		Revert() = 0;
	//@}

	//! @name Dirty Flag
	//@{
	virtual bool		IsDirty() const = 0;
				
	virtual void		SetDirty(bool dirty = true, uint32 message = kChangedDocument, const XAnyValue& value = XAnyValue()) = 0;
						/**< If dirty is true message is broadcast. */
	//@}
						
	//! @name Name
	//@{
	virtual std::wstring GetName() const = 0;
		
	virtual void 		SetName(const std::wstring& newName) = 0;
	//@}

	//! @name File Accessors
	//@{
	virtual bool		IsOnDisk() const = 0;
			
	virtual XFileSpec	GetFile() const = 0;

	virtual bool 		SetFile(const XFileSpec& spec, bool onDisk) = 0;
						/**< Note that this does not change the document's name. */
	//@}

	//! @name Misc
	//@{
	virtual void 		AddCallback(Callback callback, uint32 change = 0) = 0;
	virtual void 		RemoveCallback(Callback callback) = 0;
						/**< Called when the document state changes (eg when SetDirty is called).
						Controls displaying document data will typically register callbacks
						so they can invalidate themselves as the doc changes. By default
						the callback will be called for every change message, but you can
						specify the change you're interested in if you wish. */
	//@}
};

typedef XInterfacePtr<IDocument> IDocumentPtr;
typedef XInterfacePtr<const IDocument> IConstDocumentPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
