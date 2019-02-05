/*
 *  File:       CInt32Dialog.cpp
 *  Summary:   	Interface used to change int32's on the document.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CInt32Dialog.cpp,v $
 *		Revision 1.1  2001/03/26 04:04:05  jesjones
 *		Checked in
 *		
 */

#include "AppHeader.h"
#include "IInt32Dialog.h"

#include <IClickExtensions.h>
#include <IDialogBox.h>
#include <IPoseDialogBox.h>
#include <IRange.h>
#include <IRootControl.h>
#include <ITextBox.h>
#include <IValue.h>
#include <IWindow.h>
#include <XMiscUtils.h>

#include "IDocInfo.h"


// ===================================================================================
//	class CInt32Dialog
//!		Interface used to change int32's on the document.
// ===================================================================================
class CInt32Dialog : public IInt32Dialog {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CInt32Dialog();
	
						CInt32Dialog(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		Pose(int32 value, XCallback1<void, int32> callback);

//-----------------------------------
//	Internal API
//
private:
			void 		DoSetUp(int32 value);
			void 		DoPostCommand(XCallback1<void, int32> callback);
};

DEFINE_INTERFACE_FACTORY(CInt32Dialog)

//---------------------------------------------------------------
//
// CInt32Dialog::~CInt32Dialog
//
//---------------------------------------------------------------
CInt32Dialog::~CInt32Dialog()
{
}


//---------------------------------------------------------------
//
// CInt32Dialog::CInt32Dialog
//
//---------------------------------------------------------------
CInt32Dialog::CInt32Dialog(XBoss* boss) 
{	
	IInt32Dialog::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CInt32Dialog::Pose
//
//---------------------------------------------------------------
bool CInt32Dialog::Pose(int32 value, XCallback1<void, int32> callback)
{	
	// Initialize and open the dialog
	this->DoSetUp(value);
		
	IPoseDialogBoxPtr handler(L"Pose Dialog Box");	
	handler->Open(this);
	
	// Select the textbox
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	ITextBoxPtr textbox = root->FindChild(L"Value");
	textbox->Select();

	// Let the dialog process events until the user presses the
	// OK or Cancel button.
	bool pressedOK = handler->Run();
	if (pressedOK) 
		this->DoPostCommand(callback);
		
	handler->Close();
	
	return pressedOK;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CInt32Dialog::DoSetUp
//
//---------------------------------------------------------------
void CInt32Dialog::DoSetUp(int32 value)
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	
	IInt32ValuePtr textbox = root->FindChild(L"Value");
	textbox->Set(value, kDontRedraw);
}


//---------------------------------------------------------------
//
// CInt32Dialog::DoPostCommand
//
//---------------------------------------------------------------
void CInt32Dialog::DoPostCommand(XCallback1<void, int32> callback)
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	
	IConstInt32ValuePtr textbox = root->FindChild(L"Value");
	int32 value = textbox->Get();

	callback(value);
}


