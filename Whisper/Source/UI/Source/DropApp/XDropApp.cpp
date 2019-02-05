/*
 *  File:		XDropApp.cpp
 *  Summary:	IDropApp implementation.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDropApp.cpp,v $
 *		Revision 1.15  2001/05/30 01:18:38  jesjones
 *		IDropAppHelper now has a Startup method.
 *		
 *		Revision 1.14  2001/04/27 09:20:35  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.13  2001/04/27 04:32:32  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.12  2001/04/13 08:01:17  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.11  2001/03/06 07:32:36  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.10  2001/02/08 07:12:10  jesjones
 *		Added some whitespace.
 *		
 *		Revision 1.9  2001/01/21 00:45:36  jesjones
 *		Added an include.
 *		
 *		Revision 1.8  2001/01/05 06:24:18  jesjones
 *		No longer a subclass of XApplication.
 *		
 *		Revision 1.7  2001/01/01 03:52:44  jesjones
 *		Changed the delay back to two seconds. Added OnQuit so that the progress window can be hidden before the alert pops up.
 *		
 *		Revision 1.6  2000/12/15 09:07:49  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.5  2000/11/25 01:57:17  jesjones
 *		Dialog is displayed after one second (instead of two).
 *		
 *		Revision 1.4  2000/11/21 09:28:06  jesjones
 *		Mixed in ICommandLine.
 *		
 *		Revision 1.3  2000/11/10 06:49:04  jesjones
 *		Switched to using ~ in the #pragma mark's.
 *		
 *		Revision 1.2  2000/11/09 12:34:11  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <6>	12/21/99	JDJ		Added a stackBytes argument to the XProcessFiles ctor.
 *		 <5>	 9/11/99	JDJ		DoAEOpenApp will do nothing if DoAEOpenDocs has aleady 
 *									been called.
 *		 <4>	 7/20/99	JDJ		Tweaked DoAEOpenApp and DoAEOpenDocs prototypes.
 *		 <3>	 6/21/99	JDJ		Added __MSL_FIX_ITERATORS__ (for Pro 3).
 *		 <2>	 6/03/99	JDJ		Rewrote to use threads and a progress window.
 *		 <1>	 6/02/99	JDJ		Now descends from XApplication.
 */

#include <XWhisperHeader.h>
#include <IDropApp.h>

#include <IApplication.h>
#include <ICommandLine.h>
#include <ICommandQueue.h>
#include <IDropAppHelper.h>
#include <IDropAppProgress.h>
#include <IHierarchy.h>
#include <ITimer.h>
#include <IWindow.h>
#include <XCountFiles.h>
#include <XError.h>
#include <XExceptions.h>
#include <XFileFilter.h>
#include <XFileSystem.h>
#include <XFolderFilter.h>
#include <XMiscUtils.h>
#include <XProcessFiles.h>

#if MAC
	#include <MAppleEvent.h>
#endif

namespace Whisper {


//-----------------------------------
//	Constants
//
const MilliSecond kDelayTime  = 2000;		// amount of time to delay before showing the progress dialog ("GUI Bloopers" says to display a progress bar after one second)

#if !MAC
	const MilliSecond kUpdateFreq = 250;	// interval at which the progress bar updates
#else
	const MilliSecond kUpdateFreq = 1000;	// Mac performs a lot better if we do this relatively infrequently
#endif


// ===================================================================================
//	class XDropApp
//!		IDropApp implementation.
// ===================================================================================
class XDropApp : public IDropApp, public ICommandLine {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDropApp();
	
  						XDropApp(XBoss* boss);

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Start(XFileFilter* takeFile, XFolderFilter* takeFolder);

	virtual const SpecList& GetSelection() const 					{return mSpecs;}

	virtual void 		HandleCancel();

	virtual void 		HandleError(const std::wstring& summary, const std::wstring& narrative);

protected:
	virtual void 		OnCommandLine(const std::vector<std::wstring>& args);

//-----------------------------------
//	Internal API
//
protected:	
			void 		DoOpenWithoutFiles();
			void 		DoOpen();
			
			void 		DoQuit();
			
