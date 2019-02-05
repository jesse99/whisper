/*
 *  File:       IUserControl.h
 *  Summary:   	Creates a control for custom controls.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IUserControl.h,v $
 *		Revision 1.4  2001/03/05 05:38:52  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:25:51  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:07:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XInterfacePtr.h>
#include <XControlConstants.h>
#include <XUnknown.h>
#include <XWindowConstants.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class IHierarchy;
class XRect;
typedef XInterfacePtr<IHierarchy> IHierarchyPtr;


//-----------------------------------
//	Constants
//
const uint32 kDefaultUserProcID = kControlUserPaneProc; 
const uint32 kDefaultUserStyle  = WS_CHILD + WS_VISIBLE + SS_NOTIFY + SS_OWNERDRAW;


// ===================================================================================
//	class IUserControl
//!		Creates a control for custom controls.
// ===================================================================================
class IUserControl : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, uint32 style = kDefaultUserStyle,
							 uint32 procID = kDefaultUserProcID) = 0;
};


typedef XInterfacePtr<IUserControl> IUserControlPtr;
typedef XInterfacePtr<const IUserControl> IConstUserControlPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
