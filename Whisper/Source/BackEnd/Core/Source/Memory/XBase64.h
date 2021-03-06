/*
 *  File:       XBase64.h
 *  Summary:   	Routines to convert binary data to and from 7-bit ASCII strings.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XBase64.h,v $
 *		Revision 1.1  2001/04/27 04:15:05  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XHandle.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	Allocations
// ===================================================================================
CORE_EXPORT	std::string EncodeBase64(const void* buffer, uint32 bytes);

CORE_EXPORT	XHandle 	DecodeBase64(const std::string& text);


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

