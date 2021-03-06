/*
 *  File:       IFocus.h
 *  Summary:   	Interface for objects that are sent keystrokes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IFocus.h,v $
 *		Revision 1.4  2001/01/30 07:11:16  jesjones
 *		Added HasFocus.
 *		
 *		Revision 1.3  2000/12/14 08:21:57  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:02:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IFocus
//!		Interface for objects that are sent keystrokes.
// ===================================================================================
class IFocus : public XUnknown {

//-----------------------------------
//	API
//
public:					
	virtual bool 		WantsFocus() const = 0;

	virtual void		SetFocus() = 0;
						/**< Sets the focus to 'this'. Note that this does not give the 
						current focus a chance to refuse the focus switch. */
						
	virtual bool 		HasFocus() const = 0;
						/**< Returns true if 'this' is the current focus. */
};

typedef XInterfacePtr<IFocus> IFocusPtr;
typedef XInterfacePtr<const IFocus> IConstFocusPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
