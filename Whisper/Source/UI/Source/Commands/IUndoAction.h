/*
 *  File:       IUndoAction.h
 *  Summary:   	Interface for undoable "actions".
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IUndoAction.h,v $
 *		Revision 1.4  2000/12/24 03:17:51  jesjones
 *		Removed UI_EXPORT.
 *		
 *		Revision 1.3  2000/12/14 08:25:37  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:07:35  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	12/26/97	JDJ		Created (from Raven)
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
class IUndoContext;
typedef XInterfacePtr<IUndoContext> IUndoContextPtr;


// ===================================================================================
//	class IUndoAction
//!		Interface for undoable "actions".
// ===================================================================================
class IUndoAction : public XUnknown {

//-----------------------------------
//	Initialization
//
public:
	virtual void 		Init(const std::wstring& text) = 0;
						/**< Uses the current undo context (normally a window's or the
						app's if no window is active). Call ICommandQueue::Post
						to execute the command associated with the action. */

	virtual void 		Init(const std::wstring& text, const IUndoContextPtr& context) = 0;

//-----------------------------------
//	API
//
public:
	virtual void 		HandleDo() = 0;

	virtual void		HandleUndo() = 0;
	
	virtual void		HandleRedo() = 0;
		
	virtual std::wstring GetText() const = 0;
						/**< The string appended to the undo/redo menu item. */
};

typedef XInterfacePtr<IUndoAction> IUndoActionPtr;
typedef XInterfacePtr<const IUndoAction> IConstUndoActionPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
