/*
 *  File:		XFile.h
 *  Summary:	A class that encapsulates reading and writing to a file.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFile.h,v $
 *		Revision 1.4  2000/12/08 13:09:04  jesjones
 *		Testbed for doxygen commenting.
 *		
 *		Revision 1.3  2000/11/25 01:48:58  jesjones
 *		Added Trim.
 *		
 *		Revision 1.2  2000/11/09 12:34:59  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 4/11/99	JDJ		Removed virtual keyword from a few functions.
 *		 <3>	 2/26/99	JDJ		GetLength now returns a uint32. Added GetLength64 which returns a uint64.
 *									Ditto for GetPosition.
 *		 <2>	12/18/98	JDJ		Made XFileSpec ctor explicit.
 *		 <1>	 1/10/98	JDJ		Created
 */

#pragma once

#include <XDebug.h>
#include <XFileSpec.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Constants
//
#if MAC
	//! Controls what you can do with a file after it's opened
	enum EFilePermission {
		kReadPermission = fsRdPerm, 		//!< the file can only be read from
		kWritePermission = fsWrPerm, 		//!< the file can only be written to
		kReadWritePermission = fsRdWrPerm	//!< the file can be read and written to
		};

	//! Controls how the file pointer is moved
	enum ESeekMode {
		kSeekFromStart = fsFromStart, 	//!< the file pointer is relative to the start of the file
		kSeekFromEnd = fsFromLEOF, 		//!< the file pointer is relative to the end of the file
		kSeekFromCurrent = fsFromMark	//!< the file pointer is relative to itself
		};
		
#elif WIN
	enum EFilePermission {
		kReadPermission = GENERIC_READ, 
		kWritePermission = GENERIC_WRITE,
		 kReadWritePermission = GENERIC_READ + GENERIC_WRITE};

	enum ESeekMode {
		kSeekFromStart = FILE_BEGIN, 
		kSeekFromEnd = FILE_END, 
		kSeekFromCurrent = FILE_CURRENT};
#endif
	
#if MAC
#elif WIN
#endif

const bool kDataFork = true;
const bool kRsrcFork = false;			// used (rarely) to bypass the resource manager


// ===================================================================================
//	class XFile
//! 	A class that encapsulates reading and writing to a file.
// ===================================================================================
class FILES_EXPORT XFile {

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XFile();
			    		/**< ASSERTs if the file is still open (however it will close
						the file if it's open to make it easier to write exception
						safe code). */

	explicit			XFile(const XFileSpec& file);

						XFile(const XFolderSpec& folder, const std::wstring& fileName);
						
private:
						XFile(const XFile& rhs);
						
			XFile&		operator=(const XFile& rhs);

//-----------------------------------
//	API
//
public:
	//! @name Size
	//@{
			uint32 		GetLength() const;
			uint64 		GetLength64() const;
						/**< File must be open. (Note that you can use XFileSystem::GetSize with closed files). */
			
			void 		SetLength(uint64 newSize);
						/**< File must be open. Note that this leaves the file pointer undisturbed. */
						
			void 		Trim()								{this->SetLength(this->GetPosition64());}
						/**< Sets the file's length to where ever the file pointer is.
						You should call this after writing to an existing file. */
	//@}
	
	//! @name Open/Close
	//@{
			void 		Open(EFilePermission perm, bool fork = kDataFork);
						/**< File should already exist. Perm can be kReadPermission, kWritePermission, 
						or kReadWritePermission. */

			void 		Open(OSType creator, OSType fileType, EFilePermission perm, bool fork = kDataFork);
						/**< Creates the file if it doesn't already exist. Creator and fileType are 
						ignored on Windows. */

			void 		Close();
						/**< May throw an exception. */

			bool 		IsOpened() const;
	//@}

	//! @name Seeking
	//@{
			void 		Seek(ESeekMode mode, int64 offset);
						/**< Mode can be kSeekFromStart, kSeekFromEnd, or kSeekFromMark. */
	
			void 		SeekToStart()						{this->Seek(kSeekFromStart, 0);}
			void 		SeekToEnd()							{this->Seek(kSeekFromEnd, 0);}
	
			uint32 		GetPosition() const;
			uint64 		GetPosition64() const;
	//@}

	//! @name Read/Write
	//@{
			void 		Read(void* buffer, uint32 bytes);

			void 		Write(const void* buffer, uint32 bytes);
			
			void 		Flush();
	//@}
	
	//! @name Misc
	//@{
			XFileSpec	GetSpec() const						{return mSpec;}
	//@}
	
//-----------------------------------
//	Member Data
//
protected:
	XFileSpec	mSpec;

#if MAC
	int16		mRefNum;
	bool		mOpenForWrite;		//!< Mac only supports write-only access for shared volumes so we'll use this to ASSERT if we try to read with kWritePermission
#elif WIN
	HANDLE		mHandle;
#endif
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
