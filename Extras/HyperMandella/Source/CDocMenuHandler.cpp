/*
 *  File:       CDocMenuHandler.cpp
 *  Summary:   	The interface that handles menu commands directed to documents.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CDocMenuHandler.cpp,v $
 *		Revision 1.15  2001/04/05 03:59:19  jesjones
 *		Added duplicate menu command.
 *		
 *		Revision 1.14  2001/04/04 04:50:43  jesjones
 *		Added handlers for randomize constant, lambda, and w.
 *		
 *		Revision 1.13  2001/04/03 04:34:14  jesjones
 *		Added color formula dialogs.
 *		
 *		Revision 1.12  2001/04/03 01:02:35  jesjones
 *		Added a bunch of shader dialogs.
 *		
 *		Revision 1.11  2001/04/01 04:52:05  jesjones
 *		Added add palette and delete palette dialogs.
 *		
 *		Revision 1.10  2001/03/30 04:11:41  jesjones
 *		Added Color menu.
 *		
 *		Revision 1.9  2001/03/28 09:38:30  jesjones
 *		Added fractal formula menu.
 *		
 *		Revision 1.8  2001/03/27 06:26:36  jesjones
 *		Added camera dialog.
 *		
 *		Revision 1.7  2001/03/26 04:32:01  jesjones
 *		Added rotate, zoom, and pan commands.
 *		
 *		Revision 1.6  2001/03/25 06:06:02  jesjones
 *		Made the dialog code a bit more reuseable by introducing IDocCommands, IFloatDialog, and IComplexDialog.
 *		
 *		Revision 1.5  2001/03/23 10:52:45  jesjones
 *		Added W dialog.
 *		
 *		Revision 1.4  2001/02/28 09:52:05  jesjones
 *		Added a bailout dialog.
 *		
 *		Revision 1.3  2001/02/12 09:33:18  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.2  2001/02/09 04:29:33  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.1  2000/12/28 02:49:12  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.4  2000/12/05 09:04:07  jesjones
 *		Added code to handle the point light dialog menu commands.
 *		
 *		Revision 1.3  2000/11/12 07:53:21  jesjones
 *		Renamed the callback adaptor functions Bind. Removed the Procedure callback adaptors.
 *		
 *		Revision 1.2  2000/11/09 08:54:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include <IInstallMenuHandler.h>

#include <IApplication.h>
#include <ICommand.h>
#include <ICommandQueue.h>
#include <IDocument.h>
#include <IDocuments.h>
#include <IDocView.h>
#include <IHierarchy.h>
#include <IKeyHandler.h>
#include <IMenuHandler.h>
#include <IRange.h>
#include <IReadXML.h>
#include <IUndoActions.h>
#include <XBind.h>
#include <XFloatConversions.h>
#include <XIntConversions.h>
#include <XKeyEvents.h>
#include <XPreference.h>
#include <XURI.h>

#include "IColorFormulas.h"
#include "IComplexDialog.h"
#include "IDocCommands.h"
#include "IDocDialog.h"
#include "IDocInfo.h"
#include "IFloatDialog.h"
#include "IFormulaState.h"
#include "IFractalFunction.h"
#include "IInt32Dialog.h"
#include "IPalettes.h"
#include "IPointLightDialog.h"
#include "IRandomPalette.h"
#include "IShader.h"
#include "ITemporaryPalettes.h"


// ===================================================================================
//	class CDocMenuHandler
//!		The interface that handles menu commands directed to documents.
// ===================================================================================
class CDocMenuHandler : public IInstallMenuHandler {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~CDocMenuHandler();
	
						CDocMenuHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		InstallCommands();
	virtual void 		RemoveCommands();
						
//-----------------------------------
//	Internal API
//
private:
			void 		DoAddPalette();
			void 		DoChangePalette(const XColorTable& colors, const std::wstring& name, const std::wstring& text);
			void 		DoSetPalette();
			void 		DoRandomPalette();
			void 		DoRandomDialog();
			void 		DoAddTempPalette();
			void 		DoTempPalette(uint32 index);

			void 		DoBailoutDialog();
			void 		DoConstantDialog();
			void 		DoLambdaDialog();
			void 		DoMaxDwellDialog();
			void 		DoPointLightDialog(uint32 index);	
			void 		DoResolutionDialog();
			void 		DoWDialog();
			void 		DoRandomizeConstant();
			void 		DoRandomizeLambda();
			void 		DoRandomizeW();
			
			void 		DoRotateBy();
			void 		DoZoomBy();
			void 		DoPanBy();
			void 		DoCameraDialog();
			
			void 		DoFractalFormula();
			void 		DoMakeDefault();

			void 		DoLambertShader();
			void 		DoPhongShader();
			void 		DoAmbientCoefficient();
			void 		DoDiffuseCoefficient();
			void 		DoSpecularCoefficient();
			void 		DoSpecularExponent();
			void 		DoEditColorFormula();
			void 		DoColorFormula();
			void 		DoAddColorFormula();
			void 		DoDuplicate();

			SCommandStatus DoEnableSetPalette();
			SCommandStatus DoEnableAddPalette();
			
			SCommandStatus DoEnableRotateBy();
			SCommandStatus DoEnableZoomBy();
			SCommandStatus DoEnablePanBy();

			SCommandStatus DoEnableFractalFormula();
			SCommandStatus DoEnableConstant();
			SCommandStatus DoEnableLambda();
			SCommandStatus DoEnableAddTempPalette();
			SCommandStatus DoEnableClearTempPalette();
			SCommandStatus DoEnablePhongFeatures();
			SCommandStatus DoEnableLambertShader();
			SCommandStatus DoEnablePhongShader();
			SCommandStatus DoEnableColorFormula();
			SCommandStatus DoEnableAddColorFormula();
			
//-----------------------------------
//	Member Data
//
private:	
	float		mMinWRand;
	float		mMaxWRand;

	float		mMinConstantRand;
	float		mMaxConstantRand;
	
	float		mMinLambdaRand;
	float		mMaxLambdaRand;
	
	IDocInfo*	mDoc;
};

DEFINE_INTERFACE_FACTORY(CDocMenuHandler)

//---------------------------------------------------------------
//
// CDocMenuHandler::~CDocMenuHandler
//
//---------------------------------------------------------------
CDocMenuHandler::~CDocMenuHandler()
{
}


//---------------------------------------------------------------
//
// CDocMenuHandler::CDocMenuHandler
//
//---------------------------------------------------------------
CDocMenuHandler::CDocMenuHandler(XBoss* boss)
{	
	IInstallMenuHandler::DoSetBoss(boss);
	
	mMinWRand = -2.0f;		// $$ save this with the document?
	mMaxWRand = 2.0f;
	
	mMinConstantRand = -0.5f;
	mMaxConstantRand = 0.5f;
	
	mMinLambdaRand = -0.5f;
	mMaxLambdaRand = 0.5f;
	
	mDoc = nil;
}


//---------------------------------------------------------------
//
// CDocMenuHandler::InstallCommands
//
//---------------------------------------------------------------
void CDocMenuHandler::InstallCommands()
{
	IMenuHandlerPtr handler(L"Application");	
	
	// Cache the IDocInfo pointer
	if (mDoc == nil) {
		IDocViewPtr view(this);
		IDocInfoPtr doc(view->GetDoc());
		
		mDoc = doc.Get();
	}
			
	// Color Formula:
	XCallback0<void> action(this, &CDocMenuHandler::DoColorFormula);
	XCallback0<SCommandStatus> enabler(this, &CDocMenuHandler::DoEnableColorFormula);
	handler->RegisterCommand(L"Color Formula: ", action, enabler, this, true);
	
	// Fractal Formula:
	action.Set(this, &CDocMenuHandler::DoFractalFormula);
	enabler.Set(this, &CDocMenuHandler::DoEnableFractalFormula);
	handler->RegisterCommand(L"Fractal Formula: ", action, enabler, this, true);
	
	// Palette:
	action.Set(this, &CDocMenuHandler::DoSetPalette);
	enabler.Set(this, &CDocMenuHandler::DoEnableSetPalette);
	handler->RegisterCommand(L"Palette: ", action, enabler, this, true);
	
	// Pan By:
	action.Set(this, &CDocMenuHandler::DoPanBy);
	enabler.Set(this, &CDocMenuHandler::DoEnablePanBy);
	handler->RegisterCommand(L"Pan By: ", action, enabler, this, true);
	
	// Rotate By:
	action.Set(this, &CDocMenuHandler::DoRotateBy);
	enabler.Set(this, &CDocMenuHandler::DoEnableRotateBy);
	handler->RegisterCommand(L"Rotate By: ", action, enabler, this, true);
	
	// Zoom By:
	action.Set(this, &CDocMenuHandler::DoZoomBy);
	enabler.Set(this, &CDocMenuHandler::DoEnableZoomBy);
	handler->RegisterCommand(L"Zoom By: ", action, enabler, this, true);	
	
	// Add Color Formula
	action.Set(this, &CDocMenuHandler::DoAddColorFormula);
	enabler.Set(this, &CDocMenuHandler::DoEnableAddColorFormula);
	handler->RegisterCommand(L"Add Color Formula", action, enabler, this);
	
	// Ambient Coefficient
	action.Set(this, &CDocMenuHandler::DoAmbientCoefficient);
	handler->RegisterCommand(L"Ambient Coefficient", action, kEnabledIfDocWindow, this);
	
	// Add Palette
	action.Set(this, &CDocMenuHandler::DoAddPalette);
	enabler.Set(this, &CDocMenuHandler::DoEnableAddPalette);
	handler->RegisterCommand(L"Add Palette", action, enabler, this);
	
	// Add To Temp
	action.Set(this, &CDocMenuHandler::DoAddTempPalette);
	enabler.Set(this, &CDocMenuHandler::DoEnableAddTempPalette);
	handler->RegisterCommand(L"Add To Temp", action, enabler, this);
	
	// Bailout
	action.Set(this, &CDocMenuHandler::DoBailoutDialog);
	handler->RegisterCommand(L"Bailout", action, kEnabledIfDocWindow, this);
	
	// Camera Settings
	action.Set(this, &CDocMenuHandler::DoCameraDialog);
	handler->RegisterCommand(L"Camera Settings", action, kEnabledIfDocWindow, this);
	
	// Clear Temp
	ITemporaryPalettesPtr temporary(L"Application");
	XCallback0<void> callback5(temporary.Get(), &ITemporaryPalettes::Clear);
	enabler.Set(this, &CDocMenuHandler::DoEnableClearTempPalette);
	handler->RegisterCommand(L"Clear Temp", callback5, enabler, this);
	
	// Constant
	action.Set(this, &CDocMenuHandler::DoConstantDialog);
	enabler.Set(this, &CDocMenuHandler::DoEnableConstant);
	handler->RegisterCommand(L"Constant", action, enabler, this);
	
	// Diffuse Coefficient
	action.Set(this, &CDocMenuHandler::DoDiffuseCoefficient);
	handler->RegisterCommand(L"Diffuse Coefficient", action, kEnabledIfDocWindow, this);
	
	// Duplicate
	action.Set(this, &CDocMenuHandler::DoDuplicate);
	handler->RegisterCommand(L"Duplicate", action, kDisabledIfLowMemory+kEnabledIfDocWindow, this);
	
	// Edit Color Formula
	action.Set(this, &CDocMenuHandler::DoEditColorFormula);
	handler->RegisterCommand(L"Edit Color Formula", action, kEnabledIfDocWindow, this);
	
	// Lambert Shader
	action.Set(this, &CDocMenuHandler::DoLambertShader);
	enabler.Set(this, &CDocMenuHandler::DoEnableLambertShader);
	handler->RegisterCommand(L"Lambert Shader", action, enabler, this);
	
	// Lambda	
	action.Set(this, &CDocMenuHandler::DoLambdaDialog);
	enabler.Set(this, &CDocMenuHandler::DoEnableLambda);
	handler->RegisterCommand(L"Lambda", action, enabler, this);	
	
	// Make Default
#if DEBUG
	action.Set(this, &CDocMenuHandler::DoMakeDefault);
	handler->RegisterCommand(L"Make Default", action, kEnabledIfDocWindow, this);
#endif

	// Max Dwell
	action.Set(this, &CDocMenuHandler::DoMaxDwellDialog);
	handler->RegisterCommand(L"Max Dwell", action, kEnabledIfDocWindow, this);
	
	// Pan Left
	IDocCommandsPtr commands(mDoc);
	XCallback2<void, int32, int32> callback4(commands.Get(), &IDocCommands::Pan);
	handler->RegisterCommand(L"Pan Left", Bind2(callback4, -1, 0), kEnabledIfDocWindow, this);
	
	// Pan Right
	handler->RegisterCommand(L"Pan Right", Bind2(callback4, 1, 0), kEnabledIfDocWindow, this);
	
	// Pan Up
	handler->RegisterCommand(L"Pan Up", Bind2(callback4, 0, 1), kEnabledIfDocWindow, this);
	
	// Pan Down
	handler->RegisterCommand(L"Pan Down", Bind2(callback4, 0, -1), kEnabledIfDocWindow, this);
		
	// Phong Shader
	action.Set(this, &CDocMenuHandler::DoPhongShader);
	enabler.Set(this, &CDocMenuHandler::DoEnablePhongShader);
	handler->RegisterCommand(L"Phong Shader", action, enabler, this);
	
	// Point Light #1
	XCallback1<void, uint32> callback(this, &CDocMenuHandler::DoPointLightDialog);
	action = Bind1(callback, 0);
	handler->RegisterCommand(L"Point Light #1", action, kEnabledIfDocWindow, this);
	
	// Point Light #2
	action = Bind1(callback, 1);
	handler->RegisterCommand(L"Point Light #2", action, kEnabledIfDocWindow, this);

	// Point Light #3
	action = Bind1(callback, 2);
	handler->RegisterCommand(L"Point Light #3", action, kEnabledIfDocWindow, this);
	
	// Point Light #4
	action = Bind1(callback, 3);
	handler->RegisterCommand(L"Point Light #4", action, kEnabledIfDocWindow, this);

	// Random Palette
	action.Set(this, &CDocMenuHandler::DoRandomPalette);
	handler->RegisterCommand(L"Random Palette", action, kEnabledIfDocWindow, this);
	
	// Random Palette Options
	action.Set(this, &CDocMenuHandler::DoRandomDialog);
	handler->RegisterCommand(L"Random Options", action, kEnabledIfDocWindow, this);
	
	// Randomize Constant
	action.Set(this, &CDocMenuHandler::DoRandomizeConstant);
	handler->RegisterCommand(L"Randomize Constant", action, kEnabledIfDocWindow, this);	// context menu only so don't need a custom enabler
	
	// Randomize Lambda
	action.Set(this, &CDocMenuHandler::DoRandomizeLambda);
	handler->RegisterCommand(L"Randomize Lambda", action, kEnabledIfDocWindow, this);	// context menu only so don't need a custom enabler
	
	// Randomize W
	action.Set(this, &CDocMenuHandler::DoRandomizeW);
	handler->RegisterCommand(L"Randomize W", action, kEnabledIfDocWindow, this);
	
	// Rotate Left
	XCallback1<void, int32> callback2(commands.Get(), &IDocCommands::OrbitY);
	handler->RegisterCommand(L"Rotate Left", Bind1(callback2, -1), kEnabledIfDocWindow, this);
	
	// Rotate Right
	handler->RegisterCommand(L"Rotate Right", Bind1(callback2, 1), kEnabledIfDocWindow, this);
	
	// Rotate Up
	callback2.Set(commands.Get(), &IDocCommands::OrbitX);
	handler->RegisterCommand(L"Rotate Up", Bind1(callback2, -1), kEnabledIfDocWindow, this);
	
	// Rotate Down
	handler->RegisterCommand(L"Rotate Down", Bind1(callback2, 1), kEnabledIfDocWindow, this);
	
	// Rotate CW
	callback2.Set(commands.Get(), &IDocCommands::OrbitZ);
	handler->RegisterCommand(L"Rotate CW", Bind1(callback2, 1), kEnabledIfDocWindow, this);
	
	// Rotate CCW
	handler->RegisterCommand(L"Rotate CCW", Bind1(callback2, -1), kEnabledIfDocWindow, this);
	
	// Resolution
	action.Set(this, &CDocMenuHandler::DoResolutionDialog);
	handler->RegisterCommand(L"Resolution", action, kEnabledIfDocWindow, this);
	
	// Specular Coefficient	
	action.Set(this, &CDocMenuHandler::DoSpecularCoefficient);
	enabler.Set(this, &CDocMenuHandler::DoEnablePhongFeatures);
	handler->RegisterCommand(L"Specular Coefficient", action, enabler, this);
		
	// Specular Exponent
	action.Set(this, &CDocMenuHandler::DoSpecularExponent);
	handler->RegisterCommand(L"Specular Exponent", action, enabler, this);
	
	// W
	action.Set(this, &CDocMenuHandler::DoWDialog);
	handler->RegisterCommand(L"W", action, kEnabledIfDocWindow, this);

	// Zoom In
	XCallback1<void, bool> callback3(commands.Get(), &IDocCommands::Zoom);
	handler->RegisterCommand(L"Zoom In", Bind1(callback3, false), kEnabledIfDocWindow, this);
	
	// Zoom Out
	handler->RegisterCommand(L"Zoom Out", Bind1(callback3, true), kEnabledIfDocWindow, this);
	
	// Key Handlers
	IKeyHandlerPtr handler2(L"Application");
	XCallback1<void, uint32> action2(this, &CDocMenuHandler::DoTempPalette);
	handler2->RegisterHandler(k1Key, Bind1(action2, 0), this);	// can't use a loop because the key codes aren't contiguous on Macs...
	handler2->RegisterHandler(k2Key, Bind1(action2, 1), this);
	handler2->RegisterHandler(k3Key, Bind1(action2, 2), this);
	handler2->RegisterHandler(k4Key, Bind1(action2, 3), this);
	handler2->RegisterHandler(k5Key, Bind1(action2, 4), this);
	handler2->RegisterHandler(k6Key, Bind1(action2, 5), this);
	handler2->RegisterHandler(k7Key, Bind1(action2, 6), this);
	handler2->RegisterHandler(k8Key, Bind1(action2, 7), this);
	handler2->RegisterHandler(k9Key, Bind1(action2, 8), this);
}
		

//---------------------------------------------------------------
//
// CDocMenuHandler::RemoveCommands
//
//---------------------------------------------------------------
void CDocMenuHandler::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");
			
	// Color Formula:
	XCallback0<void> action(this, &CDocMenuHandler::DoColorFormula);
	handler->UnRegisterCommand(action);	
	
	// Fractal Formula:
	action.Set(this, &CDocMenuHandler::DoFractalFormula);
	handler->UnRegisterCommand(action);	
	
	// Palette:
	action.Set(this, &CDocMenuHandler::DoSetPalette);
	handler->UnRegisterCommand(action);	
	
	// Pan By:
	action.Set(this, &CDocMenuHandler::DoPanBy);
	handler->UnRegisterCommand(action);
	
	// Rotate By:
	action.Set(this, &CDocMenuHandler::DoRotateBy);
	handler->UnRegisterCommand(action);
	
	// Zoom By:
	action.Set(this, &CDocMenuHandler::DoZoomBy);
	handler->UnRegisterCommand(action);
	
	// Add Color Formula
	action.Set(this, &CDocMenuHandler::DoAddColorFormula);
	handler->UnRegisterCommand(action);	
	
	// Ambient Coefficient
	action.Set(this, &CDocMenuHandler::DoAmbientCoefficient);
	handler->UnRegisterCommand(action);	
	
	// Add Palette
	action.Set(this, &CDocMenuHandler::DoAddPalette);
	handler->UnRegisterCommand(action);	
	
	// Add To Temp
	action.Set(this, &CDocMenuHandler::DoAddTempPalette);
	handler->UnRegisterCommand(action);	
	
	// Bailout
	action.Set(this, &CDocMenuHandler::DoBailoutDialog);
	handler->UnRegisterCommand(action);	
	
	// Camera Settings
	action.Set(this, &CDocMenuHandler::DoCameraDialog);
	handler->UnRegisterCommand(action);
	
	// Clear Temp
	ITemporaryPalettesPtr temporary(L"Application");
	XCallback0<void> callback5(temporary.Get(), &ITemporaryPalettes::Clear);
	handler->UnRegisterCommand(callback5);
	
	// Constant
	action.Set(this, &CDocMenuHandler::DoConstantDialog);
	handler->UnRegisterCommand(action);	
	
	// Diffuse Coefficient
	action.Set(this, &CDocMenuHandler::DoDiffuseCoefficient);
	handler->UnRegisterCommand(action);	
	
	// Duplicate
	action.Set(this, &CDocMenuHandler::DoDuplicate);
	handler->UnRegisterCommand(action);	
	
	// Edit Color Formula
	action.Set(this, &CDocMenuHandler::DoEditColorFormula);
	handler->UnRegisterCommand(action);	
	
	// Lambert Shader
	action.Set(this, &CDocMenuHandler::DoLambertShader);
	handler->UnRegisterCommand(action);	
	
	// Lambda	
	action.Set(this, &CDocMenuHandler::DoLambdaDialog);
	handler->UnRegisterCommand(action);	
	
	// Make Default
#if DEBUG
	action.Set(this, &CDocMenuHandler::DoMakeDefault);
	handler->UnRegisterCommand(action);	
#endif

	// Max Dwell
	action.Set(this, &CDocMenuHandler::DoMaxDwellDialog);
	handler->UnRegisterCommand(action);	
	
	// Pan Left
	IDocCommandsPtr commands(mDoc);
	XCallback2<void, int32, int32> callback4(commands.Get(), &IDocCommands::Pan);
	handler->UnRegisterCommand(Bind2(callback4, -1, 0));
	
	// Pan Right
	handler->UnRegisterCommand(Bind2(callback4, 1, 0));
	
	// Pan Up
	handler->UnRegisterCommand(Bind2(callback4, 0, 1));
	
	// Pan Down
	handler->UnRegisterCommand(Bind2(callback4, 0, -1));
		
	// Phong Shader
	action.Set(this, &CDocMenuHandler::DoPhongShader);
	handler->UnRegisterCommand(action);	
	
	// Point Light #1
	XCallback1<void, uint32> callback(this, &CDocMenuHandler::DoPointLightDialog);
	action = Bind1(callback, 0);
	handler->UnRegisterCommand(action);	
	
	// Point Light #2
	action = Bind1(callback, 1);
	handler->UnRegisterCommand(action);	
	
	// Point Light #3
	action = Bind1(callback, 2);
	handler->UnRegisterCommand(action);	
	
	// Point Light #4
	action = Bind1(callback, 3);
	handler->UnRegisterCommand(action);	
	
	// Random Palette
	action.Set(this, &CDocMenuHandler::DoRandomPalette);
	handler->UnRegisterCommand(action);	
	
	// Random Palette Options
	action.Set(this, &CDocMenuHandler::DoRandomDialog);
	handler->UnRegisterCommand(action);	
	
	// Randomize Constant
	action.Set(this, &CDocMenuHandler::DoRandomizeConstant);
	handler->UnRegisterCommand(action);	
	
	// Randomize Lambda
	action.Set(this, &CDocMenuHandler::DoRandomizeLambda);
	handler->UnRegisterCommand(action);	
	
	// Randomize W
	action.Set(this, &CDocMenuHandler::DoRandomizeW);
	handler->UnRegisterCommand(action);	
	
	// Resolution
	action.Set(this, &CDocMenuHandler::DoResolutionDialog);
	handler->UnRegisterCommand(action);	
	
	// Rotate Left
	XCallback1<void, int32> callback2(commands.Get(), &IDocCommands::OrbitY);
	handler->UnRegisterCommand(Bind1(callback2, -1));
	
	// Rotate Right
	handler->UnRegisterCommand(Bind1(callback2, 1));
	
	// Rotate Up
	callback2.Set(commands.Get(), &IDocCommands::OrbitX);
	handler->UnRegisterCommand(Bind1(callback2, -1));
	
	// Rotate Down
	handler->UnRegisterCommand(Bind1(callback2, 1));
	
	// Rotate CW
	callback2.Set(commands.Get(), &IDocCommands::OrbitZ);
	handler->UnRegisterCommand(Bind1(callback2, 1));
	
	// Rotate CCW
	handler->UnRegisterCommand(Bind1(callback2, -1));
	
	// Specular Coefficient
	action.Set(this, &CDocMenuHandler::DoSpecularCoefficient);
	handler->UnRegisterCommand(action);	
		
	// Specular Exponent
	action.Set(this, &CDocMenuHandler::DoSpecularExponent);
	handler->UnRegisterCommand(action);	
	
	// W
	action.Set(this, &CDocMenuHandler::DoWDialog);
	handler->UnRegisterCommand(action);	

	// Zoom In
	XCallback1<void, bool> callback3(commands.Get(), &IDocCommands::Zoom);
	handler->UnRegisterCommand(Bind1(callback3, false));
	
	// Zoom Out
	handler->UnRegisterCommand(Bind1(callback3, true));
	
	// Key Handlers
	IKeyHandlerPtr handler2(L"Application");
	XCallback1<void, uint32> action2(this, &CDocMenuHandler::DoTempPalette);
	handler2->UnRegisterHandler(Bind1(action2, 0));
	handler2->UnRegisterHandler(Bind1(action2, 1));
	handler2->UnRegisterHandler(Bind1(action2, 2));
	handler2->UnRegisterHandler(Bind1(action2, 3));
	handler2->UnRegisterHandler(Bind1(action2, 4));
	handler2->UnRegisterHandler(Bind1(action2, 5));
	handler2->UnRegisterHandler(Bind1(action2, 6));
	handler2->UnRegisterHandler(Bind1(action2, 7));
	handler2->UnRegisterHandler(Bind1(action2, 8));
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CDocMenuHandler::DoAddColorFormula
//
//---------------------------------------------------------------
void CDocMenuHandler::DoAddColorFormula()
{
	const std::wstring& formula = mDoc->GetShaderInfo().colorFormula;
	
	IColorFormulasPtr formulas(L"Application");
	formulas->Add(formula);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoAddPalette
//
//---------------------------------------------------------------
void CDocMenuHandler::DoAddPalette()
{
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IDocDialogPtr dialog = persist->CreateBoss(uri, L"Add_Palette_Dialog", app);
		
	dialog->Pose(mDoc);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoAddTempPalette
//
//---------------------------------------------------------------
void CDocMenuHandler::DoAddTempPalette()
{
	ITemporaryPalettesPtr temporary(L"Application");

	temporary->Add(mDoc->GetPalette(), mDoc->GetPaletteName());
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoAmbientCoefficient
//
//---------------------------------------------------------------
void CDocMenuHandler::DoAmbientCoefficient()
{
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IFloatDialogPtr dialog = persist->CreateBoss(uri, L"Ambient_Coefficient_Dialog", app);
		
	IDocCommandsPtr commands(mDoc);
	XCallback1<void, float> callback(commands.Get(), &IDocCommands::ChangeAmbientCoefficient);
	dialog->Pose(mDoc->GetShaderInfo().ambientCoeff, callback);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoBailoutDialog
//
//---------------------------------------------------------------
void CDocMenuHandler::DoBailoutDialog()
{	
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IFloatDialogPtr dialog = persist->CreateBoss(uri, L"Bailout_Dialog", app);
		
	IDocCommandsPtr commands(mDoc);
	XCallback1<void, float> callback(commands.Get(), &IDocCommands::ChangeBailout);
	dialog->Pose(mDoc->GetFractalInfo().bailout, callback);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoCameraDialog
//
//---------------------------------------------------------------
void CDocMenuHandler::DoCameraDialog()
{
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IDocDialogPtr dialog = persist->CreateBoss(uri, L"Camera_Dialog", app);
	
	dialog->Pose(mDoc);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoChangePalette
//
//---------------------------------------------------------------
void CDocMenuHandler::DoChangePalette(const XColorTable& colors, const std::wstring& name, const std::wstring& text)
{
	// Build the callbacks that set and restore the palette
	XCallback3<void, XColorTable, std::wstring, bool> method(mDoc, &IDocInfo::SetPalette);
 	XCallback0<void> doIt = Bind3(method, colors, name, true);
 	XCallback0<void> undoIt = Bind3(method, mDoc->GetPalette(), mDoc->GetPaletteName(), true);

	// Initialize the undo action
	IUndoActionPtr action(L"Callback Action");
	action->Init(text);
	
	IUndoCallbackActionPtr undoer(action);
	undoer->SetCallbacks(doIt, undoIt);
	
	// Put the action on the command queue
	ICommandQueuePtr queue(L"Application");
	queue->Post(action);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoColorFormula
//
//---------------------------------------------------------------
void CDocMenuHandler::DoColorFormula()
{
	IMenuHandlerPtr handler(L"Application");
	std::wstring str = handler->GetCommand().substr(std::wcslen(L"Color Formula: "));
	
	IDocCommandsPtr commands(mDoc);
	commands->ChangeColorFormula(str);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoConstantDialog
//
//---------------------------------------------------------------
void CDocMenuHandler::DoConstantDialog()
{	
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IComplexDialogPtr dialog = persist->CreateBoss(uri, L"Constant_Dialog", app);
	
	IFloatRangePtr range(dialog);
	range->SetMin(mMinConstantRand);
	range->SetMax(mMaxConstantRand);
		
	IDocCommandsPtr commands(mDoc);
	XCallback1<void, CHyperComplex> callback(commands.Get(), &IDocCommands::ChangeConstant);
	if (dialog->Pose(mDoc->GetFractalInfo().constant, callback)) {
		mMinConstantRand = range->GetMin();
		mMaxConstantRand = range->GetMax();
	}
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoDiffuseCoefficient
//
//---------------------------------------------------------------
void CDocMenuHandler::DoDiffuseCoefficient()
{
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IFloatDialogPtr dialog = persist->CreateBoss(uri, L"Diffuse_Coefficient_Dialog", app);
		
	IDocCommandsPtr commands(mDoc);
	XCallback1<void, float> callback(commands.Get(), &IDocCommands::ChangeDiffuseCoefficient);
	dialog->Pose(mDoc->GetShaderInfo().diffuseCoeff, callback);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoDuplicate
//
//---------------------------------------------------------------
void CDocMenuHandler::DoDuplicate()
{
	IDocumentsPtr docs(L"Application");
	IDocumentPtr doc = docs->New();
	
	IDocInfoPtr(doc)->Reset(mDoc);
	doc->SetDirty();
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoEditColorFormula
//
//---------------------------------------------------------------
void CDocMenuHandler::DoEditColorFormula()
{
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IDocDialogPtr dialog = persist->CreateBoss(uri, L"Edit_Color_Formula_Dialog", app);
		
	dialog->Pose(mDoc);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoEnableAddColorFormula
//
//---------------------------------------------------------------
SCommandStatus CDocMenuHandler::DoEnableAddColorFormula()
{
	SCommandStatus status;

	const std::wstring& formula = mDoc->GetShaderInfo().colorFormula;

	IConstColorFormulasPtr formulas(L"Application");
	status.enabled = !formulas->Has(formula);

	return status;
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoEnableAddPalette
//
//---------------------------------------------------------------
SCommandStatus CDocMenuHandler::DoEnableAddPalette()
{
	SCommandStatus status;

	IPalettesPtr palettes(L"Application");
	const std::wstring& name = mDoc->GetPaletteName();

	status.enabled = !palettes->Has(name);

	return status;
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoEnableAddTempPalette
//
//---------------------------------------------------------------
SCommandStatus CDocMenuHandler::DoEnableAddTempPalette()
{
	SCommandStatus status;

	IConstTemporaryPalettesPtr temporary(L"Application");
	status.enabled = temporary->GetCount() < 9;

	return status;
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoEnableClearTempPalette
//
//---------------------------------------------------------------
SCommandStatus CDocMenuHandler::DoEnableClearTempPalette()
{
	SCommandStatus status;

	IConstTemporaryPalettesPtr temporary(L"Application");
	status.enabled = temporary->GetCount() > 0;

	return status;
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoEnableColorFormula
//
//---------------------------------------------------------------
SCommandStatus CDocMenuHandler::DoEnableColorFormula()
{
	SCommandStatus status;

	IMenuHandlerPtr handler(L"Application");
	const std::wstring& command = handler->GetCommand();
	
	std::wstring str = command.substr(strlen("Color Formula: "));
	std::wstring formula = mDoc->GetShaderInfo().colorFormula;
	
	status.checkmark = str == formula ? kEntireSelection+kIndependantCheckMark : kNoCheckmark;
	status.enabled = true;

	return status;
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoEnableConstant
//
//---------------------------------------------------------------
SCommandStatus CDocMenuHandler::DoEnableConstant()
{
	SCommandStatus status;

	IConstFractalFunctionPtr function = mDoc->GetFractalFunction();
	
	status.enabled = function->UsesConstant();

	return status;
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoEnableFractalFormula
//
//---------------------------------------------------------------
SCommandStatus CDocMenuHandler::DoEnableFractalFormula()
{
	SCommandStatus status;

	IConstFractalFunctionPtr function = mDoc->GetFractalFunction();

	IMenuHandlerPtr handler(L"Application");
	const std::wstring& command = handler->GetCommand();
	
	std::wstring str = command.substr(strlen("Fractal Formula: "));
	std::wstring formula = function->GetFormula();
	
	status.checkmark = str == formula ? kEntireSelection+kIndependantCheckMark : kNoCheckmark;
	status.enabled = true;

	return status;
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoEnableLambertShader
//
//---------------------------------------------------------------
SCommandStatus CDocMenuHandler::DoEnableLambertShader()
{
	SCommandStatus status;

	IConstShaderPtr shader = mDoc->GetShader();
	
	status.checkmark = shader->GetBoss()->GetName() == L"Lambert Shader" ? kEntireSelection+kIndependantCheckMark : kNoCheckmark;
	status.enabled = true;	

	return status;
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoEnableLambda
//
//---------------------------------------------------------------
SCommandStatus CDocMenuHandler::DoEnableLambda()
{
	SCommandStatus status;

	IConstFractalFunctionPtr function = mDoc->GetFractalFunction();
	
	status.enabled = function->UsesLambda();

	return status;
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoEnablePanBy
//
//---------------------------------------------------------------
SCommandStatus CDocMenuHandler::DoEnablePanBy()
{
	SCommandStatus status;

	IMenuHandlerPtr handler(L"Application");
	const std::wstring& command = handler->GetCommand();
	
	int32 value = StrToInt32(command.substr(strlen("Pan By: ")));
	XPreference<int32> panBy(L"Pan By", kDefaultPanBy);
	
	status.checkmark = value == *panBy ? kEntireSelection+kIndependantCheckMark : kNoCheckmark;
	status.enabled = true;

	return status;
}

			
//---------------------------------------------------------------
//
// CDocMenuHandler::DoEnablePhongFeatures
//
//---------------------------------------------------------------
SCommandStatus CDocMenuHandler::DoEnablePhongFeatures()
{
	SCommandStatus status;

	IConstShaderPtr shader = mDoc->GetShader();
	
	status.enabled = shader->GetBoss()->GetName() == L"Phong Shader";	// interfaces are the same for both shaders so this is the only way to distinguish them...

	return status;
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoEnablePhongShader
//
//---------------------------------------------------------------
SCommandStatus CDocMenuHandler::DoEnablePhongShader()
{
	SCommandStatus status;

	IConstShaderPtr shader = mDoc->GetShader();
	
	status.checkmark = shader->GetBoss()->GetName() == L"Phong Shader" ? kEntireSelection+kIndependantCheckMark : kNoCheckmark;
	status.enabled = true;	

	return status;
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoEnableRotateBy
//
//---------------------------------------------------------------
SCommandStatus CDocMenuHandler::DoEnableRotateBy()
{
	SCommandStatus status;

	IMenuHandlerPtr handler(L"Application");
	const std::wstring& command = handler->GetCommand();
	
	int32 value = StrToInt32(command.substr(strlen("Rotate By: ")));
	XPreference<int32> rotateBy(L"Rotate By", kDefaultRotateBy);
	
	status.checkmark = value == *rotateBy ? kEntireSelection+kIndependantCheckMark : kNoCheckmark;
	status.enabled = true;

	return status;
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoEnableSetPalette
//
//---------------------------------------------------------------
SCommandStatus CDocMenuHandler::DoEnableSetPalette()
{
	SCommandStatus status;

	const std::wstring& name = mDoc->GetPaletteName();
	
	IMenuHandlerPtr handler(L"Application");
	const std::wstring& command = handler->GetCommand();
	std::wstring name2 = handler->GetCommand().substr(std::wcslen(L"Palette: "));
	
	bool check = name == name2;
	status.checkmark = check ? kEntireSelection+kIndependantCheckMark : kNoCheckmark;
	status.enabled = true;

	return status;
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoEnableZoomBy
//
//---------------------------------------------------------------
SCommandStatus CDocMenuHandler::DoEnableZoomBy()
{
	SCommandStatus status;

	IMenuHandlerPtr handler(L"Application");
	const std::wstring& command = handler->GetCommand();
	
	double value = StrToDouble(command.substr(strlen("Zoom By: ")));
	XPreference<float> zoomBy(L"Zoom By", kDefaultZoomBy);
	
	status.checkmark = Equal(value, *zoomBy) ? kEntireSelection+kIndependantCheckMark : kNoCheckmark;
	status.enabled = true;

	return status;
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoFractalFormula
//
//---------------------------------------------------------------
void CDocMenuHandler::DoFractalFormula()
{
	IMenuHandlerPtr handler(L"Application");
	std::wstring str = handler->GetCommand().substr(std::wcslen(L"Fractal Formula: "));
	
	IDocCommandsPtr commands(mDoc);
	commands->ChangeFractalFormula(str);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoLambertShader
//
//---------------------------------------------------------------
void CDocMenuHandler::DoLambertShader()
{
	IDocCommandsPtr commands(mDoc);
	
	IShaderPtr shader(L"Lambert Shader");
	commands->ChangeShader(shader);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoLambdaDialog
//
//---------------------------------------------------------------
void CDocMenuHandler::DoLambdaDialog()
{	
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IComplexDialogPtr dialog = persist->CreateBoss(uri, L"Lambda_Dialog", app);
	
	IFloatRangePtr range(dialog);
	range->SetMin(mMinLambdaRand);
	range->SetMax(mMaxLambdaRand);
		
	IDocCommandsPtr commands(mDoc);
	XCallback1<void, CHyperComplex> callback(commands.Get(), &IDocCommands::ChangeLambda);
	if (dialog->Pose(mDoc->GetFractalInfo().lambda, callback)) {
		mMinLambdaRand = range->GetMin();
		mMaxLambdaRand = range->GetMax();
	}
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoMakeDefault
//
//---------------------------------------------------------------
void CDocMenuHandler::DoMakeDefault()
{	
	IFormulaStatePtr state(L"Application");
	state->MakeDefault(mDoc);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoMaxDwellDialog
//
//---------------------------------------------------------------
void CDocMenuHandler::DoMaxDwellDialog()
{	
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IInt32DialogPtr dialog = persist->CreateBoss(uri, L"Max_Dwell_Dialog", app);
	
	IDocCommandsPtr commands(mDoc);
	XCallback1<void, int32> callback(commands.Get(), &IDocCommands::ChangeMaxDwell);
	dialog->Pose(numeric_cast<int32>(mDoc->GetFractalInfo().maxDwell), callback);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoPanBy
//
//---------------------------------------------------------------
void CDocMenuHandler::DoPanBy()
{
	IMenuHandlerPtr handler(L"Application");
	std::wstring str = handler->GetCommand().substr(std::wcslen(L"Pan By: "));

	XPreference<int32> panBy(L"Pan By", kDefaultPanBy);
	*panBy = StrToInt32(str);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoPhongShader
//
//---------------------------------------------------------------
void CDocMenuHandler::DoPhongShader()
{
	IDocCommandsPtr commands(mDoc);
	
	IShaderPtr shader(L"Phong Shader");
	commands->ChangeShader(shader);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoPointLightDialog
//
//---------------------------------------------------------------
void CDocMenuHandler::DoPointLightDialog(uint32 index)
{	
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IPointLightDialogPtr dialog = persist->CreateBoss(uri, L"Point_Light_Dialog", app);
	
	dialog->Pose(mDoc, index);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoRandomDialog
//
//---------------------------------------------------------------
void CDocMenuHandler::DoRandomDialog()
{
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IDocDialogPtr dialog = persist->CreateBoss(uri, L"Random_Palette_Options_Dialog", app);
	
	dialog->Pose(mDoc);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoRandomizeConstant
//
//---------------------------------------------------------------
void CDocMenuHandler::DoRandomizeConstant()
{
	CHyperComplex value;
	value.x = Random(mMinConstantRand, mMaxConstantRand);
	value.y = Random(mMinConstantRand, mMaxConstantRand);
	value.z = Random(mMinConstantRand, mMaxConstantRand);
	value.w = Random(mMinConstantRand, mMaxConstantRand);
	
	IDocCommandsPtr commands(mDoc);
	commands->ChangeConstant(value);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoRandomizeLambda
//
//---------------------------------------------------------------
void CDocMenuHandler::DoRandomizeLambda()
{
	CHyperComplex value;
	value.x = Random(mMinLambdaRand, mMaxLambdaRand);
	value.y = Random(mMinLambdaRand, mMaxLambdaRand);
	value.z = Random(mMinLambdaRand, mMaxLambdaRand);
	value.w = Random(mMinLambdaRand, mMaxLambdaRand);
	
	IDocCommandsPtr commands(mDoc);
	commands->ChangeLambda(value);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoRandomizeW
//
//---------------------------------------------------------------
void CDocMenuHandler::DoRandomizeW()
{
	if (mMinWRand <= mMaxWRand) {
		float value = Random(mMinWRand, mMaxWRand);
		
		IDocCommandsPtr commands(mDoc);
		commands->ChangeW(value);
	}
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoRandomPalette
//
//---------------------------------------------------------------
void CDocMenuHandler::DoRandomPalette()
{
	IDocViewPtr view(this);
	IRandomPalettePtr random(L"Application");
	XColorTable colors = random->GetPalette();
	
	this->DoChangePalette(colors, L"????", LoadAppString(L"Random Palette"));
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoResolutionDialog
//
//---------------------------------------------------------------
void CDocMenuHandler::DoResolutionDialog()
{	
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IDocDialogPtr dialog = persist->CreateBoss(uri, L"Resolution_Dialog", app);
	
	dialog->Pose(mDoc);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoRotateBy
//
//---------------------------------------------------------------
void CDocMenuHandler::DoRotateBy()
{
	IMenuHandlerPtr handler(L"Application");
	std::wstring str = handler->GetCommand().substr(std::wcslen(L"Rotate By: "));

	XPreference<int32> rotateBy(L"Rotate By", kDefaultRotateBy);
	*rotateBy = StrToInt32(str);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoSetPalette
//
//---------------------------------------------------------------
void CDocMenuHandler::DoSetPalette()
{
	IMenuHandlerPtr handler(L"Application");
	std::wstring name = handler->GetCommand().substr(std::wcslen(L"Palette: "));

	IPalettesPtr palettes(L"Application");
	XColorTable colors = palettes->Get(name);
	
	this->DoChangePalette(colors, name, LoadAppString(L"Set Palette"));
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoSpecularCoefficient
//
//---------------------------------------------------------------
void CDocMenuHandler::DoSpecularCoefficient()
{
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IFloatDialogPtr dialog = persist->CreateBoss(uri, L"Specular_Coefficient_Dialog", app);
		
	IDocCommandsPtr commands(mDoc);
	XCallback1<void, float> callback(commands.Get(), &IDocCommands::ChangeSpecularCoefficient);
	dialog->Pose(mDoc->GetShaderInfo().specularCoeff, callback);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoSpecularExponent
//
//---------------------------------------------------------------
void CDocMenuHandler::DoSpecularExponent()
{
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IFloatDialogPtr dialog = persist->CreateBoss(uri, L"Specular_Exponent_Dialog", app);
		
	IDocCommandsPtr commands(mDoc);
	XCallback1<void, float> callback(commands.Get(), &IDocCommands::ChangeSpecularExponent);
	dialog->Pose(mDoc->GetShaderInfo().specularExp, callback);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoTempPalette
//
//---------------------------------------------------------------
void CDocMenuHandler::DoTempPalette(uint32 index)
{
#if MAC
	IKeyHandlerPtr handler(L"Application");
	const XKeyDownEvent& event = handler->GetEvent();
	if (event.WasCommandKeyDown()) 
#endif
	{
		IConstTemporaryPalettesPtr temporary(L"Application");
		if (index < temporary->GetCount()) {
			// Get the new palette
			XColorTable colors = temporary->GetPalette(index);
			std::wstring name = temporary->GetName(index);
			
			this->DoChangePalette(colors, name, LoadAppString(L"Temp Palette"));
		}
	}
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoWDialog
//
//---------------------------------------------------------------
void CDocMenuHandler::DoWDialog()
{	
	IApplicationPtr app(L"Application");

	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IFloatDialogPtr dialog = persist->CreateBoss(uri, L"W_Dialog", app);
	
	IFloatRangePtr range(dialog);
	range->SetMin(mMinWRand);
	range->SetMax(mMaxWRand);
	
	IDocCommandsPtr commands(mDoc);
	XCallback1<void, float> callback(commands.Get(), &IDocCommands::ChangeW);
	if (dialog->Pose(mDoc->GetFractalInfo().w, callback)) {
		mMinWRand = range->GetMin();
		mMaxWRand = range->GetMax();
	}
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoZoomBy
//
//---------------------------------------------------------------
void CDocMenuHandler::DoZoomBy()
{
	IMenuHandlerPtr handler(L"Application");
	std::wstring str = handler->GetCommand().substr(std::wcslen(L"Zoom By: "));

	XPreference<float> zoomBy(L"Zoom By", kDefaultZoomBy);
	*zoomBy = (float) StrToDouble(str);
}


