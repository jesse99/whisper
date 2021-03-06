/*
 *  File:		XURIFetchers.h
 *  Summary:	Classes used to load Uniform Resource Identifiers (URIs).
 *  Written by:	Jesse Jones
 *
 *	Classes:	XFileURIFetcher		- Handles scheme "file" by loading the resource using the file system.
 *				XResourceURIFetcher	- Handles scheme "resource" by loading the resource using an OS resource.
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XURIFetchers.h,v $
 *		Revision 1.4  2001/03/05 05:44:26  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:57:23  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:58:59  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XURI.h>
#include <XURIResolver.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XFileURIFetcher
//!		Handles scheme "file" by loading the resource using the file system.
// ===================================================================================
class FILES_EXPORT XFileURIFetcher : public XURIFetcher {

	typedef XURIFetcher Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XFileURIFetcher();

						XFileURIFetcher();
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual XResource* 	OnResolve(const XURI& uri);
};


// ===================================================================================
//	class XResourceURIFetcher
//!		Handles scheme "resource" by loading the resource using an OS resource.
// ===================================================================================
class FILES_EXPORT XResourceURIFetcher : public XURIFetcher {

	typedef XURIFetcher Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XResourceURIFetcher();

						XResourceURIFetcher();
						/**< The resource type is the path's extension. The resource name
						is the file name less the extension. */
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual XResource* 	OnResolve(const XURI& uri);
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

