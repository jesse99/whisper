/*
 *  File:       IInstallMenuHandler.h
 *  Summary:   	Interface used to register XMenuCommand handlers.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IInstallMenuHandler.h,v $
 *		Revision 1.1  2001/01/21 00:50:49  jesjones
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
//	class IInstallMenuHandler
//!		Interface used to register XMenuCommand handlers.
// ===================================================================================
class IInstallMenuHandler : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		InstallCommands() = 0;

	virtual void 		RemoveCommands() = 0;
};


typedef XInterfacePtr<IInstallMenuHandler> IInstallMenuHandlerPtr;
typedef XInterfacePtr<const IInstallMenuHandler> IConstInstallMenuHandlerPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
