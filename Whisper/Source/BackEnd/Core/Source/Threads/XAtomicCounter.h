/*
 *  File:       XAtomicCounter.h
 *  Summary:   	Wrapper around an integer that can be safely incremented and decremented. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XAtomicCounter.h,v $
 *		Revision 1.3  2000/12/10 08:50:12  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:26:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XAtomicCounter.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/10 08:50:12  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Converted header comments to Doxygen format.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 5/23/99	JDJ		Created
 */

#pragma once

#include <XCriticalSection.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XAtomicCounter
//!		Wrapper around an integer that can be safely incremented and decremented.
// ===================================================================================
class CORE_EXPORT XAtomicCounter {
	
//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XAtomicCounter();
	
  						XAtomicCounter(int32 count = 0);

						XAtomicCounter(const XAtomicCounter& rhs);

			XAtomicCounter& operator=(const XAtomicCounter& rhs);

//-----------------------------------
//	API
//
public:
				 		operator int32() const						{return mCount;}
	
			int32 		operator++();
			int32 		operator--();
						/**< To avoid race conditions you should use the returned value
						in conditionals (eg "if (++mCount == 1)" instead of
						"++mCount; if (mCount == 1)". */

//-----------------------------------
//	Member Data
//
private:
	volatile int32		mCount;
#if WIN
	XCriticalSection	mSection;		// used on Win95 (but not Win98)
#endif
};


// ===================================================================================
//	Inlines
// ===================================================================================
#if MAC
inline int32 XAtomicCounter::operator++()
{
	return ++mCount;
}
#endif

#if MAC
inline int32 XAtomicCounter::operator--()
{
	return --mCount;
}
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
