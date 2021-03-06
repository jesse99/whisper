/*
 *  File:       WButtonKeyHandler.cpp
 *  Summary:   	Key handler that handles keystrokes directed at buttons.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WButtonKeyHandler.cpp,v $
 *		Revision 1.3  2001/02/24 09:36:41  jesjones
 *		Calls the helper objects instead of the window proc.
 *		
 *		Revision 1.2  2001/02/21 08:15:28  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.1  2000/12/27 00:55:38  jesjones
 *		Checked in		
 */

#include <XWhisperHeader.h>
#include <IFocused.h>

#include <IClickExtensible.h>
#include <IControl.h>
#include <IControlPressedHelper.h>
#include <IKeyHandler.h>
#include <XKeyEvents.h>
#include <XMouseEvents.h>

namespace Whisper {


// ===================================================================================
//	class WButtonKeyHandler
//!		Key handler that handles keystrokes directed at buttons.
// ===================================================================================
class WButtonKeyHandler : public IFocused {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~WButtonKeyHandler();
	
						WButtonKeyHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnAcquiredFocus();
	virtual void 		OnLostFocus();
						
//-----------------------------------
//	Internal API
//
private:
			bool 		DoKey(const XKeyDownEvent& event);
			bool 		DoChar(const XCharEvent& event);
};

DEFINE_INTERFACE_FACTORY(WButtonKeyHandler)

//---------------------------------------------------------------
//
// WButtonKeyHandler::~WButtonKeyHandler
//
//---------------------------------------------------------------
WButtonKeyHandler::~WButtonKeyHandler()
{
}


//---------------------------------------------------------------
//
// WButtonKeyHandler::WButtonKeyHandler
//
//---------------------------------------------------------------
WButtonKeyHandler::WButtonKeyHandler(XBoss* boss)
{	
	IFocused::DoSetBoss(boss);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// WButtonKeyHandler::OnAcquiredFocus
//
//---------------------------------------------------------------
void WButtonKeyHandler::OnAcquiredFocus()
{
	IKeyHandlerPtr handler(L"Application");
	
	XCallback1<bool, XKeyDownEvent> keys(this, &WButtonKeyHandler::DoKey);
	XCallback1<bool, XCharEvent> chars(this, &WButtonKeyHandler::DoChar);
	handler->RegisterHandler(keys, chars, this);
}


//---------------------------------------------------------------
//
// WButtonKeyHandler::OnLostFocus
//
//---------------------------------------------------------------
void WButtonKeyHandler::OnLostFocus()
{
	IKeyHandlerPtr handler(L"Application");
	
	XCallback1<bool, XKeyDownEvent> keys(this, &WButtonKeyHandler::DoKey);
	handler->UnRegisterHandler(keys);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// WButtonKeyHandler::DoKey
//
//---------------------------------------------------------------
bool WButtonKeyHandler::DoKey(const XKeyDownEvent& event)
{
	bool handled = event.GetKey() == ' ';
		
	return handled;
}


//---------------------------------------------------------------
//
// WButtonKeyHandler::DoChar
//
//---------------------------------------------------------------
bool WButtonKeyHandler::DoChar(const XCharEvent& event)
{
	bool handled = false;
	
	if (event.GetChar() == ' ') {
		IControlPtr control(this);
		if (control->IsEnabled()) {
			MSG msg;
			msg.hwnd    = control->GetOSControl();
			msg.message = WM_LBUTTONDOWN;
			msg.wParam  = 0;				// key flags
			msg.lParam  = MAKELONG(1, 1);	// cursor position (client coordinates)
			msg.time    = ::GetCurrentTime();
				
			IClickExtensiblePtr extensible(this);
			if (extensible)
				extensible->HandleClick(msg);
			           
			IControlPressedHelperPtr pressed(this);
			if (pressed)
				pressed->OnPressed(msg);
		}
	
		handled = true;
	}
	
	return handled;
}


}	// namespace Whisper
