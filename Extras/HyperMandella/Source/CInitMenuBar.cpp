/*
 *  File:       CInitMenuBar.cpp
 *  Summary:   	Interface used to initialize the menubar.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CInitMenuBar.cpp,v $
 *		Revision 1.3  2001/04/03 04:36:51  jesjones
 *		Builds the color formula menu.
 *		
 *		Revision 1.2  2001/03/28 09:39:09  jesjones
 *		Added fractal formula menu.
 *		
 *		Revision 1.1  2000/12/28 02:49:57  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.1  2000/11/26 03:22:00  jesjones
 *		Checked in	
 */

#include "AppHeader.h"
#include <IInitMenuBar.h>

#include <IMenuBar.h>
#include <IXMLMenu.h>
#include <XObjectModel.h>
#include <XURI.h>

#include "IColorFormulas.h"
#include "IFractalFunction.h"
#include "IPalettes.h"


// ===================================================================================
//	class CInitMenuBar
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
			void 		DoBuildPaletteMenu(const IMenuBarPtr& menubar);
			void 		DoBuildFractalFormulaMenu(const IMenuBarPtr& menubar);
			void 		DoBuildColorFormulaMenu(const IMenuBarPtr& menubar);
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

	this->DoBuildPaletteMenu(menubar);
	this->DoBuildFractalFormulaMenu(menubar);
	this->DoBuildColorFormulaMenu(menubar);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CInitMenuBar::DoBuildPaletteMenu
//
//---------------------------------------------------------------
void CInitMenuBar::DoBuildPaletteMenu(const IMenuBarPtr& menubar)
{
	IPalettesPtr palettes(L"Application");
	IMenuPtr menu = menubar->GetMenuByName(LoadAppString(L"Palette"), kNonRecursive);
	
	uint32 count = palettes->GetCount();
	for (uint32 index = 0; index < count; ++index) {
		std::wstring name =  palettes->GetName(index);
		std::wstring temp = Replace(name, L"&", L"&&");
		
		menu->InsertSortedItem(temp, 0, index, L"Palette: " + name);
	}
}


//---------------------------------------------------------------
//
// CInitMenuBar::DoBuildColorFormulaMenu
//
//---------------------------------------------------------------
void CInitMenuBar::DoBuildColorFormulaMenu(const IMenuBarPtr& menubar)
{
	IColorFormulasPtr formulas(L"Application");
	IMenuPtr menu = menubar->GetMenuByName(LoadAppString(L"Color Using"), kRecursive);
	
	uint32 count = formulas->GetCount();
	for (uint32 index = 0; index < count; ++index) {
		std::wstring name =  formulas->Get(index);
		
		menu->InsertSortedItem(name, 0, index, L"Color Formula: " + name);
	}
}


//---------------------------------------------------------------
//
// CInitMenuBar::DoBuildFractalFormulaMenu
//
//---------------------------------------------------------------
void CInitMenuBar::DoBuildFractalFormulaMenu(const IMenuBarPtr& menubar)
{
	IMenuPtr menu = menubar->GetMenuByName(LoadAppString(L"Formula"), kRecursive);
	
	uint32 count = 0;
	XBoss* boss = XObjectModel::Instance()->CreateBoss(L"Fractal Functions");	// singleton boss
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end()) {
		IFractalFunctionPtr function(boss, iter);
		++iter;

		std::wstring formula = function->GetFormula();
		menu->InsertSortedItem(formula, 0, count++, L"Fractal Formula: " + formula);
	}
}


