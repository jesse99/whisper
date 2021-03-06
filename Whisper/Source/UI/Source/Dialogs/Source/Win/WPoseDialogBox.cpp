/*
 *  File:       WPoseDialogBox.cpp
 *  Summary:   	Interface used to display modal dialogs.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WPoseDialogBox.cpp,v $
 *		Revision 1.3  2001/04/21 03:33:13  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2001/02/21 08:16:46  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.1  2001/01/01 00:29:00  jesjones
 *		Moved from the Windows to the Dialog folder.
 *		
 *		Revision 1.4  2000/12/27 01:07:13  jesjones
 *		Gets ICommands interface off the app. Tickles timers.
 *		
 *		Revision 1.3  2000/12/14 08:31:19  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/12/08 08:19:14  jesjones
 *		Removed some unused includes.
 *		
 *		Revision 1.1  2000/12/07 09:49:09  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <IPoseDialogBox.h>

#include <IActiveUndoContext.h>
#include <IClickExtensions.h>
#include <ICommandQueue.h>
#include <IDialogBox.h>
#include <ITimer.h>
#include <IMenuBar.h>
#include <IWindow.h>
#include <IWindowMgr.h>

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
	IDialogBoxPtr			mDialog;
	bool					mOpened;
	bool					mCancelPressed;
	bool					mOKPressed;

	IUndoContextPtr			mOldContext;
	std::list<IWindowPtr>	mDisabledWindows;
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
		
	mOpened = false;
}


//---------------------------------------------------------------
//
// XPoseDialogBox::Open    dd
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
	
	// Disable all the enabled windows
	IWindowMgrPtr windows(L"Application");
	XWindowIter iter = windows->begin();
	while (iter != windows->end()) {
		IWindowPtr wind = *iter;
		++iter;

		if (wind != mDialog && ::IsWindowEnabled(wind->GetOSWindow())) {
			::EnableWindow(wind->GetOSWindow(), false);
			mDisabledWindows.push_back(wind);
		}
	}

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

	std::list<IWindowPtr>::iterator iter = mDisabledWindows.begin();
	while (iter != mDisabledWindows.end()) {
		IWindowPtr wind = *iter;
		++iter;

		::EnableWindow(wind->GetOSWindow(), true);
	}
	mDisabledWindows.clear();

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
	MSG msg;
	bool gotEvent = ::GetMessage(&msg, nil, 0, 0) != 0;
		
	if (gotEvent) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	} 	

	// Let commands execute
	ICommandQueuePtr commands(L"Application");
	if (commands)
		(void) commands->ExecuteCommands();

	// Give time to timers
	ITimerPtr timer(L"Application");
	if (timer)
		timer->Tickle();
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
