/*
 *  File:       IValidateText.h
 *  Summary:   	Used to reject invalid user input.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IValidateText.h,v $
 *		Revision 1.1  2000/12/19 07:44:19  jesjones
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
//	class IValidateText
//!		Used to reject invalid user input.
// ===================================================================================
class IValidateText : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Validation -----
	virtual bool 		IsValid(wchar_t ch) const = 0;
	
	virtual bool 		IsValid(const std::wstring& text) const = 0;
						
	virtual bool 		Validate(const std::wstring& text) const = 0;
						/**< Returns true if the associated boss is valid. If it's not
						valid pops up an alert. */

	// ----- Min and Max Characters -----
	virtual uint32 		GetMinChars() const = 0;
	virtual void 		SetMinChars(uint32 min) = 0;

	virtual uint32 		GetMaxChars() const = 0;	
	virtual void 		SetMaxChars(uint32 max) = 0;
};


typedef XInterfacePtr<IValidateText> IValidateTextPtr;
typedef XInterfacePtr<const IValidateText> IConstValidateTextPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
