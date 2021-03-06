/*
 *  File:       CPrefDialog.cpp
 *  Summary:   	Dialog used when editing the app preferences.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CPrefDialog.cpp,v $
 *		Revision 1.1  2001/04/05 23:45:22  jesjones
 *		Checked in
 *		
 */

#include "AppHeader.h"
#include "IAppDialog.h"

#include <IControl.h>
#include <IDialogBox.h>
#include <IPoseDialogBox.h>
#include <IRootControl.h>
#include <ITextBox.h>
#include <IValue.h>
#include <IWindow.h>
#include <XPreference.h>


// ===================================================================================
//	class CPrefDialog
//!		Dialog used when editing the app preferences.
// ===================================================================================
class CPrefDialog : public IAppDialog {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CPrefDialog();
	
						CPrefDialog(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		Pose();
						
//-----------------------------------
//	Internal API
//
private:			
			void 		DoSetUp();
			void 		DoPostCommand();
};

DEFINE_INTERFACE_FACTORY(CPrefDialog)

//---------------------------------------------------------------
//
// CPrefDialog::~CPrefDialog
//
//---------------------------------------------------------------
CPrefDialog::~CPrefDialog()
{
}


//---------------------------------------------------------------
//
// CPrefDialog::CPrefDialog
//
//---------------------------------------------------------------
CPrefDialog::CPrefDialog(XBoss* boss)
{	
	IAppDialog::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CPrefDialog::Pose
//
//---------------------------------------------------------------
bool CPrefDialog::Pose()
{		
	// Initialize and open the dialog
	this->DoSetUp();
	
	IPoseDialogBoxPtr handler(L"Pose Dialog Box");	
	IAppDialog* thisPtr = this;
	handler->Open(thisPtr);
	
	// Select the first textbox
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	ITextBoxPtr textbox = root->FindChild(L"Width");
	textbox->Select();

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
// CPrefDialog::DoSetUp
//
//---------------------------------------------------------------
void CPrefDialog::DoSetUp()
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();

	// Width
	XPreference<int32> defaultWidth(L"Default Width", 256);
	IInt32ValuePtr width = root->FindChild(L"Width");
	width->Set(*defaultWidth, kDontRedraw);

	// Height
	XPreference<int32> defaultHeight(L"Default Height", 256);
	IInt32ValuePtr height = root->FindChild(L"Height");
	height->Set(*defaultHeight, kDontRedraw);

	// Number of Colors
	XPreference<bool> useMillions(L"Use Millions of Colors", true);
	IInt32ValuePtr radios = root->FindChild(L"Number of Colors");
	radios->Set(*useMillions);

	// On Startup
	XPreference<int32> onStartup(L"On Startup", 0);
	radios = root->FindChild(L"On Startup");
	radios->Set(*onStartup);
}


//---------------------------------------------------------------
//
// CPrefDialog::DoPostCommand
//
//---------------------------------------------------------------
void CPrefDialog::DoPostCommand()
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	
	// Width
	XPreference<int32> defaultWidth(L"Default Width", 256);
	IConstInt32ValuePtr width = root->FindChild(L"Width");
	*defaultWidth = width->Get();

	// Height
	XPreference<int32> defaultHeight(L"Default Height", 256);
	IConstInt32ValuePtr height = root->FindChild(L"Height");
	*defaultHeight = height->Get();

	// Number of Colors
	XPreference<bool> useMillions(L"Use Millions of Colors", true);
	IConstInt32ValuePtr radios = root->FindChild(L"Number of Colors");
	*useMillions = radios->Get() == 1;

	// On Startup
	XPreference<int32> onStartup(L"On Startup", 0);
	radios = root->FindChild(L"On Startup");
	*onStartup = radios->Get();
}

