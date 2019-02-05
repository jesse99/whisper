/*
 *  File:		XString.h
 *  Summary:	Unicode capable string class (modeled after std::string).
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XString.h,v $
 *		Revision 1.2  2000/11/09 12:53:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <10>	 6/25/00	JDJ		Made the char* ctors explicit. Removed most of the methods that
 *									took char*'s.
 *		 <9>	 4/19/00	JDJ		operator== and != use memcmp instead of XString::compare.
 *		 <8>	 7/22/99	JDJ		mDirty now contains three bit flags (for mCData, mPData, and mCFData).
 *									Added CFString support.
 *		 <7>	 7/10/99	JDJ		swap is now a friend function so it now acts as a specialization
 *									of std::swap.
 *		 <6>	 6/24/99	JDJ		Added assign (const unsigned char*).
 *		 <5>	 6/21/99	JDJ		Added __MSL_FIX_ITERATORS__ (for Pro 3).
 *		 <4>	 2/22/99	JDJ		Methods that take a char* or XChar* and a length now
 *									have duplicate methods without the length argument (this
 *									corresponds to the standard and means we no longer
 *									attempt to find the length of a possibly unterminated
 *									string).
 *		 <3>	 2/12/99	JDJ		Added mCDataCapacity.
 *		 <2>	 1/25/99	JDJ		Added XTextTranscoder objects.
 *		 <1>	 9/17/98	JDJ		Created
 */

#pragma once

#include <climits> 
#include <iterator>

#include <XDebug.h>		

#if TARGET_CARBON
	#include <CFBase.h>			// $$$ the CoreFoundation's wacky includes causes problems with Pro5 so we'll do the including ourselves
	#include <CFArray.h>
	#include <CFData.h>
	#include <CFDictionary.h>
	#include <CFString.h>
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XTextTranscoder;


// ========================================================================================
//	class XString
// ========================================================================================
class CORE_EXPORT XString {

	friend class ZStringInitter;

//-----------------------------------
//	Types and Constants
//
public:
    typedef XChar*									iterator;
    typedef const XChar*							const_iterator;

#if MSVC >= 1100
    typedef std::reverse_iterator<const_iterator, const XChar>	const_reverse_iterator;
    typedef std::reverse_iterator<iterator, XChar>				reverse_iterator;
#else
    typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;
    typedef std::reverse_iterator<iterator>			reverse_iterator;
#endif

	enum {npos = ULONG_MAX};
	
protected:
	enum {kCDirty = 1, kPDirty = 2, kCFDirty = 4, kDirty = 1+2+4};

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XString();

						XString();
						XString(uint32 count, XChar ch);				
	explicit			XString(const char* str, XTextTranscoder* takeTranscoder = nil);
						XString(const char* str, uint32 length, XTextTranscoder* takeTranscoder = nil);
						XString(const XChar* str);
						XString(const XChar* str, uint32 length);
						XString(const XString& str, uint32 startIndex = 0, uint32 length = npos);
						// char*'s can be encoded in a variety of formats (eg ASCII, Mac OS Roman,
						// Windows Latin, etc). You can specify which transcoder to use. If a 
						// transcoder is specified it will be used whenever the string has to convert
						// to or from a char*. If a transcoder isn't specified a default transcoder
						// will be used (see SetDefaultTranscoder below). Note that if you precede
						// string literals with an 'L' the compiler will treat the string as a
						// wchar_t* so the transcoding step will be skipped (however these strings
						// may not be valid Unicode if there are character code's larger than 0x7F).

						XString(const std::wstring& str);	// $$$ temp

#if MAC
	explicit			XString(const unsigned char* str, uint32 length = npos);
						// Note that Pascal strings are always assumed to be Mac OS encoded.

			XString& 	assign(const unsigned char* str);
#endif

#if TARGET_CARBON
						XString(CFStringRef takeStr);
						
			XString&	operator=(CFStringRef takeStr)			{return this->assign(takeStr);}
			XString& 	assign(CFStringRef takeStr);
#endif

			XString& 	operator=(XChar ch)						{return this->assign(ch);}
			XString& 	operator=(const XChar* str)				{return this->assign(str);}
			XString&	operator=(const XString& str)			{return this->assign(str);}

			XString& 	assign(XChar ch);
			XString& 	assign(uint32 count, XChar ch);
			XString& 	assign(const char* str);
			XString& 	assign(const char* str, uint32 length);
			XString& 	assign(const XChar* str);
			XString& 	assign(const XChar* str, uint32 length);
			XString& 	assign(const XString& str, uint32 startIndex = 0, uint32 length = npos);

//-----------------------------------
//	API
//
public:
	// ----- Size -----
			uint32 		length() const							{return mLength;}
			uint32		size() const							{return mLength;}

