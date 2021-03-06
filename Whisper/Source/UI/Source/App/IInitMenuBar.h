/*
 *  File:       IInitMenuBar.h
 *  Summary:   	An interface used to initialize menubars.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IInitMenuBar.h,v $
 *		Revision 1.4  2001/03/05 05:37:43  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:22:56  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:02:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XCallbacks.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IInitMenuBar
//!		An interface used to initialize menubars.
// ===================================================================================
class IInitMenuBar : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		Init(OSWindow window) = 0;
						/**< Window will be nil on Macs. */
};

typedef XInterfacePtr<IInitMenuBar> IInitMenuBarPtr;
typedef XInterfacePtr<const IInitMenuBar> IConstInitMenuBarPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

