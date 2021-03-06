/*
 *  File:       CEditColorFormulaDialog.cpp
 *  Summary:   	Dialog used when changing the color formula.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CEditColorFormulaDialog.cpp,v $
 *		Revision 1.1  2001/04/03 18:10:08  jesjones
 *		Checked in
 *		
 */

#include "AppHeader.h"
#include "IDocDialog.h"

#include <IDialogBox.h>
#include <IPoseDialogBox.h>
#include <IRootControl.h>
#include <IText.h>
#include <ITextBox.h>
#include <IValidate.h>
#include <IWindow.h>
#include <XAutoPtr.h>
#include <XError.h>

#include "IDocCommands.h"
#include "IDocInfo.h"
#include "CColorParser.h"


// ===================================================================================
//	class CEditColorFormulaDialog
//!		Dialog used when changing the color formula.
// ===================================================================================
class CEditColorFormulaDialog : public IDocDialog, public IValidate {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CEditColorFormulaDialog();
	
						CEditColorFormulaDialog(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		Pose(const IDocInfoPtr& doc);
						
	virtual bool 		Validate() const;

//-----------------------------------
//	Internal API
//
private:
			void 		DoSetUp(const IConstDocInfoPtr& doc);
			void 		DoPostCommand(const IDocInfoPtr& doc);
};

DEFINE_INTERFACE_FACTORY(CEditColorFormulaDialog)

//---------------------------------------------------------------
//
// CEditColorFormulaDialog::~CEditColorFormulaDialog
//
//---------------------------------------------------------------
CEditColorFormulaDialog::~CEditColorFormulaDialog()
{
}


//---------------------------------------------------------------
//
// CEditColorFormulaDialog::CEditColorFormulaDialog
//
//---------------------------------------------------------------
CEditColorFormulaDialog::CEditColorFormulaDialog(XBoss* boss)
{	
	IDocDialog::DoSetBoss(boss);
	IValidate::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CEditColorFormulaDialog::Pose
//
//---------------------------------------------------------------
bool CEditColorFormulaDialog::Pose(const IDocInfoPtr& doc)
{	
	// Initialize and open the dialog
	this->DoSetUp(doc);
	
	IPoseDialogBoxPtr handler(L"Pose Dialog Box");	
	IDocDialog* thisPtr = this;
	handler->Open(thisPtr);
	
	// Select the first textbox
	IWindowPtr window(thisPtr);
	IRootControlPtr root = window->GetRootControl();
	ITextBoxPtr textbox = root->FindChild(L"Formula");
	textbox->Select();

	// Let the dialog process events until the user presses the
	// OK or Cancel button.
	bool pressedOK = handler->Run();
	if (pressedOK) 
		this->DoPostCommand(doc);
		
	handler->Close();
	
	return pressedOK;
}


//---------------------------------------------------------------
//
// CEditColorFormulaDialog::Validate
//
//---------------------------------------------------------------
bool CEditColorFormulaDialog::Validate() const
{
	bool valid = true;
	
	const IDocDialog* thisPtr = this;
	IConstWindowPtr window(thisPtr);
	IConstRootControlPtr root = window->GetRootControl();

	IConstTextPtr textbox = root->FindChild(L"Formula");
	std::wstring formula = textbox->GetText();

	try {
		XAutoPtr<CColorTree> tree(CColorTree::Create(formula));
		
	} catch (const XParserException& e) {
		XError::Instance()->ReportError(L"Couldn't parse the formula.", e);
			
		valid = false;

	} catch (const std::exception& e) {
		XError::Instance()->ReportError(L"Couldn't parse the formula.", e);
		valid = false;
	
	} catch (...) {
		XError::Instance()->ReportError(L"Couldn't parse the formula.", L"");
		valid = false;
	}
			
	return valid;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CEditColorFormulaDialog::DoSetUp
//
//---------------------------------------------------------------
void CEditColorFormulaDialog::DoSetUp(const IConstDocInfoPtr& doc)
{
	UNUSED(doc);
	
	IDocDialog* thisPtr = this;
	IWindowPtr window(thisPtr);
	IRootControlPtr root = window->GetRootControl();
	
	wstring formula = doc->GetShaderInfo().colorFormula;
	
	ITextPtr textbox = root->FindChild(L"Formula");
	textbox->SetText(formula, kDontRedraw);
}


//---------------------------------------------------------------
//
// CEditColorFormulaDialog::DoPostCommand
//
//---------------------------------------------------------------
void CEditColorFormulaDialog::DoPostCommand(const IDocInfoPtr& doc)
{
	IDocDialog* thisPtr = this;
	IWindowPtr window(thisPtr);
	IRootControlPtr root = window->GetRootControl();
	
	// Get the formula the user entered
	IConstTextPtr textbox = root->FindChild(L"Formula");
	std::wstring formula = textbox->GetText();
	
	// Post a command to change the formula
	IDocCommandsPtr commands(doc);
	commands->ChangeColorFormula(formula);
}

