/*
 *  File:       DropAppHelper.cpp
 *  Summary:   	IDropAppHelper implementation.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: DropAppHelper.cpp,v $
 *		Revision 1.1  2001/02/08 07:14:40  jesjones
 *		Checked in
 *		
 */

#include "AppHeader.h"
#include <IDropAppHelper.h>

#include <vector>

#include <IDropApp.h>
#include <XCountFiles.h>
#include <XDialogUtils.h>
#include <XFile.h>
#include <XFileSystem.h>
#include <XIntConversions.h>
#include <XLocker.h>
#include <XStringUtils.h>

#if MAC
	#include <MSystemInfo.h>
#endif

#include "CountFiles.h"
#include "ProcessFiles.h"


//-----------------------------------
//	Globals
//
std::wstring gCodeSectionName;		// "Code section,"
std::wstring gImportSectionName;	// "Import container"
std::string gDataSectionName;		// "Data section"

vector<std::string> gNamespaces;	// "std", "Whisper"


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// ReadFile
//
//---------------------------------------------------------------
static XHandle ReadFile(const XFileSpec& spec)
{	
	XFile file(spec);	
	file.Open(kReadPermission);

	uint32 bytes = (uint32) file.GetLength();
	XHandle data(bytes, kUseTempHeap);
	
	{
	XLocker lock(data);
		file.Read(data.GetPtr(), bytes);
	}
	
	file.Close();
	
	return data;		
}

#if __MWERKS__
#pragma mark -
#endif

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

	vector<XHandle>		mFiles;
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

	mOpenOptions.SetMessage(LoadAppString(L"Where is the map file?"));
#if MAC
	mOpenOptions.SetAppName(FromPascalStr(MSystemInfo::GetAppSpec().name));
	mOpenOptions.dialogOptionFlags = kNavNoTypePopup + kNavDontAutoTranslate + kNavDontAddTranslateItems;
#endif

	mThread = nil;
	
	Str255 str;
	GetIndString(str, 256, 1);
	gCodeSectionName = FromPascalStr(str);
	ASSERT(gCodeSectionName.length() > 0);

	GetIndString(str, 256, 2);
	gImportSectionName = FromPascalStr(str);
	ASSERT(gImportSectionName.length() > 0);

	GetIndString(str, 256, 3);
	gDataSectionName = ToAsciiStr(FromPascalStr(str));
	ASSERT(gDataSectionName.length() > 0);
	
	int16 i = 1;
	while (true) {
		GetIndString(str, 257, i++);
		if (str[0] != '\0')
			gNamespaces.push_back(std::string(ToAsciiStr(FromPascalStr(str))));
		else
			break;
	}
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
	mThread = new CProcessFiles(fileFilter, folderFilter, specs, mFiles, callback);
	
	return mThread;
}


//---------------------------------------------------------------
//
// CDropAppHelper::CreateCounterThread
//
// In order to display an accurate progress bar we need to find
// out how much of the map files we need to process. Because map
// files can be *big* we don't want to read them twice. So we'll
// read them in here and save them so we can reuse them when we
// create the process thread. This assumes that a) users have
// plenty of temp memory or b) users only process one or two map
// files at a time.
//
//---------------------------------------------------------------
XCountFiles* CDropAppHelper::CreateCounterThread(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs)
{
	IDropAppPtr drop(this);
	XCallback2<void, std::wstring, std::wstring> callback(drop.Get(), &IDropApp::HandleError);

	ASSERT(mFiles.size() == 0);
	
	mFiles.reserve(specs.size());
	for (uint32 i = 0; i < specs.size(); ++i) {
		XFileSpec spec(specs[i]);

		XHandle data = ReadFile(spec);
		mFiles.push_back(data);
	}

	XCountFiles* thread = new CCountFiles(fileFilter, folderFilter, specs, mFiles, callback);
	
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
	Whisper::DoNote(L"Done", L"");
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


