/*
 *  File:       ICurrentFocus.h
 *  Summary:   	Interface that returns the object that gets first crack at keyboard events.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ICurrentFocus.h,v $
 *		Revision 1.1  2001/01/30 07:27:24  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <IFocus.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class ICurrentFocus
//!		Interface that returns the object that gets first crack at keyboard events.
// ===================================================================================
class ICurrentFocus : public XUnknown {

//-----------------------------------
//	API
//
public:					
	virtual IFocusPtr 	GetFocus() const = 0;
						/**< Note that this will always return a valid focus. */
};

typedef XInterfacePtr<ICurrentFocus> ICurrentFocusPtr;
typedef XInterfacePtr<const ICurrentFocus> IConstCurrentFocusPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
