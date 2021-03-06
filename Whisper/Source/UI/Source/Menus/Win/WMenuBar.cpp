/*
 *  File:       WMenuBar.cpp
 *  Summary:   	Cross platform wrapper around a top level menu.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WMenuBar.cpp,v $
 *		Revision 1.11  2001/04/27 09:19:23  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.10  2001/04/21 03:32:51  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.9  2001/03/16 10:31:38  jesjones
 *		Removed a bit of obsolete debugging code.
 *		
 *		Revision 1.8  2001/03/06 07:31:57  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.7  2001/03/03 23:58:21  jesjones
 *		Added calls to DEBUGSTR in catch (...) blocks.
 *		
 *		Revision 1.6  2000/12/31 04:43:57  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.5  2000/12/14 08:31:13  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.4  2000/12/10 04:07:20  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.3  2000/11/25 01:57:52  jesjones
 *		Dtor no longer calls Redraw.
 *		
 *		Revision 1.2  2000/11/09 12:10:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IMenuBar.h>

#include <algorithm>
#include <set>

#include <IActiveMenuBar.h>
#include <IMenu.h>
#include <WSystemInfo.h>
#include <XAutoPtr.h>
#include <XExceptions.h>
#include <XInterfacePtr.h>
#include <XMiscUtils.h>
#include <XNumbers.h>
#include <XResource.h>
#include <XStringUtils.h>

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
						
	virtual HMENU 		GetOSMenu() 									{return mMenuBarH;}

	virtual uint32 		GetCount() const								{return mMenus.size();}
			
	virtual IMenuPtr 	GetMenuByIndex(uint32 index) const;
			
	virtual IMenuPtr 	GetMenuByName(const std::wstring& name, bool recursive = kRecursive) const;
						
	virtual IMenuPtr 	GetMenuByID(uint32 id, bool recursive = kRecursive) const;

	virtual IMenuPtr 	GetMenuByHandle(OSMenu menuH, bool recursive = kRecursive) const;

	virtual void 		AppendMenu(const IMenuPtr& takeMenu, bool redraw = kRedraw)	{this->InstallMenu(takeMenu, IMenuPtr(), redraw);}

	virtual void 		InstallMenu(const IMenuPtr& takeMenu, const IMenuPtr& beforeMenu, bool redraw = kRedraw);

	virtual void 		RemoveMenu(const IMenuPtr& menu, bool redraw = kRedraw);
	
	virtual void 		Redraw();
	
	virtual bool		IsValid() const							{return mIsValid;}

	virtual void		Validate()								{mIsValid = true;}
	
#if DEBUG
	virtual void 		AddCommand(const XMenuCommand& cmd);

	virtual void 		RemoveCommand(const XMenuCommand& cmd);
#endif

	virtual	XMenuCommand FindCommand(int16 id);
	virtual	IMenu::SHelp FindHelp(int16 id);
	
	virtual int16 		GetMenuID(const XMenuCommand& command) const;
	virtual void 		RemoveMenuID(int16 id);
	virtual	void		SetHelp(int16 id, const IMenu::SHelp& help);
	
	virtual HACCEL 		GetAcceleratorTable();
	virtual void 		RebuildAcceleratorTable();
	virtual void 		DestroyAcceleratorTable();

protected:	
	virtual	void 		Invariant() const;

//-----------------------------------
//	Internal API
//
private:	
#if DEBUG
			void		DoValidateMnemonics() const;
#endif
	
//-----------------------------------
//	Types
//
protected:
	typedef std::map<int16, XMenuCommand>  CommandMap;
	typedef std::map<int16, IMenu::SHelp> HelpMap;

//-----------------------------------
//	Member Data
//
private:
	std::vector<IMenuPtr>	mMenus;
	bool					mIsValid;
	
#if DEBUG
	std::set<XMenuCommand>	mCommands;		// this is used to check that all installed menu items have unique menu commands
#endif

	HWND 					mWindow;
	HMENU					mMenuBarH;
	HACCEL					mAcceleratorTable;

	static int16			msNextCmdIndex;
	static CommandMap		msCommandMap;
	static HelpMap			msHelpMap;
};

int16 					XMenuBar::msNextCmdIndex = 1;
XMenuBar::CommandMap 	XMenuBar::msCommandMap;
XMenuBar::HelpMap		XMenuBar::msHelpMap;

DEFINE_INTERFACE_FACTORY(XMenuBar)

//---------------------------------------------------------------
//
// XMenuBar::~XMenuBar
//
//---------------------------------------------------------------
XMenuBar::~XMenuBar()
{
	CALL_INVARIANT;
	
	if (mMenuBarH != nil) {
		XInterfacePtr<IActiveMenuBar> activeMenuBar(L"Application");
		if (activeMenuBar->Get() == this)
			activeMenuBar->Set(IMenuBarPtr());
			
		this->RemoveMenus(kDontRedraw);
					
		if (mAcceleratorTable != nil)
			VERIFY(::DestroyAcceleratorTable(mAcceleratorTable));
	
		VERIFY(::DestroyMenu(mMenuBarH));
	
		mMenuBarH = nil;
		mWindow = nil;
		mAcceleratorTable = nil;
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
	mWindow = nil;
	
	mAcceleratorTable = nil;	
	mMenuBarH = nil;
}

				
//---------------------------------------------------------------
//
// XMenuBar::Init
//
//---------------------------------------------------------------
void XMenuBar::Init(OSWindow window)
{
	PRECONDITION(window != nil);
	PRECONDITION(mMenuBarH == nil);
	
	mWindow = window;
		
	mMenuBarH = ::CreateMenu();
	ThrowIfNil(mMenuBarH);

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

	mIsValid = false;

	if (redraw)		
		VERIFY(::DrawMenuBar(mWindow));

	POSTCONDITION(mCommands.empty());
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
	VERIFY(::DrawMenuBar(mWindow));
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
	COMPILE_CHECK(sizeof(MENUITEMINFOW) == sizeof(MENUITEMINFOA));
	CHECK_INVARIANT;
	
	uint32 count = this->GetCount();
	uint32 atIndex = count;
	if (beforeMenu) {
		for (uint32 index = 0; index < count && atIndex == count; ++index) 
			if (mMenus[index] == beforeMenu)	
				atIndex = index;
	}
	
	std::wstring name = menu->GetName(false);			// use a local because a temporary would be destroyed to early

	PlatformString str = ToPlatformStr(name);

	MENUITEMINFOW info;
	info.cbSize        = sizeof(info);
	info.fMask         = MIIM_TYPE + MIIM_SUBMENU;
	info.fType         = MFT_STRING;
	info.fState        = 0;
	info.wID           = 0;
	info.hSubMenu      = menu->GetOSMenu();
	info.hbmpChecked   = nil;
	info.hbmpUnchecked = nil;
	info.dwItemData    = 0;
	info.dwTypeData    = WSystemInfo::HasUnicode() ? (wchar_t*) name.c_str() : (wchar_t*) str.c_str();			
	info.cch           = 0;
	
	if (WSystemInfo::HasUnicode()) {
		int32 succeeded = ::InsertMenuItemW(mMenuBarH, atIndex, true, &info);	
		ThrowIf(!succeeded);
		
	} else {
		int32 succeeded = ::InsertMenuItemA(mMenuBarH, atIndex, true, (MENUITEMINFOA*) &info);	
		ThrowIf(!succeeded);
	}
	
	menu->AddToMenuBar(this);
	mMenus.push_back(menu);
		
	if (redraw)		
		VERIFY(::DrawMenuBar(mWindow));

	mIsValid = false;

#if DEBUG
	this->DoValidateMnemonics();
#endif

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
		
		VERIFY(::RemoveMenu(mMenuBarH, numeric_cast<uint32>(iter - mMenus.begin()), MF_BYPOSITION));
		mMenus.erase(iter);

		if (redraw)		
			VERIFY(::DrawMenuBar(mWindow));
		
	} else
		DEBUGSTR("XMenuBar::RemoveMenu couldn't find the menu!");

	mIsValid = false;
}


//---------------------------------------------------------------
//
// XMenuBar::GetAcceleratorTable
//
//---------------------------------------------------------------
HACCEL XMenuBar::GetAcceleratorTable()
{	
	return mAcceleratorTable;
}


//---------------------------------------------------------------
//
// XMenuBar::RebuildAcceleratorTable
//
//---------------------------------------------------------------
void XMenuBar::RebuildAcceleratorTable()
{	
	if (mAcceleratorTable != nil) {
		VERIFY(::DestroyAcceleratorTable(mAcceleratorTable));
		mAcceleratorTable = nil;
	}
	
	try {				
		std::vector<ACCEL> table;
		table.reserve(5*mMenus.size());
		
		std::vector<IMenuPtr> menus = mMenus;
		
		while (!menus.empty()) {
			IMenuPtr menu = menus.back();
			menus.pop_back();	
					
			menu->AppendAccelerators(table);
				
			for (uint32 index = 0; index < menu->GetCount(); ++index) {
				if (menu->IsSubMenu(index))
					menus.push_back(menu->GetSubMenu(index));
			}
		}

		if (!table.empty()) {
			mAcceleratorTable = ::CreateAcceleratorTable(&(table[0]), numeric_cast<int32>(table.size()));
			ASSERT(mAcceleratorTable != nil);		// don't throw for this
		}
		
	} catch (...) {
		DEBUGSTR("Got an exception in XMenuBar::RebuildAcceleratorTable");	
	}
}


//---------------------------------------------------------------
//
// XMenuBar::DestroyAcceleratorTable
//
//---------------------------------------------------------------
void XMenuBar::DestroyAcceleratorTable()
{
	if (mAcceleratorTable != nil) {
		VERIFY(::DestroyAcceleratorTable(mAcceleratorTable));
		mAcceleratorTable = nil;
	}
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


//---------------------------------------------------------------
//
// XMenuBar::FindCommand
//
//---------------------------------------------------------------
XMenuCommand XMenuBar::FindCommand(int16 id)
{
	XMenuCommand command = kNothingCmd;
	
	CommandMap::iterator iter = msCommandMap.find(id);
	if (iter != msCommandMap.end())
		command = iter->second;
		
	return command;
}


//---------------------------------------------------------------
//
// XMenuBar::FindHelp						
//
//---------------------------------------------------------------
IMenu::SHelp XMenuBar::FindHelp(int16 id)
{
	IMenu::SHelp help;
	
	HelpMap::iterator iter = msHelpMap.find(id);
	if (iter != msHelpMap.end())
		help = iter->second;
		
	return help;
}


//---------------------------------------------------------------
//
// XMenuBar::GetMenuID
//
//---------------------------------------------------------------
int16 XMenuBar::GetMenuID(const XMenuCommand& command) const
{
	int16 id = 0;
	
	if (command != kNothingCmd) {				
		while (msCommandMap.count(msNextCmdIndex) > 0 || msNextCmdIndex == 0 || msNextCmdIndex == -1)	// id is only 16 bits so there's a slim chance that we may wrap around...
			++msNextCmdIndex;
			
		id = msNextCmdIndex++;
		msCommandMap.insert(CommandMap::value_type(id, command));
	}
	
	return id;
}


//---------------------------------------------------------------
//
// XMenuBar::RemoveMenuID
//
//---------------------------------------------------------------
void XMenuBar::RemoveMenuID(int16 id)
{
	CommandMap::iterator iter = msCommandMap.find(id);
	if (iter != msCommandMap.end())
		msCommandMap.erase(iter);			

	HelpMap::iterator iter2 = msHelpMap.find(id);
	if (iter2 != msHelpMap.end())
		msHelpMap.erase(iter2);				
}


//---------------------------------------------------------------
//
// XMenuBar::SetHelp
//
//---------------------------------------------------------------
void XMenuBar::SetHelp(int16 id, const IMenu::SHelp& help)
{
	msHelpMap[id] = help;
}

#if __MWERKS__
#pragma mark �
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
	if (gIntenseDebugging) {
		ASSERT(mCommands.count(kNothingCmd) == 0);

		this->DoValidateMnemonics();
	}
#endif
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XMenuBar::DoValidateMnemonics
//
//---------------------------------------------------------------
#if DEBUG
void XMenuBar::DoValidateMnemonics() const
{
	uint32 count = this->GetCount();

	std::vector<wchar_t> mnemonics;		// there won't be too many menus so we can get by with a vector
	mnemonics.reserve(count);

	for (uint32 index = 0; index < count; ++index) {
		IMenuPtr menu = mMenus[index];

		std::wstring name = menu->GetName(false);
		wchar_t mnemonic = menu->GetMnemonic(name);
		if (mnemonic != 0) {
			std::vector<wchar_t>::iterator iter = std::find(mnemonics.begin(), mnemonics.end(), mnemonic);
			if (iter == mnemonics.end())
				mnemonics.push_back(mnemonic);
			else
				DEBUGSTR("The menu mnemonic '", mnemonic, "' has a duplicate");
		}
	}
}
#endif


}	// namespace Whisper
