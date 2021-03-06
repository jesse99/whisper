/*
 *  File:       CFloatDialog.cpp
 *  Summary:   	Interface used to change float's on the document.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CFloatDialog.cpp,v $
 *		Revision 1.2  2001/03/26 04:30:15  jesjones
 *		Pose no longer takes a name.
 *		
 *		Revision 1.1  2001/03/25 06:04:02  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include "IFloatDialog.h"

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
//	class CFloatDialog
//!		Interface used to change float's on the document.
// ===================================================================================
class CFloatDialog : public IFloatDialog {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CFloatDialog();
	
						CFloatDialog(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		Pose(float value, XCallback1<void, float> callback);

//-----------------------------------
//	Internal API
//
private:
			void 		DoSetUp(float value);
			void 		DoPostCommand(XCallback1<void, float> callback);
			void 		DoRandom();
};

DEFINE_INTERFACE_FACTORY(CFloatDialog)

//---------------------------------------------------------------
//
// CFloatDialog::~CFloatDialog
//
//---------------------------------------------------------------
CFloatDialog::~CFloatDialog()
{
}


//---------------------------------------------------------------
//
// CFloatDialog::CFloatDialog
//
//---------------------------------------------------------------
CFloatDialog::CFloatDialog(XBoss* boss) 
{	
	IFloatDialog::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CFloatDialog::Pose
//
//---------------------------------------------------------------
bool CFloatDialog::Pose(float value, XCallback1<void, float> callback)
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
// CFloatDialog::DoRandom
//
//---------------------------------------------------------------
void CFloatDialog::DoRandom()
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	
	IConstFloatValuePtr minRandom = root->FindChild(L"Min Random");
	float min = minRandom->Get();
	
	IConstFloatValuePtr maxRandom = root->FindChild(L"Max Random");
	float max = maxRandom->Get();

	float value = 0.0f;
	if (min <= max) 
		value = (float) Random(min, max);
	
	IFloatValuePtr w = root->FindChild(L"Value");
	w->Set(value);
}


//---------------------------------------------------------------
//
// CFloatDialog::DoSetUp
//
//---------------------------------------------------------------
void CFloatDialog::DoSetUp(float value)
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	
	IFloatValuePtr textbox = root->FindChild(L"Value");
	textbox->Set(value, kDontRedraw);
	
	IConstFloatRangePtr range(this);
	if (range) {
		IFloatValuePtr minRandom = root->FindChild(L"Min Random");
		minRandom->Set(range->GetMin(), kDontRedraw);
		
		IFloatValuePtr maxRandom = root->FindChild(L"Max Random");
		maxRandom->Set(range->GetMax(), kDontRedraw);
		
		ICallbackClickExtensionPtr extension = root->FindChild(L"Random");
		extension->Init(XCallback0<void>(this, &CFloatDialog::DoRandom), 1);	
	}
}


//---------------------------------------------------------------
//
// CFloatDialog::DoPostCommand
//
//---------------------------------------------------------------
void CFloatDialog::DoPostCommand(XCallback1<void, float> callback)
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	
	// Change the value
	IConstFloatValuePtr textbox = root->FindChild(L"Value");
	float value = textbox->Get();

	callback(value);
	
	// Save the new min and max random values
	IFloatRangePtr range(this);
	if (range) {
		IConstFloatValuePtr minRandom = root->FindChild(L"Min Random");
		range->SetMin(minRandom->Get());
		
		IConstFloatValuePtr maxRandom = root->FindChild(L"Max Random");
		range->SetMax(maxRandom->Get());	
	}
}


