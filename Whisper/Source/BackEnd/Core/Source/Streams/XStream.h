/*
 *  File:		XStream.h
 *  Summary:	Base classes for binary input and output streams.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XStream.h,v $
 *		Revision 1.4  2001/03/05 05:43:35  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:56:06  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:52:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 3/18/00	JDJ		Added a name cache.
 *		 <3>	 7/14/99	JDJ		Added XInStream::GetCachedObject and XOutStream::GetCacheIndex.
 *		 <2>	 3/20/99	JDJ		Renamed XInStream::Resurrect Unflatten.
 *		 <1>	 1/11/98	JDJ		Created (from Raven)
 */

#pragma once

#include <map>
#include <vector>

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XBinaryPersistentMixin;
class XCreateByNameMixin;


// ========================================================================================
//	class XInStream
//!		Base class for binary input streams.
// ========================================================================================
class CORE_EXPORT XInStream {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XInStream();
	
						XInStream(bool raw);
						/**< If raw is true the stream is sans header. */
						
private:
						XInStream(const XInStream& rhs);
						
			XInStream&	operator=(const XInStream& rhs);
	
//-----------------------------------
//	API
//
public:
	// ----- Reading -----
	virtual void 		ReadBytes(void* dst, uint32 bytes);
		
			XBinaryPersistentMixin* GetCachedObject(uint32 index) const;
						/**< Returns a pointer to a previously streamed in object. */
	
			bool 		IsTagged() const;
						/**< When a stream with a header is saved and DEBUG is true
						each value is tagged with a type code. */
			
			uint32 		ReadTag();
	
			bool 		NeedsByteSwap() const					{return mNeedsByteSwap;}
						/**< Returns true if the data in the stream needs to be byte
						swapped. Note that the predefined streaming operators
						handle byte swapping. */

	// ----- Misc -----
			bool 		AtEnd() const							{return this->GetPosition() >= this->GetLength();}
						/**< Returns true if we're at the end of the stream. */

	virtual uint32		GetLength() const = 0;

	virtual uint32		GetPosition() const = 0;
	
	virtual void		SetPosition(uint32 newPosition) = 0;

//-----------------------------------
//	Internal API
//
public:
	virtual XBinaryPersistentMixin* Unflatten(XCreateByNameMixin* parent = nil);
						/**< Use XBinaryPersistentMixin::Unflatten instead of this. */

protected:
	virtual void 		OnReadBytes(void* dst, uint32 bytes) = 0;
	
	virtual void 		OnReadHeader();

//-----------------------------------
//	Internal Types
//
protected:
	typedef std::vector<XBinaryPersistentMixin*> ObjectCache;
	typedef std::vector<std::wstring> 			 	 NameCache;

//-----------------------------------
//	Member Data
//
protected:
	bool		mHasHeader;
	bool		mReadHeader;
	bool		mTagged;
	bool		mNeedsByteSwap;

	ObjectCache	mObjectCache;		// handles circular object references
	NameCache	mNameCache;			// handles duplicate class names when unflattening
};


// ========================================================================================
//	class XOutStream
//!		Base class for binary output streams.
// ========================================================================================
class CORE_EXPORT XOutStream {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XOutStream();
	
						XOutStream(bool raw);
						/**< If raw is false a header will be written out with the endian of
						the data and (if DEBUG) each piece of data will be tagged with
						its type. */
						
private:
						XOutStream(const XOutStream& rhs);
						
			XOutStream&	operator=(const XOutStream& rhs);
	
//-----------------------------------
//	API
//
public:
	virtual void 		WriteBytes(const void* src, uint32 bytes);
		
			uint32 		GetCacheIndex(const XBinaryPersistentMixin* object);
						/**< Returns the index of a previously streamed object or ULONG_MAX
						if the object hasn't been streamed yet. */
	
#if DEBUG
			bool 		IsTagged() const							{return mHasHeader;}

			void 		WriteTag(uint32 tag);
#endif

protected:
	virtual void 		OnWriteBytes(const void* src, uint32 bytes) = 0;

	virtual void 		OnWriteHeader();

//-----------------------------------
//	Internal API
//
public:
	virtual void		Flatten(const XBinaryPersistentMixin* object);
						/**< Use XBinaryPersistentMixin::Flatten instead of this. */

//-----------------------------------
//	Internal Types
//
protected:
	typedef std::map<const XBinaryPersistentMixin*, uint32> ObjectCache;
	typedef std::map<std::wstring, uint32> 						NameCache;

//-----------------------------------
//	Member Data
//
protected:
	bool		mHasHeader;
	bool		mWroteHeader;

	ObjectCache	mObjectCache;		// handles circular object references
	NameCache	mNameCache;			// handles duplicate class names when flattening
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
