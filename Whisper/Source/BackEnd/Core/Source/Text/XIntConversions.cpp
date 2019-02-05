/*
 *	File:		XIntConversions.cpp
 *  Summary:	Integer to string and string to integer routines.
 *	Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: XIntConversions.cpp,v $
 *		Revision 1.3  2001/04/27 04:33:31  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:39:08  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XIntConversions.h>

#include <XDebug.h>
#include <XExceptions.h>
#include <XFloatConversions.h>
#include <XNumbers.h>
#include <XStringUtils.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
enum EBases {kBinary = 2, kDecimal = 10, kHex = 16};


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// DigitsNeeded
//
//---------------------------------------------------------------
static int32 DigitsNeeded(uint32 numb, EBases base) 
{
	int32 digits = 0;
	
	do {
		digits++;
		numb /= (uint32) base;
	} while (numb != 0);
	
	return digits;
}


//---------------------------------------------------------------
//
// DigitToChar
//
//---------------------------------------------------------------
static char DigitToChar(uint32 digit) 
{
	PRECONDITION(digit < 16);
	
	char ch;
	
	if (digit < 10)
		ch = (char) (digit + '0');
	else if (digit < 16)
		ch = (char) (digit - 10 + 'A');
	else
		ch = (char) digit;
		
	return ch;
}


//---------------------------------------------------------------
//
// NumberToStr
//
//---------------------------------------------------------------
static std::wstring NumberToStr(uint32 numb, EBases base, int32 fieldWidth, char pad) 
{
	PRECONDITION(fieldWidth > 0);

	std::wstring str;
	
	if (DigitsNeeded(numb, base) <= fieldWidth) {
		uint32 index = (uint32) fieldWidth;			// start with the least significant digit
		str.resize(index);
		do {
			uint32 digit = numb % base;
			numb /= (uint32) base;
			str[--index] = DigitToChar(digit);
		} while (numb != 0);
		while (--index!= ULONG_MAX) 				// add the pad chars
			str[index] = pad;
			
	} else
		str = std::wstring((uint32) fieldWidth, '*');

	return str;
}


//---------------------------------------------------------------
//
// ValidDigit
//
//---------------------------------------------------------------
static bool ValidDigit(char ch, EBases base) 
{
	bool isValid = false;

	if (base == kBinary)
		isValid = ch == '0' || ch == '1';
	else if (base == kDecimal)
		isValid = ch >= '0' && ch <= '9';
	else if (base == kHex)
		isValid = IsHexDigit(ch);
		
	return isValid;
}


//---------------------------------------------------------------
//
// CharToDigit
//
//---------------------------------------------------------------
static int32 CharToDigit(char ch) 
{
	int32 digit;
	
	if (ch > '9')
		digit = ConvertToUpperCase(ch) - 'A' + 10;
	else
		digit = ch - '0';
		
	return digit;
}


//---------------------------------------------------------------
//
// GetSign
//
//---------------------------------------------------------------
static bool GetSign(const char*& str) 
{	
	char ch = *str;
	if (ch == '+')
		str++;
	else if (ch == '-') 
		str++;
		
	return ch == '-';
}


//---------------------------------------------------------------
//
// FindBase
//
//---------------------------------------------------------------
static EBases FindBase(const char*& str) 
{
	EBases base;
	
	if (*str == '%')
		base = kBinary;
	else if (*str == '$')
		base = kHex;
	else 
		base = kDecimal;

	if (base != kDecimal) 
		str++;
	
	return base;
}


//--------------------------------------------------------------------------------
//
// StripPadding
//
//--------------------------------------------------------------------------------
static std::wstring StripPadding(const std::wstring& str, char pad)
{
	STD::size_t first = str.find_first_not_of(pad);
	STD::size_t last = str.find_last_not_of(pad);

	return str.substr(first, last - first + 1);
}
	
	
//---------------------------------------------------------------
//
// StrToNum (const char*, int32, int32)
//
//---------------------------------------------------------------
static int32 StrToNum(const char* str, int32 min, int32 max) 
{
	int32 numb = 0;
	
	bool isNeg = GetSign(str);
	EBases base = FindBase(str);
	do {
		char ch = *str++;
		if (ValidDigit(ch, base)) {
			int32 digit = CharToDigit(ch);
			if (isNeg)
				if (numb >= (min + digit)/base)
					numb = base*numb - digit;
				else
					throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too small.")));
					
			else
				if (numb <= (max - digit)/base)
					numb = base*numb + digit;
				else 
					throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too big.")));
					
		} else
			throw std::invalid_argument(ToUTF8Str(LoadWhisperString(L"Integer number expected.")));
	} while (*str != '\0');
	
	return numb;
}


//---------------------------------------------------------------
//
// StrToNum (const char*, uint32)
//
//---------------------------------------------------------------
static uint32 StrToNum(const char* str, uint32 max) 
{
	uint32	numb = 0;
	
	EBases base = FindBase(str);
	do {
		char ch = *str++;
		if (ValidDigit(ch, base)) {
			int32 digit = CharToDigit(ch);
			if (numb <= (max - digit)/base)
				numb = base*numb + digit;
			else 
				throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too big.")));
				
		} else
			throw std::invalid_argument(ToUTF8Str(LoadWhisperString(L"Integer number expected.")));
	} while (*str != '\0');
	
	return numb;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// Int32ToStr
//
//---------------------------------------------------------------
std::wstring Int32ToStr(int32 numb, int32 fieldWidth, char pad) 
{
	std::wstring str;
	
	if (fieldWidth == -1) {
		fieldWidth = DigitsNeeded((uint32) Abs(numb), kDecimal);	
		if (numb < 0) fieldWidth++;
	}
	
	if (numb >= 0)
		str = NumberToStr((uint32) Abs(numb), kDecimal, fieldWidth, pad);
		
	else {
		str = NumberToStr((uint32) Abs(numb), kDecimal, fieldWidth, pad);
		uint32 index = 0;
		while (str[index] == pad) index++;
		if (index > 0)
			str[index-1UL] = '-';
		else
			str = std::wstring((uint32) fieldWidth, '*');
	}
	
	return str;
}


//---------------------------------------------------------------
//
// UInt32ToStr
//
//---------------------------------------------------------------
std::wstring UInt32ToStr(uint32 numb, int32 fieldWidth, char pad) 
{
	std::wstring str;
	
	if (fieldWidth == -1) 
		fieldWidth = DigitsNeeded(numb, kDecimal);	
	
	str = NumberToStr(numb, kDecimal, fieldWidth, pad);

	return str;
}


//---------------------------------------------------------------
//
// UInt8ToHex
//
//---------------------------------------------------------------
std::wstring UInt8ToHex(uint8 numb) 
{	
	return NumberToStr((uint32) numb, kHex, 2, '0');
}


//---------------------------------------------------------------
//
// UInt16ToHex
//
//---------------------------------------------------------------
std::wstring UInt16ToHex(uint16 numb) 
{	
	return NumberToStr((uint32) numb, kHex, 4, '0');
}


//---------------------------------------------------------------
//
// UInt32ToHex
//
//---------------------------------------------------------------
std::wstring UInt32ToHex (uint32 numb) 
{
	std::wstring str1, str2;
	
	str1 = NumberToStr((uint32) numb >> 16, kHex, 4, '0');
	str2 = NumberToStr((uint32) (numb & 0xFFFF), kHex, 4, '0');
	
	return str1 + str2;
}


//---------------------------------------------------------------
//
// UInt8ToBinary
//
//---------------------------------------------------------------
std::wstring UInt8ToBinary(uint8 numb) 
{
	return NumberToStr(numb, kBinary, 8, '0');
}


//---------------------------------------------------------------
//
// UInt16ToBinary
//
//---------------------------------------------------------------
std::wstring UInt16ToBinary(uint16 numb) 
{
	return NumberToStr(numb, kBinary, 16, '0');
}


//---------------------------------------------------------------
//
// UInt32ToBinary
//
//---------------------------------------------------------------
std::wstring UInt32ToBinary(uint32 numb) 
{
	std::wstring str1, str2;
	
	str1 = NumberToStr(numb >> 16, kBinary, 16, '0');
	str2 = NumberToStr(numb & 0xFFFF, kBinary, 16, '0');
	
	return str1 + str2;
}


//---------------------------------------------------------------
//
// BytesToStr
//
//---------------------------------------------------------------
std::wstring BytesToStr(uint64 bytes, int32 decPlaces)
{
	std::wstring result;
	
	if (bytes < 1024)
		result = StripLeading(DoubleToFormStr(bytes, 12, 0)) + L" " + LoadWhisperString(L"bytes");
		
	else if (bytes < 1024*1024)
		result = StripLeading(DoubleToFormStr(bytes/1024.0, 12, decPlaces)) + L" " + LoadWhisperString(L"K (kilo)");
	
	else
		result = StripLeading(DoubleToFormStr(bytes/(1024.0*1024.0), 12, decPlaces)) + L" " + LoadWhisperString(L"MB (megabyte)");
		
	return result;
}

	
//---------------------------------------------------------------
//
// StrToInt8
//
//---------------------------------------------------------------
int8 StrToInt8(const std::wstring& inStr, char pad) 
{	
	std::wstring str = StripPadding(inStr, pad);

	return (int8) StrToNum(ToPlatformStr(str).c_str(), std::numeric_limits<int8>::min(), std::numeric_limits<int8>::max());
}
	

//---------------------------------------------------------------
//
// StrToInt16
//
//---------------------------------------------------------------
int16 StrToInt16(const std::wstring& inStr, char pad) 
{	
	std::wstring str = StripPadding(inStr, pad);

	return (int16) StrToNum(ToPlatformStr(str).c_str(), std::numeric_limits<int16>::min(), std::numeric_limits<int16>::max());
}
	

//---------------------------------------------------------------
//
// StrToInt32
//
//---------------------------------------------------------------
int32 StrToInt32(const std::wstring& inStr, char pad) 
{	
	std::wstring str = StripPadding(inStr, pad);

	return StrToNum(ToPlatformStr(str).c_str(), std::numeric_limits<int32>::min(), std::numeric_limits<int32>::max());
}


//---------------------------------------------------------------
//
// StrToUInt8
//
//---------------------------------------------------------------
uint8 StrToUInt8(const std::wstring& inStr, char pad) 
{	
	std::wstring str = StripPadding(inStr, pad);

	return (uint8) StrToNum(ToPlatformStr(str).c_str(), std::numeric_limits<uint8>::max());
}


//---------------------------------------------------------------
//
// StrToUInt16
//
//---------------------------------------------------------------
uint16 StrToUInt16(const std::wstring& inStr, char pad) 
{	
	std::wstring str = StripPadding(inStr, pad);

	return (uint16) StrToNum(ToPlatformStr(str).c_str(), std::numeric_limits<uint16>::max());
}

//---------------------------------------------------------------
//
// StrToUInt32
//
//---------------------------------------------------------------
uint32 StrToUInt32(const std::wstring& inStr, char pad) 
{	
	std::wstring str = StripPadding(inStr, pad);

	return StrToNum(ToPlatformStr(str).c_str(), std::numeric_limits<uint32>::max());
}


}	// namespace Whisper

