/*
 *  File:		XMemoryMappedFile.h
 *  Summary:	A class that allows a file to be efficiently treated as a bag'o'bits.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XMemoryMappedFile.h,v $
 *		Revision 1.5  2001/04/17 01:44:26  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/03/05 05:42:39  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:54:04  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:42:38  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XFile.h>
#include <XLockable.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XMemoryMappedFile
//!		A class that allows a file to be efficiently treated as a bag'o'bits.
// ===================================================================================
class FILES_EXPORT XMemoryMappedFile : public XLockableMixin {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XMemoryMappedFile();
						/**< ASSERTs if the file is still open (however it will close
						the file if it's open to make it easier to write exception
						safe code). */

	explicit			XMemoryMappedFile(const XFileSpec& file);

						XMemoryMappedFile(const XFolderSpec& folder, const std::wstring& fileName);
						
private:
						XMemoryMappedFile(const XMemoryMappedFile& rhs);
						
			XMemoryMappedFile& operator=(const XMemoryMappedFile& rhs);

//-----------------------------------
//	New API
//
public:
	//! @name Open/Close
	//@{
			void 		Open(EFilePermission perm, uint32 maxBytes = 0);
						/**< File should already exist. maxBytes is used with writeable files. If it's
						zero the file cannot be expanded. It can be set to an arbitrarily large
						value for growable files, but if enough memory (Mac) or swap file space 
						(Win) cannot be found an exception will be thrown. */

			void 		Open(OSType creator, OSType fileType, EFilePermission perm, uint32 maxBytes = 0);
						/**< Creates the file if it doesn't already exist.
						Perm can be kReadPermission, kWritePermission, or kReadWritePermission.
						creator and fileType are ignored on Windows. */

			void 		Close();
			void 		Close(uint32 newSize);
						/**< May throw an exception. For growable files you may specify the final file size. */

			bool 		IsOpened() const;
	//@}

	//! @name Read/Write
	//@{
			uint8* 		GetBuffer();
			const uint8* GetBuffer() const					{return const_cast<XMemoryMappedFile*>(this)->GetBuffer();}
						/**< File must be locked. */
						
			uint32 		GetBufferSize() const;
						/**< Returns the size in bytes of the buffer. The file must be open. */
			
			void 		Flush();
	//@}

	//! @name Misc
	//@{
			XFileSpec	GetSpec() const						{return mSpec;}
			
#if MAC
			void 		EnablePurging(bool enable = true);
						/**< On the Mac the buffer is allocated in temp memory when possible.
						In order to work well with other app's the handle defaults to
						purgeable (for read-only files), but you can disable purging with 
						this method. */
#endif
	//@}
	
//-----------------------------------
//	Inherited API
//
protected:
			void 		Invariant() const;

	virtual void 		OnLock(bool moveHigh);

	virtual void 		OnUnlock();
	
//-----------------------------------
//	Internal API
//
protected:
			void 		DoOpen(EFilePermission perm, uint32 flags, uint32 maxBytes);

//-----------------------------------
//	Member Data
//
protected:
	XFileSpec	mSpec;
	bool		mWriting;		
	uint32		mMaxBytes;

#if MAC
	int16		mRefNum;
	bool		mReading;
	Handle		mData;				// purgable handle allocated in temp memory (or app memory if temp memory is exhausted)
	
	bool		mPurgable;

#elif WIN
	HANDLE		mFileHandle;
	HANDLE		mMapHandle;
	void*		mData;
#endif
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
