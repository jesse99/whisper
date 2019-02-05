/*
 *  File:       XXMLMenu.cpp
 *  Summary:   	Creates a menu or a menu bar from an XML resource.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLMenu.cpp,v $
 *		Revision 1.9  2001/04/27 04:39:29  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.8  2001/04/13 08:04:33  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.7  2001/04/06 04:46:45  jesjones
 *		Submenus get attached to their parent menu before they're filled out (Windows fix).
 *		
 *		Revision 1.6  2001/03/28 09:45:58  jesjones
 *		Menu items now can have platform and target attributes.
 *		
 *		Revision 1.5  2000/12/31 04:50:38  jesjones
 *		Tweaked LoadMenus so submenus work.
 *		
 *		Revision 1.4  2000/12/14 08:38:15  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/11/20 05:49:07  jesjones
 *		Added a cheesy overload work around.
 *		
 *		Revision 1.2  2000/11/10 04:47:28  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <7>	 8/04/99	JDJ		DoFindEncoding replaces two underscores with an underscore
 *									(instead of two ampersands).
 *		 <6>	 7/19/99	JDJ		Tweaked for Carbon.
 *		 <5>	 7/12/99	JDJ		GetWeight will use English if something better cannot
 *									be found.
 *		 <4>	 6/09/99	JDJ		Corrected spelling of "separator".
 *		 <3>	 4/06/99	JDJ		Added DoValidateMnemonics check to XML ctor.
 *		 <2>	 3/31/99	JDJ		InsertSortedItem returns the index of the new item.
 *		 <1>	 3/21/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IXMLMenu.h>

#include <IMenu.h>
#include <IMenuBar.h>
#include <XBoss.h>
#include <XExceptions.h>
#include <XIntConversions.h>
#include <XInterfaceMacros.h>
#include <XLoadXML.h>
#include <XLocale.h>
#include <XNumbers.h>
#include <XStringUtils.h>
#include <XXMLDoc.h>
#include <XXMLItems.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const uint32 kLanguageWeight = 4;
const uint32 kEnglishWeight  = 2;
const uint32 kRegionWeight   = 1;	


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetWeight
//
//---------------------------------------------------------------
static uint32 GetWeight(const XXMLElement* candidate, const XLocale& locale)
{
	PRECONDITION(candidate != nil);
	PRECONDITION(candidate->GetName() == L"Encoding");
		
	uint32 weight = 0;
	
	XXMLElement::const_attr_iterator iter = candidate->attr_begin();
	while (iter != candidate->attr_end()) {
		XXMLAttribute attribute = *iter;
		++iter;
		
		std::wstring value = attribute.GetValue();
		
		if (attribute.GetName() == L"language") {
			if (value == locale.GetLanguage())
				weight += kLanguageWeight;
						
			if (value == L"English")
				weight += kEnglishWeight;
						
		} else if (attribute.GetName() == L"region") {
			if (value == locale.GetRegion())
				weight += kRegionWeight;
						
		} else
			DEBUGSTR("Bogus Encoding attribute");		// should have been caught by the parser
	}
	
	return weight;
}


//---------------------------------------------------------------
//
// ValidPlatform
//
//---------------------------------------------------------------
static bool ValidPlatform(const XXMLElement* elem)
{
	PRECONDITION(elem != nil);
	
	bool valid = false;
		
	const XXMLAttribute& platform = elem->FindAttribute(L"platform");
	
	if (platform.GetValue() == L"all" ||
#if MAC
		platform.GetValue() == L"mac"
	#if TARGET_CARBON
			|| platform.GetValue() == L"carbon"
	#else
			|| platform.GetValue() == L"classic_mac"
	#endif
#elif WIN
		platform.GetValue() == L"win"
#endif
	) 
		valid = true;
	
	return valid;
}


//---------------------------------------------------------------
//
// ValidTarget
//
//---------------------------------------------------------------
static bool ValidTarget(const XXMLElement* elem)
{
	PRECONDITION(elem != nil);
	
	bool valid = false;
		
	const XXMLAttribute& target = elem->FindAttribute(L"target");
	
	if (target.GetValue() == L"all"
#if DEBUG
		|| target.GetValue() == L"debug"
#endif
#if RELEASE
		|| target.GetValue() == L"release"
#endif
#if __profile__
		|| target.GetValue() == L"profile"
#endif
	) 
		valid = true;
	
	return valid;
}


//---------------------------------------------------------------
//
// GetText
//
//---------------------------------------------------------------
static std::wstring GetText(const XXMLElement* element)
{
	PRECONDITION(element != nil);
	
	std::wstring text;
	
	XXMLElement::const_item_iterator iter = element->item_begin();
	while (iter != element->item_end()) {
		const XXMLItem* item = *iter;
		++iter;
		
		const XXMLCharData* temp = dynamic_cast<const XXMLCharData*>(item);
		ASSERT(temp != nil);
		
		text += temp->GetText();
	}
	
	return text;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XXMLMenu
//!		Creates a menu or a menu bar from an XML resource.
// ===================================================================================
class XXMLMenu : public IXMLMenu {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XXMLMenu();

						XXMLMenu(XBoss* boss);
												
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		LoadMenuBar(const IMenuBarPtr& menubar, const std::wstring& menuBoss, const XURI& uri, const std::wstring& id);

	virtual void 		LoadMenu(const IMenuBarPtr& menubar, const IMenuPtr& parent, IMenuPtr& menu, const XXMLDoc& doc, const XXMLElement& element, uint32 at, uint32 id = 0);

//-----------------------------------
//	Internal Types
//
private:
	struct SEncoding {
		std::wstring	text;
		SAccelerator	accelerator;
		IMenu::SHelp	help;
	};

//-----------------------------------
//	Internal API
//
private:			
			SEncoding 	DoFindEncoding(const XXMLElement& element);

			void 		DoAppendSubMenu(IMenuPtr& menu, const XXMLDoc& doc, const XXMLElement* element, uint32 index);
			void 		DoAppendItem(IMenuPtr& menu, const XXMLElement* element, bool rebuild, uint32 index);
};

DEFINE_INTERFACE_FACTORY(XXMLMenu)

//---------------------------------------------------------------
//
// XXMLMenu::~XXMLMenu
//
//---------------------------------------------------------------
XXMLMenu::~XXMLMenu()
{
}


//---------------------------------------------------------------
//
// XXMLMenu::XXMLMenu
//
//---------------------------------------------------------------
XXMLMenu::XXMLMenu(XBoss* boss)
{
	this->DoSetBoss(boss);
}

						
//---------------------------------------------------------------
//
// XXMLMenu::LoadMenuBar
//
//---------------------------------------------------------------
void XXMLMenu::LoadMenuBar(const IMenuBarPtr& menubar, const std::wstring& menuBoss, const XURI& uri, const std::wstring& id)
{		
	XXMLDoc doc;
	Load(doc, uri);
		
	const XXMLElement* element = doc.FindElement(id);		// will throw if id isn't found
	ASSERT(element != nil);			

	uint32 count = 0;
	XXMLElement::const_item_iterator iter = element->item_begin();
	while (iter != element->item_end()) {
		const XXMLItem* item = *iter;
		++iter;
		
		const XXMLElement* elem = dynamic_cast<const XXMLElement*>(item);
		ASSERT(elem != nil);
		ASSERT(elem->GetName() == L"Entry");

		if (ValidPlatform(elem) && ValidTarget(elem)) {
			const XXMLAttribute& attr = elem->FindAttribute(L"id");
			const std::wstring& menuID = attr.GetValue();

			const XXMLElement* menuElem = doc.FindElement(menuID);
			ASSERT(menuElem != nil);

			IMenuPtr menu(menuBoss);
			this->LoadMenu(menubar, IMenuPtr(), menu, doc, *menuElem, count++);
		}
	}
}


//---------------------------------------------------------------
//
// XXMLMenu::LoadMenu
//
//---------------------------------------------------------------
void XXMLMenu::LoadMenu(const IMenuBarPtr& menubar, const IMenuPtr& parent, IMenuPtr& menu, const XXMLDoc& doc, const XXMLElement& element, uint32 at, uint32 id)
{
	UNUSED(doc);
	PRECONDITION(element.GetName() == L"Menu");
		
	// Get the title info
	XXMLElement::const_item_iterator iter = element.item_begin();
	
	const XXMLElement* elem = dynamic_cast<const XXMLElement*>(*iter);
	ASSERT(elem != nil);
	ASSERT(elem->GetName() == L"Title");
		
	SEncoding title = this->DoFindEncoding(*elem);
	++iter;
	
	ASSERT(title.accelerator.ch == 0);
	ASSERT(title.help.checked.length() == 0);
				
	// Get the optional missing item help
	IMenu::SHelp missingHelp;
	if (iter != element.item_end()) {
		elem = dynamic_cast<const XXMLElement*>(*iter);
		if (elem != nil && elem->GetName() == L"MissingHelp") {
			++iter;
			
			SEncoding missingItem = this->DoFindEncoding(*elem);		
			ASSERT(missingItem.accelerator.ch == 0);
			missingHelp = missingItem.help;
		}
	}
	
	// Initialize the menu
	menu->Init(title.text, title.help, missingHelp, id);
	if (menubar)								// will be nil for submenus
		if (at == menubar->GetCount())
			menubar->AppendMenu(menu, false);	
		else
			menubar->InstallMenu(menu, menubar->GetMenuByIndex(at), false);	

	else if (parent)
		parent->InsertSubMenu(at, menu, false);

	// Append each menu item
	uint32 index = 0;							// can't just append because the Apple menu is filled out as soon as its created...
	while (iter != element.item_end()) {
		const XXMLItem* item = *iter;
		++iter;
		
		elem = dynamic_cast<const XXMLElement*>(item);
		ASSERT(elem != nil);
		
		bool rebuild = iter == element.item_end();
		if (elem->GetName() == L"Item")
			this->DoAppendItem(menu, elem, rebuild, index++);
			
		else if (elem->GetName() == L"SubMenu")
			this->DoAppendSubMenu(menu, doc, elem, index++);
			
		else if (elem->GetName() == L"Separator")
			menu->InsertSeparator(index++, rebuild);
			
		else
			DEBUGSTR("Bogus Menu element");		// should have been caught by the parser
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XXMLMenu::DoFindEncoding	
//
//---------------------------------------------------------------
XXMLMenu::SEncoding XXMLMenu::DoFindEncoding(const XXMLElement& element)
{
	const XXMLElement* encoding = nil;
	uint32 weight = 0;
	XLocale locale;

	// Find the best encoding for the current locale	
	XXMLElement::const_item_iterator iter = element.item_begin();
	while (iter != element.item_end()) {
		const XXMLItem* item = *iter;
		++iter;
		
		const XXMLElement* candidate = dynamic_cast<const XXMLElement*>(item);
		ASSERT(candidate != nil);
		ASSERT(candidate->GetName() == L"Encoding");
		
		uint32 w = GetWeight(candidate, locale);
		if (w > weight) {
			encoding = candidate;
			weight = w;
		}
	}
	
	if (weight < kLanguageWeight) 
		TRACEFLOW("XWarning", "Couldn't find a suitable encoding");

	if (encoding == nil)
		encoding = dynamic_cast<const XXMLElement*>(element.item_front());	// not good, but probably better than refusing to run...

	// Extract the encoding info
	SEncoding result;
	
	iter = encoding->item_begin();
	while (iter != encoding->item_end()) {
		const XXMLItem* item = *iter;
		++iter;
		
		const XXMLElement* temp = dynamic_cast<const XXMLElement*>(item);
		ASSERT(temp != nil);
		
		if (ValidPlatform(temp)) {
			if (temp->GetName() == L"Text") {
				result.text= Whisper::Replace(Whisper::GetText(temp), '_', '&', '_');
#if MAC
				result.text = Replace(result.text, L"...", std::wstring(1, kEllipsisChar));
#elif WIN
				result.text = Replace(result.text, std::wstring(1, kEllipsisChar), L"...");
#endif
			
			} else if (temp->GetName() == L"Accelerator") {	
				std::wstring value = Whisper::GetText(temp);
				std::wstring part = Whisper::Parse(value, L" +");
				while (part.length() > 0) {
					if (part == L"shift") 
						result.accelerator.modifiers += kShiftModifer;
					
					else if (part == L"control") 
						result.accelerator.modifiers += kControlModifier;
					
#if MAC
					else if (part == L"alt") 
						;
	
					else if (part == L"command") 
						result.accelerator.modifiers += kCommandModifier;
						
					else if (part == L"option") 				
						result.accelerator.modifiers += kOptionModifier;
						
					else if (part == L"default")
						result.accelerator.modifiers = kCommandModifier;
			
					else if (part.compare(0, 2, L"0x") == 0)
						;

#elif WIN
					else if (part == L"alt") 
						result.accelerator.modifiers += kAltModifier;
	
					else if (part == L"default")
						result.accelerator.modifiers = kControlModifier;
			
					else if (part.compare(0, 2, L"0x") == 0)
						result.accelerator.key = StrToInt16(L"$" + part.substr(2));

					else if (part == L"command") 
						;
						
					else if (part == L"option") 				
						;
#endif
							
					else {
						ASSERT(part.length() == 1);
						result.accelerator.ch = static_cast<char>(part[0]);
					}
					
					part = Whisper::Parse(value, L" +");
				}
				
#if WIN
				XXMLElement::const_attr_iterator iter2 = temp->attr_begin();
				while (iter2 != temp->attr_end()) {
					XXMLAttribute attribute = *iter2;
					++iter2;
					
					if (attribute.GetName() == L"text")
						result.accelerator.text = attribute.GetValue();
				}
#endif
			
			} else if (temp->GetName() == L"HelpMesg") {
				result.help.enabled = Whisper::GetText(temp);
			
			} else if (temp->GetName() == L"DisabledHelp") {
				result.help.disabled = Whisper::GetText(temp);
			
			} else if (temp->GetName() == L"CheckedHelp") {
				result.help.checked = Whisper::GetText(temp);
			
			} else
				DEBUGSTR("Bad Encoding element");	// should have been caught by the parser
		}
	}

#if WIN
	if (result.accelerator.ch != 0) {
		ASSERT(result.accelerator.key == 0);
	
	} else if (result.accelerator.key != 0) {
		ASSERT(result.accelerator.ch == 0);
		ASSERT(result.accelerator.text.length() > 0);
	}
#endif

	return result;
}


//---------------------------------------------------------------
//
// XXMLMenu::DoAppendSubMenu
//
//---------------------------------------------------------------
void XXMLMenu::DoAppendSubMenu(IMenuPtr& menu, const XXMLDoc& doc, const XXMLElement* element, uint32 index)
{
	PRECONDITION(element != nil);
	PRECONDITION(element->GetName() == L"SubMenu");
	
	XXMLAttribute attribute = element->attr_front();
	ASSERT(attribute.GetName() == L"id");
	
	std::wstring id = attribute.GetValue();
	const XXMLElement* temp = doc.FindElement(id);
	
	IMenuPtr subMenu(menu->GetBoss()->GetName());
	this->LoadMenu(IMenuBarPtr(), menu, subMenu, doc, *temp, index);
}


//---------------------------------------------------------------
//
// XXMLMenu::DoAppendItem
//
//---------------------------------------------------------------
void XXMLMenu::DoAppendItem(IMenuPtr& menu, const XXMLElement* element, bool rebuild, uint32 index)
{
	PRECONDITION(element != nil);
	PRECONDITION(element->GetName() == L"Item");
	
	if (ValidPlatform(element) && ValidTarget(element)) {
		XMenuCommand command;
		uint8 style = 0;
		
		XXMLElement::const_attr_iterator iter = element->attr_begin();
		while (iter != element->attr_end()) {
			XXMLAttribute attribute = *iter;
			++iter;
			
			std::wstring value = attribute.GetValue();
			
			if (attribute.GetName() == L"command") {
				command = value;
							
			} else if (attribute.GetName() == L"style") {
#if MAC
				std::wstring s = Whisper::Parse(value, L" +");
				while (s.length() > 0) {
					if (s == L"normal") 
						style = (uint8) (style + normal);
					
					else if (s == L"bold") 
						style += bold;
			
					else if (s == L"italic") 
						style += italic;
			
					else if (s == L"underline") 
						style += underline;
			
					else if (s == L"outline") 
						style += outline;
			
					else if (s == L"shadow") 
						style += shadow;
			
					else if (s == L"condense") 
						style += condense;
			
					else if (s == L"extend") 
						style += extend;
				
					else
						DEBUGSTR("Bogus Item style");		
			
					s = Whisper::Parse(value, L" +");
				}
#endif
											
			} else if (attribute.GetName() == L"platform" || attribute.GetName() == L"target") {
				// do nothing
				
			} else
				DEBUGSTR("Bogus Item attribute");		// should have been caught by the parser
		}

		SEncoding item = this->DoFindEncoding(*element);
		menu->InsertItem(item.text, index, command, item.help, item.accelerator, rebuild);
		
#if MAC
		if (style != 0)
			menu->SetStyle(menu->GetCount() - 1, style);
#endif
	}
}


}	// namespace Whisper