			void 		resize(uint32 newSize);
			void 		resize(uint32 newSize, XChar ch);

			uint32 		capacity() const						{return mCapacity;}
			void 		reserve(uint32 cap);
			
			bool		empty() const							{return mLength == 0;}
			void 		clear();

	// ----- Access -----
			XChar 		operator[](uint32 index) const			{ASSERT(index <= mLength); return mData[index];}	//  standard says that index == mLength returns 0...
			XChar& 		operator[](uint32 index)				{ASSERT(index <= mLength); mDirty = kDirty; return mData[index];}
		
			XChar 		at(uint32 index) const;
			XChar& 		at(uint32 index);
						// Throws a domain exception if index is out of range.

			XChar 		front() const							{ASSERT(mLength > 0); return mData[0];}
			XChar& 		front()									{ASSERT(mLength > 0); mDirty = kDirty; return mData[0];}
			XChar 		back() const							{ASSERT(mLength > 0); return mData[mLength - 1];}
			XChar& 		back()									{ASSERT(mLength > 0); mDirty = kDirty; return mData[mLength - 1];}

			XChar* 		buffer()								{ASSERT(mData[mLength] == 0); return mData;}
			const XChar* buffer() const							{ASSERT(mData[mLength] == 0); return mData;}
			uint32 		buffer_length() const					{return mLength*sizeof(XChar);}

			const char* c_str() const							{if (mDirty & kCDirty) this->DoUpdateCData(); return mCData;}
			const XChar* c_str() const							{ASSERT(mData[mLength] == 0); return mData;}
#if MAC
			const unsigned char* p_str() const;
#elif WIN
			const TCHAR* t_str() const;
						// Using this is discouraged (at least within Whisper). Instead use
						// c_str when running on NT and c_str if on Win 95. 
#endif

#if TARGET_CARBON
			CFStringRef cf_str() const;
						// Returns a copy of a cached immutable Carbon string. Call CFRetain if you're going to keep
						// the CFString around after the XString is destroyed.
#endif

	// ----- Appending -----
			XString& 	operator+=(XChar ch)					{this->resize(mLength + 1, ch); return *this;}
			XString& 	operator+=(const XChar* str)			{return this->append(str);}
			XString& 	operator+=(const XString& str)			{return this->append(str);}

			XString& 	append(XChar ch)						{this->resize(mLength + 1, ch); return *this;}
			XString& 	append(uint32 count, XChar ch)			{this->resize(mLength + count, ch); return *this;}
			XString& 	append(const char* str);
			XString& 	append(const char* str, uint32 length);
			XString& 	append(const XChar* str);
			XString& 	append(const XChar* str, uint32 length);
			XString& 	append(const XString& str, uint32 startIndex = 0, uint32 length = npos);

			void		push_back(XChar ch)						{this->resize(mLength + 1, ch);}

	// ----- Inserting -----
			XString& 	insert(uint32 index, XChar ch);
			XString& 	insert(uint32 index, uint32 count, XChar ch);
			XString& 	insert(uint32 index, const XChar* str);
			XString& 	insert(uint32 index, const XChar* str, uint32 length);
			XString& 	insert(uint32 index, const XString& str, uint32 startIndex = 0, uint32 length = npos);

	// ----- Erasing -----
     		XString& 	erase(uint32 index = 0, uint32 count = npos); 

			void		pop_back()								{this->erase(mLength - 1);}

	// ----- Replacing -----
			XString& 	replace(uint32 index, uint32 count, uint32 chCount, XChar ch);
			XString& 	replace(uint32 index, uint32 count, const XChar* str);
			XString& 	replace(uint32 index, uint32 count, const XChar* str, uint32 length);
			XString& 	replace(uint32 index, uint32 count, const XString& str, uint32 startIndex = 0, uint32 length = npos);
						// Removes count characters starting at index and inserts the new
						// characters at index.

	// ----- Find -----
			uint32 		find(XChar ch, uint32 index = 0) const;
			uint32 		find(const XChar* str, uint32 index = 0) const;
			uint32 		find(const XChar* str, uint32 index, uint32 length) const;
			uint32 		find(const XString& str, uint32 index = 0) const;
						// Returns the index of the first occurance of str after index
						// or npos if not found. length refers to str.
			
			uint32 		rfind(XChar ch, uint32 index = npos) const;
			uint32 		rfind(const XChar* str, uint32 index = npos) const;
			uint32 		rfind(const XChar* str, uint32 index , uint32 length) const;
			uint32	 	rfind(const XString& str, uint32 index = npos) const;
						// Returns the index of the last occurance of str after index
						// or npos if not found.
		
