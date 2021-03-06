/*
 *  File:		XFileStream.cpp
 *  Summary:	Stream classes that work with files.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFileStream.cpp,v $
 *		Revision 1.3  2001/04/27 04:32:39  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:35:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XFileStream.h>

#include <XFile.h>
#include <XNumbers.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const uint32 kMaxBufferSize = 32*1024UL;


// ========================================================================================
//	class XInFileStream
// ========================================================================================

//---------------------------------------------------------------
//
// XInFileStream::~XInFileStream
//
//---------------------------------------------------------------
XInFileStream::~XInFileStream()
{
}


//---------------------------------------------------------------
//
// XInFileStream::XInFileStream
//
//---------------------------------------------------------------
XInFileStream::XInFileStream(XFile& file, bool raw) : XInStream(raw), mFile(file)
{
	PRECONDITION(mFile.IsOpened());
}


//---------------------------------------------------------------
//
// XInFileStream::GetLength
//
//---------------------------------------------------------------
uint32 XInFileStream::GetLength() const
{
	return mFile.GetLength();		
}


//---------------------------------------------------------------
//
// XInFileStream::GetPosition
//
//---------------------------------------------------------------
uint32 XInFileStream::GetPosition() const
{
	return mFile.GetPosition();
}


//---------------------------------------------------------------
//
// XInFileStream::SetPosition
//
//---------------------------------------------------------------
void XInFileStream::SetPosition(uint32 newPosition)
{
	PRECONDITION(newPosition <= mFile.GetLength());

	mFile.Seek(kSeekFromStart, (int32) newPosition);
}


//---------------------------------------------------------------
//
// XInFileStream::OnReadBytes
//
//---------------------------------------------------------------
void XInFileStream::OnReadBytes(void* dst, uint32 bytes)
{
	PRECONDITION(dst != nil);
	
	mFile.Read(dst, bytes);
}

#if __MWERKS__
#pragma mark -
#endif

// ========================================================================================
//	class XOutFileStream
// ========================================================================================

//---------------------------------------------------------------
//
// XOutFileStream::~XOutFileStream
//
//---------------------------------------------------------------
XOutFileStream::~XOutFileStream()
{
}


//---------------------------------------------------------------
//
// XOutFileStream::XOutFileStream
//
//---------------------------------------------------------------
XOutFileStream::XOutFileStream(XFile& file, bool raw) : XOutStream(raw), mFile(file)
{
	PRECONDITION(mFile.IsOpened());
}


//---------------------------------------------------------------
//
// XOutFileStream::OnWriteBytes
//
//---------------------------------------------------------------
void XOutFileStream::OnWriteBytes(const void* src, uint32 bytes)
{
	PRECONDITION(src != nil);
	
	mFile.Write(src, bytes);
}


}	// namespace Whisper
