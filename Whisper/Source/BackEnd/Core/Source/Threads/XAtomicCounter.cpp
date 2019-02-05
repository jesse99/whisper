/*
 *  File:       XAtomicCounter.cpp
 *  Summary:   	Wrapper around an integer that can be safely incremented and decremented. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XAtomicCounter.cpp,v $
 *		Revision 1.2  2000/11/09 12:26:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 5/23/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XAtomicCounter.h>

#if WIN
	#include <WSystemInfo.h>
#endif

namespace Whisper {


// ===================================================================================
//	class XAtomicCounter
// ===================================================================================

//---------------------------------------------------------------
//
// XAtomicCounter::~XAtomicCounter
//
//---------------------------------------------------------------
XAtomicCounter::~XAtomicCounter()
{
}


//---------------------------------------------------------------
//
// XAtomicCounter::XAtomicCounter (int32)
//
//---------------------------------------------------------------
XAtomicCounter::XAtomicCounter(int32 count) 
{
	mCount = count;
}


//---------------------------------------------------------------
//
// XAtomicCounter::XAtomicCounter (XAtomicCounter)
//
//---------------------------------------------------------------
XAtomicCounter::XAtomicCounter(const XAtomicCounter& rhs)
{
	mCount = rhs.mCount;
}


//---------------------------------------------------------------
//
// XAtomicCounter::operator= (XAtomicCounter)
//
//---------------------------------------------------------------
XAtomicCounter& XAtomicCounter::operator=(const XAtomicCounter& rhs)
{
	if (this != &rhs)
		mCount = rhs.mCount;
	
	return *this;
}


//---------------------------------------------------------------
//
// XAtomicCounter::operator++
//
//---------------------------------------------------------------
#if WIN
int32 XAtomicCounter::operator++()
{
	int32 count;
	
	if (!WSystemInfo::IsNT() && !WSystemInfo::IsWin98()) {
		XEnterCriticalSection enter(mSection);			// on Win95 the interlocked functions only return +1, 0, or -1
		++mCount;
		
		count = mCount;
	
	} else 
		count = ::InterlockedIncrement(const_cast<int32*>(&mCount));	// to avoid a race condition we *have* to use the return value
	
	return count;
}
#endif


//---------------------------------------------------------------
//
// XAtomicCounter::operator--
//
//---------------------------------------------------------------
#if WIN
int32 XAtomicCounter::operator--()
{
	int32 count;
		
	if (!WSystemInfo::IsNT() && !WSystemInfo::IsWin98()) {
		XEnterCriticalSection enter(mSection);			
		--mCount;
		
		count = mCount;
	
	} else 
		count = ::InterlockedDecrement(const_cast<int32*>(&mCount));	
	
	return count;
}
#endif


}	// namespace Whisper
