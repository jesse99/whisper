/*
 *  File:		FastString.h
 *  Summary:	String class based on a pair of pointers into a character buffer.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: FastString.h,v $
 *		Revision 1.1  2001/01/01 03:45:45  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XDebug.h>

using namespace Whisper;
using namespace std;


// ========================================================================================
//	class FastString
// ========================================================================================
class FastString {

//-----------------------------------
//	Types and Constants
//
public:
    typedef const char*											const_iterator;

#if MSVC >= 1100
    typedef std::reverse_iterator<const_iterator, const char>	const_reverse_iterator;
#else
    typedef std::reverse_iterator<const_iterator>				const_reverse_iterator;
#endif

	enum {npos = ULONG_MAX};
	
//-----------------------------------
//	Initialization/Destruction
//
public:
						FastString()							{mBegin = mEnd = nil;}
						FastString(const char* begin, const char* end) : mBegin(begin), mEnd(end) {}
						FastString(const char* begin, uint32 len) : mBegin(begin), mEnd(begin+len) {}
						FastString(const FastString& rhs)		{mBegin = rhs.mBegin; mEnd = rhs.mEnd;}

			FastString&	operator=(const FastString& rhs)		{mBegin = rhs.mBegin; mEnd = rhs.mEnd; return *this;}

			FastString& 	assign(const char* begin, const char* end) {mBegin = begin; mEnd = end; return *this;}
			FastString& 	assign(const FastString& rhs)		{mBegin = rhs.mBegin; mEnd = rhs.mEnd; return *this;}

//-----------------------------------
//	API
//
public:
	// ----- Size -----
			uint32 		length() const							{return (uint32) (mEnd - mBegin);}
			uint32		size() const							{return (uint32) (mEnd - mBegin);}
			
			bool		empty() const							{return this->length() == 0;}

	// ----- Access -----
			char 		operator[](uint32 index) const			{ASSERT(index < this->length()); return mBegin[index];}	//  standard says that index == mLength returns 0...
		
			char 		at(uint32 index) const;
						// Throws a domain exception if index is out of range.

			char 		front() const							{ASSERT(!this->empty()); return mBegin[0];}
			char 		back() const							{ASSERT(!this->empty()); return mBegin[this->length() - 1];}

	// ----- Comparisons -----
			uint32 		find(const char* str, uint32 index = 0) const;

	friend 	bool 		operator==(const char* lhs, const FastString& rhs)	{return rhs.DoIsEqual(lhs);}
	friend 	bool 		operator==(const FastString& lhs, const char* rhs)	{return lhs.DoIsEqual(rhs);}
	friend 	bool 		operator==(const FastString& lhs, const FastString& rhs)	{return lhs.DoIsEqual(rhs);}

	friend 	bool 		operator!=(const char* lhs, const FastString& rhs)	{return !rhs.DoIsEqual(lhs);}
	friend 	bool 		operator!=(const FastString& lhs, const char* rhs)	{return !lhs.DoIsEqual(rhs);}
	friend 	bool 		operator!=(const FastString& lhs, const FastString& rhs)	{return !lhs.DoIsEqual(rhs);}

	// ----- Iterators -----
			const_iterator 			begin() const							{return mBegin;}
			const_iterator 			end() const								{return mBegin + this->length();}

			const_reverse_iterator 	rbegin() const							{return const_reverse_iterator(mBegin + this->length());}
			const_reverse_iterator 	rend() const							{return const_reverse_iterator(mBegin);}
			
	// ----- Misc -----
	friend	void 		swap(FastString& lhs, FastString& rhs)				{std::swap(lhs.mBegin, rhs.mBegin); std::swap(lhs.mEnd, rhs.mEnd);}

//-----------------------------------
//	Internal API
//
protected:
			bool 		DoIsEqual(const char* rhs) const;
			bool 		DoIsEqual(const FastString& rhs) const;

//-----------------------------------
//	Member Data
//
private:
	const char*		mBegin;
	const char*		mEnd;
};
	

