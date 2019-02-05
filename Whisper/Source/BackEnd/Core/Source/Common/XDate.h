/*
 *  File:       XDate.h
 *  Summary:   	Wrapper around system date/time struct.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDate.h,v $
 *		Revision 1.4  2001/03/05 04:41:29  jesjones
 *		Minor format changes.
 *		
 *		Revision 1.3  2000/12/10 08:51:19  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:30:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XNumbers.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	struct SDate
//!		Date/time struct
// ===================================================================================
struct CORE_EXPORT SDate {
	int32	year;			//!< full year, ie 1999 instead of 99			[0, 29940]
	int32	month;			//!< 1 == Jan									[1, 12]
	int32	dayOfWeek;		//!< 0 == Sunday, 1 == Monday					[0, 6]
	int32	day;			//!< day of the month							[1, 31]
	int32	hour;			//!< hours from midnight						[0, 23]
	int32	minute;			//!< minutes from the start of the hour			[0, 59]
	int32	second;			//!< seconds from the start of the minute		[0, 59]

			SDate();
			SDate(int32 year, int32 month, int32 day, int32 hour = 0, int32 minute = 0, int32 second = 0);
};


// ===================================================================================
//	class XDate
//!		Wrapper around system date/time struct.
// ===================================================================================
class CORE_EXPORT XDate {

//-----------------------------------
//	Initialization/Destruction
//
public:
						XDate();
						
						XDate(int32 year, int32 month, int32 day, int32 hour = 0, int32 minute = 0, int32 second = 0);

						XDate(const SDate& date);

						XDate(const OSDate& date);
						
	explicit			XDate(int64 seconds);
						// From start of epoch.
						
#if WIN
						XDate(const SYSTEMTIME& time);
#endif

//-----------------------------------
//	API
//
public:	
	//! @name Accessors
	//@{
			SDate 		GetDate() const;
			void 		SetDate(const SDate& date);

			int64 		GetTime() const									{return mTime;}
			void 		SetTime(int64 seconds)							{mTime = seconds;}
						/**< Time is seconds from the start of the epoch. */

			SDate 		GetEpoch() const;
						/**< Returns the date at which we start counting. On Windows this is 
						Jan 1, 1601. On the Mac this is Jan 1, 1904. */
						
			OSDate 		GetOSDate() const;
#if WIN
			SYSTEMTIME 	GetSystemTime() const;
#endif
			
	static	XDate 		GetCurrentDate();
	//@}

	//! @name Arithmetic operators
	//@{	
			XDate 		operator+(const XDate& rhs) const		{return XDate(mTime + rhs.mTime);}
			XDate 		operator-(const XDate& rhs) const		{return XDate(mTime - rhs.mTime);}
	
			XDate 		operator*(int64 num) const				{return XDate(mTime*num);}
	friend 	XDate 		operator*(int64 num, const XDate& rhs)	{return XDate(rhs.mTime*num);}
			XDate 		operator/(int64 num) const				{ASSERT(num != 0.0); return XDate(mTime/num);}

			XDate& 		operator+=(const XDate& rhs)			{mTime += rhs.mTime; return *this;}
			XDate& 		operator-=(const XDate& rhs)			{mTime -= rhs.mTime; return *this;}
					
			XDate& 		operator*=(int64 num)					{mTime *= num; return *this;}
			XDate&	 	operator/=(int64 num)					{ASSERT(num != 0); mTime /= num; return *this;}
	//@}

	//! @name Relational Operators
	//@{
			bool 		operator==(const XDate& rhs) const		{return mTime == rhs.mTime;}
			bool 		operator!=(const XDate& rhs) const		{return mTime != rhs.mTime;}

			bool 		operator<(const XDate& rhs) const		{return mTime <  rhs.mTime;}
			bool 		operator<=(const XDate& rhs) const		{return mTime <= rhs.mTime;}

			bool 		operator>(const XDate& rhs) const		{return mTime >  rhs.mTime;}
			bool 		operator>=(const XDate& rhs) const		{return mTime >= rhs.mTime;}
	//@}

	//! @name String Conversions
	//@{
			std::wstring GetDateString(bool shortDate = true);
						/**< Returns the date as a locale dependant string. If
						shortDate is true the string will look like "1/31/92".
						Otherwise the string will look something like 
						"Fri, Jan 31, 1992". */

			std::wstring GetTimeString(bool includeSeconds = false);
						/**< Converts the hours, minutes, and seconds part of a
						date into a locale dependant string. */
	//@}

//-----------------------------------
//	Member Data
//
private:
	int64	mTime;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
