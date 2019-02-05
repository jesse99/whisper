/*
 *  File:       IScrap.h
 *  Summary:   	Interface for objects that support the standard clipboard operations.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IScrap.h,v $
 *		Revision 1.1  2000/12/22 09:31:30  jesjones
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
//	class IScrap
//!		Interface for objects that support the standard clipboard operations.
// ===================================================================================
class IScrap : public XUnknown {

//-----------------------------------
//	API
//
public:
	//! @name Operations
	//@{
	virtual void 		Cut() = 0;
						/**< Note that these methods execute the operation immediately.
						If you want the operation to be undoable use HandleMenuCommand
						and pass in kCutCmd, kCopyCmd, kPasteCmd, or kClearCmd. */
	
	virtual void 		Copy() = 0;
	
	virtual void 		Paste() = 0;
	
	virtual void 		Clear() = 0;
	//@}

	//! @name Availability
	//@{
	virtual bool 		CanCut() const = 0;
						/**< Returns true if can copy and clear. */

	virtual bool 		CanCopy() const = 0;
						/**< Returns true if there's a selection. */
	
	virtual bool 		CanPaste() const = 0;
						/**< Returns true if there's an entry in the scrap the object can use. */

	virtual bool 		CanClear() const = 0;
						/**< Returns true if there's a selection. */
	
	virtual bool 		HasSelection() const = 0;
	//@}
};

typedef XInterfacePtr<IScrap> IScrapPtr;
typedef XInterfacePtr<const IScrap> IConstScrapPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
