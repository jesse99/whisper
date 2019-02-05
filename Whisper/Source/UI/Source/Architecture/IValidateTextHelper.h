/*
 *  File:       IValidateTextHelper.h
 *  Summary:   	Helper interface used by XValidateText.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IValidateTextHelper.h,v $
 *		Revision 1.1  2001/01/30 07:27:34  jesjones
 *		Checked in
 *		
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
//	class IValidateTextHelper
//!		Helper interface used by XValidateText.
// ===================================================================================
class IValidateTextHelper : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual bool 		OnValid(wchar_t ch) const = 0;

	virtual bool 		OnValidate(const std::wstring& text, bool showAlert) const = 0;
};


typedef XInterfacePtr<IValidateTextHelper> IValidateTextHelperPtr;
typedef XInterfacePtr<const IValidateTextHelper> IConstValidateTextHelperPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
