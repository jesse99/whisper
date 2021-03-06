/*
 *  File:		FastString.cpp
 *  Summary:	String class based on a pair of pointers into a character buffer.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: FastString.cpp,v $
 *		Revision 1.2  2001/04/21 02:25:20  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.1  2001/01/01 03:45:42  jesjones
 *		Checked in
 */
 
#include "AppHeader.h"
#include "FastString.h"

#include <XNumbers.h>


// ========================================================================================
//	class FastString
// ========================================================================================

//---------------------------------------------------------------
//
// FastString::at
//
//---------------------------------------------------------------
char FastString::at(uint32 index) const
{
	if (index >= this->length())
		throw std::out_of_range("Bad index in FastString::at");
		
	return mBegin[index];
}


//---------------------------------------------------------------
//
// FastString::find (const char*, uint32)
//
//---------------------------------------------------------------
uint32 FastString::find(const char* str, uint32 index) const
{
	PRECONDITION(str != nil);
	PRECONDITION(index <= this->length());

	uint32 pos = npos;
	uint32 count = std::strlen(str);

	const_iterator iter = std::search(this->begin() + index, this->end(), str, str + count);
	if (iter < this->end())
		pos = numeric_cast<uint32>(iter - mBegin);

	return pos;
}


//---------------------------------------------------------------
//
// FastString::DoIsEqual (const char*)
//
//---------------------------------------------------------------
bool FastString::DoIsEqual(const char* rhs) const	
{
	uint32 len = this->length();
	
	bool equal = rhs[len] == 0 && std::memcmp(mBegin, rhs, len) == 0;
	
	return equal;
}


//---------------------------------------------------------------
//
// FastString::DoIsEqual (FastString)
//
//---------------------------------------------------------------
bool FastString::DoIsEqual(const FastString& rhs) const				
{
	bool equal = mBegin == rhs.mBegin && mEnd == rhs.mEnd;
	
	if (!equal) {
		uint32 len = this->length();
	
		equal = len == rhs.length() && std::memcmp(mBegin, rhs.mBegin, len) == 0;
	}
	
	return equal;
}


