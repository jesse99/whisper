/*
 *  File:       ISetHelp.h
 *  Summary:   	Interface used with IHelpText to set the default help message.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ISetHelp.h,v $
 *		Revision 1.4  2001/03/05 05:38:31  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:25:08  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:07:04  jesjones
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
//	class ISetHelp
//!		Interface used with IHelpText to set the default help message.
// ===================================================================================
class ISetHelp : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		SetDefaultHelp(const std::wstring& text) = 0;
};


typedef XInterfacePtr<ISetHelp> ISetHelpPtr;
typedef XInterfacePtr<const ISetHelp> IConstSetHelpPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
