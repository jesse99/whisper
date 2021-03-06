/*
 *  File:       CResolutionDialog.cpp
 *  Summary:   	Dialog used to edit the number of pixels of a fractal.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CResolutionDialog.cpp,v $
 *		Revision 1.3  2001/04/03 01:04:23  jesjones
 *		Pose methods now take a const reference (makes it easier to passs in a temporary).
 *		
 *		Revision 1.2  2001/03/27 06:28:28  jesjones
 *		Descends from IDocDialog instead of IResolutionDialog.
 *		
 *		Revision 1.1  2001/03/26 04:04:09  jesjones
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

#include "IDocCommands.h"
#include "IDocInfo.h"


//-----------------------------------
//	Constants
//
const int32 kSameRatio = 0;
const int32 kSameArea  = 1;
const int32 kFreeForm  = 2;


// ===================================================================================
//	class CResolutionDialog
//!		Dialog used to edit the number of pixels of a fractal.
// ===================================================================================
class CResolutionDialog : public IDocDialog {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CResolutionDialog();
	
						CResolutionDialog(XBoss* boss);
						
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
			
			void 		DoAdjustSize(const std::wstring& name, int32 style);
			void 		DoTextChanged(ITextBox* textbox);
						
//-----------------------------------
//	Member Data
//
private:
	XPreference<int32>	mStyle;
	XSize				mOldSize;
};

DEFINE_INTERFACE_FACTORY(CResolutionDialog)

//---------------------------------------------------------------
//
// CResolutionDialog::~CResolutionDialog
//
//---------------------------------------------------------------
CResolutionDialog::~CResolutionDialog()
{
}


//---------------------------------------------------------------
//
// CResolutionDialog::CResolutionDialog
//
//---------------------------------------------------------------
CResolutionDialog::CResolutionDialog(XBoss* boss) : mStyle(L"Resolution Style", 0)
{	
	IDocDialog::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CResolutionDialog::Pose
//
//---------------------------------------------------------------
bool CResolutionDialog::Pose(const IDocInfoPtr& doc)
{	
	// Initialize and open the dialog
	this->DoSetUp(doc);
	
	IPoseDialogBoxPtr handler(L"Pose Dialog Box");	
	handler->Open(this);
	
	// Select the first location textbox
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	ITextBoxPtr textbox = root->FindChild(L"Width");
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
// CResolutionDialog::DoAdjustSize
//
//---------------------------------------------------------------
void CResolutionDialog::DoAdjustSize(const std::wstring& name, int32 style)
{
	if (style != kFreeForm) {
		IWindowPtr window(this);
		IRootControlPtr root = window->GetRootControl();
		ITextPtr widthText = root->FindChild(L"Width");
		ITextPtr heightText = root->FindChild(L"Height");

		IInt32ValuePtr widthValue(widthText);
		IInt32ValuePtr heightValue(heightText);

		long x, y;	
		if (name == L"Height") {
				y = Max(heightValue->Get(), 1L);
				if (style == kSameArea)
					x = mOldSize.width*mOldSize.height/y;
				else if (style == kSameRatio) 
					x = (y*mOldSize.width)/mOldSize.height;
				widthValue->Set(x);
						
		} else if (name == L"Width") {
			if (!widthText->IsEmpty()) {
				x = Max(widthValue->Get(), 1L);
				if (style == kSameArea)
					y = mOldSize.width*mOldSize.height/x;
				else if (style == kSameRatio) 
					y = (x*mOldSize.height)/mOldSize.width;
				heightValue->Set(y);
			}
						
		} else
			DEBUGSTR("Expected 'Width' or 'Height', not '", name, "'.");
	}
	
	*mStyle = style;			// do this so the style is persistent
}


//---------------------------------------------------------------
//
// CResolutionDialog::DoTextChanged
//
//---------------------------------------------------------------
void CResolutionDialog::DoTextChanged(ITextBox* textbox)
{	
	IControlPtr control(textbox);
	this->DoAdjustSize(control->GetName(), *mStyle);
}


//---------------------------------------------------------------
//
// CResolutionDialog::DoSetUp
//
//---------------------------------------------------------------
void CResolutionDialog::DoSetUp(const IConstDocInfoPtr& doc)
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	
	const SResolution& resolution = doc->GetResolution();
	mOldSize = XSize(resolution.width, resolution.height);
	
	// Width
	IInt32ValuePtr textbox = root->FindChild(L"Width");
	textbox->Set(resolution.width, kDontRedraw);

	// Height
	textbox = root->FindChild(L"Height");
	textbox->Set(resolution.height, kDontRedraw);

	// Depth
	textbox = root->FindChild(L"Depth");
	textbox->Set(resolution.depth, kDontRedraw);

	// Style
	IInt32ValuePtr radios = root->FindChild(L"Style Group");
	radios->Set(*mStyle);

	// Callbacks
	XCallback2<void, std::wstring, int32> callback(this, &CResolutionDialog::DoAdjustSize);
	ICallbackClickExtensionPtr extension = root->FindChild(L"Same Ratio");
	extension->Init(Bind2(callback, L"Width", kSameRatio), 1);	

	extension = root->FindChild(L"Same Area");
	extension->Init(Bind2(callback, L"Width", kSameArea), 1);	

	extension = root->FindChild(L"Free Form");
	extension->Init(Bind2(callback, L"Width", kFreeForm), 1);	

	ITextBoxPtr box = root->FindChild(L"Width");
	box->SetChangedCallback(ITextBox::ChangedCallback(this, &CResolutionDialog::DoTextChanged));

	box = root->FindChild(L"Height");
	box->SetChangedCallback(ITextBox::ChangedCallback(this, &CResolutionDialog::DoTextChanged));
}


//---------------------------------------------------------------
//
// CResolutionDialog::DoPostCommand
//
//---------------------------------------------------------------
void CResolutionDialog::DoPostCommand(const IDocInfoPtr& doc)
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	SResolution resolution;
	
	IConstInt32ValuePtr textbox = root->FindChild(L"Width");
	resolution.width = textbox->Get();

	textbox = root->FindChild(L"Height");
	resolution.height = textbox->Get();

	textbox = root->FindChild(L"Depth");
	resolution.depth = textbox->Get();

	IDocCommandsPtr commands(doc);
	commands->ChangeResolution(resolution);
}

