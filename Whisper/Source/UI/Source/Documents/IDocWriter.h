/*
 *  File:       IDocWriter.h
 *  Summary:   	XPersistDoc will iterate over the document boss to find a writer it can use. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: IDocWriter.h,v $
 *		Revision 1.4  2000/12/31 04:21:00  jesjones
 *		Fixed some comments.
 *		
 *		Revision 1.3  2000/12/14 08:21:15  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:01:25  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 8/29/00	JDJ		Created (from Raven)
 */

#pragma once

#include <XInterfacePtr.h>
#include <XFileType.h>
#include <XUnknown.h>

#if MAC
	#include <XHandle.h>
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFileSpec;


// ===================================================================================
//	class IDocWriter
//!		XPersistDoc will iterate over the document boss to find a writer it can use. 
// ===================================================================================
class IDocWriter : public XUnknown {
	
//-----------------------------------
//	API
//
public:
	virtual XFileType 	GetType() const = 0;
						/**< On Windows the extension of the first writer is used for 
						the default extension. On the Mac the first writer's
						creator and type are used for the document proxy on new
						documents. */
						
	virtual bool 		CanWrite() const = 0;
						/**< Returns true if the writer can write out the file (writers
						may not be able to write out every document the app can
						produce, eg an image processing app can't write out 16-bit
						PNG files because PNG doesn't have a 16-bit format). */
			
	virtual void 		Write(OSType type, const XFileSpec& spec) const = 0;
	
#if MAC
	virtual OSType 		GetPreviewType() const = 0;
						/**< The preview functions are called if XFileDialogOptions uses
						the kNavAllowPreviews constant (which is the default). If your
						document is an image type ('PICT', 'JPEG', etc.) you can return
						0 and Nav Services will create a preview for you. If you dont
						want a preview return '????'. */
	
	virtual XHandle 	GetPreviewData() const = 0;
						/**< Called if GetPreviewType returns a non-zero value. You'll
						typically return either a custom 'PICT' or a text string. */
#endif
};

typedef XInterfacePtr<IDocWriter> IDocWriterPtr;
typedef XInterfacePtr<const IDocWriter> IConstDocWriterPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
