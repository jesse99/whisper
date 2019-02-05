/*
 *  File:       IMemento.h
 *  Summary:   	Interface used to save and restore an object's state.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IMemento.h,v $
 *		Revision 1.2  2000/12/24 03:16:53  jesjones
 *		Added CanMerge to SMemento.
 *		
 *		Revision 1.1  2000/12/22 09:31:27  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XCountedPtr.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	struct SMemento
//!		Abstract base class for memento's returned by IMemento.
// ===================================================================================
struct UI_EXPORT SMemento 
{
	virtual 			~SMemento() = 0;
	
	virtual bool 		CanMerge(const SMemento* oldState, const XConstUnknownPtr& object) const;
						/**< Used by XMergeActions to decide if two IUndoAction's can
						be joined together. 'this' is the new state for the action on
						the top of the undo stack, oldState is the old state for the
						newly executed action. Default returns false. Note that 'this'
						and oldState are guaranteed to be the exact same type. */ 
};

typedef XCountedPtr<SMemento> SMementoPtr;


// ===================================================================================
//	class IMemento
//!		Interface used to save and restore an object's state.
// ===================================================================================
class IMemento : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual SMementoPtr GetState() const = 0;
						/**< Returns an SMemento subclass that contains everything required
						to restore the object's state. */
	
	virtual void 		SetState(const SMementoPtr& memento) = 0;
};

typedef XInterfacePtr<IMemento> IMementoPtr;
typedef XInterfacePtr<const IMemento> IConstMementoPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
