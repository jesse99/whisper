/*
 *  File:       IRootControl.h
 *  Summary:   	The topmost control in a window.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IRootControl.h,v $
 *		Revision 1.4  2001/03/05 05:38:18  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:24:42  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:06:37  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
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
class IHierarchy;

typedef XInterfacePtr<IControl> IControlPtr;
typedef XInterfacePtr<IHierarchy> IHierarchyPtr;


// ===================================================================================
//	class IRootControl
//!		The topmost control in a window.
// ===================================================================================
class IRootControl : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Initialization -----
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name) = 0;

	// ----- Misc -----
	virtual IControlPtr FindChild(const std::wstring& name) const = 0;
						/**< May return nil. */

#if MAC
	virtual void 		AddedChild(const IControlPtr& child) = 0;
	virtual void 		RemovedChild(const IControlPtr& child) = 0;
#endif
};


typedef XInterfacePtr<IRootControl> IRootControlPtr;
typedef XInterfacePtr<const IRootControl> IConstRootControlPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
