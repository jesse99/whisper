/*
 *  File:       ICheckBox.h
 *  Summary:   	A control that displays a checkbox.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ICheckBox.h,v $
 *		Revision 1.2  2000/12/14 08:20:01  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.1  2000/12/05 09:01:30  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XConstants.h>
#include <XControlConstants.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>
#include <XWindowConstants.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XRect;

class IHierarchy;
typedef XInterfacePtr<IHierarchy> IHierarchyPtr;


//-----------------------------------
//	Constants
//
const uint32 kDefaultCheckBoxProcID = kControlCheckBoxProc; 
const uint32 kDefaultCheckBoxStyle  = WS_CHILD + WS_VISIBLE + WS_TABSTOP + BS_3STATE;	


// ===================================================================================
//	class ICheckBox
//!		A control that displays a checkbox.
// ===================================================================================
class ICheckBox : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Initialization -----
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring& text,
							 uint32 style = kDefaultCheckBoxStyle,
							 uint32 procID = kDefaultCheckBoxProcID) = 0;

	// ----- State -----
	virtual bool		IsChecked() const = 0;
						/**< Returns true if the button is checked. Note that this will
						return true if the value is indeterminate. */

	virtual bool		IsIndeterminate() const = 0;
						/**< Returns true if the button in in the mixed state. */
};


typedef XInterfacePtr<ICheckBox> ICheckBoxPtr;
typedef XInterfacePtr<const ICheckBox> IConstCheckBoxPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
