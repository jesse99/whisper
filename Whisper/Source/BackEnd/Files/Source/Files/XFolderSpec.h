/*
 *  File:		XFolderSpec.h
 *  Summary:	An object that points to a (possibly non-existent) directory.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFolderSpec.h,v $
 *		Revision 1.6  2001/04/27 04:16:49  jesjones
 *		Added XPreference specialization.
 *		
 *		Revision 1.5  2001/03/25 06:13:49  jesjones
 *		Moved the constant to the top of the file.
 *		
 *		Revision 1.4  2001/03/05 05:41:44  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:52:45  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:36:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XConstants.h>
#include <XFileSystemSpec.h>
#include <XPreference.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFileFilter; 
class XFileIterator;
class XFolderFilter;
class XFolderSpec;


//-----------------------------------
//	Constants
//
extern FILES_EXPORT const XFolderSpec kInvalidFolderSpec;
#if WIN
	extern const OSVolume kOnSystemDisk;		// this isn't actually used...
#endif


// ===================================================================================
//	class XFolderSpec
//!		An object that points to a (possibly non-existent) directory.
// ===================================================================================
class FILES_EXPORT XFolderSpec : public XFileSystemSpec {

	typedef XFileSystemSpec Inherited;

//-----------------------------------
//	Types
//
public:
    typedef XFileIterator iterator;	

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XFolderSpec();
	
						XFolderSpec();
						/**< Points to a bogus directory. */
					
						XFolderSpec(const OSFSSpec& spec);
					
						XFolderSpec(const XFolderSpec& parent, const std::wstring& name);
						
	explicit			XFolderSpec(const XFileSystemSpec& spec);

#if MAC
						XFolderSpec(int16 volume, int32 dir);

						XFolderSpec(int16 volume, int32 parDir, const std::wstring& name);

						XFolderSpec(AliasHandle alias);
					
	explicit			XFolderSpec(const std::wstring& path);

	explicit			XFolderSpec(ResID aliasID);
#endif

//-----------------------------------
//	New API
//
public:
#if MAC			
	//! @name Mac Information
	//@{
			int32		GetDirID() const;
#endif
	//@}

	//! @name Special Folders
	//@{
	static 	XFolderSpec	GetAppFolder();
		
	static 	XFolderSpec	GetTempFolder(const OSVolume& volume = kOnSystemDisk);
						/**< Volume is ignored on Windows. */

#if MAC
	static 	XFolderSpec	GetSystemFolder()								{return XFolderSpec::GetFolder(kSystemFolderType);}

	static 	XFolderSpec	GetPrefsFolder()								{return XFolderSpec::GetFolder(kPreferencesFolderType);}

	static 	XFolderSpec	GetTrashFolder(int16 volume = kOnSystemDisk)	{return XFolderSpec::GetFolder(kTrashFolderType, volume);}

	static 	XFolderSpec	GetFolder(OSType folder, int16 vRefNum = kOnSystemDisk, bool createFolder = true);
						/**< Uses FindFolder to create the spec. Folder can be kSystemFolderType, 
						kDesktopFolderType, kTrashFolderType, kWhereToEmptyTrashFolderType, 
						kPrintMonitorDocsFolderType, kStartupFolderType, kShutdownFolderType, 
						kAppleMenuFolderType, kControlPanelFolderType, kExtensionFolderType, 
						kFontsFolderType, kPreferencesFolderType, or kTemporaryFolderType. */
#elif WIN
	static 	XFolderSpec	GetSystemFolder();
						/**< Note that application's will not have write access to the system
						folder if a shared version of Windows is running. */

	static 	XFolderSpec	GetWindowsFolder();
#endif
	//@}

	//! @name Iterating
	//@{
			iterator 	begin(bool recursive = kNonRecursive) const;
			
			iterator 	begin(XFileFilter* takeFileFilter, XFolderFilter* takeFolderFilter, bool recursive = kNonRecursive) const;
			
			iterator 	end() const;
	//@}
};


// ===================================================================================
//	class XPreference<XFolderSpec>
//		Core project is decoupled from Files so we have to define this here.
// ===================================================================================
template <> 
class FILES_EXPORT XPreference<XFolderSpec> {

public:
						~XPreference();
	
						XPreference(const std::wstring& name, PrefType type = kUserPref);
						
public:
								operator const XFolderSpec*() const	{return &mData;}
								operator XFolderSpec*()				{return &mData;}
						
			const XFolderSpec&	operator*() const					{return mData;}
			XFolderSpec&		operator*()							{return mData;}
						
			const XFolderSpec*	operator->() const					{return &mData;}
			XFolderSpec*		operator->()						{return &mData;}
			
			void 		Flush();
	
private:
	std::wstring	mName;	
	PrefType		mType;				
	
	XFolderSpec		mData;
	XFolderSpec		mOldData;
};


// ===================================================================================
//	Inlines
// ===================================================================================
#if MAC
	inline XFolderSpec XFolderSpec::GetTempFolder(const OSVolume& volume)	
	{
		return XFolderSpec::GetFolder(kTemporaryFolderType, volume);
	}
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
