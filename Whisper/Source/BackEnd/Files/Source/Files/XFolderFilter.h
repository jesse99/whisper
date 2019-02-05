/*
 *  File:		XFolderFilter.h
 *  Summary:	Functor used for filtering folders.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFolderFilter.h,v $
 *		Revision 1.4  2001/03/05 05:41:40  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:52:40  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:36:49  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XReferenceCounted.h>
#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFolderSpec;


// ===================================================================================
//	class XFolderFilter
//!		Functor used for filtering folders.
// ===================================================================================
class FILES_EXPORT XFolderFilter : public XReferenceCountedMixin {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XFolderFilter();
	
						XFolderFilter();

private:
						XFolderFilter(const XFolderFilter& rhs);
						
			XFolderFilter& operator=(const XFolderFilter& rhs);
					
//-----------------------------------
//	API
//
public:
	virtual bool 		operator()(const XFolderSpec& spec) const;
						/**< Return true if the folder is OK. Default always returns true. */
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