			void 		DoUpdateStatus();

#if MAC
	static 	pascal OSErr DoAEOpenApp(const AppleEvent* event, AppleEvent* reply, int32 refCon);
	static 	pascal OSErr DoAEOpenDocs(const AppleEvent* event, AppleEvent* reply, int32 refCon);
#endif

//-----------------------------------
//	Member Data
//
private:	
	SpecList			mSpecs;
	XCountFiles*		mCounterThread;
	XProcessFiles*		mProcessThread;	

	XFileFilter*		mFileFilter;
	XFolderFilter*		mFolderFilter;
	uint32				mNumFiles;

	IWindowPtr			mWindow;
	MilliSecond			mStartTime;

	bool				mErrorExit;				// there was an error: some files weren't successfully processed
	bool				mCancelExit;			// user pressed Cancel button
	
#if MAC
	bool 				mOpenedDocs;
#endif
	
	static XDropApp* 	msInstance;
};

DEFINE_INTERFACE_FACTORY(XDropApp)
XDropApp* XDropApp::msInstance = nil;

//---------------------------------------------------------------
//
// XDropApp::~XDropApp
//
//---------------------------------------------------------------
XDropApp::~XDropApp()
{
	msInstance = nil;

	if (mFileFilter != nil)
		mFileFilter->RemoveReference();

	if (mFolderFilter != nil)
		mFolderFilter->RemoveReference();

	if (mCounterThread != nil)
		mCounterThread->RemoveReference();				

	if (mProcessThread != nil)
		mProcessThread->RemoveReference();				
}


//---------------------------------------------------------------
//
// XDropApp::XDropApp
//
//---------------------------------------------------------------
XDropApp::XDropApp(XBoss* boss) : mWindow(L"Progress Window")
{
	IDropApp::DoSetBoss(boss);
	ICommandLine::DoSetBoss(boss);
	
	mErrorExit  = false;
	mCancelExit = false;
	
	mCounterThread = nil;
	mProcessThread = nil;	

	mFileFilter = nil;					
	mFolderFilter = nil;
	mNumFiles = 0;	

#if MAC
	mOpenedDocs = false;
#endif

	msInstance = this;
}


//---------------------------------------------------------------
//
// XDropApp::Start
//
//---------------------------------------------------------------
void XDropApp::Start(XFileFilter* takeFile, XFolderFilter* takeFolder)
{
	PRECONDITION(takeFile != nil);
	PRECONDITION(takeFolder != nil);
	
#if MAC
	AEEventHandlerUPP handler = NewAEEventHandlerUPP(XDropApp::DoAEOpenApp);
	OSErr err = AEInstallEventHandler(kCoreEventClass, kAEOpenApplication, handler, 0, false);
	ThrowIfOSErr(err);
	
	handler = NewAEEventHandlerUPP(XDropApp::DoAEOpenDocs);
	err = AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments, handler, 0, false);
	ThrowIfOSErr(err);
#endif

	mFileFilter = takeFile;
	mFolderFilter = takeFolder;	
}


//---------------------------------------------------------------
//
// XDropApp::HandleCancel
//
//---------------------------------------------------------------
void XDropApp::HandleCancel()
{
	if (mCounterThread != nil)
		mCounterThread->Abort();				

	if (mProcessThread != nil)
		mProcessThread->Abort();	

	// Yield is normally only called in the main event loop so
	// we call Yield here to give the threads a chance to exit
	// which will free up the internal reference count so we
	// won't have any leaks when we exit the app.		
	while ((mCounterThread != nil && mCounterThread->IsRunning()) || (mProcessThread != nil && mProcessThread->IsRunning()))
		XThread::Sleep(50);	

	if (mWindow) {
		IDropAppProgressPtr progress(mWindow);
		progress->SetCount(mNumFiles);				// need to do this so XDropAppProgress knows that it's OK to release it's ref on the progress bar
	}
				
	mCancelExit = true;	
	this->DoQuit();
}


