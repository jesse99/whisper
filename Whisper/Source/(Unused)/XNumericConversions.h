/*
 *  File:		XNumericConversions.h
 *  Summary:	Template classes used to convert numbers to and from strings.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XNumericConversions.h,v $
 *		Revision 1.3  2000/12/10 04:11:01  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:45:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 1/10/98	JDJ		Created
 */

#pragma once

#include <limits>

#include <XFloatConversions.h>
#include <XIntConversions.h>
#include <XNumbers.h>	
#include <XStringUtils.h>

namespace Whisper {


//===================================================================================
//	class XNumericConversions
//===================================================================================
template <class T> class XNumericConversions {

public:
	static std::wstring toString(T x);			
	static T 			fromString(const std::wstring& str);
};


//===================================================================================
//	Specializations
//		$$$ Probably want to add long long and unsigned long long versions.
//===================================================================================
template<> class XNumericConversions<short> {
public:
	static std::wstring toString(short x)					{return Int32ToStr(x);}
	static short 		fromString(const std::wstring& str);
};

template<> class XNumericConversions<unsigned short> {
public:
	static std::wstring   toString(unsigned short x)			{return UInt32ToStr(x);}
	static unsigned short fromString(const std::wstring& str);
};

template<> class XNumericConversions<int> {
public:
	static std::wstring toString(int x)						{return Int32ToStr(x);}
	static int 			romString(const std::wstring& str);
};

template<> class XNumericConversions<unsigned int> {
public:
	static std::wstring toString(unsigned int x)			{return UInt32ToStr(x);}
	static unsigned int fromString(const std::wstring& str);
};

template<> class XNumericConversions<long> {
public:
	static std::wstring toString(long x)					{return Int32ToStr(x);}
	static long 		fromString(const std::wstring& str);
};

template<> class XNumericConversions<unsigned long> {
public:
	static std::wstring toString(unsigned long x)			{return UInt32ToStr(x);}
	static unsigned long fromString(const std::wstring& str);
};

template<> class XNumericConversions<float> {
public:
	static std::wstring toString(float x)					{return StripLeading(DoubleToStr(x, 16, 8));}
	static float 		fromString(const std::wstring& str);
};

template<> class XNumericConversions<double> {
public:
	static std::wstring toString(double x)					{return StripLeading(DoubleToStr(x, 24, 16));}
	static double 		fromString(const std::wstring& str);
};


#if MSVC == 1100					// $$$ MSVC 5.0 won't link if these are defined in the cpp
inline short XNumericConversions<short>::fromString(const std::wstring& str)
{
	int32 value = StrToInt32(str);
	
	if (value > std::numeric_limits<short>::max())
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too big.")));

	else if (value < std::numeric_limits<short>::min())
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too small.")));

	return numeric_cast<int16>(value);
}

inline unsigned short XNumericConversions<unsigned short>::fromString(const std::wstring& str)
{
	uint32 value = StrToUInt32(str);
	
	if (value > std::numeric_limits<unsigned short>::max())
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too big.")));

	return (unsigned short) value;
}

inline int XNumericConversions<int>::fromString(const std::wstring& str)
{
	int32 value = StrToInt32(str);
	
	if (value > std::numeric_limits<int>::max())
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too big.")));

	else if (value < std::numeric_limits<int>::min())
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too small.")));

	return (int) value;
}

inline unsigned int XNumericConversions<unsigned int>::fromString(const std::wstring& str)
{
	uint32 value = StrToUInt32(str);
	
	if (value > std::numeric_limits<unsigned int>::max())
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too big.")));

	return (unsigned int) value;
}

inline long XNumericConversions<long>::fromString(const std::wstring& str)
{
	COMPILE_CHECK(sizeof(long) == sizeof(int32));
	
	int32 value = StrToInt32(str);
	
	return value;
}

inline unsigned long XNumericConversions<unsigned long>::fromString(const std::wstring& str)
{
	COMPILE_CHECK(sizeof(unsigned long) == sizeof(uint32));
	
	uint32 value = StrToUInt32(str);
	
	return (unsigned int) value;
}

inline float XNumericConversions<float>::fromString(const std::wstring& str)
{
	double value = StrToDouble(str);
	
	if (value > std::numeric_limits<float>::max())
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too big.")));

	else if (value < std::numeric_limits<float>::min())
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too small.")));

	return (float) value;
}

inline double XNumericConversions<double>::fromString(const std::wstring& str)
{
	double value = StrToDouble(str);
	
	return value;
}
#endif	// MSVC == 1100


}	// namespace Whisper


