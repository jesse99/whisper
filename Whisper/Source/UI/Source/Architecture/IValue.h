/*
 *  File:       IValue.h
 *  Summary:   	Template interface used to get values into and out of a boss.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IValue.h,v $
 *		Revision 1.2  2001/02/28 09:55:28  jesjones
 *		Added IFloatValue.
 *		
 *		Revision 1.1  2000/12/19 07:44:22  jesjones
 *		Checked in	
 */

#pragma once

#include <XConstants.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IValue
//!		Template interface used to get values into and out of a boss.
// ===================================================================================
template <typename T>
class IValue : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual T 			Get() const = 0;
	
	virtual void 		Set(T value, bool redraw = kRedraw) = 0;
};


typedef XInterfacePtr<IValue<bool> > IBoolValuePtr;
typedef XInterfacePtr<const IValue<bool> > IConstBoolValuePtr;

typedef XInterfacePtr<IValue<int32> > IInt32ValuePtr;
typedef XInterfacePtr<const IValue<int32> > IConstInt32ValuePtr;

typedef XInterfacePtr<IValue<uint32> > IUInt32ValuePtr;
typedef XInterfacePtr<const IValue<uint32> > IConstUInt32ValuePtr;

typedef XInterfacePtr<IValue<float> > IFloatValuePtr;
typedef XInterfacePtr<const IValue<float> > IConstFloatValuePtr;

typedef XInterfacePtr<IValue<double> > IDoubleValuePtr;
typedef XInterfacePtr<const IValue<double> > IConstDoubleValuePtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
