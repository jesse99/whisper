/*
 *  File:       ISetDisabledHelp.h
 *  Summary:   	Interface used with IHelpText to set the help message for objects
 *				when they are disabled.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ISetDisabledHelp.h,v $
 *		Revision 1.4  2001/03/05 05:38:27  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:25:04  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:07:01  jesjones
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
//	class ISetDisabledHelp
//!		Interface used with IHelpText to set the help message for objects when they are disabled.
// ===================================================================================
class ISetDisabledHelp : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		SetDisabledHelp(const std::wstring& text) = 0;
};


typedef XInterfacePtr<ISetDisabledHelp> ISetDisabledHelpPtr;
typedef XInterfacePtr<const ISetDisabledHelp> IConstSetDisabledHelpPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
