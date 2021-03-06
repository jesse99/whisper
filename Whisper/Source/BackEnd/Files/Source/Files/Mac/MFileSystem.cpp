/*
 *  File:		MFileSystem.cpp
 *  Summary:	Some handy functions for dealing with the file system.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MFileSystem.cpp,v $
 *		Revision 1.10  2001/05/06 10:38:13  jesjones
 *		Added MoveSpec. Removed some non-Allegro aliaii work arounds.
 *		
 *		Revision 1.9  2001/04/27 04:20:41  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.8  2001/04/21 03:26:18  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.7  2001/04/14 07:35:29  jesjones
 *		Updated for the new MoreFiles.
 *		
 *		Revision 1.6  2001/04/13 07:50:29  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.5  2001/03/05 05:39:09  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.4  2001/02/13 10:17:08  jesjones
 *		Added ChangeCreatorType.
 *		
 *		Revision 1.3  2000/11/27 03:28:50  jesjones
 *		GetTempFile takes an extension argument. GetTempFile increments the time instead of getting it anew each time through the loop.
 *		
 *		Revision 1.2  2000/11/09 09:13:52  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <11>	 7/31/00	JDJ		Tweaked GetVolumeTotalBytes and GetVolumeFreeBytes for the 3.3
 *                                  Universal Headers.
 *		 <10>	 2/06/00	JDJ		Removed LaunchApp (use the version in MDesktopMgr).
 *		 <9>	12/12/99	JDJ		Fixed a bug that could cause GetValidName to index
 *									past the end of the file name if the number of characters
 *									to drop was odd.
 *		 <8>	 7/19/99	JDJ		Tweaked for Carbon.
 *		 <7>	 6/12/99	JDJ		Added IsInTrash.
 *		 <6>	 5/21/99	JDJ		RenameFolder no longer assigns to folder.
 *		 <5>	 5/07/99	JDJ		Moved IsWritable from MFileSystem to XFileSystem.
 *		 <4>	 3/12/99	JDJ		RenameFile and RenameFolder no longer modify the spec argument.
 *		 <3>	 2/18/99	JDJ		Fixed a bug that could cause GetValidName to index
 *									past the end of the file name if the number of characters
 *									to drop was odd.
 *		 <2>	 1/20/99	JDJ		FileExists, FolderExists, and SpecExists now work 
 *									properly with empty FSSpecs.
 *		 <1>	 1/10/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <MFileSystem.h>
#include <XFileSystem.h>

#include <AppleEvents.h>
#include <Events.h>
#include <Errors.h>
#include <Files.h>
#include <Processes.h>
#include <Script.h>
#include <String.h>
#include <TextUtils.h>

#include <FileCopy.h>
#include <MDesktopMgr.h>
#include <MoreFiles.h>
#include <MoreFilesExtras.h>
#include <MProcess.h>
#include <MResUtils.h>
#include <MSystemInfo.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XFolderSpec.h>
#include <XMemUtils.h>
#include <XNumbers.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	struct SCKID
//		From tech note PT 07.
// ===================================================================================
#pragma options align=mac68k
struct SCKID {
	uint32	checkSum;
	uint32	location;
	int16	version;		// should be 4
	int16	readOnly;		// 0 == modifiable
	uint8	noBranch;		// if modifiable & noBranch != 0 then branch was made on check out 
	uint8	modifyReadOnly;	// if 1 file is modified read-only
	uint32	unused;
	uint32	checkoutTime;	// date and time of checkout
	uint32	modDate;
	uint32	pidA;
	uint32	pidB;
	int16	userID;
	int16	fileID;
	int16	revID;
	Str255	projectPath;	// these are variable length pascal strings
//	Str255	userName;
//	Str255	revisionNum;
//	Str255	fileName;
//	Str255	task;
//	WStr255	comment;		// wide pascal string (ie length is a uint16 instead of a uint8)
};
#pragma options align=reset


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// FollowFinderAlias
//
// FollowFinderAlias resolves an alias taken from a Finder alias 
// file, updating the alias record (but not the alias resource in 
// the file) if necessary. (This is from TN FL 30).
//
//---------------------------------------------------------------
pascal OSErr FollowFinderAlias(const FSSpec *fromFile,
	                           AliasHandle alias,
	                           Boolean logon,
	                           FSSpec *target,
	                           Boolean *wasChanged) 
	                           
TWOWORDINLINE(0x700F, 0xA823);  /* MOVEQ #$0F,D0; _AliasDispatch; */
        

