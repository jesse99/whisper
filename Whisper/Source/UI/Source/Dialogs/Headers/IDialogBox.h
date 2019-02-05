/*
 *  File:       IDialogBox.h
 *  Summary:   	Interface used with modal dialogs.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IDialogBox.h,v $
 *		Revision 1.1  2001/01/01 00:28:39  jesjones
 *		Moved from the Windows to the Dialog folder.
 *		
 *		Revision 1.2  2000/12/14 08:20:48  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.1  2000/12/05 09:01:33  jesjones
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


//-----------------------------------
//	Forward References
//
class IControl;
typedef XInterfacePtr<IControl> IControlPtr;


// ===================================================================================
//	class IDialogBox
//!		Interface used with modal dialogs.
// ===================================================================================
class IDialogBox : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual bool 		ValidateData() const = 0;
						/**< Returns true if all the dialog's controls are valid. If there's 
						a problem an alert is displayed. */
						
	virtual IControlPtr GetDefaultButton() const = 0;
	virtual IControlPtr GetCancelButton() const = 0;

	virtual void 		SetDefaultButton(const std::wstring& name) = 0;
	virtual void 		SetCancelButton(const std::wstring& name) = 0;
						/**< These are initialized to "ok" and "cancel". */
};

typedef XInterfacePtr<IDialogBox> IDialogBoxPtr;
typedef XInterfacePtr<const IDialogBox> IConstDialogBoxPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
