/*
 *  File:		XPointer.cpp
 *  Summary:	A class that encapsulates non-relocatable memory.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XPointer.cpp,v $
 *		Revision 1.4  2001/04/27 04:35:14  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/04/17 01:44:38  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.2  2000/11/09 12:47:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XPointer.h>

#include <XDebug.h>
#include <XDebugMalloc.h>
#include <XDebugNew.h>
#include <XInvariant.h>
#include <XMemUtils.h>
#include <XReferenceCounted.h>

namespace Whisper {


// ===================================================================================
//	class ZPointerRef
// ===================================================================================
class ZPointerRef : public XReferenceCountedMixin {

	typedef XReferenceCountedMixin Inherited;

public:
	virtual				~ZPointerRef();

						ZPointerRef();
						// Creates a nil pointer.

						ZPointerRef(uint32 bytes, bool zeroBytes);
						
			const uint8* GetPtr() const;

			uint8* 		GetPtr();

			uint32 		GetSize() const;

			void 		SetSize(uint32 bytes, bool zeroBytes);
	
protected:
			void 		Invariant() const;

private:
						ZPointerRef(const ZPointerRef& ref);
						
			ZPointerRef& operator=(const ZPointerRef& ref);

private:	
	uint8*	mPtr;
	uint32	mSize;
};


//---------------------------------------------------------------
//
// ZPointerRef::~ZPointerRef
//
//---------------------------------------------------------------
ZPointerRef::~ZPointerRef()
{
	CALL_INVARIANT;

	delete [] mPtr;
}


//---------------------------------------------------------------
//
// ZPointerRef::ZPointerRef ()
//
//---------------------------------------------------------------
ZPointerRef::ZPointerRef()
{
	mPtr = nil;
	mSize = 0;

	CALL_INVARIANT;
}

	
//---------------------------------------------------------------
//
// ZPointerRef::ZPointerRef (uint32, bool)
//
//---------------------------------------------------------------
ZPointerRef::ZPointerRef(uint32 bytes, bool zeroBytes)
{		
	mPtr = nil;
	mSize = bytes;
	
	mPtr = new uint8[mSize];
	if (zeroBytes)
		ClearMemory(mPtr, mSize);

	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// ZPointerRef::GetPtr const
//
//---------------------------------------------------------------
inline const uint8* ZPointerRef::GetPtr() const
{
	PRECONDITION(mPtr != nil);
	CHECK_INVARIANT;
	
	return (uint8 *) mPtr;
}


//---------------------------------------------------------------
//
// ZPointerRef::GetPtr
//
//---------------------------------------------------------------
inline uint8* ZPointerRef::GetPtr()
{
	PRECONDITION(mPtr != nil);
	CHECK_INVARIANT;
	
	return (uint8 *) mPtr;
}


//---------------------------------------------------------------
//
// ZPointerRef::GetSize
//
//---------------------------------------------------------------
inline uint32 ZPointerRef::GetSize() const
{
	return mSize;
}


//---------------------------------------------------------------
//
// ZPointerRef::SetSize
//
//---------------------------------------------------------------
void ZPointerRef::SetSize(uint32 bytes, bool zeroBytes)
{
	CHECK_INVARIANT;
	
	if (bytes != mSize) {
		uint8* newPtr = new uint8[bytes];			// Don't need a try block because this is the only thing that can fail.
		
		uint32 validBytes = std::min(bytes, mSize);
		BlockMoveData(mPtr, newPtr, validBytes);

		delete [] mPtr;
		mPtr = newPtr;
		
		if (bytes > mSize && zeroBytes)
			ClearMemory(mPtr + mSize, bytes - mSize);
		
		mSize = bytes;
	}
}


//---------------------------------------------------------------
//
// ZPointerRef::Invariant
//
//---------------------------------------------------------------
void ZPointerRef::Invariant() const
{	
	if (mSize > 0) 
		ASSERT(mPtr != nil);
		
#if WHISPER_OPERATOR_NEW && DEBUG && !MULTI_FRAGMENT_APP
	if (mPtr != nil) 
		gDebugMalloc->ValidateBlock(mPtr);
#endif
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XPointer
// ===================================================================================

//---------------------------------------------------------------
//
// XPointer::~XPointer
//
//---------------------------------------------------------------
XPointer::~XPointer()
{
	mPointerRef->RemoveReference();
	mPointerRef = nil;
}
	

//---------------------------------------------------------------
//
// XPointer::XPointer (uint32, bool)
//
//---------------------------------------------------------------
XPointer::XPointer(uint32 bytes, bool zeroBytes)
{
	mPointerRef = new ZPointerRef(bytes, zeroBytes);
}


//---------------------------------------------------------------
//
// XPointer::XPointer (XPointer)
//
//---------------------------------------------------------------
XPointer::XPointer(const XPointer& rhs)
{
	mPointerRef = rhs.mPointerRef;
	mPointerRef->AddReference();
}


//---------------------------------------------------------------
//
// XPointer::operator=
//
//---------------------------------------------------------------
XPointer& XPointer::operator=(const XPointer& rhs)
{
	if (mPointerRef != rhs.mPointerRef) {
		mPointerRef->RemoveReference();
			
		mPointerRef = rhs.mPointerRef;
		mPointerRef->AddReference();
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XPointer::Detach
//
//---------------------------------------------------------------
void XPointer::Detach()
{
	if (mPointerRef->GetRefCount() > 1) {
		uint32 bytes = mPointerRef->GetSize();

		ZPointerRef* oldRef = mPointerRef;
		ZPointerRef* newRef = new ZPointerRef(bytes, kDontZeroBytes);

		BlockMoveData(oldRef->GetPtr(), newRef->GetPtr(), bytes);

		mPointerRef->RemoveReference();
		mPointerRef = newRef;
	}
}


//---------------------------------------------------------------
//
// XPointer::GetPtr const
//
//---------------------------------------------------------------
const uint8* XPointer::GetPtr() const
{
	const uint8* ptr = mPointerRef->GetPtr();

	return ptr;
}


//---------------------------------------------------------------
//
// XPointer::GetPtr
//
//---------------------------------------------------------------
uint8* XPointer::GetPtr()
{
	uint8* ptr = mPointerRef->GetPtr();

	return ptr;
}


//---------------------------------------------------------------
//
// XPointer::GetSize
//
//---------------------------------------------------------------
uint32 XPointer::GetSize() const
{
	uint32 size = mPointerRef->GetSize();
	
	return size;
}


//---------------------------------------------------------------
//
// XPointer::SetSize
//
//---------------------------------------------------------------
void XPointer::SetSize(uint32 bytes, bool zeroBytes)
{
	mPointerRef->SetSize(bytes, zeroBytes);
}


//---------------------------------------------------------------
//
// XPointer::operator==
//
//---------------------------------------------------------------
bool XPointer::operator==(const XPointer& rhs) const
{
	bool equal = mPointerRef == rhs.mPointerRef;
	
	if (!equal)
		if (this->GetSize() == rhs.GetSize())
			equal = EqualMemory(this->GetPtr(), rhs.GetPtr(), this->GetSize());
	
	return equal;
}


}	// namespace Whisper
