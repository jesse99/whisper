/*
 *  File:       CFormulaState.cpp
 *  Summary:   	Implementation used to swap around the fractal state when a new fractal formula is selected.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CFormulaState.cpp,v $
 *		Revision 1.5  2001/04/21 02:24:11  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/04/05 04:00:06  jesjones
 *		Saved state now includes camera settings.
 *		
 *		Revision 1.3  2001/04/03 04:35:49  jesjones
 *		DoSave calls Trim.
 *		
 *		Revision 1.2  2001/03/30 04:13:02  jesjones
 *		MakeDefault plays a sound (on Macs).
 *		
 *		Revision 1.1  2001/03/28 09:30:41  jesjones
 *		Checked in	
 */

#include "AppHeader.h"
#include "IFormulaState.h"

#if MAC
	#include <Sound.h>
#endif

#include <IUndoActions.h>
#include <XError.h>
#include <XFile.h>
#include <XFileStream.h>
#include <XFolderSpec.h>
#include <XStreaming.h>
#include <XStringUtils.h>

#include "ICamera.h"
#include "IDocCommands.h"
#include "IDocInfo.h"
#include "IFractalFunction.h"
#include "IOrthographicCamera.h"


// ===================================================================================
//	class CFormulaState
//!		Implementation used to swap around the fractal state when a new fractal formula is selected.
// ===================================================================================
class CFormulaState : public IFormulaState {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CFormulaState();
	
