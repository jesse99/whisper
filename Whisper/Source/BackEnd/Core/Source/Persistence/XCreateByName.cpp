/*
 *  File:		XCreateByName.cpp
 *  Summary:	Mixin for objects that can be created from a string (using XObjectFactory).
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XCreateByName.cpp,v $
 *		Revision 1.2  2000/11/09 12:29:40  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 3/20/99	JDJ		Created 
 */

#include <XWhisperHeader.h>
#include <XCreateByName.h>

#include <typeinfo>

#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	class XCreateByNameMixin
// ===================================================================================

//---------------------------------------------------------------
//
// XCreateByNameMixin::~XCreateByNameMixin
//
//---------------------------------------------------------------
XCreateByNameMixin::~XCreateByNameMixin()
{
}


//---------------------------------------------------------------
//
// XCreateByNameMixin::XCreateByNameMixin
//
//---------------------------------------------------------------
XCreateByNameMixin::XCreateByNameMixin() 
{
	mClassName = L"????";			// wait until the object is fully constructed
}


//---------------------------------------------------------------
//
// XCreateByNameMixin::GetClassName
//
//---------------------------------------------------------------
std::wstring XCreateByNameMixin::GetClassName() const
{
	if (mClassName == L"????")		
		mClassName = FromAsciiStr(typeid(*this).name());
	
	return mClassName;
}

	
//---------------------------------------------------------------
//
// XCreateByNameMixin::SetClassName
//
//---------------------------------------------------------------
void XCreateByNameMixin::SetClassName(const std::wstring& name)
{
	mClassName = name;
}


}	// namespace Whisper
