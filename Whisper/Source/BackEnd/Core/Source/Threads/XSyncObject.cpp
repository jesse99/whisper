/*
 *  File:       XSyncObject.cpp
 *  Summary:   	Abstract base class for all thread synchronization objects except critical sections. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XSyncObject.cpp,v $
 *		Revision 1.3  2001/04/27 04:36:55  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:54:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XSyncObject.h>

#include <XDebug.h>
#include <XExceptions.h>
#include <XMiscUtils.h>
#include <XStringUtils.h>

#if MAC
	#include <Threads.h>
#endif

namespace Whisper {


// ===================================================================================
//	class XSyncObject
// ===================================================================================

//---------------------------------------------------------------
//
// XSyncObject::~XSyncObject
//
//---------------------------------------------------------------
XSyncObject::~XSyncObject()
{
#if WIN
	PRECONDITION(mObject != nil);

	VERIFY(::CloseHandle(mObject));
	mObject = nil;
#endif
}


//---------------------------------------------------------------
//
// XSyncObject::XSyncObject
//
//---------------------------------------------------------------
XSyncObject::XSyncObject()
{
	mObject = nil;				// created by subclasses
}


//---------------------------------------------------------------
//
// XSyncObject::Lock
//
//---------------------------------------------------------------
bool XSyncObject::Lock(MilliSecond timeout)
{
	PRECONDITION(timeout >= 0);
	
	bool locked = false;

#if WIN
	PRECONDITION(mObject != nil);		

	if (timeout == LONG_MAX)
		timeout = (MilliSecond) INFINITE;
		
	uint32 result = ::WaitForSingleObject(mObject, (uint32) timeout);
	ThrowIf(result == WAIT_ABANDONED || result == WAIT_FAILED);
	
	locked = result == WAIT_OBJECT_0;
	
#elif MAC
	DEBUGSTR("Needs to be overridden by subclasses");
#endif

	return locked;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XMultiLocker
// ===================================================================================

//---------------------------------------------------------------
//
// XMultiLocker::~XMultiLocker	
//
//---------------------------------------------------------------
XMultiLocker::~XMultiLocker()
{
	for (uint32 i = 0; i < KMaxObjects && mLocked; ++i)
		if (mObjects[i] != nil)
			mObjects[i]->Unlock();
}


//---------------------------------------------------------------
//
// XMultiLocker::XMultiLocker (XSyncObject, XSyncObject, MilliSecond)
//
//---------------------------------------------------------------
XMultiLocker::XMultiLocker(XSyncObject& object1, XSyncObject& object2, MilliSecond timeout)
{
	PRECONDITION(timeout >= 0);
	
	mObjects[0] = &object1;
	mObjects[1] = &object2;
	mObjects[2] = nil;
	
	mHandles[0] = object1.GetOSObject();
	mHandles[1] = object2.GetOSObject();
	mHandles[2] = nil;
	
#if WIN
	if (timeout == LONG_MAX)
		timeout = (MilliSecond) INFINITE;
		
	uint32 result = ::WaitForMultipleObjects(2, mHandles, true, (uint32) timeout);
	ThrowIf(result == WAIT_FAILED);
	ThrowIf(result >= WAIT_ABANDONED_0 && result <= WAIT_ABANDONED_0+1);
	
	mLocked = result >= WAIT_OBJECT_0 && result <= WAIT_OBJECT_0+1;
	
#elif MAC
	MilliSecond start = GetMilliSeconds();
	
	mLocked = false;
	while (!mLocked && (timeout == LONG_MAX || GetMilliSeconds() - start < timeout)) {
		mLocked = object1.Lock(0) && object2.Lock(0);
		
		if (!mLocked)
			(void) ::YieldToAnyThread();
	}
#endif
}


//---------------------------------------------------------------
//
// XMultiLocker::XMultiLocker (XSyncObject, XSyncObject, XSyncObject, MilliSecond)
//
//---------------------------------------------------------------
XMultiLocker::XMultiLocker(XSyncObject& object1, XSyncObject& object2, XSyncObject& object3, MilliSecond timeout)
{
	PRECONDITION(timeout >= 0);
	
	mObjects[0] = &object1;
	mObjects[1] = &object2;
	mObjects[2] = &object3;
	mObjects[3] = nil;
	
	mHandles[0] = object1.GetOSObject();
	mHandles[1] = object2.GetOSObject();
	mHandles[2] = object3.GetOSObject();
	mHandles[3] = nil;
	
#if WIN
	if (timeout == LONG_MAX)
		timeout = (MilliSecond) INFINITE;
		
	uint32 result = ::WaitForMultipleObjects(3, mHandles, true, (uint32) timeout);
	ThrowIf(result == WAIT_FAILED);
	ThrowIf(result >= WAIT_ABANDONED_0 && result <= WAIT_ABANDONED_0+2);
	
	mLocked = result >= WAIT_OBJECT_0 && result <= WAIT_OBJECT_0+2;
	
#elif MAC
	MilliSecond start = GetMilliSeconds();
	
	mLocked = false;
	while (!mLocked && (timeout == LONG_MAX || GetMilliSeconds() - start < timeout)) {
		mLocked = object1.Lock(0) && object2.Lock(0) && object3.Lock(0);
		
		if (!mLocked)
			(void) ::YieldToAnyThread();
	}
#endif
}


//---------------------------------------------------------------
//
// XMultiLocker::XMultiLocker (XSyncObject, XSyncObject, XSyncObject, XSyncObject, MilliSecond)
//
//---------------------------------------------------------------
XMultiLocker::XMultiLocker(XSyncObject& object1, XSyncObject& object2, XSyncObject& object3, XSyncObject& object4, MilliSecond timeout)
{
	PRECONDITION(timeout >= 0);
	
	mObjects[0] = &object1;
	mObjects[1] = &object2;
	mObjects[2] = &object3;
	mObjects[3] = &object4;
	mObjects[4] = nil;
	
	mHandles[0] = object1.GetOSObject();
	mHandles[1] = object2.GetOSObject();
	mHandles[2] = object3.GetOSObject();
	mHandles[3] = object4.GetOSObject();
	mHandles[4] = nil;
	
#if WIN
	if (timeout == LONG_MAX)
		timeout = (MilliSecond) INFINITE;
		
	uint32 result = ::WaitForMultipleObjects(4, mHandles, true, (uint32) timeout);
	ThrowIf(result == WAIT_FAILED);
	ThrowIf(result >= WAIT_ABANDONED_0 && result <= WAIT_ABANDONED_0+3);
	
	mLocked = result >= WAIT_OBJECT_0 && result <= WAIT_OBJECT_0+3;
	
#elif MAC
	MilliSecond start = GetMilliSeconds();
	
	mLocked = false;
	while (!mLocked && (timeout == LONG_MAX || GetMilliSeconds() - start < timeout)) {
		mLocked = object1.Lock(0) && object2.Lock(0) && object3.Lock(0) && object4.Lock(0);
		
		if (!mLocked)
			(void) ::YieldToAnyThread();
	}
#endif
}


//---------------------------------------------------------------
//
// XMultiLocker::XMultiLocker (XSyncObject*, XSyncObject*, MilliSecond)
//
//---------------------------------------------------------------
XMultiLocker::XMultiLocker(XSyncObject* begin, XSyncObject* end, MilliSecond timeout)
{
	PRECONDITION(begin != nil);
	PRECONDITION(end != nil);
	PRECONDITION(begin <= end);
	PRECONDITION(timeout >= 0);
	
	uint32 index = 0;
	XSyncObject* iter = begin;
	while (iter < end) {
		if (index >= KMaxObjects)
			throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: Waiting on too many XSyncObject objects.")));
			
		mObjects[index] = iter;
		++iter;

		mHandles[index] = mObjects[index]->GetOSObject();
		++index;
	}
		
	mObjects[index] = nil;
	mHandles[index] = nil;
	
#if WIN
	COMPILE_CHECK(KMaxObjects <= MAXIMUM_WAIT_OBJECTS);
	
	if (timeout == LONG_MAX)
		timeout = (MilliSecond) INFINITE;
		
	uint32 result = ::WaitForMultipleObjects(index, mHandles, true, (uint32) timeout);
	ThrowIf(result == WAIT_FAILED);
	ThrowIf(result >= WAIT_ABANDONED_0 && result <= WAIT_ABANDONED_0+index-1);
	
	mLocked = result >= WAIT_OBJECT_0 && result <= WAIT_OBJECT_0+index-1;
	
#elif MAC
	MilliSecond start = GetMilliSeconds();
	
	mLocked = false;
	while (!mLocked && (timeout == LONG_MAX || GetMilliSeconds() - start < timeout)) {
		mLocked = true;
		
		XSyncObject* iter = begin;
		while (iter < end && mLocked) {
			mLocked = iter->Lock(0);
			++iter;
		}

		if (!mLocked)
			(void) ::YieldToAnyThread();
	}
#endif
}


}	// namespace Whisper
