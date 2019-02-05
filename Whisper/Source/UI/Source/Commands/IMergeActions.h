/*
 *  File:       IMergeActions.h
 *  Summary:   	Interface used to join two IUndoAction's together.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IMergeActions.h,v $
 *		Revision 1.1  2000/12/24 03:14:00  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class IUndoAction;
typedef XInterfacePtr<const IUndoAction> IConstUndoActionPtr;


// ===================================================================================
//	class IMergeActions
//!		Interface used to join two IUndoAction's together. 
/*!		This is useful for things like nudging an object with the arrow keys: nudges 
 *		in the same direction should be undone as a unit. */
// ===================================================================================
class IMergeActions : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual bool 		CanMerge(const IConstUndoActionPtr& newAction) const = 0;
						/**< After newAction is executed XUndoAction will check to see
						if the action on the top of the undo stack has the same boss as
						newAction. If the bosses match it will query for an IMergeActions
						interface and call CanMerge on the action on the top of the undo
						stack. True is returned if the action can update its state such
						that newAction is no longer needed to perform undo and redo. */

	virtual void 		Merge(const IConstUndoActionPtr& newAction) = 0;
						/**< This is where the action on the top of the undo stack updates
						its internal state using newAction's state. */
};

typedef XInterfacePtr<IMergeActions> IMergeActionsPtr;
typedef XInterfacePtr<const IMergeActions> IConstMergeActionsPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
