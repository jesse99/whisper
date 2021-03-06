/*
 *  File:		MHandleRef.cpp
 *  Summary:	Letter used by XHandle.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: MHandleRef.cpp,v $
 *		Revision 1.5  2001/04/21 03:26:36  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/04/17 01:41:05  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.3  2001/03/08 08:34:42  jesjones
 *		Tweaked a comment.
 *		
 *		Revision 1.2  2000/11/09 09:14:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <ZHandleRef.h>

#include <Errors.h>
#include <Resources.h>

#include <XExceptions.h>
#include <XMemUtils.h>

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
//	class ZHandleRef
// ===================================================================================

//---------------------------------------------------------------
//
// ZHandleRef::~ZHandleRef
//
//---------------------------------------------------------------
ZHandleRef::~ZHandleRef()
{
#if DEBUG
	CALL_INVARIANT;
	
	Whisper::SetMemory(*mHandle, 0xF3, mBytes);	
#endif

	::DisposeHandle(mHandle);
}


//---------------------------------------------------------------
//
// ZHandleRef::ZHandleRef (uint32, uint32)
// 
//---------------------------------------------------------------
ZHandleRef::ZHandleRef(uint32 bytes, uint32 options) 
{
	PRECONDITION(options <= kZeroBytes + kUseTempHeap);
		
	bool zeroBytes = (options & kZeroBytes)   == kZeroBytes;
	bool tempMem   = (options & kUseTempHeap) == kUseTempHeap;
	bool appHeap   = (options & kUseAppHeap)  == kUseAppHeap;
	
	mBytes = bytes;
		
	if (tempMem) {
		OSErr err;
		mHandle = TempNewHandle((int32) (mBytes + kTailSize), &err);

	} else
		mHandle = NewHandle(mBytes + kTailSize);

	ThrowIfMemFail(mHandle);

	if (zeroBytes)
		Whisper::SetMemory(*mHandle, 0, mBytes);	
#if DEBUG
	else
		Whisper::SetMemory(*mHandle, 0xD3, mBytes);	
		
	uint32* tail = reinterpret_cast<uint32*>(*mHandle + mBytes);
	*tail = kTail;
	
	mHasTail = true;
#endif
	
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// ZHandleRef::ZHandleRef (Handle)
//
//---------------------------------------------------------------
ZHandleRef::ZHandleRef(Handle takeHandle)
{
#if DEBUG
	Whisper::ValidateHandle(takeHandle);
#endif

	mHandle = takeHandle;
	
	if (IsResource(mHandle)) {
		if (*mHandle == nil) {
			LoadResource(mHandle);
			ThrowIfResError();
		}

		HNoPurge(mHandle);
		DetachResource(mHandle);
		ThrowIfResError();
	}
	
	mBytes = (uint32) GetHandleSize(mHandle);

#if DEBUG
	mHasTail = false;
#endif
	
	if (Whisper::IsLocked(mHandle))
		mLockCount = 1;
	
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// ZHandleRef::SetSize
//
//---------------------------------------------------------------
void ZHandleRef::SetSize(uint32 bytes, bool zeroBytes)
{
	PRECONDITION(!this->IsLocked());	// we don't allow shrinking locked handles to be more compatible with Windows
	CHECK_INVARIANT;
	
	OSErr err = noErr;

	if (bytes != mBytes) {
		SetHandleSize(mHandle, bytes + kTailSize);
		ThrowIfMemError();

		if (bytes > mBytes) {
			if (zeroBytes)
				Whisper::ClearMemory(*mHandle + mBytes, bytes - mBytes);
#if DEBUG
			else
				Whisper::SetMemory(*mHandle + mBytes, 0xD3, bytes - mBytes);	
#endif
		}
		
		mBytes = bytes;
		
#if DEBUG
		uint32* tail = reinterpret_cast<uint32*>(*mHandle + mBytes);
		*tail = kTail;

		mHasTail = true;
#endif
	}

	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// ZHandleRef::ResetTail
//
//---------------------------------------------------------------
#if DEBUG
void ZHandleRef::ResetTail()
{
	uint32 bytes = ::GetHandleSize(mHandle) - kTailSize;
	
	if (bytes != mBytes) {
		uint32 needed = bytes + kTailSize;

		SetHandleSize(mHandle, needed);
		ThrowIfMemError();
		
		mBytes = bytes;
		
		int32* tail = reinterpret_cast<int32*>(this->GetPtr() + mBytes);
		*tail = kTail;
	}
}
#endif

#pragma mark ~

//---------------------------------------------------------------
//
// ZHandleRef::Invariant
//
//---------------------------------------------------------------
void ZHandleRef::Invariant() const
{
#if DEBUG			
	Whisper::ValidateHandle(mHandle);
	ASSERT(*mHandle != nil);

	if (Whisper::IsLocked(mHandle))
		ASSERT(this->IsLocked());

	if (this->IsLocked())
		ASSERT(Whisper::IsLocked(mHandle));

	int32 bytes = GetHandleSize(mHandle);
	
	if (mHasTail) {
		ASSERT(bytes == mBytes + kTailSize);

		uint32* tail = reinterpret_cast<uint32*>(*mHandle + mBytes);
		ASSERT(*tail == kTail);
		
	} else
		ASSERT(bytes == mBytes);
#endif
}


//---------------------------------------------------------------
//
// ZHandleRef::OnLock
//
//---------------------------------------------------------------
void ZHandleRef::OnLock(bool moveHigh)
{
	if (moveHigh)
		MoveHHi(mHandle);
		
	HLock(mHandle);
}


//---------------------------------------------------------------
//
// ZHandleRef::OnUnlock
//
//---------------------------------------------------------------
void ZHandleRef::OnUnlock()
{
	HUnlock(mHandle);
}


}	// namespace Whisper
