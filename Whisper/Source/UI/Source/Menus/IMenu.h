/*
 *  File:       IMenu.h
 *  Summary:   	Cross platform menu class.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IMenu.h,v $
 *		Revision 1.5  2001/04/13 07:47:23  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.4  2000/12/15 09:07:49  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/14 08:23:24  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:03:24  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <12>	12/24/99	JDJ		Added kOptionAltModifier.
 *		 <11>	 7/19/99	JDJ		Tweaked for Carbon.
 *		 <10>	 6/21/99	JDJ		Added __MSL_FIX_ITERATORS__ (for Pro 3).
 *		 <9>	 6/09/99	JDJ		Corrected spelling of "separator".
 *		 <8>	 5/15/99	JDJ		Added AppendFonts. 
 *		 <7>	 5/15/99	JDJ		Added SetFont. (Mac)
 *		 <6>	 4/06/99	JDJ		Added DoValidateMnemonics. 
 *		 <5>	 3/31/99	JDJ		InsertSortedItem returns the index of the new item.
 *		 <4>	 3/30/99	JDJ		Help messages can now be specified when items are added. 
 *		 <3>	 3/21/99	JDJ		Removed code to create an XMenu from a Mac resource. 
 *									Added code to create an XMenu from an XML resource.
 *		 <1>	10/07/98	JDJ		Created
 */

#pragma once

#include <XConstants.h>
#include <XInterfacePtr.h>
#include <XStringUtils.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class IMenuBar;
typedef XInterfacePtr<IMenuBar> IMenuBarPtr;


//-----------------------------------
//	Constants
//
const uint32 kNoCheckmark		   = 1;			//!< menu item doesn't include a checkmark
const uint32 kEntireSelection	   = 0;			//!< checkmark applies to the entire selection (eg all the text is bold)
const uint32 kPartialSelection	   = 2;			//!< checkmark applies to part of the selection (only some of the selected text is bold)
const uint32 kIndependantCheckMark = 0;			//!< checkmark is independant of other items in its group (eg bold, italic, and underline)
const uint32 kExclusiveCheckMark   = 4;			//!< checkmark excludes other items in its group (eg align left and align right)

const uint32 kNoModifier 		   = 0;			//!< accelerator requires no modifiers (not even a command key on the Mac)
const uint32 kShiftModifer		   = 1;			//!< accelerator requires that the shift key be held down
const uint32 kControlModifier	   = 2;			//!< accelerator requires that the control key be held down
#if MAC
	const uint32 kOptionModifier   = 4;			//!< accelerator requires that the option key be held down
	const uint32 kCommandModifier  = 8;			//!< accelerator requires that the control key be held down
	const uint32 kDefaultModifier  = kCommandModifier;			
#elif WIN
	const uint32 kAltModifier	   = 4;			//!< accelerator requires that the Alt key be held down
	const uint32 kDefaultModifier  = kControlModifier;			
#endif
const uint32 kOptionAltModifier    = 4;			//!< for SetCommandHandler


// ===================================================================================
//	struct SAccelerator
//!		Used to specify the accelerator key to use with menu items
// ===================================================================================
struct UI_EXPORT SAccelerator {
	char 			ch;				//!< ASCII character code
	uint32			modifiers;		//!< can be kNoModifier or sum of kShiftModifer, kControlModifier, kOptionModifier, kCommandModifier, and kAltModifier.
#if WIN
	int16			key;			//!< virtual key code (can be used instead of character code)
	std::wstring	text;			//!< menu item text (neccesary when virtual key is used, optional with character code)
#endif

#if MAC
				SAccelerator(char c = 0, uint32 m = kDefaultModifier) 	{ch = c; modifiers = m;}

		bool	operator==(const SAccelerator& rhs) const				{return ch == rhs.ch && modifiers == rhs.modifiers;}
		bool	operator!=(const SAccelerator& rhs) const				{return !this->operator==(rhs);}

#elif WIN
				SAccelerator(char c = 0, uint32 m = kDefaultModifier) 	{ch = c; key = 0; modifiers = m; text = ConvertToUpperCase(c);}
				SAccelerator(int16 k, const std::wstring& t, uint32 m)	{ch = 0; key = k; modifiers = m; text = t;}

