/*
 *  File:       IClickExtensions.h
 *	Function:	Classes for objects that affect the way their owner responds to clicks.
 *  Written by: Jesse Jones
 *
 *	Classes:	IMenuCommandClickExtension	- Sends a XMenuCommand to a specified input target.
 *				ICommandClickExtension		- Posts an XCommand.
 *				ICallbackClickExtension		- Executes an XCallback object.
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IClickExtensions.h,v $
 *		Revision 1.4  2001/03/05 05:36:57  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:20:18  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:00:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XCallbacks.h>
#include <XInterfacePtr.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class ICommand;
class IMenuHandler;

typedef XInterfacePtr<ICommand> ICommandPtr;
typedef XInterfacePtr<IMenuHandler> IMenuHandlerPtr;


// ===================================================================================
//	class IMenuCommandClickExtension
//!		Sends a XMenuCommand to a specified input target.
// ===================================================================================
class IMenuCommandClickExtension : public XUnknown {
	
//-----------------------------------
//	Initialization
//
public:	
	virtual void 		Init(const IMenuHandlerPtr& target, const XMenuCommand& command, 
							 uint32 clickCount = 0, const std::wstring& name = L"IMenuCommandClickExtension") = 0;
						/**< When the owner is clicked command is sent to target. */
};

typedef XInterfacePtr<IMenuCommandClickExtension> IMenuCommandClickExtensionPtr;
typedef XInterfacePtr<const IMenuCommandClickExtension> IConstMenuCommandClickExtensionPtr;


// ===================================================================================
//	class ICommandClickExtension
//!		Posts an ICommand.
// ===================================================================================
class ICommandClickExtension : public XUnknown {
	
//-----------------------------------
//	Initialization
//
public:	
	virtual void 		Init(const ICommandPtr& command, uint32 clickCount = 0, 
	                         const std::wstring& name = L"ICommandClickExtension") = 0;
						/**< Command will be posted when the owner is clicked. */
};

typedef XInterfacePtr<ICommandClickExtension> ICommandClickExtensionPtr;
typedef XInterfacePtr<const ICommandClickExtension> IConstCommandClickExtensionPtr;


// ===================================================================================
//	class ICallbackClickExtension
//!		Executes an XCallback object.
// ===================================================================================
class ICallbackClickExtension : public XUnknown {
	
//-----------------------------------
//	Initialization
//
public:	
	virtual void 		Init(const XCallback0<void>& callback, uint32 clickCount = 0, 
	                         const std::wstring& name = L"ICallbackClickExtension") = 0;
						/**< Executes the callback each time the owner is clicked. */
};

typedef XInterfacePtr<ICallbackClickExtension> ICallbackClickExtensionPtr;
typedef XInterfacePtr<const ICallbackClickExtension> IConstCallbackClickExtensionPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
