/*
 *  File:       WControlMouseHandler.cpp
 *  Summary:   	IMouseUpHelper implementation that handles clicking on controls.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WControlMouseHandler.cpp,v $
 *		Revision 1.7  2001/04/21 03:30:50  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/02/24 09:38:54  jesjones
 *		Notifies on mouse up instead of mouse down.
 *		
 *		Revision 1.5  2001/02/21 08:16:03  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.4  2000/12/10 04:06:29  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.3  2000/12/08 08:12:07  jesjones
 *		OnMouseDown now says the event was handled.
 *		
 *		Revision 1.2  2000/11/09 12:05:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IMouseUpHelper.h>

#include <IControl.h>
#include <IClickExtensible.h>
#include <IControlPressedHelper.h>
#include <IGeometry.h>
#include <IScrollableControl.h>
#include <XMouseEvents.h>

namespace Whisper {


// ===================================================================================
//	class XControlMouseHandler
//!		IMouseUpHelper implementation that handles clicking on controls.
// ===================================================================================
class XControlMouseHandler : public IMouseUpHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XControlMouseHandler();
	
						XControlMouseHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual bool 		OnMouseUp(const XMouseUpEvent& event);
};

DEFINE_INTERFACE_FACTORY(XControlMouseHandler)

//---------------------------------------------------------------
//
// XControlMouseHandler::~XControlMouseHandler
//
//---------------------------------------------------------------
XControlMouseHandler::~XControlMouseHandler()
{
}


//---------------------------------------------------------------
//
// XControlMouseHandler::XControlMouseHandler
//
//---------------------------------------------------------------
XControlMouseHandler::XControlMouseHandler(XBoss* boss)
{
	IMouseUpHelper::DoSetBoss(boss);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XControlMouseHandler::OnMouseUp
//
//---------------------------------------------------------------
bool XControlMouseHandler::OnMouseUp(const XMouseUpEvent& inEvent)
{
	IControlPtr control(this);
	PRECONDITION(control->IsVisible());
	
	if (control->IsEnabled()) {		
		MSG msg = inEvent.GetOSEvent();
		
		WNDPROC proc = control->GetSystemWindowProc();
		int32 result = ::CallWindowProc(proc, msg.hwnd, msg.message, msg.wParam, msg.lParam);

		if (result == 0) {
			IGeometryPtr geometry(this);
			XRect extent = geometry->GetExtent();
			XPoint loc = inEvent.GetPosition();
			
			if (extent.Contains(loc)) {	
				MSG m;
				m.hwnd    = msg.hwnd,      
				m.message = WM_LBUTTONDOWN;
				m.wParam  = msg.wParam;
				m.lParam  = msg.lParam;  
				
				XMouseDownEvent event(m); 
				
				IClickExtensiblePtr extensible(this);
				if (extensible)
					extensible->HandleClick(event);
			           
				IControlPressedHelperPtr pressed(this);
				if (pressed)
					pressed->OnPressed(event);
			}
		}
	}
		
	return kHandled;
}


}	// namespace Whisper
