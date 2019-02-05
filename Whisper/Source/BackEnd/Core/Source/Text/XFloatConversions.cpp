/*
 *	File:		XFloatConversions.cpp
 *  Summary:	Float to string and string to float routines.
 *	Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):	
 *
 *		$Log: XFloatConversions.cpp,v $
 *		Revision 1.3  2001/04/27 04:32:46  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:36:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <8>	10/31/00	JDJ		Exposed GetDecimalPoint.
 *		 <7>	 7/31/00	JDJ		DoubleToFormStr now allows decPlaces of 0.
 *		 <6>	 1/25/00	JDJ		Removed the Pro 5fc3's sscanf work around (it depends on
 *									XRegularExpression which isn't part of the Core framelet).
 *		 <5>	 6/13/99	JDJ		Passes a char* to sscanf (instead of an XChar*). Worked
 *									around a bug in Pro 5fc3's sscanf.
 *		 <4>	 3/03/99	JDJ		Added GetDecimalPoint.
 *		 <3>	 2/11/99	JDJ		StrToDouble accepts INFINITY as a synonym for INF.
 *		 <2>	 1/26/99	JDJ		Uses IsDigit instead of isdigit.
 *		 <1>	 1/11/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XFloatConversions.h>

#include <XDebug.h>
#include <XExceptions.h>
#include <XNumbers.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// ConvertDouble
//
//---------------------------------------------------------------
static std::wstring ConvertDouble(const wchar_t* controlStr, int32 fieldWidth, int32 precision, double numb, char pad)
{
	PRECONDITION(!isnan(numb));
	PRECONDITION(isfinite(numb));
	
	std::wstring str;
	
	wchar_t temp[256];
	std::swprintf(temp, 256, controlStr, fieldWidth, precision, numb);
	
	str = StripLeading(temp);
	if (numb >= 0)
		str = std::wstring(1, pad) + str;		
		
	uint32 len = str.length();
	if (len < fieldWidth && pad != 't')
		str = std::wstring(fieldWidth - len, pad) + str;		
	
	if (str.length() > fieldWidth)
		str = std::wstring((uint32) fieldWidth, '*');
	
	return str;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// StrToDouble
//
//---------------------------------------------------------------
double StrToDouble(const std::wstring& inStr, char pad)
{	
	double value;
	
	std::wstring str = ConvertToUpperCase(StripTrailing(StripLeading(inStr, std::wstring(1, pad)), std::wstring(1, pad)));
	str = Replace(str, GetDecimalPoint(), L'.');
	
	if (str == LoadWhisperString(L"NAN") || str == LoadWhisperString(L"+NAN") || str == LoadWhisperString(L"-NAN")) {				// CW Pro 1 libraries don't handle NAN or INF...
		value = NAN;
	
	} else if (str == LoadWhisperString(L"INF") || str == LoadWhisperString(L"+INF")) {
		value = INFINITY;
		
	} else if (str == LoadWhisperString(L"INFINITY") || str == LoadWhisperString(L"+INFINITY")) {
		value = INFINITY;
		
	} else if (str == LoadWhisperString(L"-INF") || str == LoadWhisperString(L"-INFINITY")) {
		value = -INFINITY;
		
	} else {
		if (str.length() == 0)
			throw std::invalid_argument("Internal Error: Can't convert an empty string into a floating point number.");
		
		if (str[0] != '+' && str[0] != '-' && str[0] != '.' && !IsDigit(str[0]))
			throw std::invalid_argument(ToUTF8Str((L"Internal Error: Can't convert " + inStr + L" into a floating point number.")));
			
		char ch = '\0';
		int32 numConverted = std::sscanf(ToPlatformStr(str).c_str(), "%lf%c", &value, &ch);
		if (numConverted != 1)
			throw std::invalid_argument(ToUTF8Str((L"Internal Error: Can't convert " + inStr + L" into a floating point number.")));
	}
	
	return value;
}


//---------------------------------------------------------------
//
// DoubleToStr
//
//---------------------------------------------------------------
std::wstring DoubleToStr(double value, int32 fieldWidth, int32 decPlaces, char pad)
{
	PRECONDITION(pad != '*');
	
	std::wstring str = DoubleToFormStr(value, fieldWidth, decPlaces, pad);
	
	if (str[0] == '*')
		str = DoubleToSciStr(value, fieldWidth, decPlaces == -1 ? decPlaces : decPlaces + 1, pad);
		
	return str;
}


//---------------------------------------------------------------
//
// DoubleToSciStr
//
//---------------------------------------------------------------
std::wstring DoubleToSciStr(double value, int32 fieldWidth, int32 sigDigits, char pad)
{
	PRECONDITION(fieldWidth >= 0);
	PRECONDITION(sigDigits == -1 || sigDigits > 0);
	
	std::wstring str;
	
	if (isnan(value)) {
		str = LoadWhisperString(L"NAN");
		
		if (str.length() > fieldWidth)
			str = std::wstring((uint32) fieldWidth, '*');
		
	} else if (!isfinite(value)) {
		if (value > 0.0)
			str = LoadWhisperString(L"INF");
		else
			str = LoadWhisperString(L"-INF");
		
		if (str.length() > fieldWidth)
			str = std::wstring((uint32) fieldWidth, '*');
			
	} else {
		int32 precision = sigDigits - 1;
		
		if (sigDigits == -1) {
			std::wstring temp = ConvertDouble(L"%*.*e", 16, 0, value, 't');
			ASSERT(temp[0] != '*');

			precision = (int32) (fieldWidth - temp.length() - 1);
		}
		
		if (precision >= 0)
			str = ConvertDouble(L"%#*.*e", fieldWidth, precision, value, pad);	// # flag causes sprintf to add a decimal point even when precision is zero
		else
			str = std::wstring((uint32) fieldWidth, '*');
	}
	
	if (str.length() < fieldWidth)
		str = std::wstring(fieldWidth - str.length(), pad) + str;

	str = Replace(str, L'.', GetDecimalPoint());

	return str;
}


//---------------------------------------------------------------
//
// DoubleToFormStr
//
//---------------------------------------------------------------
std::wstring DoubleToFormStr(double value, int32 fieldWidth, int32 decPlaces, char pad)
{
	PRECONDITION(fieldWidth >= 0);
	PRECONDITION(decPlaces >= -1);
	
	std::wstring str;
	
	if (isnan(value)) {
		str = LoadWhisperString(L"NAN");
		
		if (str.length() > fieldWidth)
			str = std::wstring((uint32) fieldWidth, '*');
		
	} else if (!isfinite(value)) {
		if (value > 0.0)
			str = LoadWhisperString(L"INF");
		else
			str = LoadWhisperString(L"-INF");
		
		if (str.length() > fieldWidth)
			str = std::wstring((uint32) fieldWidth, '*');
			
	} else if (decPlaces == 0) {		
		str = ConvertDouble(L"%*.*f", fieldWidth, decPlaces, value, pad);
			
	} else {
		int32 precision = decPlaces;
		
		if (decPlaces == -1) {
			std::wstring temp = ConvertDouble(L"%*.*f", (int32) DBL_DIG + 1, 0, value, 't');
			ASSERT(temp[0] != '*');

			precision = (int32) (fieldWidth - temp.length() - 1);
		}
		
		if (precision >= 0)
			str = ConvertDouble(L"%#*.*f", fieldWidth, precision, value, pad);	
		else
			str = std::wstring((uint32) fieldWidth, '*');
	}
	
	if (str.length() < fieldWidth)
		str = std::wstring(fieldWidth - str.length(), pad) + str;

	str = Replace(str, L'.', GetDecimalPoint());

	return str;
}


//---------------------------------------------------------------
//
// GetDecimalPoint
//
// On Windows it would be better to simply use GetNumberFormat
// to localize the string. However, as far as I can tell, the
// Windows API doesn't provide a way to convert a floating point
// string to a canonical format or to a double which makes things
// a bit dicy. The other problem with GetNumberFormat (and the Mac
// equivalent) is that they don't support scientific notation.
//
//---------------------------------------------------------------
wchar_t GetDecimalPoint()
{
	static wchar_t decimal = 0;
	
	if (decimal == 0) {
#if MAC
		Handle hand = ::GetIntlResource(0);
		ThrowIfResFail(hand);
		
		Intl0Hndl data = (Intl0Hndl) hand;
		decimal = (**data).decimalPt;
		
#elif WIN
		TCHAR buffer[2];
		int32 succeeded = ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, buffer, sizeof(buffer));
		ThrowIf(!succeeded); 

		decimal = buffer[0];
#endif
	}
	
	POSTCONDITION(decimal != 0);
	
	return decimal;
}


}	// namespace Whisper
