/*
 *  File:       MMenu.cpp
 *  Summary:   	Cross platform menu class.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MMenu.cpp,v $
 *		Revision 1.12  2001/04/27 04:21:58  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.11  2001/04/21 03:27:00  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.10  2001/04/13 07:51:11  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.9  2001/04/04 04:55:34  jesjones
 *		Dtor calls DeleteMenu if we're a context menu (fixes heap corruption problems).
 *		
 *		Revision 1.8  2001/03/03 23:57:35  jesjones
 *		Added calls to DEBUGSTR in catch (...) blocks.
 *		
 *		Revision 1.7  2000/12/31 04:32:10  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.6  2000/12/24 03:20:20  jesjones
 *		Can now call Init with a zero length name (useful for context menus).
 *		
 *		Revision 1.5  2000/12/14 08:28:05  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.4  2000/12/10 04:04:25  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.3  2000/11/27 07:13:03  jesjones
 *		Can't construct an interface pointer with nil anymore...
 *		
 *		Revision 1.2  2000/11/09 09:15:57  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <12>	 8/12/00	JDJ		Fixed InsertSortedItem so that more then one item can be added
 *									at the end of the menu.
 *		 <11>	12/12/99	JDJ		DoRebuildHelpMenu ensures that the menu help file name is valid.
 *		 <10>	 7/19/99	JDJ		Tweaked for Carbon.
 *		 <9>	 6/21/99	JDJ		Added __MSL_FIX_ITERATORS__ (for Pro 3).
 *		 <8>	 6/13/99	JDJ		Worked around an Apple bug in DoRebuildHelpMenu.
 *		 <7>	 6/09/99	JDJ		Corrected spelling of "separator".
 *		 <6>	 5/15/99	JDJ		Added AppendFonts. 
 *		 <5>	 5/15/99	JDJ		Added SetFont. 
 *		 <4>	 3/30/99	JDJ		Help messages can now be specified when items are added. 
 *		 <3>	 3/21/99	JDJ		Removed code to create an XMenu from a Mac resource.
 *		 <2>	 3/19/99	JDJ		Dtor uses XMenu::GetMenuBar instead of XMenuBar::Instance.
 *									XMenu (XString) ctor strips ampersands.
 *		 <1>	10/07/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IMenu.h>

#include <Appearance.h>
#include <Balloons.h>

#include <set>

#include <IContextMenu.h>
#include <IMenuBar.h>
#include <SLFunctions.h>
#include <MResUtils.h>
#include <MSystemInfo.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XFileSystem.h>
#include <XFont.h>
#include <XHandle.h>
#include <XIntConversions.h>
#include <XInterfaceMacros.h>
#include <XLocker.h>
#include <XMemUtils.h>
#include <XNumbers.h>
#include <XStringUtils.h>
#include <XSystemInfo.h>

#if GARBAGE_COLLECT_COM
	#include <XGarbageCollector.h>
#endif

namespace Whisper {


// ===================================================================================
//	struct SHelpResource
// ===================================================================================
#pragma options align=mac68k
struct SHelpResource {
	int16	version;			// should be 0x0002
	int32	options;			// 0 for default
	int16	defProc;			// 0 for system proc
	int16	variation;			// usually 0
	int16	numItems;			// includes missing items and menu title
	
	// missing items			// variable length fields
	// menu title
	// items
};
#pragma options align=reset


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
						
	virtual void		Init(const std::wstring& name, const SHelp& titleHelp, const SHelp& missingHelp, uint32 id);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual	std::wstring GetName(bool stripAmpersands = true) const		{UNUSED(stripAmpersands); return mName;}
	virtual void 		SetName(const std::wstring& inName, bool redraw = kRedraw);
	
	virtual	uint32 		GetCount() const						{return mMenuCommands.size();}
			
	virtual	void 		Enable(bool enable = true, bool redraw = kRedraw);

	virtual	void 		RemoveItems(bool rebuildTables = true);

	virtual	bool 		IsSubMenu() const						{return mParent;}

	virtual	bool		IsInstalled() const;

	virtual	IMenuBarPtr GetMenuBar() const;
						
	virtual	uint32 		GetID() const							{return mMenuID;}
		
	virtual	OSMenu 		GetOSMenu() const						{return mMenuH;}
												
	virtual	std::wstring GetName(uint32 index, bool stripAmpersands = true) const;

	virtual	void 		SetName(uint32 index, const std::wstring& name);
			
	virtual	void 		RemoveItem(uint32 index, bool rebuildTables = true);

	virtual	bool 		IsSeparator(uint32 index) const;

	virtual	void 		AppendSeparator(bool rebuildTables = true);
				
	virtual	void 		InsertSeparator(uint32 atIndex, bool rebuildTables = true);

	virtual	XMenuCommand GetCommand(uint32 index) const;
			
	virtual	void 		EnableItem(uint32 index, bool enable);
			
	virtual	void 		CheckItem(uint32 index, uint32 mark);
			
	virtual	void 		SetItem(uint32 index, const std::wstring& name, const XMenuCommand& cmd);
			
	virtual	void 		AppendItem(const std::wstring& name, const XMenuCommand& cmd, const SHelp& help = SHelp(), const SAccelerator& accelerator = 0, bool rebuildTables = true);
				
	virtual	void 		InsertItem(const std::wstring& name, uint32 atIndex, const XMenuCommand& cmd, const SHelp& help = SHelp(), const SAccelerator& accelerator = 0, bool rebuildTables = true);
			
	virtual	uint32 		InsertSortedItem(const std::wstring& name, uint32 startItem, uint32 count, const XMenuCommand& cmd, const SHelp& help = SHelp(), bool rebuildTables = true);

	virtual	void 		AppendFonts(OSWindow window, const XMenuCommand& cmdPrefix, bool rebuildTables = true);

	virtual	void		SetStyle(uint32 index, Style style);

	virtual	void		SetFont(uint32 index, int16 fontNum);

	virtual	XMenuCommand AddResourceItems(ResType type, const XMenuCommand& startCmd, bool rebuildTables = true);
	
	virtual	bool 		IsSubMenu(uint32 index) const;
	
	virtual	IMenuPtr 	GetSubMenu(uint32 index) const;

	virtual	void 		AppendSubMenu(const IMenuPtr& subMenu, bool rebuildTables = true);
				
	virtual	void 		InsertSubMenu(uint32 atIndex, const IMenuPtr& subMenu, bool rebuildTables = true);

	virtual IMenuPtr 	GetParent()						{return mParent;}
	virtual void 		SetParent(IMenuPtr parent) 		{mParent = parent;}

	virtual int16 		GetOSMenuID() const 			{return mOSMenuID;}
	virtual bool 		HasHelp() const 				{return mHasHelp;}
	virtual SHelp		GetHelp() const					{return mTitleHelp;}
	virtual IMenuPtr	FindSubMenu(int16 menuID) const;

	virtual	void 		AddToMenuBar(IMenuBarPtr menubar);
	virtual	void 		AddedToMenuBar(IMenuBarPtr menubar);
			
	virtual void 		RemoveFromMenuBar();

protected:
	virtual	void 		Invariant() const;

//-----------------------------------
//	Internal API
//
private:			
			void 		DoRemoveItem(uint32 index, bool rebuildTables);					
			void 		DoInsertItem(const XMenuCommand& cmd, uint32 at, const SAccelerator& accelerator, const SHelp& help);
			
			void 		DoRebuildHelpMenu();

//-----------------------------------
//	Member Data
//
private:
	OSMenu	 					mMenuH;
	IMenuPtr					mParent;		// non-nil for sub-menus
	IMenuBarPtr					mMenuBar;		// nil for uninstalled menus and submenus
	uint32						mMenuID;
	
	std::wstring				mName;
	
	SHelp						mTitleHelp;
	SHelp						mMissingHelp;

	std::vector<IMenuPtr>		mSubMenus;
	std::vector<XMenuCommand>	mMenuCommands;	
	
	int16 						mOSMenuID;
	std::vector<SHelp>			mItemHelp;	
	bool						mHasHelp;
	bool						mContextMenu;

	static int16				msNextMenuID;
};

int16 XMenu::msNextMenuID = 1;				// need small values in case the menu becomes a sub-menu

DEFINE_INTERFACE_FACTORY(XMenu)

//---------------------------------------------------------------
//
// XMenu::~XMenu
//
//---------------------------------------------------------------
XMenu::~XMenu()
{
	if (mMenuH != nil) {
		if (mContextMenu)
			::DeleteMenu(mOSMenuID);
	
		this->RemoveItems(false);				// delete all the submenus
		
		if (mParent) {
			uint32 index = ULONG_MAX;
			uint32 count = mParent->GetCount();
			
			for (uint32 candidate = 0; candidate < count && index == ULONG_MAX; ++candidate) {
				if (mParent->IsSubMenu(candidate) && mParent->GetSubMenu(candidate) == this)
					index = candidate;
			}
			
			mParent->RemoveItem(index);
		}
		
		DisposeMenu(mMenuH);
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
		
	mHasHelp = false;
	mContextMenu = false;
}


//---------------------------------------------------------------
//
// XMenu::Init
//
//---------------------------------------------------------------
void XMenu::Init(const std::wstring& name, const SHelp& titleHelp, const SHelp& missingHelp, uint32 myID)
{
	PRECONDITION(mMenuH == nil);

	mTitleHelp = titleHelp;
	mMissingHelp = missingHelp;

	mName = Whisper::Replace(name, '&', '\0', '&');
		
	mMenuID = myID;

	mOSMenuID = msNextMenuID++;

	OSStatus err = ::CreateNewMenu(mOSMenuID, 0, &mMenuH);
	ThrowIfOSErr(err);
	
	err = ::SetMenuTitle(mMenuH, ToPascalStr(mName).c_str());
	if (err != noErr) { 
		DisposeMenu(mMenuH);
		ThrowIfOSErr(err);
	}

	IContextMenuPtr context(this);
	if (context)
		mContextMenu = true;	
	
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XMenu::Enable
//
//---------------------------------------------------------------
void XMenu::Enable(bool enable, bool redraw)
{
	CHECK_INVARIANT;
		
	bool wasEnabled = ::MenuHasEnabledItems(mMenuH);	// $$$ this isn't a very good check
	if (enable) {										// $$$ in Carbon could simply axe this function...
		::EnableMenuItem(mMenuH, 0);
		if (redraw && mMenuBar && !mParent)
			::InvalMenuBar();
	
	} else {
		::DisableMenuItem(mMenuH, 0);
//		if (redraw && wasEnabled && mMenuBar != nil && mParent == nil)
		if (redraw && mMenuBar && !mParent)
			::InvalMenuBar();
	}
}


//---------------------------------------------------------------
//
// XMenu::SetName (wstring)
//
//---------------------------------------------------------------
void XMenu::SetName(const std::wstring& inName, bool redraw)	
{
	PRECONDITION(this->GetCount() == 0);
	PRECONDITION(!this->IsInstalled());
	PRECONDITION(!mParent);
	CHECK_INVARIANT;
	
	std::wstring name = Whisper::Replace(inName, '&', '\0', '&');
	
	if (name != mName) {
		MCoreString str(name);
		
		OSStatus err = SetMenuTitleWithCFString(mMenuH, str);
		ThrowIfOSErr(err);
		
		mName = name;

		if (redraw && mMenuBar && !mParent)
			::InvalMenuBar();
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
	
	if (rebuildTables)
		this->DoRebuildHelpMenu();
	
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
// XMenu::GetName (uint32)
//
//---------------------------------------------------------------
std::wstring XMenu::GetName(uint32 index, bool stripAmpersands) const
{
	PRECONDITION(index < this->GetCount());
	PRECONDITION(!this->IsSeparator(index));
	UNUSED(stripAmpersands);
	
	CFStringRef str = nil;
	OSStatus err = CopyMenuItemTextAsCFString(mMenuH, numeric_cast<uint16>(index+1), &str);
	ThrowIfOSErr(err);
	
	std::wstring text = FromCarbonStr(str);
	CFRelease(str);
	
	return text;
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
	
	std::wstring name = Whisper::Replace(inName, '&', '\0', '&');
	
	MCoreString str(name);
	OSStatus err = SetMenuItemTextWithCFString(mMenuH, numeric_cast<uint16>(index+1), str);
	ThrowIfOSErr(err);

	POSTCONDITION(this->GetName(index, false) == name);
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
		
		::DeleteMenu(subMenu->GetOSMenuID());				// note that this doesn't actually delete the menu, it just removes it from the list of active menus
	}

	::DeleteMenuItem(mMenuH, numeric_cast<int16>(index+1));
	this->DoRemoveItem(index, rebuildTables);

	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// XMenu::IsSeparator 
//
//---------------------------------------------------------------
bool XMenu::IsSeparator(uint32 index) const
{
	PRECONDITION(index < this->GetCount());
	
	Str255 name;
	::GetMenuItemText(mMenuH, numeric_cast<int16>(index+1), name);
		
	bool isSep = name[0] == 2 && name[1] == '(' && name[2] == '-';
		
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
	CHECK_INVARIANT;
		
	this->DoInsertItem(kNothingCmd, atIndex, 0, SHelp());
	::InsertMenuItem(mMenuH, "\p(-", numeric_cast<int16>(atIndex));
		
	if (rebuildTables)
		this->DoRebuildHelpMenu();
	
	POSTCONDITION(true);
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
	
	if (enable)
		::EnableMenuItem(mMenuH, numeric_cast<uint16>(index+1));
	else
		::DisableMenuItem(mMenuH, numeric_cast<uint16>(index+1));

	POSTCONDITION(true);
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
	
	char mark;
	if (style & kNoCheckmark)
		mark = noMark;
	else if (style & kPartialSelection)
		mark = '-';
	else 
		mark = kCheckMarkChar;
	
	::SetItemMark(mMenuH, numeric_cast<int16>(index+1), mark);
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
void XMenu::InsertItem(const std::wstring& name, uint32 atIndex, const XMenuCommand& cmd, const SHelp& help, const SAccelerator& accelerator, bool rebuildTables)
{
	PRECONDITION(atIndex <= this->GetCount());
	PRECONDITION(name != L"(-");						// use InsertSeparator instead
	PRECONDITION(accelerator.ch != hMenuCmd);
	CHECK_INVARIANT;
	
	if (help.enabled.length() > 0)
		this->DoInsertItem(cmd, atIndex, accelerator, help);
	else
		this->DoInsertItem(cmd, atIndex, accelerator, mMissingHelp);

	::InsertMenuItem(mMenuH, "\pdummy", numeric_cast<int16>(atIndex));	// use dummy string to avoid meta-characters
	this->SetName(atIndex, name);
	
	if (accelerator.ch != 0) {
		if (MSystemInfo::HasAppearanceMgr()) {	
			uint8 modifier = kMenuNoCommandModifier;
			if (accelerator.modifiers & kShiftModifer)
				modifier += kMenuShiftModifier;
				
			if (accelerator.modifiers & kControlModifier)
				modifier += kMenuControlModifier;
				
			if (accelerator.modifiers & kOptionModifier)
				modifier += kMenuOptionModifier;
				
			if (accelerator.modifiers & kCommandModifier)
				modifier -= kMenuNoCommandModifier;
			
			OSErr err = ::SetMenuItemModifiers(mMenuH, numeric_cast<int16>(atIndex+1), modifier);
			ThrowIfOSErr(err);

			::SetItemCmd(mMenuH, numeric_cast<int16>(atIndex+1), accelerator.ch);
			
		} else
			::SetItemCmd(mMenuH, numeric_cast<int16>(atIndex+1), accelerator.ch);
	}
	
	if (rebuildTables)
		this->DoRebuildHelpMenu();
	
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
// XMenu::SetStyle
//
//---------------------------------------------------------------
void XMenu::SetStyle(uint32 index, Style style)
{
	PRECONDITION(index < this->GetCount());
	PRECONDITION(!this->IsSeparator(index));
	CHECK_INVARIANT;
	
	::SetItemStyle(mMenuH, numeric_cast<int16>(index+1), style);
}


//---------------------------------------------------------------
//
// XMenu::SetFont
//
//---------------------------------------------------------------
void XMenu::SetFont(uint32 index, int16 fontNum)
{
	PRECONDITION(index < this->GetCount());
	PRECONDITION(!this->IsSeparator(index));
	CHECK_INVARIANT;
	
	if (MSystemInfo::HasAppearanceMgr()) {
		OSErr err = ::SetMenuItemFontID(mMenuH, numeric_cast<int16>(index+1), fontNum);
		ASSERT(err == noErr);		
	}
}


//---------------------------------------------------------------
//
// XMenu::AppendFonts
//
//---------------------------------------------------------------
void XMenu::AppendFonts(OSWindow window, const XMenuCommand& cmdPrefix, bool rebuildTables)
{	
	uint32 count = 0;
	
	GrafPtr port = GetWindowPort(window);
	
	XFont::iterator iter = XFont::begin(port);
	while (iter != XFont::end(port)) {
		std::wstring name = *iter;
		++iter;
		
		uint32 index = this->InsertSortedItem(name, 0, count++, cmdPrefix + name, SHelp(), false);

		if (MSystemInfo::HasAppearanceMgr()) {
			XFont font(name);
			this->SetFont(index, font.GetFontNumber());
		}
	}	

	if (rebuildTables)
		this->DoRebuildHelpMenu();
}


//---------------------------------------------------------------
//
// XMenu::AddResourceItems
//
//---------------------------------------------------------------
XMenuCommand XMenu::AddResourceItems(ResType type, const XMenuCommand& startCmd, bool rebuildTables)
{
	CHECK_INVARIANT;
	
	MenuHandle tempMenu = NewMenu(1, "\pdummy");
	ThrowIfNil(tempMenu);
	
	::AppendResMenu(tempMenu, type);
	
	int16 count = CountMenuItems(tempMenu);
	for (uint32 index = 2; index <= count; index++) {			// first item is a separator
		Str255 str;
		::GetMenuItemText(tempMenu, numeric_cast<int16>(index), str);
		
		XMenuCommand cmd = startCmd + L" " + UInt32ToStr(index);

		this->AppendItem(FromPascalStr(str), cmd, SHelp(), 0, false);
	}
		
	DisposeMenu(tempMenu);
		
	if (rebuildTables)
		this->DoRebuildHelpMenu();
		
	return startCmd + L" " + Int16ToStr(count);
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
	PRECONDITION(subMenu);
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
	CHECK_INVARIANT;
	
	MenuHandle subMenuH = subMenu->GetOSMenu();

	::InsertMenuItem(mMenuH, "\pdummy", numeric_cast<int16>(atIndex));		// use dummy string to avoid meta-characters

	::SetItemCmd(mMenuH, numeric_cast<int16>(atIndex+1), hMenuCmd);
	::SetItemMark(mMenuH, numeric_cast<int16>(atIndex+1), subMenu->GetOSMenuID());
	
	mMenuCommands.insert(mMenuCommands.begin() + (int32) atIndex, kNothingCmd);	
	mSubMenus.insert(mSubMenus.begin() + (int32) atIndex, subMenu);
	mItemHelp.insert(mItemHelp.begin() + (int32) atIndex, subMenu->GetHelp());
	
#if GARBAGE_COLLECT_COM
	XBoss* boss = this->GetBoss();
	XGarbageCollector::Instance()->AssignInterface(boss, mSubMenus[atIndex].GetPtr());
#endif
	
	subMenu->SetParent(this);

	IMenuBarPtr menubar = this->GetMenuBar();
	if (menubar) {
		::InsertMenu(subMenuH, hierMenu);
		subMenu->AddToMenuBar(menubar);
		subMenu->AddedToMenuBar(menubar);
	}
		
	this->SetName(atIndex, subMenu->GetName());

	if (rebuildTables)
		this->DoRebuildHelpMenu();
	
	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// XMenu::FindSubMenu 
//
//---------------------------------------------------------------
IMenuPtr XMenu::FindSubMenu(int16 menuID) const
{
	IMenuPtr subMenu;

	uint32 count = this->GetCount();
	for (uint32 index = 0; index < count && !subMenu; ++index) {
		if (this->IsSubMenu(index)) {
			IMenuPtr candidate = this->GetSubMenu(index);
			
			if (candidate->GetOSMenuID() == menuID)
				subMenu = candidate;
			else
				subMenu = candidate->FindSubMenu(menuID);
		}
	}
		
	return subMenu;
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

	std::vector<IMenuPtr>::iterator iter3 = mSubMenus.begin();
	while (iter3 != mSubMenus.end()) {
		IMenuPtr subMenu = *iter3++;

		if (subMenu)
			::InsertMenu(subMenu->GetOSMenu(), hierMenu);
	}
	
	if (!mParent)
		mMenuBar = menubar;
}


//---------------------------------------------------------------
//
// XMenu::AddedToMenuBar
//
//---------------------------------------------------------------
void XMenu::AddedToMenuBar(IMenuBarPtr menubar)
{
	UNUSED(menubar);
	
	if (mMenuCommands.size() == 0) {
		uint32 count = CountMenuItems(mMenuH);
		for (uint32 i = 0; i < count; ++i) {			// Apple menu is filled out automatically so we need to update a bit of state
			mMenuCommands.push_back(kNothingCmd);	
			mSubMenus.push_back(IMenuPtr());	
			mItemHelp.push_back(SHelp());	
		}
	}
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
	
	std::vector<IMenuPtr>::iterator iter3 = mSubMenus.begin();
	while (iter3 != mSubMenus.end()) {
		IMenuPtr subMenu = *iter3++;

		if (subMenu)
			::DeleteMenu(subMenu->GetOSMenuID());				// note that this doesn't actually delete the menu, it just removes it from the list of active menus
	}
		
	mMenuBar.Release();
}

#pragma mark ~

//---------------------------------------------------------------
//
// XMenu::Invariant
//
//---------------------------------------------------------------
void XMenu::Invariant() const
{
	Inherited::Invariant();
	
	ASSERT(mMenuH != nil);

	uint32 numItems = CountMenuItems(mMenuH);	
	ASSERT(mSubMenus.size() == numItems);
	ASSERT(mMenuCommands.size() == numItems);
	ASSERT(mItemHelp.size() == numItems);
	
	if (mMenuBar)
		ASSERT(!mParent);		// submenus can't be installed into the menubar!

#if DEBUG
	if (gIntenseDebugging) {
		CFStringRef str = nil;
		OSStatus err = CopyMenuTitleAsCFString(mMenuH, &str);
		
		ASSERT(err == noErr);
		ASSERT(mName == MCoreString(str).c_str());
		
		CFRelease(str);
	}	
#endif
}

#pragma mark ~

//---------------------------------------------------------------
//
// XMenu::DoInsertItem
//
//---------------------------------------------------------------
void XMenu::DoInsertItem(const XMenuCommand& cmd, uint32 at, const SAccelerator& accelerator, const SHelp& help)
{
	PRECONDITION(at <= mMenuCommands.size());
	UNUSED(accelerator);					// used on Windows	

	mMenuCommands.insert(mMenuCommands.begin() + (int32) at, cmd);	
	mSubMenus.insert(mSubMenus.begin() + (int32) at, IMenuPtr());	
	mItemHelp.insert(mItemHelp.begin() + (int32) at, help);	

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
void XMenu::DoRemoveItem(uint32 index, bool rebuildTables)
{
	PRECONDITION(index < mMenuCommands.size());	

	// Remove the menu command.
	XMenuCommand cmd = mMenuCommands[index];		
	mMenuCommands.erase(mMenuCommands.begin() + (int32) index);
	
#if DEBUG
	IMenuBarPtr menubar = this->GetMenuBar();
	if (menubar && cmd != kNothingCmd)
		menubar->RemoveCommand(cmd);
#endif

	// Remove the submenu.
	IMenuPtr subMenu = mSubMenus[index];		
	if (subMenu) {
		subMenu->SetParent(IMenuPtr());
		
#if GARBAGE_COLLECT_COM
		XGarbageCollector::Instance()->RemoveInterface(this->GetBoss(), mSubMenus[index].GetPtr());
#endif
	}
	
	mSubMenus.erase(mSubMenus.begin() + (int32) index);
	mItemHelp.erase(mItemHelp.begin() + (int32) index);
	
	// Rebuild the 'hmnu' resource
	if (rebuildTables)
		this->DoRebuildHelpMenu();
}


//---------------------------------------------------------------
//
// XMenu::DoRebuildHelpMenu
//
//---------------------------------------------------------------
void XMenu::DoRebuildHelpMenu() 
{
#if 0
	XHandle hmnu(sizeof(SHelpResource));
	
	SHelpResource* rsrc = reinterpret_cast<SHelpResource*>(hmnu.GetUnsafePtr());
	rsrc->version	= 0x0002;
	rsrc->options	= 0;
	rsrc->defProc	= 0;
	rsrc->variation	= 0;
	rsrc->numItems	= 2;
	
	AppendHelp(hmnu, mMissingHelp);
	AppendHelp(hmnu, mTitleHelp);

	mHasHelp = false;

	uint32 numItems = this->GetCount();
	for (uint32 index = 0; index < numItems; ++index) {
		const SHelp& help = mItemHelp[index];
		
		if (help.enabled.length() > 0 || help.disabled.length() > 0 || help.checked.length() > 0)
			mHasHelp = true;
		
		AppendHelp(hmnu, help);
	}
#endif
}


}	// namespace Whisper
