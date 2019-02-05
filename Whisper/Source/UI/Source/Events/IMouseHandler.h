/*
 *  File:       IMouseHandler.h
 *  Summary:   	Interface for objects that respond to mouse events.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IMouseHandler.h,v $
 *		Revision 1.6  2001/03/09 10:22:13  jesjones
 *		HandleHelp is now Mac only.
 *		
 *		Revision 1.5  2001/02/04 03:33:52  jesjones
 *		Got rid of OnClicked.
 *		
 *		Revision 1.4  2000/12/14 08:23:54  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/08 08:14:04  jesjones
 *		Added OnClicked to support lame Windows.
 *		
 *		Revision 1.2  2000/11/09 09:04:13  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 6/25/00	JDJ		Created
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XMouseDownEvent;
class XMouseMoveEvent;
class XMouseUpEvent;


// ===================================================================================
//	class IMouseHandler
//!		Interface for objects that respond to mouse events.
// ===================================================================================
class IMouseHandler : public XUnknown {

//-----------------------------------
//	API
//
public:					
	virtual bool 		HandleMouseDown(const XMouseDownEvent& event) = 0;
						/**< Note that you can add IMouseExtensions to change the way 
						mouse down and up events are processed. */
	
	virtual bool 		HandleMouseUp(const XMouseUpEvent& event) = 0;
		
	virtual bool 		HandleContextMenu(const XMouseDownEvent& event) = 0;
						/**< Called in response to a right mouse click (see IContextMenu.h) */
		
	virtual bool 		HandleAdjustCursor(const XMouseMoveEvent& event) = 0;

#if MAC
	virtual bool 		HandleHelp(const XMouseMoveEvent& event) = 0;
#endif
};

typedef XInterfacePtr<IMouseHandler> IMouseHandlerPtr;
typedef XInterfacePtr<const IMouseHandler> IConstMouseHandlerPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
