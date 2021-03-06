/*
 *  File:       CPointLightDialog.cpp
 *  Summary:   	Dialog used to edit IPointLight settings.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CPointLightDialog.cpp,v $
 *		Revision 1.5  2001/04/21 02:24:54  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/04/03 01:04:11  jesjones
 *		Pose methods now take a const reference (makes it easier to passs in a temporary).
 *		
 *		Revision 1.3  2001/03/27 06:27:29  jesjones
 *		Renamed a boss.
 *		
 *		Revision 1.2  2001/03/25 06:07:54  jesjones
 *		Added a comment.
 *		
 *		Revision 1.1  2000/12/28 02:50:36  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.6  2000/12/25 02:44:57  jesjones
 *		Shuffled some code around so that it works on Windows.
 *		
 *		Revision 1.5  2000/12/25 00:30:17  jesjones
 *		Added a location textboxs.
 *		
 *		Revision 1.4  2000/12/19 07:48:14  jesjones
 *		Added a brightness textbox.
 *		
 *		Revision 1.3  2000/12/17 03:44:51  jesjones
 *		Added support for color.
 *		
 *		Revision 1.2  2000/12/06 07:17:06  jesjones
 *		Added support for attenuation.
 *		
 *		Revision 1.1  2000/12/05 09:01:25  jesjones
 *		Checked in	
 */

#include "AppHeader.h"
#include "IPointLightDialog.h"

#include <IColorSwatch.h>
#include <ICommand.h>
#include <ICommandQueue.h>
#include <IDialogBox.h>
#include <IPoseDialogBox.h>
#include <IRootControl.h>
#include <ITextBox.h>
#include <IUndoActions.h>
#include <IValue.h>
#include <IValueControl.h>
#include <IWindow.h>
#include <XBind.h>

#include "IDocInfo.h"
#include "ILight.h"
#include "IPointLight.h"


// ===================================================================================
//	class CPointLightDialog
//!		Dialog used to edit IPointLight settings.
// ===================================================================================
class CPointLightDialog : public IPointLightDialog {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CPointLightDialog();
	
