/*
 *  File:       CDeleteColorFormulaDialog.cpp
 *  Summary:   	Dialog used when deleting color formulas.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CDeleteColorFormulaDialog.cpp,v $
 *		Revision 1.3  2001/04/21 02:23:40  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2001/04/05 23:51:35  jesjones
 *		Uses IAppDialog.
 *		
 *		Revision 1.1  2001/04/03 04:30:29  jesjones
 *		Checked in	
 */

#include "AppHeader.h"
#include "IAppDialog.h"

#include <IClickExtensions.h>
#include <ICommand.h>
#include <ICommandQueue.h>
#include <IControl.h>
#include <IDialogBox.h>
#include <IFocus.h>
#include <IListBox.h>
#include <IPoseDialogBox.h>
#include <IPushButton.h>
#include <IRootControl.h>
#include <IUndoActions.h>
#include <IWindow.h>
#include <XBind.h>

#include "IColorFormulas.h"
#include "IDocInfo.h"


// ===================================================================================
//	class CDeleteColorFormulaDialog
//!		Dialog used when deleting color formulas.
// ===================================================================================
class CDeleteColorFormulaDialog : public IAppDialog {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CDeleteColorFormulaDialog();
	
						CDeleteColorFormulaDialog(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		Pose();
						
//-----------------------------------
//	Internal API
//
private:
			void 		DoSelectionChanged(IListBox* names);
			void 		DoDoubleClick();
			
			void 		DoSetUp();
			void 		DoPostCommand();
};

DEFINE_INTERFACE_FACTORY(CDeleteColorFormulaDialog)

//---------------------------------------------------------------
//
// CDeleteColorFormulaDialog::~CDeleteColorFormulaDialog
//
//---------------------------------------------------------------
CDeleteColorFormulaDialog::~CDeleteColorFormulaDialog()
{
}


//---------------------------------------------------------------
//
// CDeleteColorFormulaDialog::CDeleteColorFormulaDialog
//
//---------------------------------------------------------------
CDeleteColorFormulaDialog::CDeleteColorFormulaDialog(XBoss* boss)
{	
	IAppDialog::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CDeleteColorFormulaDialog::Pose
//
//---------------------------------------------------------------
bool CDeleteColorFormulaDialog::Pose()
{		
	// Initialize and open the dialog
	this->DoSetUp();
	
	IPoseDialogBoxPtr handler(L"Pose Dialog Box");	
	IAppDialog* thisPtr = this;
	handler->Open(thisPtr);
	
	// Select the first item
	IWindowPtr window(thisPtr);
	IRootControlPtr root = window->GetRootControl();	
	IListBoxPtr names = root->FindChild(L"Names");		
	if (names->GetCount() > 0)
		names->Select(0);

	// Make the listbox the focus
	IFocusPtr focus(names);
	focus->SetFocus();
	
	names->SetSelectionCallback(IListBox::Callback(this, &CDeleteColorFormulaDialog::DoSelectionChanged));

	// Let the dialog process events until the user presses the
	// OK or Cancel button.
	bool pressedOK = handler->Run();
	if (pressedOK) 
		this->DoPostCommand();
		
	handler->Close();
	
	return pressedOK;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CDeleteColorFormulaDialog::DoSelectionChanged
//
//---------------------------------------------------------------
void CDeleteColorFormulaDialog::DoSelectionChanged(IListBox* names)
{
	PRECONDITION(names != nil);
	
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	IControlPtr button = root->FindChild(L"ok");

	button->Enable(names->HasSelection());
}


//---------------------------------------------------------------
//
// CDeleteColorFormulaDialog::DoDoubleClick
//
//---------------------------------------------------------------
void CDeleteColorFormulaDialog::DoDoubleClick()
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	
	IListBoxPtr names = root->FindChild(L"Names");
	if (names->HasSelection()) {
		IPushButtonPtr button = root->FindChild(L"ok");
		button->SimulateClick();
	}
}


//---------------------------------------------------------------
//
// CDeleteColorFormulaDialog::DoSetUp
//
//---------------------------------------------------------------
void CDeleteColorFormulaDialog::DoSetUp()
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	IColorFormulasPtr formulas(L"Application");
	
	IListBoxPtr names = root->FindChild(L"Names");
	for (uint32 i = 0; i < formulas->GetCount(); ++i)
		names->AppendItem(formulas->Get(i), kDontRedraw);
		
	ICallbackClickExtensionPtr extension(names);
	extension->Init(XCallback0<void>(this, &CDeleteColorFormulaDialog::DoDoubleClick), 2);	
}


//---------------------------------------------------------------
//
// CDeleteColorFormulaDialog::DoPostCommand
//
//---------------------------------------------------------------
void CDeleteColorFormulaDialog::DoPostCommand()
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	IColorFormulasPtr formulas(L"Application");
	
	// Delete the selected formulas,
	SUndoSequence sequence(LoadAppString(L"Delete Color Formula"));
	IConstListBoxPtr names = root->FindChild(L"Names");
	uint32 index = names->GetSelection();
	do {
		std::wstring name = names->GetItem(index);

		// create an undo action,
		IUndoActionPtr action(L"Callback Action");
		action->Init(L"");
		
		// setup the callbacks,
		XCallback1<void, std::wstring> doIt(formulas.Get(), &IColorFormulas::Delete);
		XCallback1<void, std::wstring> undoIt(formulas.Get(), &IColorFormulas::Add);

		IUndoCallbackActionPtr undo(action);
		undo->SetCallbacks(Bind1(doIt, name), Bind1(undoIt, name));

		// and schedule the action.
		ICommandQueuePtr queue(L"Application");
		queue->Post(action);
	} while (names->GetNextSelection(&index));
}

