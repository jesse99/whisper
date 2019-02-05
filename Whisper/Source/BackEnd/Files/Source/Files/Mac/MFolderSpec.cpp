/*
 *  File:		MFolderSpec.cpp
 *  Summary:	An object that points to a (possibly non-existent) directory.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MFolderSpec.cpp,v $
 *		Revision 1.3  2001/04/27 04:16:33  jesjones
 *		Added XPreference specialization.
 *		
 *		Revision 1.2  2000/11/09 09:14:23  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	10/17/99	JDJ		The ctor that takes a volume and directory id will
 *									now throw if dirNFErr or fnfErr is returned.
 *		 <2>	 6/25/99	JDJ		Added kInvalidFolderSpec.
 *		 <1>	 1/10/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XFolderSpec.h>

#include <MSystemInfo.h>
#include <XBase64.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XFileIterator.h>
#include <XFileSystem.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const XFolderSpec kInvalidFolderSpec;


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
XFolderSpec::XFolderSpec(const OSFSSpec& spec) : XFileSystemSpec(spec)
{
	if (XFileSystem::IsFile(*this))
		throw std::domain_error(ToUTF8Str((L"'" + this->GetName() + L"' is not a folder!")));
}


//---------------------------------------------------------------
//
// XFolderSpec::XFolderSpec (XFileSystemSpec)
//
//---------------------------------------------------------------
XFolderSpec::XFolderSpec(const XFileSystemSpec& spec) : XFileSystemSpec(spec)
{
	if (XFileSystem::IsFile(*this))
		throw std::domain_error(ToUTF8Str((L"'" + this->GetName() + L"' is not a folder!")));
}


//---------------------------------------------------------------
//
// XFolderSpec::XFolderSpec (XFolderSpec, string)
//
//---------------------------------------------------------------
XFolderSpec::XFolderSpec(const XFolderSpec& parent, const std::wstring& name) : XFileSystemSpec(parent, name)
{
	if (XFileSystem::IsFile(*this))
		throw std::domain_error(ToUTF8Str((L"'" + this->GetName() + L"' is not a folder!")));
}


//---------------------------------------------------------------
//
// XFolderSpec::XFolderSpec (int16, int32)
//
//---------------------------------------------------------------
XFolderSpec::XFolderSpec(int16 volume, int32 dir)
{
	OSErr err = FSMakeFSSpec(volume, dir, "\p", &mSpec);
	ThrowIfOSErr(err);

	if (XFileSystem::IsFile(*this))
		throw std::domain_error(ToUTF8Str((L"'" + this->GetName() + L"' is not a folder!")));
}


//---------------------------------------------------------------
//
// XFolderSpec::XFolderSpec (int16, int32, string)
//
//---------------------------------------------------------------
XFolderSpec::XFolderSpec(int16 volume, int32 parDir, const std::wstring& name) : XFileSystemSpec(volume, parDir, name)
{
	if (XFileSystem::IsFile(*this))
		throw std::domain_error(ToUTF8Str((L"'" + this->GetName() + L"' is not a folder!")));
}


//---------------------------------------------------------------
//
// XFolderSpec::XFolderSpec (AliasHandle)
//
//---------------------------------------------------------------
XFolderSpec::XFolderSpec(AliasHandle alias) : XFileSystemSpec(alias)
{
	if (XFileSystem::IsFile(*this))
		throw std::domain_error(ToUTF8Str((L"'" + this->GetName() + L"' is not a folder!")));
}


//---------------------------------------------------------------
//
// XFolderSpec::TFileSpec (ResID)
//
//---------------------------------------------------------------
XFolderSpec::XFolderSpec(ResID id) : XFileSystemSpec(id)
{
	if (XFileSystem::IsFile(*this))
		throw std::domain_error(ToUTF8Str((L"'" + this->GetName() + L"' is not a folder!")));
}


//---------------------------------------------------------------
//
// XFolderSpec::XFolderSpec (wstring)
//
//---------------------------------------------------------------
XFolderSpec::XFolderSpec(const std::wstring& path) : XFileSystemSpec(path)
{
	if (XFileSystem::IsFile(*this))
		throw std::domain_error(ToUTF8Str((L"'" + this->GetName() + L"' is not a folder!")));
}


//---------------------------------------------------------------
//
// XFolderSpec::GetDirID
//
//---------------------------------------------------------------
int32 XFolderSpec::GetDirID() const
{
	CInfoPBRec parms;
	parms.dirInfo.ioCompletion = nil;						
	parms.dirInfo.ioVRefNum    = mSpec.vRefNum;					
	parms.dirInfo.ioDrDirID    = mSpec.parID;					
	parms.dirInfo.ioNamePtr    = (unsigned char *) mSpec.name;				
	parms.dirInfo.ioFDirIndex  = 0;						//	flag to use name
	
	OSErr err = PBGetCatInfoSync(&parms);
	if (err == fnfErr && *this == XFolderSpec::GetAppFolder()) {	// $$$ work around for PC formatted Zip disk...
		err = noErr;
		parms.hFileInfo.ioFlAttrib = 1 << 4;
		parms.dirInfo.ioDrDirID    = MSystemInfo::GetAppSpec().parID;
	}
		
	ThrowIfOSErr(err);

	// If it's not a directory throw bdNamErr.
	if ((parms.hFileInfo.ioFlAttrib & (1 << 4)) == 0)	
		ThrowErr(bdNamErr);

	return parms.dirInfo.ioDrDirID;
}


//---------------------------------------------------------------
//
// XFolderSpec::GetAppFolder
//
//---------------------------------------------------------------
XFolderSpec XFolderSpec::GetAppFolder()
{
	XFolderSpec spec(MSystemInfo::GetAppSpec().vRefNum, MSystemInfo::GetAppSpec().parID);
	
	return spec;
}


//---------------------------------------------------------------
//
// XFolderSpec::GetFolder
//
//---------------------------------------------------------------
XFolderSpec	XFolderSpec::GetFolder(OSType folder, int16 vRefNum, bool createFolder)
{
	int16 volume;			
	int32 dir;
	
	OSErr err = FindFolder(vRefNum, folder, createFolder, &volume, &dir);
	ThrowIfOSErr(err);
	
	XFolderSpec spec(volume, dir);
	
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
				mData = XFolderSpec(aliasH);

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
// XPreference<XFolderSpec>::Flush
//
//---------------------------------------------------------------
template <> 
void XPreference<XFolderSpec>::Flush()
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