//---------------------------------------------------------------
//
// XDropApp::HandleError
//
//---------------------------------------------------------------
void XDropApp::HandleError(const std::wstring& summary, const std::wstring& narrative)
{
	IApplicationPtr app(L"Application");
	if (!app->IsQuitting()) {
		XError::Instance()->ReportError(summary, narrative);

		if (mWindow) {
			IDropAppProgressPtr progress(mWindow);
			progress->SetCount(mNumFiles);			// need to do this so XDropAppProgress knows that it's OK to release it's ref on the progress bar
		}
				
		mErrorExit = true;	
		this->DoQuit();
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XDropApp::OnCommandLine
//
//---------------------------------------------------------------
void XDropApp::OnCommandLine(const std::vector<std::wstring>& args)
{
	UNUSED(args);
	
#if !MAC
	mSpecs.reserve(args.size());

	for (uint32 index = 0; index < args.size(); ++index) {	
		XFileSystemSpec spec(args[index]);

		if (XFileSystem::SpecExists(spec))
			mSpecs.push_back(spec);
	}
	
	if (!mSpecs.empty())
		this->DoOpen();
	else
		this->DoOpenWithoutFiles();
#endif
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XDropApp::DoOpenWithoutFiles
//
// This is called when the user double-clicks on the program's
// icon. We'll simply pop up a standard file dialog to let the
// user pick a file to process. (This isn't the way the program 
// is typically used, but it allows us to easily use the high level 
// debugger).
//
//---------------------------------------------------------------
void XDropApp::DoOpenWithoutFiles()
{		
	IDropApp* thisPtr = this;
	IDropAppHelperPtr helper(thisPtr);
	XOpenReply reply = helper->GetFile(mFileFilter, mFolderFilter);
	
	uint32 count = reply.GetNumItems();
	if (count > 0) {
		mSpecs.reserve(count);
		
		for (uint32 index = 0; index < count; ++index) {
			XFileSystemSpec spec = reply.GetItem(index);

			mSpecs.push_back(spec);
		}
		
		this->DoOpen();
		
	} else {
		mCancelExit = true;
		this->DoQuit();
	}
}


//---------------------------------------------------------------
//
// XDropApp::DoOpen
//
//---------------------------------------------------------------
void XDropApp::DoOpen()
{					
	try {											
		IDropApp* thisPtr = this;
		IDropAppHelperPtr helper(thisPtr);
		
		helper->Startup(mSpecs);		// may throw XCancelException

		mCounterThread = helper->CreateCounterThread(mFileFilter, mFolderFilter, mSpecs);
		mCounterThread->Start();
				
		IDropAppProgressPtr progress(mWindow);
		progress->Init(thisPtr);
		mWindow->Open();
		
		progress->SetMessage(LoadWhisperString(L"Initializing..."));
		progress->SetDeterminate(false);
		
		mStartTime = GetMilliSeconds();

		ITimerPtr timer(thisPtr);
		XCallback0<void> callback(this, &XDropApp::DoUpdateStatus);
		timer->AddTimer(callback, kUpdateFreq);
				
	} catch (const XCancelException&) {
		mCancelExit = true;
		this->DoQuit();

	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"There was an error creating the worker threads."), e);
		mErrorExit = true;
		this->DoQuit();

	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"There was an error creating the worker threads."));
		mErrorExit = true;	
		this->DoQuit();
	}	
}


//---------------------------------------------------------------
//
// XDropApp::DoQuit
//
//---------------------------------------------------------------
void XDropApp::DoQuit()
{
	IApplicationPtr app(L"Application");
	if (!app->IsQuitting()) {
		if ((mCounterThread != nil && mCounterThread->IsRunning()))
			this->HandleCancel();
		
		else if ((mProcessThread != nil && mProcessThread->IsRunning()))
			this->HandleCancel();

		if (!mErrorExit && !mCancelExit) {
			ICommandQueuePtr commands(L"Application");
			commands->ExecuteCommands();					// if a thread threw an exception this will call HandleError
		}

		if (mWindow) {
			if (mWindow->IsOpened())
				mWindow->Close(true);
			mWindow.Release();
		}
		
		IDropApp* thisPtr = this;
		IDropAppHelperPtr helper(thisPtr);
		if (mErrorExit)
			helper->ExitingViaError();
		else if (mCancelExit)
			helper->ExitingViaCancel();
		else
			helper->ExitingNormally();
		
		ITimerPtr timer(L"Application");
		XCallback0<void> callback(this, &XDropApp::DoUpdateStatus);
		timer->RemoveTimer(callback);

		app->Quit();
	}
}


