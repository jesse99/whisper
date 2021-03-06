/*
 *  File:       XDate.cpp
 *  Summary:   	Wrapper around system date/time struct.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDate.cpp,v $
 *		Revision 1.4  2001/04/27 04:31:39  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2000/12/10 04:09:30  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:30:00  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XDate.h>

#include <XExceptions.h>

#if WIN
	#include <WSystemInfo.h>
	#include <XTinyVector.h>
#endif

namespace Whisper {


// ===================================================================================
//	struct SDate
// ===================================================================================

//---------------------------------------------------------------
//
// SDate::SDate ()
//
//---------------------------------------------------------------
SDate::SDate()
{
	year      = 0;
	month     = 0;
	dayOfWeek = 0;
	day       = 0;
	hour      = 0;
	minute    = 0;
	second    = 0;
}


//---------------------------------------------------------------
//
// SDate::SDate (int32, int32, int32, int32, int32, int32)
//
//---------------------------------------------------------------
SDate::SDate(int32 inYear, int32 inMonth, int32 inDay, int32 inHour, int32 inMinute, int32 inSecond)
{
	year   = inYear;
	month  = inMonth;
	day    = inDay;
	hour   = inHour;
	minute = inMinute;
	second = inSecond;

	dayOfWeek = 0;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XDate
// ===================================================================================

//---------------------------------------------------------------
//
// XDate::XDate ()
//
//---------------------------------------------------------------
XDate::XDate()
{
	mTime = 0;
}


//---------------------------------------------------------------
//
// XDate::XDate (int64)
//
//---------------------------------------------------------------
XDate::XDate(int64 seconds)
{
	mTime = seconds;
}


//---------------------------------------------------------------
//
// XDate::XDate (int32, int32, int32, int32, int32, int32)
//
//---------------------------------------------------------------
XDate::XDate(int32 year, int32 month, int32 day, int32 hour, int32 minute, int32 second)
{
	SDate date(year, month, day, hour, minute, second);
	
	*this = XDate(date);
}


//---------------------------------------------------------------
//
// XDate::XDate (SDate)
//
//---------------------------------------------------------------
XDate::XDate(const SDate& date)
{
	PRECONDITION(date.year >= 0);
	PRECONDITION(date.year <= 29940);
	PRECONDITION(date.month >= 1);
	PRECONDITION(date.month <= 12);
	PRECONDITION(date.day >= 1);
	PRECONDITION(date.day <= 31);
	PRECONDITION(date.hour >= 0);
	PRECONDITION(date.hour <= 23);
	PRECONDITION(date.minute >= 0);
	PRECONDITION(date.minute <= 59);
	PRECONDITION(date.second >= 0);
	PRECONDITION(date.second <= 59);
	
#if MAC
	LongDateRec dateRec;
	dateRec.ld.era        = 0;				// AD
	dateRec.ld.year       = (int16) date.year;
	dateRec.ld.month      = (int16) date.month;
	dateRec.ld.day        = (int16) date.day;
	dateRec.ld.hour       = (int16) date.hour;
	dateRec.ld.minute     = (int16) date.minute;
	dateRec.ld.second     = (int16) date.second;
	dateRec.ld.dayOfWeek  = 0;
	dateRec.ld.dayOfYear  = 0;
	dateRec.ld.weekOfYear = 0;
	dateRec.ld.pm         = 0;
	dateRec.ld.res1       = 0;
	dateRec.ld.res2       = 0;
	dateRec.ld.res3       = 0;
	
	OSDate seconds;
	::LongDateToSeconds(&dateRec, &seconds.c);
	mTime = seconds.c;

#elif WIN
	SYSTEMTIME systemTime;
	systemTime.wYear         = (uint16) date.year; 
	systemTime.wMonth        = (uint16) date.month;
	systemTime.wDayOfWeek    = 0;
	systemTime.wDay          = (uint16) date.day;
	systemTime.wHour         = (uint16) date.hour;
	systemTime.wMinute       = (uint16) date.minute;
	systemTime.wSecond       = (uint16) date.second;
	systemTime.wMilliseconds = 0;

	FILETIME fileTime;
	int32 succeeded = ::SystemTimeToFileTime(&systemTime, &fileTime);
	ThrowIf(!succeeded);
	
	mTime = fileTime.dwHighDateTime;
	mTime = (mTime << 32) + fileTime.dwLowDateTime;
	mTime /= 10000;										// convert from 100-nanosecond intervals to secconds
#endif
}


//---------------------------------------------------------------
//
// XDate::XDate (OSDate)
//
//---------------------------------------------------------------
XDate::XDate(const OSDate& date)
{
#if MAC
	mTime = date.c;

#elif WIN
	mTime = date.dwHighDateTime;
	mTime = (mTime << 32) + date.dwLowDateTime;
	mTime /= 10000;										// convert from 100-nanosecond intervals to secconds
#endif
}


//---------------------------------------------------------------
//
// XDate::XDate (SYSTEMTIME)
//
//---------------------------------------------------------------
#if WIN
XDate::XDate(const SYSTEMTIME& systemTime)
{
	FILETIME fileTime;
	int32 succeeded = ::SystemTimeToFileTime(&systemTime, &fileTime);
	ThrowIf(!succeeded);
	
	mTime = fileTime.dwHighDateTime;
	mTime = (mTime << 32) + fileTime.dwLowDateTime;
	mTime /= 10000;										// convert from 100-nanosecond intervals to secconds
}
#endif


//---------------------------------------------------------------
//
// XDate::GetSystemTime
//
//---------------------------------------------------------------
#if WIN
SYSTEMTIME XDate::GetSystemTime() const
{
	int64 time = mTime*10000;

	FILETIME fileTime;
	fileTime.dwHighDateTime = numeric_cast<uint32>(time >> 32);
	fileTime.dwLowDateTime  = numeric_cast<uint32>(time & 0xFFFFFFFF);
	
	SYSTEMTIME systemTime;
	int32 succeeded = ::FileTimeToSystemTime(&fileTime, &systemTime);
	ThrowIf(!succeeded);
	
	return systemTime;
}
#endif


//---------------------------------------------------------------
//
// XDate::GetDate
//
//---------------------------------------------------------------
SDate XDate::GetDate() const
{
	SDate date;
	
#if MAC
	LongDateRec dateRec;
	::LongSecondsToDate(&mTime, &dateRec);

	date.year      = dateRec.ld.year;
	date.month     = dateRec.ld.month;
	date.dayOfWeek = dateRec.ld.dayOfWeek - 1;
	date.day       = dateRec.ld.day;
	date.hour      = dateRec.ld.hour;
	date.minute    = dateRec.ld.minute;
	date.second    = dateRec.ld.second;

#elif WIN
	SYSTEMTIME systemTime = this->GetSystemTime();

	date.year      = systemTime.wYear;
	date.month     = systemTime.wMonth;
	date.dayOfWeek = systemTime.wDayOfWeek;
	date.day       = systemTime.wDay;
	date.hour      = systemTime.wHour;
	date.minute    = systemTime.wMinute;
	date.second    = systemTime.wSecond;
#endif

	return date;
}


//---------------------------------------------------------------
//
// XDate::SetDate
//
//---------------------------------------------------------------
void XDate::SetDate(const SDate& date)
{
	*this = XDate(date);
}


//---------------------------------------------------------------
//
// XDate::GetEpoch
//
//---------------------------------------------------------------
SDate XDate::GetEpoch() const
{
	XDate date;

	return date.GetDate();
}


//---------------------------------------------------------------
//
// XDate::GetOSDate
//
//---------------------------------------------------------------
OSDate XDate::GetOSDate() const
{
	OSDate date;
	
#if MAC
	date.c = mTime;
#elif WIN
	int64 time = mTime*10000;
	date.dwHighDateTime = numeric_cast<uint32>(time >> 32);
	date.dwLowDateTime  = numeric_cast<uint32>(time & 0xFFFFFFFF);
#endif

	return date;
}


//---------------------------------------------------------------
//
// XDate::GetCurrentDate								[static]
//
//---------------------------------------------------------------
XDate XDate::GetCurrentDate()
{
	OSDate date;
	
#if MAC
	date.hl.lHigh = 0;
	::GetDateTime(&date.hl.lLow);
	
#elif WIN
	SYSTEMTIME systemTime;
	::GetLocalTime(&systemTime);
	
	int32 succeeded = ::SystemTimeToFileTime(&systemTime, &date);
	ThrowIf(!succeeded);
#endif

	return XDate(date);
}


//---------------------------------------------------------------
//
// XDate::GetDateString
//
//---------------------------------------------------------------
std::wstring XDate::GetDateString(bool shortVersion)
{
	std::wstring text;
	
#if MAC
	Str255 str;
	SInt64 time = mTime;
	::LongDateString(&time, shortVersion ? shortDate : abbrevDate, str, nil);

	text = FromPascalStr(str);
	
#elif WIN
	SYSTEMTIME systemTime = this->GetSystemTime();

	int32 numChars = ::GetDateFormat(nil, numeric_cast<uint32>(shortVersion ? DATE_SHORTDATE : DATE_LONGDATE), &systemTime, nil, nil, 0);
	ThrowIf(numChars == 0);
	
	if (WSystemInfo::HasUnicode()) {
		XTinyVector<wchar_t> buffer(numChars+1UL);
		int32 succeeded = ::GetDateFormatW(nil, numeric_cast<uint32>(shortVersion ? DATE_SHORTDATE : DATE_LONGDATE), &systemTime, nil, buffer.buffer(), numeric_cast<int32>(buffer.size()));
		ThrowIf(!succeeded);
		
		text.assign(buffer.buffer(), numChars-1UL);		// GetDateFormat includes the trailing zero in the count so we'll subtract one
	
	} else {
		XTinyVector<char> buffer(numChars+1UL);
		int32 succeeded = ::GetDateFormatA(nil, numeric_cast<uint32>(shortVersion ? DATE_SHORTDATE : DATE_LONGDATE), &systemTime, nil, buffer.buffer(), numeric_cast<int32>(buffer.size()));
		ThrowIf(!succeeded);
		
		text = FromPlatformStr(buffer.buffer(), numChars-1UL);
	}	
#endif

	return text;
}


//---------------------------------------------------------------
//
// XDate::GetTimeString
//
//---------------------------------------------------------------
std::wstring XDate::GetTimeString(bool includeSeconds)
{
	std::wstring text;
	
#if MAC
	Str255 str;
	SInt64 time = mTime;
	::LongTimeString(&time, includeSeconds, str, nil);

	text = FromPascalStr(str);
#elif WIN
	SYSTEMTIME systemTime = this->GetSystemTime();

	int32 numChars = ::GetTimeFormat(nil, numeric_cast<uint32>(includeSeconds ? 0 : TIME_NOSECONDS), &systemTime, nil, nil, 0);
	ThrowIf(numChars == 0);
	
	if (WSystemInfo::HasUnicode()) {
		XTinyVector<wchar_t> buffer(numChars+1UL);
		int32 succeeded = ::GetTimeFormatW(nil, numeric_cast<uint32>(includeSeconds ? 0 : TIME_NOSECONDS), &systemTime, nil, buffer.buffer(), numeric_cast<int32>(buffer.size()));
		ThrowIf(!succeeded);
		
		text.assign(buffer.buffer(), numChars-1UL);	// GetTimeFormat includes the trailing zero in the count so we'll subtract one

	} else {
		XTinyVector<char> buffer(numChars+1UL);
		int32 succeeded = ::GetTimeFormatA(nil, numeric_cast<uint32>(includeSeconds ? 0 : TIME_NOSECONDS), &systemTime, nil, buffer.buffer(), numeric_cast<int32>(buffer.size()));
		ThrowIf(!succeeded);
		
		text = FromPlatformStr(buffer.buffer(), numChars-1UL);
	}
#endif

	return text;
}


}	// namespace Whisper
