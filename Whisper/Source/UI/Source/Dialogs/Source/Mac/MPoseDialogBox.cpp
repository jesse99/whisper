/*
 *  File:       MPoseDialogBox.cpp
 *  Summary:   	Interface used to display modal dialogs.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MPoseDialogBox.cpp,v $
 *		Revision 1.5  2001/04/21 03:27:43  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/02/21 08:06:47  jesjones
 *		Renamed to start with X.
 *		
 *		Revision 1.3  2001/01/21 00:34:08  jesjones
 *		No longer uses MDeactivateDesktop.
 *		
 *		Revision 1.2  2001/01/05 06:19:44  jesjones
 *		No longer descends from MEventDispatcher.
 *		
 *		Revision 1.1  2001/01/01 00:28:53  jesjones
 *		Moved from the Windows to the Dialog folder.
 *		
 *		Revision 1.6  2000/12/22 09:25:42  jesjones
 *		Uses "Application" to get the command queue.
 *		
 *		Revision 1.5  2000/12/19 07:51:41  jesjones
 *		Inherits from IFocus. Added OnCommandStatus override.
 *		
 *		Revision 1.4  2000/12/14 08:28:20  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/10 03:58:39  jesjones
 *		Compiles now.
 *		
 *		Revision 1.2  2000/12/07 09:45:47  jesjones
 *		Executes commands on the handler instead of the app.
 *		
 *		Revision 1.1  2000/12/05 09:02:08  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <IPoseDialogBox.h>

#include <IActiveUndoContext.h>
#include <IApplication.h>
#include <IClickExtensions.h>
#include <ICommandQueue.h>
#include <IControl.h>
#include <IDialogBox.h>
#include <IEventDispatcher.h>
#include <IMenuBar.h>
#include <IWindow.h>
#include <MDialogUtils.h>
#include <XEvent.h>
#include <XRegion.h>

namespace Whisper {


// ===================================================================================
//	class XPoseDialogBox
//!		Interface used to display modal dialogs.
// ===================================================================================
class XPoseDialogBox : public IPoseDialogBox {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XPoseDialogBox();
	
						XPoseDialogBox(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Open(const IDialogBoxPtr& dialog);
	
	virtual bool 		Run();
		
	virtual void 		Close();

	virtual IDialogBoxPtr GetDialog() const					{return mDialog;}

//-----------------------------------
//	Internal API
//
private:			
			void 		DoProcessNextEvent();
			
			void 		DoOKButton();
			
			void 		DoCancelButton();
			
//-----------------------------------
//	Member Data
//
private:
	IDialogBoxPtr		mDialog;
	bool				mOpened;
	bool				mCancelPressed;
	bool				mOKPressed;

	IUndoContextPtr		mOldContext;
	
	uint16				mEventMask;
	uint32				mSleepTime;
};

DEFINE_INTERFACE_FACTORY(XPoseDialogBox)

//---------------------------------------------------------------
//
// XPoseDialogBox::~XPoseDialogBox
//
//---------------------------------------------------------------
XPoseDialogBox::~XPoseDialogBox()
{
	if (mOpened)						// should only happen via an exception
		this->Close();
}


//---------------------------------------------------------------
//
// XPoseDialogBox::XPoseDialogBox
//
//---------------------------------------------------------------
XPoseDialogBox::XPoseDialogBox(XBoss* boss)
{	
	IPoseDialogBox::DoSetBoss(boss);
	
	mEventMask = everyEvent;
	mSleepTime = 6;
	
	mOpened = false;
}


//---------------------------------------------------------------
//
// XPoseDialogBox::Open
//
//---------------------------------------------------------------
void XPoseDialogBox::Open(const IDialogBoxPtr& dialog)
{	
	PRECONDITION(!mOpened);

	mDialog = dialog;
	mOKPressed = false;
	mCancelPressed = false;

	IActiveUndoContextPtr active(L"Application");
	mOldContext = active->Get();

	// Open the dialog
	IWindowPtr window(mDialog);	
	window->Open();				

	// Listen to the default button
	IControlPtr control = mDialog->GetDefaultButton();
	ASSERT(control);										// we require a default button
	
	ICallbackClickExtensionPtr extension(control);
	extension->Init(XCallback0<void>(this, &XPoseDialogBox::DoOKButton), 1);	
		
	// Listen to the cancel button
	control = mDialog->GetCancelButton();
	if (control) {											// cancel buttons are optional
		extension = ICallbackClickExtensionPtr(control);
		extension->Init(XCallback0<void>(this, &XPoseDialogBox::DoCancelButton), 1);	
	}
	
	// Show the dialog
	window->Show();
	window->MoveToFront();
	
	mOpened = true;
}

	
//---------------------------------------------------------------
//
// XPoseDialogBox::Run
//
//---------------------------------------------------------------
bool XPoseDialogBox::Run()
{
	PRECONDITION(mOpened);

	while (!mOKPressed && !mCancelPressed) {
		this->DoProcessNextEvent();
		
		if (mOKPressed && !mDialog->ValidateData())
			mOKPressed = false;
	}
	
	// This will be restored when a XWindow is activated, but people
	// often want to post undoable commands immediately after putting
	// up a dialog so we'll do it here.
	IActiveUndoContextPtr active(L"Application");
	active->Set(mOldContext);
	
	return mOKPressed;
}


//---------------------------------------------------------------
//
// XPoseDialogBox::Close
//
//---------------------------------------------------------------
void XPoseDialogBox::Close()
{
	PRECONDITION(mOpened);

	IWindowPtr window(mDialog);	
	window->Close(false);

	mOpened = false;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XPoseDialogBox::DoProcessNextEvent	
//
//---------------------------------------------------------------
void XPoseDialogBox::DoProcessNextEvent()
{
	PRECONDITION(mOpened);
	
	// Process an event
	EventRecord event;
	static XRegion mouseRgn(XRect(0, 0, 1, 1));
	bool gotEvent = ::WaitNextEvent(mEventMask, &event, mSleepTime, mouseRgn);
	
	IEventDispatcherPtr dispatcher(this);
	if (event.what == osEvt || event.what == mouseDown)
		dispatcher->HandleAdjustCursor(event);	
		
	dispatcher->DispatchEvent(event);
	
	// Let commands execute
	ICommandQueuePtr commands(L"Application");
	IMenuBarPtr menubar(L"Application");
	if (commands && commands->ExecuteCommands() && menubar)
		menubar->Invalidate();
	
	// Update the menus
	if (menubar && !menubar->IsValid()) {
		IApplicationPtr app(L"Application");
		app->HandleUpdateMenus(XEvent().GetOSEvent());
	}
}


//---------------------------------------------------------------
//
// XPoseDialogBox::DoOKButton
//
//---------------------------------------------------------------
void XPoseDialogBox::DoOKButton()
{
	mOKPressed = true;
}


//---------------------------------------------------------------
//
// XPoseDialogBox::DoCancelButton
//
//---------------------------------------------------------------
void XPoseDialogBox::DoCancelButton()
{
	mCancelPressed = true;
}

			
}	// namespace Whisper
