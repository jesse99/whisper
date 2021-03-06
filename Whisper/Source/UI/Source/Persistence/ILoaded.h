/*
 *  File:       ILoaded.h
 *  Summary:   	Interface that provides a hook that is called after an object (and all
 *				of it's child objects) is streamed in.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ILoaded.h,v $
 *		Revision 1.4  2001/02/09 04:31:49  jesjones
 *		Tweaked a comment.
 *		
 *		Revision 1.3  2000/12/14 08:23:19  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:03:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class ILoaded
//!		Interface that provides a hook that is called after an object (and all of it's child objects) is streamed in.
// ===================================================================================
class ILoaded : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		OnLoaded() = 0;
};

typedef XInterfacePtr<ILoaded> ILoadedPtr;
typedef XInterfacePtr<const ILoaded> IConstLoadedPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
