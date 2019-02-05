/*
 *  File:		XFileSystemSpec.h
 *  Summary:	An object that points to a (possibly non-existent) file/folder.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFileSystemSpec.h,v $
 *		Revision 1.4  2001/03/05 05:41:34  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:52:30  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:35:57  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#if MAC
	#include <Aliases.h>
	#include <Files.h>
	#include <Resources.h>
#endif

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFolderSpec;
class XInStream;
class XOutStream;


// ===================================================================================
//	class XFileSystemSpec
//!		An object that points to a (possibly non-existent) file/folder.
// ===================================================================================
class FILES_EXPORT XFileSystemSpec {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XFileSystemSpec();
	
						XFileSystemSpec();
						/**< Points to a bogus file. */
						
						XFileSystemSpec(const OSFSSpec& spec);

						XFileSystemSpec(const XFolderSpec& folder, const std::wstring& fileName);
	
#if MAC
						XFileSystemSpec(int16 volume, int32 parID, const std::wstring& fileName);

						XFileSystemSpec(AliasHandle alias);

	explicit			XFileSystemSpec(ResID aliasID);

	explicit			XFileSystemSpec(const std::wstring& path);
#endif

//-----------------------------------
//	API
//
public:
	//! @name Conversion Operators
	//@{
			OSFSSpec& 	GetOSSpec() 										{return mSpec;}
				
			const OSFSSpec& GetOSSpec() const								{return mSpec;}
	//@}

	//! @name Comparisons
	//@{
			bool 		operator==(const XFileSystemSpec& rhs) const;

			bool 		operator!=(const XFileSystemSpec& rhs) const		{return !this->operator==(rhs);}
	//@}
			
	//! @name Information
	//@{
			std::wstring GetName() const;
						/**< Returns the name of the file/folder. */
			
			std::wstring GetPrefix() const;
						/**< Returns the name sans extension. */

			std::wstring GetExtension() const;
						/**< Returns the text after the last period (may be empty).
						Note that extensions are always returned as lower case. */

			XFolderSpec GetParent() const;
			
			OSVolume 	GetVolume() const;

			std::wstring GetPath() const;
	
#if MAC			
			int32		GetParID() const								{return mSpec.parID;}
#endif
	//@}

	//! @name Streaming
	//@{
	friend 	XInStream& 	operator>>(XInStream& stream, XFileSystemSpec& value);
	
	friend 	XOutStream& operator<<(XOutStream& stream, const XFileSystemSpec& value);
	//@}

//-----------------------------------
//	Member Data
//
protected:
	OSFSSpec				mSpec;

#if WIN
	mutable std::wstring	mLongName;
#endif
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
