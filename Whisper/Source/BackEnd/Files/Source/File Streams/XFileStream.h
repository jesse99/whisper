/*
 *  File:		XFileStream.h
 *  Summary:	Binary stream classes that work with files.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFileStream.h,v $
 *		Revision 1.4  2001/03/05 05:41:28  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:52:20  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:35:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XConstants.h>
#include <XStream.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFile;


// ========================================================================================
//	class XInFileStream
//!		Binary stream class that works with files.
// ========================================================================================
class FILES_EXPORT XInFileStream : public XInStream {

	typedef XInStream Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XInFileStream();
	
	explicit			XInFileStream(XFile& file, bool raw = kCooked);
						/**< File must be open. Defaults to including a header. */
						
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
	XFile&	mFile;
};


// ========================================================================================
//	class XOutFileStream
//!		Binary stream class that works with files.
// ========================================================================================
class FILES_EXPORT XOutFileStream : public XOutStream {

	typedef XOutStream Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XOutFileStream();
	
	explicit			XOutFileStream(XFile& file, bool raw = kCooked);
						/**< File must be open. Defaults to including a header. */
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnWriteBytes(const void* src, uint32 bytes);
	
//-----------------------------------
//	Member Data
//
protected:
	XFile&	mFile;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
