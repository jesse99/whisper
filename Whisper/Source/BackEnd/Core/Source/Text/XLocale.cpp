/*
 *  File:		XLocale.cpp
 *  Summary:	Encapsulates a region and a language (eg US English).
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XLocale.cpp,v $
 *		Revision 1.4  2001/04/27 04:34:10  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2000/12/10 04:10:23  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:42:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XLocale.h>	

#include <XNumbers.h>	

namespace Whisper {


// ========================================================================================
//	class XLocale
// ========================================================================================

//---------------------------------------------------------------
//
// XLocale::XLocale ()
//
//---------------------------------------------------------------
XLocale::XLocale()
{
#if MAC
	int32 locale = ::GetScriptManagerVariable(smRegionCode);
#elif WIN
	LCID locale = ::GetUserDefaultLCID();
#endif

	mLanguage = this->DoGetLanguage(locale);
	mRegion   = this->DoGetRegion(locale);

//	mLanguage = L"German";
//	mRegion   = L"Germany";
}


//---------------------------------------------------------------
//
// XLocale::XLocale (OSLocale)
//
//---------------------------------------------------------------
XLocale::XLocale(OSLocale locale)
{
	mLanguage = this->DoGetLanguage(locale);
	mRegion   = this->DoGetRegion(locale);
}


//---------------------------------------------------------------
//
// XLocale::XLocale (wstring, wstring)
//
//---------------------------------------------------------------
XLocale::XLocale(const std::wstring& language, const std::wstring& region) : mLanguage(language), mRegion(region)
{
	PRECONDITION(language.length() > 0);		// note that region may be empty
}


//---------------------------------------------------------------
//
// XLocale::DoGetLanguage			
//
//---------------------------------------------------------------
std::wstring XLocale::DoGetLanguage(OSLocale locale)
{
	std::wstring language;
	
#if MAC
	switch (locale) {
		case verAfrikaans:
			language = L"Afrikaans";
			break;
		
		case verArabic:
			language = L"Arabic";
			break;
		
		case verArmenian:
			language = L"Armenian";
			break;
		
		case verBengali:
			language = L"Bangladesh";
			break;
		
		case verBulgaria:
			language = L"Bulgarian";
			break;
		
		case verByeloRussian:
			language = L"Byelorussian";
			break;
		
		case verCatalonia:
			language = L"Catalan";
			break;
		
		case verGreecePoly:
			language = L"Classical Greek";
			break;
		
		case verCroatia:
			language = L"Croatian";
			break;
		
		case verCzech:
			language = L"Czech";
			break;
		
		case verDenmark:
		case vervariantDenmark:
			language = L"Danish";
			break;
		
		case verNetherlands:
		case verNetherlandsComma:
			language = L"Dutch";
			break;
		
		case verBhutan:
			language = L"Dzongkha";
			break;
		
		case verUS:
		case verBritain:
		case verAustralia:
		case verInternational:
		case verEngCanada:
			language = L"English";
			break;
		
		case verEstonia:
			language = L"Estonian";
			break;
		
		case verFinland:
			language = L"Finnish";
			break;
		
		case verFrance:
		case verFrCanada:
		case verFrSwiss:
		case verFrenchUniversal:
		case verFrBelgium:
			language = L"French";
			break;
		
		case verGeorgian:
			language = L"Georgian";
			break;
		
		case verGermany:
		case verGrSwiss:
		case verAustria:
			language = L"German";
			break;
		
		case verGreece:
			language = L"Greek";
			break;
		
		case verIsrael:
			language = L"Hebrew";
			break;
		
		case verIndiaHindi:
			language = L"Hindi";
			break;
		
		case verHungary:
			language = L"Hungarian";
			break;
		
		case verIceland:
			language = L"Icelandic";
			break;
		
		case verIreland:
			language = L"Irish";
			break;
		
		case verIrishGaelicScript:
			language = L"Irish Gaelic";
			break;
		
		case verItaly:
		case verItalianSwiss:
			language = L"Italian";
			break;
		
		case verJapan:
			language = L"Japanese";
			break;
		
		case verKorea:
			language = L"Korean";
			break;
		
		case verLithuania:
			language = L"Lithuanian";
			break;
		
		case verPortugal:
			language = L"Portuguese";
			break;
		
		case verNorway:
		case verNynorsk:
			language = L"Norwegian";
			break;
		
		case verPunjabi:
			language = L"Pakistani";
			break;
		
		case verIran:
			language = L"Persian";
			break;
		
		case verPoland:
			language = L"Polish";
			break;
		
		case verBrazil:
			language = L"Portuguese";
			break;
		
		case verRomania:
			language = L"Romanian";
			break;
		
		case verRussia:
			language = L"Russian";
			break;
		
		case verScottishGaelic:
			language = L"Scottish Gaelic";
			break;
		
		case verSerbian:
			language = L"Serbian";
			break;
		
		case verChina:
			language = L"Simplified Chinese";
			break;
		
		case verSlovak:
			language = L"Slovakian";
			break;
		
		case verSlovenian:
			language = L"Slovenian";
			break;
		
		case verSpain:
		case verSpLatinAmerica:
			language = L"Spanish";
			break;
		
		case verSweden:
			language = L"Swedish";
			break;
		
		case verThailand:
			language = L"Thai";
			break;
		
		case verTibetan:
			language = L"Tibetan";
			break;
		
		case verTaiwan:
			language = L"Traditional Chinese";
			break;
		
		case verTurkey:
		case verTurkishModified:
			language = L"Turkish";
			break;
		
		case verUkraine:
			language = L"Ukrainian";
			break;
		
		case verPakistanUrdu:
		case verIndiaUrdu:
			language = L"Urdu";
			break;
		
		case verVietnam:
			language = L"Vietnamese";
			break;
		
		case verWelsh:
			language = L"Welsh";
			break;
		
		default:
			DEBUGSTR("XLocale::DoGetLanguage couldn't find the locale");
			language = L"English";
	}

#elif WIN
	uint16 primary   = numeric_cast<uint16>(PRIMARYLANGID(locale));
	uint16 secondary = numeric_cast<uint16>(SUBLANGID(locale));

	switch (primary) {
		case LANG_AFRIKAANS:
			language = L"Afrikaans";
			break;
		
		case LANG_ALBANIAN:
			language = L"Albanian";
			break;
	
		case LANG_ARABIC:
			language = L"Arabic";
			break;
	
		case LANG_BASQUE:
			language = L"Basque";
			break;
	
		case LANG_BELARUSIAN:
			language = L"Byelorussian";
			break;
	
		case LANG_BULGARIAN:
			language = L"Bulgarian";
			break;
	
		case LANG_CATALAN:
			language = L"Catalan";
			break;
	
		case LANG_CHINESE:
			if (secondary == SUBLANG_CHINESE_SIMPLIFIED)
				language = L"Simplified Chinese";
			else if (secondary == SUBLANG_CHINESE_TRADITIONAL)
				language = L"Traditional Chinese";
			else
				language = L"Chinese";
			break;
	
		case LANG_CROATIAN:
			language = L"Croatian";
			break;
	
		case LANG_CZECH:
			language = L"Czech";
			break;
	
		case LANG_DANISH:
			language = L"Danish";
			break;
	
		case LANG_DUTCH:
			language = L"Dutch";
			break;
	
		case LANG_ENGLISH:
			language = L"English";
			break;
	
		case LANG_ESTONIAN:
			language = L"Estonian";
			break;
	
		case LANG_FARSI:
			language = L"Farsi";
			break;
	
		case LANG_FINNISH:
			language = L"Finnish";
			break;
	
		case LANG_FRENCH:
			language = L"French";
			break;
	
		case LANG_GERMAN:
			language = L"German";
			break;
	
		case LANG_GREEK:
			language = L"Greek";
			break;
	
		case LANG_HEBREW:
			language = L"Hebrew";
			break;
	
		case LANG_HUNGARIAN:
			language = L"Hungarian";
			break;
	
		case LANG_ICELANDIC:
			language = L"Icelandic";
			break;
	
		case LANG_INDONESIAN:
			language = L"Indonesian";
			break;
	
		case LANG_ITALIAN:
			language = L"Italian";
			break;
	
		case LANG_JAPANESE:
			language = L"Japanese";
			break;
	
		case LANG_KOREAN:
			language = L"Korean";
			break;
	
		case LANG_LATVIAN:
			language = L"Latvian";
			break;
	
		case LANG_LITHUANIAN:
			language = L"Lithuanian";
			break;
	
		case LANG_NORWEGIAN:
			language = L"Norwegian";
			break;
	
		case LANG_POLISH:
			language = L"Polish";
			break;
	
		case LANG_PORTUGUESE:
			language = L"Portuguese";
			break;
	
		case LANG_ROMANIAN:
			language = L"Romanian";
			break;
	
		case LANG_RUSSIAN:
			language = L"Russian";
			break;
	
		case LANG_SLOVAK:
			language = L"Slovakian";
			break;
	
		case LANG_SLOVENIAN:
			language = L"Slovenian";
			break;
	
		case LANG_SPANISH:
			language = L"Spanish";
			break;
	
		case LANG_SWEDISH:
			language = L"Swedish";
			break;
	
		case LANG_THAI:
			language = L"Thai";
			break;
	
		case LANG_TURKISH:
			language = L"Turkish";
			break;
	
		case LANG_UKRAINIAN:
			language = L"Ukrainian";
			break;
	
		case LANG_VIETNAMESE:
			language = L"Vietnamese";
			break;
			
		default:
			DEBUGSTR("XLocale::DoGetLanguage couldn't find the locale");
			language = L"English";
	}
#endif

	return language;
}


//---------------------------------------------------------------
//
// XLocale::DoGetRegion								
//
//---------------------------------------------------------------
std::wstring XLocale::DoGetRegion(OSLocale locale)
{
	std::wstring region;
	
#if MAC
	switch (locale) {
		case verArmenian:
			region = L"Armenia";
			break;
		
		case verAustralia:
			region = L"Australia";
			break;
		
		case verAustria:
			region = L"Austria";
			break;
		
		case verBengali:
			region = L"Bangladesh";
			break;
		
		case verFrBelgium:
			region = L"Belgium";
			break;
		
		case verBhutan:
			region = L"Bhutan";
			break;
		
		case verBrazil:
			region = L"Brazil";
			break;
		
		case verBritain:
			region = L"Britain";
			break;
		
		case verBulgaria:
			region = L"Bulgaria";
			break;
		
		case verByeloRussian:
			region = L"Byelorussia";
			break;
		
		case verFrCanada:
		case verEngCanada:
			region = L"Canada";
			break;
		
		case verCatalonia:
			region = L"Catalonia";
			break;
		
		case verChina:
			region = L"China";
			break;
		
		case verCroatia:
			region = L"Croatia";
			break;
		
		case verCzech:
			region = L"Czechoslovakia";
			break;
		
		case verDenmark:
		case vervariantDenmark:
			region = L"Denmark";
			break;
		
		case verEstonia:
			region = L"Estonia";
			break;
		
		case verFinland:
			region = L"Finland";
			break;
		
		case verFrance:
		case verFrenchUniversal:
			region = L"France";
			break;
		
		case verGermany:
			region = L"Germany";
			break;
		
		case verGeorgian:
			region = L"Georgia";
			break;
		
		case verGreece:
		case verGreecePoly:
			region = L"Greece";
			break;
		
		case verHungary:
			region = L"Hungary";
			break;
		
		case verIceland:
			region = L"Iceland";
			break;
		
		case verIndiaHindi:
		case verIndiaUrdu:
			region = L"India";
			break;
		
		case verInternational:
			region = L"International";
			break;
		
		case verIran:
			region = L"Iran";
			break;
		
		case verIreland:
		case verIrishGaelicScript:
			region = L"Ireland";
			break;
		
		case verIsrael:
			region = L"Israel";
			break;
		
		case verItaly:
			region = L"Italy";
			break;
		
		case verJapan:
			region = L"Japan";
			break;
		
		case verKorea:
			region = L"Korea";
			break;
		
		case verSpLatinAmerica:
			region = L"Latin America";
			break;
		
		case verLithuania:
			region = L"Lithuania";
			break;
		
		case verNetherlands:
		case verNetherlandsComma:
			region = L"Netherlands";
			break;
		
		case verArabic:
			region = L"North Africa";
			break;
		
		case verNorway:
		case verNynorsk:
			region = L"Norway";
			break;
		
		case verPakistanUrdu:
			region = L"Pakistan";
			break;
		
		case verPoland:
			region = L"Poland";
			break;
		
		case verPortugal:
			region = L"Portugal";
			break;
		
		case verPunjabi:
			region = L"Punjab";
			break;
		
		case verRomania:
			region = L"Romania";
			break;
		
		case verRussia:
			region = L"Russia";
			break;
			
		case verScottishGaelic:
			region = L"Scotland";
			break;
		
		case verSerbian:
			region = L"Serbia";
			break;
		
		case verSlovak:
			region = L"Slovakia";
			break;
			
		case verSlovenian:
			region = L"Slovenia";
			break;
		
		case verAfrikaans:
			region = L"South Africa";
			break;
				
		case verSpain:
			region = L"Spain";
			break;
		
		case verSweden:
			region = L"Sweden";
			break;
		
		case verFrSwiss:
		case verGrSwiss:
		case verItalianSwiss:
			region = L"Switzerland";
			break;
		
		case verTaiwan:
			region = L"Taiwan";
			break;
		
		case verThailand:
			region = L"Thailand";
			break;
		
		case verTibetan:
			region = L"Tibet";
			break;
		
		case verTurkey:
			region = L"Turkey";
			break;
		
		case verTurkishModified:
			region = L"Turkey";
			break;
		
		case verUkraine:
			region = L"Ukraine";
			break;
		
		case verUS:
			region = L"USA";
			break;
			
		case verVietnam:
			region = L"Vietnam";
			break;
		
		case verWelsh:
			region = L"Wales";
			break;
			
		default:
			DEBUGSTR("XLocale::DoGetRegion couldn't find the region");
			region = L"US";
	}

#elif WIN
	uint16 primary   = numeric_cast<uint16>(PRIMARYLANGID(locale));
	uint16 secondary = numeric_cast<uint16>(SUBLANGID(locale));		// these numbers are not unique so we need to make use of language
	
	switch (primary) {
		case LANG_ARABIC:
			switch (secondary) {
				case SUBLANG_ARABIC_ALGERIA:
				case SUBLANG_ARABIC_BAHRAIN:
				case SUBLANG_ARABIC_EGYPT:
				case SUBLANG_ARABIC_IRAQ:
				case SUBLANG_ARABIC_JORDAN:
				case SUBLANG_ARABIC_KUWAIT:
				case SUBLANG_ARABIC_LIBYA:
				case SUBLANG_ARABIC_LEBANON:
				case SUBLANG_ARABIC_MOROCCO:
				case SUBLANG_ARABIC_OMAN:
				case SUBLANG_ARABIC_QATAR:
				case SUBLANG_ARABIC_SYRIA:
				case SUBLANG_ARABIC_TUNISIA:
				case SUBLANG_ARABIC_UAE:
				case SUBLANG_ARABIC_YEMEN:
					region = L"North Africa";
					break;
			default:
				DEBUGSTR("XLocale::DoGetRegion couldn't find the locale");
				region = L"";
			}
			break;
	
		case LANG_CHINESE:
			if (secondary == SUBLANG_CHINESE_SIMPLIFIED)
				region = L"China";
			else if (secondary == SUBLANG_CHINESE_HONGKONG)
				region = L"China";
			else if (secondary == SUBLANG_CHINESE_SINGAPORE)
				region = L"Singapore";
			else if (secondary == SUBLANG_CHINESE_TRADITIONAL)
				region = L"Taiwan";
			break;
	
		case LANG_DUTCH:
			if (secondary == SUBLANG_DUTCH)
				region = L"Netherlands";
			else if (secondary == SUBLANG_DUTCH_BELGIAN)
				region = L"Belgium";
			break;
	
		case LANG_ENGLISH:
			if (secondary == SUBLANG_ENGLISH_AUS)
				region = L"Australia";
			else if (secondary == SUBLANG_ENGLISH_UK)
				region = L"Britain";
			else if (secondary == SUBLANG_ENGLISH_CAN)
				region = L"Canada";
			else if (secondary == SUBLANG_ENGLISH_EIRE)
				region = L"Ireland";
			else if (secondary == SUBLANG_ENGLISH_NZ)
				region = L"New Zealand";
			else if (secondary == SUBLANG_ENGLISH_SOUTH_AFRICA)
				region = L"South Africa";
			else if (secondary == SUBLANG_ENGLISH_US)
				region = L"USA";
			break;
	
		case LANG_FRENCH:
			if (secondary == SUBLANG_FRENCH)
				region = L"France";
			else if (secondary == SUBLANG_FRENCH_BELGIAN)
				region = L"Belgium";
			else if (secondary == SUBLANG_FRENCH_CANADIAN)
				region = L"Canada";
			else if (secondary == SUBLANG_FRENCH_LUXEMBOURG)
				region = L"Luxembourg";
			else if (secondary == SUBLANG_FRENCH_SWISS)
				region = L"Switzerland";
			break;
	
		case LANG_GERMAN:
			if (secondary == SUBLANG_GERMAN)
				region = L"Germany";
			else if (secondary == SUBLANG_GERMAN_LIECHTENSTEIN)
				region = L"Liechtenstein";
			else if (secondary == SUBLANG_GERMAN_LUXEMBOURG)
				region = L"Luxembourg";
			else if (secondary == SUBLANG_GERMAN_SWISS)
				region = L"Switzerland";
			break;
		
		case LANG_ITALIAN:
			if (secondary == SUBLANG_ITALIAN)
				region = L"Italy";
			else if (secondary == SUBLANG_ITALIAN_SWISS)
				region = L"Switzerland";
			break;
	
		case LANG_KOREAN:
			if (secondary == SUBLANG_KOREAN)
				region = L"Korea";
//			else if (secondary == SUBLANG_KOREAN_JOHAB)		// missing from CW Pro 5fc3
//				region = L"Korea";
			break;
		
		case LANG_NORWEGIAN:
			if (secondary == SUBLANG_NORWEGIAN_NYNORSK)
				region = L"Norway";
			else if (secondary == SUBLANG_NORWEGIAN_BOKMAL)
				region = L"Norway";
			break;
	
		case LANG_PORTUGUESE:
			if (secondary == SUBLANG_PORTUGUESE)
				region = L"Portugal";
			else if (secondary == SUBLANG_PORTUGUESE_BRAZILIAN)
				region = L"Brazil";
			break;
	
		case LANG_SPANISH:
			switch (secondary) {
				case SUBLANG_SPANISH_MODERN:
				case SUBLANG_SPANISH:
					region = L"Spain";
					break;
	
				case SUBLANG_SPANISH_ARGENTINA:
				case SUBLANG_SPANISH_BOLIVIA:
				case SUBLANG_SPANISH_CHILE:
				case SUBLANG_SPANISH_COLOMBIA:
				case SUBLANG_SPANISH_COSTA_RICA:
				case SUBLANG_SPANISH_DOMINICAN_REPUBLIC:
				case SUBLANG_SPANISH_ECUADOR:
				case SUBLANG_SPANISH_EL_SALVADOR:
				case SUBLANG_SPANISH_GUATEMALA:
				case SUBLANG_SPANISH_HONDURAS:
				case SUBLANG_SPANISH_MEXICAN:
				case SUBLANG_SPANISH_NICARAGUA:
				case SUBLANG_SPANISH_PANAMA:
				case SUBLANG_SPANISH_PARAGUAY:
				case SUBLANG_SPANISH_PERU:
				case SUBLANG_SPANISH_PUERTO_RICO:
				case SUBLANG_SPANISH_URUGUAY:
				case SUBLANG_SPANISH_VENEZUELA:
					region = L"Latin America";
					break;
			
				default:
					DEBUGSTR("XLocale::DoGetRegion couldn't find the locale");
					region = L"";
			}
			break;
	
		case LANG_SWEDISH:
			if (secondary == SUBLANG_SWEDISH)
				region = L"Sweden";
			else if (secondary == SUBLANG_SWEDISH_FINLAND)
				region = L"Finland";
			break;
			
		default:
			DEBUGSTR("XLocale::DoGetRegion couldn't find the locale");
			region = L"";
	}
#endif

	return region;
}


}	// namespace Whisper
