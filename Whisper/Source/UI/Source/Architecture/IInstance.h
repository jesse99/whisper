/*
 *  File:       IInstance.h
 *  Summary:   	Interface that holds a reference to another boss.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IInstance.h,v $
 *		Revision 1.1  2000/12/22 09:31:24  jesjones
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


// ===================================================================================
//	class IInstance
//!		Interface that holds a reference to another boss.
// ===================================================================================
class IInstance : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual XUnknownPtr Get() const = 0;

	virtual void 		Set(const XUnknownPtr& instance) = 0;
};


typedef XInterfacePtr<IInstance> IInstancePtr;
typedef XInterfacePtr<const IInstance> IConstInstancePtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
