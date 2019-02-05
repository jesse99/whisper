/*
 *  File:       XWindowMenuHandler.cpp
 *  Summary:   	The implementation that handles menu commands directed to windows.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XWindowMenuHandler.cpp,v $
 *		Revision 1.7  2001/02/08 03:57:10  jesjones
 *		Removed KCloseBoxCmd.
 *		
 *		Revision 1.6  2001/01/30 07:16:17  jesjones
 *		Menu commands are now handled via callbacks.
 *		
 *		Revision 1.5  2001/01/21 00:23:35  jesjones
 *		Menu handling now uses callbacks.
 *		
 *		Revision 1.4  2000/12/14 08:38:05  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/05 09:20:02  jesjones
 *		Tweaked the mark pragma.
 *		
 *		Revision 1.2  2000/11/09 12:59:35  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IInstallMenuHandler.h>

#include <IMenuHandler.h>
#include <IWindow.h>
#include <XBind.h>
#include <XEvent.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class XWindowMenuHandler
//!		The implementation that handles menu commands directed to windows.
// ===================================================================================
class XWindowMenuHandler : public IInstallMenuHandler {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XWindowMenuHandler();
	
						XWindowMenuHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		InstallCommands();
	virtual void 		RemoveCommands();
						
//-----------------------------------
//	Internal API
//
private:			
			SCommandStatus DoEnableClose();
};

DEFINE_INTERFACE_FACTORY(XWindowMenuHandler)

//---------------------------------------------------------------
//
// XWindowMenuHandler::~XWindowMenuHandler
//
//---------------------------------------------------------------
XWindowMenuHandler::~XWindowMenuHandler()
{
}


//---------------------------------------------------------------
//
// XWindowMenuHandler::XWindowMenuHandler
//
//---------------------------------------------------------------
XWindowMenuHandler::XWindowMenuHandler(XBoss* boss)
{	
	IInstallMenuHandler::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XWindowMenuHandler::InstallCommands
//
//---------------------------------------------------------------
void XWindowMenuHandler::InstallCommands()
{
	IMenuHandlerPtr handler(L"Application");
	
	IWindowPtr window(this);
	XCallback1<void, bool> theAction(window.Get(), &IWindow::Close);

	// Close
	XCallback0<void> action = Bind1(theAction, false);
	XCallback0<SCommandStatus> enabler(this, &XWindowMenuHandler::DoEnableClose);
	handler->RegisterCommand(kCloseCmd, action, enabler, this);				
}


//---------------------------------------------------------------
//
// XWindowMenuHandler::RemoveCommands
//
//---------------------------------------------------------------
void XWindowMenuHandler::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");

	IWindowPtr window(this);
	XCallback1<void, bool> theAction(window.Get(), &IWindow::Close);

	// Close
	XCallback0<void> action = Bind1(theAction, false);
	handler->UnRegisterCommand(action);			
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XWindowMenuHandler::DoEnableClose
//
//---------------------------------------------------------------
SCommandStatus XWindowMenuHandler::DoEnableClose()
{
	SCommandStatus status;
	
	IWindowPtr window(this);
	const SWindowStyle& style = window->GetStyle();

#if MAC
	IMenuHandlerPtr handler(L"Application");
	const XEvent& event = handler->GetEvent();
	
	bool noModifiers = !event.WasCommandKeyDown() && !event.WasOptionKeyDown();	
	status.enabled = noModifiers && ((style.attributes & kWindowCloseBoxAttribute) != 0);
	status.name    = LoadWhisperString(L"Close Menu Item");
	
#elif WIN
	status.enabled = (style.classStyle & CS_NOCLOSE) == 0;
	status.name    = LoadWhisperString(L"Close Menu Item");
#endif

	return status;
}


}	// namespace Whisper
