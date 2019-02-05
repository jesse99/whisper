/*
 *  File:       IKeyHandler.h
 *  Summary:   	Interface on the app that routes key events to registered callbacks.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IKeyHandler.h,v $
 *		Revision 1.4  2001/01/30 07:09:09  jesjones
 *		Key events are now handled via callbacks.
 *		
 *		Revision 1.3  2000/12/14 08:23:15  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:03:11  jesjones
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


//-----------------------------------
//	Forward References
//
class XCharEvent;
class XKeyDownEvent;


// ===================================================================================
//	class IKeyHandler
//!		Interface on the app that routes key events to registered callbacks.
// ===================================================================================
class IKeyHandler : public XUnknown {

//-----------------------------------
//	Types
//
public:					
	typedef XCallback0<void> SingleKeyHandler;			//!< callback that handles a particular key
	typedef XCallback1<bool, XKeyDownEvent> KeyHandler;	//!< callback that handles virtual key codes, return true if the key was handled
	typedef XCallback1<bool, XCharEvent> CharHandler;	//!< callback that handles Unicode char events, return true if the char was handled
	
//-----------------------------------
//	API
//
public:					
	// ----- Key Processing -----
	virtual bool 		HandleKeyDown(const XKeyDownEvent& event) = 0;
						/**< Keystrokes are to the last callback that was registered. If the
						callback doesn't handle the key the next suitable callback is invoked. */

	virtual bool 		HandleChar(const XCharEvent& event) = 0;
	
	// ----- Key Registration -----
	virtual void 		RegisterHandler(int16 keyCode, SingleKeyHandler handler, const XUnknownPtr& object) = 0;
						/**< Use this for simple key handlers. Object is used to query for a IKeyExtensible 
						before the callback is invoked. Key handlers can be installed when a control becomes 
						active (IActivated) or acquires the focus (IFocused). */

	virtual void 		RegisterHandler(KeyHandler handler1, CharHandler handler2, const XUnknownPtr& object) = 0;
						/**< Use this if you want to catch all keystrokes. Note that in general
						one key press will result in a call to both handlers (although on Windows
						a key press can result in multiple calls to each handler). */

	virtual void 		UnRegisterHandler(SingleKeyHandler handler) = 0;
	virtual void 		UnRegisterHandler(KeyHandler handler) = 0;
						/**< Call this when your control/window is deactivated or loses the focus. */

	// ----- Context -----
	virtual const XKeyDownEvent& GetEvent() const = 0;
						/**< You can use this in a SingleKeyHandler if you want to do something like check
						the state of a modifier key. */
};

typedef XInterfacePtr<IKeyHandler> IKeyHandlerPtr;
typedef XInterfacePtr<const IKeyHandler> IConstKeyHandlerPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
