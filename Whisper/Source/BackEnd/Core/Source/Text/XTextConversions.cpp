/*
 *	File:		XTextConversions.cpp
 *  Summary:	Functions used to convert arbitrary types to and from strings.
 *	Written by:	Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: XTextConversions.cpp,v $
 *		Revision 1.3  2001/03/12 02:40:48  jesjones
 *		Some of the functions were enabling exceptions for the wrong streams.
 *		
 *		Revision 1.2  2001/03/02 11:25:26  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.1  2001/03/01 11:47:36  jesjones
 *		Checked in	
 */

#include <XWhisperHeader.h>
#include <XTextConversions.h>

#include <sstream>

namespace Whisper {


// ===================================================================================
//	Basic Conversions
//		$$$ These generate too much code: might want to re-implement them...
// ===================================================================================

//---------------------------------------------------------------
//
// FromStr (std::wstring, bool)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, bool& value)
{				
	std::wistringstream stream(str.c_str());
	stream.exceptions(std::ios_base::badbit | std::ios_base::failbit);
	
	stream >> std::boolalpha >> value;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// FromStr (std::wstring, int16)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, int16& value)
{				
	std::wistringstream stream(str.c_str());
	stream.exceptions(std::ios_base::badbit | std::ios_base::failbit);
	
	stream >> value;
}


//---------------------------------------------------------------
//
// FromStr (std::wstring, std::wstring, int16, int16, int16)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, const std::wstring& name, int16 min, int16 max, int16& value)
{				
	std::wistringstream stream(str.c_str());
	
	stream >> value;
	
	if (value < min) {
		std::wostringstream stream2;
		stream2.exceptions(std::ios_base::badbit | std::ios_base::failbit);	

		stream2 << min;
		
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Value should be greater than or equal to '#2'.", name, stream2.str())));
	}
	
	if (value > max) {
		std::wostringstream stream2;
		stream2 << max;
		
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Value should be less than or equal to '#2'.", name, stream2.str())));
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// FromStr (std::wstring, uint16)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, uint16& value)
{				
	std::wistringstream stream(str.c_str());
	stream.exceptions(std::ios_base::badbit | std::ios_base::failbit);
	
	stream >> value;
}


//---------------------------------------------------------------
//
// FromStr (std::wstring, std::wstring, uint16, uint16, uint16)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, const std::wstring& name, uint16 min, uint16 max, uint16& value)
{				
	std::wistringstream stream(str.c_str());
	
	stream >> value;
	
	if (value < min) {
		std::wostringstream stream2;
		stream2.exceptions(std::ios_base::badbit | std::ios_base::failbit);

		stream2 << min;
		
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Value should be greater than or equal to '#2'.", name, stream2.str())));
	}
	
	if (value > max) {
		std::wostringstream stream2;
		stream2 << max;
		
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Value should be less than or equal to '#2'.", name, stream2.str())));
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// FromStr (std::wstring, int32)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, int32& value)
{				
	std::wistringstream stream(str.c_str());
	stream.exceptions(std::ios_base::badbit | std::ios_base::failbit);
	
	stream >> value;
}


//---------------------------------------------------------------
//
// FromStr (std::wstring, std::wstring, int32, int32, int32)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, const std::wstring& name, int32 min, int32 max, int32& value)
{				
	std::wistringstream stream(str.c_str());
	
	stream >> value;
	
	if (value < min) {
		std::wostringstream stream2;
		stream2.exceptions(std::ios_base::badbit | std::ios_base::failbit);

		stream2 << min;
		
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Value should be greater than or equal to '#2'.", name, stream2.str())));
	}
	
	if (value > max) {
		std::wostringstream stream2;
		stream2 << max;
		
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Value should be less than or equal to '#2'.", name, stream2.str())));
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// FromStr (std::wstring, uint32)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, uint32& value)
{				
	std::wistringstream stream(str.c_str());
	stream.exceptions(std::ios_base::badbit | std::ios_base::failbit);
	
	stream >> value;
}


//---------------------------------------------------------------
//
// FromStr (std::wstring, std::wstring, uint32, uint32, uint32)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, const std::wstring& name, uint32 min, uint32 max, uint32& value)
{				
	std::wistringstream stream(str.c_str());
	
	stream >> value;
	
	if (value < min) {
		std::wostringstream stream2;
		stream2.exceptions(std::ios_base::badbit | std::ios_base::failbit);

		stream2 << min;
		
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Value should be greater than or equal to '#2'.", name, stream2.str())));
	}
	
	if (value > max) {
		std::wostringstream stream2;
		stream2 << max;
		
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Value should be less than or equal to '#2'.", name, stream2.str())));
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// FromStr (std::wstring, float)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, float& value)
{				
	std::wistringstream stream(str.c_str());
	stream.exceptions(std::ios_base::badbit | std::ios_base::failbit);
	
	stream >> value;
}


//---------------------------------------------------------------
//
// FromStr (std::wstring, std::wstring, float, float, float)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, const std::wstring& name, float min, float max, float& value)
{				
	std::wistringstream stream(str.c_str());
	
	stream >> value;
	
	if (value < min) {
		std::wostringstream stream2;
		stream2.exceptions(std::ios_base::badbit | std::ios_base::failbit);

		stream2 << min;
		
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Value should be greater than or equal to '#2'.", name, stream2.str())));
	}
	
	if (value > max) {
		std::wostringstream stream2;
		stream2 << max;
		
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Value should be less than or equal to '#2'.", name, stream2.str())));
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// FromStr (std::wstring, double)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, double& value)
{				
	std::wistringstream stream(str.c_str());
	stream.exceptions(std::ios_base::badbit | std::ios_base::failbit);
	
	stream >> value;
}


//---------------------------------------------------------------
//
// FromStr (std::wstring, std::wstring, double, double, double)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, const std::wstring& name, double min, double max, double& value)
{				
	std::wistringstream stream(str.c_str());
	
	stream >> value;
	
	if (value < min) {
		std::wostringstream stream2;
		stream2.exceptions(std::ios_base::badbit | std::ios_base::failbit);

		stream2 << min;
		
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Value should be greater than or equal to '#2'.", name, stream2.str())));
	}
	
	if (value > max) {
		std::wostringstream stream2;
		stream2 << max;
		
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Value should be less than or equal to '#2'.", name, stream2.str())));
	}
}


}	// namespace Whisper