			uint32 		find_first_of(XChar ch, uint32 index = 0) const							{return this->find_first_of(XString(1, ch), index);}
			uint32 		find_first_of(const XChar* str, uint32 index = 0) const					{return this->find_first_of(XString(str), index);}
			uint32 		find_first_of(const XChar* str, uint32 index , uint32 length) const		{return this->find_first_of(XString(str, length), index);}
			uint32 		find_first_of(const XString& str, uint32 index = 0) const;
						// Returns the index of the first occurance of a character in 
						// str after index or npos if not found.
		
			uint32 		find_last_of(XChar ch, uint32 index = npos) const						{return this->find_last_of(XString(1, ch), index);}
			uint32 		find_last_of(const XChar* str, uint32 index = npos) const				{return this->find_last_of(XString(str), index);}
			uint32 		find_last_of(const XChar* str, uint32 index, uint32 length) const		{return this->find_last_of(XString(str, length), index);}
			uint32 		find_last_of(const XString& str, uint32 index = npos) const;
						// Returns the index of the last occurance of a character in 
						// str before index or npos if not found.
				
			uint32 		find_first_not_of(XChar ch, uint32 index = 0) const						{return this->find_first_not_of(XString(1, ch), index);}
			uint32 		find_first_not_of(const XChar* str, uint32 index = 0) const				{return this->find_first_not_of(XString(str), index);}
			uint32 		find_first_not_of(const XChar* str, uint32 index, uint32 length) const	{return this->find_first_not_of(XString(str, length), index);}
			uint32 		find_first_not_of(const XString& str, uint32 index = 0) const;
						// Returns the index of the first occurance of a character not in 
						// str after index or npos if not found.
		
			uint32 		find_last_not_of(XChar ch, uint32 index = npos) const					{return this->find_last_not_of(XString(1, ch), index);}
			uint32 		find_last_not_of(const XChar* str, uint32 index = npos) const			{return this->find_last_not_of(XString(str), index);}
			uint32 		find_last_not_of(const XChar* str, uint32 index, uint32 length) const	{return this->find_last_not_of(XString(str, length), index);}
			uint32	 	find_last_not_of(const XString& str, uint32 index = npos) const;
						// Returns the index of the last occurance of a character not in 
						// str before index or npos if not found.

	// ----- Concatenation -----
	friend 	XString 	operator+(const XString& lhs, const XString& rhs);
	friend 	XString 	operator+(const XChar* lhs, const XString& rhs);
	friend 	XString 	operator+(XChar lhs, const XString& rhs);
	friend 	XString 	operator+(const XString& lhs, const XChar* rhs);
	friend 	XString 	operator+(const XString& lhs, XChar rhs);

	// ----- Comparisons -----
	friend 	bool 		operator==(XChar lhs, const XString& rhs)			{return rhs.length() == 1 && rhs[0] == lhs;}
	friend 	bool 		operator==(const XChar* lhs, const XString& rhs)	{return rhs.DoIsEqual(lhs);}
	friend 	bool 		operator==(const XString& lhs, XChar rhs)			{return lhs.length() == 1 && lhs[0] == rhs;}
	friend 	bool 		operator==(const XString& lhs, const XChar* rhs)	{return lhs.DoIsEqual(rhs);}
	friend 	bool 		operator==(const XString& lhs, const XString& rhs)	{return lhs.DoIsEqual(rhs);}
	
	friend 	bool 		operator!=(XChar lhs, const XString& rhs)			{return rhs.length() != 1 || rhs[0] != lhs;}
	friend 	bool 		operator!=(const XChar* lhs, const XString& rhs)	{return !rhs.DoIsEqual(lhs);}
	friend 	bool 		operator!=(const XString& lhs, XChar rhs)			{return lhs.length() != 1 || lhs[0] != rhs;}
	friend 	bool 		operator!=(const XString& lhs, const XChar* rhs)	{return !lhs.DoIsEqual(rhs);}
	friend 	bool 		operator!=(const XString& lhs, const XString& rhs)	{return !lhs.DoIsEqual(rhs);}
		
	friend 	bool 		operator<(const XString& lhs, const XString& rhs)	{return lhs.compare(rhs) < 0;}
	friend 	bool 		operator<=(const XString& lhs, const XString& rhs)	{return lhs.compare(rhs) <= 0;}
	friend 	bool 		operator>(const XString& lhs, const XString& rhs)	{return lhs.compare(rhs) > 0;}
	friend 	bool 		operator>=(const XString& lhs, const XString& rhs)	{return lhs.compare(rhs) >= 0;}
						// Note that these do a bitwise comparison. If you need to do a more
						// intelligent ordering use CompareStrings in <XStringUtils.h>.

