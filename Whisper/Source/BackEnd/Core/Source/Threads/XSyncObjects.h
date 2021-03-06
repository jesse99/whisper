/*
 *  File:       XSyncObjects.h
 *  Summary:   	Thread synchronization objects. 
 *  Written by: Jesse Jones
 *
 *	Classes:	XMutex		 - Slower more flexible version of XCriticalSection (eg timeouts can be specified).
 *				XSemaphore	 - A mutex that maintains a count and causes threads to block once the count hits zero.
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XSyncObjects.h,v $
 *		Revision 1.4  2001/03/05 05:43:44  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:56:15  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:55:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XSyncObject.h>

#if MAC
	#include <Threads.h>
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XMutex
//!		Slower more flexible version of XCriticalSection (eg timeouts can be specified).
// ===================================================================================
class CORE_EXPORT XMutex : public XSyncObject {
	
	typedef XSyncObject Inherited;
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XMutex();
	
  						XMutex(bool locked = false);
  						/**< You can specify that the mutex start out locked to avoid
  						race conditions (eg if it's possible for another thread
  						to get a pointer to the mutex before everything has been
  						initialized). */

//-----------------------------------
//	Inherited API
//
public:					
#if MAC
	virtual bool 		Lock(MilliSecond timeout = LONG_MAX);
						/**< Note that mutexes are recursive: the same thread can re-lock 
						a mutex without blocking. */
#endif

	virtual void 		Unlock();
						
//-----------------------------------
//	Member Data
//
protected:
#if MAC
	ThreadID	mOwner;
	int32		mLockCount;
#endif
};


// ===================================================================================
//	class XSemaphore
//!		A mutex that maintains a count and causes threads to block once the count hits zero.
// ===================================================================================
class CORE_EXPORT XSemaphore : public XSyncObject {

	typedef XSyncObject Inherited;
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XSemaphore();
	
  						XSemaphore(uint32 count);
  						/**< The semaphore can be locked count times before blocking.
  						A semaphore with a count of one acts like a mutex except
  						that it can be unlocked from any thread (mutex's can only
  						be unlocked by the thread that locked the mutex). */

  						XSemaphore(uint32 initialCount, uint32 maxCount);
  						/**< If you set initialCount to zero you can create a blocking
  						semaphore which can allow you to finish initializing without
  						worrying about race conditions. Once the initialization is
  						complete you can call Unlock(int32) to make the resource
  						available. */

//-----------------------------------
//	Inherited API
//
public:					
#if MAC
	virtual bool 		Lock(MilliSecond timeout = LONG_MAX);
#endif

	virtual void 		Unlock();
	
//-----------------------------------
//	New API
//
public:					
			void 		Unlock(uint32 count);
						
//-----------------------------------
//	Member Data
//
protected:
#if MAC
	uint32	mCount;
	uint32	mMaxCount;
#endif
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
