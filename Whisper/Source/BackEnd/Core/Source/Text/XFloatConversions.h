/*
 *	File:		XFloatConversions.h
 *  Summary:	Float to string and string to float routines.
 *	Written by:	Jesse Jones
 *
 *  Copyright � 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: XFloatConversions.h,v $
 *		Revision 1.3  2001/02/12 09:48:17  jesjones
 *		Added #pragma export's.
 *		
 *		Revision 1.2  2000/11/09 12:36:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	String to Float conversions
// ===================================================================================
CORE_EXPORT double 	StrToDouble(const std::wstring& str, char pad = ' ');
				// Converts a str in either format (conventional or E notation) to a 
				// real number. Throws invalid_argument if the string is not a float. 
				// If the number is too big or too small +/- INFINITY is returned. 
			

// ===================================================================================
//	Float to String conversions
// ===================================================================================
CORE_EXPORT std::wstring DoubleToStr(double numb, int32 fieldWidth = 9, int32 decPlaces = -1, char pad = ' ');
				// General real number to str conversion -- formatted if possible, 
				// scientific otherwise. Note that fieldWidth must be at least 9 to 
				// ensure output (because the exponent may be 4 digits). If fieldWidth 
				// is too small the string will be filed with asterisks. If decPlaces = 
				// -1 the entire field will be filled.

CORE_EXPORT std::wstring DoubleToSciStr(double numb, int32 fieldWidth = 9, int32 sigDigits = -1, char pad = ' ');
				// Real to scientific (E) notation str.

CORE_EXPORT std::wstring DoubleToFormStr(double numb, int32 fieldWidth = 9, int32 decPlaces = -1, char pad = ' ');
				// Real to conventional representation str.


// ===================================================================================
//	Misc
// ===================================================================================
CORE_EXPORT wchar_t GetDecimalPoint();
				// Returns the decimal point character for the current locale.


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
