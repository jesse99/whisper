/*
 *  File:       XContextMenu.cpp
 *  Summary:   	Cross platform wrapper around contextual popup menus (ie menus not attached to a menubar).
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MContextMenu.cpp,v $
 *		Revision 1.8  2001/04/27 04:19:12  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.7  2001/02/08 03:50:06  jesjones
 *		Commands are directed to the menu handler on the app now.
 *		
 *		Revision 1.6  2001/01/21 00:26:15  jesjones
 *		Renamed some methods in IMenuHandler.
 *		
 *		Revision 1.5  2000/12/31 04:25:28  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.4  2000/12/14 08:26:55  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/10 04:02:52  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:10:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <6>	 7/29/99	JDJ		Added support for submenus. DoMenuSelect uses PopUpMenuSelect 
 *									if the menu has balloon help.
 *		 <5>	 7/19/99	JDJ		Tweaked for Carbon.
 *		 <4>	 6/30/99	JDJ		Added XXMLDoc ctor.
 *		 <3>	 4/29/99	JDJ		Renamed Post Execute. Added deleteMenu argument.
 *		 <2>	12/23/98	JDJ		DoUpdateMenu wasn't setting menu item style.
 *		 <1>	10/10/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <IContextMenu.h>

#include <IMenuHandler.h>
#include <XEvent.h>
#include <XExceptions.h>
#include <XInterfaceMacros.h>
#include <XNumbers.h>
#include <XStringUtils.h>

#if MAC
	#include <MSystemInfo.h>
#endif

namespace Whisper {


// ===================================================================================
//	class XContextMenu
//!		Cross platform wrapper around contextual popup menus (ie menus not attached to a menubar).
// ===================================================================================
class XContextMenu : public IContextMenu {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XContextMenu();

						XContextMenu(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const XPoint& globalLoc);
		
	virtual void 		Init(const XPoint& globalLoc, const AEDesc* selection = nil, 
							 uint32 helpType = kCMHelpItemNoHelp, const std::wstring& helpText = L"");
		
	virtual XMenuCommand Execute();

//-----------------------------------
//	Internal API
//
private:
			void 		DoUpdateMenu(IMenuPtr menu);

			XMenuCommand DoMenuSelect();
			
//-----------------------------------
//	Member Data
//
private:
	XPoint			mLoc;

	const AEDesc*	mSelection;			// used when Contextual Menu Manager is installed
	uint32			mHelpType;
	std::wstring	mHelpText;
	
	bool			mHasHelp;
};

DEFINE_INTERFACE_FACTORY(XContextMenu)

//---------------------------------------------------------------
//
// XContextMenu::~XContextMenu
//
//---------------------------------------------------------------
XContextMenu::~XContextMenu()
{
}


//---------------------------------------------------------------
//
// XContextMenu::XContextMenu 
//
//---------------------------------------------------------------
XContextMenu::XContextMenu(XBoss* boss)
{		
	this->DoSetBoss(boss);

	mSelection = nil;
	mHelpType  = kCMHelpItemNoHelp;
	
	mHasHelp = false;
}


//---------------------------------------------------------------
//
// XContextMenu::Init (XPoint)
//
//---------------------------------------------------------------
void XContextMenu::Init(const XPoint& globalLoc)
{	
	mLoc = globalLoc;
}


//---------------------------------------------------------------
//
// XContextMenu::Init (XPoint, const AEDesc*, uint32, wstring)
//
//---------------------------------------------------------------
void XContextMenu::Init(const XPoint& globalLoc, const AEDesc* selection, uint32 helpType, const std::wstring& helpText)
{
	mLoc = globalLoc;
	
	mSelection = selection;
	mHelpType  = helpType;
	mHelpText  = helpText;
	
	mHasHelp = false;
}


//---------------------------------------------------------------
//
// XContextMenu::Execute
//
//---------------------------------------------------------------
XMenuCommand XContextMenu::Execute()
{
	// Make sure the cursor is an arrow.
	:: InitCursor();
	
	// Use the context to initialize the menu state.
	IMenuPtr menu(this);
	ASSERT(!menu->IsInstalled());
	
	this->DoUpdateMenu(menu);
	
	// Tell the Menu Manager that our menu is a submenu (this is
	// required by PopUpMenuSelect).
	::InsertMenu(menu->GetOSMenu(), hierMenu);
	
	// Popup the menu at the requested location.
	XMenuCommand command = this->DoMenuSelect();
	if (command != kNothingCmd) {
	
		// If the user has made a selection we'll send the command
		// straight to the context. 
		XEvent event;
		IMenuHandlerPtr handler(L"Application");
		handler->HandleCommand(command, event);
	}
	
	return command;
}

#pragma mark ~

//---------------------------------------------------------------
//
// XContextMenu::DoMenuSelect
//
//---------------------------------------------------------------
XMenuCommand XContextMenu::DoMenuSelect()
{
	XMenuCommand command = kNothingCmd;
		
	int16 menuID = 0;
	uint16 menuItem = 0;
	
	IMenuPtr menu(this);
	MenuHandle menuH = menu->GetOSMenu();

	if (MSystemInfo::HasContextMenus() && (mSelection != nil || !mHasHelp)) {	// ContextualMenuSelect doesn't support balloon help...
		uint32 selectionType;
		OSStatus err = ::ContextualMenuSelect(menuH, 					// menu
										   	  mLoc.GetOSPoint(), 		// global location
										   	  false,					// balloon help available ($$Ê5/12 Context Menus doc page 8 says this should be set to false)
										   	  mHelpType,				// help type
										   	  ToPascalStr(mHelpText).c_str(),	// help item string
										   	  mSelection,				// object specifier for the selected item
										   	  &selectionType,			// set to the type of selection the user made
										  	  &menuID,					// menu id of the chosen item (could be submenu)
										   	  &menuItem);				// chosen menu item number		
		if (err == noErr) {
			if (selectionType == kCMShowHelpSelected)
				command = kHelpCmd;
			else if (selectionType != kCMMenuItemSelected)
				menuID = 0;
			
		} else if (err == userCanceledErr)
			menuID = 0;

		else 
			ThrowErr(err);
		
	} else {
		int32 result = ::PopUpMenuSelect(menuH, numeric_cast<int16>(mLoc.y), numeric_cast<int16>(mLoc.x), 1);	
	
		menuID = HiWord(result);
		menuItem = (uint16) LoWord(result);
	}
	
	if (command == kNothingCmd && menuID != 0) {
		if (menuID == menu->GetOSMenuID())
			command = menu->GetCommand(menuItem - 1UL);
		
		else {
			IMenuPtr child = menu->FindSubMenu(menuID);
			if (child)
				command = child->GetCommand(menuItem - 1UL);
		}
	}
	
	return command;
}


//---------------------------------------------------------------
//
// XContextMenu::DoUpdateMenu
//
//---------------------------------------------------------------
void XContextMenu::DoUpdateMenu(IMenuPtr menu)
{	
	if (menu->HasHelp())
		mHasHelp = true;

	XEvent event;
	IMenuHandlerPtr handler(L"Application");
	for (uint32 item = 0; item < menu->GetCount(); item++) {
		if (menu->IsSubMenu(item)) {
			IMenuPtr subMenu = menu->GetSubMenu(item);
			this->DoUpdateMenu(subMenu);
			
			::InsertMenu(subMenu->GetOSMenu(), hierMenu);	// we're not part of a menubar so this isn't automatically done for us

		} else {	
			XMenuCommand command = menu->GetCommand(item);
			if (command != kNothingCmd) {
				
				// Ask the context for the status of the command associated 
				// with the menu item.
				SCommandStatus status;
				(void) handler->HandleEnableCommand(command, status, event);
				
				// Update the menu item to reflect its new status.
				menu->EnableItem(item, status.enabled);	
				menu->CheckItem(item, status.checkmark);
				
				if (status.style != 0)
					menu->SetStyle(item, status.style);
	
				if (status.name.length() > 0)
					menu->SetName(item, status.name);
			}
		}
	}
}


}	// namespace Whisper
