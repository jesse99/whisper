/*
 *  File:		MMemoryMappedFile.cpp
 *  Summary:	A class that allows a file to be efficiently treated as a bag'o'bits.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MMemoryMappedFile.cpp,v $
 *		Revision 1.5  2001/04/21 03:26:52  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/04/17 01:41:09  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.3  2000/12/10 04:04:14  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:15:45  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XMemoryMappedFile.h>

#include <XError.h>
#include <XExceptions.h>
#include <XIntConversions.h>
#include <XLocker.h>
#include <XNumbers.h>
#include <XStringUtils.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
#if DEBUG
	const uint32 kTail     = 'Tail';
	const uint32 kTailSize = sizeof(kTail);
#else
	const uint32 kTailSize = 0;
#endif


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// ThrowFileErr
//
//---------------------------------------------------------------
static void ThrowFileErr(const XFileSpec& spec, OSStatus err)
{	
	switch (err) {				
		case dirFulErr:
		case dskFulErr:
		case nsvErr:
		case ioErr:
		case fnOpnErr:
		case eofErr:
		case posErr:
		case mFulErr:
		case tmfoErr:
		case fnfErr:
		case wPrErr:
		case fLckdErr:

		case vLckdErr:
		case fBsyErr:
		case dupFNErr:
		case opWrErr:
		case rfNumErr:
		case gfpErr:
		case volOffLinErr:
		case permErr:
		case volOnLinErr:
		case nsDrvErr:
		case noMacDskErr:
		case wrPermErr:
		case dirNFErr:
		case tmwdoErr:
		case volGoneErr:
			{
#if DEBUG
			std::wstring mesg = XError::Instance()->GetText(err) + L" with file " + spec.GetName();
#else
			std::wstring mesg = LoadWhisperString(L"File error ##1 (with file '#2')", Int32ToStr(err), spec.GetName());
#endif
			throw std::runtime_error(ToUTF8Str(mesg));				
			}
			break;
			
		default:
			ThrowErr(err);
	}
}		 


//---------------------------------------------------------------
//
// ThrowIfFileErr
//
//---------------------------------------------------------------
inline void ThrowIfFileErr(const XFileSpec& spec, OSStatus err)		
{
	if (err != noErr) 
		ThrowFileErr(spec, err);
}	

#pragma mark -

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
		try {
			if (mWriting && mData != nil)
				this->Flush();
				
			(void) FSClose(mRefNum);	// if so, we need to close the file (and attempt to save any changes)

		} catch (...) {
			// can't throw from dtor
		}
	}

	if (mData != nil)
		DisposeHandle(mData);
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::XMemoryMappedFile (XFileSpec)
//
//---------------------------------------------------------------
XMemoryMappedFile::XMemoryMappedFile(const XFileSpec& file) : mSpec(file)
{
	mRefNum = kNoFileRefNum;
	
	mReading = false;
	mWriting = false;
	
	mData = nil;
	mMaxBytes = 0;
	mPurgable = true;

	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::XMemoryMappedFile (XFolderSpec, wstring)
//
//---------------------------------------------------------------
XMemoryMappedFile::XMemoryMappedFile(const XFolderSpec& folder, const std::wstring& name) : mSpec(folder, name)
{
	mRefNum = kNoFileRefNum;
	
	mReading = false;
	mWriting = false;
	
	mData = nil;
	mMaxBytes = 0;
	mPurgable = true;

	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::Open (EFilePermission, uint32)
//
//---------------------------------------------------------------
void XMemoryMappedFile::Open(EFilePermission perm, uint32 maxBytes)
{
	PRECONDITION(!this->IsOpened());
	PRECONDITION(mData == nil);
	CHECK_INVARIANT;
	
	int16 tempRef = kNoFileRefNum;
	mMaxBytes = maxBytes;
	
	// Open the file
	OSErr err = FSpOpenDF(&(mSpec.GetOSSpec()), perm, &tempRef);				
	ThrowIfFileErr(mSpec, err);
	
	// Initialize maxBytes if the user hasn't specified a value
	if (mMaxBytes == 0) {
		int32 bytes;
		err = GetEOF(tempRef, &bytes);
		ThrowIfFileErr(mSpec, err);
		
		mMaxBytes = numeric_cast<uint32>(bytes);
	}
	
	// Set some member variables
	mRefNum = tempRef;						
	mReading = perm == kReadPermission  || perm == kReadWritePermission;
	mWriting = perm == kWritePermission || perm == kReadWritePermission;

	POSTCONDITION(this->IsOpened());
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::Open (OSType, OSType, EFilePermission, uint32)
//
//---------------------------------------------------------------
void XMemoryMappedFile::Open(OSType creator, OSType fileType, EFilePermission perm, uint32 maxBytes)
{
	PRECONDITION(!this->IsOpened());
	PRECONDITION(mData == nil);
	CHECK_INVARIANT;
	
	int16 tempRef = kNoFileRefNum;
	mMaxBytes = maxBytes;
	
	// Open the file
	OSErr err = FSpOpenDF(&(mSpec.GetOSSpec()), perm, &tempRef);				
	if (err == fnfErr) {
		ASSERT(perm != kReadPermission);
		
		err = FSpCreate(&(mSpec.GetOSSpec()), creator, fileType, smSystemScript);
		ThrowIfFileErr(mSpec, err);
		
		err = FSpOpenDF(&(mSpec.GetOSSpec()), perm, &tempRef);				
	}	
	ThrowIfFileErr(mSpec, err);
	
	// Initialize maxBytes if the user hasn't specified a value
	if (mMaxBytes == 0) {
		int32 bytes;
		err = GetEOF(tempRef, &bytes);
		ThrowIfFileErr(mSpec, err);
		
		mMaxBytes = numeric_cast<uint32>(bytes);
	}
	
	// Set some member variables
	mRefNum = tempRef;						
	mReading = perm == kReadPermission  || perm == kReadWritePermission;
	mWriting = perm == kWritePermission || perm == kReadWritePermission;

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

	if (mRefNum != kNoFileRefNum) {			// TN 1089 says that closing a file twice can corrupt the file system so we'll be extra careful
		try {
			if (mWriting && mData != nil)
				this->Flush();
				
			OSErr err = FSClose(mRefNum);
			ThrowIfFileErr(mSpec, err);		// File Manager buffers data and flushes on close so an error is quite possible
			
			if (mData != nil) {
				DisposeHandle(mData);
				mData = nil;
			}
			
			mRefNum = kNoFileRefNum;		
			mReading = false;
			mWriting = false;

		} catch (...) {
			if (mData != nil) {
				DisposeHandle(mData);
				mData = nil;
			}
			
			mRefNum = kNoFileRefNum;		

			throw;
		}
	}

	POSTCONDITION(!this->IsOpened());
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::Close (uint32)
//
//---------------------------------------------------------------
void XMemoryMappedFile::Close(uint32 newSize)
{
	PRECONDITION(mWriting);
	CHECK_INVARIANT;

	if (mData != nil && newSize != mMaxBytes) {			// mData will only be nil if the file was never locked
		ASSERT(*mData != nil);
		ASSERT(newSize <= GetHandleSize(mData) - kTailSize);
		
		SetHandleSize(mData, newSize + kTailSize);
		ThrowIfMemError();
		
		mMaxBytes = newSize;

#if DEBUG
		uint32* tail = reinterpret_cast<uint32*>(*mData + mMaxBytes);
		*tail = kTail;
#endif
	}
				
	this->Close();

	POSTCONDITION(!this->IsOpened());
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::IsOpened
//
//---------------------------------------------------------------
bool XMemoryMappedFile::IsOpened() const
{
	bool open = mRefNum != kNoFileRefNum;
	
	return open;
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::GetBuffer
//
//---------------------------------------------------------------
uint8* XMemoryMappedFile::GetBuffer()
{
	PRECONDITION(this->IsLocked());
	PRECONDITION(mData != nil);
	PRECONDITION(*mData != nil);
	
	return (uint8*) *mData;
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

	if (mData != nil) {					// will only be nil if the file hasn't been locked yet
		ASSERT(*mData != nil);			// writable files aren't purgeable
					
		OSErr err = SetFPos(mRefNum, fsFromStart, 0);
		ThrowIfFileErr(mSpec, err);
	
		int32 bytes = numeric_cast<int32>(GetHandleSize(mData) - kTailSize);
	
		{
		XLocker lock(this);
			err = FSWrite(mRefNum, &bytes, *mData);
			ThrowIfFileErr(mSpec, err);
		}
		
		err = SetEOF(mRefNum, bytes);
		ThrowIfFileErr(mSpec, err);
	}
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::EnablePurging
//
//---------------------------------------------------------------
void XMemoryMappedFile::EnablePurging(bool enable)	
{	
	ASSERT_IF(enable, !mWriting);
	CHECK_INVARIANT;
	
	if (enable != mPurgable) {
		mPurgable = enable;
		
		if (mData != nil && *mData != nil) {
			if (mPurgable)
				HPurge(mData);			// this will have no effect while the handle is locked
			else
				HNoPurge(mData);
			ThrowIfMemError();
		}
	}

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
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::OnLock
//
//---------------------------------------------------------------
void XMemoryMappedFile::OnLock(bool moveHigh)
{
	PRECONDITION(this->IsOpened());

	bool dirty = false;
	OSErr err = noErr;
	
	// Allocate (or re-allocate) the handle
	if (mData == nil) {
		mData = TempNewHandle(numeric_cast<int32>(mMaxBytes + kTailSize), &err);
		if (mData == nil) 
			mData = NewHandle(mMaxBytes + kTailSize);
		ThrowIfMemFail(mData);
			
		dirty = true;
		
	} else if (*mData == nil) {
		ReallocateHandle(mData, numeric_cast<int32>(mMaxBytes + kTailSize));
		ThrowIfMemError();
			
		HNoPurge(mData);
		ThrowIfMemError();

		dirty = true;
	}
	
	// Lock it
	if (moveHigh)
		HLockHi(mData);
	else
		HLock(mData);
	ThrowIfMemError();
	
	// And read in the file as necessary
	if (dirty && mReading) {
#if DEBUG
		uint32* tail = reinterpret_cast<uint32*>(*mData + mMaxBytes);
		*tail = kTail;
#endif

		int32 bytes;
		err = GetEOF(mRefNum, &bytes);
		ThrowIfFileErr(mSpec, err);

		bytes = Min(bytes, (int32) mMaxBytes);
		
		err = FSRead(mRefNum, &bytes, *mData);
		ThrowIfFileErr(mSpec, err);
	}
}


//---------------------------------------------------------------
//
// XMemoryMappedFile::OnUnlock
//
//---------------------------------------------------------------
void XMemoryMappedFile::OnUnlock()
{
	PRECONDITION(mData != nil);
	PRECONDITION(*mData != nil);		
	
#if DEBUG	
	uint32* tail = reinterpret_cast<uint32*>(*mData + mMaxBytes);
	ASSERT(*tail == kTail);
#endif

	HUnlock(mData);
	ThrowIfMemError();
	
	if (mReading && !mWriting && mPurgable) {	
		HPurge(mData);
		ThrowIfMemError();
	}
}


}	// namespace Whisper
