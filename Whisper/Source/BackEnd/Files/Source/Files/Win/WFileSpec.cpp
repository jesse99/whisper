/*
 *  File:		WFileSpec.cpp
 *  Summary:	An object that points to a (possibly non-existent) file.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WFileSpec.cpp,v $
 *		Revision 1.7  2001/04/27 09:18:30  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.6  2001/04/27 04:16:37  jesjones
 *		Added XPreference specialization.
 *		
 *		Revision 1.5  2001/04/21 03:31:42  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/03/05 05:39:48  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/11/25 01:48:37  jesjones
 *		HasMetaData and SetMetaData no longer throw on FAT file systems.
 *		
 *		Revision 1.2  2000/11/09 12:07:23  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <6>	10/26/00	JDJ		Added HasMetaData.
 *		 <5>	10/17/99	JDJ		ReadMetaData has been renamed GetMetaData and SaveMetaData 
 *									has been renamed SetMetaData.
 *		 <4>	 8/01/99	JDJ		When throwing the ctors use the path argument instead of
 *									calling GetName (GetName returns an empty string if the path
 *									points to a folder).
 *		 <3>	 6/25/99	JDJ		Added kInvalidFileSpec.
 *		 <2>	 4/27/99	JDJ		Added ReadMetaData and SaveMetaData.
 *		 <1>	 1/10/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XFileSpec.h>

#include <WRegistryKey.h>
#include <WSystemInfo.h>
#include <XExceptions.h>
#include <XFileSystem.h>
#include <XFolderSpec.h>
#include <XStringUtils.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const XFileSpec kInvalidFileSpec;


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// IsNTFS
//
//---------------------------------------------------------------
static bool IsNTFS(const OSVolume& volume)
{
	bool is = false;
	
	std::wstring name;
	std::wstring path = volume + L":\\";
		
	uint32 maxCompLen, attributes;
	
	int32 succeeded;
	if (WSystemInfo::HasUnicode()) {
		wchar_t buffer[MAX_PATH + 1];
		succeeded = ::GetVolumeInformationW(path.c_str(), nil, 0, nil, &maxCompLen, &attributes, buffer, sizeof(buffer));
	
		is = succeeded && ConvertToLowerCase(buffer) == L"ntfs";

	} else {
		char buffer[MAX_PATH + 1];
		succeeded = ::GetVolumeInformationA(ToPlatformStr(path).c_str(), nil, 0, nil, &maxCompLen, &attributes, buffer, sizeof(buffer));
	
		is = succeeded && ConvertToLowerCase(FromPlatformStr(buffer)) == L"ntfs";
	}
			
	return is;
}		 

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XFileSpec
// ===================================================================================

//---------------------------------------------------------------
//
// XFileSpec::~XFileSpec 
//
//---------------------------------------------------------------
XFileSpec::~XFileSpec()
{
}


//---------------------------------------------------------------
//
// XFileSpec::XFileSpec ()
//
//---------------------------------------------------------------
XFileSpec::XFileSpec()
{
}


//---------------------------------------------------------------
//
// XFileSpec::XFileSpec (OSFSSpec)
//
//---------------------------------------------------------------
XFileSpec::XFileSpec(const OSFSSpec& path) : XFileSystemSpec(path)
{	
	if (XFileSystem::IsFolder(*this))
		throw std::domain_error(ToUTF8Str((L"'" + path + L"' is not a file!")));
}


//---------------------------------------------------------------
//
// XFileSpec::XFileSpec (XFileSystemSpec)
//
//---------------------------------------------------------------
XFileSpec::XFileSpec(const XFileSystemSpec& spec) : XFileSystemSpec(spec)
{	
	if (XFileSystem::IsFolder(*this))
		throw std::domain_error(ToUTF8Str((L"'" + spec.GetName() + L"' is not a file!")));
}


//---------------------------------------------------------------
//
// XFileSpec::XFileSpec (XFolderSpec, string)
//
//---------------------------------------------------------------
XFileSpec::XFileSpec(const XFolderSpec& folder, const std::wstring& fileName) : XFileSystemSpec(folder, fileName)
{	
	if (XFileSystem::IsFolder(*this))
		throw std::domain_error(ToUTF8Str((L"'" + folder.GetName() + fileName + L"' is not a file!")));
}


//---------------------------------------------------------------
//
// XFileSpec::GetMetaData (wstring, void*, uint32)
//
//---------------------------------------------------------------
void XFileSpec::GetMetaData(const std::wstring& name, void* buffer, uint32 bytes) const
{
	PRECONDITION(name.length() > 0);
	PRECONDITION(buffer != nil);
	
	HANDLE handle = nil;	
	
	try {
		// Open the stream
		int32 succeeded;
		if (WSystemInfo::HasUnicode())
			succeeded = ::SetCurrentDirectoryW(this->GetParent().GetPath().c_str());
		else
			succeeded = ::SetCurrentDirectoryA(ToPlatformStr(this->GetParent().GetPath()).c_str());
		ThrowIf(!succeeded);
		
		std::wstring stream = this->GetName() + L":" + name;
		if (WSystemInfo::HasUnicode())
			handle = ::CreateFileW(stream.c_str(), GENERIC_READ, 0, nil, OPEN_EXISTING, OPEN_ALWAYS, nil);
		else
			handle = ::CreateFileA(ToPlatformStr(stream).c_str(), GENERIC_READ, 0, nil, OPEN_EXISTING, OPEN_ALWAYS, nil);
		ThrowIfBadHandle(handle);
		
		// Read in the data
		if (bytes > 0) {
			uint32 outBytes;
			int32 succeeded = ::ReadFile(handle, buffer, bytes, &outBytes, nil);
			ThrowIf(!succeeded);
		}
		
		succeeded = ::CloseHandle(handle);
		handle = nil;
		ThrowIf(!succeeded);			
		
	} catch (...) {
		if (handle != nil && handle != INVALID_HANDLE_VALUE) 
			(void) ::CloseHandle(handle);
		throw;
	}
}


//---------------------------------------------------------------
//
// XFileSpec::GetMetaData (wstring)
//
//---------------------------------------------------------------
XHandle XFileSpec::GetMetaData(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);

	XHandle data;
	HANDLE handle = nil;	
	
	try {
		// Open the stream
		int32 succeeded;
		if (WSystemInfo::HasUnicode())
			succeeded = ::SetCurrentDirectoryW(this->GetParent().GetPath().c_str());
		else
			succeeded = ::SetCurrentDirectoryA(ToPlatformStr(this->GetParent().GetPath()).c_str());
		ThrowIf(!succeeded);
		
		std::wstring stream = this->GetName() + L":" + name;
		if (WSystemInfo::HasUnicode())
			handle = ::CreateFileW(stream.c_str(), GENERIC_READ, 0, nil, OPEN_EXISTING, OPEN_ALWAYS, nil);
		else
			handle = ::CreateFileA(ToPlatformStr(stream).c_str(), GENERIC_READ, 0, nil, OPEN_EXISTING, OPEN_ALWAYS, nil);
		ThrowIfBadHandle(handle);
		
		// Find the size of the stream
		::SetLastError(0);				// Most Windows functions only set this when they fail. We set it here because of the screwy way GetFileSize reports errors.
		
		uint32 loBytes, hiBytes;	
		loBytes = ::GetFileSize(handle, &hiBytes);
		ThrowIfErr();
				
		if (hiBytes != 0) 
			throw std::range_error(ToUTF8Str(LoadWhisperString(L"File #1's length is over 32 bits.", this->GetName())));				
		
		data.SetSize(loBytes);
		
		// Read in the data
		if (loBytes > 0) {
			uint32 outBytes;
			int32 succeeded = ::ReadFile(handle, data.GetUnsafePtr(), loBytes, &outBytes, nil);
			ThrowIf(!succeeded);
		}
					
		succeeded = ::CloseHandle(handle);
		handle = nil;
		ThrowIf(!succeeded);			

	} catch (...) {
		if (handle != nil && handle != INVALID_HANDLE_VALUE) 
			(void) ::CloseHandle(handle);
		throw;
	}
			
	return data;
}


//---------------------------------------------------------------
//
// XFileSpec::HasMetaData
//
//---------------------------------------------------------------
bool XFileSpec::HasMetaData(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);

	bool has = false;
	
	if (!XFileSystem::IsFAT(this->GetVolume())) {
		HANDLE handle = nil;	

		try {
			int32 succeeded;
			if (WSystemInfo::HasUnicode())
				succeeded = ::SetCurrentDirectoryW(this->GetParent().GetPath().c_str());
			else
				succeeded = ::SetCurrentDirectoryA(ToPlatformStr(this->GetParent().GetPath()).c_str());
			ThrowIf(!succeeded);
			
			std::wstring stream = this->GetName() + L":" + name;	// $$$ can we make this simpler?
			if (WSystemInfo::HasUnicode())
				handle = ::CreateFileW(stream.c_str(), GENERIC_READ, 0, nil, OPEN_EXISTING, OPEN_ALWAYS, nil);
			else
				handle = ::CreateFileA(ToPlatformStr(stream).c_str(), GENERIC_READ, 0, nil, OPEN_EXISTING, OPEN_ALWAYS, nil);
			ThrowIfBadHandle(handle);
			
			has = true;
								
			succeeded = ::CloseHandle(handle);
			handle = nil;
			ThrowIf(!succeeded);			

		} catch (...) {
			if (handle != nil && handle != INVALID_HANDLE_VALUE) 
				(void) ::CloseHandle(handle);
			throw;
		}
	}
				
	return has;
}


//---------------------------------------------------------------
//
// XFileSpec::SetMetaData
//
//---------------------------------------------------------------
void XFileSpec::SetMetaData(const std::wstring& name, const void* buffer, uint32 bytes)
{
	PRECONDITION(name.length() > 0);
	PRECONDITION(buffer != nil);
	
	if (!XFileSystem::IsFAT(this->GetVolume())) {
		HANDLE handle = nil;	
		
		try {
			
			// Open the stream
			int32 succeeded;
			if (WSystemInfo::HasUnicode())
				succeeded = ::SetCurrentDirectoryW(this->GetParent().GetPath().c_str());
			else
				succeeded = ::SetCurrentDirectoryA(ToPlatformStr(this->GetParent().GetPath()).c_str());
			ThrowIf(!succeeded);
			
			std::wstring stream = this->GetName() + L":" + name;
			if (WSystemInfo::HasUnicode())
				handle = ::CreateFileW(stream.c_str(), GENERIC_WRITE, 0, nil, OPEN_EXISTING, OPEN_ALWAYS, nil);
			else
				handle = ::CreateFileA(ToPlatformStr(stream).c_str(), GENERIC_WRITE, 0, nil, OPEN_EXISTING, OPEN_ALWAYS, nil);
			ThrowIfBadHandle(handle);
			
			// Write out the data
			if (bytes > 0) {
				uint32 outBytes;
				int32 succeeded = ::WriteFile(handle, buffer, bytes, &outBytes, nil);
				ThrowIf(!succeeded);
			}
			
			// Close the stream
			succeeded = ::CloseHandle(handle);
			handle = nil;
			ThrowIf(!succeeded);			
			
		} catch (...) {
			if (handle != nil && handle != INVALID_HANDLE_VALUE) 
				(void) ::CloseHandle(handle);
			throw;
		}
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XPreference<XFileSpec>
// ===================================================================================

//---------------------------------------------------------------
//
// XPreference<XFileSpec>::~XPreference
//
//---------------------------------------------------------------
template <> 
XPreference<XFileSpec>::~XPreference()
{
	this->Flush();
}


//---------------------------------------------------------------
//
// XPreference<XFileSpec>::XPreference
//
//---------------------------------------------------------------
template <> 
XPreference<XFileSpec>::XPreference(const std::wstring& name, PrefType type) : mName(name)
{	
	mType = type;
	
	try {
		WRegistryKey key(mType.key, mType.subKey != nil ? mType.subKey : WSystemInfo::GetPrefsName().c_str());
		if (key.HasValue(mName)) {							
			std::wstring str = key.ReadString(mName);	
			mData = XFileSpec(str);
		}														
		
		mOldData = mData;
			
	} catch (...) {
		// eat exceptions (we do this because preference data is non-essential and
		// because it's quite likely that if one preference fails to save they all
		// will and the user would be very annoyed by five dialogs in a row telling
		// him some bit of preference data couldn't be saved).
	}
}


//---------------------------------------------------------------
//
// XPreference<XFileSpec>::Flush
//
//---------------------------------------------------------------
template <> 
void XPreference<XFileSpec>::Flush()
{	
	try {
		if (mData != mOldData) {							
			WRegistryKey key(mType.key, mType.subKey != nil ? mType.subKey : WSystemInfo::GetPrefsName().c_str());
										
			std::wstring str = mData.GetPath();												
			key.WriteString(mName, str);					
			
			mOldData = mData;
		}
		
	} catch (...) {
		// eat exceptions (see above for rationale)
	}
}


}	// namespace Whisper
