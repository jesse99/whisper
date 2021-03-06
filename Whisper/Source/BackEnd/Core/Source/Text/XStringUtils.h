/*
 *  File:       XStringUtils.h
 *  Summary:   	Misc string utilities
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	  	
 *
 *		$Log: XStringUtils.h,v $
 *		Revision 1.4  2001/03/13 11:10:44  jesjones
 *		Minor formatting tweaks.
 *		
 *		Revision 1.3  2001/03/05 04:40:59  jesjones
 *		Added FormatAsHTML.
 *		
 *		Revision 1.2  2000/11/09 12:54:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <15>	10/31/00	JDJ		Added FormatAsXML.
 *		 <14>	 7/19/00	JDJ		Seperate string table loaders are now used for Whisper and app strings.
 *		 <13>	 4/20/00	JDJ		Added LoadWhisperString and LoadAppString overloads.
 *		 <12>	 8/06/99	JDJ		Added stricmp (MetroWerks).
 *		 <11>	 8/04/99	JDJ		Added Replace (XString, XChar, XChar, XChar).
 *		 <10>	 4/07/99	JDJ		Renamed Strip(XString, XString) StripLeading. Added Remove functions.
 *		 <9>	 4/06/99	JDJ		Renamed StrToUpperStr ConvertToUpperCase. Renamed 
 *									StrToLowerStr ConvertToUpperCase. Added overloads that 
 *									take XChar's.
 *		 <8>	 2/21/99	JDJ		Added CompareStrings. 
 *		 <7>	 2/16/99	JDJ		Added IDToStr and StrToID. 
 *		 <6>	 2/15/99	JDJ		Added locale functions and InstallStringTableLoader. Added LoadWhisperString 
 *									and LoadAppString overloads. Removed LookUpString (Mac).
 *		 <5>	 2/13/99	JDJ		Added IsWhiteSpace(XString). 
 *		 <4>	 2/05/99	JDJ		Nuked IsAlpha, IsAlphaNum, IsLower, and IsUpper. Added IsNumeric, 
 *									IsControl, IsLetter, IsIdeographic, IsAlphabetical, IsUpperCase, 
 *									IsLowerCase, IsTitleCase, IsPunctuation, IsSymbol, IsMathSymbol, 
 *									IsCurrencySymbol, IsWhiteSpace, IsCombining, IsPrivate, IsValid,
 *									and IsPrintable.
 *		 <3>	 1/27/99	JDJ		Changed Replace (string, char, char) to Replace (string, XChar, XChar).
 *		 <2>	 1/26/99	JDJ		Added IsAlpha, IsAlphaNum, IsDigit, IsLower, and IsUpper.
 *		 <1>	11/17/97	JDJ		Created (from Raven)
 */

#pragma once

#include <map>

#include <XConstants.h>
#include <XLocale.h>
#include <XUnicodeTables.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Types
//
typedef std::map<std::wstring, std::wstring> StringTable;
typedef void (*StringTableLoader)(const XLocale& locale, StringTable& table);


// ===================================================================================
//	Locales
// ===================================================================================	
CORE_EXPORT XLocale GetLocale();

CORE_EXPORT void SetLocale(const XLocale& locale);
				// This is intended for debugging: you can call this function to see what
				// the app would look like in a different locale.
			

// ===================================================================================
//	Loading Strings
// ===================================================================================	
CORE_EXPORT void InstallWhisperLoader(StringTableLoader loader);
CORE_EXPORT void InstallAppLoader(StringTableLoader loader);
				// Loaders should attempt to load strings using both the language and the
				// region. If an appropiate string cannot be found the loader should fall
				// back to using just the language. Note that XApplication, by default,
				// installs loaders that use XML files.
			
CORE_EXPORT std::wstring LoadWhisperString(const std::wstring& str);
CORE_EXPORT std::wstring LoadAppString(const std::wstring& str);
				// Whisper handles string localization differently from most frameworks:
				// instead of loading strings from a resource you pass the string to
				// LoadAppString which will then check to see if a localized version of
				// the string exists. If a localized version exists it is returned,
				// otherwise the input string is returned. By doing this the string
				// contents aren't hidden behind a resource id, the string is easier
				// to change, and the cross platform issues are eased.

