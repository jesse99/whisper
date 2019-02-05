/*
 *  File:		XHandle.h
 *  Summary:	A class that encapsulates relocatable memory.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XHandle.h,v $
 *		Revision 1.4  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/10 08:53:01  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:37:54  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 2/01/00	JDJ		Made swap a friend function.
 *		 <2>	 2/25/98	JDJ		Added ResetTail. 
 *		 <1>	 1/07/98	JDJ		Created (from Raven)
 */

#pragma once

#include <ZHandleRef.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XHandle
//!		A class that encapsulates relocatable memory.
/*!		XHandle uses reference counting so that when an XHandle is copied the old and 
 *		new objects point to the same block of memory. This means that XHandle objects 
 *		can be treated just like regular OSHandle's. In particular, they can be passed 
 *		by value without allocating a new chunk of memory.
 *
 *		There are three ways to allocate memory in Whisper: operator new, XHandle, and 
 *		XPointer. The new operator should be used to allocate small amounts of memory 
 *		that won't change size. XHandle should be used for large chunks of memory. 
 *		XPointer should be used for large chunks of memory that cannot move.
 *
 *		When using XHandle try to minimize the amount of time the handle is locked down: 
 *		while it's locked the heap may be fragmented. If a memory allocation happens 
 *		when the heap is fragmented it may fail. */
// ===================================================================================
class CORE_EXPORT XHandle : public XLockableMixin {

	typedef XLockableMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XHandle();

	explicit 			XHandle(uint32 bytes = 0, uint32 options = kDontZeroBytes + kUseAppHeap);
						/**< kUseAppHeap and kUseTempHeap are ignored on Windows. */

#if MAC
						XHandle(Handle takeHandle);
						/**< Accepts responsibility for the Handle provided in takeHandle. 
						If the handle is a Mac resource it is loaded (if neccesary) 
						and then detached. */
#endif
					
						XHandle(const XHandle& rhs);

			XHandle& 	operator=(const XHandle& rhs);
	
//-----------------------------------
//	API
//
public:
	//! @name Access
	//@{
			const uint8* GetPtr() const								{ASSERT(mHandleRef->IsLocked()); return mHandleRef->GetPtr();}

			uint8* 		GetPtr()									{ASSERT(mHandleRef->IsLocked()); return mHandleRef->GetPtr();}
				
			const uint8* GetUnsafePtr() const						{return mHandleRef->GetPtr();}
						/**< The handle may not be locked so be careful! */

			uint8* 		GetUnsafePtr()								{return mHandleRef->GetPtr();}
		
#if MAC
			Handle 		GetHandle()	const							{return mHandleRef->GetHandle();}
#endif
	//@}

	//! @name Sizing
	//@{
			uint32 		GetSize() const								{return mHandleRef->GetSize();}
						/**< Ignores the tail if present. */
			
			void 		SetSize(uint32 bytes, bool zeroBytes = kDontZeroBytes)	{mHandleRef->SetSize(bytes, zeroBytes);}
						/**< Note that this throws an exception if it fails. Also the
						handle should not be locked. If zeroBytes is set new bytes
						are zeroed instead of being uninitialized. */
	//@}

	//! @name Misc
	//@{
			void 		Detach();
						/**< If the letter is being shared this will create a new letter
						referenced only by 'this'. */
						
			bool		operator==(const XHandle& rhs) const;
						/**< Byte by byte comparison. */

			bool		operator!=(const XHandle& rhs) const		{return !this->operator==(rhs);}

	friend	void 		swap(XHandle& lhs, XHandle& rhs)			{std::swap(lhs.mHandleRef, rhs.mHandleRef);}

#if MAC && DEBUG
			void 		ResetTail();
						/**< If you're dealing with a toolbox handle it's possible that a
						toolbox call will resize the handle. If this is the case you 
						can call this function to append a new tail. */
#endif
	//@}

//-----------------------------------
//	XLockableMixin API
//
public:
	virtual void 		Lock(bool moveHigh = kDontMoveHigh);

	virtual void 		Unlock();

	virtual bool 		IsLocked() const;
	
protected:
	virtual void 		OnLock(bool)								{}

	virtual void 		OnUnlock() 									{}

//-----------------------------------
//	Member Data
//
protected:
	ZHandleRef*		mHandleRef;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
