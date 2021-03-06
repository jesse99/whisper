/*
 *  File:       XStringUtils.cpp
 *  Summary:   	Misc string utilities
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XStringUtils.cpp,v $
 *		Revision 1.7  2001/04/27 09:20:58  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.6  2001/04/27 04:36:50  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.5  2001/04/13 08:04:07  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.4  2001/03/05 04:40:52  jesjones
 *		Added FormatAsHTML.
 *		
 *		Revision 1.3  2000/12/10 04:12:03  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:53:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <19>	10/31/00	JDJ		Added FormatAsXML.
 *		 <18>	 7/21/00	JDJ		Fixed StrToID so that it properly adds spaces to pad out to four characters.
 *		 <17>	 7/19/00	JDJ		Seperate string table loaders are now used for Whisper and app strings.
 *									SetLocale clears the tables before trying to load the new strings.
 *		 <16>	 4/20/00	JDJ		Added LoadWhisperString and LoadAppString overloads.
 *		 <15>	10/02/99	JDJ		Renamed WhisperStrings.xml XStrings.xml and AppStrings.xml CStrings.xml.
 *		 <14>	 8/06/99	JDJ		Added stricmp (MetroWerks).
 *		 <13>	 8/04/99	JDJ		Added Replace (XString, XChar, XChar, XChar).
 *		 <12>	 7/23/99	JDJ		ConvertToLowerCase, ConvertToUpperCase, and CompareStrings use
 *									the CFString routines in Carbon.
 *		 <11>	 6/21/99	JDJ		Added __MSL_FIX_ITERATORS__ (for Pro 3).
 *		 <10>	 5/28/99	JDJ		Made thread safe.
 *		 <9>	 4/07/99	JDJ		Renamed Strip(XString, XString) StripLeading. Added Remove functions.
 *		 <8>	 4/06/99	JDJ		Renamed StrToUpperStr ConvertToUpperCase. Renamed 
 *									StrToLowerStr ConvertToUpperCase. Added overloads that 
 *									take XChar's.
 *		 <7>	 2/16/99	JDJ		Added IDToStr and StrToID. 
 *		 <6>	 2/13/99	JDJ		StrToUpperStr and StrToLowerStr handle Unicode characters. 
 *		 <5>	 2/13/99	JDJ		Added IsWhiteSpace(XString). 
 *		 <4>	 2/05/99	JDJ		Nuked IsAlpha, IsAlphaNum, IsLower, and IsUpper. Added CheckChar
 *									IsWhiteSpace, and IsIdeographic.
 *		 <3>	 1/27/99	JDJ		Changed Replace (string, char, char) to Replace (string, XChar, XChar).
 *		 <2>	 1/26/99	JDJ		StrToUpperStr and StrToLowerStr use Unicode functions on NT.
 *									Added IsAlpha, IsAlphaNum, IsDigit, IsLower, and IsUpper which
 *									also use Unicode functions on NT.
 *		 <1>	11/20/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XStringUtils.h>

#include <cctype>
#include <map>

#include <XConstants.h>
#include <XCriticalSection.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XNumbers.h>
#include <XTinyVector.h>

#if WIN
	#include <WSystemInfo.h>
#endif

namespace Whisper {


//-----------------------------------
//	Variables
//
static XLocale 				sLocale;
static XCriticalSection		sLocaleSection;

static StringTable			sWhisperStrings;
static StringTable			sAppStrings;

static StringTableLoader 	sWhisperLoader;	
static StringTableLoader 	sAppLoader;		



// ===================================================================================
//	Locales
// ===================================================================================	

//---------------------------------------------------------------
//
// GetLocale
//
//---------------------------------------------------------------
XLocale GetLocale()
{
	XLocale locale;
	
	{
	XEnterCriticalSection enter(sLocaleSection);
		locale = sLocale;
	}
	
	return locale;
}


//---------------------------------------------------------------
//
// SetLocale
//
//---------------------------------------------------------------
void SetLocale(const XLocale& locale)
{
	XEnterCriticalSection enter(sLocaleSection);

	if (locale != sLocale) {
		sLocale = locale;
		
		sWhisperStrings.clear();
		sAppStrings.clear();
	
		if (sWhisperLoader != nil) 
			sWhisperLoader(sLocale, sWhisperStrings);

		if (sAppLoader != nil)
			sAppLoader(sLocale, sAppStrings);
	}
}


// ===================================================================================
//	Loading
// ===================================================================================

//---------------------------------------------------------------
//
// InstallWhisperLoader
//
// Note that we're assuming the the string table loader will never
// be changed while other threads may make use of it. If this turns
// out to be a problem we'll have to use mutexes to avoid deadlocks.
//
//---------------------------------------------------------------
void InstallWhisperLoader(StringTableLoader loader)
{
	XEnterCriticalSection enter(sLocaleSection);

	if (loader != sWhisperLoader) {
		sWhisperLoader = loader;
	
		if (sWhisperLoader != nil)
			sWhisperLoader(sLocale, sWhisperStrings);
	}
}

			
//---------------------------------------------------------------
//
// InstallAppLoader
//
//---------------------------------------------------------------
void InstallAppLoader(StringTableLoader loader)
{
	XEnterCriticalSection enter(sLocaleSection);

	if (loader != sAppLoader) {
		sAppLoader = loader;
	
		if (sAppLoader != nil)
			sAppLoader(sLocale, sAppStrings);
	}
}

			
//---------------------------------------------------------------
//
// LoadWhisperString (wstring)
//
//---------------------------------------------------------------
std::wstring LoadWhisperString(const std::wstring& str)
{
	std::wstring text;
	
	StringTable::iterator iter = sWhisperStrings.find(str);
	if (iter != sWhisperStrings.end())
		text = iter->second;
	else
		text = str;
		
#if DEBUG
	XLocale defaultLocale;
	
	std::wstring region = defaultLocale.GetRegion();
	std::reverse(region.begin(), region.end());
	defaultLocale.SetRegion(region);
	
	XEnterCriticalSection enter(sLocaleSection);

	if (sLocale == defaultLocale)
		std::reverse(text.begin(), text.end());
#endif
		
	return text;
}


//---------------------------------------------------------------
//
// LoadAppString (wstring)
//
//---------------------------------------------------------------
std::wstring LoadAppString(const std::wstring& str)
{
	std::wstring text;
	
	StringTable::iterator iter = sAppStrings.find(str);
	if (iter != sAppStrings.end())
		text = iter->second;
	else
		text = str;
	
#if DEBUG
	XLocale defaultLocale;
	
	std::wstring region = defaultLocale.GetRegion();
	std::reverse(region.begin(), region.end());
	defaultLocale.SetRegion(region);
	
	XEnterCriticalSection enter(sLocaleSection);

	if (sLocale == defaultLocale)
		std::reverse(text.begin(), text.end());
#endif
		
	return text;
}


//---------------------------------------------------------------
//
// LoadWhisperString (wstring, wstring)
//
//---------------------------------------------------------------
std::wstring LoadWhisperString(const std::wstring& format, const std::wstring& arg1)
{	
	std::wstring outStr = LoadWhisperString(format);
	
	uint32 index = outStr.find(L"#1");
	outStr = outStr.replace(index, 2, arg1);
	
	return outStr;
}


//---------------------------------------------------------------
//
// LoadWhisperString (wstring, wstring, wstring)
//
//---------------------------------------------------------------
std::wstring LoadWhisperString(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2)
{	
	std::wstring outStr = LoadWhisperString(format);
	
	uint32 index = outStr.find(L"#1");
	outStr = outStr.replace(index, 2, arg1);

	index = outStr.find(L"#2");
	if (index != std::wstring::npos)
		outStr = outStr.replace(index, 2, arg2);
	
	return outStr;
}


//---------------------------------------------------------------
//
// LoadWhisperString (wstring, wstring, wstring, wstring, wstring, wstring, wstring, wstring, wstring, wstring)
//
//---------------------------------------------------------------
std::wstring LoadWhisperString(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2, const std::wstring& arg3, const std::wstring& arg4, const std::wstring& arg5, const std::wstring& arg6, const std::wstring& arg7, const std::wstring& arg8, const std::wstring& arg9)
{	
	std::wstring outStr = LoadWhisperString(format);
	
	uint32 index = outStr.find(L"#1");
	outStr = outStr.replace(index, 2, arg1);

	index = outStr.find(L"#2");
	if (index != std::wstring::npos) {
		outStr = outStr.replace(index, 2, arg2);

		index = outStr.find(L"#3");
		if (index != std::wstring::npos) {
			outStr = outStr.replace(index, 2, arg3);

			index = outStr.find(L"#4");
			if (index != std::wstring::npos) {
				outStr = outStr.replace(index, 2, arg4);
	
				index = outStr.find(L"#5");
				if (index != std::wstring::npos) {
					outStr = outStr.replace(index, 2, arg5);
		
					index = outStr.find(L"#6");
					if (index != std::wstring::npos) {
						outStr = outStr.replace(index, 2, arg6);
			
						index = outStr.find(L"#7");
						if (index != std::wstring::npos) {
							outStr = outStr.replace(index, 2, arg7);
				
							index = outStr.find(L"#8");
							if (index != std::wstring::npos) {
								outStr = outStr.replace(index, 2, arg8);
					
								index = outStr.find(L"#9");
								if (index != std::wstring::npos) 
									outStr = outStr.replace(index, 2, arg9);
							}
						}
					}
				}
			}
		}
	}
	
	return outStr;
}


//---------------------------------------------------------------
//
// LoadAppString (wstring, wstring)
//
//---------------------------------------------------------------
std::wstring LoadAppString(const std::wstring& format, const std::wstring& arg1)
{
	std::wstring outStr = LoadAppString(format);
	
	uint32 index = outStr.find(L"#1");
	outStr = outStr.replace(index, 2, arg1);

	return outStr;
}


//---------------------------------------------------------------
//
// LoadAppString (wstring, wstring, wstring)
//
//---------------------------------------------------------------
std::wstring LoadAppString(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2)
{
	std::wstring outStr = LoadAppString(format);
	
	uint32 index = outStr.find(L"#1");
	outStr = outStr.replace(index, 2, arg1);

	index = outStr.find(L"#2");
	if (index != std::wstring::npos)
		outStr = outStr.replace(index, 2, arg2);

	return outStr;
}


//---------------------------------------------------------------
//
// LoadAppString (wstring, wstring, wstring, wstring, wstring, wstring, wstring, wstring, wstring, wstring)
//
//---------------------------------------------------------------
std::wstring LoadAppString(const std::wstring& format, const std::wstring& arg1, const std::wstring& arg2, const std::wstring& arg3, const std::wstring& arg4, const std::wstring& arg5, const std::wstring& arg6, const std::wstring& arg7, const std::wstring& arg8, const std::wstring& arg9)
{
	std::wstring outStr = LoadAppString(format);
	
	uint32 index = outStr.find(L"#1");
	outStr = outStr.replace(index, 2, arg1);

	index = outStr.find(L"#2");
	if (index != std::wstring::npos) {
		outStr = outStr.replace(index, 2, arg2);

		index = outStr.find(L"#3");
		if (index != std::wstring::npos) {
			outStr = outStr.replace(index, 2, arg3);

			index = outStr.find(L"#4");
			if (index != std::wstring::npos) {
				outStr = outStr.replace(index, 2, arg4);
	
				index = outStr.find(L"#5");
				if (index != std::wstring::npos) {
					outStr = outStr.replace(index, 2, arg5);
		
					index = outStr.find(L"#6");
					if (index != std::wstring::npos) {
						outStr = outStr.replace(index, 2, arg6);
			
						index = outStr.find(L"#7");
						if (index != std::wstring::npos) {
							outStr = outStr.replace(index, 2, arg7);
				
							index = outStr.find(L"#8");
							if (index != std::wstring::npos) {
								outStr = outStr.replace(index, 2, arg8);
					
								index = outStr.find(L"#9");
								if (index != std::wstring::npos) 
									outStr = outStr.replace(index, 2, arg9);
							}
						}
					}
				}
			}
		}
	}

	return outStr;
}


// ===================================================================================
//	Disassembly
// ===================================================================================

//---------------------------------------------------------------
//
// Before
//
//---------------------------------------------------------------
std::wstring Before(const std::wstring& str, const std::wstring& sub)
{
	std::wstring result;
	
	uint32 index = str.find(sub);
	if (index != std::wstring::npos)
		result = str.substr(0, index);
	
	return result;
}


//---------------------------------------------------------------
//
// After
//
//---------------------------------------------------------------
std::wstring After(const std::wstring& str, const std::wstring& sub)
{
	std::wstring result;
	
	uint32 index = str.find(sub);
	if (index != std::wstring::npos)
		result = str.substr(index + sub.length());
	
	return result;
}


//---------------------------------------------------------------
//
// Parse
//
//---------------------------------------------------------------
std::wstring Parse(std::wstring& str, const std::wstring& term)
{
	std::wstring token;
		
	// Start at the first character not in term.
	uint32 start = str.find_first_not_of(term);
		
	// Stop at the next occurance of a character in term.
	uint32 stop = str.find_first_of(term, start + 1);
		
	// If the start is valid return the token.
	if (start != std::wstring::npos) 										
		token = str.substr(start, stop-start);
		
	// Remove the token from the string.
	if (stop != std::wstring::npos)						
		str = str.substr(stop);						
	else					
		str.resize(0);	
		
	// Remove term chars from the start of the string. (Makes it easier
	// to determine when parsing is complete).
	str = StripLeading(str, term);
										
	return token;
}


// ===================================================================================
//	Misc
// ===================================================================================

//---------------------------------------------------------------
//
// ConvertToUpperCase (wstring)	
//
// Case folding is locale dependant. For example, French speakers
// normally remove diacritical marks when upper casing except for
// French Canadians who preserve them. So, we'll call the OS
// functions when we can since they take advantage of locale 	
// info. 
//
//---------------------------------------------------------------
std::wstring ConvertToUpperCase(const std::wstring& inStr)
{
	std::wstring outStr;
	bool converted = false; 

#if TARGET_CARBON && 0						// $$$ CFStringUppercase crashes with Greek and Armenian characters
	CoreString str(CFStringCreateMutableCopy(kCFAllocatorSystemDefault, 0, ToCarbonStr(inStr).Get()));
	ThrowIfNil(str.Get());
	
	CFStringUppercase(str.Get(), nil);		// $$$ second argument is (still!) To Be Decided...
	outStr = FromCarbonStr(str.Get());					
	
	converted = true;

#else
#if WIN
	if (WSystemInfo::HasUnicode() && inStr.length() > 0) {
		uint16 language = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
		LCID locale = MAKELCID(language, SORT_DEFAULT);

		uint32 flags = LCMAP_LINGUISTIC_CASING + LCMAP_UPPERCASE;	// it'd be simpler to use CharUpper, but there's at least a chance that LCMapString will properly handle characters like LATIN SMALL LETTER SHARP S

		int32 count = ::LCMapStringW(locale, flags, inStr.c_str(), (int32) inStr.length(), nil, 0);
		ThrowIf(count == 0);

		XTinyVector<wchar_t> temp(count+1UL);
		int32 succeeded = ::LCMapStringW(locale, flags, inStr.c_str(), (int32) inStr.length(), temp.buffer(), count);
		ThrowIf(!succeeded);

		outStr.assign(temp.buffer(), (uint32) count);
		converted = true;
	}
#endif

	if (!converted) {
		outStr.reserve(inStr.length());

		for (uint32 index = 0; index < inStr.length(); index++) {
			wchar_t inChar = inStr[index];
			
			if (inChar >= 'a' && inChar <= 'z')
				outStr += (wchar_t) (inChar + 'A' - 'a');
				
			else if (inChar == 0x00DF)				// LATIN SMALL LETTER SHARP S 
				outStr += L"SS";					// $$$ neither the Unicode standard nor _Programming for the World_ mention other letters like this...
				
			else if (inChar < 128)
				outStr += inChar;
				
			else {
				wchar_t outChar = inChar;
				uint32 size = kUpperCaseMapping[0].in;
				const SCharMapping* iter = std::lower_bound(kUpperCaseMapping+1, kUpperCaseMapping+1+size, inChar);
				if (iter->in == inChar)
					outChar = iter->out;	

				outStr += outChar;
			}
		}
	}
#endif

	return outStr;
}


//---------------------------------------------------------------
//
// ConvertToLowerCase (wstring)
//
//---------------------------------------------------------------
std::wstring ConvertToLowerCase(const std::wstring& inStr)
{
	std::wstring outStr;
	bool converted = false; 

#if TARGET_CARBON && 0						// $$$ CFStringLowercase crashes with Greek and Armenian characters
	CoreString str(CFStringCreateMutableCopy(kCFAllocatorSystemDefault, 0, ToCarbonStr(inStr).Get()));
	ThrowIfNil(str.Get());
	
	CFStringLowercase(str.Get(), nil);		// $$$ second argument is (still!) To Be Decided...
	outStr = FromCarbonStr(str);					
		
	converted = true;

#else
#if WIN
	if (WSystemInfo::HasUnicode()) {
		outStr = inStr;

		(void) ::CharLowerBuffW(outStr.begin(), outStr.length());
		converted = true;
	}
#endif

	if (!converted) {
		outStr.reserve(inStr.length());

		for (uint32 index = 0; index < inStr.length(); index++) {
			wchar_t inChar = inStr[index];
			wchar_t outChar = inChar;
			
			if (inChar >= 'A' && inChar <= 'Z')
				outChar = (wchar_t) (inChar + 'a' - 'A');
				
			else if (inChar > 127) {
				uint32 size = kLowerCaseMapping[0].in;
				const SCharMapping* iter = std::lower_bound(kLowerCaseMapping+1, kLowerCaseMapping+1+size, inChar);
				if (iter->in == inChar)
					outChar = iter->out;	
			}
			
			outStr += outChar;
		}
	}
#endif

	return outStr;
}


//---------------------------------------------------------------
//
// ConvertToUpperCase (wchar_t)	
//
//---------------------------------------------------------------
wchar_t ConvertToUpperCase(wchar_t inChar)
{
	wchar_t outChar = inChar;

	if (inChar >= 'a' && inChar <= 'z')
		outChar = (wchar_t) (inChar + 'A' - 'a');
		
	else if (inChar > 127) {
		uint32 size = kUpperCaseMapping[0].in;
		const SCharMapping* iter = std::lower_bound(kUpperCaseMapping+1, kUpperCaseMapping+1+size, inChar);
		if (iter->in == inChar)
			outChar = iter->out;	
	}

	return outChar;
}


//---------------------------------------------------------------
//
// ConvertToLowerCase (wchar_t)
//
//---------------------------------------------------------------
wchar_t ConvertToLowerCase(wchar_t inChar)
{
	wchar_t outChar = inChar;
	
	if (inChar >= 'A' && inChar <= 'Z')
		outChar = (wchar_t) (inChar + 'a' - 'A');
		
	else if (inChar > 127) {
		uint32 size = kLowerCaseMapping[0].in;
		const SCharMapping* iter = std::lower_bound(kLowerCaseMapping+1, kLowerCaseMapping+1+size, inChar);
		if (iter->in == inChar)
			outChar = iter->out;	
	}
		
	return outChar;
}


//---------------------------------------------------------------
//
// CheckChar
//
//---------------------------------------------------------------
bool CheckChar(wchar_t ch, const wchar_t* table)
{
	PRECONDITION(table != nil);		
	
	int32 size = *table;
	
	const wchar_t* iter = std::lower_bound(table+1, table+1+size, ch);
	int32 index = iter - (table+1);
	
	bool is = (bool) (index & 1);		// assume first entry is false (if it's not our caller will complement the result)
	
	return is;
}


//---------------------------------------------------------------
//
// IsIdeographic
//
//---------------------------------------------------------------
bool IsIdeographic(wchar_t ch)
{
	bool is = false;
	
	if (ch >= 0x4E00 && ch <= 0x9FA5)
		is = true;
		
	else if (ch >= 0xF900 && ch <= 0xFA2D)
		is = true;

	else if (ch == 0x3007)
		is = true;
		
	else if (ch >= 0x3021 && ch <= 0x3029)
		is = true;

	return is;
}


//---------------------------------------------------------------
//
// IsWhiteSpace (wchar_t)
//
//---------------------------------------------------------------
bool IsWhiteSpace(wchar_t ch)
{
	bool is;
	
	switch (ch) {
		case 0x0009:		// HORIZONTAL TABULATION
		case 0x000A:		// LINE FEED
		case 0x000B:		// VERTICAL TABULATION
		case 0x000C:		// FORM FEED
		case 0x000D:		// CARRIAGE RETURN
			is = true;
			break;
			
		default:
			is = CheckChar(ch, kWhiteSpaceTable);
	}
	
	return is;
}


//---------------------------------------------------------------
//
// IsWhiteSpace (wstring)
//
//---------------------------------------------------------------
bool IsWhiteSpace(const std::wstring& text)
{
	bool is = text.length() > 0;
	
	for (uint32 index = 0; index < text.length() && is; ++index)
		is = IsWhiteSpace(text[index]);
	
	return is;
}


//---------------------------------------------------------------
//
// CompareStrings
//
//---------------------------------------------------------------
int CompareStrings(const std::wstring& lhs, const std::wstring& rhs)	// $$$ might want to add some options, especially when require Carbon
{	
#if TARGET_CARBON 
	CFOptionFlags flags = kCFCompareLocalized;	// use user's default locale
	int result = ::CFStringCompare(ToCarbonStr(lhs), ToCarbonStr(rhs), flags);

#elif MAC
	int result = ::CompareText(ToPlatformStr(lhs), ToPlatformStr(rhs), numeric_cast<int16>(lhs.length()), numeric_cast<int16>(rhs.length()), nil);
	
#elif WIN
	int result;
	if (WSystemInfo::HasUnicode())
		result = ::CompareStringW(LOCALE_USER_DEFAULT, 0, lhs.c_str(), (int32) lhs.length(), rhs.c_str(), (int32) rhs.length());
	else
		result = ::CompareStringA(LOCALE_USER_DEFAULT, 0, ToPlatformStr(lhs).c_str(), (int32) lhs.length(), ToPlatformStr(rhs).c_str(), (int32) rhs.length());
	ThrowIf(result < 1 || result > 3);
	
	result -= 2;
#endif

	return result; 
}


//---------------------------------------------------------------
//
// StripLeading
//
//---------------------------------------------------------------
std::wstring StripLeading(const std::wstring& str, const std::wstring& padding)
{
	uint32 index = 0;
	while (index < str.length() && padding.find(str[index]) != std::wstring::npos)
		index++;
		
	return str.substr(index);
}


//---------------------------------------------------------------
//
// StripTrailing
//
//---------------------------------------------------------------
std::wstring StripTrailing(const std::wstring& str, const std::wstring& padding)
{
	uint32 index = 0;

	if (!str.empty()) {
		index = str.size();
		while (index > 1 && padding.find(str[index-1]) != std::wstring::npos)
			index--;
	}
			
	return str.substr(0, index);
}


//---------------------------------------------------------------
//
// Replace (wstring, wchar_t, wchar_t)
//
//---------------------------------------------------------------
std::wstring Replace(const std::wstring& inStr, wchar_t oldCh, wchar_t newCh)
{
	std::wstring str = inStr;

	for (uint32 i = 0; i < str.length(); i++)
		if (str[i] == oldCh)
			str[i] = newCh;
	
	return str;
}


//---------------------------------------------------------------
//
// Replace (wstring, wstring, wstring)
//
//---------------------------------------------------------------
std::wstring Replace(const std::wstring& inStr, const std::wstring& oldText, const std::wstring& newText)
{
	std::wstring str = inStr;
	
	uint32 index = 0;
	while (true) {
		index = str.find(oldText, index);
		if (index == std::wstring::npos) 
			break;
		
		str.replace(index, oldText.length(), newText, 0, newText.length());
		index += newText.length();
	}
		
	return str;
}


//---------------------------------------------------------------
//
// Replace (wstring, wchar_t, wchar_t, wchar_t)						
//
//---------------------------------------------------------------
std::wstring Replace(const std::wstring& inName, wchar_t oldChar, wchar_t new1, wchar_t new2)
{
	std::wstring name = inName;
	
	uint32 i = 0;
	while (i < name.length()) {
		if (name[i] == oldChar) {
			if (i + 1 < name.length() && name[i + 1] == oldChar) {
				if (new2 == '\0')			// if we've found two oldChar's then
					name.erase(i, 2);		// remove them both if new2 is zero
				else {
					name[i++] = new2;		// otherwise replace them with new2
					name.erase(i, 1);		
				}
			
			} else {						// if we've found one oldChar then
				if (new1 == '\0')
					name.erase(i, 1);		// remove it if new1 is zero
				else
					name[i++] = new1;		// otherwise replace it with new1
			}
			
		} else
			++i;
	}
	
	return name;
}


//---------------------------------------------------------------
//
// Remove (wstring, wchar_t)
//
//---------------------------------------------------------------
std::wstring Remove(const std::wstring& inStr, wchar_t ch)
{
	std::wstring str;
	str.reserve(inStr.length());

	for (uint32 i = 0; i < inStr.length(); ++i)
		if (inStr[i] != ch)
			str += inStr[i];
	
	return str;
}


//---------------------------------------------------------------
//
// Remove (wstring, wstring)
//
//---------------------------------------------------------------
std::wstring Remove(const std::wstring& inStr, const std::wstring& badChars)
{
	std::wstring str;
	str.reserve(inStr.length());

	for (uint32 i = 0; i < inStr.length(); ++i)
		if (badChars.find(inStr[i]) == std::wstring::npos)
			str += inStr[i];
	
	return str;
}


//---------------------------------------------------------------
//
// FormatAsXML
//
//---------------------------------------------------------------
std::wstring FormatAsXML(const std::wstring& inText)
{
	std::wstring text;
	text.reserve(inText.length());
	
	for (uint32 i = 0; i < inText.length(); ++i) {
		wchar_t ch = inText[i];
		
		switch (ch) {
			case '<':
				text += L"&lt;";
				break;
				
			case '>':
				text += L"&gt;";
				break;
				
			case '&':
				text += L"&amp;";
				break;
				
			case '\'':
				text += L"&apos;";
				break;
				
			case '"':
				text += L"&quot;";
				break;
				
			default:
				text += ch;
		}
	}
	
	return text;
}


//---------------------------------------------------------------
//
// FormatAsHTML
//
//---------------------------------------------------------------
std::wstring FormatAsHTML(const std::wstring& inText)
{
	std::wstring text;
	text.reserve(inText.length());
	
	for (uint32 i = 0; i < inText.length(); ++i) {
		wchar_t ch = inText[i];
		
		switch (ch) {
			case '<':
				text += L"&lt;";
				break;
				
			case '>':
				text += L"&gt;";
				break;
				
			case '&':
				text += L"&amp;";
				break;
				
			case '"':
				text += L"&quot;";
				break;
				
			default:
				text += ch;
		}
	}
	
	return text;
}


//---------------------------------------------------------------
//
// Count
//
//---------------------------------------------------------------
uint32 Count(const std::wstring& str, wchar_t ch)
{
	uint32 count = 0;

	for (uint32 i = 0; i < str.length(); i++)
		if (str[i] == ch)
			++count;
	
	return count;
}


//---------------------------------------------------------------
//
// IDToStr
//
//---------------------------------------------------------------
std::wstring IDToStr(uint32 id)
{
	std::wstring str;
	
	uint32 n = id & 0xFF000000;
	if (n != 0)
		str += (wchar_t) (n >> 24);
	else
		str += L' ';
	
	n = id & 0x00FF0000;
	if (n != 0)
		str += (wchar_t) (n >> 16);
	else
		str += L' ';
	
	n = id & 0x0000FF00;
	if (n != 0)
		str += (wchar_t) (n >> 8);
	else
		str += L' ';
	
	n = id & 0x000000FF;
	if (n != 0)
		str += (wchar_t) n;
	else
		str += L' ';

	return str;
}


//---------------------------------------------------------------
//
// StrToID
//
//---------------------------------------------------------------
uint32 StrToID(const std::wstring& str)
{
	PRECONDITION(str.length() <= 4);
	
	uint32 id = 0;
	
	for (uint32 index = 0; index < str.length(); ++index) {
		uint32 n = str[index];
		ASSERT(n < 256);
		
		id = (id << 8) | n;
	}
		
	for (uint32 index = str.length(); index < 4; ++index)
		id = (id << 8) | ' ';
		
	return id;
}


//---------------------------------------------------------------
//
// stricmp
//
//---------------------------------------------------------------
#if __MWERKS__
int stricmp(const char* lhs, const char* rhs)
{
	PRECONDITION(lhs != nil);
	PRECONDITION(rhs != nil);

	const unsigned char* p1 = (unsigned char*) lhs - 1;		// from CW 5 MSL
	const unsigned char* p2 = (unsigned char*) rhs - 1;
	int c1, c2;
		
	while ((c1 = std::tolower(*++p1)) == (c2 = std::tolower(*++p2)))
		if (!c1)
			return 0;

	return c1 - c2;
}
#endif


}	// namespace Whisper


