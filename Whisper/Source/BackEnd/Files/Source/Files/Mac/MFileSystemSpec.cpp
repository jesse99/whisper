/*
 *  File:		MFileSystemSpec.cpp
 *  Summary:	An object that points to a (possibly non-existent) file/folder.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MFileSystemSpec.cpp,v $
 *		Revision 1.5  2001/05/06 10:39:35  jesjones
 *		Throws for broken aliaii.
 *		
 *		Revision 1.4  2001/04/27 04:20:51  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.3  2000/12/10 04:03:50  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:14:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 2/25/00	JDJ		Stream operators write out the platform.
 *		 <3>	 3/05/99	JDJ		GetPath checks for two trailing colons.
 *		 <2>	 2/18/99	JDJ		GetName only calls PBGetCatInfoSync if vRefNum or
 *									parID isn't zero.
 *		 <1>	 8/10/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XFileSystemSpec.h>

#include <Aliases.h>
#include <Errors.h>
#include <Resources.h>
#include <TextUtils.h>

#include <FullPath.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XFileSystem.h>
#include <XFolderSpec.h>
#include <XLocker.h>
#include <XNumbers.h>
#include <XStreaming.h>
#include <XStringUtils.h>
#include <XTextTranscoder.h>		

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// MakeResolvedFileSpec
//
// Makes an FSSpec(), then resolves it to it's end if it's an 
// alias file.
//
//---------------------------------------------------------------
static OSErr MakeResolvedFileSpec(int16 vRefNum, int32 dirID, ConstStr255Param name, FSSpec* spec)
{
	OSErr err = FSMakeFSSpec(vRefNum, dirID, name, spec);
	if (err == noErr) {
		Boolean isFolder, wasAliased;
		err = ResolveAliasFile(spec, true, &isFolder, &wasAliased);
		ThrowIfOSErr(err);				// missing files and folders are OK, but missing aliases are not
	}

	return err;
}

#pragma mark -

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
	if (platform != L"Macintosh")
		throw std::domain_error(ToUTF8Str((L"Can't use a " + platform + L" path name")));

	uint32 size;
	stream >> size;
	
	Handle alias = ::NewHandle(numeric_cast<int32>(size));
	ThrowIfNil(alias);
	
	{
	XLocker lock(alias);
		stream.ReadBytes(*alias, size);
	}
	
	value = XFileSystemSpec((AliasHandle) alias);
	
	::DisposeHandle(alias);
				
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

	std::wstring platform = L"Macintosh";		// don't localize this!
	stream << platform;

	Handle alias;
	OSErr err = ::NewAlias(nil, &value.GetOSSpec(), (AliasHandle*) &alias);
	ThrowIfOSErr(err);
	
	uint32 size = numeric_cast<uint32>(::GetHandleSize(alias));
	stream.WriteBytes(&size, sizeof(size));
	
	{
	XLocker lock(alias);
		stream.WriteBytes(*alias, size);
	}
	
	::DisposeHandle(alias);

	return stream;
}

#pragma mark -

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
	mSpec.vRefNum = 0;			// This is what FSMakeFSSpec does when there's an error...
	mSpec.parID   = 0;
	mSpec.name[0] = '\0';
}


//---------------------------------------------------------------
//
// XFileSystemSpec::XFileSystemSpec (OSFSSpec)
//
//---------------------------------------------------------------
XFileSystemSpec::XFileSystemSpec(const OSFSSpec& spec) : mSpec(spec)
{
	OSErr err = MakeResolvedFileSpec(spec.vRefNum, spec.parID, spec.name, &mSpec);
	if (err != fnfErr && err != dirNFErr && err != noErr)
		ThrowErr(err);
}


//---------------------------------------------------------------
//
// XFileSystemSpec::XFileSystemSpec (XFolderSpec, string)
//
//---------------------------------------------------------------
XFileSystemSpec::XFileSystemSpec(const XFolderSpec& folder, const std::wstring& inFileName)
{	
	std::wstring fileName = inFileName;

	if (!XFileSystem::IsValidName(fileName, folder.GetVolume())) {
		DEBUGSTR(fileName, " isn't a valid file/folder name!");
		
		fileName = XFileSystem::GetValidName(fileName, folder.GetVolume());
	}

	OSErr err = MakeResolvedFileSpec(folder.GetVolume(), folder.GetDirID(), ToPascalStr(fileName).c_str(), &mSpec);
	if (err != fnfErr && err != dirNFErr && err != noErr)
		ThrowErr(err);
}


//---------------------------------------------------------------
//
// XFileSystemSpec::XFileSystemSpec (int16, int32, string)
//
//---------------------------------------------------------------
XFileSystemSpec::XFileSystemSpec(int16 volume, int32 parID, const std::wstring& inFileName)
{
	std::wstring fileName = inFileName;

	if (!XFileSystem::IsValidName(fileName, volume)) {
		DEBUGSTR(fileName, " isn't a valid file/folder name!");
		
		fileName = XFileSystem::GetValidName(fileName, volume);
	}

	OSErr err = MakeResolvedFileSpec(volume, parID, ToPascalStr(fileName).c_str(), &mSpec);
	if (err != noErr && err != fnfErr && err != dirNFErr)
		ThrowErr(err);
}


//---------------------------------------------------------------
//
// XFileSystemSpec::XFileSystemSpec (AliasHandle)
//
//---------------------------------------------------------------
XFileSystemSpec::XFileSystemSpec(AliasHandle alias)
{
	Boolean changed;
	OSErr err = ResolveAlias(nil, alias, &mSpec, &changed);
	ThrowIfOSErr(err);				// missing files and folders are OK, but missing aliases are not
}


//---------------------------------------------------------------
//
// XFileSystemSpec::XFileSystemSpec (ResID)
//
//---------------------------------------------------------------
XFileSystemSpec::XFileSystemSpec(ResID id)
{
	AliasHandle alias = (AliasHandle) ::GetResource('alis', id);
	ThrowIfResFail(alias);
	
	HNoPurge((Handle) alias);
	ThrowIfMemError();

	Boolean changed;
	OSErr err = ResolveAlias(nil, alias, &mSpec, &changed);
	ReleaseResource((Handle) alias);

	ThrowIfOSErr(err);				// missing files and folders are OK, but missing aliases are not
}


//---------------------------------------------------------------
//
// XFileSystemSpec::XFileSystemSpec (wstring)
//
//---------------------------------------------------------------
XFileSystemSpec::XFileSystemSpec(const std::wstring& path)
{
	OSErr err = MakeResolvedFileSpec(0, 0, ToPascalStr(path).c_str(), &mSpec);
	if (err != fnfErr && err != dirNFErr && err != noErr)
		ThrowErr(err);
}


//---------------------------------------------------------------
//
// XFileSystemSpec::operator==
//
//---------------------------------------------------------------
bool XFileSystemSpec::operator==(const XFileSystemSpec& rhs) const
{
	bool equal = mSpec.vRefNum == rhs.mSpec.vRefNum && 
	             mSpec.parID == rhs.mSpec.parID && 
	             EqualString(mSpec.name, rhs.mSpec.name, false, true);	// documented as "using the comparison rules of the Macintosh file system"
	             
	return equal;
}


//---------------------------------------------------------------
//
// XFileSystemSpec::GetParent
//
//---------------------------------------------------------------
XFolderSpec XFileSystemSpec::GetParent() const
{
	return XFolderSpec(mSpec.vRefNum, mSpec.parID);
}


//---------------------------------------------------------------
//
// XFileSystemSpec::GetVolume
//
//---------------------------------------------------------------
OSVolume XFileSystemSpec::GetVolume() const
{
	return mSpec.vRefNum;
}


//---------------------------------------------------------------
//
// XFileSystemSpec::GetName
//
//---------------------------------------------------------------
std::wstring XFileSystemSpec::GetName() const
{
	std::wstring name = FromPascalStr(mSpec.name);
	
	if (name.length() == 0 && (mSpec.parID != 0 || mSpec.vRefNum != 0)) {	// name may be empty if we're a directory
		Str255 pName;
		pName[0] = '\0';								
		
		CInfoPBRec parms;
		parms.hFileInfo.ioCompletion = nil;						
		parms.hFileInfo.ioVRefNum    = mSpec.vRefNum;					
		parms.hFileInfo.ioDirID      = mSpec.parID;					
		parms.hFileInfo.ioNamePtr    = pName;				
		parms.hFileInfo.ioFDirIndex  = -1;				// not indexed
		
		OSErr err = PBGetCatInfoSync(&parms);
		ThrowIfOSErr(err);
		
		name = FromPascalStr(pName);
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
	std::wstring path;

	int16 length;
	Handle hand = nil;
	
	OSErr err = FSpGetFullPath(&mSpec, &length, &hand);
	if (err != noErr && err != fnfErr && err != dirNFErr)
		ThrowErr(err);
		
	ThrowIfNil(hand);
	ASSERT(GetHandleSize(hand) >= length);
		
	{
	XLocker lock(hand);
	XDisableUTF16Warning disabler;
		path = FromPlatformStr(reinterpret_cast<const char*>(*hand), length);
	}
	
	DisposeHandle(hand);
	
	// MoreFiles tends to add an extra colon for folder specs...
	if (path[length-1UL] == ':' && path[length-2UL] == ':')
		path.resize(length-1UL);
	
	return path;
}


}	// namespace Whisper
