/*
 *  File:		XNumericConversions.cpp
 *  Summary:	Template classes used to convert numbers to and from strings.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XNumericConversions.cpp,v $
 *		Revision 1.3  2000/12/10 04:10:57  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:44:57  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 1/10/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XNumericConversions.h>

#include <XDebug.h>
#include <XExceptions.h>
#include <XNumbers.h>
#include <XStringUtils.h>

namespace Whisper {

//using namespace std;

#if MSVC != 1100					// $$$ MSVC 5.0 won't link if these are defined in the cpp

//===================================================================================
//	class XNumericConversions<short>
//===================================================================================
short XNumericConversions<short>::fromString(const std::wstring& str)
{
	int32 value = StrToInt32(str);
	
	if (value > std::numeric_limits<short>::max())
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too big.")));

	else if (value < std::numeric_limits<short>::min())
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too small.")));

	return numeric_cast<int16>(value);
}

#if __MWERKS__
#pragma mark -
#endif

//===================================================================================
//	class XNumericConversions<unsigned short>
//===================================================================================

unsigned short XNumericConversions<unsigned short>::fromString(const std::wstring& str)
{
	uint32 value = StrToUInt32(str);
	
	if (value > std::numeric_limits<unsigned short>::max())
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too big.")));

	return (unsigned short) value;
}

#if __MWERKS__
#pragma mark -
#endif

//===================================================================================
//	class XNumericConversions<int>
//===================================================================================

int XNumericConversions<int>::fromString(const std::wstring& str)
{
	int32 value = StrToInt32(str);
	
	if (value > std::numeric_limits<int>::max())
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too big.")));

	else if (value < std::numeric_limits<int>::min())
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too small.")));

	return (int) value;
}

#if __MWERKS__
#pragma mark -
#endif

//===================================================================================
//	class XNumericConversions<unsigned int>
//===================================================================================

unsigned int XNumericConversions<unsigned int>::fromString(const std::wstring& str)
{
	uint32 value = StrToUInt32(str);
	
	if (value > std::numeric_limits<unsigned int>::max())
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too big.")));

	return (unsigned int) value;
}

#if __MWERKS__
#pragma mark -
#endif

//===================================================================================
//	class XNumericConversions<long>
//===================================================================================

long XNumericConversions<long>::fromString(const std::wstring& str)
{
	COMPILE_CHECK(sizeof(long) == sizeof(int32));
	
	int32 value = StrToInt32(str);
	
	return value;
}

#if __MWERKS__
#pragma mark -
#endif

//===================================================================================
//	class XNumericConversions<unsigned long>
//===================================================================================

unsigned long XNumericConversions<unsigned long>::fromString(const std::wstring& str)
{
	COMPILE_CHECK(sizeof(unsigned long) == sizeof(uint32));
	
	uint32 value = StrToUInt32(str);
	
	return (unsigned int) value;
}

#if __MWERKS__
#pragma mark -
#endif

//===================================================================================
//	class XNumericConversions<float>
//===================================================================================

float XNumericConversions<float>::fromString(const std::wstring& str)
{
	double value = StrToDouble(str);
	
	if (value > std::numeric_limits<float>::max())
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too big.")));

	else if (value < std::numeric_limits<float>::min())
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"The number was too small.")));

	return (float) value;
}

#if __MWERKS__
#pragma mark -
#endif

//===================================================================================
//	class XNumericConversions<double>
//===================================================================================

double XNumericConversions<double>::fromString(const std::wstring& str)
{
	double value = StrToDouble(str);
	
	return value;
}

#endif


}	// namespace Whisper


