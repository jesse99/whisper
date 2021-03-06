/*
 *  File:       IValueChanged.h
 *  Summary:   	Interface used to signal that a control's value has changed.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IValueChanged.h,v $
 *		Revision 1.1  2001/02/07 02:32:04  jesjones
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
//	class IValueChanged
//!			Interface used to signal that a control's value has changed.
// ===================================================================================
class IValueChanged : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		OnValueChanged(int32 newValue) = 0;
};


typedef XInterfacePtr<IValueChanged> IValueChangedPtr;
typedef XInterfacePtr<const IValueChanged> IConstValueChangedPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
