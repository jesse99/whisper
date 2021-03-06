/*
 *  File:		XDropAppProgress.cpp
 *  Summary:	Interface used to control the drag and drop app progress dialog.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDropAppProgress.cpp,v $
 *		Revision 1.7  2001/01/30 07:24:57  jesjones
 *		Removed XDraw.h include.
 *		
 *		Revision 1.6  2001/01/05 06:24:38  jesjones
 *		Updated for the XDropApp refactoring.
 *		
 *		Revision 1.5  2000/12/14 08:35:40  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.4  2000/12/10 04:09:50  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.3  2000/11/09 12:34:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IDropAppProgress.h>

#include <IApplication.h>
#include <IClickExtensions.h>
#include <ICommand.h>
#include <ICommands.h>
#include <IDefaultPositionWindow.h>
#include <IDropApp.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IProgressBar.h>
#include <IPushButton.h>
#include <IRootControl.h>
#include <IStaticText.h>
#include <IText.h>
#include <IValueControl.h>
#include <IWindow.h>
#include <XInterfaceMacros.h>
#include <XShapes.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const int32 kIndent = 10;				// number of pixels to indent from window edges

const int32 kBarWidth  = 300;
const int32 kBarHeight = 16;

const int32 kCaptionGap    = 4;			// pixels between progress bar and caption
const int32 kCaptionHeight = 16;

const int32 kButtonGap    = 14;			// pixels between caption and Cancel button
const int32 kButtonWidth  = 60;
const int32 kButtonHeight = 20;

const XSize kWindowSize(kBarWidth + 2*kIndent, kIndent + kBarHeight + kCaptionGap + kCaptionHeight + kButtonGap + kButtonHeight + kIndent);


// ===================================================================================
//	class XDropAppProgress
//!		Interface used to control the drag and drop app progress dialog.
// ===================================================================================
class XDropAppProgress : public IDropAppProgress {	

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XDropAppProgress();
	
						XDropAppProgress(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent);
	
	virtual void 		SetMessage(const std::wstring& mesg);
			
	virtual void 		SetCount(uint32 count);
	
	virtual void 		SetMaxCount(uint32 count);
			
	virtual void 		SetDeterminate(bool determinate = true);
	
//-----------------------------------
//	Member Data
//
private:
	uint32				mMaxCount;
	ITextPtr			mCaption;
	IValueControlPtr 	mProgressBar;
};

DEFINE_INTERFACE_FACTORY(XDropAppProgress)

//---------------------------------------------------------------
//
// XDropAppProgress::~XDropAppProgress
//
//---------------------------------------------------------------
XDropAppProgress::~XDropAppProgress()
{
}


//---------------------------------------------------------------
//
// XDropAppProgress::XDropAppProgress
//
//---------------------------------------------------------------
XDropAppProgress::XDropAppProgress(XBoss* boss) 
{
	this->DoSetBoss(boss);
	
	mMaxCount = 0;
}


//---------------------------------------------------------------
//
// XDropAppProgress::Init
//
//---------------------------------------------------------------
void XDropAppProgress::Init(const IHierarchyPtr& parent)
{
	// Initialize the window
	IApplicationPtr app(L"Application");
	
	SWindowStyle style(kModalDialog, false);
	IWindowPtr window(this);
	window->Init(parent, app->GetName(), kWindowSize, style);
	
	IDefaultPositionWindowPtr position(window);
	position->Init(kCenterOnMainMonitor);
	
	// Create the root control
	IRootControlPtr rootControl(L"Root Control");
	rootControl->Init(IHierarchyPtr(window), L"Root Control");
	IHierarchyPtr root(rootControl);
		
	// Create a progress bar
	XRect frame(kIndent, kIndent, kIndent+kBarWidth, kIndent+kBarHeight);
	IProgressBarPtr progress(L"Progress Bar");
	progress->Init(root, L"Bar", frame, 0, 0);
	mProgressBar = IValueControlPtr(progress);
	
	// Create a static text control
	frame.top    = frame.bottom + kCaptionGap;
	frame.bottom = frame.top + kCaptionHeight;
	IStaticTextPtr caption(L"Static Text");
	caption->Init(root, L"Caption", frame, L"", kBigSystemFont);
	mCaption = ITextPtr(caption);
	
	// Create the cancel button
	frame.top    = frame.bottom + kButtonGap;
	frame.bottom = frame.top + kButtonHeight;
	frame.right  = kWindowSize.width - kIndent;
	frame.left   = frame.right - kButtonWidth;
	IPushButtonPtr button(L"Push Button");
	button->Init(root, L"Cancel", frame, L"Cancel");
	
	// Route clicks on the cancel button to IDropApp::HandleCancel
	ICommandPtr command(L"Callback Command");
	ICallbackCommandPtr callback(command);

	IDropAppPtr drop(L"Application");
	XCallback0<void> proc(drop.Get(), &IDropApp::HandleCancel);		// we don't want to shutdown the app while we're processing the click so we'll post a command to call HandleCancel
	callback->Init(proc);
	
	ICommandClickExtensionPtr extension(button);
	extension->Init(command);
}


//---------------------------------------------------------------
//
// XDropAppProgress::SetMessage
//
//---------------------------------------------------------------
void XDropAppProgress::SetMessage(const std::wstring& mesg)
{
	if (mCaption)
		mCaption->SetText(mesg);
}


//---------------------------------------------------------------
//
// XDropAppProgress::SetCount
//
//---------------------------------------------------------------
void XDropAppProgress::SetCount(uint32 count)
{
	if (count < mMaxCount)
		mProgressBar->SetValue(numeric_cast<int32>(count));

	if (count == mMaxCount) {		
		mCaption.Release();
		mProgressBar.Release();
	}
}


//---------------------------------------------------------------
//
// XDropAppProgress::SetMaxCount
//
//---------------------------------------------------------------
void XDropAppProgress::SetMaxCount(uint32 count)
{
	mProgressBar->SetMaxValue(numeric_cast<int32>(count));

	mMaxCount = count;
}


//---------------------------------------------------------------
//
// XDropAppProgress::SetDeterminate
//
//---------------------------------------------------------------
void XDropAppProgress::SetDeterminate(bool determinate)
{
	IProgressBarPtr progress(mProgressBar);
	progress->MakeIndeterminate(!determinate);
}


}	// namespace Whisper
