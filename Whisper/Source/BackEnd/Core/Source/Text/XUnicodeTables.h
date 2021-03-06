/*
 *  File:       XUnicodeTables.h
 *  Summary:	Tables used to determine properties of Unicode characters.
 *  Written by: Jesse Jones
 *
 *	Abstract:	There are a bunch of ways to look up the property of a Unicode
 *				character. Here are some possibilities:
 *
 *				1) A 64K lookup table can be used. This is very fast since
 *				only one lookup is required and works well on systems with
 *				good VM since typically very little of the table is touched.
 *				However this will bloat executable sizes and Mac OS 8 does
 *				not have good VM.
 *
 *				2) Use multiple lookup tables. For example, the first table
 *				can be indexed using the high byte of the character code. Each
 *				table entry then points to another table that is indexed using
 *				the low byte of the character code. This second table will
 *				often be empty so it can be omitted (the high byte table will
 *				have a nil pointer in the appropiate slot). This is nearly
 *				as fast as the first method, but will ususually consume much
 *				less memory.
 *
 *				3) A single variable length table can be used that run length
 *				encodes alternating runs of characters that have the property
 *				in question or don't have the property. Or, even better, instead
 *				of run counts character codes can be stored. For example, the
 *				first few elements of the IsLetter table look like this: 0x0040, 
 *				0x005A, 0x0060 which means character codes up to 0x40 are not 
 *				letters, 0x41-0x5A are letters (A-Z), and 0x5B-0x60 are not 
 *				letters. Doing this makes it possible to do a binary search to 
 *				find the range a given character belongs in. Once the range is 
 *				found the property can be determined based on whether the range 
 *				index is odd or even. This is very space efficient and reasonably 
 *				speedy.
 *
 *				Currently these tables are all built using the third option.
 *				However UnicodeMapper can build type two tables and it may
 *				be worthwhile to switch some of the tables over ($$$ especially
 *				once Mac OS X starts seeing wide use $$$).
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XUnicodeTables.h,v $
 *		Revision 1.3  2001/03/13 11:11:04  jesjones
 *		Added CORE_EXPORT everywhere.
 *		
 *		Revision 1.2  2000/11/09 12:58:15  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// Normative Tables
extern CORE_EXPORT const wchar_t kDigitTable[];				// first entry is false
extern CORE_EXPORT const wchar_t kNumericTable[];			// first entry is false
extern CORE_EXPORT const wchar_t kControlTable[];			// first entry is true
extern CORE_EXPORT const wchar_t kWhiteSpaceTable[];		// first entry is false
extern CORE_EXPORT const wchar_t kCombiningTable[];			// first entry is false
extern CORE_EXPORT const wchar_t kPrivateTable[];			// first entry is false
extern CORE_EXPORT const wchar_t kValidTable[];				// first entry is true

// Informative tables
extern CORE_EXPORT const wchar_t kLetterTable[];			// first entry is false
extern CORE_EXPORT const wchar_t kUpperCaseTable[];			// first entry is false
extern CORE_EXPORT const wchar_t kLowerCaseTable[];			// first entry is false
extern CORE_EXPORT const wchar_t kTitleCaseTable[];			// first entry is false
extern CORE_EXPORT const wchar_t kPunctuationTable[];		// first entry is false
extern CORE_EXPORT const wchar_t kSymbolTable[];			// first entry is false
extern CORE_EXPORT const wchar_t kMathSymbolTable[];		// first entry is false
extern CORE_EXPORT const wchar_t kCurrencySymbolTable[];	// first entry is false

// Informative mapping tables
struct SCharMapping {
	wchar_t in;
	wchar_t out;
					
		bool 	operator==(wchar_t rhs) const		{return in == rhs;}
		bool 	operator<(wchar_t rhs) const		{return in < rhs;}
};

extern CORE_EXPORT const SCharMapping kUpperCaseMapping[];	// doesn't include first 128 characters
extern CORE_EXPORT const SCharMapping kLowerCaseMapping[];	


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
