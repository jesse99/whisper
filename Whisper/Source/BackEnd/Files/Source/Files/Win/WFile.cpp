/*
 *  File:		WFile.cpp
 *  Summary:	A class that encapsulates reading and writing to a file.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WFile.cpp,v $
 *		Revision 1.4  2001/04/27 09:18:09  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.3  2001/04/21 03:31:20  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 12:06:57  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 3/13/99	JDJ		SetLength restores the original file pointer position.
 *		 <3>	 2/26/99	JDJ		GetLength now returns a uint32. Added GetLength64 which returns a uint64.
 *									Ditto for GetPosition.
 *		 <2>	12/08/98	JDJ		Exception messages include the file name.
 *		 <1>	 1/10/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XFile.h>

#include <WSystemInfo.h>
#include <XConstants.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XFolderSpec.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	Global Functions
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

		throw std::runtime_error(ToUTF8Str((std::wstring(FromPlatformStr(mesg, count)) + LoadWhisperString(L" (with file '#1')", spec.GetName()))));	
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
//	class XFile
// ===================================================================================

//---------------------------------------------------------------
//
// XFile::~XFile
//
//---------------------------------------------------------------
XFile::~XFile()
{
	PRECONDITION(!this->IsOpened());			// in the normal course of events you should explicitly close the file
										// $$$ skip this ASSERT if uncaught_exception() returns true
										
	if (this->IsOpened())				// but if an exception was thrown the file may still be open
		(void) ::CloseHandle(mHandle);	// if so, we need to close the file

	mHandle = nil;
}


//---------------------------------------------------------------
//
// XFile::XFile (XFileSpec)
//
//---------------------------------------------------------------
XFile::XFile(const XFileSpec& file) : mSpec(file)
{		
	mHandle = nil;
}


//---------------------------------------------------------------
//
// XFile::XFile (XFolderSpec, string)
//
//---------------------------------------------------------------
XFile::XFile(const XFolderSpec& folder, const std::wstring& name) : mSpec(folder, name)
{
	mHandle = nil;
}


//---------------------------------------------------------------
//
// XFile::GetLength
//
//---------------------------------------------------------------
uint32 XFile::GetLength() const
{
	PRECONDITION(this->IsOpened());
	
	::SetLastError(0);				// Most Windows functions only set this when they fail. We set it here because of the screwy way GetFileSize reports errors.
	
	uint32 loBytes, hiBytes;	
	loBytes = ::GetFileSize(mHandle, &hiBytes);
	ThrowIfErr(mSpec);
		
	uint64 bytes = hiBytes;
	bytes = (bytes << 32) + loBytes;

	if (bytes > ULONG_MAX) {
		std::wstring mesg = LoadWhisperString(L"File #1's length is over 32 bits.", mSpec.GetName());
		throw std::range_error(ToUTF8Str(mesg));				
	}
	
	return (uint32) bytes;	
}


//---------------------------------------------------------------
//
// XFile::GetLength64
//
//---------------------------------------------------------------
uint64 XFile::GetLength64() const
{
	PRECONDITION(this->IsOpened());
	
	::SetLastError(0);				// Most Windows functions only set this when they fail. We set it here because of the screwy way GetFileSize reports errors.
	
	uint32 loBytes, hiBytes;	
	loBytes = ::GetFileSize(mHandle, &hiBytes);
	ThrowIfErr(mSpec);
		
	uint64 bytes = hiBytes;
	bytes = (bytes << 32) + loBytes;

	return bytes;	
}


//---------------------------------------------------------------
//
// XFile::SetLength
//
//---------------------------------------------------------------
void XFile::SetLength(uint64 newSize)
{
	PRECONDITION(this->IsOpened());

	uint64 oldPos = this->GetPosition64();
	
	int32 hiPos = (int32) (newSize >> 32);
	int32 loPos = (int32) newSize;
	
	::SetLastError(0);				// Most Windows functions only set this when they fail. We set it here because of the screwy way SetFilePointer reports errors.
	(void) ::SetFilePointer(mHandle, loPos, &hiPos, FILE_BEGIN);
	ThrowIfErr(mSpec);

	int32 succeeded = ::SetEndOfFile(mHandle);
	ThrowIf(mSpec, !succeeded);

	this->Seek(kSeekFromStart, (int64) oldPos);
}


//---------------------------------------------------------------
//
// XFile::Open (OSType, OSType, EFilePermission, bool)
//
//---------------------------------------------------------------
void XFile::Open(OSType creator, OSType fileType, EFilePermission perm, bool fork)
{
	UNUSED(creator);
	UNUSED(fileType);
	PRECONDITION(!this->IsOpened());
	PRECONDITION(fork == kDataFork);
	
	int32 succeeded;
	if (WSystemInfo::HasUnicode())
		succeeded = ::SetCurrentDirectoryW(mSpec.GetParent().GetPath().c_str());
	else
		succeeded = ::SetCurrentDirectoryA(ToPlatformStr(mSpec.GetParent().GetPath()).c_str());
	ThrowIf(mSpec, !succeeded);
	
	if (WSystemInfo::HasUnicode())
		mHandle = ::CreateFileW(mSpec.GetName().c_str(), perm, 0, nil, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nil);
	else
		mHandle = ::CreateFileA(ToPlatformStr(mSpec.GetName()).c_str(), perm, 0, nil, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nil);
	ThrowIfBadHandle(mSpec, mHandle);
}


//---------------------------------------------------------------
//
// XFile::Open (EFilePermission, bool)
//
//---------------------------------------------------------------
void XFile::Open(EFilePermission perm, bool fork)
{
	PRECONDITION(!this->IsOpened());
	PRECONDITION(fork == kDataFork);
	
	int32 succeeded;
	if (WSystemInfo::HasUnicode())
		succeeded = ::SetCurrentDirectoryW(mSpec.GetParent().GetPath().c_str());
	else
		succeeded = ::SetCurrentDirectoryA(ToPlatformStr(mSpec.GetParent().GetPath()).c_str());
	ThrowIf(mSpec, !succeeded);
	
	if (WSystemInfo::HasUnicode())
		mHandle = ::CreateFileW(mSpec.GetName().c_str(), perm, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nil);
	else
		mHandle = ::CreateFileA(ToPlatformStr(mSpec.GetName()).c_str(), perm, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nil);
	ThrowIfBadHandle(mSpec, mHandle);
}


//---------------------------------------------------------------
//
// XFile::Close
//
//---------------------------------------------------------------
void XFile::Close()
{
	PRECONDITION(this->IsOpened());
	
	if (mHandle != nil && mHandle != INVALID_HANDLE_VALUE) {
		int32 succeeded = ::CloseHandle(mHandle);
	
		mHandle = nil;						// make sure this is reset!
	
		ThrowIf(mSpec, !succeeded);			// it'd be nice if we could ignore errors here, but the file system may have buffered writes, and we could get an error when the buffer is flushed
	}
}


//---------------------------------------------------------------
//
// XFile::IsOpened
//
//---------------------------------------------------------------
bool XFile::IsOpened() const
{
	bool open = mHandle != nil && mHandle != INVALID_HANDLE_VALUE;
	
	return open;
}


//---------------------------------------------------------------
//
// XFile::Seek
//
//---------------------------------------------------------------
void XFile::Seek(ESeekMode mode, int64 offset)
{
	::SetLastError(0);						// Most Windows functions only set this when they fail. We set it here because of the screwy way SetFilePointer reports errors.
	
	int32 hiPos = (int32) (offset >> 32);
	int32 loPos = (int32) offset;
	
	uint32 position = ::SetFilePointer(mHandle, loPos, &hiPos, mode);	
	ThrowIfErr(mSpec);
	
	PRECONDITION(position <= this->GetLength());	// Mac doesn't support seeking past eof
}


//---------------------------------------------------------------
//
// XFile::GetPosition
//
//---------------------------------------------------------------
uint32 XFile::GetPosition() const
{
	::SetLastError(0);						// Most Windows functions only set this when they fail. We set it here because of the screwy way SetFilePointer reports errors.
	
	int32 hiPos = 0;
	uint32 loPos = ::SetFilePointer(mHandle, 0, &hiPos, FILE_CURRENT);
	ThrowIfErr(mSpec);
		
	uint64 position = hiPos;
	position = (position << 32) + loPos;

	if (position > ULONG_MAX) {
		std::wstring mesg = LoadWhisperString(L"File #1's file pointer is over 32 bits.", mSpec.GetName());
		throw std::range_error(ToUTF8Str(mesg));				
	}
	
	return (uint32) position;
}


//---------------------------------------------------------------
//
// XFile::GetPosition64
//
//---------------------------------------------------------------
uint64 XFile::GetPosition64() const
{
	::SetLastError(0);						// Most Windows functions only set this when they fail. We set it here because of the screwy way SetFilePointer reports errors.
	
	int32 hiPos = 0;
	uint32 loPos = ::SetFilePointer(mHandle, 0, &hiPos, FILE_CURRENT);
	ThrowIfErr(mSpec);
		
	uint64 position = hiPos;
	position = (position << 32) + loPos;

	return position;
}


//---------------------------------------------------------------
//
// XFile::Read
//
//---------------------------------------------------------------
void XFile::Read(void* buffer, uint32 inBytes)
{
	PRECONDITION(this->IsOpened());
	PRECONDITION(buffer != nil);
	
	if (inBytes > 0) {
		uint32 outBytes;
		int32 succeeded = ::ReadFile(mHandle, buffer, inBytes, &outBytes, nil);
		ThrowIf(mSpec, !succeeded || outBytes != inBytes);
	}
}


//---------------------------------------------------------------
//
// XFile::Write
//
//---------------------------------------------------------------
void XFile::Write(const void* buffer, uint32 inBytes)
{
	PRECONDITION(this->IsOpened());
	PRECONDITION(buffer != nil);

	if (inBytes > 0) {
		uint32 outBytes;
		int32 succeeded = ::WriteFile(mHandle, buffer, inBytes, &outBytes, nil);
		ThrowIf(mSpec, !succeeded || outBytes != inBytes);
	}
}


//---------------------------------------------------------------
//
// XFile::Flush
//
//---------------------------------------------------------------
void XFile::Flush()
{
	PRECONDITION(this->IsOpened());

	int32 succeeded = ::FlushFileBuffers(mHandle);
	ThrowIf(mSpec, !succeeded);
}


}	// namespace Whisper
