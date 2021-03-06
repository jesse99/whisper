/*
 *  File:       IGroupControl.h
 *  Summary:   	A control into which other controls can be nested. Unless you use a draw extension
 *				this will not draw anything (use IGroupBox if you want visuals).
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IGroupControl.h,v $
 *		Revision 1.4  2000/12/14 08:22:15  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/05 09:05:17  jesjones
 *		Tweaked the header comment.
 *		
 *		Revision 1.2  2000/11/09 09:02:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 7/12/00	JDJ		Created
 */

#pragma once

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
const uint32 kDefaultGroupProcID = kControlUserPaneProc; 
const uint32 kDefaultGroupStyle  = WS_CHILD + WS_VISIBLE + SS_NOTIFY;


// ===================================================================================
//	class IGroupControl
//!		A control into which other controls can be nested. Unless you use a draw extension this will not draw anything (use IGroupBox if you want visuals).
// ===================================================================================
class IGroupControl : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Initialization -----
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, uint32 style = kDefaultGroupStyle, 
							 uint32 procID = kDefaultGroupProcID) = 0;
};


typedef XInterfacePtr<IGroupControl> IGroupControlPtr;
typedef XInterfacePtr<const IGroupControl> IConstGroupControlPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
