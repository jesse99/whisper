/*
 *  File:       IUndoActionHelper.h
 *  Summary:   	Helper interface used by XUndoAction.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IUndoActionHelper.h,v $
 *		Revision 1.1  2001/01/30 07:27:31  jesjones
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


// ===================================================================================
//	class IUndoActionHelper
//!		Helper interface used by XUndoAction.
// ===================================================================================
class IUndoActionHelper : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		OnDo() = 0;

	virtual void 		OnUndo() = 0;

	virtual void 		OnRedo() = 0;
};

typedef XInterfacePtr<IUndoActionHelper> IUndoActionHelperPtr;
typedef XInterfacePtr<const IUndoActionHelper> IConstUndoActionHelperPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
