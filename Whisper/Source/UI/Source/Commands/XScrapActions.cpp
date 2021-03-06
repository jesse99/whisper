/*
 *  File:       XUndoActions.cpp
 *  Summary:   	IUndoAction's that work with IScrap and IMemento.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XScrapActions.cpp,v $
 *		Revision 1.3  2001/01/30 07:24:21  jesjones
 *		Added ICommandHelper and IUndoActionHelper.
 *		
 *		Revision 1.2  2000/12/24 03:25:59  jesjones
 *		Added copy, paste, and clear commands.
 *		
 *		Revision 1.1  2000/12/22 09:31:46  jesjones
 *		Checked in	
 */

#include <XWhisperHeader.h>
#include <IUndoActionHelper.h>

#include <IInstance.h>
#include <IMemento.h>
#include <IScrap.h>
#include <IUndoActions.h>

namespace Whisper {


// ===================================================================================
//	class XCutAction
//!		IUndoAction implementation that uses IScrap and IMemento.
// ===================================================================================
class XCutAction : public IUndoActionHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XCutAction();
	
						XCutAction(XBoss* boss);
						
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
	SMementoPtr	mOldState;
};

DEFINE_INTERFACE_FACTORY(XCutAction)

//---------------------------------------------------------------
//
// XCutAction::~XCutAction
//
//---------------------------------------------------------------
XCutAction::~XCutAction()
{
}


