/*
 *  File:       XMergeAction.cpp
 *  Summary:   	IUndoAction that uses IMemento's to allow actions to be merged.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XMergeAction.cpp,v $
 *		Revision 1.2  2001/01/30 07:24:17  jesjones
 *		Added ICommandHelper and IUndoActionHelper.
 *		
 *		Revision 1.1  2000/12/24 03:14:05  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <IMergeActions.h>
#include <IUndoActionHelper.h>

#include <IInstance.h>
#include <IMemento.h>
#include <IUndoAction.h>
#include <IUndoContext.h>

namespace Whisper {


// ===================================================================================
//	class XMergeAction
//!		IUndoAction that uses IMemento's to allow actions to be merged. 
/*!		Note that this is unlike most undo actions in that our caller is responsible 
 *		for handling the Do after calling Init. */
// ===================================================================================
class XMergeAction : public IUndoActionHelper, public IMergeActions {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XMergeAction();
	
						XMergeAction(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool		CanMerge(const IConstUndoActionPtr& newAction) const;
	virtual void		Merge(const IConstUndoActionPtr& newAction);

protected:
	virtual void 		OnDo();
	virtual void 		OnUndo();
	virtual void 		OnRedo();
	
//-----------------------------------
//	Member Data
//
protected:
	bool		mReDid;
	
	SMementoPtr	mOldState;
	SMementoPtr	mNewState;
};

DEFINE_INTERFACE_FACTORY(XMergeAction)

//---------------------------------------------------------------
//
// XMergeAction::~XMergeAction
//
//---------------------------------------------------------------
XMergeAction::~XMergeAction()
{
}


//---------------------------------------------------------------
//
// XMergeAction::XMergeAction
//
//---------------------------------------------------------------
XMergeAction::XMergeAction(XBoss* boss)
{
	IUndoActionHelper::DoSetBoss(boss);
	IMergeActions::DoSetBoss(boss);
	
	IUndoActionHelper* thisPtr = this;
	IInstancePtr instance(thisPtr);
	XUnknownPtr object = instance->Get();
	
	IConstMementoPtr memento(object);
	mOldState = memento->GetState(); 

	mReDid = false;
}


//---------------------------------------------------------------
//
// XMergeAction::CanMerge
//
//---------------------------------------------------------------
bool XMergeAction::CanMerge(const IConstUndoActionPtr& newAction) const
{
	bool canMerge = false;
	
	if (!mReDid) {
	
		// Downcast the new interface pointer to our implementation 
		// pointer (this is legal because we're only called if the
		// two bosses match).
		IConstUndoActionHelperPtr helper(newAction);
		const XMergeAction* action = dynamic_cast<const XMergeAction*>(helper.Get());
		ASSERT(action != nil);			
		
		// Get the object our action operates on.
		const IUndoActionHelper* thisPtr = this;
		IConstInstancePtr instance(thisPtr);
		XConstUnknownPtr object = instance->Get();
	
		// Get the object the new action operates on.
		IConstInstancePtr newInstance(newAction);
		XConstUnknownPtr newObject = newInstance->Get();
	
		// If they match, ask the memento's if they're compatible.
		if (object == newObject)
			canMerge = mNewState->CanMerge(action->mOldState.Get(), object);
	}
		
	return canMerge;
}


//---------------------------------------------------------------
//
// XMergeAction::Merge
//
//---------------------------------------------------------------
void XMergeAction::Merge(const IConstUndoActionPtr& newAction)
{
	IConstUndoActionHelperPtr helper(newAction);
	const XMergeAction* action = dynamic_cast<const XMergeAction*>(helper.Get());
	ASSERT(action != nil);				// we're only called if the bosses match so this should never fire
	
	mNewState = action->mNewState; 
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XMergeAction::OnDo
//
// Our creator does the Do after calling Init, but we still need
// to save the object's new state.
//
//---------------------------------------------------------------
void XMergeAction::OnDo()
{
	IUndoActionHelper* thisPtr = this;
	IInstancePtr instance(thisPtr);
	XUnknownPtr object = instance->Get();
	
	IConstMementoPtr memento(object);	
	mNewState = memento->GetState(); 		
}


//---------------------------------------------------------------
//
// XMergeAction::OnUndo
//
//---------------------------------------------------------------
void XMergeAction::OnUndo()
{
	IUndoActionHelper* thisPtr = this;
	IInstancePtr instance(thisPtr);
	XUnknownPtr object = instance->Get();
	
	IMementoPtr memento(object);
	memento->SetState(mOldState);
}


//---------------------------------------------------------------
//
// XMergeAction::OnRedo
//
//---------------------------------------------------------------
void XMergeAction::OnRedo()
{
	IUndoActionHelper* thisPtr = this;
	IInstancePtr instance(thisPtr);
	XUnknownPtr object = instance->Get();
	
	IMementoPtr memento(object);
	memento->SetState(mNewState);

	mReDid = true;
}


}	// namespace Whisper
