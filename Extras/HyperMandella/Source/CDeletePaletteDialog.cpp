/*
 *  File:       CDeletePaletteDialog.cpp
 *  Summary:   	Dialog used when deleting palettes from the Palette menu.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CDeletePaletteDialog.cpp,v $
 *		Revision 1.4  2001/04/21 02:23:44  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/05 23:51:41  jesjones
 *		Uses IAppDialog.
 *		
 *		Revision 1.2  2001/04/03 01:04:07  jesjones
 *		Pose methods now take a const reference (makes it easier to passs in a temporary).
 *		
 *		Revision 1.1  2001/04/01 04:49:22  jesjones
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

#include "IDocInfo.h"
#include "IPalettes.h"


// ===================================================================================
//	class CDeletePaletteDialog
//!		Dialog used when deleting palettes from the Palette menu.
// ===================================================================================
class CDeletePaletteDialog : public IAppDialog {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CDeletePaletteDialog();
	
						CDeletePaletteDialog(XBoss* boss);
						
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

DEFINE_INTERFACE_FACTORY(CDeletePaletteDialog)

//---------------------------------------------------------------
//
// CDeletePaletteDialog::~CDeletePaletteDialog
//
//---------------------------------------------------------------
CDeletePaletteDialog::~CDeletePaletteDialog()
{
}


//---------------------------------------------------------------
//
// CDeletePaletteDialog::CDeletePaletteDialog
//
//---------------------------------------------------------------
CDeletePaletteDialog::CDeletePaletteDialog(XBoss* boss)
{	
	IAppDialog::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CDeletePaletteDialog::Pose
//
//---------------------------------------------------------------
bool CDeletePaletteDialog::Pose()
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
	
	names->SetSelectionCallback(IListBox::Callback(this, &CDeletePaletteDialog::DoSelectionChanged));

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
// CDeletePaletteDialog::DoSelectionChanged
//
//---------------------------------------------------------------
void CDeletePaletteDialog::DoSelectionChanged(IListBox* names)
{
	PRECONDITION(names != nil);
	
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	IControlPtr button = root->FindChild(L"ok");

	button->Enable(names->HasSelection());
}


//---------------------------------------------------------------
//
// CDeletePaletteDialog::DoDoubleClick
//
//---------------------------------------------------------------
void CDeletePaletteDialog::DoDoubleClick()
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
// CDeletePaletteDialog::DoSetUp
//
//---------------------------------------------------------------
void CDeletePaletteDialog::DoSetUp()
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	IPalettesPtr palettes(L"Application");
	
	IListBoxPtr names = root->FindChild(L"Names");
	for (uint32 i = 0; i < palettes->GetCount(); ++i)
		names->AppendItem(palettes->GetName(i), kDontRedraw);
		
	ICallbackClickExtensionPtr extension(names);
	extension->Init(XCallback0<void>(this, &CDeletePaletteDialog::DoDoubleClick), 2);	
}


//---------------------------------------------------------------
//
// CDeletePaletteDialog::DoPostCommand
//
//---------------------------------------------------------------
void CDeletePaletteDialog::DoPostCommand()
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	IPalettesPtr palettes(L"Application");
	
	// Delete the selected palettes,
	SUndoSequence sequence(LoadAppString(L"Delete Palette"));
	IConstListBoxPtr names = root->FindChild(L"Names");
	uint32 index = names->GetSelection();
	do {
		std::wstring name = names->GetItem(index);
		XColorTable colors = palettes->Get(name);

		// create an undo action,
		IUndoActionPtr action(L"Callback Action");
		action->Init(L"");
		
		// setup the callbacks,
		XCallback1<void, std::wstring> doIt(palettes.Get(), &IPalettes::Delete);
		XCallback2<void, std::wstring, XColorTable> undoIt(palettes.Get(), &IPalettes::Add);

		IUndoCallbackActionPtr undo(action);
		undo->SetCallbacks(Bind1(doIt, name), Bind2(undoIt, name, colors));

		// and schedule the action.
		ICommandQueuePtr queue(L"Application");
		queue->Post(action);
	} while (names->GetNextSelection(&index));
}

