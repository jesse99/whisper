/*
 *  File:       XDocMenuHandler.cpp
 *  Summary:   	The implementation that handles menu commands directed to documents.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDocMenuHandler.cpp,v $
 *		Revision 1.5  2001/04/27 04:32:14  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/01/30 07:08:10  jesjones
 *		Menu commands are now handled via callbacks.
 *		
 *		Revision 1.3  2001/01/21 00:23:22  jesjones
 *		Menu handling now uses callbacks.
 *		
 *		Revision 1.2  2000/11/09 12:32:34  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IInstallMenuHandler.h>

#include <IDocument.h>
#include <IDocView.h>
#include <IMenuHandler.h>
#include <XEvent.h>
#include <XFileSystem.h>

namespace Whisper {


// ===================================================================================
//	class XDocMenuHandler
//!		The implementation that handles menu commands directed to documents.
// ===================================================================================
class XDocMenuHandler : public IInstallMenuHandler {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDocMenuHandler();
	
						XDocMenuHandler(XBoss* boss);
						
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
			void 		DoSave();
			
			SCommandStatus DoEnableSave();
			SCommandStatus DoEnableRevert();
};

DEFINE_INTERFACE_FACTORY(XDocMenuHandler)

//---------------------------------------------------------------
//
// XDocMenuHandler::~XDocMenuHandler
//
//---------------------------------------------------------------
XDocMenuHandler::~XDocMenuHandler()
{
}


//---------------------------------------------------------------
//
// XDocMenuHandler::XDocMenuHandler
//
//---------------------------------------------------------------
XDocMenuHandler::XDocMenuHandler(XBoss* boss) 
{	
	IInstallMenuHandler::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XDocMenuHandler::InstallCommands
//
//---------------------------------------------------------------
void XDocMenuHandler::InstallCommands()
{
	IMenuHandlerPtr handler(L"Application");
	IDocViewPtr view(this);
	IDocumentPtr doc(view->GetDoc());

	// Save
	XCallback0<void> action(this, &XDocMenuHandler::DoSave);
	XCallback0<SCommandStatus> enabler(this, &XDocMenuHandler::DoEnableSave);
	handler->RegisterCommand(kSaveCmd, action, enabler, this);				

	// Save As
	action = XCallback0<void>(doc.Get(), &IDocument::SaveAs);
	handler->RegisterCommand(kSaveAsCmd, action, kAlwaysEnabled, this);			

	// Revert
	action = XCallback0<void>(doc.Get(), &IDocument::Revert);
	enabler = XCallback0<SCommandStatus>(this, &XDocMenuHandler::DoEnableRevert);
	handler->RegisterCommand(kRevertCmd, action, enabler, this);				
}


//---------------------------------------------------------------
//
// XDocMenuHandler::RemoveCommands
//
//---------------------------------------------------------------
void XDocMenuHandler::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");
	IDocViewPtr view(this);
	IDocumentPtr doc(view->GetDoc());

	// Save
	XCallback0<void> action(this, &XDocMenuHandler::DoSave);
	handler->UnRegisterCommand(action);			

	// Save As
	action = XCallback0<void>(doc.Get(), &IDocument::SaveAs);
	handler->UnRegisterCommand(action);			

	// Revert
	action = XCallback0<void>(doc.Get(), &IDocument::Revert);
	handler->UnRegisterCommand(action);				
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XDocMenuHandler::DoSave
//
//---------------------------------------------------------------
void XDocMenuHandler::DoSave()
{
#if MAC
	IMenuHandlerPtr handler(L"Application");	
	ASSERT(!handler->GetEvent().WasOptionKeyDown());
#endif

	IDocViewPtr view(this);
	IDocumentPtr doc(view->GetDoc());

	if (doc->IsOnDisk() && XFileSystem::IsWritable(doc->GetFile()))
		doc->Save();
	else
		doc->SaveAs();
}


//---------------------------------------------------------------
//
// XDocMenuHandler::DoEnableSave
//
//---------------------------------------------------------------
SCommandStatus XDocMenuHandler::DoEnableSave()
{
	SCommandStatus status;
	
#if MAC
	IMenuHandlerPtr handler(L"Application");
	const XEvent& event = handler->GetEvent();
	
	if (!event.WasOptionKeyDown()) {
		IDocViewPtr view(this);
		IDocumentPtr doc(view->GetDoc());

		status.enabled = doc->IsDirty() || !doc->IsOnDisk();	
		status.name = LoadWhisperString(L"Save (document)");		// make sure it doesn't say "Save All"
	}
	
#else
	IDocViewPtr view(this);
	IDocumentPtr doc(view->GetDoc());

	status.enabled = doc->IsDirty() || !doc->IsOnDisk();	
#endif

	return status;
}


//---------------------------------------------------------------
//
// XDocMenuHandler::DoEnableRevert
//
//---------------------------------------------------------------
SCommandStatus XDocMenuHandler::DoEnableRevert()
{
	SCommandStatus status;
	
	IDocViewPtr view(this);
	IDocumentPtr doc(view->GetDoc());
	status.enabled = doc->IsDirty() && doc->IsOnDisk();

	return status;
}


}	// namespace Whisper
