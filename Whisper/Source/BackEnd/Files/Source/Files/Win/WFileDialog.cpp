/*
 *  File:		WFileDialog.cpp
 *  Summary:	File open and save dialogs.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998--2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WFileDialog.cpp,v $
 *		Revision 1.5  2001/04/27 09:18:18  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.4  2001/04/21 03:31:29  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/02/21 08:16:34  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.2  2000/11/09 12:07:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <7>	 6/21/99	JDJ		Added XFileDialogOptions::SetDefaultFolder. XFileDialogOptions 
 *									ctor sets nMaxCustFilter and nMaxFile using characters instead 
 *									of bytes. XFileDialog::SaveFile uses the initial dir pref. 
 *		 <6>	 6/10/99	JDJ		Uses GetProcAddress when calling the wide versions of SHGetPathFromIDList 
 *									and SHBrowseForFolder.
 *		 <5>	 6/05/99	JDJ		No longer uses MB_ICONQUESTION
 *		 <4>	 5/07/99	JDJ		Types to open and save are specified in XFileDialogOptions using
 *									XFileType objects.
 *		 <3>	 2/26/99	JDJ		OpenFile and ChooseFile remember the last folder the user was in.
 *									Frees memory allocated inside SHBrowseForFolder.
 *		 <3>	 2/25/99	JDJ		ChooseFolder filters folders and remembers last folder chosen.
 *		 <2>	 1/20/99	JDJ		Added ChooseFolder overload taking only an XFileDialogOptions.
 *		 <1>	 9/11/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XFileDialog.h>

#include <shlobj.h>

#include <WSystemInfo.h>
#include <XExceptions.h>
#include <XFileFilter.h>
#include <XFileSpec.h>
#include <XFileSystem.h>
#include <XFolderFilter.h>
#include <XFolderSpec.h>
#include <XLocker.h>
#include <XNumbers.h>
#include <XPreference.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	struct SFolderInfo
// ===================================================================================
struct SFolderInfo {
	const XFileDialogOptions* options;
	const XFolderFilter*	  filter;
};


// ===================================================================================
//	struct SFileFolderInfo
// ===================================================================================
struct SFileFolderInfo {
	const XFileDialogOptions* options;
	const XFileFilter*		  fileFilter;
	const XFolderFilter*	  folderFilter;
};


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetPath	
//
//---------------------------------------------------------------
static bool GetPath(ITEMIDLIST* pidl, std::wstring* path)
{
	PRECONDITION(path != nil);
	PRECONDITION(pidl != nil);
	
	int32 succeeded = 0;
	
	// Get a pointer to the wide version of SHGetPathFromIDList (we
	// need to do this because Win 95 only has the wide version if
	// the active desktop update is installed).
	typedef BOOL (WINAPI *SHGetPathFromIDListWProc)(LPCITEMIDLIST, LPWSTR);

    SHGetPathFromIDListWProc getPathProc = nil; 
    
    HINSTANCE shellH = GetModuleHandle(_TEXT("SHELL32.DLL")); 	
	if (shellH != nil) 
        getPathProc = (SHGetPathFromIDListWProc) GetProcAddress(shellH, _TEXT("SHGetPathFromIDListW")); 

	// Get the path
	if (WSystemInfo::HasUnicode() && getPathProc != nil) {
		wchar_t buffer[MAX_PATH];

		succeeded = getPathProc(pidl, buffer);
        if (succeeded)
			*path = buffer;
	
	} else {
		char buffer[MAX_PATH];

		succeeded = SHGetPathFromIDListA(pidl, buffer);
        if (succeeded)
			*path = FromPlatformStr(buffer);
	}
 	
 	return succeeded != 0;
}


//---------------------------------------------------------------
//
// GetSpecialFolder	
//
//---------------------------------------------------------------
static bool GetSpecialFolder(std::wstring* path, int csidl, bool createFolder)
{
	PRECONDITION(path != nil);
	
	int32 succeeded = 0;
	
	// Get a pointer to the wide and narrow versions of SHGetSpecialFolderPath 
	// (we need to do this because Win 95 only has SHGetSpecialFolderPath 
	// if the active desktop update is installed).
	typedef BOOL (WINAPI *SHGetSpecialFolderPathAProc)(HWND, LPSTR, int, BOOL);
	typedef BOOL (WINAPI *SHGetSpecialFolderPathWProc)(HWND, LPWSTR, int, BOOL);

    SHGetSpecialFolderPathAProc getPathAProc = nil; 
    SHGetSpecialFolderPathWProc getPathWProc = nil; 
    
    HINSTANCE shellH = GetModuleHandle(_TEXT("SHELL32.DLL")); 	
	if (shellH != nil) {
        getPathAProc = (SHGetSpecialFolderPathAProc) GetProcAddress(shellH, _TEXT("SHGetSpecialFolderPathA")); 
        getPathWProc = (SHGetSpecialFolderPathWProc) GetProcAddress(shellH, _TEXT("SHGetSpecialFolderPathW")); 
	}
	
	// Get the path
	if (WSystemInfo::HasUnicode() && getPathWProc != nil) {
		wchar_t buffer[MAX_PATH];

		succeeded = getPathWProc(nil, buffer, csidl, createFolder);
        if (succeeded)
			*path = buffer;
	
	} else if (getPathAProc != nil) {
		char buffer[MAX_PATH];

		succeeded = getPathAProc(nil, buffer, csidl, createFolder);
        if (succeeded)
			*path = FromPlatformStr(buffer);
	}
 	
 	return succeeded != 0;
}


//---------------------------------------------------------------
//
// BrowseForFolder
//	
//---------------------------------------------------------------
static ITEMIDLIST* BrowseForFolder(const BROWSEINFOW& info)
{
	ITEMIDLIST* items = nil;
	
	// Get a pointer to the wide version of SHBrowseForFolder (we
	// need to do this because Win 95 only has the wide version if
	// the active desktop update is installed).
	typedef LPITEMIDLIST (WINAPI *SHBrowseForFolderWProc)(LPBROWSEINFOW);

    SHBrowseForFolderWProc browseProc = nil; 
    
    HINSTANCE shellH = GetModuleHandle(_TEXT("SHELL32.DLL")); 	
	if (shellH != nil) 
        browseProc = (SHBrowseForFolderWProc) GetProcAddress(shellH, _TEXT("SHBrowseForFolderW")); 

	// Browse for the folder.
	if (WSystemInfo::HasUnicode() && browseProc != nil) 
		items = browseProc(const_cast<BROWSEINFOW*>(&info));
	else
		items = SHBrowseForFolderA((BROWSEINFOA*) &info);
 	
 	return items;		// will be nil if user canceled
}

#if __MWERKS__
#pragma mark -
#endif
	
// ===================================================================================
//	class XOpenReply
// ===================================================================================

//---------------------------------------------------------------
//
// XOpenReply::~XOpenReply
//
//---------------------------------------------------------------
XOpenReply::~XOpenReply()
{
}


//---------------------------------------------------------------
//
// XOpenReply::XOpenReply ()
//
//---------------------------------------------------------------
XOpenReply::XOpenReply()
{
	mFiltered = false;

}


//---------------------------------------------------------------
//
// XOpenReply::XOpenReply (const tagOFNA*, bool)
//
//---------------------------------------------------------------
XOpenReply::XOpenReply(const tagOFNA* reply, bool succeeded)
{
	PRECONDITION((reply->Flags & OFN_EXPLORER) != 0);		// if this is false files are seperated with spaces which means code below is wrong (and users can't include spaces in their file names)
	
	mFiltered = false;
	
	if (succeeded) {
		std::wstring path(FromPlatformStr(reply->lpstrFile));
		XFileSystemSpec spec;

		uint32 begin = path.size();

		if (reply->lpstrFile[begin] == '\0' && reply->lpstrFile[begin + 1] == '\0') {	// contrary to MSDN if there's only one file the directory and the filename are not seperated with a NULL...
			spec = XFileSystemSpec(path);

			if (XFileSystem::IsFile(spec)) 
				mSpecs.push_back(spec);
			else
				mFiltered = true;

		} else {

			++begin;
			while (begin < reply->nMaxFile && reply->lpstrFile[begin] != '\0') {
				uint32 end = begin + 1;
				while (end < reply->nMaxFile && reply->lpstrFile[end] != '\0') 
					++end;
					
				std::wstring fileName(FromPlatformStr(reply->lpstrFile + begin, end - begin));
				spec = XFileSystemSpec(path + L"\\" + fileName);
				
				if (XFileSystem::IsFile(spec)) 
					mSpecs.push_back(spec);
				else
					mFiltered = true;
					
				begin = end + 1;
			}
		}
	}
}


//---------------------------------------------------------------
//
// XOpenReply::XOpenReply (const tagOFNA*, bool, XFileFilter)
//
//---------------------------------------------------------------
XOpenReply::XOpenReply(const tagOFNA* reply, bool succeeded, const XFileFilter& fileFilter)
{
	PRECONDITION((reply->Flags & OFN_EXPLORER) != 0);		// if this is false files are seperated with spaces which means code below is wrong (and users can't include spaces in their file names)
	
	mFiltered = false;
	
	if (succeeded) {
		std::wstring path(FromPlatformStr(reply->lpstrFile));
		XFileSystemSpec spec;

		uint32 begin = path.size();

		if (reply->lpstrFile[begin] == '\0' && reply->lpstrFile[begin + 1] == '\0') {	// contrary to MSDN if there's only one file the directory and the filename are not seperated with a NULL...
			spec = XFileSystemSpec(path);

			if (XFileSystem::IsFile(spec) && fileFilter.operator()(XFileSpec(spec))) 
				mSpecs.push_back(spec);
			else
				mFiltered = true;

		} else {

			++begin;
			while (begin < reply->nMaxFile && reply->lpstrFile[begin] != '\0') {
				uint32 end = begin + 1;
				while (end < reply->nMaxFile && reply->lpstrFile[end] != '\0') 
					++end;
					
				std::wstring fileName(FromPlatformStr(reply->lpstrFile + begin, end - begin));
				spec = XFileSystemSpec(path + L"\\" + fileName);
				
				if (XFileSystem::IsFile(spec) && fileFilter.operator()(XFileSpec(spec))) 
					mSpecs.push_back(spec);
				else
					mFiltered = true;
					
				begin = end + 1;
			}
		}
	}
}


//---------------------------------------------------------------
//
// XOpenReply::XOpenReply (const tagOFNW*, bool)
//
//---------------------------------------------------------------
XOpenReply::XOpenReply(const tagOFNW* reply, bool succeeded)
{
	PRECONDITION((reply->Flags & OFN_EXPLORER) != 0);		// if this is false files are seperated with spaces which means code below is wrong (and users can't include spaces in their file names)
	
	mFiltered = false;
	
	if (succeeded) {
		std::wstring path = reply->lpstrFile;
		XFileSystemSpec spec;

		uint32 begin = path.size();

		if (reply->lpstrFile[begin] == '\0' && reply->lpstrFile[begin + 1] == '\0') {	// contrary to MSDN if there's only one file the directory and the filename are not seperated with a NULL...
			spec = XFileSystemSpec(path);

			if (XFileSystem::IsFile(spec)) 
				mSpecs.push_back(spec);
			else
				mFiltered = true;

		} else {

			++begin;
			while (begin < reply->nMaxFile && reply->lpstrFile[begin] != '\0') {
				uint32 end = begin + 1;
				while (end < reply->nMaxFile && reply->lpstrFile[end] != '\0') 
					++end;
					
				std::wstring fileName(reply->lpstrFile + begin, end - begin);
				spec = XFileSystemSpec(path + L"\\" + fileName);
				
				if (XFileSystem::IsFile(spec)) 
					mSpecs.push_back(spec);
				else
					mFiltered = true;
					
				begin = end + 1;
			}
		}
	}
}


//---------------------------------------------------------------
//
// XOpenReply::XOpenReply (const tagOFNW*, bool, XFileFilter)
//
//---------------------------------------------------------------
XOpenReply::XOpenReply(const tagOFNW* reply, bool succeeded, const XFileFilter& fileFilter)
{
	PRECONDITION((reply->Flags & OFN_EXPLORER) != 0);		// if this is false files are seperated with spaces which means code below is wrong (and users can't include spaces in their file names)
	
	mFiltered = false;
	
	if (succeeded) {
		std::wstring path = reply->lpstrFile;
		XFileSystemSpec spec;

		uint32 begin = path.size();

		if (reply->lpstrFile[begin] == '\0' && reply->lpstrFile[begin + 1] == '\0') {	// contrary to MSDN if there's only one file the directory and the filename are not seperated with a NULL...
			spec = XFileSystemSpec(path);

			if (XFileSystem::IsFile(spec) && fileFilter.operator()(XFileSpec(spec))) 
				mSpecs.push_back(spec);
			else
				mFiltered = true;

		} else {

			++begin;
			while (begin < reply->nMaxFile && reply->lpstrFile[begin] != '\0') {
				uint32 end = begin + 1;
				while (end < reply->nMaxFile && reply->lpstrFile[end] != '\0') 
					++end;
					
				std::wstring fileName(reply->lpstrFile + begin, end - begin);
				spec = XFileSystemSpec(path + L"\\" + fileName);
				
				if (XFileSystem::IsFile(spec) && fileFilter.operator()(XFileSpec(spec))) 
					mSpecs.push_back(spec);
				else
					mFiltered = true;
					
				begin = end + 1;
			}
		}
	}
}


//---------------------------------------------------------------
//
// XOpenReply::XOpenReply (_ITEMIDLIST*, XFolderFilter)
//
//---------------------------------------------------------------
XOpenReply::XOpenReply(_ITEMIDLIST* reply, const XFolderFilter& folderFilter)
{
	mFiltered = false;
	
	if (reply != nil) {							// will be nil if user canceled
		std::wstring path;
		
		bool succeeded = Whisper::GetPath(reply, &path);
		ThrowIf(!succeeded);
	
		XFileSystemSpec spec(path);
		if (XFileSystem::IsFolder(spec) && folderFilter.operator()(XFolderSpec(spec))) 
			mSpecs.push_back(spec);
		else
			mFiltered = true;
 	}
}


//---------------------------------------------------------------
//
// XOpenReply::FilteredFiles
//
//---------------------------------------------------------------
bool XOpenReply::FilteredFiles() const
{
	return mFiltered;			
}


//---------------------------------------------------------------
//
// XOpenReply::GetNumItems
//
//---------------------------------------------------------------
uint32 XOpenReply::GetNumItems() const
{
	uint32 count = mSpecs.size();
	
	return count;
}


//---------------------------------------------------------------
//
// XOpenReply::GetItem
//
//---------------------------------------------------------------
XFileSystemSpec XOpenReply::GetItem(uint32 index) const
{	
	PRECONDITION(index < this->GetNumItems());
	
	XFileSystemSpec spec = mSpecs[index];	

	return spec;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XSaveReply
// ===================================================================================

//---------------------------------------------------------------
//
// XSaveReply::~XSaveReply
//
//---------------------------------------------------------------
XSaveReply::~XSaveReply()
{
}


//---------------------------------------------------------------
//
// XSaveReply::XSaveReply ()
//
//---------------------------------------------------------------
XSaveReply::XSaveReply()
{
	mIsValid   = false;
	mReplacing = false;
}


//---------------------------------------------------------------
//
// XSaveReply::XSaveReply (const tagOFNA*, bool)
//
//---------------------------------------------------------------
XSaveReply::XSaveReply(const OPENFILENAME* reply, bool succeeded) : mSpec(FromPlatformStr(reply->lpstrFile))
{
	mIsValid   = succeeded;
	mReplacing = XFileSystem::FileExists(mSpec);
}


//---------------------------------------------------------------
//
// XSaveReply::XSaveReply (const tagOFNW*, bool)
//
//---------------------------------------------------------------
XSaveReply::XSaveReply(const tagOFNW* reply, bool succeeded) : mSpec(reply->lpstrFile)
{
	mIsValid   = succeeded;
	mReplacing = XFileSystem::FileExists(mSpec);
}


//---------------------------------------------------------------
//
// XSaveReply::GetTarget
//
//---------------------------------------------------------------
XFileSpec XSaveReply::GetTarget() const
{
	PRECONDITION(this->IsValid());
	
	return mSpec;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	struct XFileDialogOptions
// ===================================================================================

//---------------------------------------------------------------
//
// XFileDialogOptions::XFileDialogOptions
//
//---------------------------------------------------------------
XFileDialogOptions::XFileDialogOptions()
{
	COMPILE_CHECK(sizeof(OPENFILENAMEA) == sizeof(OPENFILENAMEW));
		
	mFileName[0] = '\0';
	mPrefKey = 0;

	mInitialFilter[0] = '\0';
	mInitialFilter[1] = '\0';
	mInitialDir[0] = '\0';

	lStructSize			= sizeof(OPENFILENAME);
	hwndOwner			= nil;
	hInstance			= nil;
	lpstrFilter			= nil;		
	lpstrCustomFilter	= mInitialFilter;
	nMaxCustFilter		= sizeof(mInitialFilter)/sizeof(wchar_t);
	nFilterIndex		= 1;
	lpstrFile			= mFileName;
	nMaxFile			= sizeof(mFileName)/sizeof(wchar_t);
	lpstrFileTitle		= nil;
	nMaxFileTitle		= 0;
	lpstrInitialDir		= mInitialDir;			
	lpstrTitle			= nil;
	Flags				= OFN_ALLOWMULTISELECT + OFN_EXPLORER + OFN_FILEMUSTEXIST + OFN_PATHMUSTEXIST + OFN_OVERWRITEPROMPT;
	nFileOffset			= 0;
	nFileExtension		= 0;
	lpstrDefExt			= nil;		
	lCustData			= 0;
	lpfnHook			= nil;
	lpTemplateName		= nil;
		
	std::wstring path;
	if (Whisper::GetSpecialFolder(&path, CSIDL_PERSONAL, false))	// default to starting at "My Documents" (overriden by SetDefaultFolder and the save/open functions)
		this->SetDefaultFolder(path);
}


//---------------------------------------------------------------
//
// XFileDialogOptions::SetDefaultFileName
//
//---------------------------------------------------------------
void XFileDialogOptions::SetDefaultFileName(const std::wstring& name)
{
	uint32 length = Min((uint32) name.size(), sizeof(mFileName) - 1UL);
	
	if (WSystemInfo::HasUnicode()) 
		BlockMoveData(name.c_str(), mFileName, (length+1)*sizeof(wchar_t));		
	else
		BlockMoveData(ToPlatformStr(name).c_str(), mFileName, length+1);		
}


//---------------------------------------------------------------
//
// XFileDialogOptions::SetMessage
//
//---------------------------------------------------------------
void XFileDialogOptions::SetMessage(const std::wstring& mesg)
{
	mTitle = mesg;

	if (WSystemInfo::HasUnicode()) 
		lpstrTitle = mTitle.c_str();
		
	else {
		mTitleStr = ToPlatformStr(mTitle);
		lpstrTitle = (const wchar_t*) mTitleStr.c_str();
	}
}


//---------------------------------------------------------------
//
// XFileDialogOptions::AddType
//
//---------------------------------------------------------------
void XFileDialogOptions::AddType(const XFileType& type)
{
	mTypes.push_back(type);
	
	std::wstring name = type.GetName();
	std::wstring extension = type.GetExtension();

	PRECONDITION(name.length() > 0);
	PRECONDITION(extension.length() > 0);
	PRECONDITION(extension.find('.') == std::wstring::npos);
	
	std::wstring filter = L"*." + extension;
	Whisper::Replace(filter, L";", L";*.");
	
	mExtensionList += name + std::wstring(1, '\0') + filter + std::wstring(1, '\0');

	if (WSystemInfo::HasUnicode()) 
		lpstrFilter = mExtensionList.c_str();	
			
	else {
		mExtStr = ToPlatformStr(mExtensionList);
		lpstrFilter = (const wchar_t*) mExtStr.c_str();
	}
}


//---------------------------------------------------------------
//
// XFileDialogOptions::SetDefaultExtension
//
//---------------------------------------------------------------
void XFileDialogOptions::SetDefaultExtension(const std::wstring& extension)
{
	PRECONDITION(extension.length() <= 3);
	PRECONDITION(extension.find('.') == std::wstring::npos);
	
	mDefaultExtension = extension;

	if (WSystemInfo::HasUnicode()) 
		lpstrDefExt = mDefaultExtension.c_str();		
			
	else {
		mDefExtStr = ToPlatformStr(mDefaultExtension);
		lpstrDefExt = (const wchar_t*) mDefExtStr.c_str();
	}
}


//---------------------------------------------------------------
//
// XFileDialogOptions::SetPrefKey
//
//---------------------------------------------------------------
void XFileDialogOptions::SetPrefKey(uint32 key)
{
	mPrefKey = key;
}


//---------------------------------------------------------------
//
// XFileDialogOptions::SetDefaultFolder
//
//---------------------------------------------------------------
void XFileDialogOptions::SetDefaultFolder(const XFolderSpec& folder)
{
	mDefaultPath = folder.GetPath();

	if (mDefaultPath.length() > 0) {
		ASSERT(mDefaultPath.length() <= MAX_PATH);

		if (WSystemInfo::HasUnicode()) 
			BlockMoveData(mDefaultPath.c_str(), (void*) lpstrInitialDir, (mDefaultPath.length() + 1UL)*sizeof(wchar_t));
		else
			BlockMoveData(ToPlatformStr(mDefaultPath).c_str(), (void*) lpstrInitialDir, mDefaultPath.length() + 1UL);
	}
}


//---------------------------------------------------------------
//
// XFileDialogOptions::SetAppName
//
//---------------------------------------------------------------
void XFileDialogOptions::SetAppName(const std::wstring& name)
{
	UNUSED(name);
	
	// currently unused on Windows
}


//---------------------------------------------------------------
//
// XFileDialogOptions::SetActionButtonLabel
//
//---------------------------------------------------------------
void XFileDialogOptions::SetActionButtonLabel(const std::wstring& mesg)
{
	UNUSED(mesg);
	
	// currently unused on Windows
}


//---------------------------------------------------------------
//
// XFileDialogOptions::SetCancelButtonLabel
//
//---------------------------------------------------------------
void XFileDialogOptions::SetCancelButtonLabel(const std::wstring& mesg)
{
	UNUSED(mesg);
	
	// currently unused on Windows
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XFileDialog
// ===================================================================================

//---------------------------------------------------------------
//
// XFileDialog::SaveFile (XFileDialogOptions) 			[static]
//
//---------------------------------------------------------------
XSaveReply XFileDialog::SaveFile(const XFileDialogOptions& options)
{
	PRECONDITION(options.lpstrFilter != nil);		// need at least one extension!

	const OPENFILENAMEW* ofn = &options;	// MSVC 5.0 won't let me cast away the const in one operation...
		
	// Start at the last directory the user was in
	uint32 key = options.GetPrefKey();
	std::wstring name = L"Open File Dir";
	if (key != 0)
		name += L" " + IDToStr(key);

	XPreference<std::wstring> oldDir(name, L"");
	if (oldDir->length() > 0) {
		ASSERT(oldDir->length() <= MAX_PATH);

		if (WSystemInfo::HasUnicode()) 
			BlockMoveData(oldDir->c_str(), (void*) ofn->lpstrInitialDir, (oldDir->length() + 1UL)*sizeof(wchar_t));
		else
			BlockMoveData(oldDir->c_str(), (void*) ofn->lpstrInitialDir, oldDir->length() + 1UL);
	}

	// Let the user pick a location to save with
	int32 succeeded;
	if (WSystemInfo::HasUnicode()) 
		succeeded = ::GetSaveFileNameW(const_cast<OPENFILENAMEW*>(ofn));
	else
		succeeded = ::GetSaveFileNameA((OPENFILENAMEA*) ofn);
	
	if (!succeeded) {
		uint32 err = ::CommDlgExtendedError();
		if (err != 0)						// will be zero if user canceled out of dialog
			ThrowErr(err);				
	}

	// Return the path selected by the user
	XSaveReply reply;
	if (WSystemInfo::HasUnicode()) 
		reply = XSaveReply(ofn, succeeded != 0);
	else
		reply = XSaveReply((OPENFILENAMEA*) ofn, succeeded != 0);
	
	if (reply.IsValid())
		*oldDir = reply.GetTarget().GetPath();

	return reply;
}


//---------------------------------------------------------------
//
// XFileDialog::OpenFile (XFileDialogOptions) [static]
//
//---------------------------------------------------------------
XOpenReply XFileDialog::OpenFile(const XFileDialogOptions& options)
{
	PRECONDITION(options.lpstrFilter != nil);		// need at least one extension!
		
	OPENFILENAMEW* ofn = (OPENFILENAMEW*) &options;	

	// Restore the old filter pattern last used by the user
	std::wstring name = L"Open File Extensions";
	uint32 key = options.GetPrefKey();
	if (key != 0)
		name += L" " + IDToStr(key);

	XPreference<std::wstring> extensions(name, L"");
	if (extensions->length() > 0) {
		uint32 len = Min((uint32) extensions->length(), ofn->nMaxCustFilter - 1UL);
		if (WSystemInfo::HasUnicode()) 
			BlockMoveData(extensions->c_str(), ofn->lpstrCustomFilter, (len+1)*sizeof(wchar_t));
		else
			BlockMoveData(extensions->c_str(), ofn->lpstrCustomFilter, len+1);
	}

	// Start at the last directory the user was in
	name = L"Open File Dir";
	if (key != 0)
		name += L" " + IDToStr(key);

	XPreference<std::wstring> oldDir(name, L"");
	if (oldDir->length() > 0) {
		ASSERT(oldDir->length() <= MAX_PATH);

		if (WSystemInfo::HasUnicode()) 
			BlockMoveData(oldDir->c_str(), (void*) ofn->lpstrInitialDir, (oldDir->length() + 1UL)*sizeof(wchar_t));
		else
			BlockMoveData(oldDir->c_str(), (void*) ofn->lpstrInitialDir, oldDir->length() + 1UL);
	}

	// Let the user pick a file
	int32 succeeded;
	if (WSystemInfo::HasUnicode()) 
		succeeded = ::GetOpenFileNameW(const_cast<OPENFILENAMEW*>(ofn));
	else
		succeeded = ::GetOpenFileNameA((OPENFILENAMEA*) ofn);
	
	if (!succeeded) {
		uint32 err = ::CommDlgExtendedError();
		if (err != 0)						// will be zero if user canceled out of dialog
			ThrowErr(err);				
	}

	// Return the file
	XOpenReply reply;	
	if (WSystemInfo::HasUnicode()) 
		reply = XOpenReply(ofn, succeeded != 0);
	else
		reply = XOpenReply((OPENFILENAMEA*) ofn, succeeded != 0);
	
	if (reply.GetNumItems() > 0) {
		XFileSystemSpec spec = reply.GetItem(0);

		*oldDir = spec.GetParent().GetPath();
	}

	return reply;
}


//---------------------------------------------------------------
//
// XFileDialog::OpenFile (XFileDialogOptions, XFileFilter) [static]
//
//---------------------------------------------------------------
XOpenReply XFileDialog::OpenFile(const XFileDialogOptions& options, const XFileFilter& fileFilter)
{
	PRECONDITION(options.lpstrFilter != nil);		// need at least one extension!
		
	OPENFILENAMEW* ofn = (OPENFILENAMEW*) &options;	

	// Restore the old filter pattern last used by the user
	std::wstring name = L"Open File Extensions";
	uint32 key = options.GetPrefKey();
	if (key != 0)
		name += L" " + IDToStr(key);

	XPreference<std::wstring> extensions(name, L"");
	if (extensions->length() > 0) {
		uint32 len = Min((uint32) extensions->length(), ofn->nMaxCustFilter - 1UL);
		if (WSystemInfo::HasUnicode()) 
			BlockMoveData(extensions->c_str(), ofn->lpstrCustomFilter, (len+1)*sizeof(wchar_t));
		else
			BlockMoveData(extensions->c_str(), ofn->lpstrCustomFilter, len+1);
	}

	// Start at the last directory the user was in
	name = L"Open File Dir";
	if (key != 0)
		name += L" " + IDToStr(key);

	XPreference<std::wstring> oldDir(name, L"");
	if (oldDir->length() > 0) {
		ASSERT(oldDir->length() <= MAX_PATH);

		if (WSystemInfo::HasUnicode()) 
			BlockMoveData(oldDir->c_str(), (void*) ofn->lpstrInitialDir, (oldDir->length() + 1UL)*sizeof(wchar_t));
		else
			BlockMoveData(oldDir->c_str(), (void*) ofn->lpstrInitialDir, oldDir->length() + 1UL);
	}

	// Let the user pick a file
	int32 succeeded;
	if (WSystemInfo::HasUnicode()) 
		succeeded = ::GetOpenFileNameW(const_cast<OPENFILENAMEW*>(ofn));
	else
		succeeded = ::GetOpenFileNameA((OPENFILENAMEA*) ofn);
	
	if (!succeeded) {
		uint32 err = ::CommDlgExtendedError();
		if (err != 0)						// will be zero if user canceled out of dialog
			ThrowErr(err);				
	}

	// Return the file
	XOpenReply reply;	
	if (WSystemInfo::HasUnicode()) 
		reply = XOpenReply(ofn, succeeded != 0, fileFilter);
	else
		reply = XOpenReply((OPENFILENAMEA*) ofn, succeeded != 0, fileFilter);
	
	if (reply.GetNumItems() > 0) {
		XFileSystemSpec spec = reply.GetItem(0);

		*oldDir = spec.GetParent().GetPath();
	}

	return reply;
}


//---------------------------------------------------------------
//
// XFileDialog::ChooseFile (XFileDialogOptions) 		[static]
//
//---------------------------------------------------------------
XOpenReply XFileDialog::ChooseFile(const XFileDialogOptions& options)
{
	PRECONDITION(options.lpstrFilter != nil);		// need at least one extension!
		
	OPENFILENAMEW* ofn = (OPENFILENAMEW*) &options;	

	// Restore the old filter pattern last used by the user
	std::wstring name = L"Open File Extensions";
	uint32 key = options.GetPrefKey();
	if (key != 0)
		name += L" " + IDToStr(key);

	XPreference<std::wstring> extensions(name, L"");
	if (extensions->length() > 0) {
		uint32 len = Min((uint32) extensions->length(), ofn->nMaxCustFilter - 1UL);
		if (WSystemInfo::HasUnicode()) 
			BlockMoveData(extensions->c_str(), ofn->lpstrCustomFilter, (len)*sizeof(wchar_t)+1);
		else
			BlockMoveData(extensions->c_str(), ofn->lpstrCustomFilter, len+1);
	}

	// Start at the last directory the user was in
	name = L"Open File Dir";
	if (key != 0)
		name += L" " + IDToStr(key);

	XPreference<std::wstring> oldDir(name, L"");
	if (oldDir->length() > 0) {
		ASSERT(oldDir->length() <= MAX_PATH);

		if (WSystemInfo::HasUnicode()) 
			BlockMoveData(oldDir->c_str(), (void*) ofn->lpstrInitialDir, (oldDir->length() + 1UL)*sizeof(wchar_t));
		else
			BlockMoveData(oldDir->c_str(), (void*) ofn->lpstrInitialDir, oldDir->length() + 1UL);
	}

	// Let the user pick a file
	int32 succeeded;
	if (WSystemInfo::HasUnicode()) 
		succeeded = ::GetOpenFileNameW(const_cast<OPENFILENAMEW*>(ofn));
	else
		succeeded = ::GetOpenFileNameA((OPENFILENAMEA*) ofn);
	
	if (!succeeded) {
		uint32 err = ::CommDlgExtendedError();
		if (err != 0)						// will be zero if user canceled out of dialog
			ThrowErr(err);				
	}

	// Return the file
	XOpenReply reply;	
	if (WSystemInfo::HasUnicode()) 
		reply = XOpenReply(ofn, succeeded != 0);
	else
		reply = XOpenReply((OPENFILENAMEA*) ofn, succeeded != 0);

	if (reply.GetNumItems() > 0) {
		XFileSystemSpec spec = reply.GetItem(0);

		*oldDir = spec.GetParent().GetPath();
	}

	return reply;
}


//---------------------------------------------------------------
//
// XFileDialog::ChooseFile (XFileDialogOptions, XFileFilter) [static]
//
//---------------------------------------------------------------
XOpenReply XFileDialog::ChooseFile(const XFileDialogOptions& options, const XFileFilter& fileFilter)
{
	PRECONDITION(options.lpstrFilter != nil);		// need at least one extension!
		
	OPENFILENAMEW* ofn = (OPENFILENAMEW*) &options;	

	// Restore the old filter pattern last used by the user
	std::wstring name = L"Open File Extensions";
	uint32 key = options.GetPrefKey();
	if (key != 0)
		name += L" " + IDToStr(key);

	XPreference<std::wstring> extensions(name, L"");
	if (extensions->length() > 0) {
		uint32 len = Min((uint32) extensions->length(), ofn->nMaxCustFilter - 1UL);
		if (WSystemInfo::HasUnicode()) 
			BlockMoveData(extensions->c_str(), ofn->lpstrCustomFilter, (len)*sizeof(wchar_t)+1);
		else
			BlockMoveData(extensions->c_str(), ofn->lpstrCustomFilter, len+1);
	}

	// Start at the last directory the user was in
	name = L"Open File Dir";
	if (key != 0)
		name += L" " + IDToStr(key);

	XPreference<std::wstring> oldDir(name, L"");
	if (oldDir->length() > 0) {
		ASSERT(oldDir->length() <= MAX_PATH);

		if (WSystemInfo::HasUnicode()) 
			BlockMoveData(oldDir->c_str(), (void*) ofn->lpstrInitialDir, (oldDir->length() + 1UL)*sizeof(wchar_t));
		else
			BlockMoveData(oldDir->c_str(), (void*) ofn->lpstrInitialDir, oldDir->length() + 1UL);
	}

	// Let the user pick a file
	int32 succeeded;
	if (WSystemInfo::HasUnicode()) 
		succeeded = ::GetOpenFileNameW(const_cast<OPENFILENAMEW*>(ofn));
	else
		succeeded = ::GetOpenFileNameA((OPENFILENAMEA*) ofn);
	
	if (!succeeded) {
		uint32 err = ::CommDlgExtendedError();
		if (err != 0)						// will be zero if user canceled out of dialog
			ThrowErr(err);				
	}

	// Return the file
	XOpenReply reply;	
	if (WSystemInfo::HasUnicode()) 
		reply = XOpenReply(ofn, succeeded != 0, fileFilter);
	else
		reply = XOpenReply((OPENFILENAMEA*) ofn, succeeded != 0, fileFilter);

	if (reply.GetNumItems() > 0) {
		XFileSystemSpec spec = reply.GetItem(0);

		*oldDir = spec.GetParent().GetPath();
	}

	return reply;
}


//---------------------------------------------------------------
//
// XFileDialog::ChooseFolder (XFileDialogOptions) 		[static]
//
//---------------------------------------------------------------
XOpenReply XFileDialog::ChooseFolder(const XFileDialogOptions& options)
{
	XFolderFilter filter;

	return XFileDialog::ChooseFolder(options, filter);
}

//---------------------------------------------------------------
//
// XFileDialog::ChooseFolder (XFileDialogOptions, XFileDialogOptions) [static]
//
//---------------------------------------------------------------
XOpenReply XFileDialog::ChooseFolder(const XFileDialogOptions& options, const XFolderFilter& folderFilter)
{
	COMPILE_CHECK(sizeof(BROWSEINFOA) == sizeof(BROWSEINFOW));
	
	SFolderInfo myInfo;
	myInfo.options = &options;
	myInfo.filter  = &folderFilter;	 
	
	BROWSEINFOW info;
    info.hwndOwner      = nil;
    info.pidlRoot       = nil;					
    info.pszDisplayName = options.lpstrFile;
    info.lpszTitle      = options.lpstrTitle;
    info.ulFlags        = BIF_RETURNONLYFSDIRS;
    info.lpfn           = XFileDialog::DoFolderCallback;	
    info.lParam         = (LPARAM) &myInfo;
	info.iImage         = 0;
	
	ITEMIDLIST* items = Whisper::BrowseForFolder(info);	
	XOpenReply reply(items, folderFilter);
	
	if (reply.GetNumItems() > 0) {
		std::wstring name = L"Browse Folder";

		uint32 key = options.GetPrefKey();
		if (key != 0)
			name += L" " + IDToStr(key);

		XFileSystemSpec spec = reply.GetItem(0);

		XPreference<std::wstring> path(name, L"");
		*path = spec.GetPath();
	}

	IMalloc* allocator; 
	if (SHGetMalloc(&allocator) == NOERROR && allocator != nil) { 
		allocator->Free(items); 
		allocator->Release(); 
	} 

	return reply;
}


//---------------------------------------------------------------
//
// XFileDialog::ChooseObject (XFileDialogOptions)		[static]
//
//---------------------------------------------------------------
#if 0			// this works on Win98, but not NT 4 (or, I think, Win95)
XOpenReply XFileDialog::ChooseObject(const XFileDialogOptions& options)
{
	COMPILE_CHECK(sizeof(BROWSEINFOA) == sizeof(BROWSEINFOW));
	
	SFileFolderInfo myInfo;
	myInfo.options      = &options;
	myInfo.fileFilter   = nil;	
	myInfo.folderFilter = nil;	 

	BROWSEINFOW info;
    info.hwndOwner      = nil;
    info.pidlRoot       = nil;					
    info.pszDisplayName = options.lpstrFile;
    info.lpszTitle      = options.lpstrTitle;
    info.ulFlags        = BIF_BROWSEINCLUDEFILES;
    info.lpfn           = XFileDialog::DoFileFolderCallback;	
    info.lParam         = (LPARAM) &myInfo;
	info.iImage         = 0;

	ITEMIDLIST* items = Whisper::BrowseForFolder(info);		
	XOpenReply reply(items, folderFilter);

	if (reply.GetNumItems() > 0) {
		std::wstring name = L"Browse Folder";
		uint32 key = options.GetPrefKey();
		if (key != 0)
			name += L" " + IDToStr(key);

		XFileSystemSpec spec = reply.GetItem(0);

		XPreference<std::wstring> path(name, L"");
		if (XFileSystem::IsFolder(spec))
			*path = spec.GetPath();
		else
			*path = spec.GetParent().GetPath();
	}

	IMalloc* allocator; 
	if (SHGetMalloc(&allocator) == NOERROR && allocator != nil) { 
		allocator->Free(items); 
		allocator->Release(); 
	} 

	return reply;
}
#endif


//---------------------------------------------------------------
//
// XFileDialog::ChooseObject (XFileDialogOptions, XFileFilter, XFolderFilter) [static]
//
//---------------------------------------------------------------
#if 0
XOpenReply XFileDialog::ChooseObject(const XFileDialogOptions& options, const XFileFilter& fileFilter, const XFolderFilter& folderFilter)
{
	COMPILE_CHECK(sizeof(BROWSEINFOA) == sizeof(BROWSEINFOW));
	
	SFileFolderInfo myInfo;
	myInfo.options      = &options;
	myInfo.fileFilter   = &fileFilter;	 
	myInfo.folderFilter = &folderFilter;	 

	BROWSEINFOW info;
    info.hwndOwner      = nil;
    info.pidlRoot       = nil;					
    info.pszDisplayName = options.lpstrFile;
    info.lpszTitle      = options.lpstrTitle;
    info.ulFlags        = BIF_BROWSEINCLUDEFILES;
    info.lpfn           = XFileDialog::DoFileFolderCallback;	
    info.lParam         = (LPARAM) &myInfo;
	info.iImage         = 0;

	ITEMIDLIST* items = Whisper::BrowseForFolder(info);	
	XOpenReply reply(items, fileFilter, folderFilter);

	if (reply.GetNumItems() > 0) {
		std::wstring name = L"Browse Folder";
		uint32 key = options.GetPrefKey();
		if (key != 0)
			name += L" " + IDToStr(key);

		XFileSystemSpec spec = reply.GetItem(0);

		XPreference<std::wstring> path(name, L"");
		if (XFileSystem::IsFolder(spec))
			*path = spec.GetPath();
		else
			*path = spec.GetParent().GetPath();
	}

	IMalloc* allocator; 
	if (SHGetMalloc(&allocator) == NOERROR && allocator != nil) { 
		allocator->Free(items); 
		allocator->Release(); 
	} 

	return reply;
}
#endif


//---------------------------------------------------------------
//
// XFileDialog::AskSaveChanges							[static]
//
//---------------------------------------------------------------
EAskSaveResult XFileDialog::AskSaveChanges(const std::wstring& docName, ESaveChanges action)
{
	UNUSED(action);

	EAskSaveResult result = kCancelSave;

	std::wstring text = L"' " + docName + L"' hasn't been saved. Would you like to save it?";	
	
	uint32 flags = MB_YESNOCANCEL +		// want Yes, No, and Cancel buttons
				   MB_ICONINFORMATION +	// display the info icon (the question mark icon is no longer recommended)
				   MB_DEFBUTTON1 +		// select the Yes button
				   MB_TASKMODAL +		// don't let the user do anything else in the app
				   MB_SETFOREGROUND;	// bring the app to the front

	int32 item;
	if (WSystemInfo::HasUnicode()) 
		item = MessageBoxW(nil, text.c_str(), L"", flags);
	else
		item = MessageBoxA(nil, ToPlatformStr(text).c_str(), "", flags);
		
	if (item == IDYES)
		result = kSaveChanges;
	else if (item == IDNO)
		result = kDontSave;

	return result;
}


//---------------------------------------------------------------
//
// XFileDialog::AskRevertChanges						[static]
//
//---------------------------------------------------------------
ERevertResult XFileDialog::AskRevertChanges(const std::wstring& docName)
{
	ERevertResult result = kCancelRevert;

	std::wstring text = L"Are you sure you want to discard your changes to ' " + docName + L"'?";	
	
	uint32 flags = MB_YESNO +			// want Yes and No buttons
				   MB_ICONINFORMATION +	// display the info icon (the question mark icon is no longer recommended)
				   MB_DEFBUTTON2 +		// select the No button
				   MB_TASKMODAL +		// don't let the user do anything else in the app
				   MB_SETFOREGROUND;	// bring the app to the front

	int32 item;
	if (WSystemInfo::HasUnicode()) 
		item = MessageBoxW(nil, text.c_str(), L"", flags);
	else
		item = MessageBoxA(nil, ToPlatformStr(text).c_str(), "", flags);
		
	if (item == IDYES)
		result = kDiscardChanges;

	return result;
}


//---------------------------------------------------------------
//
// XFileDialog::DoFolderCallback						[static]
//
//---------------------------------------------------------------
int CALLBACK XFileDialog::DoFolderCallback(HWND wind, UINT message, LPARAM param, LPARAM refCon)
{
	const SFolderInfo* info = reinterpret_cast<const SFolderInfo*>(refCon);

	PRECONDITION(info != nil);
	PRECONDITION(info->options != nil);
	PRECONDITION(info->filter != nil);

	switch(message) {
		case BFFM_INITIALIZED: 
			{
				std::wstring name = L"Browse Folder";
				uint32 key = info->options->GetPrefKey();
				if (key != 0)
					name += L" " + IDToStr(key);

				XPreference<std::wstring> path(name, L"");
				if (path->length() > 0)
					if (WSystemInfo::HasUnicode()) 
						SendMessageW(wind, BFFM_SETSELECTION, true, (LPARAM) path->c_str());
					else
						SendMessageA(wind, BFFM_SETSELECTION, true, (LPARAM) path->c_str());
			}
			break;

		case BFFM_SELCHANGED: 
			{
				bool enable = false;
				
				std::wstring path;
				if (Whisper::GetPath((LPITEMIDLIST) param, &path)) {
					XFileSystemSpec spec(path);
					if (XFileSystem::IsFolder(spec)) 
						enable = info->filter->operator()(XFolderSpec(spec));
				}
								
				SendMessage(wind, BFFM_ENABLEOK, 0, enable);
			}
			break;
	}

	return 0;
}


//---------------------------------------------------------------
//
// XFileDialog::DoFileFolderCallback					[static]
//
//---------------------------------------------------------------
int CALLBACK XFileDialog::DoFileFolderCallback(HWND wind, UINT message, LPARAM param, LPARAM refCon)
{
	const SFileFolderInfo* info = reinterpret_cast<const SFileFolderInfo*>(refCon);
	PRECONDITION(info != nil);

	switch(message) {
		case BFFM_INITIALIZED: 
			{
				std::wstring name = L"Browse Object";
				uint32 key = info->options->GetPrefKey();

				if (key != 0)
					name += L" " + IDToStr(key);

				XPreference<std::wstring> path(name, L"");
				if (path->length() > 0)
					if (WSystemInfo::HasUnicode()) 
						SendMessageW(wind, BFFM_SETSELECTION, true, (LPARAM) path->c_str());
					else
						SendMessageA(wind, BFFM_SETSELECTION, true, (LPARAM) path->c_str());
			}
			break;

		case BFFM_SELCHANGED: 
			{
				bool enable = false;

				std::wstring path;
				if (Whisper::GetPath((LPITEMIDLIST) param, &path)) {
					XFileSystemSpec spec(path);

					if (XFileSystem::IsFolder(spec)) 
						enable = info->folderFilter == nil || info->folderFilter->operator()(XFolderSpec(spec));
					else
						enable = info->fileFilter == nil || info->fileFilter->operator()(XFileSpec(spec));
				}
					
				SendMessage(wind, BFFM_ENABLEOK, 0, enable);
			}
			break;
	}

	return 0;
}


}	// namespace Whisper