						CPointLightDialog(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Pose(const IDocInfoPtr& doc, uint32 index);
						
//-----------------------------------
//	Internal API
//
private:
			void 		DoSetUp(const IConstDocInfoPtr& doc, uint32 index);

			void 		DoPostCommand(const IDocInfoPtr& doc, uint32 index);

			ILightPtr 	DoCreateNewLight(const IConstLightPtr& oldLight);
};

DEFINE_INTERFACE_FACTORY(CPointLightDialog)

//---------------------------------------------------------------
//
// CPointLightDialog::~CPointLightDialog
//
//---------------------------------------------------------------
CPointLightDialog::~CPointLightDialog()
{
}


//---------------------------------------------------------------
//
// CPointLightDialog::CPointLightDialog
//
//---------------------------------------------------------------
CPointLightDialog::CPointLightDialog(XBoss* boss) 
{	
	IPointLightDialog::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CPointLightDialog::Pose
//
//---------------------------------------------------------------
void CPointLightDialog::Pose(const IDocInfoPtr& doc, uint32 index)	// $$$ move this into Whisper via a template interface class?
{
	PRECONDITION(index < doc->GetNumPointLights());
	
	// Initialize and open the dialog
	this->DoSetUp(doc, index);
	
	IPoseDialogBoxPtr handler(L"Pose Dialog Box");	
	handler->Open(this);
	
	// Select the first location textbox
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	ITextBoxPtr textbox = root->FindChild(L"Location.x");
	textbox->Select();

	// Let the dialog process events until the user presses the
	// OK or Cancel button.
	if (handler->Run()) 
		this->DoPostCommand(doc, index);
		
	handler->Close();
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CPointLightDialog::DoSetUp
//
//---------------------------------------------------------------
void CPointLightDialog::DoSetUp(const IConstDocInfoPtr& doc, uint32 index)
{
	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	
	// on/off checkbox
	IConstLightPtr light = doc->GetLight(index);
	IValueControlPtr control = root->FindChild(L"On");
	control->SetValue(light->IsOn(), kDontRedraw);

	// attenuation
	IConstPointLightPtr point(light);
	ELightAttenuation attenuation = point->GetAttenuation();

	IInt32ValuePtr radios = root->FindChild(L"Attenuation Group Box");
	radios->Set(attenuation);
	
	// color
	IColorSwatchPtr swatch = root->FindChild(L"Color");
	swatch->SetColor(light->GetColor(), kDontRedraw);
	
	// brightness
	IDoubleValuePtr brightness = root->FindChild(L"Brightness");
	brightness->Set(light->GetBrightness(), kDontRedraw);
		
	// location
	X3DPoint loc = point->GetLocation();
	IDoubleValuePtr location = root->FindChild(L"Location.x");
	location->Set(loc.x, kDontRedraw);
	
	location = root->FindChild(L"Location.y");
	location->Set(loc.y, kDontRedraw);
	
	location = root->FindChild(L"Location.z");
	location->Set(loc.z, kDontRedraw);
}


//---------------------------------------------------------------
//
// CPointLightDialog::DoCreateNewLight
//
//---------------------------------------------------------------
ILightPtr CPointLightDialog::DoCreateNewLight(const IConstLightPtr& oldLight)
{
	ILightPtr newLight;

	IWindowPtr window(this);
	IRootControlPtr root = window->GetRootControl();
	
	// on/off
	IConstValueControlPtr control = root->FindChild(L"On");		
	bool on = control->GetValue() != 0;
	if (on != oldLight->IsOn()) {
		if (!newLight)
			newLight = oldLight->Clone();
			
		newLight->TurnOn(on);
	}
			
	// attenuation
	IConstInt32ValuePtr radios = root->FindChild(L"Attenuation Group Box");
	ELightAttenuation attenuation = static_cast<ELightAttenuation>(radios->Get());

	IConstPointLightPtr oldPoint(oldLight);
	if (attenuation != oldPoint->GetAttenuation()) {
		if (!newLight)
			newLight = oldLight->Clone();
			
		IPointLightPtr newPoint(newLight);
		newPoint->SeAttenuation(attenuation);
	}
	
	// color
	IConstColorSwatchPtr swatch = root->FindChild(L"Color");
	XRGBColor color = swatch->GetColor();
	if (color != oldLight->GetColor()) {
		if (!newLight)
			newLight = oldLight->Clone();
			
		newLight->SetColor(color);
	}
	
	// brightness
	IConstDoubleValuePtr brightness = root->FindChild(L"Brightness");
	double value = brightness->Get();
	if (!Equal(value, oldLight->GetBrightness())) {
		if (!newLight)
			newLight = oldLight->Clone();
			
		newLight->SetBrightness((float) value);
	}
	
	// location
	X3DPoint loc;
	IConstDoubleValuePtr location = root->FindChild(L"Location.x");
	loc.x = (float) location->Get();
	
	location = root->FindChild(L"Location.y");
	loc.y = (float) location->Get();

	location = root->FindChild(L"Location.z");
	loc.z = (float) location->Get();

	if (loc != oldPoint->GetLocation()) {
		if (!newLight)
			newLight = oldLight->Clone();
			
		IPointLightPtr newPoint(newLight);
		newPoint->SetLocation(loc);
	}
	
	return newLight;
}


//---------------------------------------------------------------
//
// CPointLightDialog::DoPostCommand
//
//---------------------------------------------------------------
void CPointLightDialog::DoPostCommand(const IDocInfoPtr& doc, uint32 index)
{
	// Get the old light and create the new light (if there were changes)
	IConstLightPtr oldLight = doc->GetLight(index);
	ILightPtr newLight = this->DoCreateNewLight(oldLight);
	if (newLight) {
	
		// Create an undo action
		IUndoActionPtr action(L"Callback Action");
		action->Init(LoadAppString(L"Change Light"));
		
		// Setup the callbacks
		XCallback3<void, uint32, ILightPtr, bool> callback(doc.Get(), &IDocInfo::SetLight);
		XCallback0<void> doIt = Bind3(callback, index, newLight, true);
		XCallback0<void> undoIt = Bind3(callback, index, oldLight->Clone(), true);

		IUndoCallbackActionPtr undo(action);
		undo->SetCallbacks(doIt, undoIt);

		// Schedule the action
		ICommandQueuePtr queue(L"Application");
		queue->Post(action);
	}
}

