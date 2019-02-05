/*
 *  File:       XContextMenu.cpp
 *  Summary:   	Cross platform wrapper around contextual popup menus (ie menus not attached to a menubar).
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998--2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WContextMenu.cpp,v $
 *		Revision 1.8  2001/02/21 08:15:40  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.7  2000/12/31 04:40:46  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.6  2000/12/27 01:04:38  jesjones
 *		Tweaked command id code to work without a menubar.
 *		
 *		Revision 1.5  2000/12/25 02:47:41  jesjones
 *		Tweaked so that context menus no longer rely on a menubar.
 *		
 *		Revision 1.4  2000/12/14 08:30:06  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/10 04:06:24  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:05:15  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 7/29/99	JDJ		DoUpdateMenu updates submenus.
 *		 <3>	 6/30/99	JDJ		Added XXMLDoc ctor.
 *		 <2>	 4/29/99	JDJ		Renamed Post Execute. Added deleteMenu argument.
 *		 <1>	10/10/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <IContextMenu.h>

#include <IMenuHandler.h>
#include <WUtils.h>
#include <XEvent.h>
#include <XExceptions.h>
#include <XInterfaceMacros.h>
#include <XNumbers.h>
#include <XStringUtils.h>

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
						
	virtual void 		Init(const XPoint& globalLoc);

//-----------------------------------
//	API
//
public:
	virtual XMenuCommand Execute();

//-----------------------------------
//	Internal API
//
protected:
			void 		DoUpdateMenu(IMenuPtr menu);

			XMenuCommand DoMenuSelect();
			
//-----------------------------------
//	Member Data
//
protected:
	XPoint		mLoc;
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
}


//---------------------------------------------------------------
//
// XContextMenu::Init
//
//---------------------------------------------------------------
void XContextMenu::Init(const XPoint& globalLoc) 
{
	mLoc = globalLoc;
}


//---------------------------------------------------------------
//
// XContextMenu::Execute
//
//---------------------------------------------------------------
XMenuCommand XContextMenu::Execute()
{
	// Make sure the cursor is an arrow.
	HCURSOR cursor = ::LoadCursor(nil, IDC_ARROW);
	(void) ::SetCursor(cursor);
	
	// Use the context to initialize the menu state.
	IMenuPtr menu(this);
	ASSERT(!menu->IsInstalled());
	
	this->DoUpdateMenu(menu);
	
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

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XContextMenu::DoMenuSelect
//
//---------------------------------------------------------------
XMenuCommand XContextMenu::DoMenuSelect()
{
	XMenuCommand command = kNothingCmd;
	
	uint32 flags = TPM_LEFTALIGN + TPM_TOPALIGN + TPM_RETURNCMD + TPM_RIGHTBUTTON;
	
	IMenuPtr menu(this);
	HMENU menuH = menu->GetOSMenu();
	
	int32 result = ::TrackPopupMenu(menuH, flags, mLoc.x, mLoc.y, 0, GetWorkWindow(), nil);
	ThrowIf(result == 0xFFFFFFFF);
	
	if (result != 0) {
		command = menu->GetCommand(numeric_cast<uint32>(result-1));
		ASSERT(command != kNothingCmd);
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
	XEvent event;
	IMenuHandlerPtr handler(L"Application");
	for (uint32 item = 0; item < menu->GetCount(); item++) {			
		if (menu->IsSubMenu(item)) {
			IMenuPtr subMenu = menu->GetSubMenu(item);
			this->DoUpdateMenu(subMenu);
			
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
				
				if (status.name != L"")
					menu->SetName(item, status.name);
			}
		}
	}
}


}	// namespace Whisper