//---------------------------------------------------------------
//
// XCutAction::XCutAction
//
//---------------------------------------------------------------
XCutAction::XCutAction(XBoss* boss) 
{
	IUndoActionHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XCutAction::OnDo
//
//---------------------------------------------------------------
void XCutAction::OnDo()
{
	IInstancePtr instance(this);
	XUnknownPtr object = instance->Get();
	
	IConstMementoPtr memento(object);
	IScrapPtr scrap(object);
	
	mOldState = memento->GetState(); 
	scrap->Cut();
}


//---------------------------------------------------------------
//
// XCutAction::OnUndo
//
//---------------------------------------------------------------
void XCutAction::OnUndo()
{
	IInstancePtr instance(this);
	XUnknownPtr object = instance->Get();
	
	IMementoPtr memento(object);
	memento->SetState(mOldState);
}


//---------------------------------------------------------------
//
// XCutAction::OnRedo
//
//---------------------------------------------------------------
void XCutAction::OnRedo()
{
	IInstancePtr instance(this);
	XUnknownPtr object = instance->Get();
	
	IMementoPtr memento(object);
	IScrapPtr scrap(object);
	
	memento->SetState(mOldState); 
	
	scrap->Cut();
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XCopyAction
//!		IUndoAction implementation that uses IScrap and IMemento.
// ===================================================================================
class XCopyAction : public IUndoActionHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XCopyAction();
	
						XCopyAction(XBoss* boss);
						
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
	SMementoPtr	mOldState;
};

DEFINE_INTERFACE_FACTORY(XCopyAction)

//---------------------------------------------------------------
//
// XCopyAction::~XCopyAction
//
//---------------------------------------------------------------
XCopyAction::~XCopyAction()
{
}


//---------------------------------------------------------------
//
// XCopyAction::XCopyAction
//
//---------------------------------------------------------------
XCopyAction::XCopyAction(XBoss* boss)
{
	IUndoActionHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XCopyAction::OnDo
//
//---------------------------------------------------------------
void XCopyAction::OnDo()
{
	IInstancePtr instance(this);
	XUnknownPtr object = instance->Get();
	
	IConstMementoPtr memento(object);
	IScrapPtr scrap(object);
	
	mOldState = memento->GetState(); 
	scrap->Copy();
}


//---------------------------------------------------------------
//
// XCopyAction::OnUndo
//
//---------------------------------------------------------------
void XCopyAction::OnUndo()
{
	IInstancePtr instance(this);
	XUnknownPtr object = instance->Get();
	
	IMementoPtr memento(object);
	memento->SetState(mOldState);
}


//---------------------------------------------------------------
//
// XCopyAction::OnRedo
//
//---------------------------------------------------------------
void XCopyAction::OnRedo()
{
	IInstancePtr instance(this);
	XUnknownPtr object = instance->Get();
	
	IMementoPtr memento(object);
	IScrapPtr scrap(object);
	
	memento->SetState(mOldState); 
	
	scrap->Copy();
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XPasteAction
//!		IUndoAction implementation that uses IScrap and IMemento.
// ===================================================================================
class XPasteAction : public IUndoActionHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XPasteAction();
	
						XPasteAction(XBoss* boss);
						
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
	SMementoPtr	mOldState;
	SMementoPtr	mNewState;
};

DEFINE_INTERFACE_FACTORY(XPasteAction)

//---------------------------------------------------------------
//
// XPasteAction::~XPasteAction
//
//---------------------------------------------------------------
XPasteAction::~XPasteAction()
{
}


//---------------------------------------------------------------
//
// XPasteAction::XPasteAction
//
//---------------------------------------------------------------
XPasteAction::XPasteAction(XBoss* boss)
{
	IUndoActionHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XPasteAction::OnDo
//
//---------------------------------------------------------------
void XPasteAction::OnDo()
{
	IInstancePtr instance(this);
	XUnknownPtr object = instance->Get();
	
	IConstMementoPtr memento(object);
	IScrapPtr scrap(object);
	
	mOldState = memento->GetState(); 
	scrap->Paste();
	mNewState = memento->GetState(); 
}


//---------------------------------------------------------------
//
// XPasteAction::OnUndo
//
//---------------------------------------------------------------
void XPasteAction::OnUndo()
{
	IInstancePtr instance(this);
	XUnknownPtr object = instance->Get();
	
	IMementoPtr memento(object);
	memento->SetState(mOldState);
}


//---------------------------------------------------------------
//
// XPasteAction::OnRedo
//
//---------------------------------------------------------------
void XPasteAction::OnRedo()
{
	IInstancePtr instance(this);
	XUnknownPtr object = instance->Get();
	
	IMementoPtr memento(object);
	memento->SetState(mNewState);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XClearAction
//!		IUndoAction implementation that uses IScrap and IMemento.
// ===================================================================================
class XClearAction : public IUndoActionHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XClearAction();
	
						XClearAction(XBoss* boss);
						
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
	SMementoPtr	mOldState;
	SMementoPtr	mNewState;
};

DEFINE_INTERFACE_FACTORY(XClearAction)

//---------------------------------------------------------------
//
// XClearAction::~XClearAction
//
//---------------------------------------------------------------
XClearAction::~XClearAction()
{
}


//---------------------------------------------------------------
//
// XClearAction::XClearAction
//
//---------------------------------------------------------------
XClearAction::XClearAction(XBoss* boss)
{
	IUndoActionHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XClearAction::OnDo
//
//---------------------------------------------------------------
void XClearAction::OnDo()
{
	IInstancePtr instance(this);
	XUnknownPtr object = instance->Get();
	
	IConstMementoPtr memento(object);
	IScrapPtr scrap(object);
	
	mOldState = memento->GetState(); 
	scrap->Clear();
	mNewState = memento->GetState(); 
}


//---------------------------------------------------------------
//
// XClearAction::OnUndo
//
//---------------------------------------------------------------
void XClearAction::OnUndo()
{
	IInstancePtr instance(this);
	XUnknownPtr object = instance->Get();
	
	IMementoPtr memento(object);
	memento->SetState(mOldState);
}


//---------------------------------------------------------------
//
// XClearAction::OnRedo
//
//---------------------------------------------------------------
void XClearAction::OnRedo()
{
	IInstancePtr instance(this);
	XUnknownPtr object = instance->Get();
	
	IMementoPtr memento(object);
	memento->SetState(mNewState);
}


}	// namespace Whisper