//---------------------------------------------------------------
//
// XDropApp::DoUpdateStatus
//
//---------------------------------------------------------------
void XDropApp::DoUpdateStatus()
{	
	try {				
		IApplicationPtr app(L"Application");
		if (!app->IsQuitting()) {
					
			// If the app finishes quickly we don't want to popup the
			// progress dialog because it's likely to be more of a
			// distraction than useful feedback.
			if (!mWindow->IsVisible()) {	
				MilliSecond elapsed = GetMilliSeconds() - mStartTime;
			
				if (elapsed > kDelayTime) {
					if (mProcessThread == nil || mProcessThread->GetCount() < 3*mNumFiles/4 || mNumFiles <= 4) {	// don't show the dialog if we're nearly done
						mWindow->Show();

						mWindow->MoveToFront();
					}
				}
			}
						
			IDropAppProgressPtr progress(mWindow);
			if (mNumFiles == 0 && !mCounterThread->IsRunning() && !mCounterThread->WasAborted()) {
				mNumFiles = mCounterThread->GetCount();

				if (mNumFiles > 0) {
					progress->SetMaxCount(mNumFiles);
					progress->SetDeterminate(true);
															
					IDropApp* thisPtr = this;
					IDropAppHelperPtr helper(thisPtr);
					mProcessThread = helper->CreateProcessThread(mFileFilter, mFolderFilter, mSpecs);
					mProcessThread->Start();
					
				} else 
					this->DoQuit();

			} else if (mProcessThread != nil) {
				if (mProcessThread->IsRunning()) {
					progress->SetMessage(mProcessThread->GetMesg());
					progress->SetCount(mProcessThread->GetCount());
				
				} else {
					progress->SetCount(mNumFiles);
					this->DoQuit();
				}
			}
		}
							
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"There was an error creating the worker threads."), e);
		mErrorExit = true;
		this->DoQuit();

	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"There was an error creating the worker threads."));
		mErrorExit = true;
		this->DoQuit();
	}	
}


//---------------------------------------------------------------
//
// XDropApp::DoAEOpenApp								[static]
//
//---------------------------------------------------------------
#if MAC
pascal OSErr XDropApp::DoAEOpenApp(const AppleEvent* event, AppleEvent* reply, int32 refCon)
{
	#pragma unused(event, reply, refCon)
	
	try {
		if (!msInstance->mOpenedDocs)				// for some reason I started getting an 'oapp' event after a 'odoc' event...
			msInstance->DoOpenWithoutFiles();
		
	} catch (const XCancelException& e) {
		msInstance->mCancelExit = true;

	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"There was an error opening the app."), e);
		msInstance->mErrorExit = true;

	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"There was an error opening the app."));
		msInstance->mErrorExit = true;
	}		
	
	return noErr;
}
#endif


//---------------------------------------------------------------
//
// XDropApp::DoAEOpenDocs								[static]
//
//---------------------------------------------------------------
#if MAC
pascal OSErr XDropApp::DoAEOpenDocs(const AppleEvent* inEvent, AppleEvent* reply, int32 refCon)
{
	#pragma unused(reply, refCon)
			
	try {
		MAppleEvent event(*inEvent);
		
		MAEList docList = event.GetDesc(keyDirectObject, typeAEList);
		
		event.CheckRequiredParameters();
		
		uint32 count = docList.GetCount();
		if (count > 0) {
			msInstance->mSpecs.reserve(count);

			for (uint32 index = 0; index < count; index++) {
				FSSpec spec;
				
				OSErr err = docList.GetItem(index, typeFSS, &spec, sizeof(spec));
				ThrowIfOSErr(err);
				
				msInstance->mSpecs.push_back(spec);
			}

			msInstance->DoOpen();
		}
		
		msInstance->mOpenedDocs = true;

	} catch (const XCancelException& e) {
		msInstance->mCancelExit = true;

	} catch (const XSystemException& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"There was an error opening the document list."), e);
		msInstance->mErrorExit = true;

	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"There was an error opening the document list."), e);
		msInstance->mErrorExit = true;

	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"There was an error opening the document list."));
		msInstance->mErrorExit = true;
	}		
				
	return noErr;
}
#endif


}	// namespace Whisper