//---------------------------------------------------------------
//
// GetCatInfo
//
//---------------------------------------------------------------
static OSErr GetCatInfo(const FSSpec& inSpec, CInfoPBRec* info)
{
	FSSpec spec = inSpec;
	
	info->hFileInfo.ioCompletion = nil;
	info->hFileInfo.ioNamePtr    = spec.name;
	info->hFileInfo.ioVRefNum    = spec.vRefNum;
	info->hFileInfo.ioFDirIndex  = 0;
	info->hFileInfo.ioDirID      = spec.parID;
	
	OSErr err = PBGetCatInfoSync(info);
	
	info->hFileInfo.ioNamePtr = nil;			// string vanishes with the function
	
	return err;
}


//---------------------------------------------------------------
//
// GetSerialNumber (OSType, OSType)		
//
//---------------------------------------------------------------
static ProcessSerialNumber GetSerialNumber(OSType type, OSType creator)
{
	ProcessSerialNumber psn = {0, kNoProcess};

	bool found = false;	
	OSErr err = noErr;
	
	ProcessInfoRec info;		
	info.processInfoLength = sizeof(ProcessInfoRec);
	info.processName       = nil;
	info.processAppSpec    = nil;
	info.processLocation   = nil;
	
	while (!found && err == noErr) {
		err = GetNextProcess(&psn);
		if (psn.highLongOfPSN == 0 && psn.lowLongOfPSN == kNoProcess)
			err = procNotFound;
			
		if (err == noErr) {
			err = GetProcessInformation(&psn, &info);
			
			found = err == noErr && info.processType == type && info.processSignature == creator;
		}
	}
	
	if (!found)
		ThrowErr(procNotFound);
	
	return psn;	
}


//---------------------------------------------------------------
//
// GetAddressOfFinder
//
//---------------------------------------------------------------
static AEDesc GetAddressOfFinder()
{
	AEDesc desc = {typeNull, nil};
	
	ProcessSerialNumber finderPSN = GetSerialNumber('FNDR', 'MACS');
	
	OSErr err = AECreateDesc(typeProcessSerialNumber, &finderPSN, sizeof(ProcessSerialNumber), &desc);
	ThrowIfOSErr(err);
	
	return desc;
} 

#pragma mark -

// ===================================================================================
//	class MFileSystem
// ===================================================================================

//---------------------------------------------------------------
//
// MFileSystem::HasDataFork								[static]
//
//---------------------------------------------------------------
bool MFileSystem::HasDataFork(const XFileSpec& file)
{
	int16 refNum;
	OSErr err = FSpOpenDF(&(file.GetOSSpec()), fsCurPerm, &refNum);
	if (err == noErr)
		(void) FSClose(refNum);
		
	return err == noErr;
						
}


//---------------------------------------------------------------
//
// MFileSystem::HasRsrcFork								[static]
//
//---------------------------------------------------------------
bool MFileSystem::HasRsrcFork(const XFileSpec& file)
{
	int16 refNum;
	OSErr err = FSpOpenRF(&(file.GetOSSpec()), fsCurPerm, &refNum);
	if (err == noErr)
		(void) FSClose(refNum);
		
	return err == noErr;
}


//---------------------------------------------------------------
//
// MFileSystem::GetDataForkSize							[static]
//
//---------------------------------------------------------------
uint64 MFileSystem::GetDataForkSize(const XFileSpec& file)
{
	int32 dataSize, rsrcSize;
	
	OSErr err = FSpGetFileSize(&(file.GetOSSpec()), &dataSize, &rsrcSize);
	ThrowIfOSErr(err);

	return (uint64) dataSize;
}


//---------------------------------------------------------------
//
// MFileSystem::GetRsrcForkSize							[static]
//
//---------------------------------------------------------------
uint64 MFileSystem::GetRsrcForkSize(const XFileSpec& file)
{
	int32 dataSize, rsrcSize;
	
	OSErr err = FSpGetFileSize(&(file.GetOSSpec()), &dataSize, &rsrcSize);
	ThrowIfOSErr(err);

	return (uint64) rsrcSize;
}


//---------------------------------------------------------------
//
// MFileSystem::ChangeCreatorType						[static]
//
//---------------------------------------------------------------
void MFileSystem::ChangeCreatorType(const XFileSpec& file, OSType creator, OSType fileType)
{
	OSErr err = FSpChangeCreatorType(&file.GetOSSpec(), creator, fileType);
	ThrowIfOSErr(err);
}

#pragma mark ~

