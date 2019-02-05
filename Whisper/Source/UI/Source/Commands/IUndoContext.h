/*
 *  File:       IUndoContext.h
 *  Summary:   	A context for undo (each window and the app typically have one of these).
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IUndoContext.h,v $
 *		Revision 1.4  2001/03/05 05:38:48  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:25:48  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:07:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
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
typedef XInterfacePtr<IUndoAction> IUndoActionPtr;


// ===================================================================================
//	class IUndoContext
//!		A context for undo (each window and the app typically have one of these).
// ===================================================================================
class IUndoContext : public XUnknown {

//-----------------------------------
//	API
//
public:
	//! @name Undo
	//@{
	virtual bool		CanUndo() const = 0;
			
	virtual void		Undo() = 0;
			
	virtual std::wstring GetUndoText() const = 0;
	//@}
			
	//! @name Redo
	//@{
	virtual bool		CanRedo() const = 0;
			
	virtual void		Redo() = 0;
			
	virtual std::wstring GetRedoText() const = 0;
			
	virtual void		ClearRedoHistory() = 0;
	//@}

	//! @name History
	//@{
	virtual void		ClearHistory() = 0;
						/**< Clears the undo and redo stacks. */
			
	virtual void		SetMaxCommands(uint32 newMax) = 0;
						/**< Defaults to 32. */
			
	virtual IUndoActionPtr GetUndoCommand() const = 0;
						/**< Returns the command that will be executed when the user selects Undo.
						This is useful for accumulating commands (eg typing characters). */
			
	virtual IUndoActionPtr GetRedoCommand() const = 0;
						/**< Returns the command that will be executed when the user selects Redo.
						This is useful if you're a fan of symmetry. */
	//@}

//-----------------------------------
//	Internal API
//
public:
	virtual void		AddCommand(const IUndoActionPtr& action) = 0;
						
	virtual bool		HasCommand(const IUndoActionPtr& action) const = 0;
};

typedef XInterfacePtr<IUndoContext> IUndoContextPtr;
typedef XInterfacePtr<const IUndoContext> IConstUndoContextPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