CORE_EXPORT std::wstring LoadWhisperString(const std::wstring& format, const std::wstring& arg1);
CORE_EXPORT std::wstring LoadWhisperString(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2);
CORE_EXPORT std::wstring LoadWhisperString(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2, const std::wstring& arg3, const std::wstring& arg4 = L"", const std::wstring& arg5 = L"", const std::wstring& arg6 = L"", const std::wstring& arg7 = L"", const std::wstring& arg8 = L"", const std::wstring& arg9 = L"");
				// Strings often need to be assembled at runtime. This poses a problem for
				// localization since the parts of the final string may need to appear in
				// different locations in some languages. These functions address this
				// problem through the use of a format string. For example, "The folder #1 
				// contains #2 files for #3 lines." will use arg1 through arg3. When the
				// string is localized "#1", "#2", and "#3" may appear in a different order.

CORE_EXPORT std::wstring LoadAppString(const std::wstring& format, const std::wstring& arg1);
CORE_EXPORT std::wstring LoadAppString(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2);
CORE_EXPORT std::wstring LoadAppString(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2, const std::wstring& arg3, const std::wstring& arg4 = L"", const std::wstring& arg5 = L"", const std::wstring& arg6 = L"", const std::wstring& arg7 = L"", const std::wstring& arg8 = L"", const std::wstring& arg9 = L"");
				// We overload the most common versions for better code generation.
					
		
// ===================================================================================
//	Character Properties			
// ===================================================================================
CORE_EXPORT bool CheckChar(wchar_t ch, const wchar_t* table);
				// Helper function used within Whisper. Client code should not have to call this.

inline bool 	IsControl(wchar_t ch)							{return !CheckChar(ch, kControlTable);}
				// Returns true for special non-printing characters. This includes characters
				// in [0x0000, 0x001F] (the standard ASCII control codes), [0x007F, 0x009F]
				// (delete and unnamed control codes), and a sprinkling of others (eg 0x200C
				// "ZERO WIDTH NON-JOINER").

CORE_EXPORT bool IsWhiteSpace(wchar_t ch);
CORE_EXPORT bool IsWhiteSpace(const std::wstring& text);
				// Returns true for characters that are used only for spacing. This includes 
				// "SPACE", "NO-BREAK SPACE" and some wackier characters like "THIN SPACE", 
				// "HAIR SPACE", and "ZERO WIDTH SPACE". It also includes the control codes 
				// "HORIZONTAL TABULATION", "LINE FEED", "CARRIAGE RETURN", "FORM FEED", and 
				// "VERTICAL TABULATION".

inline bool 	IsLetter(wchar_t ch)							{return CheckChar(ch, kLetterTable);}
				// Returns true for characters used to compose words. This includes both
				// alphabetical and ideographic characters, but does not include modifier
				// letters, eg "CIRCUMFLEX ACCENT", "GRAVE ACCENT", etc.

CORE_EXPORT bool IsIdeographic(wchar_t ch);
				// Returns true for ideographic letters (eg Chinese and Japanese characters).

inline bool 	IsAlphabetical(wchar_t ch)					{return IsLetter(ch) && !IsIdeographic(ch);}
				// Returns true for non-ideographic letters.

inline bool 	IsUpperCase(wchar_t ch)						{return CheckChar(ch, kUpperCaseTable);}
inline bool 	IsLowerCase(wchar_t ch)						{return CheckChar(ch, kLowerCaseTable);}
				// Case is used by the Latin, Greek, Cyrillic, and Armenian characters. Note
				// that this includes composite characters. For example, "LATIN SMALL LETTER A WITH TILDE"
				// is a lower case character.

inline bool	 	IsTitleCase(wchar_t ch)						{return CheckChar(ch, kTitleCaseTable);}
				// Returns true for a handful of characters intended to be used at the start
				// of words, eg "LATIN CAPITAL LETTER L WITH SMALL LETTER J".
			
