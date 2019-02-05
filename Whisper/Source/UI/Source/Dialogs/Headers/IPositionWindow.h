/*
 *  File:       IPositionWindow.h
 *  Summary:   	Used to position windows to default locations when they're opened.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IPositionWindow.h,v $
 *		Revision 1.1  2001/01/01 00:28:46  jesjones
 *		Moved from the Windows to the Dialog folder.
 *		
 *		Revision 1.4  2000/12/14 08:24:17  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/11/17 08:55:43  jesjones
 *		Changed the header comment to make it clearer that IPositionWindow is only used to position windows: IRestoreWindowPosition handles saving and restoring window positions.
 *		
 *		Revision 1.2  2000/11/09 09:06:05  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	10/18/00	JDJ		Created
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IPositionWindow
//!		Used to position windows to default locations when they're opened.
// ===================================================================================
class IPositionWindow : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		Position() = 0;
						/**< Queries for an IWindow and repositions it. Note that the
						caller is responsible for ensuring that the window is on
						screen. */
};

typedef XInterfacePtr<IPositionWindow> IPositionWindowPtr;
typedef XInterfacePtr<const IPositionWindow> IConstPositionWindowPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
