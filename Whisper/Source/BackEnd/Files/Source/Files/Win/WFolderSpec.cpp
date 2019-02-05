/*
 *  File:		WFolderSpec.cpp
 *  Summary:	An object that points to a (possibly non-existent) directory.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WFolderSpec.cpp,v $
 *		Revision 1.6  2001/04/27 09:18:46  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.5  2001/04/27 04:16:42  jesjones
 *		Added XPreference specialization.
 *		
 *		Revision 1.4  2001/03/05 05:40:01  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/11/27 03:29:52  jesjones
 *		GetTempFolder passes the right char count into GetTempFolderW.
 *		
 *		Revision 1.2  2000/11/09 12:07:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 8/01/99	JDJ		When throwing the ctors use the path argument instead of
 *									calling GetName (GetName returns an empty string if the path
 *									points to a file).
 *		 <3>	 6/25/99	JDJ		Added kInvalidFolderSpec.
 *		 <2>	 2/21/99	JDJ		Added GetSystemFolder and GetWindowsFolder (Win).
 *		 <1>	 1/10/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XFolderSpec.h>

#include <WSystemInfo.h>
#include <XConstants.h>
#include <XExceptions.h>
#include <XFileIterator.h>
#include <XFileSystem.h>
#include <XStringUtils.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const XFolderSpec kInvalidFolderSpec;

const OSVolume kOnSystemDisk = L"c";		// this isn't actually used...


// ===================================================================================
//	class XFolderSpec
// ===================================================================================

//---------------------------------------------------------------
//
// XFolderSpec::~XFolderSpec
//
//---------------------------------------------------------------
XFolderSpec::~XFolderSpec()
{
}


//---------------------------------------------------------------
//
// XFolderSpec::XFolderSpec ()
//
//---------------------------------------------------------------
XFolderSpec::XFolderSpec()
{
}


//---------------------------------------------------------------
//
// XFolderSpec::XFolderSpec (OSFSSpec)
//
//---------------------------------------------------------------
XFolderSpec::XFolderSpec(const OSFSSpec& path) : XFileSystemSpec(path)
{	
	if (XFileSystem::IsFile(*this))
		throw std::domain_error(ToUTF8Str((L"'" + path + L"' is not a folder!")));
}


//---------------------------------------------------------------
//
// XFolderSpec::XFolderSpec (XFileSystemSpec)
//
//---------------------------------------------------------------
XFolderSpec::XFolderSpec(const XFileSystemSpec& spec) : XFileSystemSpec(spec)
{	
	if (XFileSystem::IsFile(*this))
		throw std::domain_error(ToUTF8Str((L"'" + spec.GetName() + L"' is not a folder!")));
}


//---------------------------------------------------------------
//
// XFolderSpec::XFolderSpec (XFolderSpec, string)
//
//---------------------------------------------------------------
XFolderSpec::XFolderSpec(const XFolderSpec& parent, const std::wstring& name) : XFileSystemSpec(parent, name)
{
	if (XFileSystem::IsFile(*this))
		throw std::domain_error(ToUTF8Str((L"'" + parent.GetName() + name + L"' is not a folder!")));
}


//---------------------------------------------------------------
//
// XFolderSpec::GetAppFolder							[static]
//
//---------------------------------------------------------------
XFolderSpec XFolderSpec::GetAppFolder()
{	
	std::wstring appPath;

	if (WSystemInfo::HasUnicode()) {
		wchar_t buffer[MAX_PATH];
		uint32 count = GetModuleFileNameW(nil, buffer, sizeof(buffer));
		ThrowIf(count == 0);
	
		appPath.assign(buffer, count);

	} else {
		char buffer[MAX_PATH];
		uint32 count = GetModuleFileNameA(nil, buffer, sizeof(buffer));
		ThrowIf(count == 0);
	
		appPath = FromPlatformStr(buffer, count);
	}
	
	std::wstring appFolder;
	size_t pos = appPath.rfind('\\');
	appFolder = appPath.substr(0, pos);
	
	return appFolder;
}


//---------------------------------------------------------------
//
// XFolderSpec::GetTempFolder							[static]
//
//---------------------------------------------------------------
XFolderSpec	XFolderSpec::GetTempFolder(const OSVolume& volume)
{
	UNUSED(volume);
	
	XFolderSpec spec;

	if (WSystemInfo::HasUnicode()) {
		wchar_t buffer[MAX_PATH];
		
		uint32 count = GetTempPathW(sizeof(buffer)/sizeof(wchar_t), buffer);
		ThrowIf(count == 0);
		
		spec = std::wstring(buffer);
	
	} else {
		char buffer[MAX_PATH];
		
		uint32 count = GetTempPathA(sizeof(buffer), buffer);
		ThrowIf(count == 0);
		
		spec = FromPlatformStr(buffer);
	}
		
	return spec;
}


//---------------------------------------------------------------
//
// XFolderSpec::GetSystemFolder							[static]
//
//---------------------------------------------------------------
XFolderSpec	XFolderSpec::GetSystemFolder()
{
	XFolderSpec spec;

	if (WSystemInfo::HasUnicode()) {
		wchar_t buffer[MAX_PATH];
		
		uint32 count = GetSystemDirectoryW(buffer, sizeof(buffer));
		ThrowIf(count == 0);
		
		spec = std::wstring(buffer);
	
	} else {
		char buffer[MAX_PATH];
		
		uint32 count = GetSystemDirectoryA(buffer, sizeof(buffer));
		ThrowIf(count == 0);
		
		spec = FromPlatformStr(buffer);
	}
		
	return spec;
}


//---------------------------------------------------------------
//
// XFolderSpec::GetWindowsFolder						[static]
//
//---------------------------------------------------------------
XFolderSpec	XFolderSpec::GetWindowsFolder()
{
	XFolderSpec spec;

	if (WSystemInfo::HasUnicode()) {
		wchar_t buffer[MAX_PATH];
		
		uint32 count = GetWindowsDirectoryW(buffer, sizeof(buffer));
		ThrowIf(count == 0);
		
		spec = std::wstring(buffer);
	
	} else {
		char buffer[MAX_PATH];
		
		uint32 count = GetWindowsDirectoryA(buffer, sizeof(buffer));
		ThrowIf(count == 0);
		
		spec = FromPlatformStr(buffer);
	}
		
	return spec;
}


//---------------------------------------------------------------
//
// XFolderSpec::begin (bool)
//
// Note that these have to be outlined to avoid circular references
// in the headers.
//
//---------------------------------------------------------------
XFolderSpec::iterator XFolderSpec::begin(bool recursive) const		
{
	return XFileIterator(*this, nil, nil, recursive);
}


//---------------------------------------------------------------
//
// XFolderSpec::begin (XFileFilter*, XFolderFilter*, bool)
//
//---------------------------------------------------------------
XFolderSpec::iterator XFolderSpec::begin(XFileFilter* takeFileFilter, XFolderFilter* takeFolderFilter, bool recursive) const	
{
	return XFileIterator(*this, takeFileFilter, takeFolderFilter, recursive);
}


//---------------------------------------------------------------
//
// XFolderSpec::end
//
//---------------------------------------------------------------
XFolderSpec::iterator XFolderSpec::end() const										
{
	return XFileIterator(*this);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XPreference<XFolderSpec>
// ===================================================================================

//---------------------------------------------------------------
//
// XPreference<XFolderSpec>::~XPreference
//
//---------------------------------------------------------------
template <> 
XPreference<XFolderSpec>::~XPreference()
{
	this->Flush();
}


//---------------------------------------------------------------
//
// XPreference<XFolderSpec>::XPreference
//
//---------------------------------------------------------------
template <> 
XPreference<XFolderSpec>::XPreference(const std::wstring& name, PrefType type) : mName(name)
{	
	mType = type;
	
	try {
		WRegistryKey key(mType.key, mType.subKey != nil ? mType.subKey : WSystemInfo::GetPrefsName().c_str());
		if (key.HasValue(mName)) {							
			std::wstring str = key.ReadString(mName);	
			mData = XFolderSpec(str);
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
// XPreference<XFolderSpec>::Flush
//
//---------------------------------------------------------------
template <> 
void XPreference<XFolderSpec>::Flush()
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
