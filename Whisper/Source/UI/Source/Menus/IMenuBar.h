/*
 *  File:       IMenuBar.h
 *  Summary:   	Cross platform wrapper around a top level menu.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IMenuBar.h,v $
 *		Revision 1.4  2001/03/05 05:37:54  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:23:30  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:03:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <IMenu.h>
#include <XConstants.h>
#include <XInterfacePtr.h>
#include <XTypes.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class IMenu;
typedef XInterfacePtr<IMenu> IMenuPtr;


// ===================================================================================
//	class IMenuBar
//!		Cross platform wrapper around a top level menu.
// ===================================================================================
class IMenuBar : public XUnknown {

//-----------------------------------
//	Initialization
//
public:
	virtual void 		Init(OSWindow window) = 0;
						/**< Window is ignored on the Mac. */
						
//-----------------------------------
//	Public API
//
public:					
	//! @name MenuBar
	//@{
	virtual void 		RemoveMenus(bool redraw = kRedraw) = 0;
						/**< Removes all the installed menus. */

	virtual void 		Invalidate() = 0;
						/**< Call this after doing an operation that may change the state 
						of a menu item. */
						
#if WIN
	virtual HMENU 		GetOSMenu() = 0;
#endif
	//@}

	//! @name Menus
	//@{
	virtual uint32 		GetCount() const = 0;
						/**< Note that this does not include submenus. */
			
	virtual IMenuPtr 	GetMenuByIndex(uint32 index) const = 0;
			
	virtual IMenuPtr 	GetMenuByName(const std::wstring& name, bool recursive = kRecursive) const = 0;
						/**< Returns nil if the menu isn't found. */
						
	virtual IMenuPtr 	GetMenuByID(uint32 id, bool recursive = kRecursive) const = 0;
						/**< Returns nil if the menu isn't found. */

	virtual IMenuPtr 	GetMenuByHandle(OSMenu menuH, bool recursive = kRecursive) const = 0;
						/**< Returns nil if the menu isn't found. */

	virtual void 		AppendMenu(const IMenuPtr& takeMenu, bool redraw = kRedraw) = 0;

	virtual void 		InstallMenu(const IMenuPtr& takeMenu, const IMenuPtr& beforeMenu, bool redraw = kRedraw) = 0;

	virtual void 		RemoveMenu(const IMenuPtr& menu, bool redraw = kRedraw) = 0;
	//@}
	
	//! @name Menu Commands
	//@{
#if MAC
	virtual XMenuCommand GetCommandFromClick(const EventRecord& event) const = 0;

	virtual XMenuCommand GetCommandFromKey(const EventRecord& event) const = 0;

#elif WIN
	virtual	XMenuCommand FindCommand(int16 id) = 0;
	virtual	IMenu::SHelp FindHelp(int16 id) = 0;

	virtual int16 		GetMenuID(const XMenuCommand& command) const = 0;
	virtual void 		RemoveMenuID(int16 id) = 0;
	virtual	void		SetHelp(int16 id, const IMenu::SHelp& help) = 0;
#endif
	//@}

//-----------------------------------
//	Internal API
//
public:					
	virtual void 		Redraw() = 0;
	
	virtual bool		IsValid() const = 0;

	virtual void		Validate() = 0;
	
#if DEBUG
	virtual void 		AddCommand(const XMenuCommand& cmd) = 0;

	virtual void 		RemoveCommand(const XMenuCommand& cmd) = 0;
#endif

#if WIN
	virtual HACCEL 		GetAcceleratorTable() = 0;
	virtual void 		RebuildAcceleratorTable() = 0;
	virtual void 		DestroyAcceleratorTable() = 0;
#endif
};

typedef XInterfacePtr<IMenuBar> IMenuBarPtr;
typedef XInterfacePtr<const IMenuBar> IConstMenuBarPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
