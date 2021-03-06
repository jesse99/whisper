/*
 *  File:       CCameraDialog.cpp
 *  Summary:   	Dialog used to change the camera position and orientation.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CCameraDialog.cpp,v $
 *		Revision 1.2  2001/04/03 01:04:03  jesjones
 *		Pose methods now take a const reference (makes it easier to passs in a temporary).
 *		
 *		Revision 1.1  2001/03/27 06:24:11  jesjones
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
#include <IUndoActions.h>
#include <IValidate.h>
#include <IValue.h>
#include <IValueControl.h>
#include <IWindow.h>
#include <XBind.h>
#include <XDialogUtils.h>
#include <XPreference.h>

#include "ICamera.h"
#include "IDocCommands.h"
#include "IDocInfo.h"
#include "IOrthographicCamera.h"


// ===================================================================================
//	class CCameraDialog
//!		Dialog used to change the camera position and orientation.
// ===================================================================================
class CCameraDialog : public IDocDialog, public IValidate {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CCameraDialog();
	
						CCameraDialog(XBoss* boss);
						
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

DEFINE_INTERFACE_FACTORY(CCameraDialog)

//---------------------------------------------------------------
//
// CCameraDialog::~CCameraDialog
//
//---------------------------------------------------------------
CCameraDialog::~CCameraDialog()
{
}


//---------------------------------------------------------------
//
// CCameraDialog::CCameraDialog
//
//---------------------------------------------------------------
CCameraDialog::CCameraDialog(XBoss* boss)
{	
	IDocDialog::DoSetBoss(boss);
	IValidate::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CCameraDialog::Pose
//
//---------------------------------------------------------------
bool CCameraDialog::Pose(const IDocInfoPtr& doc)
{	
	// Initialize and open the dialog
	this->DoSetUp(doc);
	
	IPoseDialogBoxPtr handler(L"Pose Dialog Box");	
	IDocDialog* thisPtr = this;
	handler->Open(thisPtr);
	
	// Select the first textbox
	IWindowPtr window(thisPtr);
	IRootControlPtr root = window->GetRootControl();
	ITextBoxPtr textbox = root->FindChild(L"Location.x");
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
// CCameraDialog::Validate
//
//---------------------------------------------------------------
bool CCameraDialog::Validate() const
{
	bool valid = true;
	
	const IDocDialog* thisPtr = this;
	IConstWindowPtr window(thisPtr);
	IConstRootControlPtr root = window->GetRootControl();

	IConstFloatValuePtr textbox = root->FindChild(L"Hither");
	float hither = textbox->Get();

	textbox = root->FindChild(L"Yon");
	float yon = textbox->Get();

	if (hither >= yon) {
		Whisper::DoStop(LoadAppString(L"Hither must be less than yon!"), L"");
		
		ITextBoxPtr box = root->FindChild(L"Hither");
		box->Select();

		valid = false;
	}
			
	return valid;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CCameraDialog::DoSetUp
//
//---------------------------------------------------------------
void CCameraDialog::DoSetUp(const IConstDocInfoPtr& doc)
{
	IDocDialog* thisPtr = this;
	IWindowPtr window(thisPtr);
	IRootControlPtr root = window->GetRootControl();
	
	IConstCameraPtr camera(doc);
	const SCameraPlacement& placement = camera->GetPlacement();
	const SCameraRange& range = camera->GetRange();

	IFloatValuePtr textbox = root->FindChild(L"Location.x");
	textbox->Set(placement.location.x, kDontRedraw);
	
	textbox = root->FindChild(L"Location.y");
	textbox->Set(placement.location.y, kDontRedraw);

	textbox = root->FindChild(L"Location.z");
	textbox->Set(placement.location.z, kDontRedraw);

	textbox = root->FindChild(L"Interest.x");
	textbox->Set(placement.pointOfInterest.x, kDontRedraw);
	
	textbox = root->FindChild(L"Interest.y");
	textbox->Set(placement.pointOfInterest.y, kDontRedraw);

	textbox = root->FindChild(L"Interest.z");
	textbox->Set(placement.pointOfInterest.z, kDontRedraw);

	textbox = root->FindChild(L"Up.x");
	textbox->Set(placement.upVector.x, kDontRedraw);
	
	textbox = root->FindChild(L"Up.y");
	textbox->Set(placement.upVector.y, kDontRedraw);

	textbox = root->FindChild(L"Up.z");
	textbox->Set(placement.upVector.z, kDontRedraw);

	textbox = root->FindChild(L"Hither");
	textbox->Set(range.hither, kDontRedraw);

	textbox = root->FindChild(L"Yon");
	textbox->Set(range.yon, kDontRedraw);
	
	IConstOrthographicCameraPtr orthographic(camera);
	if (orthographic) {
		X2DSize size = orthographic->GetViewSize();

		textbox = root->FindChild(L"Width");
		textbox->Set(size.width, kDontRedraw);

		textbox = root->FindChild(L"Height");
		textbox->Set(size.height, kDontRedraw);
	
	} else {
		IControlPtr control = root->FindChild(L"Width");
		control->Enable(false, kDontRedraw);

		control = root->FindChild(L"Height");
		control->Enable(false, kDontRedraw);
	}
}


//---------------------------------------------------------------
//
// CCameraDialog::DoPostCommand
//
//---------------------------------------------------------------
void CCameraDialog::DoPostCommand(const IDocInfoPtr& doc)
{
	IDocDialog* thisPtr = this;
	IWindowPtr window(thisPtr);
	IRootControlPtr root = window->GetRootControl();
	
	ICameraPtr camera(doc);
	SCameraPlacement placement;
	SCameraRange range;

	IConstFloatValuePtr textbox = root->FindChild(L"Location.x");
	placement.location.x = textbox->Get();
	
	textbox = root->FindChild(L"Location.y");
	placement.location.y = textbox->Get();

	textbox = root->FindChild(L"Location.z");
	placement.location.z = textbox->Get();

	textbox = root->FindChild(L"Interest.x");
	placement.pointOfInterest.x = textbox->Get();
	
	textbox = root->FindChild(L"Interest.y");
	placement.pointOfInterest.y = textbox->Get();

	textbox = root->FindChild(L"Interest.z");
	placement.pointOfInterest.z = textbox->Get();

	textbox = root->FindChild(L"Up.x");
	placement.upVector.x = textbox->Get();
	
	textbox = root->FindChild(L"Up.y");
	placement.upVector.y = textbox->Get();

	textbox = root->FindChild(L"Up.z");
	placement.upVector.z = textbox->Get();

	textbox = root->FindChild(L"Hither");
	range.hither = textbox->Get();

	textbox = root->FindChild(L"Yon");
	range.yon = textbox->Get();
	
	SUndoSequence sequence(LoadAppString(L"Camera Settings"));

	IDocCommandsPtr commands(doc);
	IOrthographicCameraPtr orthographic(camera);
	if (orthographic) {
		X2DSize size;

		textbox = root->FindChild(L"Width");
		size.width = textbox->Get();

		textbox = root->FindChild(L"Height");
		size.height = textbox->Get();

		commands->SetViewSize(size);
	}
	
	commands->SetPlacement(placement);
	commands->SetRange(range);
}

