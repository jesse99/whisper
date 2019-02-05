/*
 *  File:       IRadioButton.h
 *  Summary:   	A control that displays a radio button.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IRadioButton.h,v $
 *		Revision 1.2  2000/12/14 08:24:31  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.1  2000/12/05 09:01:47  jesjones
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
const uint32 kDefaultRadioButtonProcID = kControlRadioButtonProc; 
const uint32 kDefaultRadioButtonStyle  = WS_CHILD + WS_VISIBLE + WS_TABSTOP + BS_RADIOBUTTON;	


// ===================================================================================
//	class IRadioButton
//!		A control that displays a radio button.
// ===================================================================================
class IRadioButton : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Initialization -----
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring& text,
							 const std::wstring& family, uint32 style = kDefaultRadioButtonStyle,
							 uint32 procID = kDefaultRadioButtonProcID) = 0;
						/**< Radio buttons normally have a value of 0 or 1, but they can 
						also have a value of 2 which denotes a mixed state (eg a radio
						button labeled "Left Justify" would use 2 if only part of the 
						selected text was left justified). Note that Win32 doesn't
						support mixed state radio buttons. Families are used to group 
						radio buttons: only one radio button can be on in a family. */

	// ----- State -----
	virtual bool		IsSelected() const = 0;
						/**< Returns true if the button is checked. Note that this will
						return true if the value is indeterminate. */

	virtual bool		IsIndeterminate() const = 0;
						/**< Returns true if the button in in the mixed state. Always
						returns false on Windows. */
};


typedef XInterfacePtr<IRadioButton> IRadioButtonPtr;
typedef XInterfacePtr<const IRadioButton> IConstRadioButtonPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
