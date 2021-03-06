/*
 *  File:		XPointerStream.h
 *  Summary:	Binary stream classes that work with raw memory pointers.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XPointerStream.h,v $
 *		Revision 1.4  2001/03/05 05:43:04  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:54:36  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:47:18  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XConstants.h>
#include <XStream.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ========================================================================================
//	class XInPointerStream
//!		Binary stream class that works with raw memory pointers.
// ========================================================================================
class CORE_EXPORT XInPointerStream : public XInStream {

	typedef XInStream Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XInPointerStream();
	
						XInPointerStream(const void* ptr, uint32 bufferSize, bool raw = kRaw);
						/**< Defaults to not including a stream header. */
												
//-----------------------------------
//	Inherited API
//
public:
	virtual uint32		GetLength() const;

	virtual uint32		GetPosition() const;
	
	virtual void		SetPosition(uint32 newPosition);

protected:
	virtual void 		OnReadBytes(void* dst, uint32 bytes);

//-----------------------------------
//	Member Data
//
protected:
	const uint8*	mPtr;
	uint32			mSize;
	uint32			mPos;
};


// ========================================================================================
//	class XOutPointerStream
//!		Binary stream class that works with raw memory pointers.
// ========================================================================================
class CORE_EXPORT XOutPointerStream : public XOutStream {

	typedef XOutStream Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XOutPointerStream();
	
						XOutPointerStream(void* ptr, uint32 bufferSize, bool raw = kRaw);
						/**< Defaults to not including a stream header. Handle starts with reserve
						unused bytes. */
												
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnWriteBytes(const void* src, uint32 bytes);

//-----------------------------------
//	Member Data
//
protected:
	uint8*		mPtr;
	uint32		mSize;
	uint32		mPos;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
