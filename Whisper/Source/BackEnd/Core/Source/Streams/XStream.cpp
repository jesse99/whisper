/*
 *  File:		XStream.cpp
 *  Summary:	Base classes for binary input and output streams.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XStream.cpp,v $
 *		Revision 1.4  2001/04/27 04:36:32  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/03/06 07:33:22  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.2  2000/11/09 12:52:34  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 3/18/00	JDJ		Added a name cache.
 *		 <4>	 7/14/99	JDJ		Added XInStream::GetCachedObject and XOutStream::GetCacheIndex.
 *		 <3>	 6/21/99	JDJ		Added __MSL_FIX_ITERATORS__ (for Pro 3).
 *		 <2>	 3/20/99	JDJ		Renamed XInStream::Resurrect Unflatten.
 *		 <1>	 1/11/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XStream.h>

#include <XBinaryPersistent.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XMiscUtils.h>
#include <XObjectFactory.h>
#include <XStreaming.h>
#include <XStringUtils.h>

namespace Whisper {


// ========================================================================================
//	class XInStream
// ========================================================================================

//---------------------------------------------------------------
//
// XInStream::~XInStream
//
//---------------------------------------------------------------
XInStream::~XInStream()
{
}


//---------------------------------------------------------------
//
// XInStream::XInStream
//
//---------------------------------------------------------------
XInStream::XInStream(bool raw)
{
	mHasHeader     = !raw;
	mReadHeader    = false;
	mTagged        = false;
	mNeedsByteSwap = false;

	// Preload the nil object into our cache (this allows us to
	// safely unflatten nil objects).
	mObjectCache.push_back(nil);
	ASSERT(mObjectCache.size() == 1);
	ASSERT(mObjectCache[0] == nil);
}


//---------------------------------------------------------------
//
// XInStream::Unflatten
//
// This is a slightly tweaked version of code contributed by Eric
// Berdahl.
//
//---------------------------------------------------------------
XBinaryPersistentMixin* XInStream::Unflatten(XCreateByNameMixin* parent)
{
	XBinaryPersistentMixin* object = nil;
	uint32 index;

	std::wstring name;
	*this >> name;

	// An empty class name is the marker for a reference to an
	// object we've already streamed in.
	if (name.length() == 0) {	
		*this >> index;

		ASSERT(index < mObjectCache.size());
		object = mObjectCache[index];
		
	// Otherwise we're streaming in a brand new object.
	} else {
	
		// If the name is the escape character then we can look it
		// up in our name cache. If not we'll add it to the name
		// cache so that we can look it up next time.
		if (name.length() == 1 && name[0] == kEscapeChar) {
			*this >> index;

			ASSERT(index < mNameCache.size());
			name = mNameCache[index];
			
		} else
			mNameCache.push_back(name);
	
		// Create the object and verify that it is a persistent object.
		XCreateByNameMixin* theObject = XObjectFactory::Instance()->CreateObject(name, parent);

		object = dynamic_cast<XBinaryPersistentMixin*>(theObject);
		if (object == nil)
			throw std::runtime_error(ToUTF8Str((L"Internal error: " + name + L" isn't an XBinaryPersistentMixin")));

		// Save the object in our cache so that we can handle circular
		// object references and stream it in.
		try {
			mObjectCache.push_back(object);

			object->HandleStreamIn(*this);		

		} catch (...) {
			if (!mObjectCache.empty() && mObjectCache.back() == object)
				mObjectCache.pop_back();
				
			delete object;
			throw;
		}
	}

	return object;
}


//---------------------------------------------------------------
//
// XInStream::GetCachedObject
//
//---------------------------------------------------------------
XBinaryPersistentMixin* XInStream::GetCachedObject(uint32 index) const
{
	PRECONDITION(index < mObjectCache.size());

	XBinaryPersistentMixin* object = mObjectCache[index];
	
	return object;
}


//---------------------------------------------------------------
//
// XInStream::ReadBytes
//
//---------------------------------------------------------------
void XInStream::ReadBytes(void* dst, uint32 bytes)
{
	PRECONDITION(dst != nil);
	
	if (mHasHeader && !mReadHeader)
		this->OnReadHeader();
		
	if (bytes > 0)
		this->OnReadBytes(dst, bytes);
}


//---------------------------------------------------------------
//
// XInStream::IsTagged
//
//---------------------------------------------------------------
bool XInStream::IsTagged() const						
{
	if (mHasHeader && !mReadHeader) {
		XInStream* thisPtr = const_cast<XInStream*>(this);
		thisPtr->OnReadHeader();
	}
		
	return mTagged;
}


//---------------------------------------------------------------
//
// XInStream::ReadTag
//
//---------------------------------------------------------------
uint32 XInStream::ReadTag()
{
	PRECONDITION(mReadHeader);			// need a header to determine if we have tags
	PRECONDITION(mTagged);
	
	uint32 tag;
	this->OnReadBytes(&tag, sizeof(uint32));
	if (mNeedsByteSwap)
		ByteSwap(tag);
	
	return tag;
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XInStream::OnReadHeader
//
//---------------------------------------------------------------
void XInStream::OnReadHeader()
{
	PRECONDITION(mHasHeader);
	PRECONDITION(!mReadHeader);
	
	uint32 signature;
	this->OnReadBytes(&signature, sizeof(uint32));
	ASSERT(signature == 'RAVE' || signature == 'EVAR');
	
	mNeedsByteSwap = signature == 'EVAR';
	
	int32 version;
	this->OnReadBytes(&version, sizeof(int32));
	if (mNeedsByteSwap)
		ByteSwap(version);
	if (version > 1)
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: Stream header version is too large.")));

	int16 tagged;											// sizeof(bool) may vary so we'll use a short
	this->OnReadBytes(&tagged, sizeof(int16));	
	if (mNeedsByteSwap)
		ByteSwap(tagged);
	mTagged = (bool) tagged;

	mReadHeader = true;
}

#if __MWERKS__
#pragma mark -
#endif

// ========================================================================================
//	class XOutStream
// ========================================================================================

//---------------------------------------------------------------
//
// XOutStream::~XOutStream
//
//---------------------------------------------------------------
XOutStream::~XOutStream()
{
}


//---------------------------------------------------------------
//
// XOutStream::XOutStream
//
//---------------------------------------------------------------
XOutStream::XOutStream(bool raw)
{
	mHasHeader   = !raw;
	mWroteHeader = false;
	
	// Preload the nil object into our object cache (this allows
	// us to safely flatten nil objects).
	mObjectCache[nil] = 0;
	ASSERT(mObjectCache[nil] == 0);
	ASSERT(mObjectCache.size() == 1);
}


//---------------------------------------------------------------
//
// XOutStream::Flatten
//
// This is a slightly tweaked version of code contributed by Eric
// Berdahl.
//
//---------------------------------------------------------------
void XOutStream::Flatten(const XBinaryPersistentMixin* object)
{
	ObjectCache::const_iterator iter = mObjectCache.find(object);
	if (iter != mObjectCache.end()) {
	
		// We use an empty string as the signal that this is a 
		// cookie for a previously streamed object.
		std::wstring emptyName;
		*this << emptyName;

		const uint32 index = (*iter).second;
		*this << index;

	} else {
			
		// We preloaded the cache with nil, so nil pointers
		// should never get here.
		ASSERT(object != nil);

		// Put the new object into the cache (this allows circular
		// object graphs to be streamed out and in).
		const uint32 index = mObjectCache.size();
		mObjectCache[object] = index;
		ASSERT(mObjectCache.size() == index + 1);

		// Stream out the full representation of the object.
		std::wstring name = object->GetClassName();
		NameCache::const_iterator iter = mNameCache.find(name);
		if (iter == mNameCache.end()) {
			*this << name;	
			mNameCache.insert(NameCache::value_type(name, mNameCache.size()));
		
		} else 
			*this << std::wstring(1, kEscapeChar) << iter->second;
		
		object->HandleStreamOut(*this);
	}
}


//---------------------------------------------------------------
//
// XOutStream::GetCacheIndex
//
//---------------------------------------------------------------
uint32 XOutStream::GetCacheIndex(const XBinaryPersistentMixin* object)
{
	PRECONDITION(object != nil);
	
	uint32 index = ULONG_MAX;
	
	ObjectCache::const_iterator iter = mObjectCache.find(object);
	if (iter != mObjectCache.end()) 
		index = (*iter).second;

	return index;
}


//---------------------------------------------------------------
//
// XOutStream::WriteBytes
//
//---------------------------------------------------------------
void XOutStream::WriteBytes(const void* src, uint32 bytes)
{
	PRECONDITION(src != nil);
	
	if (mHasHeader && !mWroteHeader)
		this->OnWriteHeader();
		
	if (bytes > 0)
		this->OnWriteBytes(src, bytes);
}


//---------------------------------------------------------------
//
// XOutStream::WriteTag
//
//---------------------------------------------------------------
#if DEBUG
void XOutStream::WriteTag(uint32 tag)
{
	PRECONDITION(mHasHeader);						// need a header to let XInStream know that the data is tagged
	
	if (mHasHeader && !mWroteHeader)
		this->OnWriteHeader();
		
	this->OnWriteBytes(&tag, sizeof(uint32));
}
#endif

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XOutStream::OnWriteHeader
//
//---------------------------------------------------------------
void XOutStream::OnWriteHeader()
{
	PRECONDITION(mHasHeader);
	PRECONDITION(!mWroteHeader);
	
	uint32 signature = 'RAVE';
	this->OnWriteBytes(&signature, sizeof(uint32));
	
	int32 version = 1;
	this->OnWriteBytes(&version, sizeof(int32));

#if DEBUG
	int16 tagged = 1;
#else
	int16 tagged = 0;
#endif
	this->OnWriteBytes(&tagged, sizeof(int16));	
	
	mWroteHeader = true;
}


}	// namespace Whisper
