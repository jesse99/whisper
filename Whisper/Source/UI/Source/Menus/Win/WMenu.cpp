/*
 *  File:       WMenu.cpp
 *  Summary:   	Cross platform menu class.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WMenu.cpp,v $
 *		Revision 1.13  2001/04/27 09:19:16  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.12  2001/04/21 03:32:43  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.11  2001/04/17 01:42:17  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.10  2001/04/13 08:00:45  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.9  2000/12/31 04:43:38  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.8  2000/12/27 01:05:01  jesjones
 *		Tweaked command id code to work without a menubar.
 *		
 *		Revision 1.7  2000/12/25 02:47:41  jesjones
 *		Tweaked so that context menus no longer rely on a menubar.
 *		
 *		Revision 1.6  2000/12/24 03:20:33  jesjones
 *		Can now call Init with a zero length name (useful for context menus).
 *		
 *		Revision 1.5  2000/12/14 08:31:03  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.4  2000/12/10 04:07:10  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.3  2000/11/20 05:49:31  jesjones
 *		Added a cheesy overload work around.
 *		
 *		Revision 1.2  2000/11/09 12:10:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <7>	 8/12/00	JDJ		Fixed InsertSortedItem so that more then one item can be added
 *									at the end of the menu.
 *		 <6>	 6/09/99	JDJ		Corrected spelling of "separator".
 *		 <5>	 5/15/99	JDJ		Added AppendFonts. 
 *		 <4>	 4/06/99	JDJ		Added DoValidateMnemonics. Rewrote InsertItem accelerator 
 *									text generation code to correctly handle multiple modifier 
 *									keys
 *		 <3>	 3/30/99	JDJ		Help messages can now be specified when items are added. 
 *		 <2>	 3/20/99	JDJ		Only calls RebuildAcceleratorTable if the menubar
 *									is the active menubar.
 *		 <1>	10/10/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IMenu.h>

#include <set>

#include <IActiveMenuBar.h>
#include <IMenuBar.h>
#include <WSystemInfo.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XFont.h>
#include <XIntConversions.h>
#include <XInterfaceMacros.h>
#include <XInterfacePtr.h>
#include <XMemUtils.h>
#include <XNumbers.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	class XMenu
//!		Cross platform menu class.
// ===================================================================================
class XMenu : public IMenu {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XMenu();
	
						XMenu(XBoss* boss);

	virtual void 		Init(const std::wstring& name, const SHelp& titleHelp = SHelp(), const SHelp& missingHelp = SHelp(), uint32 id = 0);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual std::wstring GetName(bool stripAmpersands = true) const;
	virtual void 		SetName(const std::wstring& inName, bool redraw = kRedraw);
	
	virtual uint32 		GetCount() const						{return mMenuCommands.size();}
			
	virtual void 		Enable(bool enable = true, bool redraw = kRedraw);

	virtual void 		RemoveItems(bool rebuildTables = true);

	virtual bool 		IsSubMenu() const						{return mParent;}

	virtual bool		IsInstalled() const;
						
	virtual IMenuBarPtr GetMenuBar() const;
						
	virtual uint32 		GetID() const							{return mMenuID;}
		
	virtual OSMenu 		GetOSMenu() const						{return mMenuH;}
												
	virtual std::wstring GetName(uint32 index, bool stripAmpersands = true) const;

	virtual void 		SetName(uint32 index, const std::wstring& name);
			
	virtual void 		RemoveItem(uint32 index, bool rebuildTables = true);

	virtual bool 		IsSeparator(uint32 index) const;

	virtual void 		AppendSeparator(bool rebuildTables = true);
				
	virtual void 		InsertSeparator(uint32 atIndex, bool rebuildTables = true);

	virtual XMenuCommand GetCommand(uint32 index) const;
			
	virtual void 		EnableItem(uint32 index, bool enable = true);
			
	virtual void 		CheckItem(uint32 index, uint32 mark);

	virtual void 		SetItem(uint32 index, const std::wstring& name, const XMenuCommand& cmd);
			
	virtual void 		AppendItem(const std::wstring& name, const XMenuCommand& cmd, const SHelp& help = SHelp(), const SAccelerator& accelerator = 0, bool rebuildTables = true);
				
	virtual void 		InsertItem(const std::wstring& name, uint32 atIndex, const XMenuCommand& cmd, const SHelp& help = SHelp(), const SAccelerator& accelerator = 0, bool rebuildTables = true);
			
	virtual uint32 		InsertSortedItem(const std::wstring& name, uint32 startItem, uint32 count, const XMenuCommand& cmd, const SHelp& help = SHelp(), bool rebuildTables = true);

	virtual void 		AppendFonts(OSWindow window, const XMenuCommand& cmdPrefix, bool rebuildTables = true);
	
	virtual bool 		IsSubMenu(uint32 index) const;
	
	virtual IMenuPtr	GetSubMenu(uint32 index) const;

	virtual void 		AppendSubMenu(const IMenuPtr& subMenu, bool rebuildTables = true);
				
	virtual void 		InsertSubMenu(uint32 atIndex, const IMenuPtr& subMenu, bool rebuildTables = true);

	virtual IMenuPtr 	GetParent()						{return mParent;}
	virtual void 		SetParent(IMenuPtr parent)		{mParent = parent;}

	virtual	void 		AddToMenuBar(IMenuBarPtr menubar);
	virtual void 		RemoveFromMenuBar();

	virtual SHelp		GetHelp() const					{return mTitleHelp;}

#if DEBUG
	virtual void		AddMnemonics(std::vector<wchar_t>& mnemonics) const;
	
	virtual	wchar_t		GetMnemonic(const std::wstring& name) const;
#endif

	virtual void 		AppendAccelerators(std::vector<ACCEL>& table);

protected:
	virtual	void 		Invariant() const;

//-----------------------------------
//	Internal API
//
private:						
			void 		DoRemoveItem(uint32 index, bool deleteMenu, bool rebuildTables);
					
			void 		DoInsertItem(const XMenuCommand& cmd, uint32 at, const SAccelerator& accelerator);
			
			HMENU 		DoGetParent() const;

			uint32 		DoGetIndex(HMENU parent) const;

#if DEBUG
			void		DoValidateMnemonics() const;
#endif

//-----------------------------------
//	Member Data
//
protected:
	OSMenu	 					mMenuH;
	IMenuPtr					mParent;		// non-nil for sub-menus
	IMenuBarPtr					mMenuBar;		// nil for uninstalled menus and submenus
	uint32						mMenuID;
	
	std::wstring				mName;
	
	SHelp						mTitleHelp;
	SHelp						mMissingHelp;

	std::vector<IMenuPtr>		mSubMenus;
	std::vector<XMenuCommand>	mMenuCommands;	
	std::vector<SAccelerator>	mAccelerators;
};

DEFINE_INTERFACE_FACTORY(XMenu)

//---------------------------------------------------------------
//
// XMenu::~XMenu
//
//---------------------------------------------------------------
XMenu::~XMenu()
{
	if (mMenuH != nil) {
		this->RemoveItems(false);			// delete all the submenus
		
		if (mParent) {
			uint32 index = ULONG_MAX;
			uint32 count = mParent->GetCount();
			
			for (uint32 candidate = 0; candidate < count && index == ULONG_MAX; ++candidate) {
				if (mParent->IsSubMenu(candidate) && mParent->GetSubMenu(candidate) == this)
					index = candidate;
			}
			
			mParent->RemoveItem(index);
		}
		
		VERIFY(DestroyMenu(mMenuH));
	}
}


//---------------------------------------------------------------
//
// XMenu::XMenu 
//
//---------------------------------------------------------------
XMenu::XMenu(XBoss* boss) 
{	
	this->DoSetBoss(boss);

	mMenuH = nil;
}


//---------------------------------------------------------------
//
// XMenu::Init
//
//---------------------------------------------------------------
void XMenu::Init(const std::wstring& name, const SHelp& titleHelp, const SHelp& missingHelp, uint32 myID)
{	
	mName = name;
	mTitleHelp = titleHelp;
	mMissingHelp = missingHelp;
			
	mMenuID = myID;
	
	mMenuH = CreatePopupMenu();
	ThrowIfNil(mMenuH);
	
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XMenu::GetName (bool)
//
//---------------------------------------------------------------
std::wstring XMenu::GetName(bool stripAmpersands) const
{
	std::wstring name = mName;

	if (stripAmpersands)
		name = Whisper::Replace(name, '&', 0, '&');
	
	return name;
}


//---------------------------------------------------------------
//
// XMenu::Enable
//
//---------------------------------------------------------------
void XMenu::Enable(bool enable, bool redraw)
{
	CHECK_INVARIANT;
	
	HMENU parent = this->DoGetParent();
	if (parent != nil) {
		uint32 index = this->DoGetIndex(parent);
		uint32 flags = (uint32) (MF_BYPOSITION + (enable ? MFS_ENABLED : MFS_GRAYED));
		
		int32 oldState = ::EnableMenuItem(parent, index, flags);
		ThrowIf(oldState == 0xFFFFFFFF);

		if (redraw && oldState != (enable ? MFS_ENABLED : MFS_GRAYED) - MF_BYPOSITION)
			if (mMenuBar)
				mMenuBar->Redraw();
	}
}


//---------------------------------------------------------------
//
// XMenu::SetName (wstring)
//
//---------------------------------------------------------------
void XMenu::SetName(const std::wstring& name, bool redraw)
{
	CHECK_INVARIANT;
	
	COMPILE_CHECK(sizeof(MENUITEMINFOW) == sizeof(MENUITEMINFOA));
	
	if (name != mName) {
		HMENU parent = this->DoGetParent();
		if (parent != nil) {
			uint32 index = this->DoGetIndex(parent);
			
			PlatformString str = ToPlatformStr(name);

			MENUITEMINFOW info;
			info.cbSize        = sizeof(info);
			info.fMask         = MIIM_TYPE;
			info.fType         = MFT_STRING;
			info.fState        = 0;
			info.wID           = 0;
			info.hSubMenu      = nil;
			info.hbmpChecked   = nil;
			info.hbmpUnchecked = nil;
			info.dwItemData    = 0;
			info.dwTypeData    = WSystemInfo::HasUnicode() ? (wchar_t*) name.c_str() : (wchar_t*) str.c_str();			
			info.cch           = 0;
			
			if (WSystemInfo::HasUnicode()) {
				int32 succeeded = ::SetMenuItemInfoW(parent, index, true, &info);	
				ThrowIf(!succeeded);
				
			} else {
				int32 succeeded = ::SetMenuItemInfoA(parent, index, true, (MENUITEMINFOA*) &info);	
				ThrowIf(!succeeded);
			}
	
			if (redraw)
				if (mMenuBar)
					mMenuBar->Redraw();	
		}
				
		mName = name;

		if (redraw)
			if (mMenuBar)
				mMenuBar->Redraw();
	}
		
	POSTCONDITION(this->GetName() == name);
}


//---------------------------------------------------------------
//
// XMenu::RemoveItems
//
//---------------------------------------------------------------
void XMenu::RemoveItems(bool rebuildTables)
{
	CHECK_INVARIANT;
		
	uint32 count = this->GetCount();
	
	while (count > 0)
		this->RemoveItem(--count, false);		// we'll remove items from the back since mMenuCommands is a vector

	if (rebuildTables) {
		IMenuBarPtr menubar = this->GetMenuBar();
	
		IActiveMenuBarPtr activeMenuBar(L"Application");
		if (activeMenuBar->Get() == menubar && menubar)
			menubar->RebuildAcceleratorTable();
	}
	
	POSTCONDITION(this->GetCount() == 0);
}


//---------------------------------------------------------------
//
// XMenu::IsInstalled
//
//---------------------------------------------------------------
bool XMenu::IsInstalled() const						
{
	bool installed = mMenuBar;
	
	IMenuPtr parent = mParent;
	while (!installed && parent) {
		installed = parent->IsInstalled();
		parent = parent->GetParent();
	}
	
	return installed;
}


//---------------------------------------------------------------
//
// XMenu::GetMenuBar
//
//---------------------------------------------------------------
IMenuBarPtr XMenu::GetMenuBar() const
{
	IMenuBarPtr menubar = mMenuBar;
	
	if (!menubar && mParent)
		menubar = mParent->GetMenuBar();
	
	return menubar;
}


//---------------------------------------------------------------
//
// XMenu::GetName (uint32, bool)
//
//---------------------------------------------------------------
std::wstring XMenu::GetName(uint32 index, bool stripAmpersands) const
{
	PRECONDITION(index < this->GetCount());
	PRECONDITION(!this->IsSeparator(index));
	
	// Get the menu item text.
	std::wstring name;

	if (WSystemInfo::HasUnicode()) {
		wchar_t buffer[1024];
		int32 count = ::GetMenuStringW(mMenuH, index, buffer, sizeof(buffer), MF_BYPOSITION);
		ThrowIf(count == 0);
		
		name.assign(buffer, numeric_cast<uint32>(count));
	
	} else {
		char buffer[1024];
		int32 count = ::GetMenuStringA(mMenuH, index, buffer, sizeof(buffer), MF_BYPOSITION);
		ThrowIf(count == 0);
		
		name = FromPlatformStr(buffer, numeric_cast<uint32>(count));
	}
		
	// Strip out the ampersands.
	if (stripAmpersands)
		name = Whisper::Replace(name, '&', 0, '&');
	
	// Remove the accelerator text.
	if (mAccelerators[index] != 0) {
		uint32 pos = name.rfind('\t');
		ASSERT(pos != std::wstring::npos);
		
		name = name.substr(0, pos);
	}

	return name;
}


//---------------------------------------------------------------
//
// XMenu::SetName (uint32, wstring)
//
//---------------------------------------------------------------
void XMenu::SetName(uint32 index, const std::wstring& inName)
{
	PRECONDITION(index < this->GetCount());
	PRECONDITION(!this->IsSeparator(index));
	CHECK_INVARIANT;
	
	if (inName != this->GetName(false)) {
		std::wstring name = inName;

		// If the menu item has an accelerator we need to tack on the
		// accelerator text to the new name.
		if (mAccelerators[index] != 0) {
			std::wstring oldName;

			if (WSystemInfo::HasUnicode()) {
				wchar_t buffer[1024];
				int32 count = ::GetMenuStringW(mMenuH, index, buffer, sizeof(buffer), MF_BYPOSITION);
				ThrowIf(count == 0);
				
				oldName.assign(buffer, numeric_cast<uint32>(count));
			
			} else {
				char buffer[1024];
				int32 count = ::GetMenuStringA(mMenuH, index, buffer, sizeof(buffer), MF_BYPOSITION);
				ThrowIf(count == 0);
				
				oldName = FromPlatformStr(buffer, numeric_cast<uint32>(count));
			}
			
			uint32 pos = oldName.rfind('\t');
			ASSERT(pos != std::wstring::npos);
			
			name += oldName.substr(pos);
		}

		// Set the menu item text.
		COMPILE_CHECK(sizeof(MENUITEMINFOW) == sizeof(MENUITEMINFOA));
		
		PlatformString str = ToPlatformStr(name);

		MENUITEMINFOW info;
		info.cbSize        = sizeof(info);
		info.fMask         = MIIM_TYPE;
		info.fType         = MFT_STRING;
		info.fState        = 0;
		info.wID           = 0;
		info.hSubMenu      = nil;
		info.hbmpChecked   = nil;
		info.hbmpUnchecked = nil;
		info.dwItemData    = 0;
		info.dwTypeData    = WSystemInfo::HasUnicode() ? (wchar_t*) name.c_str() : (wchar_t*) str.c_str();			
		info.cch           = 0;
		
		if (WSystemInfo::HasUnicode()) {
			int32 succeeded = ::SetMenuItemInfoW(mMenuH, index, true, &info);	
			ThrowIf(!succeeded);
			
		} else {
			int32 succeeded = ::SetMenuItemInfoA(mMenuH, index, true, (MENUITEMINFOA*) &info);	
			ThrowIf(!succeeded);
		}

#if DEBUG
		this->DoValidateMnemonics();
#endif
	}

	POSTCONDITION(this->GetName(index, false) == inName);
}


//---------------------------------------------------------------
//
// XMenu::RemoveItem
//
//---------------------------------------------------------------
void XMenu::RemoveItem(uint32 index, bool rebuildTables)
{
	PRECONDITION(index < this->GetCount());
	CHECK_INVARIANT;
		
	if (this->IsSubMenu(index)) {
		IMenuPtr subMenu = this->GetSubMenu(index);
		
		subMenu->RemoveItems(false);	

		if (this->IsInstalled())
			subMenu->RemoveFromMenuBar();	

	} else if (rebuildTables && mAccelerators[index].ch == 0 && mAccelerators[index].key == 0)
		rebuildTables = false;

	this->DoRemoveItem(index, false, rebuildTables);
	VERIFY(::RemoveMenu(mMenuH, index, MF_BYPOSITION));
}


//---------------------------------------------------------------
//
// XMenu::IsSeparator 
//
//---------------------------------------------------------------
bool XMenu::IsSeparator(uint32 index) const
{
	PRECONDITION(index < this->GetCount());
	
	MENUITEMINFO info;
	info.cbSize        = sizeof(info);
	info.fMask         = MIIM_TYPE;
	info.fType         = 0;
	info.fState        = 0;
	info.wID           = 0;
	info.hSubMenu      = nil;
	info.hbmpChecked   = nil;
	info.hbmpUnchecked = nil;
	info.dwItemData    = 0;
	info.dwTypeData    = nil;			// actually an LPTSTR...
	info.cch           = 0;
	
	int32 succeeded = ::GetMenuItemInfo(mMenuH, index, true, &info);
	ThrowIf(!succeeded);
	
	bool isSep = info.fType == MFT_SEPARATOR;
	
	return isSep;
}


//---------------------------------------------------------------
//
// XMenu::AppendSeparator 
//
//---------------------------------------------------------------
void XMenu::AppendSeparator(bool rebuildTables)
{
	CHECK_INVARIANT;
	
	this->InsertSeparator(this->GetCount(), rebuildTables);
}


//---------------------------------------------------------------
//
// XMenu::InsertSeparator 
//
//---------------------------------------------------------------
void XMenu::InsertSeparator(uint32 atIndex, bool rebuildTables)
{
	PRECONDITION(atIndex <= this->GetCount());
	UNUSED(rebuildTables);
	CHECK_INVARIANT;
		
	MENUITEMINFO info;
	info.cbSize        = sizeof(info);
	info.fMask         = MIIM_TYPE;
	info.fType         = MFT_SEPARATOR;
	info.fState        = 0;
	info.wID           = 0;
	info.hSubMenu      = nil;
	info.hbmpChecked   = nil;
	info.hbmpUnchecked = nil;
	info.dwItemData    = 0;
	info.dwTypeData    = nil;			// actually an LPTSTR...
	info.cch           = 0;
	
	int32 succeeded = ::InsertMenuItem(mMenuH, atIndex, true, &info);
	ThrowIf(!succeeded);
	
	this->DoInsertItem(kNothingCmd, atIndex, 0);
}


//---------------------------------------------------------------
//
// XMenu::GetCommand
//
//---------------------------------------------------------------
XMenuCommand XMenu::GetCommand(uint32 index) const
{
	PRECONDITION(index < this->GetCount());
	
	XMenuCommand command = kNothingCmd;		

	if (!this->IsSubMenu(index))
		command = mMenuCommands[index];		
	
	return command;
}


//---------------------------------------------------------------
//
// XMenu::EnableItem
//
//---------------------------------------------------------------
void XMenu::EnableItem(uint32 index, bool enable)
{
	PRECONDITION(index < this->GetCount());
	PRECONDITION(!this->IsSeparator(index));
	CHECK_INVARIANT;
	
	uint32 flags = (uint32) (MF_BYPOSITION + (enable ? MFS_ENABLED : MFS_GRAYED));
	
	int32 oldState = ::EnableMenuItem(mMenuH, index, flags);
	ThrowIf(oldState == 0xFFFFFFFF);
}


//---------------------------------------------------------------
//
// XMenu::CheckItem
//
//---------------------------------------------------------------
void XMenu::CheckItem(uint32 index, uint32 style)
{
	PRECONDITION(index < this->GetCount());
	PRECONDITION(!this->IsSubMenu(index));
	PRECONDITION(!this->IsSeparator(index));
	CHECK_INVARIANT;
		
	if (style & kExclusiveCheckMark) {
		int32 succeeded = ::CheckMenuRadioItem(mMenuH, index, index, index, MF_BYPOSITION);
		ThrowIf(!succeeded);
		
	} else {
		uint32 flags = (uint32) (MF_BYPOSITION + (style == kNoCheckmark ? MF_UNCHECKED : MF_CHECKED));
		
		uint32 oldState = ::CheckMenuItem(mMenuH, index, flags);
		ThrowIf(oldState == 0xFFFFFFFF);
	}
}


//---------------------------------------------------------------
//
// XMenu::SetItem
//
//---------------------------------------------------------------
void XMenu::SetItem(uint32 index, const std::wstring& str, const XMenuCommand& cmd)
{
	PRECONDITION(index < this->GetCount());
	PRECONDITION(!this->IsSubMenu(index));
	CHECK_INVARIANT;
	
	this->RemoveItem(index);
	this->InsertItem(str, index, cmd);
}


//---------------------------------------------------------------
//
// XMenu::AppendItem 
//
//---------------------------------------------------------------
void XMenu::AppendItem(const std::wstring& name, const XMenuCommand& cmd, const SHelp& help, const SAccelerator& accelerator, bool rebuildTables)
{
	PRECONDITION(name != L"(-");					// use AppendSeparator instead
	CHECK_INVARIANT;
	
	this->InsertItem(name, this->GetCount(), cmd, help, accelerator, rebuildTables);
}


//---------------------------------------------------------------
//
// XMenu::InsertItem 
//
//---------------------------------------------------------------
void XMenu::InsertItem(const std::wstring& inName, uint32 atIndex, const XMenuCommand& cmd, const SHelp& help, const SAccelerator& accelerator, bool rebuildTables)
{
	PRECONDITION(atIndex <= this->GetCount());
	PRECONDITION(accelerator.ch == 0 || accelerator.key == 0);	// only one of these should be specified!
	CHECK_INVARIANT;
	
	std::wstring name = inName;
	
	// Append the accelerator text.
	if (accelerator.ch != 0 || accelerator.key != 0) {
		ASSERT_IF(accelerator.key != 0, accelerator.text.length() > 0);
			
		name += L"\t";

		if (accelerator.text.length() > 0)
			name += accelerator.text;

		else {
			ASSERT(accelerator.key == 0);

			std::wstring suffix;

			if (accelerator.modifiers & kShiftModifer)
				suffix += LoadWhisperString(L"Shift (accelerator)");
				
			if (accelerator.modifiers & kControlModifier) {
				if (suffix.length() > 0)
					suffix += '+';
				suffix += LoadWhisperString(L"Ctrl (accelerator)");
			}

			if (accelerator.modifiers & kAltModifier) {
				if (suffix.length() > 0)
					suffix += '+';
				suffix += LoadWhisperString(L"Alt (accelerator)");
			}

			if (suffix.length() > 0)
				suffix += '+';
			suffix += accelerator.ch;

			name += suffix;
		}
	}
	
	// Insert the menu item.
	COMPILE_CHECK(sizeof(MENUITEMINFOW) == sizeof(MENUITEMINFOA));

	IMenuBarPtr menubar = this->GetMenuBar();

	PlatformString str = ToPlatformStr(name);

	MENUITEMINFOW info;
	info.cbSize        = sizeof(info);
	info.fMask         = MIIM_TYPE + MIIM_ID;
	info.fType         = MFT_STRING;
	info.fState        = 0;
	info.wID           = menubar ? menubar->GetMenuID(cmd) : atIndex+1;	// context menus dont have a menubar
	info.hSubMenu      = nil;
	info.hbmpChecked   = nil;
	info.hbmpUnchecked = nil;
	info.dwItemData    = 0;
	info.dwTypeData    = WSystemInfo::HasUnicode() ? (wchar_t*) name.c_str() : (wchar_t*) str.c_str();			
	info.cch           = 0;
	
	if (WSystemInfo::HasUnicode()) {
		int32 succeeded = ::InsertMenuItemW(mMenuH, atIndex, true, &info);	
		ThrowIf(!succeeded);
		
	} else {
		int32 succeeded = ::InsertMenuItemA(mMenuH, atIndex, true, (MENUITEMINFOA*) &info);	
		ThrowIf(!succeeded);
	}
	
	// Update some member variables.
	if (help.enabled.length() > 0) {
		this->DoInsertItem(cmd, atIndex, accelerator);
		if (menubar)
			menubar->SetHelp((int16) info.wID, help);
	
	} else {
		this->DoInsertItem(cmd, atIndex, accelerator);
		if (menubar)
			menubar->SetHelp((int16) info.wID, mMissingHelp);
	}
		
	// Rebuild the accelerator table as neccesary.
	if (rebuildTables && (accelerator.ch != 0 || accelerator.key != 0)) {		
		IActiveMenuBarPtr activeMenuBar(L"Application");
		if (activeMenuBar->Get() == menubar && menubar)
			menubar->RebuildAcceleratorTable();
	}
		
#if DEBUG
	this->DoValidateMnemonics();
#endif

	POSTCONDITION(true);
}

	
//---------------------------------------------------------------
//
// XMenu::InsertSortedItem
//
//---------------------------------------------------------------
uint32 XMenu::InsertSortedItem(const std::wstring& itemStr, uint32 startItem, uint32 count, const XMenuCommand& cmd, const SHelp& help, bool rebuildTables)
{
	PRECONDITION(startItem <= this->GetCount());
	CHECK_INVARIANT;

	uint32 atIndex;
	
	if (count > 0) {
		std::vector<std::wstring> items;
		items.reserve(count);
		
		// Fill our array with the items we're going to stick itemStr 
		// into. Note that these items should already be sorted.
		uint32 max = std::min(count, this->GetCount() - startItem);
		for (uint32 i = 0; i < max; ++i) {			
			items.push_back(this->GetName(startItem + i));
			
			ASSERT(i == 0 || items[i-1] <= items[i]);		
		}
					
		// Find the last position at which we can insert itemStr.
		std::vector<std::wstring>::iterator iter = std::upper_bound(items.begin(), items.end(), itemStr);

		// Because vector uses random access iterators we can get the 
		// index at which we need to stick itemStr by simply subtracting 
		// the iterators.
		int32 at = iter - items.begin();
		
		atIndex = startItem + at;
		
	} else
		atIndex = startItem;
		
	this->InsertItem(itemStr, atIndex, cmd, help, 0, rebuildTables);

	POSTCONDITION(true);
	
	return atIndex;
}


//---------------------------------------------------------------
//
// XMenu::AppendFonts
//
//---------------------------------------------------------------
void XMenu::AppendFonts(OSWindow window, const XMenuCommand& cmdPrefix, bool rebuildTables)
{
	UNUSED(rebuildTables);					// no accelerators were added so we can skip this
	
	OSDrawContext dc = ::GetDC(window);
	ThrowIfNil(dc);		
	
	try {
		uint32 count = 0;
		
		XFont::iterator iter = XFont::begin(dc);
		while (iter != XFont::end(dc)) {
			std::wstring name = *iter;
			++iter;
			
			this->InsertSortedItem(name, 0, count++, cmdPrefix + name, SHelp(), false);
		}	

	} catch (...) {
		ReleaseDC(window, dc);
		throw;
	}
	
	ReleaseDC(window, dc);
}


//---------------------------------------------------------------
//
// XMenu::IsSubMenu
//
//---------------------------------------------------------------
bool XMenu::IsSubMenu(uint32 index) const
{
	PRECONDITION(index < this->GetCount());
	
	bool isSub = mSubMenus[index];
		
	return isSub;
}


//---------------------------------------------------------------
//
// XMenu::GetSubMenu
//
//---------------------------------------------------------------
IMenuPtr XMenu::GetSubMenu(uint32 index) const
{
	IMenuPtr subMenu = mSubMenus[index];
	PRECONDITION(subMenu);
	
	return subMenu;
}


//---------------------------------------------------------------
//
// XMenu::AppendSubMenu 
//
//---------------------------------------------------------------
void XMenu::AppendSubMenu(const IMenuPtr& subMenu, bool rebuildTables)
{
	CHECK_INVARIANT;
	
	this->InsertSubMenu(this->GetCount(), subMenu, rebuildTables);
}

	
//---------------------------------------------------------------
//
// XMenu::InsertSubMenu
//
//---------------------------------------------------------------
void XMenu::InsertSubMenu(uint32 atIndex, const IMenuPtr& subMenu, bool rebuildTables)
{
	PRECONDITION(atIndex <= this->GetCount());
	COMPILE_CHECK(sizeof(MENUITEMINFOW) == sizeof(MENUITEMINFOA));
	CHECK_INVARIANT;

	std::wstring name = subMenu->GetName(false);	// use a local since a temp will disappear
	
	PlatformString str = ToPlatformStr(name);

	MENUITEMINFOW info;
	info.cbSize        = sizeof(info);
	info.fMask         = MIIM_TYPE + MIIM_SUBMENU;
	info.fType         = MFT_STRING;
	info.fState        = 0;
	info.wID           = 0;
	info.hSubMenu      = subMenu->GetOSMenu();
	info.hbmpChecked   = nil;
	info.hbmpUnchecked = nil;
	info.dwItemData    = 0;
	info.dwTypeData    = WSystemInfo::HasUnicode() ? (wchar_t*) name.c_str() : (wchar_t*) str.c_str();			
	info.cch           = 0;
	
	if (WSystemInfo::HasUnicode()) {
		int32 succeeded = ::InsertMenuItemW(mMenuH, atIndex, true, &info);	
		ThrowIf(!succeeded);
		
	} else {
		int32 succeeded = ::InsertMenuItemA(mMenuH, atIndex, true, (MENUITEMINFOA*) &info);	
		ThrowIf(!succeeded);
	}
	
	subMenu->SetParent(this);
	this->DoInsertItem(kNothingCmd, atIndex, 0);
		
	mSubMenus[atIndex] = subMenu;
	
	IMenuBarPtr menubar = this->GetMenuBar();
	if (menubar)
		subMenu->AddToMenuBar(menubar);
		
	if (rebuildTables) {
		IMenuBarPtr menubar = this->GetMenuBar();

		IActiveMenuBarPtr activeMenuBar(L"Application");
		if (activeMenuBar->Get() == menubar && menubar)
			menubar->RebuildAcceleratorTable();
	}
		
#if DEBUG
	this->DoValidateMnemonics();
#endif

	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// XMenu::AddToMenuBar
//
//---------------------------------------------------------------
void XMenu::AddToMenuBar(IMenuBarPtr menubar)
{
	PRECONDITION(!mMenuBar);
	PRECONDITION(menubar);
	
#if DEBUG
	std::vector<XMenuCommand>::iterator iter = mMenuCommands.begin();
	while (iter != mMenuCommands.end()) {
		XMenuCommand cmd = *iter++;

		if (cmd != kNothingCmd)
			menubar->AddCommand(cmd);
	}

	std::vector<IMenuPtr>::iterator iter2 = mSubMenus.begin();
	while (iter2 != mSubMenus.end()) {
		IMenuPtr subMenu = *iter2++;

		if (subMenu)
			subMenu->AddToMenuBar(menubar);
	}
#endif

	if (!mParent)
		mMenuBar = menubar;
}


//---------------------------------------------------------------
//
// XMenu::RemoveFromMenuBar
//
//---------------------------------------------------------------
void XMenu::RemoveFromMenuBar()
{
	PRECONDITION(mMenuBar || mParent);
	
#if DEBUG
	IMenuBarPtr menubar = this->GetMenuBar();
	ASSERT(menubar);

	std::vector<XMenuCommand>::iterator iter = mMenuCommands.begin();
	while (iter != mMenuCommands.end()) {
		XMenuCommand cmd = *iter++;

		if (cmd != kNothingCmd)
			menubar->RemoveCommand(cmd);
	}

	std::vector<IMenuPtr>::iterator iter2 = mSubMenus.begin();
	while (iter2 != mSubMenus.end()) {
		IMenuPtr subMenu = *iter2++;

		if (subMenu)
			subMenu->RemoveFromMenuBar();
	}
#endif
	
	mMenuBar.Release();
}


//---------------------------------------------------------------
//
// XMenu::GetMnemonic									
//
//---------------------------------------------------------------
#if DEBUG
wchar_t XMenu::GetMnemonic(const std::wstring& name) const
{
	wchar_t mnemonic = 0;
	
	uint32 i = 0;
	while (i < name.length() && mnemonic == 0) {
		if (i + 1 < name.length()) {
			if (name[i] == '&') {
				if (name[i + 1] == '&')
					++i;					// skip past both ampersands
				else
					mnemonic = name[i + 1];
			}
		}
		++i;
	}
	
	return ConvertToLowerCase(mnemonic);	
}
#endif


//---------------------------------------------------------------
//
// XMenu::AddMnemonics
//
//---------------------------------------------------------------
#if DEBUG
void XMenu::AddMnemonics(std::vector<wchar_t>& mnemonics) const
{
	uint32 count = this->GetCount();
	for (uint32 index = 0; index < count; ++index) {
		wchar_t mnemonic = 0;

		if (this->IsSubMenu(index)) {
			const IMenuPtr subMenu = this->GetSubMenu(index);

			subMenu->AddMnemonics(mnemonics);

			mnemonic = this->GetMnemonic(subMenu->GetName(false));

		} else if (!this->IsSeparator(index)) 
			mnemonic = this->GetMnemonic(this->GetName(index, false));

		if (mnemonic != 0) {
			std::vector<wchar_t>::iterator iter = std::find(mnemonics.begin(), mnemonics.end(), mnemonic);
			if (iter == mnemonics.end())
				mnemonics.push_back(mnemonic);
			else
				DEBUGSTR("The menu '", this->GetName(), "' has a duplicate '", mnemonic, "' mnemonic");
		}
	}
}
#endif


//---------------------------------------------------------------
//
// XMenu::AppendAccelerators
//
//---------------------------------------------------------------
void XMenu::AppendAccelerators(std::vector<ACCEL>& table)
{
	uint32 count = mAccelerators.size();
	for (uint32 index = 0; index < count; ++index) {
		const SAccelerator& accelerator = mAccelerators[index];
		
		if (accelerator.ch != 0 || accelerator.key != 0) {			
			ASSERT(!this->IsSeparator(index));
			ASSERT(!this->IsSubMenu(index));

			uint32 id = ::GetMenuItemID(mMenuH, (int32) index);
			ThrowIf(id == 0xFFFFFFFF);

			// Microsoft's documentation says that ACCEL can be
			// defined using either a virtual key code or an ASCII
			// character code. However ASCII codes don't work in
			// conjunction with the control key (although they do
			// work when the alt key is used). In order to eliminate
			// these sorts of problems we'll convert ASCII characters
			// into virtual key codes.
			int16 key = accelerator.key;
			if (key == 0) {
				HKL keyboard = ::GetKeyboardLayout(0);
				int16 scanCode = ::VkKeyScanEx(accelerator.ch, keyboard);
				key = numeric_cast<int16>(scanCode & 0xFF);			// strip off shift state
			}

			ACCEL accel;
			accel.fVirt = FVIRTKEY;	
			accel.key   = numeric_cast<uint16>(key);
			accel.cmd   = numeric_cast<uint16>(id);
			
			if (accelerator.modifiers & kShiftModifer)
				accel.fVirt += FSHIFT;
				
			if (accelerator.modifiers & kControlModifier)
				accel.fVirt += FCONTROL;
				
			if (accelerator.modifiers & kAltModifier)
				accel.fVirt += FALT;
							
			table.push_back(accel);
		}
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XMenu::Invariant
//
//---------------------------------------------------------------
void XMenu::Invariant() const
{
	Inherited::Invariant();
	
	ASSERT(mMenuH != nil);
	
	int32 count = ::GetMenuItemCount(mMenuH);
	ASSERT(mSubMenus.size() == count);
	ASSERT(mMenuCommands.size() == count);
	ASSERT(mAccelerators.size() == count);
	
	if (mMenuBar)
		ASSERT(!mParent);		// submenus can't be installed into the menubar!

	HMENU parent = this->DoGetParent();
	if (parent != nil) {
		uint32 index = this->DoGetIndex(parent);
		
		if (WSystemInfo::HasUnicode()) {
			wchar_t buffer[1024];
			count = ::GetMenuStringW(parent, index, buffer, sizeof(buffer), MF_BYPOSITION);
			ASSERT(mName == std::wstring(buffer, numeric_cast<uint32>(count)));
		
		} else {
			char buffer[1024];
			count = ::GetMenuStringA(parent, index, buffer, sizeof(buffer), MF_BYPOSITION);
			ASSERT(mName == FromPlatformStr(buffer, numeric_cast<uint32>(count)));
		}
	}

#if DEBUG
	if (gIntenseDebugging)
		this->DoValidateMnemonics();
#endif
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XMenu::DoGetParent
//
//---------------------------------------------------------------
HMENU XMenu::DoGetParent() const
{
	HMENU parent = nil;
	
	if (mParent)
		parent = mParent->GetOSMenu();
		
	else if (mMenuBar)
		parent = mMenuBar->GetOSMenu();
	
	return parent;
}


//---------------------------------------------------------------
//
// XMenu::DoGetIndex
//
//---------------------------------------------------------------
uint32 XMenu::DoGetIndex(HMENU parent) const
{
	PRECONDITION(parent != nil);
	
	uint32 index = ULONG_MAX;
	
	int32 count = ::GetMenuItemCount(parent);
	for (uint32 candidate = 0; candidate < count && index == ULONG_MAX; ++candidate) {
		HMENU subMenu = ::GetSubMenu(parent, (int32) candidate);
		if (subMenu == mMenuH)
			index = candidate;
	}
	
	POSTCONDITION(index < count);
	
	return index;
}


//---------------------------------------------------------------
//
// XMenu::DoInsertItem
//
//---------------------------------------------------------------
void XMenu::DoInsertItem(const XMenuCommand& cmd, uint32 at, const SAccelerator& accelerator)
{
	PRECONDITION(at <= mMenuCommands.size());	

	mMenuCommands.insert(mMenuCommands.begin() + (int32) at, cmd);	
	mSubMenus.insert(mSubMenus.begin() + (int32) at, IMenuPtr());	
	mAccelerators.insert(mAccelerators.begin() + (int32) at, accelerator);	

#if DEBUG
	IMenuBarPtr menubar = this->GetMenuBar();
	if (menubar && cmd != kNothingCmd)
		menubar->AddCommand(cmd);
#endif
}


//---------------------------------------------------------------
//
// XMenu::DoRemoveItem
//
//---------------------------------------------------------------
void XMenu::DoRemoveItem(uint32 index, bool deleteMenu, bool rebuildTables)
{
	PRECONDITION(index < mMenuCommands.size());	

	// Remove the menu command.
	XMenuCommand cmd = mMenuCommands[index];		
	mMenuCommands.erase(mMenuCommands.begin() + (int32) index);
	
	IMenuBarPtr menubar = this->GetMenuBar();
#if DEBUG
	if (menubar && cmd != kNothingCmd)
		menubar->RemoveCommand(cmd);
#endif

	// Remove the submenu.
	IMenuPtr subMenu = mSubMenus[index];		
	if (subMenu) {
		IMenu* nilMenu = nil;
		subMenu->SetParent(nilMenu);
		
		if (deleteMenu) 
			mSubMenus[index] = IMenuPtr();
	}
	
	mSubMenus.erase(mSubMenus.begin() + (int32) index);
	mAccelerators.erase(mAccelerators.begin() + (int32) index);
	
	// Remove the id from the command and help maps.
	if (subMenu) {
		uint32 id = ::GetMenuItemID(mMenuH, (int32) index);	
		if (id != 0xFFFFFFFF)
			menubar->RemoveMenuID(numeric_cast<int16>(id));
	}

	// Rebuild the accelerator table
	if (rebuildTables) {
		IMenuBarPtr menubar = this->GetMenuBar();

		IActiveMenuBarPtr activeMenuBar(L"Application");
		if (activeMenuBar->Get() == menubar && menubar)
			menubar->RebuildAcceleratorTable();
	}
}


//---------------------------------------------------------------
//
// XMenu::DoValidateMnemonics
//
//---------------------------------------------------------------
#if DEBUG
void XMenu::DoValidateMnemonics() const
{
	uint32 count = this->GetCount();
	
	std::vector<wchar_t> mnemonics;		// there won't be too many items so we can get by with a vector
	mnemonics.reserve(count);

	this->AddMnemonics(mnemonics);
}
#endif


}	// namespace Whisper
