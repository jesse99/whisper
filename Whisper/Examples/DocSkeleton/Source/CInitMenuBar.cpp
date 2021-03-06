/*
 *  File:       CInitMenuBar.cpp
 *  Summary:   	Implementation used to initialize the menubar.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CInitMenuBar.cpp,v $
 *		Revision 1.5  2001/11/25 23:36:11  jesjones
 *		Removed Quit from the file menu when running with aqua.
 *		
 *		Revision 1.4  2001/02/23 10:07:28  jesjones
 *		Limited the number of color submenu entries to 16 on Windows (app starts up too slowly if there are too many menu items).
 *		
 *		Revision 1.3  2001/01/03 05:48:24  jesjones
 *		Disabled some of the color menus on Windows (too slow).
 *		
 *		Revision 1.2  2001/01/01 03:42:38  jesjones
 *		Changed the app menu commands to use mixed case names.
 *		
 *		Revision 1.1  2000/12/31 04:23:34  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include <IInitMenuBar.h>

#include <IMenuBar.h>
#include <IXMLMenu.h>
#include <XColorSet.h>
#include <XIntConversions.h>
#include <XURI.h>

#if MAC
	#include <MSystemInfo.h>
#endif

using namespace Whisper;
using namespace std;


// ===================================================================================
//	class CInitMenuBar
//!		Implementation used to initialize the menubar. 
/*!		On the Mac this is attached to the application boss. On Windows it is attached 
 *		to each window. */
// ===================================================================================
class CInitMenuBar : public IInitMenuBar {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CInitMenuBar();
	
  						CInitMenuBar(XBoss* boss);	

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(OSWindow window);

//-----------------------------------
//	Internal API
//
private:
			void 		DoAddColors(const IMenuBarPtr& menubar, const std::wstring& name, const std::wstring& command, const XColorSet& colors);
};

DEFINE_INTERFACE_FACTORY(CInitMenuBar)

//---------------------------------------------------------------
//
// CInitMenuBar::~CInitMenuBar
//
//---------------------------------------------------------------
CInitMenuBar::~CInitMenuBar()
{
}


//---------------------------------------------------------------
//
// CInitMenuBar::CInitMenuBar
//
//---------------------------------------------------------------
CInitMenuBar::CInitMenuBar(XBoss* boss)
{
	IInitMenuBar::DoSetBoss(boss);
}
	
	
//---------------------------------------------------------------
//
// CInitMenuBar::Init
//
//---------------------------------------------------------------
void CInitMenuBar::Init(OSWindow window)
{
	IMenuBarPtr menubar(this);
	menubar->Init(window);

	IXMLMenuPtr xmlMenu(L"Application");
	xmlMenu->LoadMenuBar(menubar, L"Menu", XURI(L"resource://Menus.bXML"), L"MenuBar");

	this->DoAddColors(menubar, L"Whisper Colors", L"Whisper Color: ", kWhisperColors);
	this->DoAddColors(menubar, L"HTML Colors", L"HTML Color: ", kHTMLColors);
	this->DoAddColors(menubar, L"Crayola Colors", L"Crayola Color: ", kCrayolaColors);
	this->DoAddColors(menubar, L"Netscape Colors", L"Netscape Color: ", kNetscapeColors);
	
#if MAC
	if (MSystemInfo::HasAqua())
	{
		IMenuPtr menu = menubar->GetMenuByName(LoadAppString(L"File"));
		
		uint32 count = menu->GetCount();
		menu->RemoveItem(count - 1);	// Quit
		menu->RemoveItem(count - 2);	// Seperator
	}
#endif
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CInitMenuBar::DoAddColors
//
//---------------------------------------------------------------
void CInitMenuBar::DoAddColors(const IMenuBarPtr& menubar, const std::wstring& name, const std::wstring& command, const XColorSet& colors)
{
	IMenuPtr menu = menubar->GetMenuByName(LoadAppString(name), kRecursive);
		
	uint32 count = colors.size();
#if WIN
	count = std::min(count, 16UL);		// Windows bogs down pretty badly with long menus...
#endif
	for (uint32 i = 0; i < count; ++i)
		menu->InsertSortedItem(colors[i].name, 0, i, command + UInt32ToStr(i));
}



