/*
 *  File:		XHandleStream.cpp
 *  Summary:	Stream classes that work with XHandle's.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XHandleStream.cpp,v $
 *		Revision 1.3  2001/04/27 04:33:05  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:37:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XHandleStream.h>

#include <XExceptions.h>
#include <XNumbers.h>
#include <XStringUtils.h>

namespace Whisper {


// ========================================================================================
//	class XInHandleStream
// ========================================================================================

//---------------------------------------------------------------
//
// XInHandleStream::~XInHandleStream
//
//---------------------------------------------------------------
XInHandleStream::~XInHandleStream()
{
}


//---------------------------------------------------------------
//
// XInHandleStream::XInHandleStream
//
//---------------------------------------------------------------
XInHandleStream::XInHandleStream(const XHandle& hand, uint32 startPos, bool raw) : XInStream(raw), mHandle(hand)
{
	PRECONDITION(startPos < mHandle.GetSize() || (startPos == 0 && mHandle.GetSize() == 0));

	mPos = startPos;
}


//---------------------------------------------------------------
//
// XInHandleStream::GetLength
//
//---------------------------------------------------------------
uint32 XInHandleStream::GetLength() const
{
	return mHandle.GetSize();
}


//---------------------------------------------------------------
//
// XInHandleStream::GetPosition
//
//---------------------------------------------------------------
uint32 XInHandleStream::GetPosition() const
{
	return mPos;
}


//---------------------------------------------------------------
//
// XInHandleStream::SetPosition
//
//---------------------------------------------------------------
void XInHandleStream::SetPosition(uint32 newPosition)
{
	PRECONDITION(newPosition <= mHandle.GetSize());

	mPos = newPosition;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XInHandleStream::OnReadBytes
//
//---------------------------------------------------------------
void XInHandleStream::OnReadBytes(void* dst, uint32 bytes)
{
	PRECONDITION(dst != nil);
	
	uint32 size = mHandle.GetSize();
	if (mPos + bytes <= size) {
		BlockMoveData(mHandle.GetUnsafePtr() + mPos, dst, bytes);

		mPos += bytes;

	} else
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: XInHandleStream::ReadBytes went past eof.")));
}

#if __MWERKS__
#pragma mark -
#endif

// ========================================================================================
//	class XOutHandleStream
// ========================================================================================

//---------------------------------------------------------------
//
// XOutHandleStream::~XOutHandleStream
//
//---------------------------------------------------------------
XOutHandleStream::~XOutHandleStream()
{
	mHandle.SetSize(mPos);					// if we were given a handle we need to make sure it's the correct size when we're done
}


//---------------------------------------------------------------
//
// XOutHandleStream::XOutHandleStream (uint32, bool)
//
//---------------------------------------------------------------
XOutHandleStream::XOutHandleStream(uint32 reserve, bool raw) : XOutStream(raw), mHandle(reserve)
{
	mPos = 0;
}


//---------------------------------------------------------------
//
// XOutHandleStream::XOutHandleStream (XHandle, uint32, bool)
//
//---------------------------------------------------------------
XOutHandleStream::XOutHandleStream(const XHandle& hand, uint32 startPos, bool raw) : XOutStream(raw), mHandle(hand)
{
	PRECONDITION(startPos <= hand.GetSize());

	mPos = startPos;
}


//---------------------------------------------------------------
//
// XOutHandleStream::GetHandle
//
//---------------------------------------------------------------
XHandle XOutHandleStream::GetHandle() const
{
	XHandle result = mHandle;

	result.SetSize(mPos);

	return result;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XOutHandleStream::OnWriteBytes
//
//---------------------------------------------------------------
void XOutHandleStream::OnWriteBytes(const void* src, uint32 bytes)
{
	if (mPos + bytes > mHandle.GetSize())
		this->DoExpandHandle(mPos + bytes);

	BlockMoveData(src, mHandle.GetUnsafePtr() + mPos, bytes);

	mPos += bytes;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XOutHandleStream::DoExpandHandle
//
//---------------------------------------------------------------
void XOutHandleStream::DoExpandHandle(uint32 requiredSize)
{
	try {
		uint32 size = Max(2*mHandle.GetSize(), requiredSize);
		mHandle.SetSize(size);

	} catch (...) {
		mHandle.SetSize(requiredSize);
	}
}


}	// namespace Whisper
