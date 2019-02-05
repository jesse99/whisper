/*
 *  File:       CAppMenuHandler.cpp
 *  Summary:   	The implementation that handles menu commands directed to the app.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CAppMenuHandler.cpp,v $
 *		Revision 1.4  2001/02/09 04:29:28  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.3  2001/01/30 07:07:35  jesjones
 *		Menu commands are now handled via callbacks.
 *		
 *		Revision 1.2  2001/01/21 00:22:32  jesjones
 *		Menu handling now uses callbacks.
 *		
 *		Revision 1.1  2000/12/31 04:23:07  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include <IInstallMenuHandler.h>

#include <IMenuHandler.h>
#include <IWindowMgr.h>
#include <XDialogUtils.h>
#include <XStringUtils.h>

using namespace Whisper;


// ===================================================================================
//	class CAppMenuHandler
//!		The implementation that handles menu commands directed to the app.
// ===================================================================================
class CAppMenuHandler : public IInstallMenuHandler {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~CAppMenuHandler();
	
						CAppMenuHandler(XBoss* boss);
						
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
			void 		DoAbout();
};

DEFINE_INTERFACE_FACTORY(CAppMenuHandler)

//---------------------------------------------------------------
//
// CAppMenuHandler::~CAppMenuHandler
//
//---------------------------------------------------------------
CAppMenuHandler::~CAppMenuHandler()
{
}


//---------------------------------------------------------------
//
// CAppMenuHandler::CAppMenuHandler
//
//---------------------------------------------------------------
CAppMenuHandler::CAppMenuHandler(XBoss* boss)
{	
	IInstallMenuHandler::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CAppMenuHandler::InstallCommands
//
//---------------------------------------------------------------
void CAppMenuHandler::InstallCommands()
{
	IMenuHandlerPtr handler(L"Application");
	
	// About
	XCallback0<void> action(this, &CAppMenuHandler::DoAbout);
	handler->RegisterCommand(kAboutCmd, action, kDisabledIfDialog, this);			
}


//---------------------------------------------------------------
//
// CAppMenuHandler::RemoveCommands
//
//---------------------------------------------------------------
void CAppMenuHandler::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");
	
	// About
	XCallback0<void> action(this, &CAppMenuHandler::DoAbout);
	handler->UnRegisterCommand(action);			
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CAppMenuHandler::DoAbout
//
//---------------------------------------------------------------
void CAppMenuHandler::DoAbout()
{
	Whisper::DoNote(LoadAppString(L"DocSkeleton 1.0"), LoadAppString(L"A skeleton for document oriented applications using the Whisper framework. Jesse Jones (c) 2000"));
}

	

