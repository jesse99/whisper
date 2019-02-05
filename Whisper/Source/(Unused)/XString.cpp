/*
 *  File:		XString.cpp
 *  Summary:	Unicode capable string class (modeled after std::string).
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XString.cpp,v $
 *		Revision 1.4  2001/04/27 04:36:40  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2000/12/10 04:11:50  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:53:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <16>	 6/25/00	JDJ		Made the char* ctors explicit. Removed most of the methods that
 *									took char*'s.
 *		 <15>	 7/22/99	JDJ		mDirty now contains three bit flags (for mCData, mPData, and mCFData).
 *									Added CFString support.
 *		 <14>	 7/10/99	JDJ		swap is now a friend function so it now acts as a specialization
 *									of std::swap.
 *		 <13>	 7/09/99	JDJ		Fixed several bugs in swap.
 *		 <12>	 6/24/99	JDJ		Added assign (const unsigned char*).
 *		 <11>	 5/28/99	JDJ		Made thread safe.
 *		 <10>	 4/19/99	JDJ		DoSetDirty builds mCData if __MWERKS__ (instead of if MAC).
 *		 <9>	 2/22/99	JDJ		Methods that take a char* or XChar* and a length now
 *									have duplicate methods without the length argument (this
 *									corresponds to the standard and means we no longer
 *									attempt to find the length of a possibly unterminated
 *									string).
 *		 <8>	 2/15/99	JDJ		Rewrote replace methods to be faster (they do all the work
 *									themselves instead of calling erase() and insert()).
 *		 <7>	 2/12/99	JDJ		Added mCDataCapacity which makes DoResetDirty slightly
 *									faster and DoSetDirty a lot faster (on Macs in debug builds).
 *		 <6>	 2/09/99	JDJ		DoResize no longer does extra work when shrinking strings
 *									using mBuffer.
 *		 <5>	 1/29/99	JDJ		Uses wcslen instead of a custom wstrlen.
 *		 <4>	 1/28/99	JDJ		Added COMPILE_CHECK to make sure wchar_t is 16-bits.
 *		 <3>	 1/25/99	JDJ		Added XTextTranscoder objects.
 *		 <2>	 1/22/98	JDJ		DoResetDirty no longer checks characters with codes
 *									larger than 127 against mCData and mPData.
 *		 <1>	 9/17/98	JDJ		Created
 */
 
#include <XWhisperHeader.h>
#include <XString.h>		

#include <algorithm>
#include <stdexcept>

#include <XCriticalSection.h>
#include <XExceptions.h>
#include <XNumbers.h>
#include <XTextTranscoders.h>		
#include <XTinyVector.h>

namespace Whisper {


// ========================================================================================
//	class ZStringInitter
// ========================================================================================
class ZStringInitter {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~ZStringInitter();

						ZStringInitter();
						
