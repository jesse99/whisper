/*
 *  File:       CWidthDialog.cpp
 *  Summary:   	IDialog implementation used to change the pen width.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CWidthDialog.cpp,v $
 *		Revision 1.3  2001/02/17 10:25:01  jesjones
 *		Tweaked some comments.
 *		
 *		Revision 1.2  2001/01/01 03:44:25  jesjones
 *		Uses a helper function when posting an undo action.
 *		
 *		Revision 1.1  2000/12/31 04:23:56  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include "IDialog.h"

#include <IDialogBox.h>
#include <IPoseDialogBox.h>
#include <IRootControl.h>
#include <ITextBox.h>
#include <IUndoActions.h>
#include <IValue.h>
#include <IWindow.h>
#include <XBind.h>

#include "IDocInfo.h"


// ===================================================================================
//	class CWidthDialog
//!		IDialog implementation used to change the pen width.
// ===================================================================================
class CWidthDialog : public IDialog {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CWidthDialog();
	
						CWidthDialog(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Pose(IDocInfoPtr& doc);
						
//-----------------------------------
//	Internal API
//
private:
			void 		DoSetUp(const IConstDocInfoPtr& doc);
			void 		DoPostCommand(IDocInfoPtr& doc);
};

DEFINE_INTERFACE_FACTORY(CWidthDialog)

//---------------------------------------------------------------
//
// CWidthDialog::~CWidthDialog
//
//---------------------------------------------------------------
CWidthDialog::~CWidthDialog()
{
}


//---------------------------------------------------------------
//
// CWidthDialog::CWidthDialog
//
//---------------------------------------------------------------
CWidthDialog::CWidthDialog(XBoss* boss) 
{	
	IDialog::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CWidthDialog::Pose
//
//---------------------------------------------------------------
void CWidthDialog::Pose(IDocInfoPtr& doc)
{	
	// Initialize and open the dialog.
	this->DoSetUp(doc);
	
	IPoseDialogBoxPtr handler(L"Pose Dialog Box");	
	handler->Open(this);
	
	// Select the textbox (note that on Windows we have to do this
	// after we call IPoseDialogBox::Open).
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	ITextBoxPtr textbox = root->FindChild(L"Text");
	textbox->Select();

	// Let the dialog process events until the user presses the
	// OK or Cancel button.
	if (handler->Run()) 
		this->DoPostCommand(doc);
		
	handler->Close();
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CWidthDialog::DoSetUp
//
//---------------------------------------------------------------
void CWidthDialog::DoSetUp(const IConstDocInfoPtr& doc)
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	
	IInt32ValuePtr textbox = root->FindChild(L"Text");
	textbox->Set(doc->GetPenWidth(), kDontRedraw);
}


//---------------------------------------------------------------
//
// CWidthDialog::DoPostCommand
//
//---------------------------------------------------------------
void CWidthDialog::DoPostCommand(IDocInfoPtr& doc)
{
	// Get the new width value.
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	
	IConstInt32ValuePtr textbox = root->FindChild(L"Text");
	int32 newValue = textbox->Get();

	// If the width has changed then,
	int32 oldValue = doc->GetPenWidth();
	if (newValue != oldValue) {
	
		// create a callback pointing to the SetPenWidth method,
		XCallback2<void, int32, bool> callback(doc.Get(), &IDocInfo::SetPenWidth);
		
		// create zero-argument callbacks by binding the pen widths
		// to our original callback,
		XCallback0<void> doIt = Bind2(callback, newValue, true);
		XCallback0<void> undoIt = Bind2(callback, oldValue, true);

		// and hand the zero-argument callbacks off to IUndoCallbackAction
		// to support undo.
		IUndoCallbackAction::Post(LoadAppString(L"Change Pen Width"), doIt, undoIt);
	}
}





