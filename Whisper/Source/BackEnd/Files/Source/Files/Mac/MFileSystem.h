/*
 *  File:		MFileSystem.h
 *  Summary:	Some handy functions dealing with the file system.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MFileSystem.h,v $
 *		Revision 1.5  2001/05/06 10:39:03  jesjones
 *		Added MoveSpec. Removed ResolveSpec and ResolveAlias (use MAlias instead).
 *		
 *		Revision 1.4  2001/04/27 04:20:47  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.3  2001/02/13 10:16:56  jesjones
 *		Added ChangeCreatorType.
 *		
 *		Revision 1.2  2000/11/09 09:14:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XFileSpec.h>
#include <XFolderSpec.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	Constants
// ===================================================================================
const uint16 kAliasBit  = 1U << 15;
const uint16 kLockedBit = 1U << 0;
const uint16 kFolderBit = 1U << 4;


// ===================================================================================
//	class MFileSystem
//!		Some handy functions dealing with the file system..
// ===================================================================================
class FILES_EXPORT MFileSystem {

//-----------------------------------
//	API
//
public:
	// ----- Files -----
	static 	bool 		HasDataFork(const XFileSpec& file);
	static 	bool 		HasRsrcFork(const XFileSpec& file);

	static 	uint64 		GetDataForkSize(const XFileSpec& file);
	static 	uint64 		GetRsrcForkSize(const XFileSpec& file);

	static void 		ChangeCreatorType(const XFileSpec& file, OSType creator, OSType fileType);
	
	// ----- Folders -----
	static uint32 		GetCount(const XFolderSpec& folder);
						/**< Returns the number of files and folders within the specified folder. */

	static FSSpec 		GetItem(const XFolderSpec& folder, uint32 index);
						/**< Returns a file or folder inside the specified folder. */

	// ----- Files and/or Folders -----
	static	bool		IsAliasFile(const XFileSystemSpec& spec);
	static	bool		IsLocked(const XFileSystemSpec& spec);		
	static	bool		IsInTrash(const XFileSystemSpec& spec);

	static 	uint32		GetCreationDate(const XFileSystemSpec& spec);	
	static 	uint32		GetModDate(const XFileSystemSpec& spec);
	
	static void 		MoveSpec(const XFileSystemSpec& spec, const XFolderSpec& dst);
						/**< The spec and dst must be on the same volume. */
	
	// ----- Volumes -----
	static	void		FlushVolume(int16 volume);
	
	static	uint32		GetVolumeBlockSize(int16 volume);
	
	static	bool		IsVolumeLocked(int16 volume);
						/**< Returns true if the volume is locked by software or hardware. */
	
	// ----- Finder -----
	static 	void 		SetFileLock(const XFileSpec& file, bool lock = true);
						/**< Set the file's lock bit. */
						
	static	void		SetNameLock(const XFileSystemSpec& spec, bool lock = true);
						/**< Sets the nameLocked bit in the Finder so the user can't rename the 
						file or customize the icon. */

	static	void		ForceFinderUpdate(const XFolderSpec& folder);
						/**< Use this to update the Finder windows. */
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
