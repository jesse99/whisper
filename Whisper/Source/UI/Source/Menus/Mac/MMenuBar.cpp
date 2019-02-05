/*
 *  File:       MMenuBar.cpp
 *  Summary:   	Cross platform wrapper around a top level menu.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MMenuBar.cpp,v $
 *		Revision 1.7  2001/04/21 03:27:07  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/04/13 07:51:18  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.5  2001/03/06 07:31:42  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.4  2000/12/31 04:32:30  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.3  2000/12/14 08:28:14  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:16:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <6>	 7/19/99	JDJ		Tweaked for Carbon.
 *		 <5>	 7/14/99	JDJ		Uses XXMLDoc::Load.
 *		 <4>	 4/06/99	JDJ		Added GetMenuByHandle.
 *		 <3>	 3/21/99	JDJ		Removed AddMenus. Added a ctor that takes a uri and a list of menus.
 *		 <2>	 3/19/99	JDJ		Constructor now takes an OSWindow on Mac side.
 *		 <1>	10/07/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IMenuBar.h>

#include <Appearance.h>
#include <Events.h>
#include <Resources.h>
#include <ToolUtils.h>

#include <algorithm>
#include <set>

#include <IActiveMenuBar.h>
#include <IMenu.h>
#include <MSystemInfo.h>
#include <XAutoPtr.h>
#include <XExceptions.h>
#include <XInterfacePtr.h>
#include <XMemUtils.h>
#include <XMiscUtils.h>
#include <XResource.h>
#include <XStringUtils.h>

#if GARBAGE_COLLECT_COM
	#include <XGarbageCollector.h>
#endif

namespace Whisper {


// ===================================================================================
//	class XMenuBar
//!		Cross platform wrapper around a top level menu.
// ===================================================================================
class XMenuBar : public IMenuBar {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XMenuBar();
						
						XMenuBar(XBoss* boss);

	virtual void 		Init(OSWindow window);
		
//-----------------------------------
//	Inherited API
//
public:					
	virtual void 		RemoveMenus(bool redraw = kRedraw);

	virtual void 		Invalidate()									{mIsValid = false;}
						
	virtual uint32 		GetCount() const								{return mMenus.size();}
			
	virtual IMenuPtr 	GetMenuByIndex(uint32 index) const;
			
	virtual IMenuPtr 	GetMenuByName(const std::wstring& name, bool recursive = kRecursive) const;
						
	virtual IMenuPtr 	GetMenuByID(uint32 id, bool recursive = kRecursive) const;

	virtual IMenuPtr 	GetMenuByHandle(OSMenu menuH, bool recursive = kRecursive) const;

	virtual void 		AppendMenu(const IMenuPtr& takeMenu, bool redraw = kRedraw)	{this->InstallMenu(takeMenu, IMenuPtr(), redraw);}

	virtual void 		InstallMenu(const IMenuPtr& takeMenu, const IMenuPtr& beforeMenu, bool redraw = kRedraw);

	virtual void 		RemoveMenu(const IMenuPtr& menu, bool redraw = kRedraw);
	
	virtual XMenuCommand GetCommandFromClick(const EventRecord& event) const;

	virtual XMenuCommand GetCommandFromKey(const EventRecord& event) const;

	virtual void 		Redraw();
	
	virtual bool		IsValid() const							{return mIsValid;}

	virtual void		Validate()								{mIsValid = true;}
	
#if DEBUG
	virtual void 		AddCommand(const XMenuCommand& cmd);

	virtual void 		RemoveCommand(const XMenuCommand& cmd);
#endif

protected:
	virtual	void 		Invariant() const;

//-----------------------------------
//	Internal API
//
private:					
			XMenuCommand DoFindCommand(ResID menuID, uint32 item) const;
		
			uint32 		DoGetMenuIndex(ResID menuID) const;

			void 		DoInstallMenu(const IMenuPtr& menu, int16 beforeMenuID, bool redraw);
	
//-----------------------------------
//	Member Data
//
protected:
	std::vector<IMenuPtr>	mMenus;
	bool					mIsValid;
	
#if DEBUG
	std::set<XMenuCommand>	mCommands;		// this is used to check that all installed menu items have unique menu commands
#endif
};

DEFINE_INTERFACE_FACTORY(XMenuBar)

//---------------------------------------------------------------
//
// XMenuBar::~XMenuBar
//
//---------------------------------------------------------------
XMenuBar::~XMenuBar()
{
	CALL_INVARIANT;
	
	if (!this->GetBoss()->IsPrototype()) {
		XInterfacePtr<IActiveMenuBar> activeMenuBar(L"Application");
		if (activeMenuBar->Get() == this)
			activeMenuBar->Set(IMenuBarPtr());
		
		this->RemoveMenus(kDontRedraw);
	}
}

					
//---------------------------------------------------------------
//
// XMenuBar::XMenuBar
//
//---------------------------------------------------------------
XMenuBar::XMenuBar(XBoss* boss)
{	
	this->DoSetBoss(boss);

	mIsValid = false;
}

				
//---------------------------------------------------------------
//
// XMenuBar::Init
//
//---------------------------------------------------------------
void XMenuBar::Init(OSWindow window)
{
	UNUSED(window);
	
	mIsValid = false;

	CALL_INVARIANT;
}

				
//---------------------------------------------------------------
//
// XMenuBar::RemoveMenus
//
//---------------------------------------------------------------
void XMenuBar::RemoveMenus(bool redraw)
{
	CHECK_INVARIANT;

	for (uint32 index = mMenus.size() - 1; index != ULONG_MAX; index--) {
		IMenuPtr menu = mMenus[index];
		
		menu->RemoveItems();
		this->RemoveMenu(menu);
	}

	if (redraw)
		::InvalMenuBar();

	mIsValid = false;

#if DEBUG
	ASSERT(mCommands.empty());
#endif
	POSTCONDITION(this->GetCount() == 0);
}


//---------------------------------------------------------------
//
// XMenuBar::GetMenuByIndex
//
//---------------------------------------------------------------
IMenuPtr XMenuBar::GetMenuByIndex(uint32 index) const			
{
	PRECONDITION(index < mMenus.size());
	CHECK_INVARIANT;
	
	IMenuPtr menu = mMenus[index];		
		
	POSTCONDITION(menu);
	
	return menu;
}


//---------------------------------------------------------------
//
// XMenuBar::GetMenuByName
//
//---------------------------------------------------------------
IMenuPtr XMenuBar::GetMenuByName(const std::wstring& name, bool recursive) const			
{
	CHECK_INVARIANT;

	IMenuPtr menu;
	
	std::vector<IMenuPtr> menus = mMenus;
	
	while (!menus.empty() && !menu) {
		IMenuPtr candidate = menus.back();
		menus.pop_back();	
		
		if (candidate->GetName() == name)
			menu = candidate;
			
		else if (recursive) {
			for (uint32 index = 0; index < candidate->GetCount(); ++index) {
				if (candidate->IsSubMenu(index))
					menus.push_back(candidate->GetSubMenu(index));
			}
		}
	}
				
	return menu;
}


//---------------------------------------------------------------
//
// XMenuBar::GetMenuByID
//
//---------------------------------------------------------------
IMenuPtr XMenuBar::GetMenuByID(uint32 id, bool recursive) const			
{
	CHECK_INVARIANT;

	IMenuPtr menu;
	
	std::vector<IMenuPtr> menus = mMenus;
	
	while (!menus.empty() && !menu) {
		IMenuPtr candidate = menus.back();
		menus.pop_back();	
				
		if (candidate->GetID() == id)
			menu = candidate;
			
		else if (recursive) {
			for (uint32 index = 0; index < candidate->GetCount(); ++index) {
				if (candidate->IsSubMenu(index))
					menus.push_back(candidate->GetSubMenu(index));
			}
		}
	}
				
	return menu;
}


//---------------------------------------------------------------
//
// XMenuBar::GetMenuByHandle
//
//---------------------------------------------------------------
IMenuPtr XMenuBar::GetMenuByHandle(OSMenu menuH, bool recursive) const			
{
	CHECK_INVARIANT;

	IMenuPtr menu;
	
	std::vector<IMenuPtr> menus = mMenus;
	
	while (!menus.empty() && !menu) {
		IMenuPtr candidate = menus.back();
		menus.pop_back();	
				
		if (candidate->GetOSMenu() == menuH)
			menu = candidate;
			
		else if (recursive) {
			for (uint32 index = 0; index < candidate->GetCount(); ++index) {
				if (candidate->IsSubMenu(index))
					menus.push_back(candidate->GetSubMenu(index));
			}
		}
	}
				
	return menu;
}


//---------------------------------------------------------------
//
// XMenuBar::Redraw
//
//---------------------------------------------------------------
void XMenuBar::Redraw()
{
	::InvalMenuBar();
}


//---------------------------------------------------------------
//
// XMenuBar::InstallMenu
//
//---------------------------------------------------------------
void XMenuBar::InstallMenu(const IMenuPtr& menu, const IMenuPtr& beforeMenu, bool redraw)
{
	PRECONDITION(!menu->IsInstalled());
	ASSERT_IF(menu->GetID() != 0, !this->GetMenuByID(menu->GetID()));	// make sure there's no id clash
	CHECK_INVARIANT;
	
	int16 beforeMenuID = 0;
	if (beforeMenu) 
		beforeMenuID = beforeMenu->GetOSMenuID();

	this->DoInstallMenu(menu, beforeMenuID, redraw);
		
	POSTCONDITION(this->GetCount() > 0);
}


//---------------------------------------------------------------
//
// XMenuBar::RemoveMenu
//
//---------------------------------------------------------------
void XMenuBar::RemoveMenu(const IMenuPtr& menu, bool redraw)
{
	PRECONDITION(menu->IsInstalled());
	CHECK_INVARIANT;
	
	std::vector<IMenuPtr>::iterator iter = std::find(mMenus.begin(), mMenus.end(), menu);
	if (iter != mMenus.end()) {		
		menu->RemoveFromMenuBar();
		::DeleteMenu(menu->GetOSMenuID());

#if GARBAGE_COLLECT_COM
		XGarbageCollector::Instance()->RemoveInterface(this->GetBoss(), iter->GetPtr());
#endif
	
		mMenus.erase(iter);

		if (redraw)
			::InvalMenuBar();
		
	} else
		DEBUGSTR("XMenuBar::RemoveMenu couldn't find the menu!");

	mIsValid = false;
		
	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// XMenuBar::GetCommandFromClick
//
//---------------------------------------------------------------
XMenuCommand XMenuBar::GetCommandFromClick(const EventRecord& event) const
{
	PRECONDITION(mIsValid);
	CHECK_INVARIANT;

	XMenuCommand cmd = kNothingCmd;
	
	int32 choice = MenuSelect(event.where);

	if (HiWord(choice) != 0)
		cmd = this->DoFindCommand(HiWord(choice), LoWord(choice) - 1UL);
			
	return cmd;
}


//---------------------------------------------------------------
//
// XMenuBar::GetCommandFromKey
//
//---------------------------------------------------------------
XMenuCommand XMenuBar::GetCommandFromKey(const EventRecord& event) const
{
	PRECONDITION(mIsValid);
	CHECK_INVARIANT;

	XMenuCommand cmd = kNothingCmd;
		
	uint32 choice;
	if (MSystemInfo::HasAppearanceMgr())
		choice = MenuEvent(&event);
	
	else {
		char ch = (char) (event.message & charCodeMask);
		choice = (uint32) MenuKey(ch);
	}

	if (HiWord(choice) != 0)
		cmd = this->DoFindCommand(HiWord(choice), LoWord(choice) - 1UL);
		
	else if (IsCommandPeriod(event))
		cmd = kAbortCmd;
			
	return cmd;
}


//---------------------------------------------------------------
//
// XMenuBar::AddCommand
//
//---------------------------------------------------------------
#if DEBUG
void XMenuBar::AddCommand(const XMenuCommand& cmd)			
{
	PRECONDITION(cmd != kNothingCmd); 
	PRECONDITION(mCommands.count(cmd) == 0); 
	
	mCommands.insert(cmd);
}
#endif


//---------------------------------------------------------------
//
// XMenuBar::RemoveCommand
//
//---------------------------------------------------------------
#if DEBUG
void XMenuBar::RemoveCommand(const XMenuCommand& cmd)			
{
	PRECONDITION(cmd != kNothingCmd); 

	std::set<XMenuCommand>::iterator iter = mCommands.find(cmd);
	PRECONDITION(iter != mCommands.end());

	mCommands.erase(iter);
}
#endif

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XMenuBar::Invariant
//
//---------------------------------------------------------------
void XMenuBar::Invariant() const
{
	IMenuBar::Invariant();
	
#if DEBUG
	if (gIntenseDebugging)
		ASSERT(mCommands.count(kNothingCmd) == 0);
#endif
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XMenuBar::DoFindCommand
//
//---------------------------------------------------------------
XMenuCommand XMenuBar::DoFindCommand(ResID menuID, uint32 item) const
{
	CHECK_INVARIANT;

	XMenuCommand command = kNothingCmd;
	
	std::vector<IMenuPtr> menus = mMenus;
	
	while (!menus.empty() && command == kNothingCmd) {
		IMenuPtr candidate = menus.back();
		menus.pop_back();	
				
		if (candidate->GetOSMenuID() == menuID) {
			command = candidate->GetCommand(item);
			ASSERT(command != kNothingCmd);
			
		} else {
			for (uint32 index = 0; index < candidate->GetCount(); ++index) {
				if (candidate->IsSubMenu(index))
					menus.push_back(candidate->GetSubMenu(index));
			}
		}
	}
		
	POSTCONDITION(command != kNothingCmd);
		
	return command;
}


//---------------------------------------------------------------
//
// XMenuBar::DoInstallMenu
//
//---------------------------------------------------------------
void XMenuBar::DoInstallMenu(const IMenuPtr& menu, int16 beforeMenuID, bool redraw)
{
	PRECONDITION(menu);
	PRECONDITION(beforeMenuID != hierMenu);
	PRECONDITION(!menu->IsInstalled());
	
	// Install the menu in our list.
	mMenus.push_back(menu);
	menu->AddToMenuBar(this);
	
#if GARBAGE_COLLECT_COM
	XBoss* boss = this->GetBoss();
	XGarbageCollector::Instance()->AssignInterface(boss, mMenus.back().GetPtr());
#endif
	
	// Install the menu in the menubar.
	MenuHandle menuH = menu->GetOSMenu(); 
	::InsertMenu(menuH, beforeMenuID);
	
	menu->AddedToMenuBar(this);

	// Invalidate the menubar.
	if (redraw)
		InvalMenuBar();		

	mIsValid = false;
}


}	// namespace Whisper
