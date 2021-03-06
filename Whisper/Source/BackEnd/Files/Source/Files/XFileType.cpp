/*
 *  File:       XFileType.h
 *  Summary:   	A class used to encapsulate the notion of a "file type". 
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XFileType.cpp,v $
 *		Revision 1.2  2000/11/09 12:36:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <2>	 7/12/99	JDJ		Match no longer tries to match empty extensions.
 *		 <1>	 5/07/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XFileType.h>

#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	class XFileType
// ===================================================================================

//---------------------------------------------------------------
//
// XFileType::~XFileType
//
//---------------------------------------------------------------
XFileType::~XFileType()
{
}


//---------------------------------------------------------------
//
// XFileType::XFileType ()
//
//---------------------------------------------------------------
XFileType::XFileType()
{	
	mCreator = 0;
	mType = 0;
}


//---------------------------------------------------------------
//
// XFileType::XFileType (wstring, wstring, OSType, OSType)
//
//---------------------------------------------------------------
XFileType::XFileType(const std::wstring& name, const std::wstring& extension, OSType creator, OSType type) : mName(name), mExtension(extension)
{	
	mCreator = creator;
	mType = type;
}


//---------------------------------------------------------------
//
// XFileType::XFileType OSType, OSType)
//
//---------------------------------------------------------------
#if MAC
XFileType::XFileType(OSType creator, OSType type)
{	
	mCreator = creator;
	mType = type;
}
#endif


//---------------------------------------------------------------
//
// XFileType::XFileType (wstring, wstring)
//
//---------------------------------------------------------------
XFileType::XFileType(const std::wstring& name, const std::wstring& extension) : mName(name), mExtension(extension)
{	
	mCreator = 0;
	mType = 0;
}


//---------------------------------------------------------------
//
// XFileType::XFileType (XFileType)
//
//---------------------------------------------------------------
XFileType::XFileType(const XFileType& rhs) : mName(rhs.mName), mExtension(rhs.mExtension)
{	
	mCreator = rhs.mCreator;
	mType = rhs.mType;
}


//---------------------------------------------------------------
//
// XFileType::Match
//
//---------------------------------------------------------------
uint32 XFileType::Match(const std::wstring& extension, OSType type) const
{
	UNUSED(type);					// unused on Windows
	
	uint32 result = 0;
	
	if (mExtension == L"*")
		result = 1;
		
	else if (extension.length() > 0) {
		result = extension.length() == mExtension.length() ? 1UL : 0UL;
		for (uint32 i = 0; i < mExtension.length() && result > 0; ++i) {
			if (mExtension[i] != '?')
				result = ConvertToLowerCase(extension[i]) == ConvertToLowerCase(mExtension[i]) ? 1UL : 0UL;
		}
	}

#if MAC
	result += mType == '****' || type == mType ? 1UL : 0UL;
#endif

	return result;
}


}	// namespace Whisper
