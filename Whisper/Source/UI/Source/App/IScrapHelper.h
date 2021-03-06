/*
 *  File:       IScrapHelper.h
 *  Summary:   	Helper interface used by XScrap.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IScrapHelper.h,v $
 *		Revision 1.1  2001/01/05 06:14:58  jesjones
 *		Refactored XScrap so that it uses an IScrapHelper interface.
 *		
 */

#pragma once

#include <XHandle.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IScrapHelper
//!		Helper interface used by XScrap.
// ===================================================================================
class IScrapHelper : public XUnknown {

//-----------------------------------
//	API
//
public:
	//! @name Operations
	//@{
	virtual XHandle 	Copy(uint32 type) = 0;

	virtual void	 	Paste(uint32 type, const XHandle& data) = 0;

	virtual void 		Clear() = 0;

	virtual bool 		HasSelection() const = 0;
	//@}

	//! @name Clipboard Types
	//@{
	virtual uint32 		GetCopyCount() const = 0;	
	virtual uint32		GetCopyType(uint32 index) const = 0;
						/**< Copy command will place all of these formats into the clipboard. */
	
	virtual uint32 		GetPasteCount() const = 0;	
	virtual uint32		GetPasteType(uint32 index) const = 0;
						/**< Paste command will iterate over all of these until it finds one
						of the formats in the clipboard. */
	//@}
};

typedef XInterfacePtr<IScrapHelper> IScrapHelperPtr;
typedef XInterfacePtr<const IScrapHelper> IConstScrapHelperPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
