/*
 *	File:		XTextConversions.h
 *  Summary:	Functions used to convert arbitrary types to and from strings.
 *	Written by:	Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: XTextConversions.h,v $
 *		Revision 1.2  2001/03/02 11:25:29  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.1  2001/03/01 11:47:41  jesjones
 *		Checked in
 */

#pragma once

#include <XStringUtils.h>
#include <XTranscode.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	Basic Conversions
//		We provide these to avoid instantiating gobs of code in our clients (this isn't
//		too terrible because the linker will strip the duplicates out, but it looks bad
//		and makes the compiler work a lot harder).
// ===================================================================================
CORE_EXPORT void 	FromStr(const std::wstring& str, bool& value);

CORE_EXPORT void 	FromStr(const std::wstring& str, int16& value);
CORE_EXPORT void 	FromStr(const std::wstring& str, const std::wstring& name, int16 min, int16 max, int16& value);

CORE_EXPORT void 	FromStr(const std::wstring& str, uint16& value);
CORE_EXPORT void 	FromStr(const std::wstring& str, const std::wstring& name, uint16 min, uint16 max, uint16& value);

CORE_EXPORT void 	FromStr(const std::wstring& str, int32& value);
CORE_EXPORT void 	FromStr(const std::wstring& str, const std::wstring& name, int32 min, int32 max, int32& value);

CORE_EXPORT void 	FromStr(const std::wstring& str, uint32& value);
CORE_EXPORT void 	FromStr(const std::wstring& str, const std::wstring& name, uint32 min, uint32 max, uint32& value);

CORE_EXPORT void 	FromStr(const std::wstring& str, float& value);
CORE_EXPORT void 	FromStr(const std::wstring& str, const std::wstring& name, float min, float max, float& value);

CORE_EXPORT void 	FromStr(const std::wstring& str, double& value);
CORE_EXPORT void 	FromStr(const std::wstring& str, const std::wstring& name, double min, double max, double& value);

inline 		void 	FromStr(const std::wstring& str, std::wstring& value)		{value = str;}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
