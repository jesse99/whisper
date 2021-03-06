/*
 *  File:       IInit.h
 *  Summary:   	Generic initialization interface.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IInit.h,v $
 *		Revision 1.4  2001/02/07 02:25:40  jesjones
 *		Renamed methods to start with On.
 *		
 *		Revision 1.3  2000/12/14 08:22:45  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:02:44  jesjones
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
//	class IInit
//!		Generic initialization interface.
// ===================================================================================
class IInit : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		OnInit() = 0;
	virtual void 		OnCleanup() = 0;
};


typedef XInterfacePtr<IInit> IInitPtr;
typedef XInterfacePtr<const IInit> IConstInitPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
