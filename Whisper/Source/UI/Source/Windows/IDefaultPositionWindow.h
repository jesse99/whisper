/*
 *  File:       IDefaultPositionWindow.h
 *  Summary:   	Used to control how the default IPositionWindow implementation behaves.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IDefaultPositionWindow.h,v $
 *		Revision 1.4  2001/03/05 05:37:12  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:20:44  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:00:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Constants
//
enum EAutoPosition {kDontAutoPosition = 1, 
					kCenterOnMainMonitor = 3, kAlertPositionOnMainMonitor = 4, kStaggerOnMainMonitor = 5,
					kCenterOnTopWindow = 7, kAlertPositionOnTopWindow = 8, kStaggerOnTopWindow = 9,
					kCenterOnParentMonitor = 11, kAlertPositionOnParentMonitor = 12, kStaggerOnParentMonitor = 13};


// ===================================================================================
//	class IDefaultPositionWindow
//!		Used to control how the default IPositionWindow implementation behaves.
// ===================================================================================
class IDefaultPositionWindow : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		Init(EAutoPosition where) = 0;
};

typedef XInterfacePtr<IDefaultPositionWindow> IDefaultPositionWindowPtr;
typedef XInterfacePtr<const IDefaultPositionWindow> IConstDefaultPositionWindowPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
