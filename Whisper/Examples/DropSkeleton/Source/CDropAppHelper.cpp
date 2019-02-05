/*
 *  File:       CDropAppHelper.cpp
 *  Summary:   	IDropAppHelper implementation.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CDropAppHelper.cpp,v $
 *		Revision 1.3  2001/09/03 22:14:11  jesjones
 *		Added the new Startup method.
 *		
 *		Revision 1.2  2001/02/08 07:11:36  jesjones
 *		Tweaked a comment.
 *		
 *		Revision 1.1  2001/01/05 06:14:09  jesjones
 *		Refactored the drag and drop code so that it uses new IDropApp and IDropAppHelper interfaces.
 *		
 *		Revision 1.6  2001/01/01 03:40:15  jesjones
 *		Switched back to using OnQuit.
 *		
 *		Revision 1.5  2000/11/21 09:20:26  jesjones
 *		Changed the #pragma mark char.
 *		
 *		Revision 1.4  2000/11/09 08:51:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
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

#include "CProcessFiles.h"


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
	XFileDialogOptions	mOpenOptions;			// options for OnGetFile
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
	UNUSED(fileFilter);
	XOpenReply reply = XFileDialog::ChooseFolder(mOpenOptions, *folderFilter);		
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
	uint64 bytes = mThread->GetNumBytes();
	
	IDropAppPtr drop(this);
	const IDropApp::SpecList& specs = drop->GetSelection();

	if (specs.size() == 1) {
		if (XFileSystem::IsFile(specs[0]))
			text = LoadAppString(L"The file '#1' is #2.", specs[0].GetName(), BytesToStr(bytes));
		else 
			text = LoadAppString(L"The folder '#1' contains #2 files and #3.", specs[0].GetName(), UInt32ToStr(numFiles), BytesToStr(bytes));
	
	} else if (specs.size() > 1) {
		text = LoadAppString(L"The selected items contain #1 files and #2.", UInt32ToStr(numFiles), BytesToStr(bytes));
	
	} else
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


