/*
 *  File:       WInterfacePtr.cpp
 *  Summary:    Smart pointer that wraps a COM interface pointer.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WInterfacePtr.cpp,v $
 *		Revision 1.1  2001/03/17 07:34:36  jesjones
 *		Checked in
 *		
 */

#include <XWhisperHeader.h>
#include <WInterfacePtr.h>

namespace Whisper {


// ===================================================================================
//	class WInterfacePtr
// ===================================================================================

//---------------------------------------------------------------
//
// WInterfacePtr<IUnknown>::WInterfacePtr
//
//---------------------------------------------------------------
template<>
WInterfacePtr<IUnknown, &IID_IUnknown>::WInterfacePtr(const WInterfacePtr<IUnknown, &IID_IUnknown>& rhs)
{
	mPtr = rhs.mPtr;

	if (mPtr != nil)
		(void) mPtr->AddRef();
}


//---------------------------------------------------------------
//
// WInterfacePtr<IUnknown>::Reset
//
//---------------------------------------------------------------
template<>
IUnknown* WInterfacePtr<IUnknown, &IID_IUnknown>::Reset(IUnknown* takePtr)
{
	if (mPtr != takePtr) {
		if (takePtr != nil)
			(void) takePtr->AddRef();

		if (mPtr != nil)
			(void) mPtr->Release();

		mPtr = takePtr;
	}
	
	return mPtr;
}



}	// namespace Whisper
