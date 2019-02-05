/*
 *	File:		XIntConversions.h
 *  Summary:	Integer to string and string to integer routines.
 *	Written by:	Jesse Jones
 *
 *  Copyright © 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: XIntConversions.h,v $
 *		Revision 1.2  2000/11/09 12:39:13  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <2>	 7/19/00	JDJ		Added BytesToStr.
 *		 <1>	 1/01/98	JDJ		Created (from Raven)
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	String to Int conversions
// 		Allows leading pad chars and an optional sign followed by an optional base 
// 		character (% for binary and $ for hex). Throws invalid_argument if the string 
//		is not an integer and range_error if the number is too large or too small.
// ===================================================================================
CORE_EXPORT uint32 	StrToUInt32(const std::wstring& str, char pad = ' ');
CORE_EXPORT uint16 	StrToUInt16(const std::wstring& str, char pad = ' ');
CORE_EXPORT uint8 	StrToUInt8(const std::wstring& str, char pad = ' ');
			
CORE_EXPORT int32 	StrToInt32(const std::wstring& str, char pad = ' ');
CORE_EXPORT int16 	StrToInt16(const std::wstring& str, char pad = ' ');
CORE_EXPORT int8 	StrToInt8(const std::wstring& str, char pad = ' ');


// ===================================================================================
//	Int to String conversions
// 		Converts an int to a string using fieldWidth characters. If fieldWidth is too 
// 		small the str is filled with *'s, if fieldWidth is too large pad chars are used 
// 		in the front, if fieldWidth is -1 a minimum fieldWidth is used.
// ===================================================================================
CORE_EXPORT std::wstring UInt32ToStr(uint32 numb, int32 fieldWidth = -1, char pad = ' ');
inline std::wstring 	 UInt16ToStr(uint16 numb, int32 fieldWidth = -1, char pad = ' ')	{return UInt32ToStr(numb, fieldWidth, pad);}
inline std::wstring 	 UInt8ToStr(uint8 numb, int32 fieldWidth = -1, char pad = ' ')	{return UInt32ToStr(numb, fieldWidth, pad);}

CORE_EXPORT std::wstring Int32ToStr(int32 numb, int32 fieldWidth = -1, char pad = ' ');
inline std::wstring 	 Int16ToStr(int16 numb, int32 fieldWidth = -1, char pad = ' ')	{return Int32ToStr(numb, fieldWidth, pad);}
inline std::wstring 	 Int8ToStr(int8 numb, int32 fieldWidth = -1, char pad = ' ')		{return Int32ToStr(numb, fieldWidth, pad);}   

CORE_EXPORT std::wstring BytesToStr(uint64 bytes, int32 decPlaces = 1);	// returns strings like "100 bytes", "2.5 K", or "100.3 MB".


// ===================================================================================
//	Int to Hex string conversions
// 		These return 8, 4, and 2 digit hex strings. 
// ===================================================================================
CORE_EXPORT std::wstring UInt32ToHex(uint32 numb);
CORE_EXPORT std::wstring UInt16ToHex(uint16 numb);
CORE_EXPORT std::wstring UInt8ToHex(uint8 numb);

inline std::wstring 	Int32ToHex(int32 numb)				{return UInt32ToHex((uint32) numb);}
inline std::wstring 	Int16ToHex(int16 numb)				{return UInt16ToHex((uint16) numb);}
inline std::wstring 	Int8ToHex(int8 numb)				{return UInt8ToHex((uint8) numb);}
	

// ===================================================================================
//	Int to Binary string conversions
//		These return 32, 16, and 8 digit binary strings. 
// ===================================================================================
CORE_EXPORT std::wstring UInt32ToBinary(uint32 numb);
CORE_EXPORT std::wstring UInt16ToBinary(uint16 numb);
CORE_EXPORT std::wstring UInt8ToBinary(uint8 numb);

inline std::wstring 	Int32ToBinary(int32 numb)			{return UInt32ToBinary((uint32) numb);}
inline std::wstring 	Int16ToBinary(int16 numb)			{return UInt16ToBinary((uint16) numb);}
inline std::wstring		Int8ToBinary(int8 numb)				{return UInt8ToBinary((uint8) numb);}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
