/*
 *  File:		MHandle.cpp
 *  Summary:	A class that encapsulates relocatable memory.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XHandle.cpp,v $
 *		Revision 1.3  2001/04/27 04:33:02  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:37:50  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XHandle.h>

#include <XDebug.h>
#include <XExceptions.h>
#include <XMemUtils.h>
#include <XReferenceCounted.h>

namespace Whisper {


// ===================================================================================
//	class XHandle
// ===================================================================================

//---------------------------------------------------------------
//
// XHandle::~XHandle
//
//---------------------------------------------------------------
XHandle::~XHandle()
{
	mHandleRef->RemoveReference();
	mHandleRef = nil;
}


//---------------------------------------------------------------
//
// XHandle::XHandle (uint32, uint32)
//
//---------------------------------------------------------------
XHandle::XHandle(uint32 bytes, uint32 options)
{
	mHandleRef = new ZHandleRef(bytes, options);
}


//---------------------------------------------------------------
//
// XHandle::XHandle (Handle)
//
//---------------------------------------------------------------
#if MAC
XHandle::XHandle(Handle takeHandle)
{
	PRECONDITION(takeHandle != nil);

	try {
		mHandleRef = new ZHandleRef(takeHandle);
		
	} catch (...) {
		if (IsResource(takeHandle))
			ReleaseResource(takeHandle);
		else
			DisposeHandle(takeHandle);
			
		throw;
	}
}
#endif


//---------------------------------------------------------------
//
// XHandle::XHandle (XHandle)
//
//---------------------------------------------------------------
XHandle::XHandle(const XHandle& hand)
{
	mHandleRef = hand.mHandleRef;
	mHandleRef->AddReference();
}


//---------------------------------------------------------------
//
// XHandle::operator=
//
//---------------------------------------------------------------
XHandle& XHandle::operator=(const XHandle& hand)
{
	PRECONDITION(!this->IsLocked());
	
	if (mHandleRef != hand.mHandleRef) {
		mHandleRef->RemoveReference();

		hand.mHandleRef->AddReference();
		mHandleRef = hand.mHandleRef;
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XHandle::Detach
//
//---------------------------------------------------------------
void XHandle::Detach()
{
	PRECONDITION(!this->IsLocked());
	
	if (mHandleRef->GetRefCount() > 1) {
#if MAC
		Handle data = mHandleRef->GetHandle();
		
		OSErr err = HandToHand(&data);
		ThrowIfOSErr(err);
	
		try {
			ZHandleRef* newRef = new ZHandleRef(data);
	
			mHandleRef->RemoveReference();
			mHandleRef = newRef;
			
		} catch(...) {
			if (data != nil)
				DisposeHandle(data);
			throw;
		}
		
#elif WIN		
		uint32 bytes = mHandleRef->GetSize();

		ZHandleRef* newRef = new ZHandleRef(bytes, 0);
		BlockMoveData(mHandleRef->GetPtr(), newRef->GetPtr(), bytes);

		mHandleRef->RemoveReference();
		mHandleRef = newRef;
#endif
	}
}


//---------------------------------------------------------------
//
// XHandle::Lock
//
//---------------------------------------------------------------
void XHandle::Lock(bool moveHigh)
{
	mHandleRef->Lock(moveHigh);
}


//---------------------------------------------------------------
//
// XHandle::Unlock
//
//---------------------------------------------------------------
void XHandle::Unlock()
{
	mHandleRef->Unlock();
}


//---------------------------------------------------------------
//
// XHandle::IsLocked
//
//---------------------------------------------------------------
bool XHandle::IsLocked() const
{
	return mHandleRef->IsLocked();
}
	

//---------------------------------------------------------------
//
// XHandle::operator==
//
//---------------------------------------------------------------
bool XHandle::operator==(const XHandle& rhs) const
{
	bool equal = mHandleRef == rhs.mHandleRef;
	
	if (!equal)
		if (this->GetSize() == rhs.GetSize())
			equal = EqualMemory(this->GetUnsafePtr(), rhs.GetUnsafePtr(), this->GetSize());
	
	return equal;
}


//---------------------------------------------------------------
//
// XHandle::ResetTail
//
//---------------------------------------------------------------
#if MAC && DEBUG
void XHandle::ResetTail()
{
	mHandleRef->ResetTail();
}
#endif


}	// namespace Whisper
