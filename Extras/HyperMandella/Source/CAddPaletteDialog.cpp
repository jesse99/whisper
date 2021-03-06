/*
 *  File:       CAddPaletteDialog.cpp
 *  Summary:   	Dialog used when adding palettes to the Palette menu.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CAddPaletteDialog.cpp,v $
 *		Revision 1.2  2001/04/03 01:03:59  jesjones
 *		Pose methods now take a const reference (makes it easier to passs in a temporary).
 *		
 *		Revision 1.1  2001/04/01 04:49:16  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include "IDocDialog.h"

#include <IDialogBox.h>
#include <IPoseDialogBox.h>
#include <IRootControl.h>
#include <IText.h>
#include <ITextBox.h>
#include <IWindow.h>

#include "IDocInfo.h"
#include "IPalettes.h"


// ===================================================================================
//	class CAddPaletteDialog
//!		Dialog used when adding palettes to the Palette menu.
// ===================================================================================
class CAddPaletteDialog : public IDocDialog {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CAddPaletteDialog();
	
						CAddPaletteDialog(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		Pose(const IDocInfoPtr& doc);
						
//-----------------------------------
//	Internal API
//
private:
			void 		DoSetUp(const IConstDocInfoPtr& doc);
			void 		DoPostCommand(const IDocInfoPtr& doc);
};

DEFINE_INTERFACE_FACTORY(CAddPaletteDialog)

//---------------------------------------------------------------
//
// CAddPaletteDialog::~CAddPaletteDialog
//
//---------------------------------------------------------------
CAddPaletteDialog::~CAddPaletteDialog()
{
}


//---------------------------------------------------------------
//
// CAddPaletteDialog::CAddPaletteDialog
//
//---------------------------------------------------------------
CAddPaletteDialog::CAddPaletteDialog(XBoss* boss)
{	
	IDocDialog::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CAddPaletteDialog::Pose
//
//---------------------------------------------------------------
bool CAddPaletteDialog::Pose(const IDocInfoPtr& doc)
{	
	// Initialize and open the dialog
	this->DoSetUp(doc);
	
	IPoseDialogBoxPtr handler(L"Pose Dialog Box");	
	IDocDialog* thisPtr = this;
	handler->Open(thisPtr);
	
	// Select the first textbox
	IWindowPtr window(thisPtr);
	IRootControlPtr root = window->GetRootControl();
	ITextBoxPtr textbox = root->FindChild(L"Name");
	textbox->Select();

	// Let the dialog process events until the user presses the
	// OK or Cancel button.
	bool pressedOK = handler->Run();
	if (pressedOK) 
		this->DoPostCommand(doc);
		
	handler->Close();
	
	return pressedOK;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CAddPaletteDialog::DoSetUp
//
//---------------------------------------------------------------
void CAddPaletteDialog::DoSetUp(const IConstDocInfoPtr& doc)
{
	UNUSED(doc);
	
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	
	ITextPtr textbox = root->FindChild(L"Name");
	textbox->SetText(L"", kDontRedraw);
}


//---------------------------------------------------------------
//
// CAddPaletteDialog::DoPostCommand
//
//---------------------------------------------------------------
void CAddPaletteDialog::DoPostCommand(const IDocInfoPtr& doc)
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	
	// Get the name the user picked
	IConstTextPtr textbox = root->FindChild(L"Name");
	std::wstring name = textbox->GetText();
	
	// Add the palette to the IPalettes interface
	IPalettesPtr palettes(L"Application");
	palettes->Add(name, doc->GetPalette());
}

