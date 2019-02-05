/*
 *  File:       IUndoActions.h
 *  Summary:   	Some handy undo actions.
 *  Written by: Jesse Jones
 *
 *	Classes:	SUndoSequence		- Automatically groups posted commands into an IMultipleUndoAction.
 *				IMultipleUndoAction - Executes undoable actions as a group/transaction.
 *				IUndoCallbackAction	- Uses XCallback objects to perform the do and undo actions.
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IUndoActions.h,v $
 *		Revision 1.8  2001/03/28 09:41:02  jesjones
 *		Added a comment.
 *		
 *		Revision 1.7  2001/03/27 06:30:42  jesjones
 *		Added SUndoSequence.
 *		
 *		Revision 1.6  2001/02/17 09:27:59  jesjones
 *		Removed UI_EXPORT.
 *		
 *		Revision 1.5  2001/01/01 03:48:07  jesjones
 *		Added IUndoCallbackAction::Post methods.
 *		
 *		Revision 1.4  2000/12/25 00:33:07  jesjones
 *		Fixed the initial checkin comment.
 *		
 *		Revision 1.3  2000/12/14 08:25:41  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:07:39  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <IUndoAction.h>
#include <XCallbacks.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	struct SUndoSequence
//!		Automatically groups posted commands into an IMultipleUndoAction.
// ===================================================================================
struct UI_EXPORT SUndoSequence {
						~SUndoSequence();
	
						SUndoSequence(const std::wstring& text);
						/**< Commands that are posted while this is active are automatically
						grouped in an IMultipleUndoAction. Note that sequences can be nested. */
};


// ===================================================================================
//	class IBeginUndoSequence
//!		Used by SUndoSequence to mark the start of a sequence.
// ===================================================================================
class IBeginUndoSequence : public XUnknown {
};


// ===================================================================================
//	class IEndUndoSequence
//!		Used by SUndoSequence to mark the end of a sequence.
// ===================================================================================
class IEndUndoSequence : public XUnknown {
};


// ===================================================================================
//	class IMultipleUndoAction
//!		Executes undoable actions as a group/transaction.
// ===================================================================================
class IMultipleUndoAction : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		AddAction(const IUndoActionPtr& action) = 0;
						/**< This acts like a transaction: ie if an exception is thrown 
						in HandleDo any actions that were successfully executed are
						undone and the exception is rethrown. */
};


// ===================================================================================
//	class IUndoCallbackAction
//!		Uses XCallback objects to perform the do and undo actions.
// ===================================================================================
class IUndoCallbackAction : public XUnknown {

//-----------------------------------
//	Types
//
public:	
	typedef XCallback0<void> Callback;

//-----------------------------------
//	Initialization
//
public:	
	virtual void 		SetCallbacks(const Callback& doIt, const Callback& undoIt) = 0;
	virtual void 		SetCallbacks(const Callback& doIt, const Callback& undoIt, const Callback& redoIt) = 0;
	
	static void 		Post(const std::wstring& message, const Callback& doIt, const Callback& undoIt);
	static void 		Post(const std::wstring& message, const Callback& doIt, const Callback& undoIt, const Callback& redoIt);
						/**< Helper functions that create a new undo action, call SetCallbacks,
						and post the new action to the event queue. */
};

typedef XInterfacePtr<IBeginUndoSequence> IBeginUndoSequencePtr;
typedef XInterfacePtr<const IBeginUndoSequence> IConstBeginUndoSequencePtr;

typedef XInterfacePtr<IEndUndoSequence> IEndUndoSequencePtr;
typedef XInterfacePtr<const IEndUndoSequence> IConstEndUndoSequencePtr;

typedef XInterfacePtr<IMultipleUndoAction> IMultipleUndoActionPtr;
typedef XInterfacePtr<const IMultipleUndoAction> IConstMultipleUndoActionPtr;

typedef XInterfacePtr<IUndoCallbackAction> IUndoCallbackActionPtr;
typedef XInterfacePtr<const IUndoCallbackAction> IConstUndoCallbackActionPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
