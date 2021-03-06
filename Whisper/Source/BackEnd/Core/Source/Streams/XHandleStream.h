/*
 *  File:		XHandleStream.h
 *  Summary:	Binary stream classes that work with XHandle's.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XHandleStream.h,v $
 *		Revision 1.4  2001/03/05 05:41:52  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:53:05  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:38:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XHandle.h>
#include <XStream.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ========================================================================================
//	class XInHandleStream
//!		Binary stream class that work with XHandle's.
// ========================================================================================
class CORE_EXPORT XInHandleStream : public XInStream {

	typedef XInStream Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XInHandleStream();
	
	explicit			XInHandleStream(const XHandle& hand, uint32 startPos = 0, bool raw = kRaw);
						/**< Defaults to not including a stream header. */
												
//-----------------------------------
//	New API
//
public:
			XHandle 	GetHandle() const							{return mHandle;}

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
	XHandle		mHandle;
	uint32		mPos;
};


// ========================================================================================
//	class XOutHandleStream
//!		Binary stream class that work with XHandle's.
// ========================================================================================
class CORE_EXPORT XOutHandleStream : public XOutStream {

	typedef XOutStream Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XOutHandleStream();
	
	explicit			XOutHandleStream(uint32 reserve = 16, bool raw = kRaw);
						/**< Defaults to not including a stream header. Handle starts with reserve
						unused bytes. */
						
	explicit			XOutHandleStream(const XHandle& hand, uint32 startPos = 0, bool raw = kRaw);
						
//-----------------------------------
//	New API
//
public:
			XHandle 	GetHandle() const;

//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnWriteBytes(const void* src, uint32 bytes);

//-----------------------------------
//	Internal API
//
protected:
	virtual void 		DoExpandHandle(uint32 requiredSize);

//-----------------------------------
//	Member Data
//
protected:
	XHandle		mHandle;
	uint32		mPos;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