//---------------------------------------------------------------
//
// MFileSystem::MoveSpec								[static]
//
//---------------------------------------------------------------
void MFileSystem::MoveSpec(const XFileSystemSpec& srcSpec, const XFolderSpec& dstFolder)
{
	PRECONDITION(srcSpec.GetVolume() == dstFolder.GetVolume());

#if 1
	OSErr err = FSpCatMove(&srcSpec.GetOSSpec(), &dstFolder.GetOSSpec());
	ThrowIfOSErr(err); 

#elif 0
	OSErr err = FSpMoveRename(&srcSpec.GetOSSpec(), &dstFolder.GetOSSpec(), nil);
	ThrowIfOSErr(err); 

#else
	FSSpec src = srcSpec.GetOSSpec();

	FSSpec dst;
	dst.vRefNum = dstFolder.GetVolume();
	dst.parID   = dstFolder.GetDirID();
	BlockMoveData(src.name, dst.name, src.name[0]+1UL);
	
	OSErr err = FSpMoveRename(&src, &dst, nil);
	ThrowIfOSErr(err); 
#endif
}


//---------------------------------------------------------------
//
// MFileSystem::IsAliasFile								[static]
//
//---------------------------------------------------------------
bool MFileSystem::IsAliasFile(const XFileSystemSpec& spec)
{
	Boolean isAliasFile, isFolder;
	OSErr err = ::IsAliasFile(&(spec.GetOSSpec()), &isAliasFile, &isFolder);

	bool isAlias = err == noErr && isAliasFile;
			
	return isAlias;
}


//---------------------------------------------------------------
//
// MFileSystem::IsLocked								[static]
//
//---------------------------------------------------------------
bool MFileSystem::IsLocked(const XFileSystemSpec& spec)
{
	OSErr err = FSpCheckObjectLock(&(spec.GetOSSpec()));
	if (err != noErr && err != fLckdErr)
		ThrowErr(err);
		
	bool locked = err == fLckdErr;
	
	return locked;	
}


//---------------------------------------------------------------
//
// MFileSystem::IsInTrash								[static]
//
//---------------------------------------------------------------
bool MFileSystem::IsInTrash(const XFileSystemSpec& spec)
{
	bool inTrash = false;
	
	FSSpec newSpec = spec.GetOSSpec();
	
	int16 trashVol;
	int32 trashDir;
	OSStatus err = ::FindFolder(kOnSystemDisk, kTrashFolderType, kDontCreateFolder, &trashVol, &trashDir);
	
	// Check the file and all it's parent folders until we find one
	// in the trash or hit a root folder.
	while (err == noErr && newSpec.parID != fsRtParID && !inTrash) {
		inTrash = newSpec.vRefNum == trashVol && newSpec.parID == trashDir;
		
		err = ::FSMakeFSSpec(newSpec.vRefNum, newSpec.parID, "\p", &newSpec);
	}
		
	return inTrash;
}


//---------------------------------------------------------------
//
// MFileSystem::GetCreationDate							[static]
//
//---------------------------------------------------------------
uint32 MFileSystem::GetCreationDate(const XFileSystemSpec& spec)
{
	uint32 date = 0;

	CInfoPBRec info;
	std::memset(&info, 0, sizeof(CInfoPBRec));
	OSErr err = GetCatInfo((spec.GetOSSpec()), &info);
	ThrowIfOSErr(err);
	
	bool isDirectory = (info.hFileInfo.ioFlAttrib & ioDirMask) != 0;
	if (isDirectory)
		date = info.dirInfo.ioDrCrDat;
	else
		date = info.hFileInfo.ioFlCrDat;

	return date;
}


//---------------------------------------------------------------
//
// MFileSystem::GetModDate								[static]
//
//---------------------------------------------------------------
uint32 MFileSystem::GetModDate(const XFileSystemSpec& spec)
{
	uint32 date = 0;

	CInfoPBRec info;
	std::memset(&info, 0, sizeof(CInfoPBRec));
	OSErr err = GetCatInfo((spec.GetOSSpec()), &info);
	ThrowIfOSErr(err);
	
	bool isDirectory = (info.hFileInfo.ioFlAttrib & ioDirMask) != 0;
	if (isDirectory)
		date = info.dirInfo.ioDrMdDat;
	else
		date = info.hFileInfo.ioFlMdDat;

	return date;
}


//---------------------------------------------------------------
//
// MFileSystem::GetCount								[static]
//
//---------------------------------------------------------------
uint32 MFileSystem::GetCount(const XFolderSpec& folder)
{
	Str255 fileName = "\p";
	
	CInfoPBRec block;
	block.dirInfo.ioCompletion = nil;
	block.dirInfo.ioNamePtr    = fileName;
	block.dirInfo.ioVRefNum    = folder.GetVolume();
	block.dirInfo.ioFDirIndex  = -1;
	block.dirInfo.ioDrDirID    = folder.GetDirID();
	
	OSErr err = PBGetCatInfoSync(&block);
	if (err == dirNFErr)
		block.dirInfo.ioDrNmFls = 0;
	else
		ThrowIfOSErr(err);
		
	return block.dirInfo.ioDrNmFls;
}


