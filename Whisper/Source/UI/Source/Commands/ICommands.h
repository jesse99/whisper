/*
 *  File:       ICommands.h
 *  Summary:   	Some handy command classes.
 *  Written by: Jesse Jones
 *
 *	Classes:	IDeferredMenuCommand	- Routes a menu command to an IInputTarget.
 *				IDeferredErrorCommand	- Calls ReportError.
 *				ICallbackCommand		- Calls an arbitrary function with a preset list of arguments.
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ICommands.h,v $
 *		Revision 1.5  2001/04/04 04:53:34  jesjones
 *		Updated IDeferredMenuCommand so that it uses the app's command handler.
 *		
 *		Revision 1.4  2001/03/05 05:37:09  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:20:31  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:00:39  jesjones
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
class IMenuHandler;
typedef XInterfacePtr<IMenuHandler> IMenuHandlerPtr;


// ===================================================================================
//	class IDeferredMenuCommand
//!		Sends a menu command to the installed menu handlers.
// ===================================================================================
class IDeferredMenuCommand : public XUnknown {

public:
	virtual void 		Init(const XMenuCommand& command) = 0;
};

typedef XInterfacePtr<IDeferredMenuCommand> IDeferredMenuCommandPtr;
typedef XInterfacePtr<const IDeferredMenuCommand> IConstDeferredMenuCommandPtr;


// ===================================================================================
//	class IDeferredErrorCommand
//!		Calls ReportError.
// ===================================================================================
class IDeferredErrorCommand : public XUnknown {

public:
	virtual void 		Init(const std::wstring& summary, const std::exception* e) = 0;
						/**< e may be nil */

	virtual void 		Init(const std::wstring& summary, const std::wstring& narrative) = 0;
};

typedef XInterfacePtr<IDeferredErrorCommand> IDeferredErrorCommandPtr;
typedef XInterfacePtr<const IDeferredErrorCommand> IConstDeferredErrorCommandPtr;


// ===================================================================================
//	class ICallbackCommand
//!		Calls an arbitrary function with a preset list of arguments.
// ===================================================================================
class ICallbackCommand : public XUnknown {

public:
	virtual void 		Init(const XCallback0<void>& callback) = 0;
};

typedef XInterfacePtr<ICallbackCommand> ICallbackCommandPtr;
typedef XInterfacePtr<const ICallbackCommand> IConstCallbackCommandPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

