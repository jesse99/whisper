/*
 *  File:       XTextBoxMenuHandler.cpp
 *  Summary:   	Menu command handlers for ITextBox's.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTextBoxMenuHandler.cpp,v $
 *		Revision 1.3  2001/03/28 09:45:04  jesjones
 *		Commands are installed on focus not activate.
 *		
 *		Revision 1.2  2001/01/30 07:08:23  jesjones
 *		Menu commands are now handled via callbacks.
 *		
 *		Revision 1.1  2001/01/21 00:50:57  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <IFocused.h>

#include <IControl.h>
#include <IMenuHandler.h>
#include <ITextBox.h>

namespace Whisper {


// ===================================================================================
//	class XTextBoxMenuHandler
//!		Menu command handlers for ITextBox's.
// ===================================================================================
class XTextBoxMenuHandler : public IFocused {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XTextBoxMenuHandler();
	
						XTextBoxMenuHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnAcquiredFocus();
	virtual void 		OnLostFocus();
};

DEFINE_INTERFACE_FACTORY(XTextBoxMenuHandler)

//---------------------------------------------------------------
//
// XTextBoxMenuHandler::~XTextBoxMenuHandler
//
//---------------------------------------------------------------
XTextBoxMenuHandler::~XTextBoxMenuHandler()
{
}


//---------------------------------------------------------------
//
// XTextBoxMenuHandler::XTextBoxMenuHandler
//
//---------------------------------------------------------------
XTextBoxMenuHandler::XTextBoxMenuHandler(XBoss* boss)
{
	IFocused::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XTextBoxMenuHandler::OnAcquiredFocus
//
//---------------------------------------------------------------
void XTextBoxMenuHandler::OnAcquiredFocus()
{	
	IMenuHandlerPtr handler(L"Application");
	
	ITextBoxPtr textbox(this);	
	IControlPtr control(this);

	// Select All
	XCallback0<void> action(textbox.Get(), &ITextBox::SelectAll);
	XCallback0<bool> enabler(control.Get(), &IControl::IsEnabled);
	handler->RegisterCommand(kSelectAllCmd, action, enabler, this);				
}


//---------------------------------------------------------------
//
// XTextBoxMenuHandler::OnLostFocus
//
//---------------------------------------------------------------
void XTextBoxMenuHandler::OnLostFocus()
{
	IMenuHandlerPtr handler(L"Application");

	ITextBoxPtr textbox(this);	

	// Select All
	XCallback0<void> action(textbox.Get(), &ITextBox::SelectAll);
	handler->UnRegisterCommand(action);			
}


}	// namespace Whisper
