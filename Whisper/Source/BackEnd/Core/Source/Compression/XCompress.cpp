/*
 *  File:       XCompress.cpp
 *  Summary:   	C++ wrapper around PNG's gzip compression engine.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XCompress.cpp,v $
 *		Revision 1.5  2001/04/27 04:31:18  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/03/15 09:08:24  jesjones
 *		Added a comment.
 *		
 *		Revision 1.3  2000/12/10 04:09:25  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:28:33  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XCompress.h>
 
#include <string.h>
#include <zlib.h>		

#include <XLocker.h>
#include <XNumbers.h>
#include <XStringUtils.h>

namespace Whisper {

// $$$ When building DLL targets on Windows "zlib Intel.Lib" is included in both the Core
// $$$ and Graphics projects. This causes about 70K of extra bloat. I spent some time
// $$$ trying to export these functions from the Core project without much luck...

// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// ZipAllocate
//
//---------------------------------------------------------------
void* ZipAllocate(void* refCon, unsigned int numItems, unsigned int size)
{
	UNUSED(refCon);
	
	uint32 bytes = numItems*size;
	void* ptr = new uint8[bytes];
	memset(ptr, 0, bytes);
	
	return ptr;
}


//---------------------------------------------------------------
//
// ZipFree
//
//---------------------------------------------------------------
void ZipFree(void* refCon, void* adr)
{
	UNUSED(refCon);

	delete [] static_cast<uint8*>(adr);
}


//---------------------------------------------------------------
//
// ThrowZipError
//
//---------------------------------------------------------------
void ThrowZipError(int32 err)
{
	if (err == Z_MEM_ERROR) 
		throw std::bad_alloc();
	else 
		throw XZipException(err, ToUTF8Str(LoadWhisperString(L"Couldn't (un)zip")).c_str());
}		


//---------------------------------------------------------------
//
// ThrowIfZipError
//
//---------------------------------------------------------------
void ThrowIfZipError(int32 err)
{
	if (err == Z_MEM_ERROR) 
		throw std::bad_alloc();

	else if (err != Z_OK)
		throw XZipException(err, ToUTF8Str(LoadWhisperString(L"Couldn't (un)zip")).c_str());
}		

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	High Level API
// ===================================================================================

//---------------------------------------------------------------
//
// GetMaxZippedBytes
//
// Destination buffer must be at least 0.1% larger than srcLen + 
// 12 bytes.
//
//---------------------------------------------------------------
uint32 GetMaxZippedBytes(uint32 srcLen)
{
	PRECONDITION(srcLen < 8*1024L*1024L);

	uint32 bytes = numeric_cast<uint32>(srcLen + srcLen*1.001 + 12);
	
	return bytes;
}


//---------------------------------------------------------------
//
// Zip
//
//---------------------------------------------------------------
void Zip(const void* src, uint32 srcLen, void* dst, uint32* dstLen, int16 level)
{
	PRECONDITION(src != nil);
	PRECONDITION(dst != nil);
	PRECONDITION(dstLen != nil);
	PRECONDITION(srcLen < 8*1024L*1024L);
	PRECONDITION(*dstLen >= GetMaxZippedBytes(srcLen));
	PRECONDITION(*dstLen < 8*1024L*1024L);
	PRECONDITION(level >= kDontCompress);
	PRECONDITION(level <= kTightestCompress);

	z_stream stream;

	stream.next_in   = (uint8 *) src;
	stream.avail_in  = srcLen;
	stream.next_out  = (uint8 *) dst;
	stream.avail_out = *dstLen;

	stream.zalloc = ZipAllocate;		
	stream.zfree  = ZipFree;

	int32 err = deflateInit(&stream, level);
	ThrowIfZipError(err);
	
	err = deflate(&stream, Z_FINISH);	
	if (err == Z_STREAM_END) {
		err = deflateEnd(&stream);
		ThrowIfZipError(err);

		*dstLen = stream.total_out;

	} else {
		if (err == Z_OK)
			err = Z_BUF_ERROR;		

		(void) deflateEnd(&stream);

		ThrowZipError(err);
	}
}

			
//---------------------------------------------------------------
//
// Unzip
//
//---------------------------------------------------------------
void Unzip(const void* src, uint32 srcLen, void* dst, uint32* dstLen)
{
	PRECONDITION(src != nil);
	PRECONDITION(dst != nil);
	PRECONDITION(dstLen != nil);
	PRECONDITION(srcLen < 8*1024L*1024L);
	PRECONDITION(*dstLen >= srcLen - srcLen*1.001 - 12);
	PRECONDITION(*dstLen < 8*1024L*1024L);

	z_stream stream;

	stream.next_in  = (uint8 *) src;
	stream.avail_in = srcLen;

	stream.next_out  = (uint8 *) dst;
	stream.avail_out = *dstLen;

	stream.zalloc = ZipAllocate;	
	stream.zfree  = ZipFree;

	int32 err = inflateInit(&stream);
	ThrowIfZipError(err);

	err = inflate(&stream, Z_FINISH);
	if (err == Z_STREAM_END) {
		err = inflateEnd(&stream);
		ThrowIfZipError(err);

		*dstLen = stream.total_out;
	
	} else {
		(void) inflateEnd(&stream);
		ThrowZipError(err);
	}
}


}		// namespace Whisper
