/*
 *  File:       XUndoAction.cpp
 *  Summary:   	Helper class for IUndoAction.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XUndoAction.cpp,v $
 *		Revision 1.8  2001/04/27 04:37:40  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.7  2001/04/02 23:36:02  jesjones
 *		Tweaked for CW Pro 5.3
 *		
 *		Revision 1.6  2001/03/28 09:46:45  jesjones
 *		Undo sequences can now be nested.
 *		
 *		Revision 1.5  2001/03/27 07:43:48  jesjones
 *		Added support for undo sequences.
 *		
 *		Revision 1.4  2001/01/30 07:24:27  jesjones
 *		Added ICommandHelper and IUndoActionHelper.
 *		
 *		Revision 1.3  2000/12/24 03:26:58  jesjones
 *		Added support for merging undo actions.
 *		
 *		Revision 1.2  2000/11/09 12:57:55  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IUndoAction.h>

#include <IActiveUndoContext.h>
#include <IMergeActions.h>
#include <IUndoActionHelper.h>
#include <IUndoActions.h>
#include <IUndoContext.h>

namespace Whisper {


// ===================================================================================
//	class XUndoAction
//!		Helper class for IUndoAction.
// ===================================================================================
class XUndoAction : public IUndoAction {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XUndoAction();
	
						XUndoAction(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const std::wstring& text);
	virtual void 		Init(const std::wstring& text, const IUndoContextPtr& context);

	virtual void 		HandleDo();
	virtual void		HandleUndo();
	virtual void		HandleRedo();
		
	virtual std::wstring GetText() const;
	
//-----------------------------------
//	Internal API
//
private:
			bool 		DoMerge();
			bool 		DoSequence();
	
//-----------------------------------
//	Internal Types
//
private:
	typedef std::vector<IMultipleUndoActionPtr> SequenceStack;
	
//-----------------------------------
//	Member Data
//
private:
	IUndoContextPtr			mContext;
	std::wstring			mText;
	
	static SequenceStack	msSequences;
};

DEFINE_INTERFACE_FACTORY(XUndoAction)
XUndoAction::SequenceStack XUndoAction::msSequences;

//---------------------------------------------------------------
//
// XUndoAction::~XUndoAction
//
//---------------------------------------------------------------
XUndoAction::~XUndoAction()
{
}


//---------------------------------------------------------------
//
// XUndoAction::XUndoAction
//
//---------------------------------------------------------------
XUndoAction::XUndoAction(XBoss* boss)
{
	IUndoAction::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XUndoAction::Init (wstring)
//
//---------------------------------------------------------------
void XUndoAction::Init(const std::wstring& text)
{
	IActiveUndoContextPtr activeContext(L"Application");
	mContext = activeContext->Get();
	
	mText = text;

	IUndoActionHelperPtr helper(this);	// run the IUndoActionHelper ctor (XMergeAction's ctor has to be run before OnDo is called)
}


//---------------------------------------------------------------
//
// XUndoAction::Init (wstring, IUndoContextPtr)
//
//---------------------------------------------------------------
void XUndoAction::Init(const std::wstring& text, const IUndoContextPtr& context)
{
	mContext = context;
	
	mText = text;

	IUndoActionHelperPtr helper(this);	// run the IUndoActionHelper ctor (XMergeAction's ctor has to be run before OnDo is called)
}


//---------------------------------------------------------------
//
// XUndoAction::HandleDo
//
//---------------------------------------------------------------
void XUndoAction::HandleDo()
{
	if (!this->DoSequence()) {
		IUndoActionHelperPtr helper(this);
		helper->OnDo();			

		if (mContext)
			if (!this->DoMerge())
				mContext->AddCommand(this);		// 'this' is saved on the undo stack so it will stick around
	}
}


//---------------------------------------------------------------
//
// XUndoAction::HandleUndo
//
//---------------------------------------------------------------
void XUndoAction::HandleUndo()
{
	IUndoActionHelperPtr helper(this);
	helper->OnUndo();			
}


//---------------------------------------------------------------
//
// XUndoAction::HandleRedo
//
//---------------------------------------------------------------
void XUndoAction::HandleRedo()
{
	IUndoActionHelperPtr helper(this);
	helper->OnRedo();
}


//---------------------------------------------------------------
//
// XUndoAction::GetText
//
//---------------------------------------------------------------
std::wstring XUndoAction::GetText() const
{
	PRECONDITION(mText.length() > 0);
	
	return mText;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XUndoAction::DoMerge
//
// Join the two actions iff we find the merge interface, the 
// bosses match, and the action OKs the merge.
//
//---------------------------------------------------------------
bool XUndoAction::DoMerge()
{
	bool merged = false;
	
	IMergeActionsPtr merge = mContext->GetUndoCommand();
	
	if (merge && merge->GetBoss()->GetName() == this->GetBoss()->GetName() && merge->CanMerge(this)) {
		merge->Merge(this);				// 'this' will be deleted when we return
		merged = true;
	}
		
	return merged;
}


//---------------------------------------------------------------
//
// XUndoAction::DoSequence
//
//---------------------------------------------------------------
bool XUndoAction::DoSequence()
{
	bool did = false;
	
	IBeginUndoSequencePtr begin(this);
	IEndUndoSequencePtr end(this);

	// If we hit the start of an undo sequence then create a new
	// multiple action command to hold the next set of commands.
	if (begin) {			
		IUndoActionPtr action(L"Multiple Action");
		action->Init(IUndoActionPtr(this)->GetText());

		msSequences.push_back(IMultipleUndoActionPtr(action));	// need IMultipleUndoActionPtr for Pro 5.3
		
		did = true;
	
	// If we've reached the end of the sequence we need to execute
	// the sequence and add it to the undo stack.
	} else if (end) {
		ASSERT(!msSequences.empty());
		
		IUndoActionHelperPtr helper = msSequences.back();
		msSequences.pop_back();
		helper->OnDo();			

		if (mContext)
			mContext->AddCommand(IUndoActionPtr(helper));	// need IUndoActionPtr for Pro 5.3
			
		did = true;
	
	// If we're in the middle of a sequence then add ourselves
	// to the multiple action command.
	} else if (!msSequences.empty()) {
		msSequences.back()->AddAction(this);
		did = true;
	}
			
	return did;
}


}	// namespace Whisper
