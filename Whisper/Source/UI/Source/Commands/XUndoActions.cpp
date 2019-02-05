/*
 *  File:       XUndoActions.cpp
 *  Summary:   	Some handy undo actions.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XUndoActions.cpp,v $
 *		Revision 1.7  2001/04/27 04:37:44  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.6  2001/03/27 06:31:25  jesjones
 *		Added SUndoSequence. Multiple undo action calls the helper instead of the command.
 *		
 *		Revision 1.5  2001/01/30 07:24:31  jesjones
 *		Added ICommandHelper and IUndoActionHelper.
 *		
 *		Revision 1.4  2001/01/01 03:53:28  jesjones
 *		Added IUndoCallbackAction::Post methods.
 *		
 *		Revision 1.3  2000/12/14 08:37:43  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:58:05  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	12/18/99	JDJ		XUndoClosureAction allows for the redo action to be
 *									different than the do action.
 *		 <2>	 6/24/99	JDJ		Added XUndoClosureAction.
 *		 <1>	12/26/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <IUndoActions.h>

#include <list>

#include <IActiveUndoContext.h>
#include <ICommand.h>
#include <ICommandQueue.h>
#include <IUndoActionHelper.h>
#include <IUndoContext.h>
#include <XInterfaceMacros.h>
#include <IUndoActionHelper.h>

namespace Whisper {


// ===================================================================================
//	struct SUndoSequence
// ===================================================================================

//---------------------------------------------------------------
//
// SUndoSequence::~SUndoSequence
//
//---------------------------------------------------------------
SUndoSequence::~SUndoSequence()
{
	IUndoActionPtr action(L"End Sequence Action");
	action->Init(L"");
	
	ICommandQueuePtr queue(L"Application");
	queue->Post(action);
}

	
//---------------------------------------------------------------
//
// SUndoSequence::SUndoSequence
//
//---------------------------------------------------------------
SUndoSequence::SUndoSequence(const std::wstring& text)
{
	IUndoActionPtr action(L"Begin Sequence Action");
	action->Init(text);
	
	ICommandQueuePtr queue(L"Application");
	queue->Post(action);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XBeginUndoSequence
//!		Used by SUndoSequence to mark the start of a sequence.
// ===================================================================================
class XBeginUndoSequence : public IBeginUndoSequence {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XBeginUndoSequence();
	
						XBeginUndoSequence(XBoss* boss);
};

DEFINE_INTERFACE_FACTORY(XBeginUndoSequence)

//---------------------------------------------------------------
//
// XBeginUndoSequence::~XBeginUndoSequence
//
//---------------------------------------------------------------
XBeginUndoSequence::~XBeginUndoSequence()
{
}


//---------------------------------------------------------------
//
// XBeginUndoSequence::XBeginUndoSequence
//
//---------------------------------------------------------------
XBeginUndoSequence::XBeginUndoSequence(XBoss* boss)
{
	IBeginUndoSequence::DoSetBoss(boss);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XEndUndoSequence
//!		Used by SUndoSequence to mark the end of a sequence.
// ===================================================================================
class XEndUndoSequence : public IEndUndoSequence {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XEndUndoSequence();
	
						XEndUndoSequence(XBoss* boss);
};

DEFINE_INTERFACE_FACTORY(XEndUndoSequence)

//---------------------------------------------------------------
//
// XEndUndoSequence::~XEndUndoSequence
//
//---------------------------------------------------------------
XEndUndoSequence::~XEndUndoSequence()
{
}


//---------------------------------------------------------------
//
// XEndUndoSequence::XEndUndoSequence
//
//---------------------------------------------------------------
XEndUndoSequence::XEndUndoSequence(XBoss* boss)
{
	IEndUndoSequence::DoSetBoss(boss);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XDummyUndoAction
//!		Dummy IUndoActionHelper used by the begin and end sequence commands.
// ===================================================================================
class XDummyUndoAction : public IUndoActionHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XDummyUndoAction();
	
						XDummyUndoAction(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		OnDo();
	virtual void 		OnUndo();
	virtual void 		OnRedo();
};

DEFINE_INTERFACE_FACTORY(XDummyUndoAction)

//---------------------------------------------------------------
//
// XDummyUndoAction::~XDummyUndoAction
//
//---------------------------------------------------------------
XDummyUndoAction::~XDummyUndoAction()
{
}


//---------------------------------------------------------------
//
// XDummyUndoAction::XDummyUndoAction
//
//---------------------------------------------------------------
XDummyUndoAction::XDummyUndoAction(XBoss* boss)
{
	IUndoActionHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XDummyUndoAction::OnDo
//
//---------------------------------------------------------------
void XDummyUndoAction::OnDo()
{
	DEBUGSTR("No one should be calling XDummyUndoAction::OnDo!");
}


//---------------------------------------------------------------
//
// XDummyUndoAction::OnUndo
//
//---------------------------------------------------------------
void XDummyUndoAction::OnUndo()
{
	DEBUGSTR("No one should be calling XDummyUndoAction::OnUndo!");
}


//---------------------------------------------------------------
//
// XDummyUndoAction::OnRedo
//
//---------------------------------------------------------------
void XDummyUndoAction::OnRedo()
{
	DEBUGSTR("No one should be calling XDummyUndoAction::OnRedo!");
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XMultipleUndoAction
//!		Executes undoable actions as a group/transaction.
// ===================================================================================
class XMultipleUndoAction : public IUndoActionHelper, public IMultipleUndoAction {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XMultipleUndoAction();
	
						XMultipleUndoAction(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		AddAction(const IUndoActionPtr& action);

	virtual void 		OnDo();

	virtual void 		OnUndo();

	virtual void 		OnRedo();
	
//-----------------------------------
//	Internal Types
//
protected:
	typedef std::list<IUndoActionPtr> ActionList;

//-----------------------------------
//	Member Data
//
protected:
	ActionList	mActions;
};

DEFINE_INTERFACE_FACTORY(XMultipleUndoAction)

//---------------------------------------------------------------
//
// XMultipleUndoAction::~XMultipleUndoAction
//
//---------------------------------------------------------------
XMultipleUndoAction::~XMultipleUndoAction()
{
}


//---------------------------------------------------------------
//
// XMultipleUndoAction::XMultipleUndoAction
//
//---------------------------------------------------------------
XMultipleUndoAction::XMultipleUndoAction(XBoss* boss)
{
	IUndoActionHelper::DoSetBoss(boss);
	IMultipleUndoAction::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XMultipleUndoAction::AddAction
//
//---------------------------------------------------------------
void XMultipleUndoAction::AddAction(const IUndoActionPtr& action)
{
	PRECONDITION(action);
	
	mActions.push_back(action);
}


//---------------------------------------------------------------
//
// XMultipleUndoAction::OnDo
//
//---------------------------------------------------------------
void XMultipleUndoAction::OnDo()
{
	ActionList::iterator iter = mActions.begin();
	
	try {
		while (iter != mActions.end()) {
			IUndoActionHelperPtr action = *iter;
			++iter;
			
			action->OnDo();
		}
	
	} catch (...) {
		while (iter != mActions.begin()) {		// undo any commands that were completed
			--iter;

			IUndoActionHelperPtr action = *iter;
			
			action->OnUndo();
		}
		throw;
	}
}


//---------------------------------------------------------------
//
// XMultipleUndoAction::OnUndo
//
//---------------------------------------------------------------
void XMultipleUndoAction::OnUndo()
{
	ActionList::reverse_iterator iter = mActions.rbegin();
	
	try {
		while (iter != mActions.rend()) {
			IUndoActionHelperPtr action = *iter;
			++iter;
			
			action->OnUndo();
		}
	
	} catch (...) {
		while (iter != mActions.rbegin()) {		// redo any commands that were completed
			--iter;

			IUndoActionHelperPtr action = *iter;
			
			action->OnRedo();
		}
		throw;
	}
}


//---------------------------------------------------------------
//
// XMultipleUndoAction::OnRedo
//
//---------------------------------------------------------------
void XMultipleUndoAction::OnRedo()
{
	ActionList::iterator iter = mActions.begin();
	
	try {
		while (iter != mActions.end()) {
			IUndoActionHelperPtr action = *iter;
			++iter;
			
			action->OnRedo();
		}
	
	} catch (...) {
		while (iter != mActions.begin()) {		// undo any commands that were completed
			--iter;

			IUndoActionHelperPtr action = *iter;
			
			action->OnUndo();
		}
		throw;
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class IUndoCallbackAction
// ===================================================================================

//---------------------------------------------------------------
//
// IUndoCallbackAction::Post (Callback, Callback)
//
//---------------------------------------------------------------
void IUndoCallbackAction::Post(const std::wstring& message, const Callback& doIt, const Callback& undoIt)
{
	// Create an undo action,
	IUndoActionPtr action(L"Undo Callback Action");
	action->Init(message);

	IUndoCallbackActionPtr undo(action);
	undo->SetCallbacks(doIt, undoIt);

	// and schedule the action.
	ICommandQueuePtr queue(L"Application");
	queue->Post(action);
}

	
//---------------------------------------------------------------
//
// IUndoCallbackAction::Post (Callback, Callback)
//
//---------------------------------------------------------------
void IUndoCallbackAction::Post(const std::wstring& message, const Callback& doIt, const Callback& undoIt, const Callback& redoIt)
{
	// Create an undo action,
	IUndoActionPtr action(L"Undo Callback Action");
	action->Init(message);

	IUndoCallbackActionPtr undo(action);
	undo->SetCallbacks(doIt, undoIt, redoIt);

	// and schedule the action.
	ICommandQueuePtr queue(L"Application");
	queue->Post(action);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XUndoCallbackAction
//!		Uses XCallback objects to perform the do and undo actions.
// ===================================================================================
class XUndoCallbackAction : public IUndoActionHelper, public IUndoCallbackAction {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XUndoCallbackAction();
	
						XUndoCallbackAction(XBoss* boss);
						
	virtual void 		SetCallbacks(const XCallback0<void>& doIt, const XCallback0<void>& undoIt);
	
	virtual void 		SetCallbacks(const XCallback0<void>& doIt, const XCallback0<void>& undoIt, const XCallback0<void>& redoIt);

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		OnDo();

	virtual void 		OnUndo();

	virtual void 		OnRedo();
	
//-----------------------------------
//	Member Data
//
protected:
	XCallback0<void>	mDoIt;
	XCallback0<void>	mUndoIt;
	XCallback0<void>	mRedoIt;
};

DEFINE_INTERFACE_FACTORY(XUndoCallbackAction)

//---------------------------------------------------------------
//
// XUndoCallbackAction::~XUndoCallbackAction
//
//---------------------------------------------------------------
XUndoCallbackAction::~XUndoCallbackAction()
{
}


//---------------------------------------------------------------
//
// XUndoCallbackAction::XUndoCallbackAction
//
//---------------------------------------------------------------
XUndoCallbackAction::XUndoCallbackAction(XBoss* boss)
{
	IUndoActionHelper::DoSetBoss(boss);
	IUndoCallbackAction::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XUndoCallbackAction::SetCallbacks (Callback, Callback)
//
//---------------------------------------------------------------
void XUndoCallbackAction::SetCallbacks(const Callback& doIt, const Callback& undoIt)
{
	mDoIt   = doIt;
	mUndoIt = undoIt;
	mRedoIt = doIt;
}

	
//---------------------------------------------------------------
//
// XUndoCallbackAction::SetCallbacks (Callback, Callback, Callback)
//
//---------------------------------------------------------------
void XUndoCallbackAction::SetCallbacks(const Callback& doIt, const Callback& undoIt, const Callback& redoIt)
{
	mDoIt   = doIt;
	mUndoIt = undoIt;
	mRedoIt = redoIt;
}


//---------------------------------------------------------------
//
// XUndoCallbackAction::OnDo
//
//---------------------------------------------------------------
void XUndoCallbackAction::OnDo()
{
	mDoIt();
}


//---------------------------------------------------------------
//
// XUndoCallbackAction::OnUndo
//
//---------------------------------------------------------------
void XUndoCallbackAction::OnUndo()
{
	mUndoIt();
}


//---------------------------------------------------------------
//
// XUndoCallbackAction::OnRedo
//
//---------------------------------------------------------------
void XUndoCallbackAction::OnRedo()
{
	mRedoIt();
}


}	// namespace Whisper
