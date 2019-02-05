/*
 *  File:       ISeperator.h
 *  Summary:   	A control that draws a divider line.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ISeperator.h,v $
 *		Revision 1.1  2001/03/23 10:49:54  jesjones
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
const uint32 kDefaultSeperatorProcID = kControlSeparatorLineProc; 				//!< automatically switches to vertical if the control is taller than it is wide
const uint32 kDefaultSeperatorStyle  = WS_CHILD + WS_VISIBLE + SS_ETCHEDHORZ;	


// ===================================================================================
//	class ISeperator
//!		A control that draws a divider line.
// ===================================================================================
class ISeperator : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Initialization -----
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, uint32 style = kDefaultSeperatorStyle,
							 uint32 procID = kDefaultSeperatorProcID) = 0;
};


typedef XInterfacePtr<ISeperator> ISeperatorPtr;
typedef XInterfacePtr<const ISeperator> IConstSeperatorPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
