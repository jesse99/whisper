/*
 *  File:		WMemoryMappedFile.cpp
 *  Summary:	A class that allows a file to be efficiently treated as a bag'o'bits.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WMemoryMappedFile.cpp,v $
 *		Revision 1.6  2001/04/27 09:19:08  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.5  2001/04/21 03:32:34  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/04/17 01:42:09  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.3  2000/12/10 04:07:02  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:10:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XMemoryMappedFile.h>

#include <WSystemInfo.h>
#include <XExceptions.h>
#include <XFolderSpec.h>
#include <XNumbers.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// ThrowErr
//
//---------------------------------------------------------------
static void ThrowErr(const XFileSpec& spec, uint32 err)
{
	if (err == ERROR_NOT_ENOUGH_MEMORY || err == ERROR_OUTOFMEMORY) {
		throw XSysMemoryException();

	} else if (WSystemInfo::HasUnicode()) {
		wchar_t mesg[512];
		uint32 count = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,					// source and processing options
									  nil,											// message source
									  err,											// message indentifier
									  MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),// language identifier
									  mesg,											// message buffer
									  sizeof(mesg),									// maximum size of message buffer
									  nil);											// message inserts

		throw std::runtime_error(ToUTF8Str((std::wstring(mesg, count) + LoadWhisperString(L" (with file '#1')", spec.GetName()))));	

	} else {
		char mesg[512];
		uint32 count = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,					// source and processing options
									  nil,											// message source
									  err,											// message indentifier
									  MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),// language identifier
									  mesg,											// message buffer
									  sizeof(mesg),									// maximum size of message buffer
									  nil);											// message inserts

		throw std::runtime_error(ToUTF8Str((FromPlatformStr(mesg, count) + LoadWhisperString(L" (with file '#1')", spec.GetName()))));	
	}
}		 


//---------------------------------------------------------------
//
// ThrowIfErr (XFileSpec, uint32)
//
//---------------------------------------------------------------
inline void ThrowIfErr(const XFileSpec& spec, uint32 err)						
{
	if (err != NO_ERROR) 
		ThrowErr(spec, err);
}


//---------------------------------------------------------------
//
// ThrowIfErr (XFileSpec)
//
//---------------------------------------------------------------
inline void ThrowIfErr(const XFileSpec& spec)						
{
	ThrowIfErr(spec, GetLastError());
}


//---------------------------------------------------------------
//
// ThrowIf
//
//---------------------------------------------------------------
inline void ThrowIf(const XFileSpec& spec, bool flag)						
{
	if (flag) {
		ThrowIfErr(spec, GetLastError()); 
		ThrowErr(spec, (uint32) E_FAIL);
	}
}


//---------------------------------------------------------------
//
// ThrowIfBadHandle
//
//---------------------------------------------------------------
inline void	ThrowIfBadHandle(const XFileSpec& spec, HANDLE h)					
{
	if (h == nil || h == INVALID_HANDLE_VALUE) {
		ThrowIfErr(spec, GetLastError()); 
		ThrowErr(spec, ERROR_INVALID_HANDLE);
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XMemoryMappedFile
// ===================================================================================

//---------------------------------------------------------------
//
// XMemoryMappedFile::~XMemoryMappedFile
//
//---------------------------------------------------------------
XMemoryMappedFile::~XMemoryMappedFile()
{
	PRECONDITION(!this->IsOpened());			// in the normal course of events you should explicitly close the file
	CALL_INVARIANT;
	
	if (this->IsOpened()) {				// but if an exception was thrown the file may still be open
		if (mData != nil) {
			(void) ::UnmapViewOfFile(mData);
			mData = nil;
		}

		if (mMapHandle != nil && mMapHandle != INVALID_HANDLE_VALUE) {
			(void) ::CloseHandle(mMapHandle);
			mMapHandle = nil;
		}

		if (mFileHandle != nil && mFileHandle != INVALID_HANDLE_VALUE) {
			(void) ::CloseHandle(mFileHandle);
			mFileHandle = nil;
		}
	}

	mFileHandle = nil;
	mMapHandle  = nil;	
	mData = nil;
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::XMemoryMappedFile (XFileSpec)
//
//---------------------------------------------------------------
XMemoryMappedFile::XMemoryMappedFile(const XFileSpec& file) : mSpec(file)
{
	mFileHandle = nil;
	mMapHandle  = nil;	
	mData       = nil;

	mMaxBytes = 0;
	mWriting = false;

	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::XMemoryMappedFile (XFolderSpec, wstring)
//
//---------------------------------------------------------------
XMemoryMappedFile::XMemoryMappedFile(const XFolderSpec& folder, const std::wstring& name) : mSpec(folder, name)
{
	mFileHandle = nil;
	mMapHandle  = nil;
	mData       = nil;

	mMaxBytes = 0;
	mWriting = false;

	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::Open (EFilePermission, uint32)
//
//---------------------------------------------------------------
void XMemoryMappedFile::Open(EFilePermission perm, uint32 maxBytes)
{
	CHECK_INVARIANT;

	this->DoOpen(perm, OPEN_EXISTING, maxBytes);

	POSTCONDITION(this->IsOpened());
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::Open (OSType, OSType, EFilePermission, uint32)
//
//---------------------------------------------------------------
void XMemoryMappedFile::Open(OSType creator, OSType fileType, EFilePermission perm, uint32 maxBytes)
{
	UNUSED(creator);
	UNUSED(fileType);
	CHECK_INVARIANT;
	
	this->DoOpen(perm, OPEN_ALWAYS, maxBytes);

	POSTCONDITION(this->IsOpened());
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::Close ()
//
//---------------------------------------------------------------
void XMemoryMappedFile::Close()
{
	PRECONDITION(this->IsOpened());
	CHECK_INVARIANT;
	
	int32 succeeded1 = ::UnmapViewOfFile(mData);
	int32 succeeded2 = ::CloseHandle(mMapHandle);
	int32 succeeded3 = ::CloseHandle(mFileHandle);

	mFileHandle = nil;
	mMapHandle  = nil;
	mData       = nil;
	mWriting    = false;
	
	ThrowIf(!succeeded1 || !succeeded2 || !succeeded3);

	POSTCONDITION(!this->IsOpened());
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::Close (uint32)
//
//---------------------------------------------------------------
void XMemoryMappedFile::Close(uint32 newSize)
{
	PRECONDITION(this->IsOpened());
	PRECONDITION(mWriting);
	CHECK_INVARIANT;

	int32 succeeded1 = ::UnmapViewOfFile(mData);		// need to close the file map handle before we can use normal file manager calls
	int32 succeeded2 = ::CloseHandle(mMapHandle);
	int32 succeeded3 = true;
	
	if (succeeded1 && succeeded2) {
		::SetLastError(0);								// Most Windows functions only set this when they fail. We set it here because of the screwy way SetFilePointer reports errors.
		int32 size = numeric_cast<int32>(newSize);
		(void) ::SetFilePointer(mFileHandle, 0, &size, FILE_BEGIN);
		succeeded3 = GetLastError() == NO_ERROR;
	
		succeeded3 = succeeded3 && ::SetEndOfFile(mFileHandle);
	}

	if (::CloseHandle(mFileHandle) == 0)
		succeeded3 = false;

	mFileHandle = nil;
	mMapHandle  = nil;
	mData       = nil;
	mWriting    = false;
	
	ThrowIf(!succeeded1 || !succeeded2 || !succeeded3);

	POSTCONDITION(!this->IsOpened());
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::IsOpened
//
//---------------------------------------------------------------
bool XMemoryMappedFile::IsOpened() const
{
	bool open = mFileHandle != nil && mFileHandle != INVALID_HANDLE_VALUE;
	
	return open;
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::GetBuffer
//
//---------------------------------------------------------------
uint8* XMemoryMappedFile::GetBuffer()
{
	PRECONDITION(this->IsLocked());		// Mac requirement
	PRECONDITION(mData != nil);
	
	return (uint8*) mData;
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::GetBufferSize
//
//---------------------------------------------------------------
uint32 XMemoryMappedFile::GetBufferSize() const
{
	PRECONDITION(this->IsOpened());		// mMaxBytes is set via Open
		
	return mMaxBytes;
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::Flush
//
//---------------------------------------------------------------
void XMemoryMappedFile::Flush()
{
	PRECONDITION(mWriting);
	PRECONDITION(this->IsOpened());
	CHECK_INVARIANT;
	
	int32 succeeded = ::FlushViewOfFile(mData, 0);
	ThrowIf(mSpec, !succeeded);

	POSTCONDITION(true);
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XMemoryMappedFile::Invariant
//
//---------------------------------------------------------------
void XMemoryMappedFile::Invariant() const
{	
	if (mFileHandle == nil || mFileHandle != INVALID_HANDLE_VALUE) {
		ASSERT(mMapHandle != nil && mMapHandle != INVALID_HANDLE_VALUE);
		ASSERT(mData != nil);
		
	} else {
		ASSERT(mMapHandle == nil || mMapHandle == INVALID_HANDLE_VALUE);
		ASSERT(mData == nil);
	}
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::OnLock
//
//---------------------------------------------------------------
void XMemoryMappedFile::OnLock(bool moveHigh)
{
	PRECONDITION(this->IsOpened());
	UNUSED(moveHigh);

	// don't need to lock on Windows
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::OnUnlock
//
//---------------------------------------------------------------
void XMemoryMappedFile::OnUnlock()
{
	// don't need to lock on Windows
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XMemoryMappedFile::DoOpen 
//
//---------------------------------------------------------------
void XMemoryMappedFile::DoOpen(EFilePermission perm, uint32 flags, uint32 maxBytes)
{
	PRECONDITION(!this->IsOpened());
	
	mFileHandle = nil;
	mMapHandle  = nil;
	mData       = nil;
	
	mMaxBytes = maxBytes;

	// Open the file handle
	int32 succeeded;
	if (WSystemInfo::HasUnicode())
		succeeded = ::SetCurrentDirectoryW(mSpec.GetParent().GetPath().c_str());
	else
		succeeded = ::SetCurrentDirectoryA(ToPlatformStr(mSpec.GetParent().GetPath()).c_str());
	ThrowIf(mSpec, !succeeded);
	
	if (WSystemInfo::HasUnicode())
		mFileHandle = ::CreateFileW(mSpec.GetName().c_str(), perm, 0, nil, flags, FILE_ATTRIBUTE_NORMAL, nil);
	else
		mFileHandle = ::CreateFileA(ToPlatformStr(mSpec.GetName()).c_str(), perm, 0, nil, flags, FILE_ATTRIBUTE_NORMAL, nil);
	ThrowIfBadHandle(mSpec, mFileHandle);
	
	try {
		// Find the maximum file size if it wasn't specified by the user
		if (mMaxBytes == 0) {			
			uint32 loBytes, hiBytes;	
			::SetLastError(0);				// Most Windows functions only set this when they fail. We set it here because of the screwy way GetFileSize reports errors.
			loBytes = ::GetFileSize(mFileHandle, &hiBytes);
			ThrowIfErr(mSpec);
			ThrowIf(hiBytes != 0);
							
			mMaxBytes = loBytes;
		}
	
		// Open the memory map handle
		SetLastError(NO_ERROR);
		uint32 protection = (uint32) (perm == kReadPermission ? PAGE_READONLY : PAGE_READWRITE);
		mMapHandle = ::CreateFileMapping(mFileHandle, nil, protection, 0, mMaxBytes, nil);
		ThrowIfBadHandle(mSpec, mMapHandle);
		
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			::CloseHandle(mMapHandle);
			mMapHandle = nil;
			
			ThrowErr(ERROR_ALREADY_EXISTS);
		}
		
		// Map the file into our address space
		uint32 access = (uint32) (perm == kReadPermission ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS);
		mData = ::MapViewOfFile(mMapHandle, access, 0, 0, 0);
		ThrowIf(mData == nil);

	} catch (...) {
		if (mMapHandle != nil && mMapHandle != INVALID_HANDLE_VALUE) {
			(void) ::CloseHandle(mMapHandle);
			mMapHandle = nil;
		}

		if (mFileHandle != nil && mFileHandle != INVALID_HANDLE_VALUE) {
			(void) ::CloseHandle(mFileHandle);
			mFileHandle = nil;
		}
		
		throw;
	}

	mWriting = perm == kWritePermission || perm == kReadWritePermission;
}


}	// namespace Whisper
