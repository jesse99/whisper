/*
 *  File:		MFileSpec.cpp
 *  Summary:	An object that points to a (possibly non-existent) file.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MFileSpec.cpp,v $
 *		Revision 1.4  2001/04/27 04:16:29  jesjones
 *		Added XPreference specialization.
 *		
 *		Revision 1.3  2001/04/21 03:26:11  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 09:13:45  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <6>	10/26/00	JDJ		Added HasMetaData.
 *		 <5>	10/25/00	JDJ		Made GetMetaData const. 
 *		 <4>	10/17/99	JDJ		ReadMetaData has been renamed GetMetaData and SaveMetaData 
 *									has been renamed SetMetaData.
 *		 <3>	 6/25/99	JDJ		Added kInvalidFileSpec.
 *		 <2>	 4/27/99	JDJ		Added ReadMetaData and SaveMetaData.
 *		 <1>	 1/10/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XFileSpec.h>

#include <MResUtils.h>
#include <XBase64.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XFileSystem.h>
#include <XStringUtils.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const XFileSpec kInvalidFileSpec;


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
XFileSpec::XFileSpec(const OSFSSpec& spec) : XFileSystemSpec(spec)
{
	if (XFileSystem::IsFolder(*this))
		throw std::domain_error(ToUTF8Str((L"'" + this->GetName() + L"' is not a file!")));
}


//---------------------------------------------------------------
//
// XFileSpec::XFileSpec (XFileSystemSpec)
//
//---------------------------------------------------------------
XFileSpec::XFileSpec(const XFileSystemSpec& spec) : XFileSystemSpec(spec)
{
	if (XFileSystem::IsFolder(*this))
		throw std::domain_error(ToUTF8Str((L"'" + this->GetName() + L"' is not a file!")));
}


//---------------------------------------------------------------
//
// XFileSpec::XFileSpec (XFolderSpec, string)
//
//---------------------------------------------------------------
XFileSpec::XFileSpec(const XFolderSpec& folder, const std::wstring& fileName) : XFileSystemSpec(folder, fileName)
{	
	if (XFileSystem::IsFolder(*this))
		throw std::domain_error(ToUTF8Str((L"'" + this->GetName() + L"' is not a file!")));
}


//---------------------------------------------------------------
//
// XFileSpec::XFileSpec (int16, int32, string)
//
//---------------------------------------------------------------
XFileSpec::XFileSpec(int16 volume, int32 parID, const std::wstring& fileName) : XFileSystemSpec(volume, parID, fileName)
{
	if (XFileSystem::IsFolder(*this))
		throw std::domain_error(ToUTF8Str((L"'" + this->GetName() + L"' is not a file!")));
}


//---------------------------------------------------------------
//
// XFileSpec::XFileSpec (AliasHandle)
//
//---------------------------------------------------------------
XFileSpec::XFileSpec(AliasHandle alias) : XFileSystemSpec(alias)
{
	if (XFileSystem::IsFolder(*this))
		throw std::domain_error(ToUTF8Str((L"'" + this->GetName() + L"' is not a file!")));
}


//---------------------------------------------------------------
//
// XFileSpec::XFileSpec (ResID)
//
//---------------------------------------------------------------
XFileSpec::XFileSpec(ResID id) : XFileSystemSpec(id)
{
	if (XFileSystem::IsFolder(*this))
		throw std::domain_error(ToUTF8Str((L"'" + this->GetName() + L"' is not a file!")));
}


//---------------------------------------------------------------
//
// XFileSpec::XFileSpec (wstring)
//
//---------------------------------------------------------------
XFileSpec::XFileSpec(const std::wstring& path) : XFileSystemSpec(path)
{
	if (XFileSystem::IsFolder(*this))
		throw std::domain_error(ToUTF8Str((L"'" + this->GetName() + L"' is not a file!")));
}


//---------------------------------------------------------------
//
// XFileSpec::GetCreator
//
//---------------------------------------------------------------
OSType XFileSpec::GetCreator() const
{
	FInfo info;
	
	OSErr err = HGetFInfo(this->GetVolume(), this->GetParID(), ToPascalStr(this->GetName()).c_str(), &info);
	ThrowIfOSErr(err);
	
	return info.fdCreator;
}


//---------------------------------------------------------------
//
// XFileSpec::GetType
//
//---------------------------------------------------------------
OSType XFileSpec::GetType() const
{
	FInfo info;
	
	OSErr err = HGetFInfo(this->GetVolume(), this->GetParID(), ToPascalStr(this->GetName()).c_str(), &info);
	ThrowIfOSErr(err);
	
	return info.fdType;
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
	
	MOpenResFile resFork(mSpec, fsRdPerm);
	
	if (Whisper::Has1Resource('MetD', name)) 
		Whisper::ReadResource('MetD', name, buffer, bytes);
}


//---------------------------------------------------------------
//
// XFileSpec::GetMetaData (wstring)
//
//---------------------------------------------------------------
XHandle XFileSpec::GetMetaData(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);

	MOpenResFile resFork(mSpec, fsRdPerm);
	
	XHandle data;
	if (Whisper::Has1Resource('MetD', name)) 
		data = Whisper::ReadResource('MetD', name);
	
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

	MOpenResFile resFork(mSpec, fsRdPerm);
	
	bool has = Whisper::Has1Resource('MetD', name);
	
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

	MOpenResFile resFork(mSpec, fsWrPerm);
		
	Whisper::WriteResource('MetD', name, buffer, bytes);
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
		MCoreString appName = ToCarbonStr(XSystemInfo::GetAppName());
		
		CFStringRef appRef  = mType == kSystemPref ? kCFPreferencesAnyApplication : (CFStringRef) appName;
		CFStringRef userRef = mType == kUserPref ? kCFPreferencesCurrentUser : kCFPreferencesAnyUser;
			
		CFTypeRef dataRef = CFPreferencesCopyValue(ToCarbonStr(mName), appRef, userRef, kCFPreferencesAnyHost);
		if (dataRef != nil) {
			MCFObject<CFStringRef> data((CFStringRef) dataRef);
		
			if (CFStringGetTypeID() == CFGetTypeID(*data)) {		// we only write out CFString's so this should match...
				std::wstring str = FromCarbonStr(*data);
				XHandle data = DecodeBase64(ToAsciiStr(str));
				
				AliasHandle aliasH = (AliasHandle) data.GetHandle();
				mData = XFileSpec(aliasH);

			} else
				DEBUGSTR("XPreference data wasn't CFString!");
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
			AliasHandle aliasH;
			OSErr err = ::NewAlias(nil, &mData.GetOSSpec(), &aliasH);
			ThrowIfOSErr(err);
		
			XHandle data((Handle) aliasH);
			XLocker lock(data);
							
			MCoreString appName = ToCarbonStr(XSystemInfo::GetAppName());
			MCoreString str = ToCarbonStr(FromAsciiStr(EncodeBase64(data.GetPtr(), data.GetSize()).c_str()));												
			
			CFStringRef appRef  = mType == kSystemPref ? kCFPreferencesAnyApplication : (CFStringRef) appName;
			CFStringRef userRef = mType == kUserPref ? kCFPreferencesCurrentUser : kCFPreferencesAnyUser;
			
			CFPreferencesSetValue(ToCarbonStr(mName), str, appRef, userRef, kCFPreferencesAnyHost);			
			VERIFY(CFPreferencesAppSynchronize(appRef));
			
			mOldData = mData;
		}
		
	} catch (...) {
		// eat exceptions (see above for rationale)
	}
}


}	// namespace Whisper
