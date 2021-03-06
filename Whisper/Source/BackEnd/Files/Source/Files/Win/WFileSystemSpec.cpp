/*
 *  File:		WFileSystemSpec.cpp
 *  Summary:	An object that points to a (possibly non-existent) file/folder.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WFileSystemSpec.cpp,v $
 *		Revision 1.5  2001/04/27 09:18:41  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.4  2001/04/21 03:31:54  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/03/17 12:27:50  jesjones
 *		(XFolderSpec, wstring) ctor now works if the string points to a shortcut.
 *		
 *		Revision 1.2  2000/11/09 12:07:46  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 7/21/00	JDJ		If the path passed into ResolveShortcut doesn't point to a file
 *									or a shortcut ResolveShortcut will check to see if there's a 
 *									shortcut file with that name in the directory.
 *		 <4>	 2/25/00	JDJ		Stream operators write out the platform.
 *		 <3>	 7/26/99	JDJ		Rewrote GetVolume so that it should work with networked volumes.
 *		 <2>	 6/10/99	JDJ		ResolveShortcut uses GetProcAddress to call SHGetFileInfoW.
 *		 <1>	 8/10/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XFileSystemSpec.h>

#include <shellapi.h>
#include <shlobj.h>

#include <WSystemInfo.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XFileSystem.h>
#include <XFolderSpec.h>
#include <XStreaming.h>
#include <XStringUtils.h>
#include <WInterfacePtr.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// ResolveShortcut
//
//---------------------------------------------------------------
static std::wstring ResolveShortcut(const std::wstring& inPath)
{
	std::wstring path = inPath;
	std::wstring target = path;

	typedef DWORD (WINAPI *SHGetFileInfoWProc)(LPCWSTR, DWORD, SHFILEINFOW FAR*, UINT, UINT);

    SHGetFileInfoWProc getInfoWProc = nil; 
    
    HINSTANCE shellH = GetModuleHandle(_TEXT("SHELL32.DLL")); 	// Win 95 only has SHGetFileInfoW if the active desktop update is installed...
	if (shellH != nil) 
        getInfoWProc = (SHGetFileInfoWProc) GetProcAddress(shellH, _TEXT("SHGetFileInfoW")); 

	if (WSystemInfo::HasUnicode() && getInfoWProc != nil) {
		SHFILEINFOW info;	
		uint32 suceeded = getInfoWProc(path.c_str(), 0, &info, sizeof(info), SHGFI_ATTRIBUTES);	// does the path point to a shortcut?
		if (!suceeded) {
			path += L".lnk";																	// if not, is there a shortcut with that name in the directory?
			suceeded = getInfoWProc(path.c_str(), 0, &info, sizeof(info), SHGFI_ATTRIBUTES);
		}
		
		if (suceeded) {	// don't throw since file need not exist
			if (info.dwAttributes & SFGAO_LINK) {
				WInterfacePtr<IShellLinkW, &IID_IShellLinkW> sl(CLSID_ShellLink);
				WInterfacePtr<IPersistFile, &IID_IPersistFile> pf(sl.GetIUnknown());
	
				HRESULT result = pf->Load(path.c_str(), STGM_READ);
				ThrowIfFailed(result);
	
				result = sl->Resolve(nil, SLR_NO_UI);	// $$$ allow UI? will we need a parent window?
				ThrowIfFailed(result);
	
				wchar_t buffer[MAX_PATH]; 
				result = sl->GetPath(buffer, sizeof(buffer), nil, 0);	// $$$ MFC uses 0 for flags although GetPath isn't documented as accepting zero...
				ThrowIfFailed(result);
	
				target = buffer;
			}
		}

	} else {
		SHFILEINFOA info2;		
		uint32 suceeded2 = SHGetFileInfoA(ToPlatformStr(path).c_str(), 0, &info2, sizeof(info2), SHGFI_ATTRIBUTES);
		if (!suceeded2) {
			path += L".lnk";
			suceeded2 = SHGetFileInfoA(ToPlatformStr(path).c_str(), 0, &info2, sizeof(info2), SHGFI_ATTRIBUTES);
		}

		if (suceeded2) {	// don't throw since file need not exist
			if (info2.dwAttributes & SFGAO_LINK) {
				WInterfacePtr<IShellLinkA, &IID_IShellLinkA> sl(CLSID_ShellLink);
				WInterfacePtr<IPersistFile, &IID_IPersistFile> pf(sl.GetIUnknown());
	
				HRESULT result = pf->Load(path.c_str(), STGM_READ);
				ThrowIfFailed(result);
	
				result = sl->Resolve(nil, SLR_NO_UI);	
				ThrowIfFailed(result);
	
				char buffer[MAX_PATH]; 
				result = sl->GetPath(buffer, sizeof(buffer), nil, 0);	
				ThrowIfFailed(result);
	
				target = FromPlatformStr(buffer);
			}
		}
	}

	return target;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// operator>> (XFileSystemSpec)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, XFileSystemSpec& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'FlSp');
		
	std::wstring platform;
	stream >> platform;
	if (platform != L"Windows")
		throw std::domain_error(ToUTF8Str((L"Can't use a " + platform + L" path name")));

	std::wstring path;
	stream >> path;
	
	value = path;
			
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (XFileSystemSpec)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const XFileSystemSpec& value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('FlSp');
#endif

	std::wstring platform = L"Windows";		// don't localize this!
	stream << platform;

	std::wstring path = value.GetPath();
	stream << path;						// write out a path instead of a shortcut since the path can be easily edited using RegEdit

	return stream;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XFileSystemSpec
// ===================================================================================

//---------------------------------------------------------------
//
// XFileSystemSpec::~XFileSystemSpec
//
//---------------------------------------------------------------
XFileSystemSpec::~XFileSystemSpec()
{
}


//---------------------------------------------------------------
//
// XFileSystemSpec::XFileSystemSpec ()
//
//---------------------------------------------------------------
XFileSystemSpec::XFileSystemSpec()
{
}


//---------------------------------------------------------------
//
// XFileSystemSpec::XFileSystemSpec (OSFSSpec)
//
//---------------------------------------------------------------
XFileSystemSpec::XFileSystemSpec(const OSFSSpec& path) : mSpec(ResolveShortcut(path))
{	
	if (mSpec.length() > MAX_PATH)
		throw std::domain_error(ToUTF8Str((L"Path was too long (for '" + this->GetName() + L"')")));
}


//---------------------------------------------------------------
//
// XFileSystemSpec::XFileSystemSpec (XFolderSpec, string)
//
//---------------------------------------------------------------
XFileSystemSpec::XFileSystemSpec(const XFolderSpec& folder, const std::wstring& inFileName)
{	
	PRECONDITION(inFileName.find(L':') == std::wstring::npos);

	std::wstring fileName = ResolveShortcut(inFileName);
	
	// If we resolved a shortcut we'll get back a path
	if (fileName.find(L':') != std::wstring::npos) {
		mSpec = fileName;
	
	// Otherwise we need to use folder and the file name.
	} else {
		if (!XFileSystem::IsValidName(fileName, folder.GetVolume())) {
			DEBUGSTR(fileName, " isn't a valid file name!");
			
			fileName = XFileSystem::GetValidName(fileName, folder.GetVolume());
		}
		
		std::wstring parent = folder.GetPath();

		uint32 length = parent.length();
		if (length == 0)									
			throw std::range_error("Internal Error: Bad (empty) path.");
		else if (parent[length - 1] == '\\')							// may happen if parent is a root directory
			mSpec = parent + fileName;
		else
			mSpec = parent + L"\\" + fileName;
	}
			
	if (mSpec.length() > MAX_PATH)
		throw std::domain_error(ToUTF8Str((L"Path was too long (for '" + this->GetName() + L"')")));
}


//---------------------------------------------------------------
//
// XFileSystemSpec::operator==
//
//---------------------------------------------------------------
bool XFileSystemSpec::operator==(const XFileSystemSpec& rhs) const
{
	bool equal = this->GetPath() == rhs.GetPath();
	             
	return equal;
}


//---------------------------------------------------------------
//
// XFileSystemSpec::GetParent
//
//---------------------------------------------------------------
XFolderSpec XFileSystemSpec::GetParent() const
{
	std::wstring parent;
	
	size_t pos = mSpec.rfind('\\');
	if (pos != std::wstring::npos) 
		if (pos == 0)
			throw std::range_error(ToUTF8Str((L"Internal Error: Couldn't get the parent folder of " + mSpec + L".")));	
		else 
			parent = mSpec.substr(0, pos);
			
	else
		throw std::range_error(ToUTF8Str((L"Internal Error: Couldn't get the parent folder of " + mSpec + L".")));
				
	return parent;
}


//---------------------------------------------------------------
//
// XFileSystemSpec::GetVolume
//
//---------------------------------------------------------------
OSVolume XFileSystemSpec::GetVolume() const
{	
	uint32 pos = mSpec.find(':');
	if (pos == std::wstring::npos)
		throw std::range_error(ToUTF8Str((L"Internal Error: Couldn't get the drive letter of " + mSpec + L".")));

	std::wstring volume = mSpec.substr(0, pos);		// not sure what the restrictions for network volume names are, so for bad names we'll wait until the volume is used before throwing
				
	return volume;
}


//---------------------------------------------------------------
//
// XFileSystemSpec::GetName
//
// Windows will sometimes hand you a short path (eg when the user
// drops files on the application icon the app gets short paths). 
// This doesn't cause any problems with the OS but could cause
// problems if you want to get the name of the spec or parent
// folder. (There is a GetLongPathName call, but it's only present
// in Win 98 and NT 5.0).
//
//---------------------------------------------------------------
std::wstring XFileSystemSpec::GetName() const
{
	std::wstring name = mLongName;

	if (name.length() == 0) {
		HANDLE findH = nil;		
		
		if (WSystemInfo::HasUnicode()) {
			WIN32_FIND_DATAW info;
			findH = ::FindFirstFileW(mSpec.c_str(), &info);				
		
			if (findH != nil && findH != INVALID_HANDLE_VALUE) {
				mLongName = info.cFileName;
				name = mLongName;

				VERIFY(::FindClose(findH));
			}

		} else {
			WIN32_FIND_DATAA info;
			findH = ::FindFirstFileA(ToPlatformStr(mSpec).c_str(), &info);				
		
			if (findH != nil && findH != INVALID_HANDLE_VALUE) {
				mLongName = FromPlatformStr(info.cFileName);
				name = mLongName;

				VERIFY(::FindClose(findH));
			}
		}

		if (findH == nil || findH == INVALID_HANDLE_VALUE) {
			uint32 pos = mSpec.rfind('\\');		// will get here if the file doesn't exist yet
			name = mSpec.substr(pos + 1);
		}
	}

	return name;
}


//---------------------------------------------------------------
//
// XFileSystemSpec::GetPrefix
//
//---------------------------------------------------------------
std::wstring XFileSystemSpec::GetPrefix() const
{
	std::wstring prefix;

	std::wstring name = this->GetName();
	
	uint32 pos = name.rfind('.');
	if (pos != std::wstring::npos) 
		prefix = name.substr(0, pos);
		
	return prefix;
}


//---------------------------------------------------------------
//
// XFileSystemSpec::GetExtension
//
//---------------------------------------------------------------
std::wstring XFileSystemSpec::GetExtension() const
{
	std::wstring extension;

	std::wstring name = this->GetName();
	
	uint32 pos = name.rfind('.');
	if (pos != std::wstring::npos) 
		extension = ConvertToLowerCase(name.substr(pos + 1));
		
	return extension;
}


//---------------------------------------------------------------
//
// XFileSystemSpec::GetPath
//
//---------------------------------------------------------------
std::wstring XFileSystemSpec::GetPath() const
{
	return mSpec;
}


}	// namespace Whisper
