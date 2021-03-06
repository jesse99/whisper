/*
 *  File:       IMenuExtension.h
 *	Function:	Interface that allows you to override the way an object responds to menu commands.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
struct SCommandStatus;
class XEvent;


// ===================================================================================
//	class IMenuExtension
//!		Interface that allows you to override the way an object responds to menu commands.
// ===================================================================================
class IMenuExtension : public XUnknown {
	
//-----------------------------------
//	API 
//
public:
	// ----- Name -----
	virtual std::wstring GetName() const = 0;
	
	virtual void 		SetName(const std::wstring& name) = 0;
									
	// ----- Action -----
	virtual bool		HandleCommand(const XMenuCommand& command, const XEvent& event) = 0;
	virtual bool		HandleEnableCommand(const XMenuCommand& command, SCommandStatus& status, const XEvent& event) = 0;
						/**< Return true if the command was handled. */
};

typedef XInterfacePtr<IMenuExtension> IMenuExtensionPtr;
typedef XInterfacePtr<const IMenuExtension> IConstMenuExtensionPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
