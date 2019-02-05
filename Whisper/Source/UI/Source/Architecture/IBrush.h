/*
 *	File:		IBrush.h
 *	Function:	Interface that provides access to an XBrush.
 *	Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: IBrush.h,v $
 *		Revision 1.1  2001/02/07 05:01:18  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XBrush.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IBrush
//!		Interface that provides access to an XBrush.
// ===================================================================================
class IBrush : public XUnknown {
				
//-----------------------------------
//	API
//
public:
	virtual void 		SetEnabledBrush(const XBrush& brush) = 0;
	virtual XBrush 		GetEnabledBrush() const = 0;
						/**< Defaults to kWhiteBrush. */

	virtual void 		SetDisabledBrush(const XBrush& brush) = 0;
	virtual XBrush 		GetDisabledBrush() const = 0;
						/**< Defaults to kWhiteBrush. */
};


typedef XInterfacePtr<IBrush> IBrushPtr;
typedef XInterfacePtr<const IBrush> IConstBrushPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
