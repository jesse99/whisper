/*
 *  File:       CAppMenuHandler.cpp
 *  Summary:   	The interface that handles menu commands directed to the app.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CAppMenuHandler.cpp,v $
 *		Revision 1.11  2001/05/06 10:35:28  jesjones
 *		Disabled a few menu commands when a dialog is up.
 *		
 *		Revision 1.10  2001/04/21 02:23:03  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.9  2001/04/06 04:41:13  jesjones
 *		Uses IActiveMenuBar to get the menubar (so Windows works).
 *		
 *		Revision 1.8  2001/04/05 23:50:05  jesjones
 *		Added IAppDialog and a pref dialog.
 *		
 *		Revision 1.7  2001/04/04 04:49:17  jesjones
 *		Added 2001 to the About box's copyright notice.
 *		
 *		Revision 1.6  2001/04/03 04:34:07  jesjones
 *		Added color formula dialogs.
 *		
 *		Revision 1.5  2001/04/01 04:51:59  jesjones
 *		Added add palette and delete palette dialogs.
 *		
 *		Revision 1.4  2001/03/28 09:36:16  jesjones
 *		Added a comment.
 *		
 *		Revision 1.3  2001/02/09 04:29:23  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.2  2000/12/31 04:17:20  jesjones
 *		Fixed the header comment.
 *		
 *		Revision 1.1  2000/12/28 02:48:10  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:52:25  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include <IInstallMenuHandler.h>

#include <IActiveMenuBar.h>
#include <IApplication.h>
#include <IDocView.h>
#include <IHierarchy.h>
#include <IMenuBar.h>
#include <IMenuHandler.h>
#include <IReadXML.h>
#include <IWindowMgr.h>
#include <XDialogUtils.h>
#include <XURI.h>

#include "IAppDialog.h"
#include "IColorFormulas.h"
#include "IDocInfo.h"
#include "IPalettes.h"


// ===================================================================================
//	class CAppMenuHandler
//!		The interface that handles menu commands directed to the app.
// ===================================================================================
class CAppMenuHandler : public IInstallMenuHandler {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~CAppMenuHandler();
	
						CAppMenuHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		InstallCommands();	
	virtual void 		RemoveCommands();
	
//-----------------------------------
//	Internal API
//
private:
			void 		DoDeletePalette();
			void 		DoDeleteColorFormula();
			void 		DoAbout();
			void 		DoPrefs();

			SCommandStatus DoEnableDeletePalette();
			SCommandStatus DoEnableDeleteColorFormula();
			
			void 		DoAddedPalette(const std::wstring& name);
			void 		DoDeletedPalette(const std::wstring& name);
			
			void 		DoAddedColorFormula(const std::wstring& name);
			void 		DoDeletedColorFormula(const std::wstring& name);
};

DEFINE_INTERFACE_FACTORY(CAppMenuHandler)

//---------------------------------------------------------------
//
// CAppMenuHandler::~CAppMenuHandler
//
//---------------------------------------------------------------
CAppMenuHandler::~CAppMenuHandler()
{
}


//---------------------------------------------------------------
//
// CAppMenuHandler::CAppMenuHandler
//
//---------------------------------------------------------------
CAppMenuHandler::CAppMenuHandler(XBoss* boss) 
{	
	IInstallMenuHandler::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CAppMenuHandler::InstallCommands
//
//---------------------------------------------------------------
void CAppMenuHandler::InstallCommands()
{
	IMenuHandlerPtr handler(L"Application");
	
	// About
	XCallback0<void> action(this, &CAppMenuHandler::DoAbout);
	handler->RegisterCommand(kAboutCmd, action, kDisabledIfDialog, this);			
	
	// Delete Color Formula
	action.Set(this, &CAppMenuHandler::DoDeleteColorFormula);	
	XCallback0<SCommandStatus> enabler(this, &CAppMenuHandler::DoEnableDeleteColorFormula);
	handler->RegisterCommand(L"Delete Color Formula", action, enabler, this);
	
	// Delete Palette
	action.Set(this, &CAppMenuHandler::DoDeletePalette);	
	enabler.Set(this, &CAppMenuHandler::DoEnableDeletePalette);
	handler->RegisterCommand(L"Delete Palette", action, enabler, this);
	
	// Preferences
	action.Set(this, &CAppMenuHandler::DoPrefs);	
	handler->RegisterCommand(L"Preferences", action, kDisabledIfDialog, this);
	
	// IPalettes callbacks
	IPalettesPtr palettes(L"Application");
	palettes->SetAddCallback(XCallback1<void, std::wstring>(this, &CAppMenuHandler::DoAddedPalette));
	palettes->SetDeleteCallback(XCallback1<void, std::wstring>(this, &CAppMenuHandler::DoDeletedPalette));
	
	// IColorFormulas callbacks
	IColorFormulasPtr formulas(L"Application");
	formulas->SetAddCallback(XCallback1<void, std::wstring>(this, &CAppMenuHandler::DoAddedColorFormula));
	formulas->SetDeleteCallback(XCallback1<void, std::wstring>(this, &CAppMenuHandler::DoDeletedColorFormula));
}


//---------------------------------------------------------------
//
// CAppMenuHandler::RemoveCommands
//
//---------------------------------------------------------------
void CAppMenuHandler::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");
	
	// About
	XCallback0<void> action(this, &CAppMenuHandler::DoAbout);
	handler->UnRegisterCommand(action);			
	
	// Delete Color Formula
	action.Set(this, &CAppMenuHandler::DoDeleteColorFormula);
	handler->UnRegisterCommand(action);	
	
	// Delete Palette
	action.Set(this, &CAppMenuHandler::DoDeletePalette);
	handler->UnRegisterCommand(action);	

	// Preferences
	action.Set(this, &CAppMenuHandler::DoPrefs);	
	handler->UnRegisterCommand(action);	
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CAppMenuHandler::DoAbout
//
//---------------------------------------------------------------
void CAppMenuHandler::DoAbout()
{
	Whisper::DoNote(LoadAppString(L"HyperMandella 1.0"), LoadAppString(L"A 4D fractal renderer written using the Whisper framework. Jesse Jones (c) 2000-2001"));
}


//---------------------------------------------------------------
//
// CAppMenuHandler::DoPrefs
//
//---------------------------------------------------------------
void CAppMenuHandler::DoPrefs()
{
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IAppDialogPtr dialog = persist->CreateBoss(uri, L"Pref_Dialog", app);
		
	dialog->Pose();
}

	
//---------------------------------------------------------------
//
// CAppMenuHandler::DoDeleteColorFormula
//
//---------------------------------------------------------------
void CAppMenuHandler::DoDeleteColorFormula()
{
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IAppDialogPtr dialog = persist->CreateBoss(uri, L"Delete_Color_Formula_Dialog", app);
		
	dialog->Pose();
}


//---------------------------------------------------------------
//
// CAppMenuHandler::DoDeletePalette
//
//---------------------------------------------------------------
void CAppMenuHandler::DoDeletePalette()
{
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IAppDialogPtr dialog = persist->CreateBoss(uri, L"Delete_Palette_Dialog", app);
		
	dialog->Pose();
}


//---------------------------------------------------------------
//
// CAppMenuHandler::DoEnableDeleteColorFormula
//
//---------------------------------------------------------------
SCommandStatus CAppMenuHandler::DoEnableDeleteColorFormula()
{
	SCommandStatus status;

	IWindowMgrPtr windowMgr(this);
	if (windowMgr->GetTopModal() == nil) {
		IColorFormulasPtr formulas(L"Application");
		status.enabled = formulas->GetCount() > 0;
	}
	
	return status;
}


//---------------------------------------------------------------
//
// CAppMenuHandler::DoEnableDeletePalette
//
//---------------------------------------------------------------
SCommandStatus CAppMenuHandler::DoEnableDeletePalette()
{
	SCommandStatus status;

	IWindowMgrPtr windowMgr(this);
	if (windowMgr->GetTopModal() == nil) {
		IPalettesPtr palettes(L"Application");
		status.enabled = palettes->GetCount() > 0;
	}
	
	return status;
}


//---------------------------------------------------------------
//
// CAppMenuHandler::DoAddedPalette
//
//---------------------------------------------------------------
void CAppMenuHandler::DoAddedPalette(const std::wstring& name)
{
	IActiveMenuBarPtr active(L"Application");
	IMenuBarPtr menubar = active->Get();
	IMenuPtr menu = menubar->GetMenuByName(LoadAppString(L"Palette"), kNonRecursive);
	IPalettesPtr palettes(L"Application");
	
	std::wstring temp = Replace(name, L"&", L"&&");
		
	uint32 count = palettes->GetCount();
	if (count > 1)
		menu->InsertSortedItem(temp, 0, count-1, L"Palette: " + name);
	else
		menu->InsertItem(temp, 0, L"Palette: " + name);
}


//---------------------------------------------------------------
//
// CAppMenuHandler::DoDeletedPalette
//
//---------------------------------------------------------------
void CAppMenuHandler::DoDeletedPalette(const std::wstring& name)
{
	IActiveMenuBarPtr active(L"Application");
	IMenuBarPtr menubar = active->Get();
	IMenuPtr menu = menubar->GetMenuByName(LoadAppString(L"Palette"), kNonRecursive);
	IPalettesPtr palettes(L"Application");
	
	bool found = false;
	uint32 count = palettes->GetCount();
	for (uint32 i = 0; i < count && !found; ++i) {
		std::wstring candidate = menu->GetName(i);
		if (candidate == name) {
			menu->RemoveItem(i);
			found = true;
		}
	}
	
	POSTCONDITION(found);
}


//---------------------------------------------------------------
//
// CAppMenuHandler::DoAddedColorFormula
//
//---------------------------------------------------------------
void CAppMenuHandler::DoAddedColorFormula(const std::wstring& name)
{
	IActiveMenuBarPtr active(L"Application");
	IMenuBarPtr menubar = active->Get();
	IMenuPtr menu = menubar->GetMenuByName(LoadAppString(L"Color Using"), kRecursive);
	IColorFormulasPtr formulas(L"Application");
			
	uint32 count = formulas->GetCount();
	if (count > 1)
		menu->InsertSortedItem(name, 0, count-1, L"Color Formula: " + name);
	else
		menu->InsertItem(name, 0, L"Color Formula: " + name);
}


//---------------------------------------------------------------
//
// CAppMenuHandler::DoDeletedColorFormula
//
//---------------------------------------------------------------
void CAppMenuHandler::DoDeletedColorFormula(const std::wstring& name)
{
	IActiveMenuBarPtr active(L"Application");
	IMenuBarPtr menubar = active->Get();
	IMenuPtr menu = menubar->GetMenuByName(LoadAppString(L"Color Using"), kRecursive);
	IColorFormulasPtr formulas(L"Application");
	
	bool found = false;
	uint32 count = formulas->GetCount();
	for (uint32 i = 0; i < count && !found; ++i) {
		std::wstring candidate = menu->GetName(i);
		if (candidate == name) {
			menu->RemoveItem(i);
			found = true;
		}
	}
	
	POSTCONDITION(found);
}