						CFormulaState(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		SetState(IDocInfoPtr& doc, const std::wstring& formula, bool dirtyDoc = false);
	virtual void 		ChangeState(IDocInfoPtr& doc, const std::wstring& formula);
	virtual void 		MakeDefault(const IConstDocInfoPtr& doc);
	
//-----------------------------------
//	Internal API
//
private:
			void 		DoLoad();
			void 		DoSave();

//-----------------------------------
//	Internal Types
//
private:
	struct SState {
		std::wstring		formula;
		SFractalInfo		info;			
		SCameraPlacement	placement;
		SCameraRange		range;
		X2DSize				viewSize;
		
					SState() : viewSize(2.5f, 2.0f)				{}
					SState(const std::wstring& f) : formula(f), viewSize(2.5f, 2.0f)	{}
		
			bool 	operator==(const std::wstring& rhs) const	{return formula == rhs;}
			bool 	operator<(const std::wstring& rhs) const	{return formula < rhs;}
			
			friend XInStream&  operator>>(XInStream& stream, SState& value);
			friend XOutStream& operator<<(XOutStream& stream, const SState& value);
	};

//-----------------------------------
//	Member Data
//
private:
	vector<SState>	mState;
	bool			mSaveFailed;
};

DEFINE_INTERFACE_FACTORY(CFormulaState)

//---------------------------------------------------------------
//
// operator>> (SState)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, CFormulaState::SState& value)			
{
	int32 version;
	stream >> version
	       >> value.formula 
	       >> value.info 
	       >> value.placement 
	       >> value.range
	       >> value.viewSize; 
	ASSERT(version == 1);
	
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (SState)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const CFormulaState::SState& value)	
{
	int32 version = 1;
	stream << version
	       << value.formula 
	       << value.info 
	       << value.placement 
	       << value.range
	       << value.viewSize; 
	
	return stream;
}


//---------------------------------------------------------------
//
// CFormulaState::~CFormulaState
//
//---------------------------------------------------------------
CFormulaState::~CFormulaState()
{
}


//---------------------------------------------------------------
//
// CFormulaState::CFormulaState
//
//---------------------------------------------------------------
CFormulaState::CFormulaState(XBoss* boss) 
{	
	IFormulaState::DoSetBoss(boss);
	
	mSaveFailed = false;
	
	try {
		this->DoLoad();
	} catch (...) {
	}
}


//---------------------------------------------------------------
//
// CFormulaState::SetState
//
//---------------------------------------------------------------
void CFormulaState::SetState(IDocInfoPtr& doc, const std::wstring& formula, bool dirtyDoc)
{
	SState state;
	
	vector<SState>::iterator iter = std::find(mState.begin(), mState.end(), formula);
	if (iter != mState.end())
		state = *iter;

	doc->SetFractalInfo(state.info, dirtyDoc);

	ICameraPtr camera(doc);
	camera->SetPlacement(state.placement, dirtyDoc);
	camera->SetRange(state.range, dirtyDoc);

	IOrthographicCameraPtr ortho(doc);
	ortho->SetViewSize(state.viewSize, dirtyDoc);
}


//---------------------------------------------------------------
//
// CFormulaState::ChangeState
//
//---------------------------------------------------------------
void CFormulaState::ChangeState(IDocInfoPtr& doc, const std::wstring& formula)
{
	SState state;
	
	vector<SState>::iterator iter = std::find(mState.begin(), mState.end(), formula);
	if (iter != mState.end()) 
		state = *iter;

	SUndoSequence sequence(LoadAppString(L"Change Formula"));

	IDocCommandsPtr commands(doc);	
	commands->ChangeFractalInfo(state.info);
	commands->SetPlacement(state.placement);
	commands->SetRange(state.range);
	commands->SetViewSize(state.viewSize);
}


//---------------------------------------------------------------
//
// CFormulaState::MakeDefault
//
//---------------------------------------------------------------
void CFormulaState::MakeDefault(const IConstDocInfoPtr& doc)
{
	IConstFractalFunctionPtr function = doc->GetFractalFunction();
	std::wstring formula = function->GetFormula();

	vector<SState>::iterator iter = std::find(mState.begin(), mState.end(), formula);
	if (iter == mState.end()) {
		mState.push_back(formula);
		iter = mState.end() - 1;
	}
					
	iter->info = doc->GetFractalInfo();

	IConstCameraPtr camera(doc);
	iter->placement = camera->GetPlacement();
	iter->range     = camera->GetRange();

	IConstOrthographicCameraPtr ortho(doc);
	iter->viewSize = ortho->GetViewSize();

	if (!mSaveFailed) {
		this->DoSave();
		
#if MAC
		Handle sound = GetResource('snd ', 256);
		if (sound != nil)
			(void) SndPlay(nil, (SndListHandle) sound, true);
#endif
	}	
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CFormulaState::DoLoad
//
//---------------------------------------------------------------
void CFormulaState::DoLoad()
{
	mState.clear();
	
	XFolderSpec dir(XFolderSpec::GetAppFolder(), L"Settings");
	XFileSpec spec(dir, L"Default Fractals");

	XFile file(spec);
	file.Open(kReadPermission);
	
	XInFileStream stream(file);
	
	uint32 count;
	stream >> count;
	mState.reserve(count);
	
	for (uint32 i = 0; i < count; ++i) {
		SState state;
		stream >> state;
		mState.push_back(state);
	}
	
	file.Close();
}


//---------------------------------------------------------------
//
// CFormulaState::DoSave
//
//---------------------------------------------------------------
void CFormulaState::DoSave()
{
	PRECONDITION(!mSaveFailed);		// might be on a locked volume so don't keep pestering the user

	try {
		XFolderSpec dir(XFolderSpec::GetAppFolder(), L"Settings");
		XFileSpec spec(dir, L"Default Fractals");
		XFile file(spec);
		file.Open('HypM', 'Setg', kWritePermission);
		
		XOutFileStream stream(file);
		
		uint32 count = mState.size();
		stream << count;
		for (uint32 i = 0; i < mState.size(); ++i) {
			const SState& state = mState[i];
			stream << state;
		}
		
		file.Trim();
		file.Close();
	
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't save the default fractal info."), e);
		mSaveFailed = true;

	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't save the default fractal info."));
		mSaveFailed = true;
	}	
}


