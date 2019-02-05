/*
 *  File:       WWindowKeyHandler.cpp
 *  Summary:   	The interface that handles key strokes directed to windows.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998--2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WWindowKeyHandler.cpp,v $
 *		Revision 1.8  2001/04/21 03:34:45  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.7  2001/02/23 10:10:23  jesjones
 *		No longer eats everything (so dialogs work).
 *		
 *		Revision 1.6  2001/02/21 08:17:24  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.5  2001/01/30 07:09:30  jesjones
 *		Key events are now handled via callbacks.
 *		
 *		Revision 1.4  2000/12/27 03:58:31  jesjones
 *		Added an ITabber interface to handle the tab and arrow keys.
 *		
 *		Revision 1.3  2000/12/05 09:17:16  jesjones
 *		Renamed the class to start with X instead of W.
 *		
 *		Revision 1.2  2000/11/09 12:13:13  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IActivated.h>

#include <IKeyHandler.h>
#include <ITabber.h>
#include <IWindow.h>
#include <WUtils.h>
#include <XExceptions.h>
#include <XKeyEvents.h>

namespace Whisper {


// ===================================================================================
//	class XWindowKeyHandler
//!		The interface that handles key strokes directed to windows.
// ===================================================================================
class XWindowKeyHandler : public IActivated {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XWindowKeyHandler();
	
						XWindowKeyHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnActivated();
	virtual void 		OnDeactivated();

//-----------------------------------
//	Internal API
//
private:
			bool 		DoKey(const XKeyDownEvent& event);
			bool 		DoChar(const XCharEvent& event);

			void 		DoTab();
			void 		DoLeftArrow();
			void 		DoUpArrow();
			void 		DoRightArrow();
			void 		DoDownArrow();

			bool 		DoAltChar(const XCharEvent& event);
			bool 		DoMnemonic(HWND control, const std::wstring& mnemonic);
			bool 		DoRouteMnemonic(HWND control);
};

DEFINE_INTERFACE_FACTORY(XWindowKeyHandler)

//---------------------------------------------------------------
//
// XWindowKeyHandler::~XWindowKeyHandler
//
//---------------------------------------------------------------
XWindowKeyHandler::~XWindowKeyHandler()
{
}


//---------------------------------------------------------------
//
// XWindowKeyHandler::XWindowKeyHandler
//
//---------------------------------------------------------------
XWindowKeyHandler::XWindowKeyHandler(XBoss* boss) 
{	
	IActivated::DoSetBoss(boss);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XWindowKeyHandler::OnActivated
//
//---------------------------------------------------------------
void XWindowKeyHandler::OnActivated()
{
	IKeyHandlerPtr handler(L"Application");
		
	ITabberPtr tabber(this);
	if (tabber) {
		XCallback0<void> handle(this, &XWindowKeyHandler::DoTab);
		handler->RegisterHandler(kTabKey, handle, this);
		
		handle = XCallback0<void>(this, &XWindowKeyHandler::DoLeftArrow);
		handler->RegisterHandler(kLeftArrowKey, handle, this);
		
		handle = XCallback0<void>(this, &XWindowKeyHandler::DoUpArrow);
		handler->RegisterHandler(kUpArrowKey, handle, this);
		
		handle = XCallback0<void>(this, &XWindowKeyHandler::DoRightArrow);
		handler->RegisterHandler(kRightArrowKey, handle, this);
		
		handle = XCallback0<void>(this, &XWindowKeyHandler::DoDownArrow);
		handler->RegisterHandler(kDownArrowKey, handle, this);
	
		XCallback1<bool, XKeyDownEvent> keys(this, &XWindowKeyHandler::DoKey);
		XCallback1<bool, XCharEvent> chars(this, &XWindowKeyHandler::DoChar);
		handler->RegisterHandler(keys, chars, this);
	}
}


//---------------------------------------------------------------
//
// XWindowKeyHandler::OnDeactivated
//
//---------------------------------------------------------------
void XWindowKeyHandler::OnDeactivated()
{
	IKeyHandlerPtr handler(L"Application");
		
	ITabberPtr tabber(this);
	if (tabber) {
		XCallback0<void> handle(this, &XWindowKeyHandler::DoTab);
		handler->UnRegisterHandler(handle);
		
		handle = XCallback0<void>(this, &XWindowKeyHandler::DoLeftArrow);
		handler->UnRegisterHandler(handle);
		
		handle = XCallback0<void>(this, &XWindowKeyHandler::DoUpArrow);
		handler->UnRegisterHandler(handle);
		
		handle = XCallback0<void>(this, &XWindowKeyHandler::DoRightArrow);
		handler->UnRegisterHandler(handle);
		
		handle = XCallback0<void>(this, &XWindowKeyHandler::DoDownArrow);
		handler->UnRegisterHandler(handle);
	
		XCallback1<bool, XKeyDownEvent> keys(this, &XWindowKeyHandler::DoKey);
		handler->UnRegisterHandler(keys);
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XWindowKeyHandler::DoKey
//
//---------------------------------------------------------------
bool XWindowKeyHandler::DoKey(const XKeyDownEvent& event)
{	
	UNUSED(event);
	
	return kNotHandled;
}


//---------------------------------------------------------------
//
// XWindowKeyHandler::DoChar
//
//---------------------------------------------------------------
bool XWindowKeyHandler::DoChar(const XCharEvent& event)
{	
	bool handled = this->DoAltChar(event);		// mnemonics can be invoked without the Alt key...
	
	return handled;
}


//---------------------------------------------------------------
//
// XWindowKeyHandler::DoTab
//
//---------------------------------------------------------------
void XWindowKeyHandler::DoTab()
{
	bool forward = GetKeyState(VK_SHIFT) >= 0;
	
	ITabberPtr tabber(this);
	(void) tabber->HandleTab(forward);
}


//---------------------------------------------------------------
//
// XWindowKeyHandler::DoLeftArrow
//
//---------------------------------------------------------------
void XWindowKeyHandler::DoLeftArrow()
{
	ITabberPtr tabber(this);
	(void) tabber->HandleArrowKey(kLeftArrowKey);
}


//---------------------------------------------------------------
//
// XWindowKeyHandler::DoUpArrow
//
//---------------------------------------------------------------
void XWindowKeyHandler::DoUpArrow()
{
	ITabberPtr tabber(this);
	(void) tabber->HandleArrowKey(kUpArrowKey);
}


//---------------------------------------------------------------
//
// XWindowKeyHandler::DoRightArrow
//
//---------------------------------------------------------------
void XWindowKeyHandler::DoRightArrow()
{
	ITabberPtr tabber(this);
	(void) tabber->HandleArrowKey(kRightArrowKey);
}


//---------------------------------------------------------------
//
// XWindowKeyHandler::DoDownArrow
//
//---------------------------------------------------------------
void XWindowKeyHandler::DoDownArrow()
{
	ITabberPtr tabber(this);
	(void) tabber->HandleArrowKey(kDownArrowKey);
}


//---------------------------------------------------------------
//
// XWindowKeyHandler::DoAltChar
//
//---------------------------------------------------------------
bool XWindowKeyHandler::DoAltChar(const XCharEvent& event)
{		
	wchar_t ch = event.GetChar();

	std::wstring mnemonic(2, '&');
	mnemonic[1] = ConvertToLowerCase(ch);

	XInterfacePtr<IWindow> window(this);
	OSWindow windH = window->GetOSWindow();
	
	bool handled = this->DoMnemonic(::GetWindow(windH, GW_CHILD), mnemonic);
			
	return handled;
}


//---------------------------------------------------------------
//
// XWindowKeyHandler::DoMnemonic	
//
// I haven't been able to find any good discussions of windows
// and mnemonics in MSDN or my reference books. However I think
// that the way this normally works is that IsDialogMessage and
// the dialog box window proc work together to make this happen.
// This isn't really what we want since it's restricted to dialog
// boxes (and Whisper doesn't use dialog box window procs).
//
// So, we'll take care of all the mnemonic key handling ourselves.
//
//---------------------------------------------------------------
bool XWindowKeyHandler::DoMnemonic(HWND control, const std::wstring& mnemonic)
{
	bool handled = false;
	
	while (control != nil && !handled) {
	
		// In order for the control to be used as a mnemonic it must
		// be visible, enabled,
		if (::IsWindowVisible(control) && ::IsWindowEnabled(control)) {
		
			// cannot have the SS_NOPREFIX style,
			int32 style = ::GetWindowLong(control, GWL_STYLE);
			if ((style & SS_NOPREFIX) == 0) {
			
				// and the mnemonic character must be part of the
				// control text.
				std::wstring name = ConvertToLowerCase(Whisper::GetText(control));
				uint32 pos = name.find(mnemonic);
				if (pos != std::wstring::npos) 
					handled = this->DoRouteMnemonic(control);
			}
		}
		
		if (!handled) 
			handled = this->DoMnemonic(::GetWindow(control, GW_CHILD), mnemonic);
		
		control = ::GetWindow(control, GW_HWNDNEXT);
	}

	return handled;
}


//---------------------------------------------------------------
//
// XWindowKeyHandler::DoRouteMnemonic
//
//---------------------------------------------------------------
bool XWindowKeyHandler::DoRouteMnemonic(HWND control)
{
	PRECONDITION(control != nil);
	
	bool handled = false;

	char className[32];
	int32 count = ::GetClassNameA(control, className, sizeof(className));
	if (count > 0) {
		if (stricmp(className, "Button") == 0) {
			(void) ::SetFocus(control);
			(void) ::PostMessage(control, WM_COMMAND, MAKELONG(0, BN_CLICKED), (int32) control);
			handled = true;
			
		} else if (stricmp(className, "Static") == 0) {
			HWND next = ::GetWindow(control, GW_HWNDPREV);		// this is pretty goofy, but it's what Windows does...
			while (next != nil && !handled) {
				if (::IsWindowVisible(next) && ::IsWindowEnabled(next)) {
					int32 style = ::GetWindowLong(next, GWL_STYLE);
					if ((style & WS_TABSTOP) == WS_TABSTOP)
						handled = this->DoRouteMnemonic(next);
				}
								
				next = ::GetWindow(next, GW_HWNDPREV);
			}

		} else {
			(void) ::SetFocus(control);
			handled = true;
		}
	}

	return handled;
}


}	// namespace Whisper
