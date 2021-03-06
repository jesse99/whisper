/*
 *  File:		XXMLAttribute.cpp
 *  Summary:	Encapsulates an XML attribute.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLAttribute.cpp,v $
 *		Revision 1.2  2000/11/09 12:59:54  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <3>	10/06/00	JDJ	 	Changed to use template member functions instead
 *									of hard coding selected types. 
 *		 <2>	 3/02/00	JDJ	 	Replaced references to XRGBColor with OSColor. GetColorValue 
 *									throws if a component is outside [0.0, 1.0].
 *		 <1>	 3/02/00	JDJ		Split off from XXMLDoc.cpp
 */

#include <XWhisperHeader.h>
#include <XXMLAttribute.h>

#include <XStreaming.h>

namespace Whisper {


// ===================================================================================
//	class XXMLAttribute
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLAttribute::~XXMLAttribute
//
//---------------------------------------------------------------
XXMLAttribute::~XXMLAttribute()
{
}


//---------------------------------------------------------------
//
// XXMLAttribute::XXMLAttribute ()
//
//---------------------------------------------------------------
XXMLAttribute::XXMLAttribute()
{
	mImplied = false;
}


//---------------------------------------------------------------
//
// XXMLAttribute::XXMLAttribute (const wchar_t*, const wchar_t*, bool)
//
//---------------------------------------------------------------
XXMLAttribute::XXMLAttribute(const wchar_t* name, const wchar_t* value, bool implied) : mName(name), mValue(value)
{
	mImplied = implied;
}


//---------------------------------------------------------------
//
// XXMLAttribute::XXMLAttribute (const wchar_t*, wstring, bool)
//
//---------------------------------------------------------------
XXMLAttribute::XXMLAttribute(const wchar_t* name, const std::wstring& value, bool implied) : mName(name), mValue(value)
{
	mImplied = implied;
}


//---------------------------------------------------------------
//
// XXMLAttribute::XXMLAttribute (wstring, const wchar_t*, bool)
//
//---------------------------------------------------------------
XXMLAttribute::XXMLAttribute(const std::wstring& name, const wchar_t* value, bool implied) : mName(name), mValue(value)
{
	mImplied = implied;
}


//---------------------------------------------------------------
//
// XXMLAttribute::XXMLAttribute (wstring, wstring, bool)
//
//---------------------------------------------------------------
XXMLAttribute::XXMLAttribute(const std::wstring& name, const std::wstring& value, bool implied) : mName(name), mValue(value)
{
	mImplied = implied;
}


//---------------------------------------------------------------
//
// XXMLAttribute::XXMLAttribute (XXMLAttribute)
//
//---------------------------------------------------------------
XXMLAttribute::XXMLAttribute(const XXMLAttribute& rhs) : mName(rhs.mName), mValue(rhs.mValue)
{
	mImplied = rhs.mImplied;
}


//---------------------------------------------------------------
//
// XXMLAttribute::SetValue
//
//---------------------------------------------------------------
void XXMLAttribute::SetValue(const std::wstring& value)
{
	mValue = value;
	mImplied = false;
}


//---------------------------------------------------------------
//
// XXMLAttribute::operator<<
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const XXMLAttribute& value)
{
	stream << 1L << value.mName << value.mValue << value.mImplied;
	
	return stream;
}


//---------------------------------------------------------------
//
// XXMLAttribute::operator>>
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, XXMLAttribute& value)
{
	int32 version;
	stream >> version >> value.mName >> value.mValue >> value.mImplied;
	
	if (version != 1)
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: Invalid XXMLAttribute version.")));
	
	return stream;
}


}	// namespace Whisper

