/*
 *  File:		WFileSystem.cpp
 *  Summary:	Some handy functions for dealing with the file system.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WFileSystem.cpp,v $
 *		Revision 1.8  2001/04/27 09:18:36  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.7  2001/04/21 03:31:48  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/03/05 05:39:55  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.5  2000/11/27 03:28:38  jesjones
 *		GetTempFile takes an extension argument. GetTempFile increments the time instead of getting it anew each time through the loop.
 *		
 *		Revision 1.4  2000/11/25 01:47:38  jesjones
 *		Fixed IsFAT.
 *		
 *		Revision 1.3  2000/11/20 05:37:43  jesjones
 *		Added std:: to a few functions calls.
 *		
 *		Revision 1.2  2000/11/09 12:07:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <7>	 4/02/02	JDJ		DeleteFolder sets the current directory before calling RemoveDirectory.
 *		 <6>	12/12/99	JDJ		GetValidName uses three periods instad of the bogus L"�" when
 *									the name has to be shortened.
 *		 <5>	 6/21/99	JDJ		Added IsFAT and IsNTFS. IsValidChar allows printable Unicode
 *									chars on NTFS.
 *		 <4>	 5/07/99	JDJ		Added IsWritable.
 *		 <3>	 3/12/99	JDJ		RenameFile and RenameFolder no longer modify the spec argument.
 *		 <2>	 2/18/99	JDJ		Fixed a bug that could cause GetValidName to index
 *									past the end of the file name if the number of characters
 *									to drop was odd.
 *		 <1>	 1/10/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XFileSystem.h>

#include <mmsystem.h>

#include <WSystemInfo.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XFolderSpec.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// EmptyFolder
//
// Deletes everything within the folder.
//
//---------------------------------------------------------------
static void EmptyFolder(const XFolderSpec& parent)
{
	std::wstring searchString = parent.GetPath() + L"\\*.*";
	
	HANDLE hand = nil;
	if (WSystemInfo::HasUnicode()) {
		WIN32_FIND_DATAW info;
		hand = ::FindFirstFileW(searchString.c_str(), &info);
		ThrowIfBadHandle(hand);
	
		int32 succeeded = true;
		while (succeeded) {	
			if ((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && info.cFileName[0] != '.') {
				XFolderSpec folder(parent, info.cFileName);
				XFileSystem::DeleteFolder(folder);
			
			} else if (info.cFileName[0] != '.') {
				XFileSpec file(parent, info.cFileName);
				XFileSystem::DeleteFile(file);
			}
			
			succeeded = ::FindNextFileW(hand, &info);
		}
	
	} else {
		WIN32_FIND_DATAA info;
		hand = ::FindFirstFileA(ToPlatformStr(searchString).c_str(), &info);
		ThrowIfBadHandle(hand);
	
		int32 succeeded = true;
		while (succeeded) {	
			if ((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && info.cFileName[0] != '.') {
				XFolderSpec folder(parent, FromPlatformStr(info.cFileName));
				XFileSystem::DeleteFolder(folder);
			
			} else if (info.cFileName[0] != '.') {
				XFileSpec file(parent, FromPlatformStr(info.cFileName));
				XFileSystem::DeleteFile(file);
			}
			
			succeeded = ::FindNextFileA(hand, &info);
		}
	}
	
	(void) ::FindClose(hand);
}


//---------------------------------------------------------------
//
// IsValidChar	
//
// This is from the InfoViewer topic "Long Filenames and the 
// Protected-Mode FAT File System" (it appears that these rules
// also hold for NTFS).
//
//---------------------------------------------------------------
static bool IsValidChar(uint16 ch, bool hasUnicode)	
{
	bool valid = false;
	
	if (ch >= 'a' && ch <= 'z')
		valid = true;
	
	else if (ch >= 'A' && ch <= 'Z')
		valid = true;
	
	else if (ch >= '0' && ch <= '9')
		valid = true;
				
	else if (ch == ' ')
		valid = true;

	else if (hasUnicode && ch >= 128 && IsPrintable(ch))
		valid = true;

	else if (!hasUnicode && ch >= 128 && ch <= 255)
		valid = true;
		
	else if (ch == '$' || ch == '%' || ch == '\'' || ch == '-' || ch == '_' || 
			 ch == '@' || ch == '~' || ch == '`'  || ch == '!' || ch == '(' || 
			 ch == ')' || ch == '{' || ch == '}'  || ch == '^' || ch == '#' || 
			 ch == '&' || ch == '+' || ch == ','  || ch == ';' || ch == '=' || 
			 ch == '[' || ch == ']')
		valid = true;
				
	return valid;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XFileSystem
// ===================================================================================

//---------------------------------------------------------------
//
// XFileSystem::CreateFolder							[static]
//
//---------------------------------------------------------------
void XFileSystem::CreateFolder(const XFolderSpec& folder)
{
	PRECONDITION(!XFileSystem::FolderExists(folder));

	if (!XFileSystem::FolderExists(folder)) {
		int32 succeeded;
		if (WSystemInfo::HasUnicode())
			succeeded = ::CreateDirectoryW(folder.GetPath().c_str(), nil);
		else
			succeeded = ::CreateDirectoryA(ToPlatformStr(folder.GetPath()).c_str(), nil);
		ThrowIf(!succeeded);
	}
}

			
//---------------------------------------------------------------
//
// XFileSystem::RenameFolder							[static]
//
//---------------------------------------------------------------
void XFileSystem::RenameFolder(const XFolderSpec& oldFolder, const std::wstring& inNewName)
{
	PRECONDITION(XFileSystem::FolderExists(oldFolder));
	PRECONDITION(inNewName.length() > 0);
	
	std::wstring newName = XFileSystem::GetValidName(inNewName, oldFolder.GetVolume());
	
	XFolderSpec newFolder(oldFolder.GetParent(), newName);
	
	int32 succeeded;
	if (WSystemInfo::HasUnicode())
		succeeded = ::MoveFileW(oldFolder.GetPath().c_str(), newFolder.GetPath().c_str());
	else
		succeeded = ::MoveFileA(ToPlatformStr(oldFolder.GetPath()).c_str(), ToPlatformStr(newFolder.GetPath()).c_str());
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XFileSystem::DeleteFolder							[static]
//
//---------------------------------------------------------------
void XFileSystem::DeleteFolder(const XFolderSpec& folder)
{
	PRECONDITION(XFileSystem::FolderExists(folder));

	EmptyFolder(folder);
	
	int32 succeeded;
	if (WSystemInfo::HasUnicode())		// if the current directory is "folder" RemoveDirectory will fail with a weird error message...
		succeeded = ::SetCurrentDirectoryW(folder.GetParent().GetPath().c_str());
	else
		succeeded = ::SetCurrentDirectoryA(ToPlatformStr(folder.GetParent().GetPath()).c_str());
	ThrowIf(!succeeded);
	
	if (WSystemInfo::HasUnicode())
		succeeded = ::RemoveDirectoryW(folder.GetPath().c_str());
	else
		succeeded = ::RemoveDirectoryA(ToPlatformStr(folder.GetPath()).c_str());
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XFileSystem::FolderExists							[static]
//
//---------------------------------------------------------------
bool XFileSystem::FolderExists(const XFileSystemSpec& folder)
{
	DWORD attributes;
	if (WSystemInfo::HasUnicode())
		attributes = ::GetFileAttributesW(folder.GetPath().c_str());
	else
		attributes = ::GetFileAttributesA(ToPlatformStr(folder.GetPath()).c_str());
		
	bool exists = attributes != (DWORD) -1L && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	
	return exists;
}


//---------------------------------------------------------------
//
// XFileSystem::GetTempFolder							[static]
//
//---------------------------------------------------------------
XFolderSpec XFileSystem::GetTempFolder(const std::wstring& prefix)
{
	XFolderSpec tempItem;
	
	XFolderSpec tempFolder = XFolderSpec::GetTempFolder();
	
	do {
		char buffer[32];
		std::sprintf(buffer, "%d", timeGetTime());

		std::wstring fileName = prefix + L" " + FromPlatformStr(buffer);
		fileName = XFileSystem::GetValidName(fileName, tempFolder.GetVolume());	// may need to truncate (this happens from the middle so we should exit the loop)
		
		tempItem = XFolderSpec(tempFolder, fileName);
	} while (XFileSystem::FolderExists(tempItem));

	return tempItem;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XFileSystem::CreateFile								[static]
//
//---------------------------------------------------------------
void XFileSystem::CreateFile(const XFileSpec& file, OSType creator, OSType fileType)
{
	PRECONDITION(!XFileSystem::FileExists(file));
	UNUSED(creator);
	UNUSED(fileType);

	int32 succeeded;
	if (WSystemInfo::HasUnicode())
		succeeded = ::SetCurrentDirectoryW(file.GetParent().GetPath().c_str());
	else
		succeeded = ::SetCurrentDirectoryA(ToPlatformStr(file.GetParent().GetPath()).c_str());
	ThrowIf(!succeeded);
	
	HANDLE hand;
	if (WSystemInfo::HasUnicode())
		hand = ::CreateFileW(file.GetName().c_str(), GENERIC_WRITE, 0, nil, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nil);
	else
		hand = ::CreateFileA(ToPlatformStr(file.GetName()).c_str(), GENERIC_WRITE, 0, nil, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nil);
	ThrowIfBadHandle(hand);

	succeeded = ::CloseHandle(hand);
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XFileSystem::RenameFile								[static]
//
//---------------------------------------------------------------
void XFileSystem::RenameFile(const XFileSpec& oldFile, const std::wstring& inNewName)
{
	PRECONDITION(XFileSystem::FileExists(oldFile));
	PRECONDITION(inNewName.length() > 0);
	
	std::wstring newName = XFileSystem::GetValidName(inNewName, oldFile.GetVolume());
	
	XFileSpec newFile(oldFile.GetParent(), newName);
	
	int32 succeeded;
	if (WSystemInfo::HasUnicode())
		succeeded = ::MoveFileW(oldFile.GetPath().c_str(), newFile.GetPath().c_str());
	else
		succeeded = ::MoveFileA(ToPlatformStr(oldFile.GetPath()).c_str(), ToPlatformStr(newFile.GetPath()).c_str());
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XFileSystem::CopyFile								[static]
//
//---------------------------------------------------------------
void XFileSystem::CopyFile(const XFileSpec& srcFile, const XFileSpec& dstFile, bool preflight)
{
	UNUSED(preflight);
	
#if _WIN32_WINNT >= 0x0400
	int32 succeeded;
	if (WSystemInfo::HasUnicode())
		succeeded = ::CopyFileExW(srcFile.GetPath().c_str(), 	// existing file
								  dstFile.GetPath().c_str(),	// new file
								  nil,							// progress routine
								  nil,							// refCon
								  nil,							// cancel flag
								  COPY_FILE_FAIL_IF_EXISTS);	// copy flags
	else
		succeeded = ::CopyFileExA(ToPlatformStr(srcFile.GetPath()), 	// existing file
								  ToPlatformStr(dstFile.GetPath()),	// new file
								  nil,							// progress routine
								  nil,							// refCon
								  nil,							// cancel flag
								  COPY_FILE_FAIL_IF_EXISTS);	// copy flags
#else
	int32 succeeded;
	if (WSystemInfo::HasUnicode())
		succeeded = ::CopyFileW(srcFile.GetPath().c_str(), 		// existing file
						        dstFile.GetPath().c_str(),		// new file
							    true);							// fail if new file exists
	else
		succeeded = ::CopyFileA(ToPlatformStr(srcFile.GetPath()).c_str(), 		// existing file
						        ToPlatformStr(dstFile.GetPath()).c_str(),		// new file
							    true);							// fail if new file exists
#endif
	
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XFileSystem::DeleteFile								[static]
//
//---------------------------------------------------------------
void XFileSystem::DeleteFile(const XFileSpec& file)
{
	PRECONDITION(XFileSystem::FileExists(file));

	int32 succeeded;
	if (WSystemInfo::HasUnicode())
		succeeded = ::DeleteFileW(file.GetPath().c_str());
	else
		succeeded = ::DeleteFileA(ToPlatformStr(file.GetPath()).c_str());
	ThrowIf(!succeeded);
}


//---------------------------------------------------------------
//
// XFileSystem::FileExists								[static]
//
//---------------------------------------------------------------
bool XFileSystem::FileExists(const XFileSystemSpec& file)
{
	DWORD attributes;
	if (WSystemInfo::HasUnicode())
		attributes = ::GetFileAttributesW(file.GetPath().c_str());
	else
		attributes = ::GetFileAttributesA(ToPlatformStr(file.GetPath()).c_str());
		
	bool exists = attributes != (DWORD) -1L && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
	
	return exists;
}


//---------------------------------------------------------------
//
// XFileSystem::IsWritable								[static]
//
//---------------------------------------------------------------
bool XFileSystem::IsWritable(const XFileSpec& file)
{
	bool writable = false;
	
	int32 succeeded;
	if (WSystemInfo::HasUnicode())
		succeeded = ::SetCurrentDirectoryW(file.GetParent().GetPath().c_str());
	else
		succeeded = ::SetCurrentDirectoryA(ToPlatformStr(file.GetParent().GetPath()).c_str());
	
	if (succeeded) {
		HANDLE handle = nil;

		// File is writable if we can open it with write permission.
		if (WSystemInfo::HasUnicode())
			handle = ::CreateFileW(file.GetName().c_str(), GENERIC_WRITE, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nil);
		else
			handle = ::CreateFileA(ToPlatformStr(file.GetName()).c_str(), GENERIC_WRITE, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nil);
		
		// If the file doesn't exist yet we need to make sure that 
		// the volume isn't locked.
		if (handle == nil || handle == INVALID_HANDLE_VALUE) {
			if (WSystemInfo::HasUnicode())
				handle = ::CreateFileW(file.GetName().c_str(), GENERIC_WRITE, 0, nil, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, nil);
			else
				handle = ::CreateFileA(ToPlatformStr(file.GetName()).c_str(), GENERIC_WRITE, 0, nil, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, nil);
		}
		
		if (handle != nil && handle != INVALID_HANDLE_VALUE) {
			writable = true;
			(void) ::CloseHandle(handle);
		}
	}
			
	return writable;
}


//---------------------------------------------------------------
//
// XFileSystem::GetFileSize								[static]
//
//---------------------------------------------------------------
uint64 XFileSystem::GetFileSize(const XFileSpec& file)
{	
	uint64 bytes;

	HANDLE hand;
	if (WSystemInfo::HasUnicode()) {
		WIN32_FIND_DATAW info;
		hand = ::FindFirstFileW(file.GetPath().c_str(), &info);
		ThrowIfBadHandle(hand);
	
		bytes = info.nFileSizeHigh;
		bytes = (bytes << 32) + info.nFileSizeLow;
	
	} else {
		WIN32_FIND_DATAA info;
		hand = ::FindFirstFileA(ToPlatformStr(file.GetPath()).c_str(), &info);
		ThrowIfBadHandle(hand);
	
		bytes = info.nFileSizeHigh;
		bytes = (bytes << 32) + info.nFileSizeLow;
	}

	VERIFY(::FindClose(hand));	
	
	return bytes;	
}


//---------------------------------------------------------------
//
// XFileSystem::GetTempFile								[static]
//
// $$$ Might be better to use GetTempFileName, but the documentation
// $$$ suggests that it doesn't take advantage of long file names.
// $$$ But there is a GetTempFileName32...
//
//---------------------------------------------------------------
XFileSpec XFileSystem::GetTempFile(const std::wstring& prefix, const std::wstring& extension)
{
	XFileSpec tempFile;
	
	XFolderSpec tempFolder = XFolderSpec::GetTempFolder();
	
	uint32 time = timeGetTime();
	
	do {		
		char buffer[32];
		std::sprintf(buffer, "%d", time++);

		std::wstring fileName = prefix + FromAsciiStr(buffer) + L"." + extension;
		fileName = XFileSystem::GetValidName(fileName, tempFolder.GetVolume());	// may need to truncate (this happens from the middle so we should exit the loop)
		
		tempFile = XFileSpec(tempFolder, fileName);
	} while (XFileSystem::SpecExists(tempFile));

	return tempFile;
}


//---------------------------------------------------------------
//
// XFileSystem::GetUniqueFile							[static]
//
//---------------------------------------------------------------
XFileSpec XFileSystem::GetUniqueFile(const XFileSpec& srcSpec)
{
	XFileSpec dstSpec = srcSpec;
	
	if (XFileSystem::SpecExists(dstSpec)) {
		std::wstring dstName = srcSpec.GetName() + LoadWhisperString(L" copy (duplicate file)");
		dstName = XFileSystem::GetValidName(dstName, dstSpec.GetVolume());		// truncate if need be
		dstSpec = XFileSpec(srcSpec.GetParent(), dstName);		
		
		int32 count = 2;										// Finder names the second copy "Hello copy 2"
		while (XFileSystem::SpecExists(dstSpec)) {				
			char buffer[32];
			std::sprintf(buffer, "%d", count++);
			
			std::wstring name = dstName + L" " + FromPlatformStr(buffer);
			name = XFileSystem::GetValidName(name, dstSpec.GetVolume());		// truncate if need be (this happens from the middle so we should exit the loop)
			
			dstSpec = XFileSpec(srcSpec.GetParent(), name);
		}
	}
	
	return dstSpec;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XFileSystem::SpecExists								[static]
//
//---------------------------------------------------------------
bool XFileSystem::SpecExists(const XFileSystemSpec& spec)
{
	DWORD attributes;
	if (WSystemInfo::HasUnicode())
		attributes = ::GetFileAttributesW(spec.GetPath().c_str());
	else
		attributes = ::GetFileAttributesA(ToPlatformStr(spec.GetPath()).c_str());
	
	return attributes != (DWORD) -1L;
}


//---------------------------------------------------------------
//
// XFileSystem::IsFile									[static]
//
//---------------------------------------------------------------
bool XFileSystem::IsFile(const XFileSystemSpec& spec)
{
	bool isFile = false;

	DWORD attributes;
	if (WSystemInfo::HasUnicode())
		attributes = ::GetFileAttributesW(spec.GetPath().c_str());
	else
		attributes = ::GetFileAttributesA(ToPlatformStr(spec.GetPath()).c_str());

	if (attributes != (DWORD) -1L)
		isFile = (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
	
	return isFile;
}


//---------------------------------------------------------------
//
// XFileSystem::IsFolder								[static]
//
//---------------------------------------------------------------
bool XFileSystem::IsFolder(const XFileSystemSpec& spec)
{
	bool isFolder = false;

	DWORD attributes;
	if (WSystemInfo::HasUnicode())
		attributes = ::GetFileAttributesW(spec.GetPath().c_str());
	else
		attributes = ::GetFileAttributesA(ToPlatformStr(spec.GetPath()).c_str());

	if (attributes != (DWORD) -1L)
		isFolder = (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	
	return isFolder;
}


//---------------------------------------------------------------
//
// XFileSystem::IsReadOnly								[static]
//
//---------------------------------------------------------------
bool XFileSystem::IsReadOnly(const XFileSystemSpec& spec)
{
	DWORD attributes;
	if (WSystemInfo::HasUnicode())
		attributes = ::GetFileAttributesW(spec.GetPath().c_str());
	else
		attributes = ::GetFileAttributesA(ToPlatformStr(spec.GetPath()).c_str());
	ThrowIf(attributes == (DWORD) -1L);
	
	bool isFolder = (attributes & FILE_ATTRIBUTE_READONLY) != 0;
	
	return isFolder;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XFileSystem::GetVolumeTotalBytes						[static]
//
//---------------------------------------------------------------
uint64 XFileSystem::GetVolumeTotalBytes(const OSVolume& volume)
{
	std::wstring path = volume + L":\\";
	
	ULARGE_INTEGER bytesAvail;			// free bytes that user can use
	ULARGE_INTEGER totalBytes;			// total bytes on drive
	ULARGE_INTEGER totalFreeBytes;		// total free bytes on drive
	
	int32 succeeded;
	if (WSystemInfo::HasUnicode())
		succeeded = ::GetDiskFreeSpaceExW(path.c_str(), &bytesAvail, &totalBytes, &totalFreeBytes);
	else
		succeeded = ::GetDiskFreeSpaceExA(ToPlatformStr(path).c_str(), &bytesAvail, &totalBytes, &totalFreeBytes);
	ThrowIf(!succeeded);
	
	uint64 bytes = totalBytes.HighPart;
	bytes = (bytes << 32) + totalBytes.LowPart;

	return bytes;
}


//---------------------------------------------------------------
//
// XFileSystem::GetVolumeFreeBytes						[static]
//
//---------------------------------------------------------------
uint64 XFileSystem::GetVolumeFreeBytes(const OSVolume& volume)
{
	std::wstring path = volume + L":\\";
	
	ULARGE_INTEGER bytesAvail;			// free bytes that user can use
	ULARGE_INTEGER totalBytes;			// total bytes on drive
	ULARGE_INTEGER totalFreeBytes;		// total free bytes on drive
	
	int32 succeeded;
	if (WSystemInfo::HasUnicode())
		succeeded = ::GetDiskFreeSpaceExW(path.c_str(), &bytesAvail, &totalBytes, &totalFreeBytes);
	else
		succeeded = ::GetDiskFreeSpaceExA(ToPlatformStr(path).c_str(), &bytesAvail, &totalBytes, &totalFreeBytes);
	ThrowIf(!succeeded);
	
	uint64 bytes = bytesAvail.HighPart;
	bytes = (bytes << 32) + bytesAvail.LowPart;

	return bytes;
}


//---------------------------------------------------------------
//
// XFileSystem::GetMaxFileSpecLength					[static]
//
//---------------------------------------------------------------
uint32 XFileSystem::GetMaxFileSpecLength(const OSVolume& volume)
{
	std::wstring path = volume + L":\\";
	
	uint32 maxCompLen, attributes;
		
	int32 succeeded;
	if (WSystemInfo::HasUnicode()) {
		wchar_t buffer[MAX_PATH + 1];
		succeeded = ::GetVolumeInformationW(path.c_str(), buffer, sizeof(buffer), nil, &maxCompLen, &attributes, nil, 0);
	
	} else {
		char buffer[MAX_PATH + 1];
		succeeded = ::GetVolumeInformationA(ToPlatformStr(path).c_str(), buffer, sizeof(buffer), nil, &maxCompLen, &attributes, nil, 0);
	}
	ThrowIf(!succeeded);
	
	return maxCompLen;
}


//---------------------------------------------------------------
//
// XFileSystem::GetVolumeName							[static]
//
//---------------------------------------------------------------
std::wstring XFileSystem::GetVolumeName(const OSVolume& volume)
{
	std::wstring name;
	std::wstring path = volume + L":\\";
		
	uint32 maxCompLen, attributes;
	
	int32 succeeded;
	if (WSystemInfo::HasUnicode()) {
		wchar_t buffer[MAX_PATH + 1];
		succeeded = ::GetVolumeInformationW(path.c_str(), buffer, sizeof(buffer), nil, &maxCompLen, &attributes, nil, 0);
	
		name = buffer;

	} else {
		char buffer[MAX_PATH + 1];
		succeeded = ::GetVolumeInformationA(ToPlatformStr(path).c_str(), buffer, sizeof(buffer), nil, &maxCompLen, &attributes, nil, 0);
	
		name = FromPlatformStr(buffer);
	}
	ThrowIf(!succeeded);
	
	return name;
}


//---------------------------------------------------------------
//
// XFileSystem::IsFAT									[static]
//
//---------------------------------------------------------------
bool XFileSystem::IsFAT(const OSVolume& volume)
{
	std::wstring path = volume + L":\\";
		
	uint32 maxCompLen, attributes;
	
	char buffer[MAX_PATH + 1];
	int32 succeeded = ::GetVolumeInformationA(ToPlatformStr(path).c_str(), nil, 0, nil, &maxCompLen, &attributes, buffer, sizeof(buffer));
	ThrowIf(!succeeded);

	bool is = ConvertToLowerCase(FromPlatformStr(buffer)) == L"fat" ||
	          ConvertToLowerCase(FromPlatformStr(buffer)) == L"fat32" ;		// CodeWarrior doesn't have stricmp...
	
	return is;
}


//---------------------------------------------------------------
//
// XFileSystem::IsNTFS									[static]
//
//---------------------------------------------------------------
bool XFileSystem::IsNTFS(const OSVolume& volume)
{
	std::wstring path = volume + L":\\";
		
	uint32 maxCompLen, attributes;
	
	char buffer[MAX_PATH + 1];
	int32 succeeded = ::GetVolumeInformationA(ToPlatformStr(path).c_str(), nil, 0, nil, &maxCompLen, &attributes, buffer, sizeof(buffer));
	ThrowIf(!succeeded);

	bool is = ConvertToLowerCase(FromPlatformStr(buffer)) == L"ntfs";	// CodeWarrior doesn't have stricmp...
	
	return is;
}


//---------------------------------------------------------------
//
// XFileSystem::IsNetworkVolume							[static]
//
//---------------------------------------------------------------
bool XFileSystem::IsNetworkVolume(const OSVolume& volume)
{
	std::wstring path = volume + L":\\";
			
	UINT type;
	if (WSystemInfo::HasUnicode())
		type = ::GetDriveTypeW(path.c_str());
	else
		type = ::GetDriveTypeA(ToPlatformStr(path).c_str());
	ASSERT(type != 0);							// can't be determined
	ASSERT(type != 1);							// root directory doesn't exist
	
	bool isNetwork = type == DRIVE_REMOTE;
	
	return isNetwork;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XFileSystem::IsValidName								[static]
//
//---------------------------------------------------------------
bool XFileSystem::IsValidName(const std::wstring& name, const OSVolume& volume)
{
	uint32 maxLength = XFileSystem::GetMaxFileSpecLength(volume);
	
	bool valid = name.length() <= maxLength && name.length() > 0;
	
	if (valid) {
		bool hasUnicode = XFileSystem::IsNTFS(volume);

		for (uint32 index = 0; index < name.length() && valid; ++index)
			valid = IsValidChar(name[index], hasUnicode) || name[index] == '.';
			
//		if (valid)
//			valid = Count(name, '.') <= 1;	// can't have more than one period (this is apparently OK for folders, and maybe files?)
	}
	
	return valid;
}


//---------------------------------------------------------------
//
// XFileSystem::GetValidName							[static]
//
//---------------------------------------------------------------
std::wstring XFileSystem::GetValidName(const std::wstring& name, const OSVolume& volume, uint32 maxLen)
{
	PRECONDITION(name.length() >= 5);			// need room for at least a character followed by a three letter extension
	
	uint32 max = XFileSystem::GetMaxFileSpecLength(volume);
	if (maxLen > max)
		maxLen = max;
		
	std::wstring outName = name;
	
	// Replace bad characters with dashes.
	bool hasUnicode = XFileSystem::IsNTFS(volume);

	uint32 index;
	for (index = 0; index < outName.length(); ++index)
		if (!IsValidChar(outName[index], hasUnicode) && outName[index] != ' ' && outName[index] != '.')
			outName[index] = '-';
	
	uint32 numPeriods = Count(outName, '.');
	if (numPeriods > 1) {
		for (index = 1; index < outName.length() && numPeriods > 1; ++index) {
			if (outName[index] == '.') {
				outName[index] = '-';
				--numPeriods;
			}
		}
	}
	
	// Truncate from the middle if the name is too large.
	if (outName.length() > maxLen) {
		uint32 length = outName.length();
		uint32 pos = outName.rfind('.');
		if (pos != std::wstring::npos) 
			length = pos;
			
		uint32 overflow = outName.length() - maxLen + 3;	// add three for the periods
		
		uint32 first = (length - overflow)/2;				// first character we're going to drop
		uint32 last  = first + overflow - 1;				// last character we're going to drop
		
		outName = outName.substr(0, first) + L"..." + outName.substr(last + 1);
		POSTCONDITION(outName.length() <= maxLen);
	}
	
	POSTCONDITION(XFileSystem::IsValidName(outName, volume));
	
	return outName;
}


}	// namespace Whisper
