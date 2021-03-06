/*
 *  File:       IBinding.h
 *  Summary:   	Used with IParentChangedSize to affect how a child's size changes after
 *				it's parent's size changes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IBinding.h,v $
 *		Revision 1.4  2001/03/05 05:36:52  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:19:48  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 08:59:47  jesjones
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
//	struct SBinding
//!		Struct used by IBinding.
// ===================================================================================
struct UI_EXPORT SBinding {
	bool	left;
	bool	top;
	bool	right;
	bool	bottom;
	
		SBinding()									{left = top = right = bottom = true;}
		SBinding(bool l, bool t, bool r, bool b)	{left = l; top = t; right = r; bottom = b;}
};


// ===================================================================================
//	class IBinding
//!		Used with IParentChangedSize to affect how a child's size changes after it's parent's size changes.
// ===================================================================================
class IBinding : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual SBinding 	GetBinding() const = 0;
						/**< The binding affects what happens when the parent changes
						sizes. If an edge in the binding is set that edge will stay
						the same distance from the parent as it resizes. The
						bindings all default to false so the control doesn't move
						or resize when it's parent is changed. */

	virtual void 		SetBinding(const SBinding& binding) = 0;
};


typedef XInterfacePtr<IBinding> IBindingPtr;
typedef XInterfacePtr<const IBinding> IConstBindingPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
