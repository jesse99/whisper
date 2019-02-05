/*
 *  File:       IXMLMenu.h
 *  Summary:   	Creates a menu or a menu bar from an XML resource.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IXMLMenu.h,v $
 *		Revision 1.6  2001/04/13 07:47:40  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.5  2001/04/06 04:46:06  jesjones
 *		LoadMenu takes a parent menu.
 *		
 *		Revision 1.4  2001/03/05 05:39:01  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:26:24  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:08:11  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class IMenu;
class IMenuBar;
class XURI;
class XXMLDoc;
class XXMLElement;

typedef XInterfacePtr<IMenu> IMenuPtr;
typedef XInterfacePtr<IMenuBar> IMenuBarPtr;


// ===================================================================================
//	class IXMLMenu
//!		Creates a menu or a menu bar from an XML resource.
// ===================================================================================
class IXMLMenu : public XUnknown {

	typedef XUnknown Inherited;

//-----------------------------------
//	API
//
public:
	virtual void 		LoadMenuBar(const IMenuBarPtr& menubar, const std::wstring& menuBoss, 
									const XURI& uri, const std::wstring& id) = 0;
						/**< Uri should point to a "Menus: XML element. ID should point to
						a "MenuBar" element. */

	virtual void 		LoadMenu(const IMenuBarPtr& menubar, const IMenuPtr& parent, IMenuPtr& menu, 
								 const XXMLDoc& doc, const XXMLElement& element, uint32 at, uint32 id = 0) = 0;
						/**< Initializes a menu using a "Menu" element. For submenus menubar 
						will be nil and parent will be non-nil. */
};

typedef XInterfacePtr<IXMLMenu> IXMLMenuPtr;
typedef XInterfacePtr<const IXMLMenu> IConstXMLMenuPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