		bool	operator==(const SAccelerator& rhs) const				{return ch == rhs.ch && modifiers == rhs.modifiers && key == rhs.key && text == rhs.text;}
		bool	operator!=(const SAccelerator& rhs) const				{return !this->operator==(rhs);}
#endif
};


// ===================================================================================
//	class IMenu
//!		Cross platform menu class.
/*!		Whisper's menus are a bit unusual in that menu items are tagged with a string 
 *		instead of an integral command id. This is annoying because the standard menu 
 *		resource tools cannot be used, but nice because it essentially eliminates command 
 *		id collisions, is easy to interpret while debugging, and allows you to encode 
 *		arbitrary amounts of information into the string (eg a zoom menu might have 
 *		commands like "Zoom By: 2.0", "Zoom By: 4.0", etc. When processing the command 
 *		you simply remove the common prefix and convert the string to a double. This 
 *		is easier and more flexible than what you could do with an integral command id). */
// ===================================================================================
class IMenu : public XUnknown {

	typedef XUnknown Inherited;

//-----------------------------------
//	Types
//
public:
	struct SHelp {
		std::wstring	enabled;
		std::wstring	disabled;
		std::wstring	checked;
	};

	typedef XInterfacePtr<IMenu> IThisPtr;		// can't use IMenuPtr inside IMenu's declaration...

//-----------------------------------
//	Initialization
//
public:
	virtual void 		Init(const std::wstring& name, const SHelp& titleHelp = SHelp(), const SHelp& missingHelp = SHelp(), uint32 id = 0) = 0;
						/**< Menu id's are optional. TitleHelp is used to display help about 
						the entire menu. MissingHelp is used for items that do not define 
						help messages. */
								
//-----------------------------------
//	Public API
//
public:
	//! @name Menu
	//@{
	virtual std::wstring GetName(bool stripAmpersands = true) const = 0;
	
	virtual void 		SetName(const std::wstring& inName, bool redraw = kRedraw) = 0;

	virtual uint32 		GetCount() const = 0;
			
	virtual void 		Enable(bool enable = true, bool redraw = kRedraw) = 0;

	virtual void 		RemoveItems(bool rebuildTables = true) = 0;
						/**< If rebuildTables is true IMenu will rebuild some cached information.
						On Windows the accelerator table will be rebuilt if the menu is
						installed into the active menubar. On the Mac the balloon help
						resource will be rebuilt. */

	virtual bool 		IsSubMenu() const = 0;

	virtual bool		IsInstalled() const = 0;
						/**< Returns true if the menu is directly or indirectly installed 
						in the menu bar. */
						
	virtual IMenuBarPtr GetMenuBar() const = 0;
						/**< Will return nil if the menu isn't installed. */
						
	virtual uint32 		GetID() const = 0;
						/**< On the Mac this may not be the same as the MenuHandle's id. */
		
	virtual OSMenu 		GetOSMenu() const = 0;
	//@}
												
	//! @name Generic Menu Items
	//@{
	virtual std::wstring GetName(uint32 index, bool stripAmpersands = true) const = 0;
						/**< Note that, by default, the returned string has single 
						ampsersands removed and double ampersands converted to 
						single ampsersands. */

	virtual void 		SetName(uint32 index, const std::wstring& name) = 0;
						/**< Note that ampersands are special: a single ampersand is used 
						to signal a mnemonic on Windows (it will be removed on a Mac). 
						If you want an ampersand to appear in the menu item text use 
						two ampersands (&&). */
			
	virtual void 		RemoveItem(uint32 index, bool rebuildTables = true) = 0;
	//@}

	//! @name Separators
	//@{
	virtual bool 		IsSeparator(uint32 index) const = 0;

	virtual void 		AppendSeparator(bool rebuildTables = true) = 0;
				
	virtual void 		InsertSeparator(uint32 atIndex, bool rebuildTables = true) = 0;
						/**< New item will appear at atIndex. */
	//@}
			
