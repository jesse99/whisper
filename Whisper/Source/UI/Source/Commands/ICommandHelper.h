/*
 *  File:       ICommandHelper.h
 *  Summary:   	Helper interface used by XCommand.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ICommandHelper.h,v $
 *		Revision 1.1  2001/01/30 07:27:21  jesjones
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
//	class ICommandHelper
//!		Helper interface used by XCommand.
// ===================================================================================
class ICommandHelper : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		OnExecute() = 0;
};

typedef XInterfacePtr<ICommandHelper> ICommandHelperPtr;
typedef XInterfacePtr<const ICommandHelper> IConstCommandHelperPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

