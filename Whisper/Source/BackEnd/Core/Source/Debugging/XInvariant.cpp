/*
 *  File:       XInvariant.cpp
 *  Summary:    Classes and macros used to check object invariants.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XInvariant.cpp,v $
 *		Revision 1.7  2001/10/04 06:13:56  jesjones
 *		Tweaked ValidatePtr for OS X.
 *		
 *		Revision 1.6  2001/04/27 04:33:41  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.5  2001/04/17 01:44:00  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/04/13 08:01:25  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2001/03/08 08:09:53  jesjones
 *		Outlined the methods in release (to avoid linker warnings when building DLL targets).
 *		
 *		Revision 1.2  2000/11/09 12:40:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XInvariant.h>

#include <XCriticalSection.h>
#include <XDebug.h>

namespace Whisper {
#if DEBUG

 
// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// ValidatePtr
//
//---------------------------------------------------------------
static void ValidatePtr(const void* thisPtr)
{
	ASSERT(thisPtr != nil);					// may fire since we no longer get here via a virtual function

	ASSERT(((int32) thisPtr & 3) == 0);		// PPC and Intel objects are 4-byte aligned

#if MAC
//	ASSERT(IsPointerValid((Ptr) thisPtr));	// $$$ doesn't seem to be working in OS 10.1
#endif
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZCheckInvariant
// ===================================================================================

static std::map<const void*, int32> sNesting;
static XCriticalSection sMutex;

//---------------------------------------------------------------
//
// ZCheckInvariant::EnteringObject						[static]
//
//---------------------------------------------------------------
bool ZCheckInvariant::EnteringObject(const void* thisPtr)
{
	ValidatePtr(thisPtr);	
	
	XEnterCriticalSection lock(sMutex);

	int32& count = sNesting[thisPtr];	// default construction of a POD type zero-initializes (see section 8.5 of the standard)
	bool entered = ++count == 1;
		
	return entered;
}


//---------------------------------------------------------------
//
// ZCheckInvariant::LeavingObject						[static]
//
//---------------------------------------------------------------
void ZCheckInvariant::LeavingObject(const void* thisPtr)
{
	ValidatePtr(thisPtr);	
	
	XEnterCriticalSection lock(sMutex);
	
	std::map<const void*, int32>::iterator iter = sNesting.find(thisPtr);
	ASSERT(iter != sNesting.end());
	ASSERT(iter->second > 0);
	
	if (--(iter->second) == 0)
		sNesting.erase(iter);
}


#endif	// DEBUG
}		// namespace Whisper