//---------------------------------------------------------------
//
// MFileSystem::GetItem									[static]
//
//---------------------------------------------------------------
FSSpec MFileSystem::GetItem(const XFolderSpec& folder, uint32 index)
{
	Str255 fileName;
	fileName[0] = 0;
	
	CInfoPBRec block;
	block.hFileInfo.ioCompletion = nil;
	block.hFileInfo.ioNamePtr    = fileName;
	block.hFileInfo.ioVRefNum    = folder.GetVolume();
	block.hFileInfo.ioFDirIndex  = numeric_cast<int16>(index + 1);
	block.hFileInfo.ioDirID      = folder.GetDirID();
	
	OSErr err = PBGetCatInfoSync(&block);
	ThrowIfOSErr(err);

	FSSpec spec;
	err = FSMakeFSSpec(folder.GetVolume(), folder.GetDirID(), fileName, &spec);
	if (err != noErr) 
		ThrowIfOSErr(err);

	return spec;
}

#pragma mark ~

//---------------------------------------------------------------
//
// MFileSystem::FlushVolume								[static]
//
//---------------------------------------------------------------
void MFileSystem::FlushVolume(int16 volume)
{
	OSErr err = FlushVol(nil, volume);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MFileSystem::GetVolumeBlockSize						[static]
//
//---------------------------------------------------------------
uint32 MFileSystem::GetVolumeBlockSize(int16 volume)
{
	HParamBlockRec pb;
	pb.volumeParam.ioCompletion = nil;
	pb.volumeParam.ioNamePtr    = nil;
	pb.volumeParam.ioVRefNum    = volume;
	pb.volumeParam.ioVolIndex   = 0;
	
	OSErr err = PBHGetVInfoSync(&pb);
	ThrowIfOSErr(err);
	
	return (uint32) pb.volumeParam.ioVAlBlkSiz;
}


//---------------------------------------------------------------
//
// MFileSystem::IsVolumeLocked							[static]
//
//---------------------------------------------------------------
bool MFileSystem::IsVolumeLocked(int16 volume)
{
	OSErr err = CheckVolLock(nil, volume);
	if (err != noErr && err != wPrErr && err != vLckdErr)
		ThrowErr(err);
		
	bool locked = err == wPrErr || err == vLckdErr;
	
	return locked;
}

#pragma mark ~

//---------------------------------------------------------------
//
// MFileSystem::SetFileLock								[static]
//
//---------------------------------------------------------------
void MFileSystem::SetFileLock(const XFileSpec& file, bool lock)
{
	PRECONDITION(XFileSystem::FileExists(file));
	
	if (lock)
		FSpSetFLock(&(file.GetOSSpec()));
	else
		FSpRstFLock(&(file.GetOSSpec()));
}


//---------------------------------------------------------------
//
// MFileSystem::SetNameLock								[static]
//
//---------------------------------------------------------------
void MFileSystem::SetNameLock(const XFileSystemSpec& file, bool lock)
{
	OSErr err;
	
	if (lock)
		err = FSpSetNameLocked(&(file.GetOSSpec()));
	else
		err = FSpClearNameLocked(&(file.GetOSSpec()));

	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MFileSystem::ForceFinderUpdate						[static]
//
// Code is adapted from 'Zawphing' in Develop 20.
//
//---------------------------------------------------------------
void MFileSystem::ForceFinderUpdate(const XFolderSpec& folder)
{
	const OSType kAEFinderSuite	= 'fndr';
	const OSType kAEUpdate		= 'fupd';

	AEDesc ae      = {typeNull, nil};
	AEDesc target  = {typeNull, nil};
	AEDesc objSpec = {typeNull, nil};
	
	try {
		// Get the address of the Finder and make an "Update" event
		target = GetAddressOfFinder();
		OSErr err = ::AECreateAppleEvent(kAEFinderSuite, kAEUpdate, &target, 
		                                 kAutoGenerateReturnID, kAnyTransactionID, 
		                                 &ae);
		ThrowIfOSErr(err);
						
		err = ::AEDisposeDesc(&target);
		target.dataHandle = nil;
		ASSERT(err == noErr);
		
		// Create the direct object of the Apple Event.
		err = ::AECreateDesc(typeFSS, &(folder.GetOSSpec()), sizeof(FSSpec), &objSpec);
		ThrowIfOSErr(err);

		err = ::AEPutParamDesc(&ae, keyDirectObject, &objSpec);
		ThrowIfOSErr(err);

		err = ::AEDisposeDesc(&objSpec);
		objSpec.dataHandle = nil;
		ASSERT(err == noErr);
		
		// Send the Apple Event (reply isn't filled in because we used the kAENoReply flag).
		AEDesc reply = {typeNull, nil};
		err = AESend(&ae, &reply, kAENoReply, kAENormalPriority, kAEDefaultTimeout, nil, nil);
		ThrowIfOSErr(err);
		
		ASSERT(reply.dataHandle == nil);
		
	} catch (...) {
		(void) ::AEDisposeDesc(&target);
		(void) ::AEDisposeDesc(&objSpec);
		(void) ::AEDisposeDesc(&ae);
		throw;
	}

	OSErr err = ::AEDisposeDesc(&ae);
	ASSERT(err == noErr);
}

#pragma mark -

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
		int32 dirID;
		
		OSErr err = FSpDirCreate(&(folder.GetOSSpec()), smSystemScript, &dirID);
		ThrowIfOSErr(err);
	}
}

			
//---------------------------------------------------------------
//
// XFileSystem::RenameFolder							[static]
//
//---------------------------------------------------------------
void XFileSystem::RenameFolder(const XFolderSpec& folder, const std::wstring& inNewName)
{
	PRECONDITION(XFileSystem::FolderExists(folder));
	PRECONDITION(inNewName.length() > 0);
	
	std::wstring newName = XFileSystem::GetValidName(inNewName, folder.GetVolume());
	
	const FSSpec spec = folder.GetOSSpec();
	OSErr err = FSpRename(&spec, ToPascalStr(newName).c_str());
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// XFileSystem::DeleteFolder							[static]
//
//---------------------------------------------------------------
void XFileSystem::DeleteFolder(const XFolderSpec& folder)
{
	PRECONDITION(XFileSystem::FolderExists(folder));

	OSErr err = ::DeleteDirectory(folder.GetVolume(), folder.GetDirID(), nil);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// XFileSystem::FolderExists							[static]
//
//---------------------------------------------------------------
bool XFileSystem::FolderExists(const XFileSystemSpec& folder)
{
	bool exists = false;
	
	FSSpec spec = folder.GetOSSpec();
	if (spec.vRefNum != 0 || spec.parID != 0 || spec.name[0] != '\0') { 	// need to do this because PBGetCatInfoSync returns noErr for empty FSSpecs...
		CInfoPBRec	pb;
		pb.dirInfo.ioCompletion	= nil;
		pb.dirInfo.ioNamePtr	= (unsigned char *) spec.name;
		pb.dirInfo.ioVRefNum	= spec.vRefNum;
		pb.dirInfo.ioFDirIndex	= 0;							// get the info for the thing specified by "folderName"
		pb.dirInfo.ioDrDirID	= spec.parID;
		
		OSErr err = PBGetCatInfoSync(&pb);
	
		exists = err == noErr && (pb.dirInfo.ioFlAttrib & kFolderBit) != 0;
	}
		
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
		int32 ticks = (int32) TickCount();

		Str255 str;
		NumToString(ticks, str);
		std::wstring fileName = prefix + L" " + FromPascalStr(str);
		
		tempItem = XFolderSpec(tempFolder, fileName);
	} while (XFileSystem::FolderExists(tempItem));

	return tempItem;
}

#pragma mark ~

//---------------------------------------------------------------
//
// XFileSystem::CreateFile								[static]
//
//---------------------------------------------------------------
void XFileSystem::CreateFile(const XFileSpec& file, OSType creator, OSType fileType)
{
	PRECONDITION(!XFileSystem::FileExists(file));

	OSErr err = FSpCreate(&(file.GetOSSpec()), creator, fileType, smSystemScript);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// XFileSystem::RenameFile								[static]
//
//---------------------------------------------------------------
void XFileSystem::RenameFile(const XFileSpec& file, const std::wstring& inNewName)
{
	PRECONDITION(XFileSystem::FileExists(file));
	PRECONDITION(inNewName.length() > 0);
	
	std::wstring newName = XFileSystem::GetValidName(inNewName, file.GetVolume());

	OSErr err = FSpRename(&(file.GetOSSpec()), ToPascalStr(newName).c_str());
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// XFileSystem::CopyFile								[static]
//
//---------------------------------------------------------------
void XFileSystem::CopyFile(const XFileSpec& srcFile, const XFileSpec& dstFile, bool preflight)
{
	OSErr err = FSpFileCopy(&(srcFile.GetOSSpec()), &(dstFile.GetParent().GetOSSpec()), ToPascalStr(dstFile.GetName()).c_str(), nil, 0, preflight);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// XFileSystem::DeleteFile								[static]
//
//---------------------------------------------------------------
void XFileSystem::DeleteFile(const XFileSpec& file)
{
	PRECONDITION(XFileSystem::FileExists(file));

	OSErr err = FSpDelete(&(file.GetOSSpec()));
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// XFileSystem::FileExists								[static]
//
//---------------------------------------------------------------
bool XFileSystem::FileExists(const XFileSystemSpec& file)
{
	bool exists = false;
	
	FSSpec spec = file.GetOSSpec();
	if (spec.vRefNum != 0 || spec.parID != 0 || spec.name[0] != '\0') { 	// need to do this because PBGetCatInfoSync returns noErr for empty FSSpecs...
		CInfoPBRec	pb;
		pb.dirInfo.ioCompletion	= nil;
		pb.dirInfo.ioNamePtr	= (unsigned char *) spec.name;	
		pb.dirInfo.ioVRefNum	= spec.vRefNum;
		pb.dirInfo.ioFDirIndex	= 0;							// get the info for the thing specified by "folderName"
		pb.dirInfo.ioDrDirID	= spec.parID;
		
		OSErr err = PBGetCatInfoSync(&pb);
	
		exists = err == noErr && (pb.dirInfo.ioFlAttrib & kFolderBit) == 0;
	}
	
	return exists;
}


//---------------------------------------------------------------
//
// XFileSystem::IsWritable								[static]
//
//---------------------------------------------------------------
bool XFileSystem::IsWritable(const XFileSpec& file)
{
	// File is writable if we can open it with write permission.
	int16 refNum;
	OSErr err = FSpOpenDF(&(file.GetOSSpec()), fsWrPerm, &refNum);
	bool writable = err == noErr;
		
	if (err == noErr)			
		(void) FSClose(refNum);
	
	// If the file doesn't exist yet we need to make sure that the
	// volume isn't locked.
	if (err == fnfErr) {
		err = CheckVolLock(nil, file.GetVolume());		// returns wPrErr or vLckdErr if the volume is locked
		writable = err == noErr;
	
	// If the file does exist and it's writable we need to make
	// sure that it's not checked into a version control system.
	} else if (writable) {
		refNum = FSpOpenResFile(&(file.GetOSSpec()), fsRdPerm);
		err = ResError();
		
		if (err == noErr) {		
			Handle hand = Get1IndResource('ckid', 1);	// will be released when resource fork is closed
			if (hand != nil) {
				const SCKID* ckid = reinterpret_cast<const SCKID*>(*hand);
				if (ckid->version == 4)
					writable = ckid->readOnly != 0 || (ckid->readOnly == 0 && ckid->modifyReadOnly);
				else
					writable = false;					// bad version: there's no way of telling what's up
			}

			CloseResFile(refNum);
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
	int32 dataSize, rsrcSize;
	
	OSErr err = FSpGetFileSize(&(file.GetOSSpec()), &dataSize, &rsrcSize);
	ThrowIfOSErr(err);

	return (uint64) dataSize;
}


//---------------------------------------------------------------
//
// XFileSystem::GetTempFile								[static]
//
//---------------------------------------------------------------
XFileSpec XFileSystem::GetTempFile(const std::wstring& prefix, const std::wstring& extension)
{
	XFileSpec tempFile;
	
	XFolderSpec tempFolder = XFolderSpec::GetTempFolder();
	
	int32 ticks = (int32) TickCount();

	do {
		Str255 str;
		NumToString(ticks++, str);
		std::wstring fileName = prefix + FromPascalStr(str) + L"." + extension;
		fileName = XFileSystem::GetValidName(fileName, tempFolder.GetVolume());		// truncate to 31 chars
		
		tempFile = XFileSpec(tempFolder, fileName);
	} while (XFileSystem::SpecExists(tempFile.GetOSSpec()));

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
	
	if (XFileSystem::SpecExists(dstSpec.GetOSSpec())) {
		std::wstring dstName = srcSpec.GetName() + LoadWhisperString(L" copy (duplicate file)");
		dstName = XFileSystem::GetValidName(dstName, dstSpec.GetVolume());	// truncate to 31 chars
		dstSpec = XFileSpec(srcSpec.GetParent(), dstName);		
		
		int16 count = 2;										// Finder names the second copy "Hello copy 2"
		while (XFileSystem::SpecExists(dstSpec.GetOSSpec())) {
			Str255 str;
			NumToString(count++, str);
			
			std::wstring name = dstName + L" " + FromPascalStr(str);
			name = XFileSystem::GetValidName(name, dstSpec.GetVolume());	// truncate to 31 chars
			
			dstSpec = XFileSpec(srcSpec.GetParent(), name);
		}
	}
	
	return dstSpec;
}

#pragma mark ~

//---------------------------------------------------------------
//
// XFileSystem::SpecExists								[static]
//
//---------------------------------------------------------------
bool XFileSystem::SpecExists(const XFileSystemSpec& inSpec)
{
	bool exists = false;
	
	FSSpec spec = inSpec.GetOSSpec();
	if (spec.vRefNum != 0 || spec.parID != 0 || spec.name[0] != '\0') { 	// need to do this because PBGetCatInfoSync returns noErr for empty FSSpecs...
		CInfoPBRec	pb;
		pb.dirInfo.ioCompletion	= nil;
		pb.dirInfo.ioNamePtr	= (unsigned char *) spec.name;	
		pb.dirInfo.ioVRefNum	= spec.vRefNum;
		pb.dirInfo.ioFDirIndex	= 0;							// get the info for the thing specified by "folderName"
		pb.dirInfo.ioDrDirID	= spec.parID;
		
		OSErr err = PBGetCatInfoSync(&pb);
		exists = err == noErr;
	}
	
	return exists;
}


//---------------------------------------------------------------
//
// XFileSystem::IsFile									[static]
//
//---------------------------------------------------------------
bool XFileSystem::IsFile(const XFileSystemSpec& inSpec)
{
	OSFSSpec spec = inSpec.GetOSSpec();
	
	CInfoPBRec	pb;
	pb.dirInfo.ioCompletion	= nil;
	pb.dirInfo.ioNamePtr	= (unsigned char *) spec.name;
	pb.dirInfo.ioVRefNum	= spec.vRefNum;
	pb.dirInfo.ioFDirIndex	= 0;							// get the info for the thing specified by "folderName"
	pb.dirInfo.ioDrDirID	= spec.parID;
	
	OSErr err = PBGetCatInfoSync(&pb);
	ASSERT(err == noErr || err == fnfErr);
	
	bool is = err == noErr && (pb.dirInfo.ioFlAttrib & kFolderBit) == 0;
	
	return is;
}


//---------------------------------------------------------------
//
// XFileSystem::IsFolder								[static]
//
//---------------------------------------------------------------
bool XFileSystem::IsFolder(const XFileSystemSpec& inSpec)
{
	OSFSSpec spec = inSpec.GetOSSpec();
	
	CInfoPBRec	pb;
	pb.dirInfo.ioCompletion	= nil;
	pb.dirInfo.ioNamePtr	= (unsigned char *) spec.name;
	pb.dirInfo.ioVRefNum	= spec.vRefNum;
	pb.dirInfo.ioFDirIndex	= 0;							// get the info for the thing specified by "folderName"
	pb.dirInfo.ioDrDirID	= spec.parID;
	
	OSErr err = PBGetCatInfoSync(&pb);
	ASSERT(err == noErr || err == fnfErr);

	bool is = err == noErr && (pb.dirInfo.ioFlAttrib & kFolderBit) != 0;
	
	return is;
}


//---------------------------------------------------------------
//
// XFileSystem::IsReadOnly								[static]
//
//---------------------------------------------------------------
bool XFileSystem::IsReadOnly(const XFileSystemSpec& inSpec)
{
	OSFSSpec spec = inSpec.GetOSSpec();
	
	OSErr err = FSpCheckObjectLock(&spec);
	if (err != noErr && err != fLckdErr)
		ThrowErr(err);
		
	bool locked = err == fLckdErr;
	
	return locked;	
}

#pragma mark ~

//---------------------------------------------------------------
//
// XFileSystem::GetVolumeTotalBytes						[static]
//
//---------------------------------------------------------------
uint64 XFileSystem::GetVolumeTotalBytes(const OSVolume& volume)
{
	OSErr err;
	uint64 bytes;
	
	if (CheckCFMLink(PBXGetVolInfoSync)) {
		XVolumeParam block;
		Whisper::ClearMemory(&block, sizeof(block));
		
		block.ioVRefNum = volume;

		err = PBXGetVolInfoSync(&block);			// HFS+ savvy call
		ThrowIfOSErr(err);
		
		bytes = block.ioVTotalBytes;

	} else {
		int16 vRefNum;
		uint32 freeBytes, totalBytes;
		
		err = HGetVInfo(volume, nil, &vRefNum, &freeBytes, &totalBytes);
		ThrowIfOSErr(err);
		
		bytes = totalBytes;
	}
		
	return bytes;
}


//---------------------------------------------------------------
//
// XFileSystem::GetVolumeFreeBytes						[static]
//
//---------------------------------------------------------------
uint64 XFileSystem::GetVolumeFreeBytes(const OSVolume& volume)
{
	OSErr err;
	uint64 bytes;
	
	if (CheckCFMLink(PBXGetVolInfoSync)) {
		XVolumeParam block;
		Whisper::ClearMemory(&block, sizeof(block));
		
		block.ioVRefNum = volume;

		err = PBXGetVolInfoSync(&block);			// HFS+ savvy call
		ThrowIfOSErr(err);
		
		bytes = block.ioVFreeBytes;

	} else {
		int16 vRefNum;
		uint32 freeBytes, totalBytes;
		
		err = HGetVInfo(volume, nil, &vRefNum, &freeBytes, &totalBytes);
		ThrowIfOSErr(err);
		
		bytes = freeBytes;
	}
		
	return bytes;
}


//---------------------------------------------------------------
//
// XFileSystem::GetMaxFileSpecLength					[static]
//
//---------------------------------------------------------------
uint32 XFileSystem::GetMaxFileSpecLength(const OSVolume& volume)
{
	UNUSED(volume);
	
	return 32;
}


//---------------------------------------------------------------
//
// XFileSystem::GetVolumeName							[static]
//
//---------------------------------------------------------------
std::wstring XFileSystem::GetVolumeName(const OSVolume& volume)
{
	Str255	vName;

	HParamBlockRec pb;
	pb.volumeParam.ioCompletion = nil;
	pb.volumeParam.ioNamePtr    = vName;
	pb.volumeParam.ioVRefNum    = volume;
	pb.volumeParam.ioVolIndex   = 0;
	
	OSErr err = PBHGetVInfoSync(&pb);
	ThrowIfOSErr(err);
	
	return FromPascalStr(vName);
}


//---------------------------------------------------------------
//
// XFileSystem::IsNetworkVolume							[static]
//
//---------------------------------------------------------------
bool XFileSystem::IsNetworkVolume(const OSVolume& volume)
{
	GetVolParmsInfoBuffer vParmsBuf;
	
	int32 bytes = sizeof(vParmsBuf);
	OSErr err = HGetVolParms(nil, volume, &vParmsBuf, &bytes);
	ThrowIfOSErr(err);
	
	bool networked = isNetworkVolume(&vParmsBuf);
	
	return networked;
}

#pragma mark ~

//---------------------------------------------------------------
//
// XFileSystem::IsValidName								[static]
//
//---------------------------------------------------------------
bool XFileSystem::IsValidName(const std::wstring& name, const OSVolume& volume)
{
	uint32 maxLength = XFileSystem::GetMaxFileSpecLength(volume);
	
	bool valid = name.length() <= maxLength && name.length() > 0;
	
	for (uint32 index = 0; index < name.length() && valid; ++index)
		valid = name[index] != ':';
	
	return valid;
}


//---------------------------------------------------------------
//
// XFileSystem::GetValidName							[static]
//
//---------------------------------------------------------------
std::wstring XFileSystem::GetValidName(const std::wstring& name, const OSVolume& volume, uint32 maxLen)
{
	PRECONDITION(name.length() > 0);	

	uint32 max = XFileSystem::GetMaxFileSpecLength(volume);
	if (maxLen > max)
		maxLen = max;
		
	std::wstring outName = name;
	
	// Replace colons with dashes (like the Finder).
	for (int16 index = 0; index < outName.length(); index++) {
		if (outName[index] == ':')
			outName[index] = '-';
	}
	
	// Truncate from the middle if the name is too large.
	if (outName.length() > maxLen) {
		uint32 length = outName.length();
		uint32 pos = outName.rfind('.');
		if (pos != std::wstring::npos) 
			length = pos;
			
		uint32 overflow = outName.length() - maxLen + 1;	// add one for the ellipsis character
		
		uint32 first = (length - overflow)/2;		// first character we're going to drop
		uint32 last  = first + overflow - 1;		// last character we're going to drop
		
		outName = outName.substr(0, first) + FromPlatformStr("�") + outName.substr(last + 1);
		POSTCONDITION(outName.length() <= maxLen);
	}
	
	return outName;
}


}	// namespace Whisper
