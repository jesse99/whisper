/*
 *  File:       XCompress.h
 *  Summary:   	C++ wrapper around PNG's gzip compression engine.
 *  Written by: Jesse Jones
 *
 *	Abstract:	gzip is an LZ77 variant that is freely redistributable (see
 *				zlib.h for the legal mumbo jumbo). gzip compresses about as
 *				well as StuffIt, but is a bit faster.
 *
 *  Copyright © 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XCompress.h,v $
 *		Revision 1.4  2000/12/15 09:07:47  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/10 08:50:50  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:28:37  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 9/28/98	JDJ		Created (from Raven)
 */

#pragma once

#include <XExceptions.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	Constants
// ===================================================================================
const int16 kDontCompress 		= 0;	//!< don't do any compression
const int16 kFastestCompress	= 1;	//!< compress as fast as possible (note that decompression speed is relatively insensitive to the compression level)
const int16 kDefaultCompress	= 6;	//!< default compression level
const int16 kTightestCompress 	= 9;	//!< compress as tightly as possible


// ===================================================================================
//	XZipException
//!		The exception thrown by PNG's gzip wrappers.
// ===================================================================================
class CORE_EXPORT XZipException : public std::runtime_error { 

public: 
			XZipException(int32 err, const char* mesg) : std::runtime_error(mesg) {mError = err;}
public:
	int32	mError;
};


// ===================================================================================
//	Medium Level API
//		See zlib.h for the low level API. 
// ===================================================================================
CORE_EXPORT uint32 	GetMaxZippedBytes(uint32 srcLen);
					// Returns the maximum length of srcLen bytes of zipped data.

CORE_EXPORT void 	Zip(const void* src, uint32 srcLen, void* dst, uint32* dstLen, int16 level = kDefaultCompress);
					// Compresses the data at src using the specified compression level. 
					// Upon entry dstLen is the length of the dst buffer (and must be at 
					// least GetMaxZippedBytes() bytes). On exit dstLen is the length of 
					// the zipped data. 
			
CORE_EXPORT void 	Unzip(const void* src, uint32 srcLen, void* dst, uint32* dstLen);
					// Decompresses the data at src. On entry dstLen is the length of 
					// the destination buffer. On exit it's the length of the unzipped 
					// data. 


// ===================================================================================
//	Helpers
// ===================================================================================
CORE_EXPORT void 	ThrowIfZipError(int32 err);
					// Throws XZipException or std::bad_alloc. You'll only need to use 
					// this if you use the zlib.h interfaces directly.

CORE_EXPORT void 	ThrowZipError(int32 err);

CORE_EXPORT void* 	ZipAllocate(void* refCon, unsigned int numItems, unsigned int size);
					// Used by gzip to allocate memory. 
			
CORE_EXPORT void 	ZipFree(void* refCon, void* adr);


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