inline bool 	IsNumeric(wchar_t ch)							{return CheckChar(ch, kNumericTable);}
				// Returns true for characters used to represent numbers. This includes the
				// standard Arabic digits (which are widely used, even in Asia), fractions,
				// subscripts, superscripts, Roman numerals, encircled numbers, etc.

inline bool 	IsDigit(wchar_t ch)							{return CheckChar(ch, kDigitTable);}
				// Returns true for decimal digits which are numeric characters that can be
				// combined to form decimal-radix numbers. This includes Arabic digits, but
				// not subscripts, superscripts, Roman numerals, etc.

inline bool 	IsBinaryDigit(wchar_t ch)						{return ch == L'0' || ch == L'1';}
inline bool 	IsArabicDigit(wchar_t ch)						{return ch >= L'0' && ch <= L'9';}
inline bool 	IsHexDigit(wchar_t ch)							{return (ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'f') || (ch >= L'A' && ch <= L'F');}

inline bool 	IsPunctuation(wchar_t ch)						{return CheckChar(ch, kPunctuationTable);}
				// Returns true for "EXCLAMATION MARK", "QUOTATION MARK", "PERCENT SIGN",
				// "APOSTROPHE", "LEFT PARENTHESIS", "COMMA", "COLON","SEMICOLON", "QUESTION MARK", 
				// "LEFT SQUARE BRACKET", "LEFT CURLY BRACKET", "SOFT HYPHEN", "INVERTED QUESTION MARK", 
				// "HYPHEN", "NON-BREAKING HYPHEN", "LEFT SINGLE QUOTATION MARK", "BULLET", 
				// "LEFT ANGLE BRACKET", etc.

inline bool 	IsSymbol(wchar_t ch)							{return CheckChar(ch, kSymbolTable);}
				// Returns true for "NUMBER SIGN", "AMPERSAND", "ASTERISK", "COMMERCIAL AT", 
				// "VERTICAL LINE", "TILDE", "COPYRIGHT SIGN", "DEGREE CELSIUS", "DEGREE FAHRENHEIT", 
				// "LEFTWARDS ARROW", "HOURGLASS", "SYMBOL FOR LINE FEED", "BOX DRAWINGS LIGHT VERTICAL", 
				// "BLACK TELEPHONE", "SAGITTARIUS", "MUSIC SHARP SIGN", etc.

inline bool 	IsMathSymbol(wchar_t ch)						{return CheckChar(ch, kMathSymbolTable);}
				// Returns true for "PLUS SIGN", "LESS-THAN SIGN", "EQUALS SIGN", "GREATER-THAN SIGN",
				// "NOT SIGN", "PLUS-MINUS SIGN", "MULTIPLICATION SIGN", "FRACTION SLASH", 
				// "SUPERSCRIPT PLUS SIGN", "SUBSCRIPT MINUS", "FOR ALL", "COMPLEMENT", etc.
			
inline bool 	IsCurrencySymbol(wchar_t ch)					{return CheckChar(ch, kCurrencySymbolTable);}
				// Returns true for "DOLLAR SIGN", "CENT SIGN", "POUND SIGN", "YEN SIGN", etc.
	
inline bool 	IsCombining(wchar_t ch)							{return CheckChar(ch, kCombiningTable);}
				// Returns true for characters that modify the prior non-combining character
				// For example, Unicode includes a composed character called "LATIN CAPITAL 
				// LETTER A WITH CIRCUMFLEX" that can also be represented by the character pair:
				// "LATIN CAPITAL LETTER A" followed by "COMBINING CIRCUMFLEX ACCENT".

inline bool 	IsPrivate(wchar_t ch)							{return CheckChar(ch, kPrivateTable);}
				// Returns true for character codes inside the Private Use area. (This
				// includes the Corporate Use Zone which is used, for example, by Apple for
				// characters like the Apple logo).

inline bool 	IsValid(wchar_t ch)								{return !CheckChar(ch, kValidTable);}
				// Returns true if ch is an assigned Unicode 2.0 character code.
			
