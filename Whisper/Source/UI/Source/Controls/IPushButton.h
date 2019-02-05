/*
 *  File:       IPushButton.h
 *  Summary:   	The standard push button control.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IPushButton.h,v $
 *		Revision 1.4  2000/12/14 08:24:26  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/05 09:06:13  jesjones
 *		Added SimulateClick and some methods for default and cancel buttons.
 *		
 *		Revision 1.2  2000/11/09 09:06:13  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 7/06/00	JDJ		Created
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
const uint32 kDefaultPushButtonProcID = kControlPushButtonProc; 
const uint32 kDefaultPushButtonStyle  = WS_CHILD + WS_VISIBLE + WS_TABSTOP + BS_PUSHBUTTON;


// ===================================================================================
//	class IPushButton
//!		The standard push button control.
// ===================================================================================
class IPushButton : public XUnknown {

//-----------------------------------
//	API
//
public:
	//! @name Initialization
	//@{
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring& text, 
							 uint32 style = kDefaultPushButtonStyle,
							 uint32 procID = kDefaultPushButtonProcID) = 0;

#if MAC
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring& text, 
							 ResID iconID,  uint32 style, uint32 procID) = 0;
						/**< procID should be kControlPushButLeftIconProc or kControlPushButRightIconProc. */
#endif
	//@}
	
	//! @name Default Button
	//@{
	virtual bool 		IsDefaultButton() const = 0;
			
	virtual void 		MakeDefaultButton(bool make = true) = 0;
	//@}

	//! @name Cancel Button
	//@{
	virtual bool 		IsCancelButton() const = 0;
			
	virtual void 		MakeCancelButton(bool make = true) = 0;
	//@}

	//! @name Misc
	//@{
	virtual void 		SimulateClick() = 0;
						/**< Acts as if the user clicked on the button. */
	//@}
};

typedef XInterfacePtr<IPushButton> IPushButtonPtr;
typedef XInterfacePtr<const IPushButton> IConstPushButtonPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
