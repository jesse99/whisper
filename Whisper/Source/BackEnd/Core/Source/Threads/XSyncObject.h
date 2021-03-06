/*
 *  File:       XSyncObject.h
 *  Summary:   	Abstract base class for all thread synchronization objects except critical sections. 
 *  Written by: Jesse Jones
 *
 *	Classes:	XSyncObject		- The thread synchronization base class.
 *				XSingleLocker	- Stack based class for locking XSyncObject's.
 *				XMultiLocker	- Stack based class for locking multiple XSyncObject's (avoids dead-locks).
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XSyncObject.h,v $
 *		Revision 1.5  2001/03/05 05:43:39  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.4  2001/03/01 11:42:21  jesjones
 *		Disabled copying for a few classes.
 *		
 *		Revision 1.3  2000/12/10 08:56:10  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:54:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Types
//
#if WIN
	typedef HANDLE	OSSyncObject;
#elif MAC
	typedef uint32	OSSyncObject;		// dummy
#endif


// ===================================================================================
//	class XSyncObject
//!		Abstract base class for all thread synchronization objects except critical sections. 
// ===================================================================================
class CORE_EXPORT XSyncObject {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XSyncObject();

						XSyncObject();
						
private:
						XSyncObject(const XSyncObject& rhs);						
			XSyncObject& operator=(const XSyncObject& rhs);

//-----------------------------------
//	API
//
public:					
	virtual bool 		Lock(MilliSecond timeout = LONG_MAX);
						/**< Returns true if the current thread was able to acquire the lock. */

	virtual	void 		Unlock() = 0;

			OSSyncObject GetOSObject()								{return mObject;}
						
//-----------------------------------
//	Member Data
//
protected:
	OSSyncObject	mObject;
};


// ===================================================================================
//	class XSingleLocker
//!		Stack based class for locking XSyncObject's.
// ===================================================================================
class CORE_EXPORT XSingleLocker {

public:
						~XSingleLocker();

						XSingleLocker(XSyncObject& object, MilliSecond timeout = LONG_MAX);

			bool 		AcquiredLock() const						{return mLocked;}
						/**< Returns true if the current thread was able to acquire the lock.
						You should call this if you specified a timeout. */
						
private:
						XSingleLocker(const XSingleLocker& rhs);						
			XSingleLocker& operator=(const XSingleLocker& rhs);
						
private:
	XSyncObject&	mObject;
	bool			mLocked;
};


// ===================================================================================
//	class XMultiLocker
//!		Stack based class for locking multiple XSyncObject's (avoids dead-locks).
// ===================================================================================
class CORE_EXPORT XMultiLocker {

public:
						~XMultiLocker();

						XMultiLocker(XSyncObject& object1, XSyncObject& object2, MilliSecond timeout = LONG_MAX);
						XMultiLocker(XSyncObject& object1, XSyncObject& object2, XSyncObject& object3, MilliSecond timeout = LONG_MAX);
						XMultiLocker(XSyncObject& object1, XSyncObject& object2, XSyncObject& object3, XSyncObject& object4, MilliSecond timeout = LONG_MAX);
				
						XMultiLocker(XSyncObject* begin, XSyncObject* end, MilliSecond timeout = LONG_MAX);
						// $$$ Use a template member?

			bool 		AcquiredLock() const						{return mLocked;}
						/**< Returns true if the current thread was able to acquire the lock.
						You should call this if you specified a timeout. */
						
private:
						XMultiLocker(const XMultiLocker& rhs);						
			XMultiLocker& operator=(const XMultiLocker& rhs);
						
public:
	enum {KMaxObjects = 8};

private:
	XSyncObject*	mObjects[KMaxObjects];
	OSSyncObject	mHandles[KMaxObjects];
	bool			mLocked;
};


// ===================================================================================
//	Inlines
// ===================================================================================
inline XSingleLocker::~XSingleLocker()
{	
	if (mLocked)
		mObject.Unlock();		// $$ Unlock may throw so this is a bit dicy. However it seems better to risk a call to terminate than to eat all exceptions...
}

inline XSingleLocker::XSingleLocker(XSyncObject& object, MilliSecond timeout) : mObject(object)
{
	mLocked = mObject.Lock(timeout);
}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
