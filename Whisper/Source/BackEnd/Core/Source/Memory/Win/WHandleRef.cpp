/*
 *  File:		WHandleRef.cpp
 *  Summary:	Letter used by XHandle.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: WHandleRef.cpp,v $
 *		Revision 1.4  2001/04/21 03:32:13  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/17 01:42:04  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.2  2000/11/09 12:08:20  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <ZHandleRef.h>

#include <XDebugMalloc.h>
#include <XDebugNew.h>
#include <XExceptions.h>
#include <XMemUtils.h>

#if WIN
	#include <WSystemInfo.h>
#endif

namespace Whisper {


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
	CALL_INVARIANT;

	delete [] mHandle;
}


//---------------------------------------------------------------
//
// ZHandleRef::ZHandleRef (uint32, uint32)
// 
//---------------------------------------------------------------
ZHandleRef::ZHandleRef(uint32 bytes, uint32 options) 
{
	PRECONDITION(bytes < 16*1024L*1024L);
	PRECONDITION(options <= kZeroBytes + kUseTempHeap);
		
	bool zeroBytes = (options & kZeroBytes) == kZeroBytes;
	
	mBytes = bytes;
		
	mHandle = new uint8[mBytes];

	if (zeroBytes)
		SetMemory(mHandle, 0, mBytes);	
	 
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// ZHandleRef::SetSize
//
//---------------------------------------------------------------
void ZHandleRef::SetSize(uint32 bytes, bool zeroBytes)
{
	PRECONDITION(!this->IsLocked());
	CHECK_INVARIANT;
	
	if (bytes != mBytes) {
		uint8* newHandle = new uint8[bytes];
		
		uint32 validBytes = std::min(bytes, mBytes);
		BlockMoveData(mHandle, newHandle, validBytes);

		delete [] mHandle;
		mHandle = newHandle;

		if (bytes > mBytes && zeroBytes) 
			ClearMemory(mHandle + mBytes, bytes - mBytes);
		
		mBytes = bytes;
	}

	POSTCONDITION(true);
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// ZHandleRef::Invariant
//
//---------------------------------------------------------------
void ZHandleRef::Invariant() const
{	
	ASSERT(mHandle != nil);
		
#if WHISPER_OPERATOR_NEW && DEBUG
	gDebugMalloc->ValidateBlock(mHandle);
#endif
}


//---------------------------------------------------------------
//
// ZHandleRef::OnLock
//
//---------------------------------------------------------------
void ZHandleRef::OnLock(bool moveHigh)
{
	UNUSED(moveHigh);
}


//---------------------------------------------------------------
//
// ZHandleRef::OnUnlock
//
//---------------------------------------------------------------
void ZHandleRef::OnUnlock()
{
}


}	// namespace Whisper
