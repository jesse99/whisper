/*
 *  File:       IMenuHandler.h
 *  Summary:   	Interface on the app that routes menu commands to registered callbacks.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IMenuHandler.h,v $
 *		Revision 1.6  2001/02/09 04:32:15  jesjones
 *		Added kEnabledIfDocWindow.
 *		
 *		Revision 1.5  2001/01/30 07:16:13  jesjones
 *		Menu commands are now handled via callbacks.
 *		
 *		Revision 1.4  2001/01/21 00:23:03  jesjones
 *		Menu handling now uses callbacks.
 *		
 *		Revision 1.3  2000/12/14 08:23:42  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:04:00  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <IMenu.h>
#include <XCallbacks.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XEvent;

enum EMenuEnabling {kAlwaysEnabled = 0, 		//!< the menu item is always enabled
					kDisabledIfLowMemory = 1, 	//!< the menu item is disabled if the app is low on memory
					kEnabledIfDocWindow = 2, 	//!< the menu item is enabled if a document window is active
					kDisabledIfDialog = 4, 		//!< the menu item is disbled if a dialog is up
					kNeverEnabled = 255			//!< the menu item is always disabled
					};


// ===================================================================================
//	struct SCommandStatus
//!		Struct used by IMenuHandler to enable/disable menu items.
// ===================================================================================
struct UI_EXPORT SCommandStatus {
	std::wstring	name;			//!< If name is empty menu item text won't change.
	bool			enabled;
	uint32			checkmark;		//!< kNoCheckmark or sum of kEntireSelection, kPartialSelection, kIndependantCheckMark, and kExclusiveCheckMark (eg bold and italic can both be selected so they're independant, left/center/right align act like radio buttons so they exclude the other items)
#if MAC
	Style			style;			//!< can be normal or sum of bold, italic, underline, outline, shadow, condense, and extend
#endif
	
	SCommandStatus() 
	{
		enabled   = false; 
		checkmark = kNoCheckmark; 
#if MAC
		style     = normal;
#endif
	}
};


// ===================================================================================
//	class IMenuHandler
//!		Interface on the app that routes menu commands to registered callbacks.
// ===================================================================================
class IMenuHandler : public XUnknown {

//-----------------------------------
//	Types
//
public:					
	typedef XCallback0<void> Action;				//!< callback that is called when a menu item is chosen
	typedef XCallback0<SCommandStatus> Enabler;		//!< callback used to enable, change the name, and set the checkmark of menu items
	typedef XCallback0<bool> SimpleEnabler;			//!< callback used to enable/disable menu items
	
//-----------------------------------
//	API
//
public:					
	// ----- Command Processing -----
	virtual bool 		HandleCommand(const XMenuCommand& command, const XEvent& event) = 0;
						/**< Called by Whisper when the user chooses a menu item. Walks the
						list of registered handlers and calls the first one that matches. */
	
	virtual bool 		HandleEnableCommand(const XMenuCommand& command, SCommandStatus& status, const XEvent& event) = 0;
						/**< Called by Whisper when a menu needs to be enabled. Walks the
						list of registered handlers and calls the first one that matches. */

	// ----- Command Registration -----
	virtual void 		RegisterCommand(const XMenuCommand& command, Action action, uint32 enableFlags, const XUnknownPtr& object, bool prefix = false) = 0;
	virtual void 		RegisterCommand(const XMenuCommand& command, Action action, Enabler enabler, const XUnknownPtr& object, bool prefix = false) = 0;
	virtual void 		RegisterCommand(const XMenuCommand& command, Action action, SimpleEnabler enabler, const XUnknownPtr& object, bool prefix = false) = 0;
						/**< You will normally register menu command handlers when a control
						or window is activated (using the IActivated interface). It's possible 
						for two objects to register handlers for the same menu command. In this
						case the last object to register a handler will get the command (so
						controls get commands before windows). Object is used to query for a
						IMenuExtensible before the callbacks are invoked. Prefix can be used
						if you have a family of related commands that you want to handle in one 
						callback. */

	virtual void 		UnRegisterCommand(Action action) = 0;
						/**< Call this when your control/window is deactivated (using the
						IActivate interface). */

	// ----- Context -----
	virtual const XEvent& GetEvent() const = 0;
						/**< Occasionally menu commands change their behavior when a modifier
						key is held down. You can call this method to check the state of the
						modifier keys at the time the menu item was selected. */

	virtual const XMenuCommand& GetCommand() const = 0;
						/** You can call this inside an action or enabler command to find out
						which command is being processed. This can occasionally be useful if
						you're encoding information inside the command string. */
};

typedef XInterfacePtr<IMenuHandler> IMenuHandlerPtr;
typedef XInterfacePtr<const IMenuHandler> IConstMenuHandlerPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
