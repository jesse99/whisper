/*
 *  File:       XAppMenuHandler.cpp
 *  Summary:   	The IInstallMenuHandler implementation that handles the standard app commands.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XAppMenuHandler.cpp,v $
 *		Revision 1.8  2001/04/13 08:00:59  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.7  2001/03/01 11:41:34  jesjones
 *		Added #if DEBUG to a bit of code.
 *		
 *		Revision 1.6  2001/02/12 09:47:36  jesjones
 *		No longer registers parse tests for DLL targets.
 *		
 *		Revision 1.5  2001/01/30 07:07:47  jesjones
 *		Menu commands are now handled via callbacks.
 *		
 *		Revision 1.4  2001/01/21 00:23:12  jesjones
 *		Menu handling now uses callbacks.
 *		
 *		Revision 1.3  2000/12/31 04:47:05  jesjones
 *		Removed DisplayAboutBox.
 *		
 *		Revision 1.2  2000/11/09 12:25:35  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IInstallMenuHandler.h>

#include <IActiveUndoContext.h>
#include <IApplication.h>
#include <IWindow.h>
#include <IMenuBar.h>
#include <IMenuHandler.h>
#include <IUndoContext.h>
#include <IWindowMgr.h>
#include <XExceptions.h>
#include <XIntConversions.h>
#include <XRegisterCoreTests.h>
#include <XRegisterFileTests.h>
#include <XRegisterGraphicTests.h>
#include <XRegisterParseTests.h>

namespace Whisper {


// ===================================================================================
//	class XAppMenuHandler
//!		The IInstallMenuHandler implementation that handles the standard app commands.
// ===================================================================================
class XAppMenuHandler : public IInstallMenuHandler {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XAppMenuHandler();
	
						XAppMenuHandler(XBoss* boss);
						
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
			SCommandStatus DoEnableQuit();

			void 		DoUndo();
			SCommandStatus DoEnableUndo();

			void 		DoRedo();
			SCommandStatus DoEnableRedo();
};

DEFINE_INTERFACE_FACTORY(XAppMenuHandler)

//---------------------------------------------------------------
//
// XAppMenuHandler::~XAppMenuHandler
//
//---------------------------------------------------------------
XAppMenuHandler::~XAppMenuHandler()
{
}


//---------------------------------------------------------------
//
// XAppMenuHandler::XAppMenuHandler
//
//---------------------------------------------------------------
XAppMenuHandler::XAppMenuHandler(XBoss* boss) 
{	
	IInstallMenuHandler::DoSetBoss(boss);
	
#if DEBUG				
	RegisterCoreTests();
	RegisterFileTests();
	RegisterGraphicTests();
#if !MULTI_FRAGMENT_APP				// $$$ 
	RegisterParseTests();
#endif
#endif
}


//---------------------------------------------------------------
//
// XAppMenuHandler::InstallCommands
//
//---------------------------------------------------------------
void XAppMenuHandler::InstallCommands()
{
	IMenuHandlerPtr handler(L"Application");
	
	IApplicationPtr app(this);

	// Quit
	XCallback0<void> action(app.Get(), &IApplication::Quit);
	XCallback0<SCommandStatus> enabler(this, &XAppMenuHandler::DoEnableQuit);
	handler->RegisterCommand(kQuitCmd, action, enabler, this);			

	// Undo
	action = XCallback0<void> (this, &XAppMenuHandler::DoUndo);
	enabler = XCallback0<SCommandStatus> (this, &XAppMenuHandler::DoEnableUndo);
	handler->RegisterCommand(kUndoCmd, action, enabler, this);				

	// Redo
	action = XCallback0<void> (this, &XAppMenuHandler::DoRedo);
	enabler = XCallback0<SCommandStatus> (this, &XAppMenuHandler::DoEnableRedo);
	handler->RegisterCommand(kRedoCmd, action, enabler, this);				
}


//---------------------------------------------------------------
//
// XAppMenuHandler::RemoveCommands
//
//---------------------------------------------------------------
void XAppMenuHandler::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");
	
	IApplicationPtr app(this);

	// Quit
	XCallback0<void> action(app.Get(), &IApplication::Quit);
	handler->UnRegisterCommand(action);			

	// Undo
	action = XCallback0<void> (this, &XAppMenuHandler::DoUndo);
	handler->UnRegisterCommand(action);				

	// Redo
	action = XCallback0<void> (this, &XAppMenuHandler::DoRedo);
	handler->UnRegisterCommand(action);				
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XAppMenuHandler::DoUndo
//
//---------------------------------------------------------------
void XAppMenuHandler::DoUndo()
{
	bool handled = true;
	
	IActiveUndoContextPtr activeContext(this);
	IUndoContextPtr context;
	if (activeContext)
		context = activeContext->Get();
	
	context->Undo();
}


//---------------------------------------------------------------
//
// XAppMenuHandler::DoRedo
//
//---------------------------------------------------------------
void XAppMenuHandler::DoRedo()
{
	bool handled = true;
	
	IApplicationPtr app(this);
	IActiveUndoContextPtr activeContext(this);
	IUndoContextPtr context;
	if (activeContext)
		context = activeContext->Get();
	
	context->Redo();
}


//---------------------------------------------------------------
//
// XAppMenuHandler::DoEnableQuit
//
//---------------------------------------------------------------
SCommandStatus XAppMenuHandler::DoEnableQuit()
{
	SCommandStatus status;
	
	IWindowMgrPtr windowMgr(this);
#if DEBUG
	status.enabled = !gMonkeyLives && windowMgr->GetTopModal() == nil;
#else
	status.enabled = windowMgr->GetTopModal() == nil;
#endif

	return status;
}


//---------------------------------------------------------------
//
// XAppMenuHandler::DoEnableUndo
//
//---------------------------------------------------------------
SCommandStatus XAppMenuHandler::DoEnableUndo()
{
	SCommandStatus status;
			
	IApplicationPtr app(this);
	IActiveUndoContextPtr activeContext(this);
	IUndoContextPtr context;
	if (activeContext)
		context = activeContext->Get();

	if (context) {
		status.enabled = context->CanUndo() && !app->IsMemoryLow();
		status.name    = context->GetUndoText();
		
	} else {
		status.enabled = false;
		status.name    = LoadWhisperString(L"Can't Undo");
	}

	return status;
}


//---------------------------------------------------------------
//
// XAppMenuHandler::DoEnableRedo
//
//---------------------------------------------------------------
SCommandStatus XAppMenuHandler::DoEnableRedo()
{
	SCommandStatus status;
			
	IApplicationPtr app(this);
	IActiveUndoContextPtr activeContext(this);
	IUndoContextPtr context;
	if (activeContext)
		context = activeContext->Get();

	if (context) {
		status.enabled = context->CanRedo() && !app->IsMemoryLow();
		status.name    = context->GetRedoText();
		
	} else {
		status.enabled = false;
		status.name    = LoadWhisperString(L"Can't Redo");
	}

	return status;
}


}	// namespace Whisper
