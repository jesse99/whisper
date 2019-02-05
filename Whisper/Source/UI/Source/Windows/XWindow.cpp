/*
 *  File:       XWindow.h
 *  Summary:   	Base class for cross platform window objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XWindow.cpp,v $
 *		Revision 1.5  2001/03/09 10:12:42  jesjones
 *		Dialogs now include that funky question mark icon on Windows.
 *		
 *		Revision 1.4  2000/12/08 08:15:06  jesjones
 *		Got rid of the clip children flag (causes problems with the root control). Dialogs now use the popup option.
 *		
 *		Revision 1.3  2000/11/25 01:58:43  jesjones
 *		Style now includes WS_CLIPCHILDREN.
 *		
 *		Revision 1.2  2000/11/09 12:59:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IWindow.h>

namespace Whisper {


// ===================================================================================
//	struct SWindowStyle
// ===================================================================================

//---------------------------------------------------------------
//
// SWindowStyle::SWindowStyle 
//
// Initialize to semi-reasonable values.
//
//---------------------------------------------------------------
SWindowStyle::SWindowStyle(EWindowType inType, bool show)
{
	type = inType;
	showNew	= show;		
	iconID = 0;	
	drawSizeGrip = false;

	if (type == kModalDialog) {
		targetable		= true;		
		hideOnSuspend	= false;
		
		windClass       = kMovableModalWindowClass;
		attributes      = kWindowNoAttributes;
		clickThrough	= false;		
		getSelectClick	= false;	

		styleEx    = WS_EX_DLGMODALFRAME+WS_EX_CONTEXTHELP;
		style      = WS_CAPTION+WS_POPUP+WS_SYSMENU; 
		classStyle = CS_DBLCLKS + CS_NOCLOSE;

	} else if (type == kAlert) {
		targetable		= true;		
		hideOnSuspend	= false;
		
		windClass       = kMovableAlertWindowClass;
		attributes      = kWindowNoAttributes;
		clickThrough	= false;		
		getSelectClick	= false;	

		styleEx    = WS_EX_DLGMODALFRAME;
		style      = WS_CAPTION+WS_POPUP; 
		classStyle = CS_DBLCLKS + CS_NOCLOSE;

	} else if (type == kDocumentWindow) {
		targetable		= true;		
		hideOnSuspend	= false;
		
		windClass       = kDocumentWindowClass;
		attributes      = kWindowCloseBoxAttribute + kWindowResizableAttribute + kWindowCollapseBoxAttribute + kWindowFullZoomAttribute;
		clickThrough	= false;		
		getSelectClick	= false;	
	
		styleEx = 0;
		style   = WS_OVERLAPPED + 		// we're a top level window
//				  WS_CLIPCHILDREN +		// don't draw over our children
				  WS_CAPTION + 			// we have a title bar
				  WS_MINIMIZEBOX +		// we have a minimize box
				  WS_MAXIMIZEBOX +		// we have a maximize box
				  WS_SYSMENU +			// we have a system menu
				  WS_THICKFRAME;		// we have a sizing border
		classStyle = CS_DBLCLKS;
		drawSizeGrip = true;
	
	} else {
		targetable		= false;		
		hideOnSuspend	= true;
		
		windClass       = kFloatingWindowClass;
		attributes      = kWindowCloseBoxAttribute + kWindowCollapseBoxAttribute;
		clickThrough	= false;		
		getSelectClick	= false;	

		styleEx = 0;
		style   = WS_POPUP + 			// we're a popup window
				  WS_CAPTION + 			// we have a title bar
				  WS_BORDER +			// we have a thin border
				  WS_SYSMENU;			// we have a system menu
		classStyle = CS_DBLCLKS;
	}
}


}	// namespace Whisper