	//! @name Menu Commands
	//@{
	virtual XMenuCommand GetCommand(uint32 index) const = 0;
			
	virtual void 		EnableItem(uint32 index, bool enable = true) = 0;
			
	virtual void 		CheckItem(uint32 index, uint32 mark) = 0;
						/**< Mark can be kNoCheckmark or sum of kEntireSelection, kPartialSelection,
						kIndependantCheckMark, and kExclusiveCheckMark. */
			
	virtual void 		SetItem(uint32 index, const std::wstring& name, const XMenuCommand& cmd) = 0;
			
	virtual void 		AppendItem(const std::wstring& name, const XMenuCommand& cmd, const SHelp& help = SHelp(), const SAccelerator& accelerator = 0, bool rebuildTables = true) = 0;
						/**< Items must be added and deleted using these methods to keep the
						command table consistent. Modifiers can be kNoModifier or sum of
						kShiftModifer, kControlModifier, kOptionModifier, kCommandModifier, 
						and kAltModifier.  */
				
	virtual void 		InsertItem(const std::wstring& name, uint32 atIndex, const XMenuCommand& cmd, const SHelp& help = SHelp(), const SAccelerator& accelerator = 0, bool rebuildTables = true) = 0;
						/**< New item will appear at atIndex. */
			
	virtual uint32 		InsertSortedItem(const std::wstring& name, uint32 startItem, uint32 count, const XMenuCommand& cmd, const SHelp& help = SHelp(), bool rebuildTables = true) = 0;
						/**< Inserts an item into into the sorted range [startItem, startItem + count]. 
						Returns the index of the new item. */

	virtual void 		AppendFonts(OSWindow window, const XMenuCommand& cmdPrefix, bool rebuildTables = true) = 0;
						/**< Appends a list of fonts to the menu. The commands will be cmdPrefix + fontName. */

#if MAC
	virtual void		SetStyle(uint32 index, Style style) = 0;
						/**< style can be normal, bold, italic, underline, outline, shadow, condense, 
						or extend */

	virtual void		SetFont(uint32 index, int16 fontNum) = 0;

	virtual XMenuCommand AddResourceItems(ResType type, const XMenuCommand& startCmd, bool rebuildTables = true) = 0;
						/**< Appends the names of all resources of the specified type
						to the menu. The new commands start at startCmd. The last 
						command string is returned. */
#endif
	//@}
	
	//! @name Sub-Menus
	//@{
	virtual bool 		IsSubMenu(uint32 index) const = 0;
	
	virtual IThisPtr 	GetSubMenu(uint32 index) const = 0;

	virtual void 		AppendSubMenu(const IThisPtr& subMenu, bool rebuildTables = true) = 0;
				
	virtual void 		InsertSubMenu(uint32 atIndex, const IThisPtr& subMenu, bool rebuildTables = true) = 0;
	//@}

//-----------------------------------
//	Internal API
//
public:					
	virtual IThisPtr 	GetParent() = 0;
	virtual void 		SetParent(IThisPtr parent) = 0;

	virtual	void 		AddToMenuBar(IMenuBarPtr menubar) = 0;
			
	virtual void 		RemoveFromMenuBar() = 0;

	virtual SHelp		GetHelp() const = 0;

#if MAC
	virtual	void 		AddedToMenuBar(IMenuBarPtr menubar) = 0;

	virtual int16 		GetOSMenuID() const = 0;
	virtual bool 		HasHelp() const = 0;
	virtual IThisPtr	FindSubMenu(int16 menuID) const = 0;

#elif WIN
#if DEBUG
	virtual void		AddMnemonics(std::vector<wchar_t>& mnemonics) const = 0;
	virtual	wchar_t		GetMnemonic(const std::wstring& name) const = 0;
#endif
	virtual void 		AppendAccelerators(std::vector<ACCEL>& table) = 0;
#endif
};

typedef XInterfacePtr<IMenu> IMenuPtr;
typedef XInterfacePtr<const IMenu> IConstMenuPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
