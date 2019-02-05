/*
 *  File:		MCoreString.cpp
 *  Summary:	Wrapper around an immutable CFString.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MCoreString.cpp,v $
 *		Revision 1.2  2001/04/21 03:24:41  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.1  2001/04/13 08:07:17  jesjones
 *		Checked in
 */
 
#include <XWhisperHeader.h>
#include <MCoreString.h>		

#include <XExceptions.h>

namespace Whisper {


// ========================================================================================
//	class MCoreString
// ========================================================================================

//---------------------------------------------------------------
//
// MCoreString::~MCoreString
//
//---------------------------------------------------------------
MCoreString::~MCoreString()
{
	CFRelease(mString);
	delete [] mCStr;
}


//---------------------------------------------------------------
//
// MCoreString::MCoreString ()
//
//---------------------------------------------------------------
MCoreString::MCoreString()
{	
	mString = CFStringCreateWithCharacters(kCFAllocatorSystemDefault, nil, 0);
	ThrowIfNil(mString);
						
	mCStr = nil;
}


//---------------------------------------------------------------
//
// MCoreString::MCoreString (unsigned char*)
//
//---------------------------------------------------------------
MCoreString::MCoreString(const unsigned char* str)
{
	PRECONDITION(str != nil);
	
	CFStringEncoding encoding = CFStringGetSystemEncoding();
	mString = CFStringCreateWithPascalString(kCFAllocatorSystemDefault, str, encoding);
	ThrowIfNil(mString);
			
	mCStr = nil;
}


//---------------------------------------------------------------
//
// MCoreString::MCoreString (char*)
//
//---------------------------------------------------------------
MCoreString::MCoreString(const char* str)
{
	PRECONDITION(str != nil);
	
	CFStringEncoding encoding = CFStringGetSystemEncoding();
	mString = CFStringCreateWithCString(kCFAllocatorSystemDefault, str, encoding);
	ThrowIfNil(mString);
			
	mCStr = nil;
}


//---------------------------------------------------------------
//
// MCoreString::MCoreString (const unsigned char*, CFStringEncoding)
//
//---------------------------------------------------------------
MCoreString::MCoreString(const unsigned char* str, CFStringEncoding encoding)
{
	PRECONDITION(str != nil);
	
	mString = CFStringCreateWithPascalString(kCFAllocatorSystemDefault, str, encoding);
	ThrowIfNil(mString);
			
	mCStr = nil;
}


//---------------------------------------------------------------
//
// MCoreString::MCoreString (char*, CFStringEncoding)
//
//---------------------------------------------------------------
MCoreString::MCoreString(const char* str, CFStringEncoding encoding)
{
	PRECONDITION(str != nil);
	
	mString = CFStringCreateWithCString(kCFAllocatorSystemDefault, str, encoding);
	ThrowIfNil(mString);
			
	mCStr = nil;
}


//---------------------------------------------------------------
//
// MCoreString::MCoreString (wchar_t*)
//
//---------------------------------------------------------------
MCoreString::MCoreString(const wchar_t* str)
{
	PRECONDITION(str != nil);
	
	mString = CFStringCreateWithCharacters(kCFAllocatorSystemDefault, (UniChar*) str, (int32) std::wcslen(str));
	ThrowIfNil(mString);
			
	mCStr = nil;
}


//---------------------------------------------------------------
//
// MCoreString::MCoreString (wstring)
//
//---------------------------------------------------------------
MCoreString::MCoreString(const std::wstring& str)
{	
	mString = CFStringCreateWithCharacters(kCFAllocatorSystemDefault, (UniChar*) str.c_str(), (int32) str.length());
	ThrowIfNil(mString);
			
	mCStr = nil;
}


//---------------------------------------------------------------
//
// MCoreString::MCoreString (CFStringRef)
//
//---------------------------------------------------------------
MCoreString::MCoreString(CFStringRef str)
{
	PRECONDITION(str != nil);
	
	mString = str;
	CFRetain(mString);
			
	mCStr = nil;
}


//---------------------------------------------------------------
//
// MCoreString::MCoreString (CFMutableStringRef)
//
//---------------------------------------------------------------
MCoreString::MCoreString(CFMutableStringRef str)
{
	PRECONDITION(str != nil);
	
	mString = CFStringCreateCopy(kCFAllocatorSystemDefault, str);	
	ThrowIfNil(mString);
		
	mCStr = nil;
}


//---------------------------------------------------------------
//
// MCoreString::MCoreString (MCoreString)
//
//---------------------------------------------------------------
MCoreString::MCoreString(const MCoreString& str)
{	
	mString = str.mString;			// immutable so we can refcount
	CFRetain(mString);
		
	mCStr = nil;
}
				

//---------------------------------------------------------------
//
// MCoreString::operator= (MCoreString)
//
//---------------------------------------------------------------
MCoreString& MCoreString::operator=(const MCoreString& rhs)
{
	if (this != &rhs) {
		CFRelease(mString);

		mString = rhs.mString;			// immutable so we can refcount
		CFRetain(mString);
		
		delete [] mCStr; 
		mCStr = nil; 
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// MCoreString::length
//
//---------------------------------------------------------------
uint32 MCoreString::length() const
{
	uint32 len = (uint32) CFStringGetLength(mString);
	
	return len;
}


//---------------------------------------------------------------
//
// MCoreString::c_str ()
//
//---------------------------------------------------------------
const wchar_t* MCoreString::c_str() const
{
	if (mCStr == nil) {
		uint32 len = this->length();
		mCStr = new wchar_t[len+1];
	
		CFRange range = {0, (int32) len};
		CFStringGetCharacters(mString, range, (UniChar*) mCStr);
		mCStr[len] = '\0';
	}
	
	return mCStr;
}


//---------------------------------------------------------------
//
// swap (MCoreString, MCoreString)
//
//---------------------------------------------------------------
void swap(MCoreString& lhs, MCoreString& rhs)
{
	std::swap(lhs.mString, rhs.mString);
	std::swap(lhs.mCStr, rhs.mCStr);
}


}	// namespace Whisper
