/*
 *  File:       App.cpp
 *  Summary:   	An IApplication implementation for drag and drop apps.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: App.cpp,v $
 *		Revision 1.5  2001/04/21 02:22:59  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/01/01 03:39:41  jesjones
 *		Switched back to using OnQuit.
 *		
 *		Revision 1.3  2000/11/09 08:51:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include <XDropApp.h>

#include <XDialogUtils.h>
#include <XFileFilter.h>
#include <XFileSystem.h>
#include <XFolderFilter.h>
#include <XIntConversions.h>
#include <XStringUtils.h>

#include "ProcessFiles.h"


// ===================================================================================
//	class CApp
// ===================================================================================
class CApp : public XDropApp {

	typedef XDropApp Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CApp();
	
  						CApp(XBoss* boss);	

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		HandleOpen();
	
protected:
	virtual void 		OnQuit();

	virtual XProcessFiles* OnCreateProcessThread(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs);
	
//-----------------------------------
//	Member Data
//
protected:
	uint32			mNumItems;		// number of files/folders dropped on the app
	std::wstring	mItemName;		// the name of the first item dropped on the app
	bool			mWasFile;
	
	CProcessFiles*	mThread;
};

DEFINE_INTERFACE_FACTORY(CApp)

//---------------------------------------------------------------
//
// CApp::~CApp
//
//---------------------------------------------------------------
CApp::~CApp()
{
}


//---------------------------------------------------------------
//
// CApp::CApp
//
//---------------------------------------------------------------
CApp::CApp(XBoss* boss) : XDropApp(boss)
{
#if MAC
	// This can have a major impact on performance (eg in one test
	// XCountFiles dropped from 6.45 seconds to 1.75 seconds when
	// this line was enabled). OTOH enabling this may degrade the
	// performance of other apps...
	this->SetSleepTime(20);			

	mAcceptsFolders = true;
	mAcceptsFiles = true;
#elif WIN
	mAcceptsFolders = false;
	mAcceptsFiles = true;
	mOpenOptions.AddType(XFileType(L"XML Files", L"xml"));
#endif
	
	mNumItems = 0;
	mWasFile  = false;
	
	mFileFilter = new XFileFilter;
	mFolderFilter = new XFolderFilter;
	
	mFileFilter->AddType(XFileType(L"XML Files", L"XML"));
	
	mThread = nil;
}
	

//---------------------------------------------------------------
//
// CApp::HandleOpen
//
//---------------------------------------------------------------
void CApp::HandleOpen()
{	
	mNumItems = mSpecs.size();
		
	if (mNumItems == 1) {
		XFileSystemSpec spec(mSpecs[0]);		// get a compile error if use a const ref with MSVC 5...
		
		mWasFile  = !XFileSystem::IsFolder(spec);
		mItemName = spec.GetName();
	}
		
	Inherited::HandleOpen();
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CApp::OnQuit
//
//---------------------------------------------------------------
void CApp::OnQuit()
{
	std::wstring text;			

	if (mThread != nil && !mThread->IsRunning() && !mThread->WasAborted()) {		
		if (mNumItems == 1) {
			if (mWasFile) 
				text = LoadAppString(L"Compiled '#1'.", mItemName);
			else 
				text = LoadAppString(L"Compiled one file in '#1'.", mItemName);
		
		} else if (mNumItems > 1)
			text = LoadAppString(L"Compiled #1 files.", UInt32ToStr(mNumFiles));
			
		if (text.length() > 0)
			Whisper::DoNote(text, L"");
	}
}


//---------------------------------------------------------------
//
// CApp::OnCreateProcessThread
//
//---------------------------------------------------------------
XProcessFiles* CApp::OnCreateProcessThread(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs)
{
	PRECONDITION(mThread == nil);
		
	XCallback2<void, std::wstring, std::wstring> callback(this, &CApp::HandleError);
	mThread = new CProcessFiles(fileFilter, folderFilter, specs, callback);
	
	return mThread;
}




