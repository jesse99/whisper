/*
 *  File:       IValidate.h
 *  Summary:   	Used by dialogs to ensure that all fields are valid before the dialog is dismissed.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IValidate.h,v $
 *		Revision 1.3  2000/12/19 07:49:04  jesjones
 *		Renamed IsValid Validate.
 *		
 *		Revision 1.2  2000/12/14 08:25:55  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.1  2000/12/05 09:01:51  jesjones
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
//	class IValidate
//!		Used by dialogs to ensure that all fields are valid before the dialog is dismissed.
// ===================================================================================
class IValidate : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual bool 		Validate() const = 0;
						/**< Returns true if the associated boss is valid. If it's not
						valid pops up an alert (and usually selects the offending
						widget). */
};


typedef XInterfacePtr<IValidate> IValidatePtr;
typedef XInterfacePtr<const IValidate> IConstValidatePtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