inline bool 	IsPrintable(wchar_t ch)							{return IsValid(ch) && !IsControl(ch);}


// ===================================================================================
//	Disassembly
// ===================================================================================
CORE_EXPORT std::wstring Before(const std::wstring& str, const std::wstring& subStr);
				// Returns the string before subStr or an empty string if subStr isn't found.

CORE_EXPORT std::wstring After(const std::wstring& str, const std::wstring& subStr);
				// Returns the string after subStr or an empty string if subStr isn't found.

CORE_EXPORT std::wstring Parse(std::wstring& str, const std::wstring& term);
				// Removes and returns next word in str. Word is char run not in term. 
				// When the last word is returned str will be empty.
			

// ===================================================================================
//	Mutators
// ===================================================================================
CORE_EXPORT std::wstring ConvertToUpperCase(const std::wstring& lowerStr);
CORE_EXPORT std::wstring ConvertToLowerCase(const std::wstring& upperStr);
				// Note that the output may be of different size than the input. Also the
				// Unicode standard recommends using ConvertToLowerCase for tasks like loose
				// comparisons and indexing.
			
CORE_EXPORT wchar_t ConvertToUpperCase(wchar_t ch);
CORE_EXPORT wchar_t ConvertToLowerCase(wchar_t ch);
				// Note that it's better to use the string versions where possible. (For 
				// example, ConvertToUpperCase(wchar_t) can't properly convert the German 
				// sharp S character).

CORE_EXPORT std::wstring StripLeading(const std::wstring& str, const std::wstring& padding = L" ");
				// Removes characters in pad from the start of str.

CORE_EXPORT std::wstring StripTrailing(const std::wstring& str, const std::wstring& padding = L" ");
				// Removes characters in pad from the end of str.

CORE_EXPORT std::wstring Replace(const std::wstring& str, wchar_t oldChar, wchar_t newChar);
CORE_EXPORT std::wstring Replace(const std::wstring& str, const std::wstring& oldText, const std::wstring& newText);

CORE_EXPORT std::wstring Replace(const std::wstring& str, wchar_t oldChar, wchar_t new1, wchar_t new2);
				// This one's a bit weird: it replaces two adjacent copies of oldChar with 
				// new2 and replaces single copies of oldChar with new1. If new1 or new2 is
				// zero oldChar is removed. (This is used on the Mac side to strip out lone
				// ampersands and convert two amspersands into a single ampersand).

CORE_EXPORT std::wstring Remove(const std::wstring& str, wchar_t ch);
CORE_EXPORT std::wstring Remove(const std::wstring& str, const std::wstring& badChars);
				// Removes all instances of ch/badChars from str.


CORE_EXPORT std::wstring FormatAsXML(const std::wstring& str);
				// Replaces bad XML characters with the appropiate character reference
				// (eg '<' is replaced with "&lt;").

CORE_EXPORT std::wstring FormatAsHTML(const std::wstring& str);
				// Like FormatAsXML except "'" isn't converted. 


// ===================================================================================
//	Misc
// ===================================================================================
CORE_EXPORT int CompareStrings(const std::wstring& lhs, const std::wstring& rhs);
				// Returns -1 if lhs < rhs, 0 if lhs == rhs, and +1 if lhs > rhs.
				// Unlike the string relational operators this accounts for the
				// current locale.
			
inline bool 	LesserString(const std::wstring& lhs, const std::wstring& rhs)		{return CompareStrings(lhs, rhs) == -1;}
inline bool 	GreaterString(const std::wstring& lhs, const std::wstring& rhs)		{return CompareStrings(lhs, rhs) == +1;}
				// Helper functions for use with STL.
			
CORE_EXPORT uint32 Count(const std::wstring& str, wchar_t ch);
				// Returns the number of instances of ch in str.
			
CORE_EXPORT std::wstring IDToStr(uint32 type);
CORE_EXPORT uint32 	StrToID(const std::wstring& str);	
				// Converts four character codes to and from strings.

#if __MWERKS__
CORE_EXPORT int stricmp(const char* lhs, const char* rhs);
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
