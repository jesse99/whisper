/*
 *  File:       IParentChangedSize.h
 *  Summary:   	Optional interface that is invoked for children whose parent size changes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IParentChangedSize.h,v $
 *		Revision 1.4  2001/03/05 05:38:08  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:24:05  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:05:37  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XConstants.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XSize;


// ===================================================================================
//	class IParentChangedSize
//!		Optional interface that is invoked for children whose parent size changes.
// ===================================================================================
class IParentChangedSize : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		HandleParentSizeChange(const XSize& delta, bool redraw = kRedraw) = 0;
						/**< This will be called after the parent's size changes. Delta is 
						the new parent size minus the old parent size. */
};

typedef XInterfacePtr<IParentChangedSize> IParentChangedSizePtr;
typedef XInterfacePtr<const IParentChangedSize> IConstParentChangedSizePtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
