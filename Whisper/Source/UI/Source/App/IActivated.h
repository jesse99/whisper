/*
 *  File:       IActivated.h
 *  Summary:   	Hook for notification when an object is activated or deactivated.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IActivated.h,v $
 *		Revision 1.1  2001/01/30 07:27:17  jesjones
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


// ===================================================================================
//	class IActivated
//!		Hook for notification when an object is activated or deactivated.
// ===================================================================================
class IActivated : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		OnActivated() = 0;
	virtual void 		OnDeactivated() = 0;
};

typedef XInterfacePtr<IActivated> IActivatedPtr;
typedef XInterfacePtr<const IActivated> IConstActivatedPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

