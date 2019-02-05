/*
 *  File:		MCoreString.h
 *  Summary:	Wrapper around an immutable CFString.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MCoreString.h,v $
 *		Revision 1.1  2001/04/13 08:07:20  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XDebug.h>		

#if TARGET_CARBON
	#include <CFBase.h>			// $$ the CoreFoundation's wacky includes causes problems with Pro5 so we'll do the including ourselves
	#include <CFArray.h>
	#include <CFData.h>
	#include <CFDictionary.h>
	#include <CFString.h>
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ========================================================================================
//	class MCoreString
//!		Wrapper around an immutable CFString.
// ========================================================================================
class CORE_EXPORT MCoreString {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~MCoreString();

						MCoreString();
						
						MCoreString(const char* str);				
						MCoreString(const unsigned char* str);				
						/**< Uses default system encoding. */

						MCoreString(const char* str, CFStringEncoding encoding);				
						MCoreString(const unsigned char* str, CFStringEncoding encoding);				

						MCoreString(const wchar_t* str);				
						MCoreString(const std::wstring& str);				
						/**< Assumes Unicode encoding. */

						MCoreString(CFStringRef str);
						/**< Bumps the ref count. */
						
						MCoreString(CFMutableStringRef str);
						/**< Makes a copy. */

						MCoreString(const MCoreString& str);				
						
			MCoreString& operator=(const MCoreString& rhs);

//-----------------------------------
//	API
//
public:
	// ----- Size -----
			uint32 		length() const;
			uint32		size() const							{return this->length();}
			bool		empty() const							{return this->length() == 0;}

	// ----- Access -----
			const wchar_t* c_str() const;
						/**< Use the routines in XTranscode.h if you need other encodings. */
						
						operator CFStringRef() const			{return mString;}

	// ----- Misc -----
	friend	void 		swap(MCoreString& lhs, MCoreString& rhs);

//-----------------------------------
//	Member Data
//
private:
	CFStringRef			mString;
	mutable wchar_t*	mCStr;	
};
	

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
