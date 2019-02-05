/*
 *  File:		XFileSpec.h
 *  Summary:	An object that points to a (possibly non-existent) file.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFileSpec.h,v $
 *		Revision 1.6  2001/04/27 04:16:45  jesjones
 *		Added XPreference specialization.
 *		
 *		Revision 1.5  2001/03/25 06:13:46  jesjones
 *		Moved the constant to the top of the file.
 *		
 *		Revision 1.4  2000/12/10 08:52:15  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.3  2000/11/25 01:48:48  jesjones
 *		HasMetaData and SetMetaData no longer throw on FAT file systems.
 *		
 *		Revision 1.2  2000/11/09 12:35:39  jesjones
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

#pragma once

#include <XFileSystemSpec.h>
#include <XHandle.h>
#include <XPreference.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFileSpec;


//-----------------------------------
//	Constants
//
extern FILES_EXPORT const XFileSpec kInvalidFileSpec;


// ===================================================================================
//	class XFileSpec
//!		An object that points to a (possibly non-existent) file.
// ===================================================================================
class FILES_EXPORT XFileSpec : public XFileSystemSpec {

	typedef XFileSystemSpec Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XFileSpec();
	
						XFileSpec();
						/**< Points to a bogus file. */
						
						XFileSpec(const OSFSSpec& spec);

						XFileSpec(const XFolderSpec& folder, const std::wstring& fileName);
	
	explicit			XFileSpec(const XFileSystemSpec& spec);

#if MAC
						XFileSpec(int16 volume, int32 parID, const std::wstring& fileName);

						XFileSpec(AliasHandle alias);

	explicit			XFileSpec(const std::wstring& path);

	explicit			XFileSpec(ResID aliasID);
#endif

//-----------------------------------
//	New API
//
public:
	// ----- Info -----
#if MAC			
			OSType		GetCreator() const;
			
		 	OSType		GetType() const;
#endif

	// ----- Meta-Data -----
			void 		SetMetaData(const std::wstring& name, const void* buffer, uint32 bytes);
						/**< Saves the data in a resource (Mac) or a stream (NTFS). No-op on FAT
						file systems. */

			void 		GetMetaData(const std::wstring& name, void* buffer, uint32 bytes) const;
						/**< Throws on FAT file systems. */
			
			XHandle 	GetMetaData(const std::wstring& name) const;
						/**< Throws on FAT file systems. */

			bool 		HasMetaData(const std::wstring& name) const;
						/**< Returns false on FAT file systems. */
};


// ===================================================================================
//	class XPreference<XFileSpec>
//		Core project is decoupled from Files so we have to define this here.
// ===================================================================================
template <> 
class FILES_EXPORT XPreference<XFileSpec> {

public:
						~XPreference();
	
						XPreference(const std::wstring& name, PrefType type = kUserPref);
						
public:
								operator const XFileSpec*() const	{return &mData;}
								operator XFileSpec*()				{return &mData;}
						
			const XFileSpec&	operator*() const					{return mData;}
			XFileSpec&			operator*()							{return mData;}
						
			const XFileSpec*	operator->() const					{return &mData;}
			XFileSpec*			operator->()						{return &mData;}
			
			void 		Flush();
	
private:
	std::wstring	mName;	
	PrefType		mType;				
	
	XFileSpec		mData;
	XFileSpec		mOldData;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
