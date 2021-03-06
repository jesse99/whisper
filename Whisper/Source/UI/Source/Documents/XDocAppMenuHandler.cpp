/*
 *  File:       XDocAppMenuHandler.cpp
 *  Summary:   	The IInstallMenuHandler implementation that handles the document app commands.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDocAppMenuHandler.cpp,v $
 *		Revision 1.6  2001/02/08 03:55:10  jesjones
 *		Fixed the close command.
 *		
 *		Revision 1.5  2001/01/30 07:08:05  jesjones
 *		Menu commands are now handled via callbacks.
 *		
 *		Revision 1.4  2001/01/21 00:23:17  jesjones
 *		Menu handling now uses callbacks.
 *		
 *		Revision 1.3  2000/12/31 04:48:00  jesjones
 *		Fixed the header comment.
 *		
 *		Revision 1.2  2000/11/09 12:32:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IInstallMenuHandler.h>

#include <IApplication.h>
#include <IDocument.h>
#include <IDocuments.h>
#include <IDocView.h>
#include <IMenuHandler.h>
#include <IWindowMgr.h>
#include <XEvent.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	class XDocAppMenuHandler
//!		The IInstallMenuHandler implementation that handles the document app commands.
// ===================================================================================
class XDocAppMenuHandler : public IInstallMenuHandler {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XDocAppMenuHandler();
	
						XDocAppMenuHandler(XBoss* boss);
						
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
#if MAC
			void 		DoClose();
			void 		DoSave();

			SCommandStatus DoEnableClose();
			SCommandStatus DoEnableSave();

			bool 		DoCanCloseAll() const;
			bool 		DoCanSaveAll() const;
#endif
};

DEFINE_INTERFACE_FACTORY(XDocAppMenuHandler)

//---------------------------------------------------------------
//
// XDocAppMenuHandler::~XDocAppMenuHandler
//
//---------------------------------------------------------------
XDocAppMenuHandler::~XDocAppMenuHandler()
{
}


//---------------------------------------------------------------
//
// XDocAppMenuHandler::XDocAppMenuHandler
//
//---------------------------------------------------------------
XDocAppMenuHandler::XDocAppMenuHandler(XBoss* boss) 
{	
	IInstallMenuHandler::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XDocAppMenuHandler::InstallCommands
//
//---------------------------------------------------------------
void XDocAppMenuHandler::InstallCommands()
{
	IMenuHandlerPtr handler(L"Application");
	IDocumentsPtr docs(this);

	// New
	XCallback0<void> action(docs.Get(), &IDocuments::New);
	handler->RegisterCommand(kNewCmd, action, kDisabledIfLowMemory+kDisabledIfDialog, this);			

	// Open
	void (IDocuments::*ptr)() = &IDocuments::Open;			// Open is overloaded so we have to help the compiler out
	action = XCallback0<void> (docs.Get(), ptr);
	handler->RegisterCommand(kOpenCmd, action, kDisabledIfLowMemory+kDisabledIfDialog, this);				

#if MAC
	// Close
	action = XCallback0<void> (this, &XDocAppMenuHandler::DoClose);
	XCallback0<SCommandStatus> enabler(this, &XDocAppMenuHandler::DoEnableClose);
	handler->RegisterCommand(kCloseCmd, action, enabler, this);				
#endif				

#if MAC
	// Save
	action = XCallback0<void> (this, &XDocAppMenuHandler::DoSave);
	enabler = XCallback0<SCommandStatus>(this, &XDocAppMenuHandler::DoEnableSave);
	handler->RegisterCommand(kSaveCmd, action, enabler, this);
#endif				
}


//---------------------------------------------------------------
//
// XDocAppMenuHandler::RemoveCommands
//
//---------------------------------------------------------------
void XDocAppMenuHandler::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");
	IDocumentsPtr docs(this);

	// New
	XCallback0<void> action(docs.Get(), &IDocuments::New);
	handler->UnRegisterCommand(action);			

	// Open
	void (IDocuments::*ptr)() = &IDocuments::Open;			// Open is overloaded so we have to help the compiler out
	action = XCallback0<void> (docs.Get(), ptr);
	handler->UnRegisterCommand(action);				

#if MAC
	// Close
	action = XCallback0<void> (this, &XDocAppMenuHandler::DoClose);
	handler->UnRegisterCommand(action);				
#endif			

#if MAC
	// Save
	action = XCallback0<void> (this, &XDocAppMenuHandler::DoSave);
	handler->UnRegisterCommand(action);	
#endif			
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XDocAppMenuHandler::DoClose
//
//---------------------------------------------------------------
#if MAC
void XDocAppMenuHandler::DoClose()
{
	IMenuHandlerPtr handler(L"Application");
	const XEvent& event = handler->GetEvent();
	
	IDocumentsPtr docs(this);
	if (event.WasCommandKeyDown())
		docs->ForceCloseAll();
		
	else if (event.WasOptionKeyDown())
		docs->CloseAll();
		
	else
		DEBUGSTR("XDocAppMenuHandler::DoClose was called but a modifier key isn't down?");
}
#endif


//---------------------------------------------------------------
//
// XDocAppMenuHandler::DoSave
//
//---------------------------------------------------------------
#if MAC
void XDocAppMenuHandler::DoSave()
{
	IMenuHandlerPtr handler(L"Application");
	const XEvent& event = handler->GetEvent();
	
	IDocumentsPtr docs(this);
	if (event.WasOptionKeyDown())
		docs->SaveAll();	
	else
		DEBUGSTR("XDocAppMenuHandler::DoSave was called but a modifier key isn't down?");
}
#endif


//---------------------------------------------------------------
//
// XDocAppMenuHandler::DoEnableClose
//
//---------------------------------------------------------------
#if MAC
SCommandStatus XDocAppMenuHandler::DoEnableClose()
{
	SCommandStatus status;
	
	IMenuHandlerPtr handler(L"Application");
	const XEvent& event = handler->GetEvent();
	
	IWindowMgrPtr windowMgr(this);
	if (windowMgr->GetTopModal() == nil && event.WasOptionKeyDown()) {			
		status.enabled = this->DoCanCloseAll();
		status.name    = LoadWhisperString(L"Close All (documents)");

	} else if (windowMgr->GetTopModal() == nil && event.WasCommandKeyDown()) {
		status.enabled = this->DoCanCloseAll();
		status.name    = LoadWhisperString(L"Junk All (documents)");

	} else {
		status.enabled = false;
		status.name    = LoadWhisperString(L"Close (document)");
	}

	return status;
}
#endif


//---------------------------------------------------------------
//
// XDocAppMenuHandler::DoEnableSave
//
//---------------------------------------------------------------
#if MAC
SCommandStatus XDocAppMenuHandler::DoEnableSave()
{
	SCommandStatus status;
	
	IMenuHandlerPtr handler(L"Application");
	const XEvent& event = handler->GetEvent();
	
	IWindowMgrPtr windowMgr(this);
	if (windowMgr->GetTopModal() == nil && event.WasOptionKeyDown()) {
		status.enabled = this->DoCanSaveAll();
		status.name    = LoadWhisperString(L"Save All (documents)");
	
	} else {
		status.enabled = false;
		status.name    = LoadWhisperString(L"Save (document)");
	}

	return status;
}
#endif


//---------------------------------------------------------------
//
// XDocAppMenuHandler::DoCanCloseAll
//
//---------------------------------------------------------------
#if MAC
bool XDocAppMenuHandler::DoCanCloseAll() const
{
	bool can = false;
	
	IWindowMgrPtr windows(L"Application");
	XWindowIter iter = windows->begin();
	while (iter != windows->end() && !can) {
		IWindowPtr window = *iter;
		++iter;
		
		IDocViewPtr docView(window);
		if (docView) 
			can = true;
	}

	return can;
}
#endif


//---------------------------------------------------------------
//
// XDocAppMenuHandler::DoCanSaveAll
//
//---------------------------------------------------------------
#if MAC
bool XDocAppMenuHandler::DoCanSaveAll() const
{
	bool can = false;
	
	IWindowMgrPtr windows(L"Application");
	XWindowIter iter = windows->begin();
	while (iter != windows->end() && !can) {
		IWindowPtr window = *iter;
		++iter;
		
		IDocViewPtr docView(window);
		if (docView) 
			can = docView->GetDoc()->IsDirty();
	}
	
	return can;
}
#endif


}	// namespace Whisper
