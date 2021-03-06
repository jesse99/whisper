/*
 *  File:       IChildNode.h
 *  Summary:   	Interface that provides a hook that is called when nodes are added
 *				and removed from IHierarchy parent nodes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IChildNode.h,v $
 *		Revision 1.4  2001/02/09 04:31:14  jesjones
 *		Renamed methods to start with On.
 *		
 *		Revision 1.3  2000/12/14 08:20:05  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:00:06  jesjones
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
//	class IChildNode
//!		Interface that provides a hook that is called when nodes are added and removed from IHierarchy parent nodes.
// ===================================================================================
class IChildNode : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		OnAdopted() = 0;
						/**< XHierarchy::AddChild calls this for each child implementation
						after adding the child. */
						
	virtual void 		OnOrphaned() = 0;
						/**< XHierarchy::RemoveChild calls this for each child implementation
						before removing the child. */
};

typedef XInterfacePtr<IChildNode> IChildNodePtr;
typedef XInterfacePtr<const IChildNode> IConstChildNodePtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