			void 		Create();
};

static ZStringInitter 	sStringInitter;
static XCriticalSection	sTranscoderSection;

//---------------------------------------------------------------
//
// ZStringInitter::~ZStringInitter
//
//---------------------------------------------------------------
ZStringInitter::~ZStringInitter()
{
	delete XString::msDefaultTranscoder;
	XString::msDefaultTranscoder = nil;
}


//---------------------------------------------------------------
//
// ZStringInitter::ZStringInitter
//
//---------------------------------------------------------------
ZStringInitter::ZStringInitter()
{
	this->Create();
}


//---------------------------------------------------------------
//
// ZStringInitter::Create
//
// Need to jump through these hoops because typically another
// compilation unit will be initialized before this one so we
// can't rely on sStringInitter to initialize msDefaultTranscoder.
//
//---------------------------------------------------------------
void ZStringInitter::Create()
{
	if (XString::msDefaultTranscoder == nil) {
//		XEnterCriticalSection enter(sTranscoderSection);	// $$ order of initialization problem

		if (XString::msDefaultTranscoder == nil) 
#if MAC
			XString::msDefaultTranscoder = new XMacRomanTranscoder;
#elif WIN
			XString::msDefaultTranscoder = new XWindowsLatin1Transcoder;
#endif
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ========================================================================================
//	Global Functions
// ========================================================================================

XString operator+(const XString& lhs, const XString& rhs)	{XString result = lhs; result += rhs; return result;}
XString operator+(const XChar* lhs, const XString& rhs)		{XString result = lhs; result += rhs; return result;}
XString operator+(XChar lhs, const XString& rhs)			{XString result(1, lhs); result += rhs; return result;}
XString operator+(const XString& lhs, const XChar* rhs)		{XString result = lhs; result += rhs; return result;}
XString operator+(const XString& lhs, XChar rhs)			{XString result = lhs; result += rhs; return result;}
		// None of these get inlined by CW 5...
		
#if __MWERKS__
#pragma mark -
#endif

// ========================================================================================
//	class XString
// ========================================================================================

XTextTranscoder* XString::msDefaultTranscoder = nil;

//---------------------------------------------------------------
//
// XString::~XString
//
//---------------------------------------------------------------
XString::~XString()
{
	if (mCapacity > kBufferSize) 
		delete [] mData;

	delete [] mCData;

#if MAC
	delete [] mPData;
#endif

#if TARGET_CARBON
	if (mCFString != nil)
		CFRelease(mCFString);
#endif

	delete mTranscoder;

	mData = nil;
}


//---------------------------------------------------------------
//
// XString::XString ()
//
//---------------------------------------------------------------
XString::XString()
{
	COMPILE_CHECK(sizeof(XChar) == 2);	// XChar is typedefed as a wchar_t to allow for better type checking and to support wide string literals, however wchar_t can be of any size, if it's 8 bits we're hosed, if it's 32-bits things should still work except we won't be writing out valid utf-16 streams...
	
	mData     = mBuffer;
	mLength   = 0;
	mCapacity = kBufferSize;

	mData[mLength] = 0;

	mDirty = 0;
	
	mCData = nil;
	mCDataCapacity = 0;
#if MAC
	mPData = nil;
#endif
#if TARGET_CARBON
	mCFString = nil;
#endif

	mTranscoder = nil;			// defer creating this until we need it

	this->DoSetDirty();
}


//---------------------------------------------------------------
//
// XString::XString (uint32, XChar)
//
//---------------------------------------------------------------
XString::XString(uint32 count, XChar ch)
{
	if (count > kBufferSize) {
		mData     = new XChar[count + 1];
		mCapacity = count;
	
	} else {
		mData     = mBuffer;
		mCapacity = kBufferSize;
	}	
	
	mLength = count;
	for (uint32 index = 0; index < mLength; ++index)
		mData[index] = ch;
	mData[mLength] = 0;

	mCData = nil;
	mCDataCapacity = 0;
#if MAC
	mPData = nil;
#endif
#if TARGET_CARBON
	mCFString = nil;
#endif

	mTranscoder = nil;			// defer creating this until we need it

	this->DoSetDirty();
}
				

//---------------------------------------------------------------
//
// XString::XString (const char*, XTextTranscoder*)
//
//---------------------------------------------------------------
XString::XString(const char* str, XTextTranscoder* takeTranscoder)
{
	PRECONDITION(str != nil);
	
	if (msDefaultTranscoder == nil)
		sStringInitter.Create();
	
	if (takeTranscoder == nil)
		mTranscoder = msDefaultTranscoder->Clone();	// each XString needs a seperate transcoder so that we can stay thread safe
	else
		mTranscoder = takeTranscoder;
		
	uint32 srcLen = STD::strlen(str);
	uint32 dstLen = mTranscoder->GetDstBytes(str, srcLen)/2;

	if (dstLen > kBufferSize) {
		mData     = new XChar[dstLen + 1];
		mCapacity = dstLen;
	
	} else {
		mData     = mBuffer;
		mCapacity = kBufferSize;
	}	
		
	mLength = mTranscoder->ConvertToUTF16(str, srcLen, mData, dstLen*sizeof(XChar))/2;
	ASSERT(mLength <= dstLen);
	mData[mLength] = '\0';

	mCData = nil;
	mCDataCapacity = 0;
#if MAC
	mPData = nil;
#endif
#if TARGET_CARBON
	mCFString = nil;
#endif

	this->DoSetDirty();
}


//---------------------------------------------------------------
//
// XString::XString (const char*, uint32, XTextTranscoder*)
//
//---------------------------------------------------------------
XString::XString(const char* str, uint32 length, XTextTranscoder* takeTranscoder)
{
	PRECONDITION(str != nil);
	PRECONDITION(length < XString::npos);

	if (msDefaultTranscoder == nil)
		sStringInitter.Create();

	if (takeTranscoder == nil)
		mTranscoder = msDefaultTranscoder->Clone();	// each XString needs a seperate transcoder so that we can stay thread safe
	else
		mTranscoder = takeTranscoder;
		
	uint32 srcLen = length;
	uint32 dstLen = mTranscoder->GetDstBytes(str, srcLen)/2;

	if (dstLen > kBufferSize) {
		mData     = new XChar[dstLen + 1];
		mCapacity = dstLen;

	} else {
		mData     = mBuffer;
		mCapacity = kBufferSize;
	}	
		
	mLength = mTranscoder->ConvertToUTF16(str, srcLen, mData, dstLen*sizeof(XChar))/2;
	ASSERT(mLength <= dstLen);
	mData[mLength] = '\0';

	mCData = nil;
	mCDataCapacity = 0;

#if MAC
	mPData = nil;
#endif
#if TARGET_CARBON
	mCFString = nil;
#endif

	this->DoSetDirty();
}


//---------------------------------------------------------------
//
// XString::XString (const unsigned char*, uint32)
//
//---------------------------------------------------------------
#if MAC
XString::XString(const unsigned char* str, uint32 length)
{
	PRECONDITION(str != nil);
	
	XMacRomanTranscoder transcoder;
	XDisableUTF16Warning disabler;
	
	uint32 srcLen = Min((uint32) str[0], length);
	uint32 dstLen = transcoder.GetDstBytes((char*) str+1, srcLen)/2;

	if (dstLen > kBufferSize) {
		mData     = new XChar[dstLen + 1];
		mCapacity = dstLen;
	
	} else {
		mData     = mBuffer;
		mCapacity = kBufferSize;
	}	
	
	mLength = transcoder.ConvertToUTF16((char*) str+1, srcLen, mData, dstLen*sizeof(XChar))/2;
	ASSERT(mLength <= dstLen);
	mData[mLength] = '\0';

	mCData = nil;	
	mCDataCapacity = 0;
#if MAC
	mPData = nil;
#endif
#if TARGET_CARBON
	mCFString = nil;
#endif

	mTranscoder = nil;			// defer creating this until we need it

	this->DoSetDirty();
}
#endif


//---------------------------------------------------------------
//
// XString::XString (const XChar*)
//
//---------------------------------------------------------------
XString::XString(const XChar* str)
{
	PRECONDITION(str != nil);	

	uint32 count = STD::wcslen(str);

	if (count > kBufferSize) {
		mData     = new XChar[count + 1];
		mCapacity = count;

	} else {
		mData     = mBuffer;
		mCapacity = kBufferSize;
	}	

	mLength = count;
	BlockMoveData(str, mData, mLength*sizeof(XChar));
	mData[mLength] = 0;

	mCData = nil;	
	mCDataCapacity = 0;
#if MAC
	mPData = nil;
#endif
#if TARGET_CARBON
	mCFString = nil;
#endif

	mTranscoder = nil;			// defer creating this until we need it

	this->DoSetDirty();
}


//---------------------------------------------------------------
//
// XString::XString (std::wstring)
//
//---------------------------------------------------------------
XString::XString(const std::wstring& str)
{
	uint32 count = str.length();

	if (count > kBufferSize) {
		mData     = new XChar[count + 1];
		mCapacity = count;

	} else {
		mData     = mBuffer;
		mCapacity = kBufferSize;
	}	

	mLength = count;
	BlockMoveData(str.c_str(), mData, mLength*sizeof(XChar));
	mData[mLength] = 0;

	mCData = nil;	
	mCDataCapacity = 0;
#if MAC
	mPData = nil;
#endif
#if TARGET_CARBON
	mCFString = nil;
#endif

	mTranscoder = nil;			// defer creating this until we need it

	this->DoSetDirty();
}


//---------------------------------------------------------------
//
// XString::XString (const XChar*, uint32)
//
//---------------------------------------------------------------
XString::XString(const XChar* str, uint32 length)
{
	PRECONDITION(str != nil);
	PRECONDITION(length < XString::npos);

	uint32 count = length;

	if (count > kBufferSize) {
		mData     = new XChar[count + 1];
		mCapacity = count;
	
	} else {
		mData     = mBuffer;
		mCapacity = kBufferSize;
	}	
	
	mLength = count;
	BlockMoveData(str, mData, mLength*sizeof(XChar));
	mData[mLength] = 0;

	mCData = nil;	
	mCDataCapacity = 0;
#if MAC
	mPData = nil;
#endif
#if TARGET_CARBON
	mCFString = nil;
#endif

	mTranscoder = nil;			// defer creating this until we need it

	this->DoSetDirty();
}


//---------------------------------------------------------------
//
// XString::XString (XString, uint32, uint32)
//
//---------------------------------------------------------------
XString::XString(const XString& str, uint32 startIndex, uint32 length)
{
	PRECONDITION(startIndex <= str.length());		// $$$ standard says that out_of_range should be thrown for these
		
	uint32 count = Min(str.length() - startIndex, length);

	if (count > kBufferSize) {
		mData     = new XChar[count + 1];
		mCapacity = count;
	
	} else {
		mData     = mBuffer;
		mCapacity = kBufferSize;
	}	
	
	mLength = count;
	BlockMoveData(str.buffer() + startIndex, mData, mLength*sizeof(XChar));
	mData[mLength] = 0;

	mCData = nil;	
	mCDataCapacity = 0;
#if MAC
	mPData = nil;
#endif
#if TARGET_CARBON
	mCFString = nil;
#endif

	if (str.mTranscoder != nil)
		mTranscoder = str.mTranscoder->Clone();
	else
		mTranscoder = nil;			// defer creating this until we need it

	this->DoSetDirty();
}


//---------------------------------------------------------------
//
// XString::XString (CFStringRef)
//
//---------------------------------------------------------------
#if TARGET_CARBON
XString::XString(CFStringRef takeStr)
{
	PRECONDITION(takeStr != nil);
	COMPILE_CHECK(sizeof(XChar) == sizeof(UniChar));

	uint32 count = numeric_cast<uint32>(CFStringGetLength(takeStr));

	if (count > kBufferSize) {
		try {
			mData     = new XChar[count + 1];
			mCapacity = count;
		
		} catch (...) {
			CFRelease(takeStr);
			throw;
		}
	
	} else {
		mData     = mBuffer;
		mCapacity = kBufferSize;
	}	
	
	const UniChar* chars = CFStringGetCharactersPtr(takeStr);
	if (chars != nil) {
		BlockMoveData(chars, mData, count*sizeof(XChar));
	
	} else {		
		CFRange range;
		range.location = 0;
		range.length   = numeric_cast<int32>(count);
		CFStringGetCharacters(takeStr, range, (UniChar*) mData);
	}
	
	CFRelease(takeStr);
	takeStr = nil;

	mLength = count;
	mData[mLength] = 0;

	mCData = nil;	
	mCDataCapacity = 0;
#if MAC
	mPData = nil;
#endif
#if TARGET_CARBON
	mCFString = nil;
#endif

	mTranscoder = nil;			// defer creating this until we need it

	this->DoSetDirty();
}
#endif


//---------------------------------------------------------------
//
// XString::assign (XChar)
//
//---------------------------------------------------------------
XString& XString::assign(XChar ch)
{
	this->DoResize(1, false);
	
	mData[0] = ch;

	this->DoSetDirty();
	
	return *this;
}


//---------------------------------------------------------------
//
// XString::assign (uint32, XChar)
//
//---------------------------------------------------------------
XString& XString::assign(uint32 count, XChar ch)
{
	this->DoResize(count, false);
	
	for (uint32 index = 0; index < count; ++index)
		mData[index] = ch;
	
	this->DoSetDirty();

	return *this;
}


//---------------------------------------------------------------
//
// XString::assign (const char*)
//
//---------------------------------------------------------------
XString& XString::assign(const char* str)
{
	PRECONDITION(str != nil);

	if (msDefaultTranscoder == nil)
		sStringInitter.Create();

	if (mTranscoder == nil)
		mTranscoder = msDefaultTranscoder->Clone();

	uint32 srcLen = STD::strlen(str);
	uint32 dstLen = mTranscoder->GetDstBytes(str, srcLen)/2;

	this->DoResize(dstLen, false);

	mLength = mTranscoder->ConvertToUTF16(str, srcLen, mData, dstLen*sizeof(XChar))/2;
	ASSERT(mLength <= dstLen);
	mData[mLength] = '\0';

	this->DoSetDirty();

	return *this;
}


//---------------------------------------------------------------
//
// XString::assign (const char*, uint32)
//
//---------------------------------------------------------------
XString& XString::assign(const char* str, uint32 length)
{
	PRECONDITION(str != nil);
	PRECONDITION(length < XString::npos);

	if (msDefaultTranscoder == nil)
		sStringInitter.Create();
	
	if (mTranscoder == nil)
		mTranscoder = msDefaultTranscoder->Clone();

	uint32 srcLen = length;
	uint32 dstLen = mTranscoder->GetDstBytes(str, srcLen)/2;

	this->DoResize(dstLen, false);
	
	mLength = mTranscoder->ConvertToUTF16(str, srcLen, mData, dstLen*sizeof(XChar))/2;
	ASSERT(mLength <= dstLen);
	mData[mLength] = '\0';
	
	this->DoSetDirty();

	return *this;
}


//---------------------------------------------------------------
//
// XString::assign (const XChar*)
//
//---------------------------------------------------------------
XString& XString::assign(const XChar* str)
{
	PRECONDITION(str != nil);

	uint32 count = STD::wcslen(str);

	this->DoResize(count, false);

	BlockMoveData(str, mData, count*sizeof(XChar));

	this->DoSetDirty();

	return *this;
}


//---------------------------------------------------------------
//
// XString::assign (const XChar*, uint32)
//
//---------------------------------------------------------------
XString& XString::assign(const XChar* str, uint32 length)
{
	PRECONDITION(str != nil);
	PRECONDITION(length < XString::npos);
	
	uint32 count = length;

	this->DoResize(count, false);
	
	BlockMoveData(str, mData, count*sizeof(XChar));
	
	this->DoSetDirty();

	return *this;
}


//---------------------------------------------------------------
//
// XString::assign (XString, uint32, uint32)
//
//---------------------------------------------------------------
XString& XString::assign(const XString& str, uint32 startIndex, uint32 length)
{
	PRECONDITION(startIndex <= str.length());
		
	uint32 count = Min(str.length() - startIndex, length);

	this->DoResize(count, false);
	
	if (str.mTranscoder != nil) {
		XTextTranscoder* transcoder = str.mTranscoder->Clone();
		delete mTranscoder;
		mTranscoder = transcoder;
	}
	
	BlockMoveData(str.buffer() + startIndex, mData, count*sizeof(XChar));
	
	this->DoSetDirty();

	return *this;
}


//---------------------------------------------------------------
//
// XString::assign (const unsigned char*)
//
//---------------------------------------------------------------
#if MAC
XString& XString::assign(const unsigned char* str)
{
	PRECONDITION(str != nil);

	XMacRomanTranscoder transcoder;
	XDisableUTF16Warning disabler;

	uint32 srcLen = *str;
	uint32 dstLen = transcoder.GetDstBytes((char*) str+1, srcLen)/2;

	this->DoResize(dstLen, false);

	mLength = transcoder.ConvertToUTF16((char*) str+1, srcLen, mData, dstLen*sizeof(XChar))/2;
	ASSERT(mLength <= dstLen);
	mData[mLength] = '\0';

	this->DoSetDirty();

	return *this;
}
#endif


//---------------------------------------------------------------
//
// XString::assign (CFStringRef)
//
//---------------------------------------------------------------
#if TARGET_CARBON
XString& XString::assign(CFStringRef takeStr)
{
	PRECONDITION(takeStr != nil);
	
	uint32 count = numeric_cast<uint32>(CFStringGetLength(takeStr));

	try {
		this->DoResize(count, false);
	} catch (...) {
		CFRelease(takeStr);
		throw;
	}
	
	const UniChar* chars = CFStringGetCharactersPtr(takeStr);
	if (chars != nil) {
		BlockMoveData(chars, mData, count*sizeof(XChar));
	
	} else {		
		CFRange range;
		range.location = 0;
		range.length   = numeric_cast<int32>(count);
		CFStringGetCharacters(takeStr, range, (UniChar*) mData);
	}
	
	CFRelease(takeStr);
	this->DoSetDirty();

	return *this;
}
#endif


//---------------------------------------------------------------
//
// XString::resize (uint32)
//
//---------------------------------------------------------------
void XString::resize(uint32 newSize)
{
	if (newSize > mLength) {
		uint32 oldCount = mLength;
		
		this->DoResize(newSize, true);
		
		STD::memset(mData + oldCount, 0, (mLength - oldCount)*sizeof(XChar));
	
		this->DoSetDirty();

	} else if (newSize < mLength) {
		this->DoResize(newSize, true);
		this->DoSetDirty();
	}
}


//---------------------------------------------------------------
//
// XString::resize (uint32, XChar)
//
//---------------------------------------------------------------
void XString::resize(uint32 newSize, XChar ch)
{
	if (newSize > mLength) {
		uint32 oldCount = mLength;
		
		this->DoResize(newSize, true);
			
		for (uint32 index = oldCount; index < mLength; ++index)
			mData[index] = ch;
	
		this->DoSetDirty();

	} else if (newSize < mLength) {
		this->DoResize(newSize, true);
		this->DoSetDirty();
	}
}


//---------------------------------------------------------------
//
// XString::reserve
//
//---------------------------------------------------------------
void XString::reserve(uint32 cap)
{
	if (cap > mCapacity) {
		XChar* newData = nil;
		uint32 newCap = 0;
		
		if (cap > kBufferSize) {
			newCap  = cap;
			newData = new (std::nothrow) XChar[newCap + 1];
		
		} else {
			newData = mBuffer;
			newCap  = kBufferSize;
		}	

		if (newData != nil) {
			if (newData != mData) {
				BlockMoveData(mData, newData, (mLength + 1)*sizeof(XChar));

				this->DoSetDirty();
			}
			
			if (mCapacity > kBufferSize) 
				delete [] mData;
				
			mData     = newData;
			mCapacity = newCap;
		}
	}
	
	POSTCONDITION(mCapacity >= mLength);
	POSTCONDITION(mData == mBuffer || mCapacity > kBufferSize);
}


//---------------------------------------------------------------
//
// XString::clear
//
//---------------------------------------------------------------
void XString::clear()
{
	if (mLength > 0) {
		this->DoResize(0, false);
		this->DoSetDirty();
	}
}


//---------------------------------------------------------------
//
// XString::at const
//
//---------------------------------------------------------------
XChar XString::at(uint32 index) const
{
	if (index >= mLength)
		throw std::out_of_range("Bad index in XString::at");
		
	return mData[index];
}


//---------------------------------------------------------------
//
// XString::at
//
//---------------------------------------------------------------
XChar& XString::at(uint32 index)
{
	if (index >= mLength)
		throw std::out_of_range("Bad index in XString::at");
		
	mDirty = kDirty;
		
	return mData[index];
}


//---------------------------------------------------------------
//
// XString::p_str
//
//---------------------------------------------------------------
#if MAC
const unsigned char* XString::p_str() const
{
	if (mLength > 255)
		throw std::range_error("XString::p_str is limited to strings with sizes less than 256");
		
	if (mDirty & kPDirty) 
		this->DoUpdatePData(); 
	
	return mPData;
}
#endif


//---------------------------------------------------------------
//
// XString::cf_str
//
//---------------------------------------------------------------
#if TARGET_CARBON
CFStringRef XString::cf_str() const
{
	if (mDirty & kCFDirty) 
		this->DoUpdateCFData(); 
	
	return mCFString;
}
#endif


//---------------------------------------------------------------
//
// XString::append (const char*)
//
//---------------------------------------------------------------
XString& XString::append(const char* str)
{
	PRECONDITION(str != nil);	

	if (msDefaultTranscoder == nil)
		sStringInitter.Create();

	uint32 srcLen = STD::strlen(str);

	if (srcLen > 0) {
		if (mTranscoder == nil)
			mTranscoder = msDefaultTranscoder->Clone();

		uint32 dstLen = mTranscoder->GetDstBytes(str, srcLen)/2;

		uint32 oldLength = mLength;
		this->DoResize(mLength + dstLen, true);

		mLength = oldLength + mTranscoder->ConvertToUTF16(str, srcLen, mData + oldLength, dstLen*sizeof(XChar))/2;
		ASSERT(mLength - oldLength <= dstLen);
		mData[mLength] = '\0';

		this->DoSetDirty();
	}	

	POSTCONDITION(mData[mLength] == 0);

	return *this;
}


//---------------------------------------------------------------
//
// XString::append (const char*, uint32)
//
//---------------------------------------------------------------
XString& XString::append(const char* str, uint32 length)
{
	PRECONDITION(str != nil);
	PRECONDITION(length < XString::npos);
	
	if (msDefaultTranscoder == nil)
		sStringInitter.Create();
	
	uint32 srcLen = length;
	if (srcLen > 0) {
		if (mTranscoder == nil)
			mTranscoder = msDefaultTranscoder->Clone();
	
		uint32 dstLen = mTranscoder->GetDstBytes(str, srcLen)/2;
		
		uint32 oldLength = mLength;
		this->DoResize(mLength + dstLen, true);
		
		mLength = oldLength + mTranscoder->ConvertToUTF16(str, srcLen, mData + oldLength, dstLen*sizeof(XChar))/2;
		ASSERT(mLength - oldLength <= dstLen);
		mData[mLength] = '\0';

		this->DoSetDirty();
	}
		
	POSTCONDITION(mData[mLength] == 0);
	
	return *this;
}


//---------------------------------------------------------------
//
// XString::append (const XChar*)
//
//---------------------------------------------------------------
XString& XString::append(const XChar* str)
{
	PRECONDITION(str != nil);

	uint32 count = STD::wcslen(str);
	if (count > 0) {
		uint32 oldLength = mLength;
		this->DoResize(mLength + count, true);

		BlockMoveData(str, mData + oldLength, count*sizeof(XChar));

		this->DoSetDirty();
	}	

	POSTCONDITION(mData[mLength] == 0);

	return *this;
}


//---------------------------------------------------------------
//
// XString::append (const XChar*, uint32)
//
//---------------------------------------------------------------
XString& XString::append(const XChar* str, uint32 length)
{
	PRECONDITION(str != nil);
	PRECONDITION(length < XString::npos);

	
	uint32 count = length;
	if (count > 0) {
		uint32 oldLength = mLength;
		this->DoResize(mLength + count, true);
		
		BlockMoveData(str, mData + oldLength, count*sizeof(XChar));

		this->DoSetDirty();
	}
		
	POSTCONDITION(mData[mLength] == 0);
	
	return *this;
}


//---------------------------------------------------------------
//
// XString::append (XString, uint32, uint32)
//
//---------------------------------------------------------------
XString& XString::append(const XString& str, uint32 startIndex, uint32 length)
{
	PRECONDITION(startIndex <= str.length());
		
	uint32 count = Min(str.length() - startIndex, length);

	if (count > 0) {
		uint32 oldLength = mLength;
		this->DoResize(mLength + count, true);
		
		BlockMoveData(str.buffer() + startIndex, mData + oldLength, count*sizeof(XChar));

		this->DoSetDirty();
  	}
		
	POSTCONDITION(mData[mLength] == 0);
	
	return *this;
}


//---------------------------------------------------------------
//
// XString::insert (uint32, XChar)
//
//---------------------------------------------------------------
XString& XString::insert(uint32 index, XChar ch)
{
	PRECONDITION(index <= mLength);
		
	this->DoResize(mLength + 1, true);
	
	BlockMoveData(mData + index, mData + index + 1, (mLength - index - 1)*sizeof(XChar));
	
	mData[index] = ch;
	this->DoSetDirty();
	
	POSTCONDITION(mData[mLength] == 0);
	
	return *this;
}


//---------------------------------------------------------------
//
// XString::insert (uint32, uint32, XChar)
//
//---------------------------------------------------------------
XString& XString::insert(uint32 index, uint32 count, XChar ch)
{
	PRECONDITION(index <= mLength);
		
	if (count > 0) {
		this->DoResize(mLength + count, true);
		
		BlockMoveData(mData + index, mData + index + count, (mLength - index - count)*sizeof(XChar));
		
		for (uint32 i = 0; i < count; ++i)
			mData[index + i] = ch;
		this->DoSetDirty();
	}
		
	POSTCONDITION(mData[mLength] == 0);
	
	return *this;
}


//---------------------------------------------------------------
//
// XString::insert (uint32, const XChar*)
//
//---------------------------------------------------------------
XString& XString::insert(uint32 index, const XChar* str)
{
	PRECONDITION(str != nil);
	PRECONDITION(index <= mLength);

	uint32 count = STD::wcslen(str);
	if (count > 0) {
		this->DoResize(mLength + count, true);

		BlockMoveData(mData + index, mData + index + count, (mLength - index - count)*sizeof(XChar));

		BlockMoveData(str, mData + index, count*sizeof(XChar));
		this->DoSetDirty();
	}

	POSTCONDITION(mData[mLength] == 0);

	return *this;
}


//---------------------------------------------------------------
//
// XString::insert (uint32, const XChar*, uint32)
//
//---------------------------------------------------------------
XString& XString::insert(uint32 index, const XChar* str, uint32 length)
{
	PRECONDITION(str != nil);
	PRECONDITION(index <= mLength);
	PRECONDITION(length < XString::npos);
		
	uint32 count = length;
	if (count > 0) {
		this->DoResize(mLength + count, true);
		
		BlockMoveData(mData + index, mData + index + count, (mLength - index - count)*sizeof(XChar));
		
		BlockMoveData(str, mData + index, count*sizeof(XChar));
		this->DoSetDirty();
	}
		
	POSTCONDITION(mData[mLength] == 0);
	
	return *this;
}


//---------------------------------------------------------------
//
// XString::insert (uint32, XString, uint32, uint32)
//
//---------------------------------------------------------------
XString& XString::insert(uint32 index, const XString& str, uint32 startIndex, uint32 length)
{
	PRECONDITION(index <= mLength);
	PRECONDITION(startIndex <= str.length());
		
	uint32 count = Min(str.length() - startIndex, length);

	if (count > 0) {
		this->DoResize(mLength + count, true);
		
		BlockMoveData(mData + index, mData + index + count, (mLength - index - count)*sizeof(XChar));
		
		BlockMoveData(str.buffer() + startIndex, mData + index, count*sizeof(XChar));
		this->DoSetDirty();
	}
		
	POSTCONDITION(mData[mLength] == 0);
	
	return *this;
}


//---------------------------------------------------------------
//
// XString::erase
//
//---------------------------------------------------------------
XString& XString::erase(uint32 index, uint32 count)
{
	PRECONDITION(index < mLength);
			
	if (index == 0 && count >= mLength) {
		this->DoResize(0, false);
		this->DoSetDirty();
	
	} else if (count > 0) {
		count = Min(mLength, count);		// need this to avoid overflow

		if (index + count > mLength)
			count = mLength - index;
										
		BlockMoveData(mData + index + count, mData + index, (mLength - (index + count))*sizeof(XChar));
		
		this->DoResize(mLength - count, true);
		this->DoSetDirty();
	}
	
	POSTCONDITION(mData[mLength] == 0);
	
	return *this;
}
 

//---------------------------------------------------------------
//
// XString::replace (uint32, uint32, uint32, XChar)
//
//---------------------------------------------------------------
XString& XString::replace(uint32 index, uint32 count, uint32 numInserted, XChar ch)
{
	PRECONDITION(index <= mLength);
		
	uint32 numRemoved  = Min(mLength, count);

	if (numInserted > 0 || numRemoved > 0) {
		uint32 delta;
	
		if (numInserted > numRemoved) {
			delta = numInserted - numRemoved;
			this->DoResize(mLength + delta, true);
			
			BlockMoveData(mData + index, mData + index + delta, (mLength - index - delta)*sizeof(XChar));
			
		} else if (numInserted < numRemoved) {
			if (index + numRemoved > mLength)
				numRemoved = mLength - index;
				
			delta = numRemoved - numInserted;
											
			BlockMoveData(mData + index + delta, mData + index, (mLength - (index + delta))*sizeof(XChar));
			
			this->DoResize(mLength - delta, true);
		}
		
		while (numInserted--)
			mData[index++] = ch;
				
		this->DoSetDirty();
	}
			
	POSTCONDITION(mData[mLength] == 0);
	
	return *this;
}


//---------------------------------------------------------------
//
// XString::replace (uint32, uint32, const XChar*)
//
//---------------------------------------------------------------
XString& XString::replace(uint32 index, uint32 count, const XChar* str)
{
	PRECONDITION(index <= mLength);
	PRECONDITION(str != nil);

	uint32 numInserted = STD::wcslen(str);
	uint32 numRemoved  = Min(mLength, count);

	if (numInserted > 0 || numRemoved > 0) {
		uint32 delta;

		if (numInserted > numRemoved) {
			delta = numInserted - numRemoved;
			this->DoResize(mLength + delta, true);

			BlockMoveData(mData + index, mData + index + delta, (mLength - index - delta)*sizeof(XChar));

		} else if (numInserted < numRemoved) {
			if (index + numRemoved > mLength)
				numRemoved = mLength - index;

			delta = numRemoved - numInserted;
											
			BlockMoveData(mData + index + delta, mData + index, (mLength - (index + delta))*sizeof(XChar));
			
			this->DoResize(mLength - delta, true);
		}
		
		BlockMoveData(str, mData + index, numInserted*sizeof(XChar));
		
		this->DoSetDirty();
	}
			
	POSTCONDITION(mData[mLength] == 0);

	return *this;
}


//---------------------------------------------------------------
//
// XString::replace (uint32, uint32, const XChar*, uint32)
//
//---------------------------------------------------------------
XString& XString::replace(uint32 index, uint32 count, const XChar* str, uint32 length)
{
	PRECONDITION(index <= mLength);
	PRECONDITION(str != nil);
	PRECONDITION(length < XString::npos);

	uint32 numInserted = length;
	uint32 numRemoved  = Min(mLength, count);

	if (numInserted > 0 || numRemoved > 0) {
		uint32 delta;
	
		if (numInserted > numRemoved) {
			delta = numInserted - numRemoved;
			this->DoResize(mLength + delta, true);
			
			BlockMoveData(mData + index, mData + index + delta, (mLength - index - delta)*sizeof(XChar));
			
		} else if (numInserted < numRemoved) {
			if (index + numRemoved > mLength)
				numRemoved = mLength - index;
				
			delta = numRemoved - numInserted;
											
			BlockMoveData(mData + index + delta, mData + index, (mLength - (index + delta))*sizeof(XChar));
			
			this->DoResize(mLength - delta, true);
		}
		
		BlockMoveData(str, mData + index, numInserted*sizeof(XChar));
		
		this->DoSetDirty();
	}
			
	POSTCONDITION(mData[mLength] == 0);
	
	return *this;
}


//---------------------------------------------------------------
//
// XString::replace (uint32, uint32, XString, uint32, uint32)
//
//---------------------------------------------------------------
XString& XString::replace(uint32 index, uint32 count, const XString& str, uint32 startIndex, uint32 length)
{
	PRECONDITION(index <= mLength);
	PRECONDITION(startIndex <= str.length());
		
	uint32 numInserted = Min(str.length() - startIndex, length);
	uint32 numRemoved  = Min(mLength, count);

	if (numInserted > 0 || numRemoved > 0) {
		uint32 delta;
	
		if (numInserted > numRemoved) {
			delta = numInserted - numRemoved;
			this->DoResize(mLength + delta, true);
			
			BlockMoveData(mData + index, mData + index + delta, (mLength - index - delta)*sizeof(XChar));
			
		} else if (numInserted < numRemoved) {
			if (index + numRemoved > mLength)
				numRemoved = mLength - index;
				
			delta = numRemoved - numInserted;
											
			BlockMoveData(mData + index + delta, mData + index, (mLength - (index + delta))*sizeof(XChar));
			
			this->DoResize(mLength - delta, true);
		}
		
		BlockMoveData(str.buffer() + startIndex, mData + index, numInserted*sizeof(XChar));
		
		this->DoSetDirty();
	}
			
	POSTCONDITION(mData[mLength] == 0);
	
	return *this;
}


//---------------------------------------------------------------
//
// XString::find (XChar, uint32)
//
//---------------------------------------------------------------
uint32 XString::find(XChar ch, uint32 index) const
{
	PRECONDITION(index <= mLength);
	
	uint32 pos = npos;
	
	for (uint32 j = index; j < mLength && pos == npos; ++j) {
		if (mData[j] == ch)
			pos = j;
	}
	
	return pos;
}


//---------------------------------------------------------------
//
// XString::find (const XChar*, uint32)
//
//---------------------------------------------------------------
uint32 XString::find(const XChar* str, uint32 index) const
{
	PRECONDITION(str != nil);
	PRECONDITION(index <= mLength);

	uint32 pos = npos;
	uint32 count = STD::wcslen(str);

	const_iterator iter = std::search(this->begin() + index, this->end(), str, str + count);
	if (iter < this->end())
		pos = numeric_cast<uint32>(iter - mData);

	return pos;
}


//---------------------------------------------------------------
//
// XString::find (const XChar*, uint32, uint32)
//
//---------------------------------------------------------------
uint32 XString::find(const XChar* str, uint32 index, uint32 length) const
{
	PRECONDITION(str != nil);
	PRECONDITION(index <= mLength);
	PRECONDITION(length < XString::npos);
	
	uint32 pos = npos;

	uint32 count = length;

	const_iterator iter = std::search(this->begin() + index, this->end(), str, str + count);
	if (iter < this->end())
		pos = numeric_cast<uint32>(iter - mData);
	
	return pos;
}


//---------------------------------------------------------------
//
// XString::find (XString, uint32)
//
//---------------------------------------------------------------
uint32 XString::find(const XString& str, uint32 index) const
{
	PRECONDITION(index <= mLength);
	
	uint32 pos = npos;

	const_iterator iter = std::search(this->begin() + index, this->end(), str.begin(), str.end());
	if (iter < mData + mLength)
		pos = numeric_cast<uint32>(iter - mData);
	
	return pos;
}


//---------------------------------------------------------------
//
// XString::rfind (XChar, uint32)
//
//---------------------------------------------------------------
uint32 XString::rfind(XChar ch, uint32 index) const
{	
	uint32 pos = npos;
	
	index = Min(mLength - 1, index);
	
	for (uint32 j = index; pos == npos && j < ULONG_MAX; --j) {
		if (mData[j] == ch)
			pos = j;
	}
	
	return pos;
}


//---------------------------------------------------------------
//
// XString::rfind (const XChar*, uint32)
//
//---------------------------------------------------------------
uint32 XString::rfind(const XChar* str, uint32 index) const
{
	PRECONDITION(str != nil);	

	uint32 pos = npos;
	uint32 count = STD::wcslen(str);

	index = Min(mLength - 1, index);

#if MSVC >= 1100
    typedef std::reverse_iterator<const XChar*, const XChar> reverse_iter;
#else
    typedef std::reverse_iterator<const XChar*> reverse_iter;
#endif

	const_reverse_iterator iter = std::search(this->rbegin() + (int32) (mLength - index - 1), this->rend(), reverse_iter(str + count), reverse_iter(str));
	if (iter < this->rend())
		pos = mLength - (iter - this->rbegin()) - 1 - (count - 1);
	
	return pos;
}


//---------------------------------------------------------------
//
// XString::rfind (const XChar*, uint32, uint32)
//
//---------------------------------------------------------------
uint32 XString::rfind(const XChar* str, uint32 index, uint32 length) const
{
	PRECONDITION(str != nil);
	PRECONDITION(length < XString::npos);
	
	uint32 pos = npos;

	uint32 count = length;
	index = Min(mLength - 1, index);

#if MSVC >= 1100
    typedef std::reverse_iterator<const XChar*, const XChar> reverse_iter;
#else
    typedef std::reverse_iterator<const XChar*> reverse_iter;
#endif

	const_reverse_iterator iter = std::search(this->rbegin() + (int32) (mLength - index - 1), this->rend(), reverse_iter(str + count), reverse_iter(str));
	if (iter < this->rend())
		pos = mLength - (iter - this->rbegin()) - 1 - (count - 1);
	
	return pos;
}


//---------------------------------------------------------------
//
// XString::rfind (XString, uint32)
//
//---------------------------------------------------------------
uint32 XString::rfind(const XString& str, uint32 index) const
{	
	uint32 pos = npos;

	index = Min(mLength - 1, index);

	const_reverse_iterator iter = std::search(this->rbegin() + (int32) (mLength - index - 1), this->rend(), str.rbegin(), str.rend());
	if (iter < this->rend())
		pos = mLength - (iter - this->rbegin()) - 1 - (str.length() - 1);
	
	return pos;
}


//---------------------------------------------------------------
//
// XString::find_first_of
//
//---------------------------------------------------------------
uint32 XString::find_first_of(const XString& str, uint32 index) const
{
	PRECONDITION(index <= mLength);
	
	uint32 pos = npos;
	
	if (str.length() > 0) {
		for (uint32 i = index; i < mLength && pos == npos; ++i) {
			if (str.find(mData[i]) != npos)
				pos = i;
		}
	}
	
	return pos;
}


//---------------------------------------------------------------
//
// XString::find_last_of
//
//---------------------------------------------------------------
uint32 XString::find_last_of(const XString& str, uint32 index) const
{
	uint32 pos = npos;
	
	if (str.length() > 0) {
		index = Min(mLength - 1, index);
	
		for (uint32 i = index; pos == npos && i < ULONG_MAX; --i) {
			if (str.find(mData[i]) != npos)
				pos = i;
		}
	}
	
	return pos;
}


//---------------------------------------------------------------
//
// XString::find_first_not_of
//
//---------------------------------------------------------------
uint32 XString::find_first_not_of(const XString& str, uint32 index) const
{
	PRECONDITION(index <= mLength);
	
	uint32 pos = npos;
	
	if (str.length() > 0) {
		for (uint32 i = index; i < mLength && pos == npos; ++i) {
			if (str.find(mData[i]) == npos)
				pos = i;
		}
		
	} else if (mLength > 0)
		pos = 0;
	
	return pos;
}


//---------------------------------------------------------------
//
// XString::find_last_not_of
//
//---------------------------------------------------------------
uint32 XString::find_last_not_of(const XString& str, uint32 index) const
{
	uint32 pos = npos;
	
	if (str.length() > 0) {
		index = Min(mLength - 1, index);
	
		for (uint32 i = index; pos == npos && i < ULONG_MAX; --i) {
			if (str.find(mData[i]) == npos)
				pos = i;
		}

	} else if (mLength > 0)
		pos = mLength - 1;
	
	return pos;
}


//---------------------------------------------------------------
//
// XString::compare (const XChar*)
//
//---------------------------------------------------------------
int XString::compare(const XChar* rhs) const
{
	PRECONDITION(rhs != nil);
	
	int result = 0;
	
	uint32 index;
	for (index = 0; index < mLength && result == 0; ++index) {
		if (rhs[index] == '\0')
			result = +1;			
		else if (mData[index] < rhs[index])
			result = -1;
		else if (mData[index] > rhs[index])
			result = +1;
	}
		
	if (result == 0 && rhs[index] != '\0')
		result = -1;
	
	return result;
}


//---------------------------------------------------------------
//
// XString::compare (XString)
//
//---------------------------------------------------------------
int XString::compare(const XString& rhs) const
{
	int result = 0;
	
	uint32 index;
	for (index = 0; index < mLength && result == 0; ++index) {
		if (index == rhs.length())
			result = +1;			
		else if (mData[index] < rhs[index])
			result = -1;
		else if (mData[index] > rhs[index])
			result = +1;
	}
	
	if (result == 0 && index < rhs.length())
		result = -1;
	
	return result;
}


//---------------------------------------------------------------
//
// XString::compare (uint32, uint32, const XChar*, uint32)
//
//---------------------------------------------------------------
int XString::compare(uint32 index, uint32 count, const XChar* inRhs, uint32 length) const
{
	XString lhs(*this, index, count);
	XString rhs(inRhs, 0, length);
	
	int result = lhs.compare(rhs);
	
	return result;
}


//---------------------------------------------------------------
//
// XString::compare (uint32, uint32, XString, uint32, uint32)
//
//---------------------------------------------------------------
int XString::compare(uint32 index, uint32 count, const XString& inRhs, uint32 startIndex, uint32 length) const
{
	XString lhs(*this, index, count);
	XString rhs(inRhs, startIndex, length);
	
	int result = lhs.compare(rhs);
	
	return result;
}


//---------------------------------------------------------------
//
// XString::substr
//
// CW 5 won't inline this...
//
//---------------------------------------------------------------
XString XString::substr(uint32 index, uint32 count) const	
{
	return XString(*this, index, count);
}


//---------------------------------------------------------------
//
// XString::copy (unsigned char*, uint32, uint32)
//
//---------------------------------------------------------------
#if MAC
uint32 XString::copy(unsigned char* buffer, uint32 count, uint32 index) const
{
	PRECONDITION(buffer != nil);
	PRECONDITION(index <= mLength);
		
	count = Min(mLength - index, count);
	
	XMacRomanTranscoder transcoder;
	count = transcoder.ConvertFromUTF16(mData + index, count*sizeof(XChar), (char*) buffer, count*sizeof(unsigned char));
		
	return count;
}
#endif


//---------------------------------------------------------------
//
// XString::copy (char*, uint32, uint32)
//
//---------------------------------------------------------------
uint32 XString::copy(char* buffer, uint32 count, uint32 index) const
{
	PRECONDITION(buffer != nil);
	PRECONDITION(index <= mLength);
	
	count = Min(mLength - index, count);
	
	if (msDefaultTranscoder == nil)
		sStringInitter.Create();
	
	if (mTranscoder == nil)
		const_cast<XString*>(this)->mTranscoder = msDefaultTranscoder->Clone();

	count = mTranscoder->ConvertFromUTF16(mData + index, count*sizeof(XChar), buffer, count*sizeof(unsigned char));
		
	return count;
}


//---------------------------------------------------------------
//
// XString::copy (XChar*, uint32, uint32)
//
//---------------------------------------------------------------
uint32 XString::copy(XChar* buffer, uint32 count, uint32 index) const
{
	PRECONDITION(buffer != nil);
	PRECONDITION(index <= mLength);
	
	count = Min(mLength - index, count);
	
	BlockMoveData(mData + index, buffer, count*sizeof(XChar));
		
	return count;
}


//---------------------------------------------------------------
//
// XString::swap
//
//---------------------------------------------------------------
void swap(XString& lhs, XString& rhs)
{
	std::swap(lhs.mData, rhs.mData);					
	std::swap(lhs.mLength, rhs.mLength);
	std::swap(lhs.mCapacity, rhs.mCapacity);
	
	std::swap(lhs.mDirty, rhs.mDirty);					
	std::swap(lhs.mCData, rhs.mCData);
	std::swap(lhs.mCDataCapacity, rhs.mCDataCapacity);
#if MAC
	std::swap(lhs.mPData, rhs.mPData);
#endif
#if TARGET_CARBON
	std::swap(lhs.mCFString, rhs.mCFString);
#endif

	std::swap(lhs.mTranscoder, rhs.mTranscoder);					

	XChar tempBuffer[XString::kBufferSize+1];
	ASSERT(sizeof(lhs.mBuffer) == sizeof(tempBuffer));

	BlockMoveData(lhs.mBuffer, tempBuffer, (uint32) sizeof(lhs.mBuffer));
	BlockMoveData(rhs.mBuffer, lhs.mBuffer, (uint32) sizeof(lhs.mBuffer));
	BlockMoveData(tempBuffer, rhs.mBuffer, (uint32) sizeof(lhs.mBuffer));
	
	if (lhs.mData == rhs.mBuffer)
		lhs.mData = lhs.mBuffer;
	
	if (rhs.mData == lhs.mBuffer)
		rhs.mData = rhs.mBuffer;
}


//---------------------------------------------------------------
//
// XString::SetTranscoder
//
//---------------------------------------------------------------
void XString::SetTranscoder(XTextTranscoder* takeTranscoder)
{
	PRECONDITION(takeTranscoder != nil);
	
	delete mTranscoder;
	mTranscoder = takeTranscoder;
}


//---------------------------------------------------------------
//
// XString::SetDefaultTranscoder						[static]
//
//---------------------------------------------------------------
void XString::SetDefaultTranscoder(XTextTranscoder* takeTranscoder)
{
	PRECONDITION(takeTranscoder != nil);

	XEnterCriticalSection enter(sTranscoderSection);

	ASSERT(msDefaultTranscoder != nil);
	
	delete msDefaultTranscoder;
	msDefaultTranscoder = takeTranscoder;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XString::DoResize
//
//---------------------------------------------------------------
void XString::DoResize(uint32 count, bool copyData)
{
	PRECONDITION(mCapacity >= mLength);
	
	// If the new count is less than half the current capacity
	// then shrink the string.
	if (count < mCapacity/2 || (count < mLength && count <= kBufferSize) && mData != mBuffer) {	// second test keeps our ASSERTs at the bottom of this function happy
		XChar* newData = nil;
		uint32 newCap = 0;
		
		if (count > kBufferSize) {
			newCap  = count;
			newData = new XChar[newCap + 1];
		
		} else {
			newData = mBuffer;
			newCap  = kBufferSize;
		}	
		
		if (copyData && newData != mData)
			BlockMoveData(mData, newData, Min(count, mLength)*sizeof(XChar));

		if (mCapacity > kBufferSize) 
			delete [] mData;
			
		mData     = newData;
		mLength   = count;
		mCapacity = newCap;
	
	// If the new count is just a little smaller then just set
	// the length (so our reserve space will grow a bit).
	} else if (count < mLength) {
		mLength = count;
	
	// If the new count is greater than our current capacity
	// we need to grow the string.
	} else  if (count > mCapacity) {
		XChar* newData = nil;
		uint32 newCap = 0;
		
		if (count > kBufferSize) {
			newCap  = 2*count;
			newData = new (std::nothrow) XChar[newCap + 1];	// attempt to allocate an array twice as large as neccesary so appending takes (amortized) constant time
			
			if (newData == nil) {
				newCap  = count;
				newData = new XChar[newCap + 1];
			}
		
		} else {
			newData = mBuffer;
			newCap  = kBufferSize;
		}	

		if (copyData && newData != mData)
			BlockMoveData(mData, newData, Min(count, mLength)*sizeof(XChar));

		if (mCapacity > kBufferSize) 
			delete [] mData;
			
		mData     = newData;
		mLength   = count;
		mCapacity = newCap;
	
	// If the new count is greater than our current length but
	// within our capacity we can just set the length.
	} else if (count > mLength) {
		mLength = count;
	}
	
	mData[mLength] = 0;

	POSTCONDITION(mLength == count);
	POSTCONDITION(mCapacity >= mLength);
	POSTCONDITION(mData == mBuffer || mLength > kBufferSize);
}


//---------------------------------------------------------------
//
// XString::DoSetDirty
//
//---------------------------------------------------------------
#if DEBUG
void XString::DoSetDirty()
{
	mDirty = kDirty;
	
	// The CodeWarrior debugger doesn't do a very good job displaying 
	// wide strings so in debug builds we'll ensure that mCData
	// is always up to date. (We don't need to do this with MSVC
	// because we can clue the debugger in by using the AutoExp.Dat 
	// file). 
#if __MWERKS__

	// Resize mCData as needed (it's important that we be intelligent
	// about this because, unlike DoResetDirty, we can wind up doing
	// this a lot as strings are manipulated).
	if (mLength < mCDataCapacity/2 || mCData == nil) {	
		delete [] mCData; 
		mCData = nil;
		mCDataCapacity = 0;
	
		uint32 len = Max(mLength, (uint32) kBufferSize);		// it seems silly to allocate tiny buffers so we'll ensure that they're not too small
		mCData = new char[len + 1];
		mCDataCapacity = len;

	} else if (mLength > mCDataCapacity) {
		delete [] mCData; 
		mCData = nil;
		mCDataCapacity = 0;
	
		mCData = new char[2*mLength + 1];
		mCDataCapacity = 2*mLength;
	}

	// Convert the Unicode string into a char*. It'd be simpler
	// to call DoResetDirty, but that's too inefficient to do
	// here. So, we'll do a fast but crude translation that
	// will normally be sufficient for debugging.
	uint32 index = 0;
	do {
		mCData[index] = (char) mData[index];
	} while (mData[index++] != '\0');
#endif
}
#endif


//---------------------------------------------------------------
//
// XString::DoUpdateCData
//
//---------------------------------------------------------------
void XString::DoUpdateCData() const
{	
	PRECONDITION(mDirty & kCDirty);
	PRECONDITION(mData[mLength] == 0);
	
	if (msDefaultTranscoder == nil)
		sStringInitter.Create();

	if (mTranscoder == nil)
		const_cast<XString*>(this)->mTranscoder = msDefaultTranscoder->Clone();

	uint32 srcBytes = mLength*sizeof(XChar);
	uint32 dstBytes = mTranscoder->GetDstBytes(mData, srcBytes);

	if (dstBytes < mCDataCapacity/2 || dstBytes > mCDataCapacity || mCData == nil) {	
		delete [] mCData; 
		mCData = nil;
		mCDataCapacity = 0;
	
		uint32 len = Max(dstBytes, (uint32) kBufferSize);	// it seems silly to allocate tiny buffers so we'll ensure that they're not too small
		mCData = new char[len + 1];							// note that we don't double the mCData size when growing under the assumption that most strings will stop growing once c_str() is called
		mCDataCapacity = len;
	}
	
	uint32 length = mTranscoder->ConvertFromUTF16(mData, srcBytes, mCData, dstBytes);
	ASSERT(length <= dstBytes);
	ASSERT(length <= mCDataCapacity);
	mCData[length] = '\0';
			
	mDirty = numeric_cast<uint8>(mDirty & (~kCDirty));
	
#if DEBUG
//	if (gIntenseDebugging) 
//		for (index = 0; index <= mLength; ++index)			// these checks aren't valid for formats like utf-8...
//			if (mData[index] <= 0x7F)
//				ASSERT(mCData[index] == mData[index]);
#endif
}


//---------------------------------------------------------------
//
// XString::DoUpdatePData
//
//---------------------------------------------------------------
#if MAC		
void XString::DoUpdatePData() const
{	
	PRECONDITION(mDirty & kPDirty);
	PRECONDITION(mData[mLength] == 0);
	
	if (mLength < 256) {
		if (mPData == nil)
			mPData = new unsigned char[256];

		XMacRomanTranscoder transcoder;
		XDisableUTF16Warning disabler;

		uint32 length = transcoder.ConvertFromUTF16(mData, mLength*sizeof(XChar), (char*) mPData+1, 255);
		mPData[0] = (unsigned char) Min(length, 255UL);
	}
			
	mDirty &= ~kPDirty;
	
#if DEBUG
	uint32 index;
	if (mLength < 256) {
		if (gIntenseDebugging) 
			for (index = 0; index < mLength; ++index)
				if (mData[index] <= 0x7F)
					ASSERT(mPData[index + 1] == mData[index]);
		ASSERT(mPData[0] == mLength);
	}
#endif
}
#endif


//---------------------------------------------------------------
//
// XString::DoUpdateCFData
//
//---------------------------------------------------------------
#if TARGET_CARBON		
void XString::DoUpdateCFData() const
{	
	PRECONDITION(mDirty & kCFDirty);
	PRECONDITION(mData[mLength] == 0);
	
	if (mCFString == nil) {
		mCFString = CFStringCreateMutable(kCFAllocatorSystemDefault, 0);
		ThrowIfNil(mCFString);
			
	} else {
		CFRange range;
		range.location = 0;
		range.length   = CFStringGetLength(mCFString);
		CFStringDelete(mCFString, range);
	}
			
	CFStringAppendCharacters(mCFString, (const UniChar*) mData, numeric_cast<int32>(mLength));

	mDirty &= ~kCFDirty;
}
#endif


}	// namespace Whisper
