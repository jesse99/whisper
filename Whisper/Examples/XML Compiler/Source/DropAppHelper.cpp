/*
 *  File:       CDropAppHelper.cpp
 *  Summary:   	IDropAppHelper implementation.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: DropAppHelper.cpp,v $
 *		Revision 1.3  2001/08/29 01:03:52  jesjones
 *		Defined Startup.
 *		
 *		Revision 1.2  2001/02/22 07:26:41  jesjones
 *		GetFile gets files instead of folders on Windows.
 *		
 *		Revision 1.1  2001/01/21 00:50:41  jesjones
 *		Checked in
 *		
 */

#include "AppHeader.h"
#include <IDropAppHelper.h>

#include <IDropApp.h>
#include <XCountFiles.h>
#include <XDialogUtils.h>
#include <XFileSystem.h>
#include <XIntConversions.h>
#include <XStringUtils.h>

#if MAC
	#include <MSystemInfo.h>
#endif

#include "ProcessFiles.h"


// ===================================================================================
//	class CDropAppHelper
//!		IDropAppHelper implementation
// ===================================================================================
class CDropAppHelper : public IDropAppHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CDropAppHelper();
	
  						CDropAppHelper(XBoss* boss);	

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Startup(const std::vector<XFileSystemSpec>& specs);	

	virtual XProcessFiles* CreateProcessThread(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs);	
	virtual XCountFiles* CreateCounterThread(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs);

	virtual XOpenReply 	GetFile(XFileFilter* fileFilter, XFolderFilter* folderFilter);

	virtual void 		ExitingNormally();	
	virtual void 		ExitingViaCancel();
	virtual void 		ExitingViaError();

//-----------------------------------
//	Member Data
//
private:
	XFileDialogOptions	mOpenOptions;			//!< options for OnGetFile
	CProcessFiles*		mThread;
};

DEFINE_INTERFACE_FACTORY(CDropAppHelper)

//---------------------------------------------------------------
//
// CDropAppHelper::~CDropAppHelper
//
//---------------------------------------------------------------
CDropAppHelper::~CDropAppHelper()
{
}


//---------------------------------------------------------------
//
// CDropAppHelper::CDropAppHelper
//
//---------------------------------------------------------------
CDropAppHelper::CDropAppHelper(XBoss* boss)
{
	IDropAppHelper::DoSetBoss(boss);

#if MAC
	mOpenOptions.SetAppName(FromPascalStr(MSystemInfo::GetAppSpec().name));
	mOpenOptions.dialogOptionFlags = kNavNoTypePopup + kNavDontAutoTranslate + kNavDontAddTranslateItems;
#elif WIN
	mOpenOptions.AddType(XFileType(L"XML Files", L"xml"));
#endif

	mThread = nil;
}
	

//---------------------------------------------------------------
//
// CDropAppHelper::Startup
//
//---------------------------------------------------------------
void CDropAppHelper::Startup(const std::vector<XFileSystemSpec>& specs)
{
	UNUSED(specs);
}


//---------------------------------------------------------------
//
// CDropAppHelper::CreateProcessThread
//
// Used to create the thread that processes the files.
//
//---------------------------------------------------------------
XProcessFiles* CDropAppHelper::CreateProcessThread(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs)
{		
	IDropAppPtr drop(this);
	XCallback2<void, std::wstring, std::wstring> callback(drop.Get(), &IDropApp::HandleError);
	mThread = new CProcessFiles(fileFilter, folderFilter, specs, callback);
	
	return mThread;
}


//---------------------------------------------------------------
//
// CDropAppHelper::CreateCounterThread
//
// Used to create the thread that counts the number of files to
// be processed (so the progress bar is accurate). Normally the
// default class is OK, but you can return a larger number if your 
// app spends a lot of time processing each file. For example, 
// UnicodeMapper's count files thread return the sum of the number 
// of files and the total file bytes. OnProcessFile mucks with 
// XProcessFiles::mCount and calls Yield inside it's loop so that 
// the progress bar updates as the huge Unicode data files get 
// processed.
//
//---------------------------------------------------------------
XCountFiles* CDropAppHelper::CreateCounterThread(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs)
{
	IDropAppPtr drop(this);
	XCallback2<void, std::wstring, std::wstring> callback(drop.Get(), &IDropApp::HandleError);

	XCountFiles* thread = new XCountFiles(fileFilter, folderFilter, specs, callback);
	
	return thread;
}


//---------------------------------------------------------------
//
// CDropAppHelper::GetFile
//
//---------------------------------------------------------------
XOpenReply CDropAppHelper::GetFile(XFileFilter* fileFilter, XFolderFilter* folderFilter)
{
#if WIN
	UNUSED(folderFilter);
	XOpenReply reply = XFileDialog::ChooseFile(mOpenOptions, *fileFilter);		
#else
	XOpenReply reply = XFileDialog::ChooseObject(mOpenOptions, *fileFilter, *folderFilter);		
#endif
			
	return reply;
}


//---------------------------------------------------------------
//
// CDropAppHelper::ExitingNormally
//
//---------------------------------------------------------------
void CDropAppHelper::ExitingNormally()
{
	std::wstring text;			

	// Generally you'll want to pop up an alert after your thread
	// is done to let the user know what you did. This is the place
	// where you do that. The LoadAppString calls are for internation-
	// alization: the string will be translated using the CStrings.xml
	// file and the current locale/region.
	uint32 numFiles = mThread->GetNumFiles();
	
	IDropAppPtr drop(this);
	const IDropApp::SpecList& specs = drop->GetSelection();

	if (specs.size() == 1 && XFileSystem::IsFile(specs[0]) && numFiles == 1)
		text = LoadAppString(L"Compiled '#1'.", specs[0].GetName());

	else if (numFiles > 0)
		text = LoadAppString(L"Compiled #1 files.", UInt32ToStr(numFiles));
	
	else
		text = LoadAppString(L"Drop a file on the app to count the number of bytes in the file. Drop a folder on the app to (recursively) count the number of bytes within the folder.");

	Whisper::DoNote(text, L"");
}


//---------------------------------------------------------------
//
// CDropAppHelper::ExitingViaCancel
//
//---------------------------------------------------------------
void CDropAppHelper::ExitingViaCancel()
{
	// don't bother showing an alert if the user canceled out
}


//---------------------------------------------------------------
//
// CDropAppHelper::ExitingViaError
//
//---------------------------------------------------------------
void CDropAppHelper::ExitingViaError()
{
	// don't bother showing an alert if there was an error (an
	// error alert will already have been displayed)
}


