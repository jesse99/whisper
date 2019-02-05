/*
 *  File:       CDocCommands.cpp
 *  Summary:   	Implementation that changes the document's state via undoable commands.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CDocCommands.cpp,v $
 *		Revision 1.7  2001/04/21 02:23:53  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/04/03 04:36:26  jesjones
 *		Added ChangeColorFormula.
 *		
 *		Revision 1.5  2001/04/03 01:04:48  jesjones
 *		Added a bunch of shader related methods.
 *		
 *		Revision 1.4  2001/03/28 09:36:41  jesjones
 *		Added ChangeFormula.
 *		
 *		Revision 1.3  2001/03/27 06:26:10  jesjones
 *		Added camera commands.
 *		
 *		Revision 1.2  2001/03/26 04:33:17  jesjones
 *		Added rotate, zoom, and pan commands.
 *		
 *		Revision 1.1  2001/03/25 06:03:58  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include "IDocCommands.h"

#include <ICommand.h>
#include <ICommandQueue.h>
#include <IUndoActions.h>
#include <XBind.h>
#include <XPreference.h>
#include <XStringUtils.h>

#include "ICamera.h"
#include "IDocInfo.h"
#include "IFormulaState.h"
#include "IFractalFunction.h"
#include "IOrthographicCamera.h"
#include "IShader.h"


//-----------------------------------
//	Constants
//
const bool kDirtyDoc = true;


// ===================================================================================
//	class CDocCommands
//!		Implementation that changes the document's state via undoable commands.
// ===================================================================================
class CDocCommands : public IDocCommands {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CDocCommands();
	
						CDocCommands(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		ChangeBailout(float value);
	virtual void 		ChangeW(float value);
	virtual void 		ChangeConstant(const CHyperComplex& value);
	virtual void 		ChangeLambda(const CHyperComplex& value);
	virtual void 		ChangeMaxDwell(int32 value);
	virtual void 		ChangeFractalInfo(const SFractalInfo& newInfo);
	virtual void 		ChangeFractalFormula(const std::wstring& formula);

	virtual void 		ChangeResolution(const SResolution& value);	

	virtual void 		OrbitX(int32 units);
	virtual void 		OrbitY(int32 units);
	virtual void 		OrbitZ(int32 units);
	virtual void 		Zoom(bool zoomOut);
	virtual void 		Pan(int32 horzUnits, int32 vertUnits);
	
	virtual void 		SetPlacement(const SCameraPlacement& value);
	virtual void 		SetRange(const SCameraRange& value);
	virtual void 		SetViewSize(const X2DSize& value);

	virtual void 		ChangeAmbientCoefficient(float value);
	virtual void 		ChangeDiffuseCoefficient(float value);
	virtual void 		ChangeSpecularCoefficient(float value);
	virtual void 		ChangeSpecularExponent(float value);
	virtual void 		ChangeShader(const IShaderPtr& shader);
	virtual void 		ChangeColorFormula(const std::wstring& formula);

//-----------------------------------
//	Internal API
//
private:
			void 		DoFractalInfo(const SFractalInfo& newInfo, const std::wstring& message);
			void 		DoShaderInfo(const SShaderInfo& newInfo, const std::wstring& message);
};

DEFINE_INTERFACE_FACTORY(CDocCommands)

//---------------------------------------------------------------
//
// CDocCommands::~CDocCommands
//
//---------------------------------------------------------------
CDocCommands::~CDocCommands()
{
}


//---------------------------------------------------------------
//
// CDocCommands::CDocCommands
//
//---------------------------------------------------------------
CDocCommands::CDocCommands(XBoss* boss) 
{	
	IDocCommands::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CDocCommands::ChangeAmbientCoefficient
//
//---------------------------------------------------------------
void CDocCommands::ChangeAmbientCoefficient(float value)
{
	IDocInfoPtr doc(this);
	
	const SShaderInfo& oldInfo = doc->GetShaderInfo();
	if (!Whisper::Equal(value, oldInfo.ambientCoeff)) {
	
		SShaderInfo newInfo = oldInfo;
		newInfo.ambientCoeff = value;
		
		this->DoShaderInfo(newInfo, LoadAppString(L"Change Ambient Coefficient"));
	}
}


//---------------------------------------------------------------
//
// CDocCommands::ChangeBailout
//
//---------------------------------------------------------------
void CDocCommands::ChangeBailout(float value)
{
	IDocInfoPtr doc(this);
	
	const SFractalInfo& oldInfo = doc->GetFractalInfo();
	if (!Whisper::Equal(value, oldInfo.bailout)) {
	
		SFractalInfo newInfo = oldInfo;
		newInfo.bailout = value;
		
		this->DoFractalInfo(newInfo, LoadAppString(L"Change Bailout"));
	}
}


//---------------------------------------------------------------
//
// CDocCommands::ChangeColorFormula
//
//---------------------------------------------------------------
void CDocCommands::ChangeColorFormula(const std::wstring& formula)
{
	IDocInfoPtr doc(this);
	
	const SShaderInfo& oldInfo = doc->GetShaderInfo();

	if (formula != oldInfo.colorFormula) {
		SShaderInfo newInfo = oldInfo;
		newInfo.colorFormula = formula;
	
		// Create an undo action,
		IUndoActionPtr action(L"Callback Action");
		action->Init(LoadAppString(L"Change Color Formula"));
		
		// setup the callbacks,
		XCallback2<void, SShaderInfo, bool> callback(doc.Get(), &IDocInfo::SetShaderInfo);
		XCallback0<void> doIt = Bind2(callback, newInfo, kDirtyDoc);
		XCallback0<void> undoIt = Bind2(callback, oldInfo, kDirtyDoc);

		IUndoCallbackActionPtr undo(action);
		undo->SetCallbacks(doIt, undoIt);

		// and schedule the action.
		ICommandQueuePtr queue(L"Application");
		queue->Post(action);
	}
}


//---------------------------------------------------------------
//
// CDocCommands::ChangeConstant
//
//---------------------------------------------------------------
void CDocCommands::ChangeConstant(const CHyperComplex& value)
{
	IDocInfoPtr doc(this);
	
	const SFractalInfo& oldInfo = doc->GetFractalInfo();
	if (value != oldInfo.constant) {
	
		SFractalInfo newInfo = oldInfo;
		newInfo.constant = value;
		
		this->DoFractalInfo(newInfo, LoadAppString(L"Change Constant"));
	}
}


//---------------------------------------------------------------
//
// CDocCommands::ChangeDiffuseCoefficient
//
//---------------------------------------------------------------
void CDocCommands::ChangeDiffuseCoefficient(float value)
{
	IDocInfoPtr doc(this);
	
	const SShaderInfo& oldInfo = doc->GetShaderInfo();
	if (!Whisper::Equal(value, oldInfo.diffuseCoeff)) {
	
		SShaderInfo newInfo = oldInfo;
		newInfo.diffuseCoeff = value;
		
		this->DoShaderInfo(newInfo, LoadAppString(L"Change Diffuse Coefficient"));
	}
}


//---------------------------------------------------------------
//
// CDocCommands::ChangeFractalFormula
//
//---------------------------------------------------------------
void CDocCommands::ChangeFractalFormula(const std::wstring& formula)
{
	IDocInfoPtr doc(this);
	
	IConstFractalFunctionPtr function = doc->GetFractalFunction();
	std::wstring oldFormula = function->GetFormula();

	if (formula != oldFormula) {
		SUndoSequence sequence(LoadAppString(L"Change Fractal Formula"));
	
		// Create an undo action,
		IUndoActionPtr action(L"Callback Action");
		action->Init(L"");
		
		// setup the callbacks,
		void (IDocInfo::*ptr)(const std::wstring&, bool) = &IDocInfo::SetFractalFunction;	// SetFractalFunction is overloaded so we have to help the compiler out
		XCallback2<void, std::wstring, bool> callback(doc.Get(), ptr);
		XCallback0<void> doIt = Bind2(callback, formula, kDirtyDoc);
		XCallback0<void> undoIt = Bind2(callback, oldFormula, kDirtyDoc);

		IUndoCallbackActionPtr undo(action);
		undo->SetCallbacks(doIt, undoIt);

		// and schedule the action.
		ICommandQueuePtr queue(L"Application");
		queue->Post(action);

		// Change the fractal's state to something reasonable for the
		// new formula (this will fire off one or more undoable commands).
		IFormulaStatePtr state(L"Application");
		state->ChangeState(doc, formula);
	}
}


//---------------------------------------------------------------
//
// CDocCommands::ChangeFractalInfo
//
//---------------------------------------------------------------
void CDocCommands::ChangeFractalInfo(const SFractalInfo& newInfo)
{
	IDocInfoPtr doc(this);
	
	const SFractalInfo& oldInfo = doc->GetFractalInfo();
	if (newInfo != oldInfo) 
		this->DoFractalInfo(newInfo, LoadAppString(L"Change Fractal Info"));
}


//---------------------------------------------------------------
//
// CDocCommands::ChangeLambda
//
//---------------------------------------------------------------
void CDocCommands::ChangeLambda(const CHyperComplex& value)
{
	IDocInfoPtr doc(this);
	
	const SFractalInfo& oldInfo = doc->GetFractalInfo();
	if (value != oldInfo.lambda) {
	
		SFractalInfo newInfo = oldInfo;
		newInfo.lambda = value;
		
		this->DoFractalInfo(newInfo, LoadAppString(L"Change Lambda"));
	}
}


//---------------------------------------------------------------
//
// CDocCommands::ChangeMaxDwell
//
//---------------------------------------------------------------
void CDocCommands::ChangeMaxDwell(int32 value)
{
	IDocInfoPtr doc(this);
	
	const SFractalInfo& oldInfo = doc->GetFractalInfo();
	if (value != oldInfo.maxDwell) {
	
		SFractalInfo newInfo = oldInfo;
		newInfo.maxDwell = numeric_cast<uint32>(value);
		
		this->DoFractalInfo(newInfo, LoadAppString(L"Change Max Dwell"));
	}
}


//---------------------------------------------------------------
//
// CDocCommands::ChangeResolution
//
//---------------------------------------------------------------
void CDocCommands::ChangeResolution(const SResolution& value)
{
	IDocInfoPtr doc(this);
	
	const SResolution& oldValue = doc->GetResolution();
	if (value != oldValue) {
	
		// Create an undo action,
		IUndoActionPtr action(L"Callback Action");
		action->Init(LoadAppString(L"Change Resolution"));
		
		// setup the callbacks,
		XCallback2<void, SResolution, bool> callback(doc.Get(), &IDocInfo::SetResolution);
		XCallback0<void> doIt = Bind2(callback, value, kDirtyDoc);
		XCallback0<void> undoIt = Bind2(callback, oldValue, kDirtyDoc);

		IUndoCallbackActionPtr undo(action);
		undo->SetCallbacks(doIt, undoIt);

		// and schedule the action.
		ICommandQueuePtr queue(L"Application");
		queue->Post(action);
	}
}


//---------------------------------------------------------------
//
// CDocCommands::ChangeShader
//
//---------------------------------------------------------------
void CDocCommands::ChangeShader(const IShaderPtr& shader)
{
	IDocInfoPtr doc(this);
	
	const wstring oldBoss = doc->GetShader()->GetBoss()->GetName();
	if (shader->GetBoss()->GetName() != oldBoss) {
	
		// Create an undo action,
		IUndoActionPtr action(L"Callback Action");
		action->Init(LoadAppString(L"Change Shader"));
		
		// setup the callbacks,
		XCallback2<void, IShaderPtr, bool> callback(doc.Get(), &IDocInfo::SetShader);
		XCallback0<void> doIt = Bind2(callback, shader, kDirtyDoc);
		XCallback0<void> undoIt = Bind2(callback, IShaderPtr(oldBoss), kDirtyDoc);

		IUndoCallbackActionPtr undo(action);
		undo->SetCallbacks(doIt, undoIt);

		// and schedule the action.
		ICommandQueuePtr queue(L"Application");
		queue->Post(action);
	}
}


//---------------------------------------------------------------
//
// CDocCommands::ChangeSpecularCoefficient
//
//---------------------------------------------------------------
void CDocCommands::ChangeSpecularCoefficient(float value)
{
	IDocInfoPtr doc(this);
	
	const SShaderInfo& oldInfo = doc->GetShaderInfo();
	if (!Whisper::Equal(value, oldInfo.specularCoeff)) {
	
		SShaderInfo newInfo = oldInfo;
		newInfo.specularCoeff = value;
		
		this->DoShaderInfo(newInfo, LoadAppString(L"Change Specular Coefficient"));
	}
}


//---------------------------------------------------------------
//
// CDocCommands::ChangeSpecularExponent
//
//---------------------------------------------------------------
void CDocCommands::ChangeSpecularExponent(float value)
{
	IDocInfoPtr doc(this);
	
	const SShaderInfo& oldInfo = doc->GetShaderInfo();
	if (!Whisper::Equal(value, oldInfo.specularExp)) {
	
		SShaderInfo newInfo = oldInfo;
		newInfo.specularExp = value;
		
		this->DoShaderInfo(newInfo, LoadAppString(L"Change Specular Exponent"));
	}
}

//---------------------------------------------------------------
//
// CDocCommands::ChangeW
//
//---------------------------------------------------------------
void CDocCommands::ChangeW(float value)
{
	IDocInfoPtr doc(this);
	
	const SFractalInfo& oldInfo = doc->GetFractalInfo();
	if (!Whisper::Equal(value, oldInfo.w)) {
	
		SFractalInfo newInfo = oldInfo;
		newInfo.w = value;
		
		this->DoFractalInfo(newInfo, LoadAppString(L"Change W"));
	}
}


//---------------------------------------------------------------
//
// CDocCommands::OrbitX
//
//---------------------------------------------------------------
void CDocCommands::OrbitX(int32 units)
{
	PRECONDITION(units != 0);
	
	ICameraPtr camera(this);
	
	XPreference<int32> rotateBy(L"Rotate By", kDefaultRotateBy);
	float theta = units*(*rotateBy);

	// Create an undo action,
	IUndoActionPtr action(L"Callback Action");
	action->Init(LoadAppString(units < 0 ? L"Rotate Up" : L"Rotate Down"));
	
	// setup the callbacks,
	const bool kDirtyDoc = true;
	XCallback2<void, double, bool> callback(camera.Get(), &ICamera::OrbitX);
	XCallback0<void> doIt = Bind2(callback, theta, kDirtyDoc);
	XCallback0<void> undoIt = Bind2(callback, -theta, kDirtyDoc);

	IUndoCallbackActionPtr undo(action);
	undo->SetCallbacks(doIt, undoIt);

	// and schedule the action.
	ICommandQueuePtr queue(L"Application");
	queue->Post(action);
}


//---------------------------------------------------------------
//
// CDocCommands::OrbitY
//
//---------------------------------------------------------------
void CDocCommands::OrbitY(int32 units)
{
	PRECONDITION(units != 0);
	
	ICameraPtr camera(this);
	
	XPreference<int32> rotateBy(L"Rotate By", kDefaultRotateBy);
	float theta = units*(*rotateBy);

	// Create an undo action,
	IUndoActionPtr action(L"Callback Action");
	action->Init(LoadAppString(units < 0 ? L"Rotate Left" : L"Rotate Right"));
	
	// setup the callbacks,
	const bool kDirtyDoc = true;
	XCallback2<void, double, bool> callback(camera.Get(), &ICamera::OrbitY);
	XCallback0<void> doIt = Bind2(callback, -theta, kDirtyDoc);
	XCallback0<void> undoIt = Bind2(callback, theta, kDirtyDoc);

	IUndoCallbackActionPtr undo(action);
	undo->SetCallbacks(doIt, undoIt);

	// and schedule the action.
	ICommandQueuePtr queue(L"Application");
	queue->Post(action);
}


//---------------------------------------------------------------
//
// CDocCommands::OrbitZ
//
//---------------------------------------------------------------
void CDocCommands::OrbitZ(int32 units)
{
	PRECONDITION(units != 0);
	
	ICameraPtr camera(this);
	
	XPreference<int32> rotateBy(L"Rotate By", kDefaultRotateBy);
	float theta = units*(*rotateBy);

	// Create an undo action,
	IUndoActionPtr action(L"Callback Action");
	action->Init(LoadAppString(units < 0 ? L"Rotate CW" : L"Rotate CCW"));
	
	// setup the callbacks,
	const bool kDirtyDoc = true;
	XCallback2<void, double, bool> callback(camera.Get(), &ICamera::OrbitZ);
	XCallback0<void> doIt = Bind2(callback, theta, kDirtyDoc);
	XCallback0<void> undoIt = Bind2(callback, -theta, kDirtyDoc);

	IUndoCallbackActionPtr undo(action);
	undo->SetCallbacks(doIt, undoIt);

	// and schedule the action.
	ICommandQueuePtr queue(L"Application");
	queue->Post(action);
}


//---------------------------------------------------------------
//
// CDocCommands::Pan
//
//---------------------------------------------------------------
void CDocCommands::Pan(int32 horzUnits, int32 vertUnits)
{
	PRECONDITION(horzUnits != 0 || vertUnits != 0);
	
	ICameraPtr camera(this);
	
	XPreference<int32> translateBy(L"Pan By", kDefaultPanBy);

	X2DPoint delta;
	delta.x = horzUnits*(*translateBy/100.0f);
	delta.y = vertUnits*(*translateBy/100.0f);

	// Create an undo action,
	IUndoActionPtr action(L"Callback Action");
	action->Init(LoadAppString(L"Pan"));
	
	// setup the callbacks,
	const bool kDirtyDoc = true;
	XCallback2<void, X2DPoint, bool> callback(camera.Get(), &ICamera::Pan);
	XCallback0<void> doIt = Bind2(callback, delta, kDirtyDoc);
	XCallback0<void> undoIt = Bind2(callback, -delta, kDirtyDoc);

	IUndoCallbackActionPtr undo(action);
	undo->SetCallbacks(doIt, undoIt);

	// and schedule the action.
	ICommandQueuePtr queue(L"Application");
	queue->Post(action);
}

	
//---------------------------------------------------------------
//
// CDocCommands::SetPlacement
//
//---------------------------------------------------------------
void CDocCommands::SetPlacement(const SCameraPlacement& value)
{
	ICameraPtr camera(this);
	
	SCameraPlacement oldValue = camera->GetPlacement();
	if (value != oldValue) {
	
		// Create an undo action,
		IUndoActionPtr action(L"Callback Action");
		action->Init(LoadAppString(L"Camera Placement"));
		
		// setup the callbacks,
		XCallback3<void, SCameraPlacement, bool, bool> callback(camera.Get(), &ICamera::SetPlacement);
		XCallback0<void> doIt = Bind3(callback, value, true, kDirtyDoc);
		XCallback0<void> undoIt = Bind3(callback, oldValue, true, kDirtyDoc);

		IUndoCallbackActionPtr undo(action);
		undo->SetCallbacks(doIt, undoIt);

		// and schedule the action.
		ICommandQueuePtr queue(L"Application");
		queue->Post(action);
	}
}


//---------------------------------------------------------------
//
// CDocCommands::SetRange
//
//---------------------------------------------------------------
void CDocCommands::SetRange(const SCameraRange& value)
{
	ICameraPtr camera(this);
	
	SCameraRange oldValue = camera->GetRange();
	if (value != oldValue) {
	
		// Create an undo action,
		IUndoActionPtr action(L"Callback Action");
		action->Init(LoadAppString(L"Hither/Yon"));
		
		// setup the callbacks,
		XCallback2<void, SCameraRange, bool> callback(camera.Get(), &ICamera::SetRange);
		XCallback0<void> doIt = Bind2(callback, value, kDirtyDoc);
		XCallback0<void> undoIt = Bind2(callback, oldValue, kDirtyDoc);

		IUndoCallbackActionPtr undo(action);
		undo->SetCallbacks(doIt, undoIt);

		// and schedule the action.
		ICommandQueuePtr queue(L"Application");
		queue->Post(action);
	}
}


//---------------------------------------------------------------
//
// CDocCommands::SetViewSize
//
//---------------------------------------------------------------
void CDocCommands::SetViewSize(const X2DSize& value)
{
	IOrthographicCameraPtr camera(this);
	
	X2DSize oldValue = camera->GetViewSize();
	if (value != oldValue) {
	
		// Create an undo action,
		IUndoActionPtr action(L"Callback Action");
		action->Init(LoadAppString(L"View Size"));
		
		// setup the callbacks,
		XCallback2<void, X2DSize, bool> callback(camera.Get(), &IOrthographicCamera::SetViewSize);
		XCallback0<void> doIt = Bind2(callback, value, kDirtyDoc);
		XCallback0<void> undoIt = Bind2(callback, oldValue, kDirtyDoc);

		IUndoCallbackActionPtr undo(action);
		undo->SetCallbacks(doIt, undoIt);

		// and schedule the action.
		ICommandQueuePtr queue(L"Application");
		queue->Post(action);
	}
}


//---------------------------------------------------------------
//
// CDocCommands::Zoom
//
//---------------------------------------------------------------
void CDocCommands::Zoom(bool zoomOut)
{
	ICameraPtr camera(this);
	
	XPreference<float> zoomBy(L"Zoom By", kDefaultZoomBy);
	double scaleBy = zoomOut ? *zoomBy : 1.0/(*zoomBy);

	// Create an undo action,
	IUndoActionPtr action(L"Callback Action");
	action->Init(LoadAppString(zoomOut ? L"Zoom Out" : L"Zoom In"));
	
	// setup the callbacks,
	const bool kDirtyDoc = true;
	XCallback2<void, double, bool> callback(camera.Get(), &ICamera::Zoom);
	XCallback0<void> doIt = Bind2(callback, scaleBy, kDirtyDoc);
	XCallback0<void> undoIt = Bind2(callback, 1.0/scaleBy, kDirtyDoc);

	IUndoCallbackActionPtr undo(action);
	undo->SetCallbacks(doIt, undoIt);

	// and schedule the action.
	ICommandQueuePtr queue(L"Application");
	queue->Post(action);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CDocCommands::DoShaderInfo
//
//---------------------------------------------------------------
void CDocCommands::DoShaderInfo(const SShaderInfo& newInfo, const std::wstring& message)
{
	// Get the old info,
	IDocInfoPtr doc(this);
	const SShaderInfo& oldInfo = doc->GetShaderInfo();
	ASSERT(oldInfo != newInfo);

	// create an undo action,
	IUndoActionPtr action(L"Callback Action");
	action->Init(message);
	
	// setup the callbacks,
	XCallback2<void, SShaderInfo, bool> callback(doc.Get(), &IDocInfo::SetShaderInfo);
	XCallback0<void> doIt = Bind2(callback, newInfo, kDirtyDoc);
	XCallback0<void> undoIt = Bind2(callback, oldInfo, kDirtyDoc);

	IUndoCallbackActionPtr undo(action);
	undo->SetCallbacks(doIt, undoIt);

	// and schedule the action.
	ICommandQueuePtr queue(L"Application");
	queue->Post(action);
}


//---------------------------------------------------------------
//
// CDocCommands::DoFractalInfo
//
//---------------------------------------------------------------
void CDocCommands::DoFractalInfo(const SFractalInfo& newInfo, const std::wstring& message)
{
	// Get the old info,
	IDocInfoPtr doc(this);
	const SFractalInfo& oldInfo = doc->GetFractalInfo();
	ASSERT(oldInfo != newInfo);

	// create an undo action,
	IUndoActionPtr action(L"Callback Action");
	action->Init(message);
	
	// setup the callbacks,
	XCallback2<void, SFractalInfo, bool> callback(doc.Get(), &IDocInfo::SetFractalInfo);
	XCallback0<void> doIt = Bind2(callback, newInfo, kDirtyDoc);
	XCallback0<void> undoIt = Bind2(callback, oldInfo, kDirtyDoc);

	IUndoCallbackActionPtr undo(action);
	undo->SetCallbacks(doIt, undoIt);

	// and schedule the action.
	ICommandQueuePtr queue(L"Application");
	queue->Post(action);
}


