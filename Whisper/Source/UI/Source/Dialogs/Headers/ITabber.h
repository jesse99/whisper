/*
 *  File:       ITabber.h
 *  Summary:   	Interface used with windows to tab between controls.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ITabber.h,v $
 *		Revision 1.1  2001/01/01 00:28:49  jesjones
 *		Moved from the Windows to the Dialog folder.
 *		
 *		Revision 1.1  2000/12/27 00:55:00  jesjones
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
//	class ITabber
//!		Interface used with windows to tab between controls.
// ===================================================================================
class ITabber : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual bool 		HandleTab(bool forward) = 0;
						/**< Makes the next or the preceding focusable control the focus.
						Return true if the tab key was handled. */
						
	virtual bool 		HandleArrowKey(int16 key) = 0;
						/**< Windows treats the arrow keys like the tab key. Usually does
						nothing on the Mac. Return true if the arrow key was handled. */
};

typedef XInterfacePtr<ITabber> ITabberPtr;
typedef XInterfacePtr<const ITabber> IConstTabberPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