			int 		compare(const XChar* rhs) const;
			int 		compare(const XString& rhs) const;
						// Returns -1 if this < rhs, 0 if this == rhs, or +1 if this > rhs.

			int 		compare(uint32 index, uint32 count, const XChar* rhs, uint32 length = npos) const;
			int 		compare(uint32 index, uint32 count, const XString& rhs, uint32 startIndex = 0, uint32 length = npos) const;

	// ----- Iterators -----
			iterator 				begin()									{return mData;}
			const_iterator 			begin() const							{return mData;}

			iterator 				end()									{return mData + mLength;}
			const_iterator 			end() const								{return mData + mLength;}

			reverse_iterator 		rbegin()								{return reverse_iterator(mData + mLength);}
			const_reverse_iterator 	rbegin() const							{return const_reverse_iterator(mData + mLength);}

			reverse_iterator 		rend()									{return reverse_iterator(mData);}
			const_reverse_iterator 	rend() const							{return const_reverse_iterator(mData);}
			
	// ----- Encoding -----
			void 		SetTranscoder(XTextTranscoder* takeTranscoder);
			
	static	void 		SetDefaultTranscoder(XTextTranscoder* takeTranscoder);
						// Used when converting to and from char*'s. Defaults to XMacRomanTranscoder
						// on Macs and XWindowsLatin1Transcoder on Windows. Note that you can use
						// XNaiveTranscoder if the default transcoders are too slow (and you're
						// willing to risk generating bogus Unicode).
						
	// ----- Misc -----
			XString 	substr(uint32 index = 0, uint32 count = npos) const;
		
#if MAC
			uint32 		copy(unsigned char* buffer, uint32 count, uint32 index = 0) const;
#endif
			uint32 		copy(char* buffer, uint32 count, uint32 index = 0) const;
			uint32 		copy(XChar* buffer, uint32 count, uint32 index = 0) const;
						// Copies up to count characters into buffer. First character copied is at index.
						// Returns number of characters that were copied. Note that these methods merely
						// copy the characters: they don't do things like appending NULLs.

	friend	void 		swap(XString& lhs, XString& rhs);

//-----------------------------------
//	Internal API
//
protected:
	enum {kBufferSize = 32};
	
			void 		DoResize(uint32 count, bool copyData);

			void 		DoSetDirty();

			void 		DoUpdateCData() const;

#if MAC		
			void 		DoUpdatePData() const;
#endif

#if TARGET_CARBON		
			void 		DoUpdateCFData() const;
#endif
			
			bool 		DoIsEqual(const XChar* rhs) const					{return std::memcmp(mData, rhs, (mLength+1)*sizeof(XChar)) == 0;}
			bool 		DoIsEqual(const XString& rhs) const					{return mLength == rhs.mLength && std::memcmp(mData, rhs.mData, mLength*sizeof(XChar)) == 0;}

//-----------------------------------
//	Member Data
//
private:
	XChar*						mData;						// if mLength <= kBufferSize then mData points to mBuffer, otherwise mData is allocated from the heap 
	uint32						mLength;	
	uint32						mCapacity;
	
	XChar						mBuffer[kBufferSize + 1];	// most strings are small so we use this to avoid overhead from heap allocations and deletions (add one for trailing zero)

	mutable uint8				mDirty;						
	mutable char*				mCData;
	mutable uint32 				mCDataCapacity;

#if MAC
	mutable unsigned char*		mPData;						// $$ we could merge this into mCData, but in most cases this doesn't seem like it would save much space
#endif
	
#if TARGET_CARBON
	mutable CFMutableStringRef	mCFString;
#endif

	XTextTranscoder*			mTranscoder;

	static XTextTranscoder*		msDefaultTranscoder;
};
	

// ========================================================================================
//	Inlines
// ========================================================================================
#if !DEBUG
	inline void XString::DoSetDirty()
	{
		mDirty = kDirty;
	}
#endif

#if WIN
	inline const TCHAR* XString::t_str() const
	{		
		ASSERT(mData[mLength] == 0);

#ifdef UNICODE      
		return mData;
#else
		if (mDirty & kCDirty) 
			this->DoUpdateCData(); 
		return mCData;
#endif
	}
#endif

#if __MWERKS__ > 0 && __MWERKS__ < 0x2200
   __MSL_FIX_ITERATORS__(Whisper::XString);
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
