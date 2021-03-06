/*
 *  File:       MWindowKeyHandler.cpp
 *  Summary:   	The interface that handles key strokes directed to windows.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MWindowKeyHandler.cpp,v $
 *		Revision 1.5  2001/01/30 07:09:25  jesjones
 *		Key events are now handled via callbacks.
 *		
 *		Revision 1.4  2000/12/27 03:58:27  jesjones
 *		Added an ITabber interface to handle the tab and arrow keys.
 *		
 *		Revision 1.3  2000/12/05 09:13:13  jesjones
 *		Renamed the class to start with X instead of M.
 *		
 *		Revision 1.2  2000/11/09 09:22:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IActivated.h>

#include <IKeyHandler.h>
#include <ITabber.h>
#include <XConstants.h>
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
			void 		DoTab();
			void 		DoLeftArrow();
			void 		DoUpArrow();
			void 		DoRightArrow();
			void 		DoDownArrow();
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

#pragma mark ~

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
	}
}

#pragma mark ~

//---------------------------------------------------------------
//
// XWindowKeyHandler::DoTab
//
//---------------------------------------------------------------
void XWindowKeyHandler::DoTab()
{
	ITabberPtr tabber(this);

	IKeyHandlerPtr handler(L"Application");
	const XKeyDownEvent& event = handler->GetEvent();
	bool forward = (event.GetOSEvent().modifiers & shiftKey) == 0;
	
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


}	// namespace Whisper
