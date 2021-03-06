/*
 *  File:       IPersistDoc.h
 *  Summary:   	The interface used by XDocument to write and read the document's data. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: IPersistDoc.h,v $
 *		Revision 1.4  2001/03/05 05:38:12  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:24:09  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:05:40  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XInterfacePtr.h>
#include <XFileDialog.h>
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
//	class IPersistDoc
//!		The interface used by XDocument to write and read the document's data. 
// ===================================================================================
class IPersistDoc : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void		HandleWrite(OSType type, const XFileSpec& spec) = 0;
	virtual void 		HandleRead(const XFileSpec& spec) = 0;
	
	virtual XSaveReply 	SaveFile(const std::wstring& prompt, const std::wstring& fileName) = 0;
						/**< Pops up a dialog asking the user where to save the document. */
	
	virtual bool 		CanReplace(const XSaveReply& reply) = 0;
						/**< Returns true if it's OK to overwrite reply.GetTarget(). */

#if MAC
	virtual OSType 		GetDefaultCreator() const = 0;
	virtual OSType 		GetDefaultType() const = 0;
						/**< Used with document proxies when the document isn't on disk. */
						
						
	virtual void 		SavePreview(const XFileSpec& spec) = 0;
#endif
};

typedef XInterfacePtr<IPersistDoc> IPersistDocPtr;
typedef XInterfacePtr<const IPersistDoc> IConstPersistDocPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper




