/*
 *  File:       XUndoContext.cpp
 *  Summary:   	A singleton that manages the undo/redo list.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XUndoContext.cpp,v $
 *		Revision 1.4  2001/04/27 04:37:49  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2000/12/14 08:37:47  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:58:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IUndoContext.h>

#include <list>

#include <IActiveUndoContext.h>
#include <IUndoAction.h>
#include <XError.h>
#include <XExceptions.h>
#include <XInterfaceMacros.h>
#include <XMiscUtils.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	class XUndoContext
//!		A context for undo (each window and the app typically have one of these).
// ===================================================================================
class XUndoContext : public IUndoContext {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XUndoContext();
	
						XUndoContext(XBoss* boss);

//-----------------------------------
//	Inherited API
//
public:
	virtual bool		CanUndo() const;
			
	virtual void		Undo();
			
	virtual std::wstring GetUndoText() const;
			
	virtual bool		CanRedo() const;
			
	virtual void		Redo();
			
	virtual std::wstring GetRedoText() const;
			
	virtual void		ClearRedoHistory();

	virtual void		ClearHistory();
			
	virtual void		SetMaxCommands(uint32 newMax);
			
	virtual IUndoActionPtr GetUndoCommand() const;
			
	virtual IUndoActionPtr GetRedoCommand() const;

	virtual void		AddCommand(const IUndoActionPtr& action);
						
	virtual bool		HasCommand(const IUndoActionPtr& action) const;

//-----------------------------------
//	Member Data
//
private:
	uint32 						mMaxCommands;
	
	std::list<IUndoActionPtr>	mUndoStack;
	std::list<IUndoActionPtr>	mRedoStack;
};

DEFINE_INTERFACE_FACTORY(XUndoContext)

//---------------------------------------------------------------
//
// XUndoContext::~XUndoContext
//
//---------------------------------------------------------------
XUndoContext::~XUndoContext()
{
#if DEBUG
	if (!this->GetBoss()->IsPrototype()) {
		XInterfacePtr<IActiveUndoContext> activeContext(L"Application");
		ASSERT(activeContext->Get() != this);
	}
#endif

	this->ClearHistory();
}


//---------------------------------------------------------------
//
// XUndoContext::XUndoContext
//
//---------------------------------------------------------------
XUndoContext::XUndoContext(XBoss* boss)
{
	this->DoSetBoss(boss);

	mMaxCommands = 32;			
}


//---------------------------------------------------------------
//
// XUndoContext::CanUndo
//
//---------------------------------------------------------------
bool XUndoContext::CanUndo() const
{
	bool can = !mUndoStack.empty();
	
	return can;
}


//---------------------------------------------------------------
//
// XUndoContext::Undo
//
//---------------------------------------------------------------
void XUndoContext::Undo()
{
	PRECONDITION(this->CanUndo());
	
	IUndoActionPtr action;
		
	try {
		action = mUndoStack.back();
		mUndoStack.pop_back();
	
		action->HandleUndo();
		
		mRedoStack.push_back(action);

	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't undo."), e);
	
	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't undo."));
	}			
}


//---------------------------------------------------------------
//
// XUndoContext::GetUndoText
//
//---------------------------------------------------------------
std::wstring XUndoContext::GetUndoText() const
{
	std::wstring text;
	
	if (this->CanUndo()) {
		const IUndoActionPtr& action = mUndoStack.back();
		text = LoadWhisperString(L"Undo #1", action->GetText());
		
	} else
		text = LoadWhisperString(L"Can't Undo");
		
	return text;
}


//---------------------------------------------------------------
//
// XUndoContext::GetUndoCommand
//
//---------------------------------------------------------------
IUndoActionPtr XUndoContext::GetUndoCommand() const
{
	IUndoActionPtr action;
	
	if (!mUndoStack.empty())
		action = mUndoStack.back();
		
	return action;
}


//---------------------------------------------------------------
//
// XUndoContext::CanRedo
//
//---------------------------------------------------------------
bool XUndoContext::CanRedo() const
{
	bool can = !mRedoStack.empty();
	
	return can;
}


//---------------------------------------------------------------
//
// XUndoContext::Redo
//
//---------------------------------------------------------------
void XUndoContext::Redo()
{
	PRECONDITION(this->CanRedo());
	
	IUndoActionPtr action;
		
	try {
		action = mRedoStack.back();
		mRedoStack.pop_back();

		action->HandleRedo();
		
		mUndoStack.push_back(action);

	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't redo."), e);
	
	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't redo."));
	}			
}


//---------------------------------------------------------------
//
// XUndoContext::GetRedoText
//
//---------------------------------------------------------------
std::wstring XUndoContext::GetRedoText() const
{
	std::wstring text;
	
	if (this->CanRedo()) {
		const IUndoActionPtr& action = mRedoStack.back();
		text = LoadWhisperString(L"Redo #1", action->GetText());
		
	} else
		text = LoadWhisperString(L"Can't Redo");
		
	return text;
}


//---------------------------------------------------------------
//
// XUndoContext::GetRedoCommand
//
//---------------------------------------------------------------
IUndoActionPtr XUndoContext::GetRedoCommand() const
{
	IUndoActionPtr action;
	
	if (!mRedoStack.empty())
		action = mRedoStack.back();
		
	return action;
}


//---------------------------------------------------------------
//
// XUndoContext::ClearRedoHistory
//
//---------------------------------------------------------------
void XUndoContext::ClearRedoHistory()
{
	mRedoStack.clear();
}


//---------------------------------------------------------------
//
// XUndoContext::ClearHistory
//
//---------------------------------------------------------------
void XUndoContext::ClearHistory()
{
	mRedoStack.clear();
	mUndoStack.clear();
}


//---------------------------------------------------------------
//
// XUndoContext::SetMaxCommands
//
//---------------------------------------------------------------
void XUndoContext::SetMaxCommands(uint32 newMax)
{
	PRECONDITION(newMax >= 1);
	
	mMaxCommands = newMax;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XUndoContext::AddCommand
//
//---------------------------------------------------------------
void XUndoContext::AddCommand(const IUndoActionPtr& action)
{
	PRECONDITION(action);
	PRECONDITION(std::find(mUndoStack.begin(), mUndoStack.end(), action) == mUndoStack.end());
	PRECONDITION(std::find(mRedoStack.begin(), mRedoStack.end(), action) == mRedoStack.end());
	
	mUndoStack.push_back(action);				
	
	this->ClearRedoHistory();
		
	while (mUndoStack.size() > mMaxCommands)
		mUndoStack.pop_front();
}


//---------------------------------------------------------------
//
// XUndoContext::HasCommand
//
//---------------------------------------------------------------
bool XUndoContext::HasCommand(const IUndoActionPtr& action) const
{	
	bool has = false;

	if (std::find(mUndoStack.begin(), mUndoStack.end(), action) != mUndoStack.end())
		has = true;
		
	else if (std::find(mRedoStack.begin(), mRedoStack.end(), action) != mRedoStack.end())
		has = true;
 
	return has;
}


}	// namespace Whisper
