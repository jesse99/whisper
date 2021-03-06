/*
 *  File:       IGroupBox.h
 *  Summary:   	A control that displays a labeled frame around it's children.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IGroupBox.h,v $
 *		Revision 1.2  2000/12/14 08:22:10  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.1  2000/12/05 09:01:36  jesjones
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
const uint32 kDefaultGroupBoxProcID   = kControlGroupBoxTextTitleProc; 
const uint32 kSecondaryGroupBoxProcID = kControlGroupBoxSecondaryTextTitleProc; 

const uint32 kDefaultGroupBoxStyle = WS_CHILD + WS_VISIBLE + BS_GROUPBOX;	


// ===================================================================================
//	class IGroupBox
//!		A control that displays a labeled frame around it's children.
// ===================================================================================
class IGroupBox : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Initialization -----
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring& text,
							 uint32 style = kDefaultGroupBoxStyle,
							 uint32 procID = kDefaultGroupBoxProcID) = 0;
};


typedef XInterfacePtr<IGroupBox> IGroupBoxPtr;
typedef XInterfacePtr<const IGroupBox> IConstGroupBoxPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
