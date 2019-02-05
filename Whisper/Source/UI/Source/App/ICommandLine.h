/*
 *  File:       ICommandLine.h
 *  Summary:   	An object used to initialize the app.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ICommandLine.h,v $
 *		Revision 1.4  2001/03/05 05:37:02  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:20:24  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:00:32  jesjones
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
//	class ICommandLine
//!		An object used to initialize the app.
// ===================================================================================
class ICommandLine : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		OnCommandLine(const std::vector<std::wstring>& args) = 0;
};

typedef XInterfacePtr<ICommandLine> ICommandLinePtr;
typedef XInterfacePtr<const ICommandLine> IConstCommandLinePtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

