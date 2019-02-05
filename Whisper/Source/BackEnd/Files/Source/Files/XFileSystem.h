/*
 *  File:		XFileSystem.h
 *  Summary:	Some handy functions for dealing with the file system.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFileSystem.h,v $
 *		Revision 1.4  2000/12/10 08:52:26  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.3  2000/11/27 03:27:34  jesjones
 *		GetTempFile takes an extension argument.
 *		
 *		Revision 1.2  2000/11/09 12:35:52  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 6/21/99	JDJ		Added IsFAT and IsNTFS.
 *		 <3>	 5/07/99	JDJ		Moved IsWritable from MFileSystem to XFileSystem.
 *		 <2>	 3/12/99	JDJ		RenameFile and RenameFolder no longer modify the spec argument.
 *		 <1>	 1/10/98	JDJ		Created (from Raven)
 */

#pragma once

#include <XFileSpec.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFolderSpec;


// ===================================================================================
//	class XFileSystem
//!		Some handy functions for dealing with the file system.
// ===================================================================================
class FILES_EXPORT XFileSystem {

//-----------------------------------
//	API
//
public:
	//! @name Folders
	//@{
	static 	void 		CreateFolder(const XFolderSpec& folder);
						
	static 	void 		RenameFolder(const XFolderSpec& folder, const std::wstring& newName);

	static 	void 		DeleteFolder(const XFolderSpec& folder);
						/**< Recursively deletes all files and folders within the folder
						before deleting the folder. */

	static 	bool 		FolderExists(const XFileSystemSpec& folder);

	static 	XFolderSpec	GetTempFolder(const std::wstring& prefix);
						/**< Returns a spec to an unused folder name in the Temporary items folder. */
	//@}
						
	//! @name Files
	//@{
	static 	void 		CreateFile(const XFileSpec& file, OSType creator, OSType fileType);
						/**< creator and fileType are ignored on Windows. */

	static 	void 		RenameFile(const XFileSpec& file, const std::wstring& newName);

	static 	void 		CopyFile(const XFileSpec& srcFile, const XFileSpec& dstFile, bool preflight = true);
						/**< Copies srcFile to dstFile. An exception will be thrown if dstFile already exists.
						If preflight is true the destination volume is checked to ensure that it has
						enough allocation blocks before the copying starts. */

	static 	void 		DeleteFile(const XFileSpec& file);

	static 	bool 		FileExists(const XFileSystemSpec& file);

	static 	bool 		IsWritable(const XFileSpec& file);

	static 	uint64 		GetFileSize(const XFileSpec& file);
						/**< Returns the length of the file (doesn't count the Mac's resource fork). */

	static 	XFileSpec	GetTempFile(const std::wstring& prefix, const std::wstring& extension);
						/**< Returns a spec to an unused file name in the Temporary items folder.
						The file name will be prefix + digit + extension string. Unlike the 
						Mac, Windows does not automatically delete these files. */
						
	static 	XFileSpec	GetUniqueFile(const XFileSpec& file);
						/**< Returns a file spec for an unused file that is similar to 'file'.
						(This works like the Finder's copy command: if 'file's name is "Hello", 
						and exists, a spec for a file named "Hello Copy" is returned). */
	//@}

	//! @name Files and/or Folders
	//@{
	static 	bool 		SpecExists(const XFileSystemSpec& spec);
						/**< Returns true if spec points to a file or folder. */

	static	bool		IsFile(const XFileSystemSpec& spec);

	static	bool		IsFolder(const XFileSystemSpec& spec);
	
	static	bool		IsReadOnly(const XFileSystemSpec& spec);
	//@}

	//! @name Volumes
	//@{
	static 	uint64		GetVolumeTotalBytes(const OSVolume& volume);

	static 	uint64		GetVolumeFreeBytes(const OSVolume& volume);
	
	static	uint32		GetMaxFileSpecLength(const OSVolume& volume);
						/**< Returns the maximum length of a file or folder name. */

	static	std::wstring GetVolumeName(const OSVolume& volume);
	
	static	bool 		IsNetworkVolume(const OSVolume& volume);

	static	bool 		IsValidName(const std::wstring& name, const OSVolume& volume);

	static	std::wstring GetValidName(const std::wstring& name, const OSVolume& volume, uint32 maxLen = 31);
						/**< Converts name to a valid file/folder name by replacing bad characters
						and/or truncating. */

#if WIN
	static	bool 		IsFAT(const OSVolume& volume);

	static	bool 		IsNTFS(const OSVolume& volume);
#endif
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
