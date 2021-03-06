/*
 *  File:       CRandomPaletteDialog.cpp
 *  Summary:   	Dialog used set the parameters used by the random palette command.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CRandomPaletteDialog.cpp,v $
 *		Revision 1.2  2001/04/03 01:04:16  jesjones
 *		Pose methods now take a const reference (makes it easier to passs in a temporary).
 *		
 *		Revision 1.1  2001/03/30 04:08:59  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include "IDocDialog.h"

#include <IClickExtensions.h>
#include <IControl.h>
#include <IDialogBox.h>
#include <IPoseDialogBox.h>
#include <IRootControl.h>
#include <IText.h>
#include <ITextBox.h>
#include <IValue.h>
#include <IValueControl.h>
#include <IWindow.h>
#include <XBind.h>
#include <XPreference.h>

#include "IDocInfo.h"
#include "IRandomPalette.h"


// ===================================================================================
//	class CRandomPaletteDialog
//!		Dialog used set the parameters used by the random palette command.
// ===================================================================================
class CRandomPaletteDialog : public IDocDialog {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CRandomPaletteDialog();
	
						CRandomPaletteDialog(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		Pose(const IDocInfoPtr& doc);
						
//-----------------------------------
//	Internal API
//
private:
			void 		DoSetUp();
			void 		DoPostCommand();
			void 		DoDefault();
};

DEFINE_INTERFACE_FACTORY(CRandomPaletteDialog)

//---------------------------------------------------------------
//
// CRandomPaletteDialog::~CRandomPaletteDialog
//
//---------------------------------------------------------------
CRandomPaletteDialog::~CRandomPaletteDialog()
{
}


//---------------------------------------------------------------
//
// CRandomPaletteDialog::CRandomPaletteDialog
//
//---------------------------------------------------------------
CRandomPaletteDialog::CRandomPaletteDialog(XBoss* boss)
{	
	IDocDialog::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CRandomPaletteDialog::Pose
//
//---------------------------------------------------------------
bool CRandomPaletteDialog::Pose(const IDocInfoPtr& doc)
{	
	UNUSED(doc);
	
	// Initialize and open the dialog
	this->DoSetUp();
	
	IPoseDialogBoxPtr handler(L"Pose Dialog Box");	
	handler->Open(this);
	
	// Select the first location textbox
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	ITextBoxPtr textbox = root->FindChild(L"Number of Colors");
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
// CRandomPaletteDialog::DoDefault
//
//---------------------------------------------------------------
void CRandomPaletteDialog::DoDefault()
{
	IDocDialog* thisPtr = this;
	IWindowPtr window(thisPtr);
	IRootControlPtr root = window->GetRootControl();
	
	// Number of Colors
	IInt32ValuePtr textbox = root->FindChild(L"Number of Colors");
	textbox->Set(numeric_cast<int32>(kDefaultNumColors));

	// Complexity
	textbox = root->FindChild(L"Complexity");
	textbox->Set(kDefaultComplexity);

	// Saturation
	IDoubleValuePtr textbox2 = root->FindChild(L"Saturation");
	textbox2->Set(kDefaultSaturation);

	// Value
	textbox2 = root->FindChild(L"Value");
	textbox2->Set(kDefaultValue);
}


//---------------------------------------------------------------
//
// CRandomPaletteDialog::DoSetUp
//
//---------------------------------------------------------------
void CRandomPaletteDialog::DoSetUp()
{
	IDocDialog* thisPtr = this;
	IWindowPtr window(thisPtr);
	IRootControlPtr root = window->GetRootControl();
	
	XPreference<uint32>	numColors(L"Number of Colors", kDefaultNumColors);
	XPreference<int32>  complexity(L"Color Complexity", kDefaultComplexity);
	XPreference<double>	saturation(L"Color Saturation", kDefaultSaturation);
	XPreference<double>	value(L"Color Value", kDefaultValue);

	// Number of Colors
	IInt32ValuePtr textbox = root->FindChild(L"Number of Colors");
	textbox->Set(numeric_cast<int32>(*numColors), kDontRedraw);

	// Complexity
	textbox = root->FindChild(L"Complexity");
	textbox->Set(*complexity, kDontRedraw);

	// Saturation
	IDoubleValuePtr textbox2 = root->FindChild(L"Saturation");
	textbox2->Set(*saturation, kDontRedraw);

	// Value
	textbox2 = root->FindChild(L"Value");
	textbox2->Set(*value, kDontRedraw);

	// Callbacks
	XCallback0<void> callback(this, &CRandomPaletteDialog::DoDefault);
	ICallbackClickExtensionPtr extension = root->FindChild(L"Default");
	extension->Init(callback, 1);	
}


//---------------------------------------------------------------
//
// CRandomPaletteDialog::DoPostCommand
//
//---------------------------------------------------------------
void CRandomPaletteDialog::DoPostCommand()
{
	IDocDialog* thisPtr = this;
	IWindowPtr window(thisPtr);
	IRootControlPtr root = window->GetRootControl();
	
	XPreference<uint32>	numColors(L"Number of Colors", kDefaultNumColors);
	XPreference<int32>  complexity(L"Color Complexity", kDefaultComplexity);
	XPreference<double>	saturation(L"Color Saturation", kDefaultSaturation);
	XPreference<double>	value(L"Color Value", kDefaultValue);

	// Number of Colors
	IConstInt32ValuePtr textbox = root->FindChild(L"Number of Colors");
	*numColors = numeric_cast<uint32>(textbox->Get());

	// Complexity
	textbox = root->FindChild(L"Complexity");
	*complexity = textbox->Get();

	// Saturation
	IConstDoubleValuePtr textbox2 = root->FindChild(L"Saturation");
	*saturation = textbox2->Get();

	// Value
	textbox2 = root->FindChild(L"Value");
	*value = textbox2->Get();
}

