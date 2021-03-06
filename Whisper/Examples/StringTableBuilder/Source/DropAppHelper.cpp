/*
 *  File:       CDropAppHelper.cpp
 *  Summary:   	IDropAppHelper implementation.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: DropAppHelper.cpp,v $
 *		Revision 1.2  2001/08/30 01:12:27  jesjones
 *		Implemented the new Startup method.
 *		
 *		Revision 1.1  2001/02/08 07:14:18  jesjones
 *		Checked in
 *		
 */

#include "AppHeader.h"
#include <IDropAppHelper.h>

#include <IDropApp.h>
#include <XCountFiles.h>
#include <XDialogUtils.h>
#include <XExceptions.h>
#include <XFileFilter.h>
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

	mOpenOptions.SetMessage(L"Pick a folder to search for LoadAppString (or LoadWhisperString)");
	mOpenOptions.SetPrefKey('PkFl');

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
	// Ask the user where the string table file is
	XFileDialogOptions options;
	options.SetMessage(LoadAppString(L"Where's the string table file?"));
	mOpenOptions.SetPrefKey('PkTb');
#if WIN
	options.AddType(XFileType(L"xml", L"xml"));
#endif

	XFileFilter filter;
	filter.AddType(XFileType(L"xml", L"xml"));
#if MAC
	filter.AddType(XFileType('****', 'TEXT'));
#endif

	XOpenReply reply = XFileDialog::ChooseFile(options, filter);
	if (reply.GetNumItems() == 0) 
		throw XCancelException(L"Canceled string table find");

	XFileSpec spec(reply.GetItem(0));

	// Create a thread to update the table
	IDropAppPtr drop(this);
	XCallback2<void, std::wstring, std::wstring> callback(drop.Get(), &IDropApp::HandleError);
	mThread = new CProcessFiles(spec, fileFilter, folderFilter, specs, callback);
	
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
	uint32 numInserted  = mThread->GetNumInserted();
	uint32 numRemoved   = mThread->GetNumRemoved();
	uint32 numProcessed = mThread->GetCount();
		
	std::wstring text;			
	if (numInserted > 0 && numRemoved > 0)
		text = LoadAppString(L"#1 files were processed, #2 strings were added, and #3 strings were removed.", UInt32ToStr(numProcessed), UInt32ToStr(numInserted), UInt32ToStr(numRemoved));
	else if (numInserted > 0)
		text = LoadAppString(L"#1 files were processed and #2 strings were added.", UInt32ToStr(numProcessed), UInt32ToStr(numInserted));
	else if (numRemoved > 0)
		text = LoadAppString(L"#1 files were processed and #2 strings were removed.", UInt32ToStr(numProcessed), UInt32ToStr(numRemoved));
	else
		text = LoadAppString(L"#1 files were processed and no strings were added or removed.", UInt32ToStr(numProcessed));

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


