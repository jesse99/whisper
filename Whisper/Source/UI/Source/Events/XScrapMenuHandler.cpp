/*
 *  File:       XScrapMenuHandler.cpp
 *  Summary:   	Menu command handlers that use IMemento to create undoable IScrap actions.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XScrapMenuHandler.cpp,v $
 *		Revision 1.6  2001/04/02 23:35:58  jesjones
 *		Tweaked for CW Pro 5.3
 *		
 *		Revision 1.5  2001/03/28 09:45:01  jesjones
 *		Commands are installed on focus not activate.
 *		
 *		Revision 1.4  2001/01/30 07:08:19  jesjones
 *		Menu commands are now handled via callbacks.
 *		
 *		Revision 1.3  2001/01/21 00:23:31  jesjones
 *		Menu handling now uses callbacks.
 *		
 *		Revision 1.2  2000/12/24 03:26:28  jesjones
 *		Added support for copy, paste, and clear.
 *		
 *		Revision 1.1  2000/12/22 09:31:49  jesjones
 *		Checked in	
 */

#include <XWhisperHeader.h>
#include <IFocused.h>

#include <ICommand.h>
#include <ICommandQueue.h>
#include <IInstance.h>
#include <IMemento.h>
#include <IMenuHandler.h>
#include <IScrap.h>
#include <IUndoAction.h>
#include <XBind.h>

namespace Whisper {


// ===================================================================================
//	class XScrapMenuHandler
//!		Menu command handlers that use IMemento to create undoable IScrap actions.
// ===================================================================================
class XScrapMenuHandler : public IFocused {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XScrapMenuHandler();
	
						XScrapMenuHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnAcquiredFocus();
	virtual void 		OnLostFocus();

//-----------------------------------
//	Internal API
//
protected:
			void 		DoAction(const std::wstring& boss, const std::wstring& name);
};

DEFINE_INTERFACE_FACTORY(XScrapMenuHandler)

//---------------------------------------------------------------
//
// XScrapMenuHandler::~XScrapMenuHandler
//
//---------------------------------------------------------------
XScrapMenuHandler::~XScrapMenuHandler()
{
}


//---------------------------------------------------------------
//
// XScrapMenuHandler::XScrapMenuHandler
//
//---------------------------------------------------------------
XScrapMenuHandler::XScrapMenuHandler(XBoss* boss)
{
	IFocused::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XScrapMenuHandler::OnAcquiredFocus
//
//---------------------------------------------------------------
void XScrapMenuHandler::OnAcquiredFocus()
{	
	IMenuHandlerPtr handler(L"Application");	
	IConstScrapPtr scrap(this);

	XCallback2<void, std::wstring, std::wstring> theAction(this, &XScrapMenuHandler::DoAction);

	// Cut
	XCallback0<void> action = Bind2(theAction, L"Cut Action", LoadWhisperString(L"Cut"));
	XCallback0<bool> enabler(scrap.Get(), &IScrap::CanCut);
	handler->RegisterCommand(kCutCmd, action, enabler, this);				

	// Copy
	action = Bind2(theAction, L"Copy Action", LoadWhisperString(L"Copy"));
	enabler = XCallback0<bool>(scrap.Get(), &IScrap::CanCopy);
	handler->RegisterCommand(kCopyCmd, action, enabler, this);			

	// Paste
	action = Bind2(theAction, L"Paste Action", LoadWhisperString(L"Paste"));
	enabler = XCallback0<bool>(scrap.Get(), &IScrap::CanPaste);
	handler->RegisterCommand(kPasteCmd, action, enabler, this);				

	// Clear
	action = Bind2(theAction, L"Clear Action", LoadWhisperString(L"Clear"));
	enabler = XCallback0<bool>(scrap.Get(), &IScrap::CanClear);
	handler->RegisterCommand(kClearCmd, action, enabler, this);	
}


//---------------------------------------------------------------
//
// XScrapMenuHandler::OnLostFocus
//
//---------------------------------------------------------------
void XScrapMenuHandler::OnLostFocus()
{
	IMenuHandlerPtr handler(L"Application");

	XCallback2<void, std::wstring, std::wstring> theAction(this, &XScrapMenuHandler::DoAction);

	// Cut
	XCallback0<void> action = Bind2(theAction, L"Cut Action", LoadWhisperString(L"Cut"));
	handler->UnRegisterCommand(action);			

	// Copy
	action = Bind2(theAction, L"Copy Action", LoadWhisperString(L"Copy"));
	handler->UnRegisterCommand(action);			

	// Paste
	action = Bind2(theAction, L"Paste Action", LoadWhisperString(L"Paste"));
	handler->UnRegisterCommand(action);				

	// Clear
	action = Bind2(theAction, L"Clear Action", LoadWhisperString(L"Clear"));
	handler->UnRegisterCommand(action);				
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XScrapMenuHandler::DoAction
//
//---------------------------------------------------------------
void XScrapMenuHandler::DoAction(const std::wstring& boss, const std::wstring& name)
{	
	IUndoActionPtr action(boss);
	action->Init(name);
				
	IInstancePtr instance(action);
	instance->Set(this);
	
	ICommandQueuePtr queue(L"Application");
	queue->Post(ICommandPtr(action));			// need ICommandPtr for Pro 5.3
}


}	// namespace Whisper
