/*
 *	File:		IPen.h
 *	Function:	Interface that provides access to an XPen.
 *	Written by:	Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: IPen.h,v $
 *		Revision 1.1  2001/02/07 05:01:27  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XInterfacePtr.h>
#include <XPen.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IPen
//!		Interface that provides access to an XPen.
// ===================================================================================
class IPen : public XUnknown {
				
//-----------------------------------
//	API
//
public:
	virtual XPen 		GetEnabledPen() const = 0;
	virtual void 		SetEnabledPen(const XPen& pen) = 0;
						/**< Defaults to kBlackPen. */

	virtual XPen 		GetDisabledPen() const = 0;
	virtual void 		SetDisabledPen(const XPen& pen) = 0;
						/**< Defaults to kGrayPen. */
};


typedef XInterfacePtr<IPen> IPenPtr;
typedef XInterfacePtr<const IPen> IConstPenPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
