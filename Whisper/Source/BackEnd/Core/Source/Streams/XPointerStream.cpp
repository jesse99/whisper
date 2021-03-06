/*
 *  File:		XPointerStream.cpp
 *  Summary:	Stream classes that work with raw memory pointers.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XPointerStream.cpp,v $
 *		Revision 1.3  2001/04/27 04:35:17  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:47:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XPointerStream.h>

#include <XExceptions.h>
#include <XNumbers.h>
#include <XStringUtils.h>

namespace Whisper {


// ========================================================================================
//	class XInPointerStream
// ========================================================================================

//---------------------------------------------------------------
//
// XInPointerStream::~XInPointerStream
//
//---------------------------------------------------------------
XInPointerStream::~XInPointerStream()
{
}


//---------------------------------------------------------------
//
// XInPointerStream::XInPointerStream
//
//---------------------------------------------------------------
XInPointerStream::XInPointerStream(const void* ptr, uint32 bufferSize, bool raw) : XInStream(raw)
{
	PRECONDITION(ptr != nil);

	mPtr = reinterpret_cast<const uint8*>(ptr);
	mSize = bufferSize;
	mPos = 0;
}


//---------------------------------------------------------------
//
// XInPointerStream::GetLength
//
//---------------------------------------------------------------
uint32 XInPointerStream::GetLength() const
{
	return mSize;
}


//---------------------------------------------------------------
//
// XInPointerStream::GetPosition
//
//---------------------------------------------------------------
uint32 XInPointerStream::GetPosition() const
{
	return mPos;
}


//---------------------------------------------------------------
//
// XInPointerStream::SetPosition
//
//---------------------------------------------------------------
void XInPointerStream::SetPosition(uint32 newPosition)
{
	PRECONDITION(newPosition <= mSize);

	mPos = newPosition;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XInPointerStream::OnReadBytes
//
//---------------------------------------------------------------
void XInPointerStream::OnReadBytes(void* dst, uint32 bytes)
{
	PRECONDITION(dst != nil);
	
	if (mPos + bytes <= mSize) {
		BlockMoveData(mPtr + mPos, dst, bytes);

		mPos += bytes;

	} else
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: XInPointerStream::ReadBytes went past eof.")));
}

#if __MWERKS__
#pragma mark -
#endif

// ========================================================================================
//	class XOutPointerStream
// ========================================================================================

//---------------------------------------------------------------
//
// XOutPointerStream::~XOutPointerStream
//
//---------------------------------------------------------------
XOutPointerStream::~XOutPointerStream()
{
}


//---------------------------------------------------------------
//
// XOutPointerStream::XOutPointerStream 
//
//---------------------------------------------------------------
XOutPointerStream::XOutPointerStream(void* ptr, uint32 bufferSize, bool raw) : XOutStream(raw)
{
	PRECONDITION(ptr != nil);

	mPtr = reinterpret_cast<uint8*>(ptr);
	mSize = bufferSize;
	mPos = 0;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XOutPointerStream::OnWriteBytes
//
//---------------------------------------------------------------
void XOutPointerStream::OnWriteBytes(const void* src, uint32 bytes)
{
	if (mPos + bytes > mSize)
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: XOutPointerStream::OnWriteBytes exceeded buffer space.")));

	BlockMoveData(src, mPtr + mPos, bytes);

	mPos += bytes;
}


}	// namespace Whisper
