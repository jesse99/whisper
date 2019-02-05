/*
 *  File:       CComplexDialog.cpp
 *  Summary:   	Interface used to change CHyperComplex's on the document.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CComplexDialog.cpp,v $
 *		Revision 1.2  2001/03/26 04:30:12  jesjones
 *		Pose no longer takes a name.
 *		
 *		Revision 1.1  2001/03/25 06:03:53  jesjones
 *		Checked in	
 */

#include "AppHeader.h"
#include "IComplexDialog.h"

#include <IClickExtensions.h>
#include <IDialogBox.h>
#include <IPoseDialogBox.h>
#include <IRange.h>
#include <IRootControl.h>
#include <ITextBox.h>
#include <IValue.h>
#include <IWindow.h>

#include "IDocCommands.h"
#include "IDocInfo.h"


// ===================================================================================
//	class CComplexDialog
//!		Interface used to change CHyperComplex's on the document.
// ===================================================================================
class CComplexDialog : public IComplexDialog {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CComplexDialog();
	
						CComplexDialog(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		Pose(const CHyperComplex& value, XCallback1<void, CHyperComplex> callback);

//-----------------------------------
//	Internal API
//
private:
			void 		DoSetUp(const CHyperComplex& value);
			void 		DoPostCommand(XCallback1<void, CHyperComplex> callback);
			void 		DoRandom();
			void 		DoSetRandom(double min, double max, const std::wstring& name);
};

DEFINE_INTERFACE_FACTORY(CComplexDialog)

//---------------------------------------------------------------
//
// CComplexDialog::~CComplexDialog
//
//---------------------------------------------------------------
CComplexDialog::~CComplexDialog()
{
}


//---------------------------------------------------------------
//
// CComplexDialog::CComplexDialog
//
//---------------------------------------------------------------
CComplexDialog::CComplexDialog(XBoss* boss) 
{	
	IComplexDialog::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CComplexDialog::Pose
//
//---------------------------------------------------------------
bool CComplexDialog::Pose(const CHyperComplex& value, XCallback1<void, CHyperComplex> callback)
{	
	// Initialize and open the dialog
	this->DoSetUp(value);
		
	IPoseDialogBoxPtr handler(L"Pose Dialog Box");	
	handler->Open(this);
	
	// Select the textbox
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	ITextBoxPtr textbox = root->FindChild(L"Value.x");
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
// CComplexDialog::DoRandom
//
//---------------------------------------------------------------
void CComplexDialog::DoRandom()
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	
	IConstFloatValuePtr minRandom = root->FindChild(L"Min Random");
	float min = minRandom->Get();
	
	IConstFloatValuePtr maxRandom = root->FindChild(L"Max Random");
	float max = maxRandom->Get();

	this->DoSetRandom(min, max, L"Value.x");
	this->DoSetRandom(min, max, L"Value.y");
	this->DoSetRandom(min, max, L"Value.z");
	this->DoSetRandom(min, max, L"Value.w");
}


//---------------------------------------------------------------
//
// CComplexDialog::DoSetRandom
//
//---------------------------------------------------------------
void CComplexDialog::DoSetRandom(double min, double max, const std::wstring& name)
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();

	float value = 0.0f;
	if (min <= max) 
		value = (float) Random(min, max);
	
	IFloatValuePtr w = root->FindChild(name);
	w->Set(value);
}


//---------------------------------------------------------------
//
// CComplexDialog::DoSetUp
//
//---------------------------------------------------------------
void CComplexDialog::DoSetUp(const CHyperComplex& value)
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	IConstFloatRangePtr range(this);
	
	IFloatValuePtr x = root->FindChild(L"Value.x");
	x->Set(value.x, kDontRedraw);
	
	IFloatValuePtr y = root->FindChild(L"Value.y");
	y->Set(value.y, kDontRedraw);
	
	IFloatValuePtr z = root->FindChild(L"Value.z");
	z->Set(value.z, kDontRedraw);
	
	IFloatValuePtr w = root->FindChild(L"Value.w");
	w->Set(value.w, kDontRedraw);
	
	IFloatValuePtr minRandom = root->FindChild(L"Min Random");
	minRandom->Set(range->GetMin(), kDontRedraw);
	
	IFloatValuePtr maxRandom = root->FindChild(L"Max Random");
	maxRandom->Set(range->GetMax(), kDontRedraw);
	
	ICallbackClickExtensionPtr extension = root->FindChild(L"Random");
	extension->Init(XCallback0<void>(this, &CComplexDialog::DoRandom), 1);	
}


//---------------------------------------------------------------
//
// CComplexDialog::DoPostCommand
//
//---------------------------------------------------------------
void CComplexDialog::DoPostCommand(XCallback1<void, CHyperComplex> callback)
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	
	// Change the constant
	CHyperComplex value;
	IConstFloatValuePtr x = root->FindChild(L"Value.x");
	value.x = x->Get();

	IConstFloatValuePtr y = root->FindChild(L"Value.y");
	value.y = y->Get();

	IConstFloatValuePtr z = root->FindChild(L"Value.z");
	value.z = z->Get();

	IConstFloatValuePtr w = root->FindChild(L"Value.w");
	value.w = w->Get();

	callback(value);
	
	// Save the new min and max random values
	IFloatRangePtr range(this);

	IConstFloatValuePtr minRandom = root->FindChild(L"Min Random");
	range->SetMin(minRandom->Get());
	
	IConstFloatValuePtr maxRandom = root->FindChild(L"Max Random");
	range->SetMax(maxRandom->Get());	
}


