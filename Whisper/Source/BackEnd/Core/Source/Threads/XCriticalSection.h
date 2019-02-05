/*
 *  File:       XCriticalSection.h
 *  Summary:   	Provides a fast way to prevent other threads from touching a shared resource. 
 *  Written by: Jesse Jones
 *
 *	Classes:	XCriticalSection	  - Create one of these for each shared resource.
 *				XEnterCriticalSection - Use this when manipulating the resource.
 *
 *  Copyright © 1999-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XCriticalSection.h,v $
 *		Revision 1.5  2001/03/05 05:41:01  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.4  2001/03/01 11:42:17  jesjones
 *		Disabled copying for a few classes.
 *		
 *		Revision 1.3  2000/12/10 08:51:09  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:29:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#if MAC
	#include <Threads.h>
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XCriticalSection
//!		The simplest and fastest way to protect a shared resource. Used in conjunction with XEnterCriticalSection.
// ===================================================================================
class CORE_EXPORT XCriticalSection {

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XCriticalSection();

						XCriticalSection();
						
private:
						XCriticalSection(const XCriticalSection& rhs);						
			XCriticalSection& operator=(const XCriticalSection& rhs);

//-----------------------------------
//	API
//
public:
			void 		Lock();
						/**< Note that critical sections are recursive: the same thread
						can re-enter a critical section without blocking. */
			
			void 		Unlock();

//-----------------------------------
//	Member Data
//
protected:					
#if WIN
	volatile CRITICAL_SECTION	mObject;
#elif MAC
	volatile ThreadID			mOwner;
	volatile int32				mLockCount;
#endif
};


// ===================================================================================
//	class XEnterCriticalSection
//!		Used to lock a XCriticalSection.
// ===================================================================================
class CORE_EXPORT XEnterCriticalSection {

public:
				~XEnterCriticalSection();

				XEnterCriticalSection(XCriticalSection& object);
				
private:
				XEnterCriticalSection(const XEnterCriticalSection& rhs);
		XEnterCriticalSection& operator=(const XEnterCriticalSection& rhs);
				
private:
	XCriticalSection&	mObject;
};


// ===================================================================================
//	Inlines
// ===================================================================================
#if WIN
inline void XCriticalSection::Lock()
{
	::EnterCriticalSection(const_cast<CRITICAL_SECTION*>(&mObject));
}
#endif

#if WIN
inline void XCriticalSection::Unlock()
{
	::LeaveCriticalSection(const_cast<CRITICAL_SECTION*>(&mObject));
}
#endif


inline XEnterCriticalSection::~XEnterCriticalSection()
{
	mObject.Unlock();
}

inline XEnterCriticalSection::XEnterCriticalSection(XCriticalSection& object) : mObject(object)
{
	mObject.Lock();
}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
