/*
 *  File:       XThreadSafeIter.h
 *  Summary:	An iterator that only allows one thread at a time to iterate.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XThreadSafeIter.h,v $
 *		Revision 1.3  2000/12/10 08:56:39  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:56:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XThreadSafeIter.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/10 08:56:39  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Converted header comments to Doxygen format.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	  5/27/00	JDJ		Created.
 */

#pragma once

#include <iterator>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XThreadSafeIter
//!		An iterator that only allows one thread at a time to iterate.
// ===================================================================================
template<class ITER, class MUTEX> 
class XThreadSafeIter : public std::iterator<std::bidirectional_iterator_tag, std::iterator_traits<ITER>::value_type> {	

//-----------------------------------
//	Initialization/Destruction
//
public:
								~XThreadSafeIter()								{if (mMutex != nil) mMutex->Unlock();}

								XThreadSafeIter()								{mMutex = nil;}

								XThreadSafeIter(const ITER& iter, MUTEX* mutex) : mIter(iter), mMutex(mutex)		{if (mMutex != nil) mMutex->Lock();}

								XThreadSafeIter(const XThreadSafeIter& rhs) : mIter(rhs.mIter), mMutex(rhs.mMutex)	{if (mMutex != nil) mMutex->Lock();}
			
			XThreadSafeIter& 	operator=(const XThreadSafeIter& rhs);

//-----------------------------------
//	Initialization/Destruction
//
public:
			std::iterator_traits<ITER>::value_type	operator*() const			{return *mIter;}
			std::iterator_traits<ITER>::pointer	operator->() const				{return &(*mIter);}

			XThreadSafeIter&	operator++()									{++mIter; return *this;}
			XThreadSafeIter 	operator++(int)									{XThreadSafeIter temp = *this; ++mIter; return temp;}

			XThreadSafeIter& 	operator--()									{--mIter; return *this;}
			XThreadSafeIter 	operator--(int)									{XThreadSafeIter temp = *this; --mIter; return temp;}
						
			bool 				operator==(const XThreadSafeIter& rhs) const	{return mIter == rhs.mIter;}	
			bool 				operator!=(const XThreadSafeIter& rhs) const	{return mIter != rhs.mIter;}

//-----------------------------------
//	Member Data
//
protected:
	ITER	mIter;
	MUTEX*	mMutex;
};


// ===================================================================================
//	Outlined Methods
// ===================================================================================
template<class ITER, class MUTEX>
XThreadSafeIter<ITER, MUTEX>& XThreadSafeIter<ITER, MUTEX>::operator=(const XThreadSafeIter& rhs)
{
	if (this != &rhs) {
		if (rhs.mMutex != nil) 
			rhs.mMutex->Lock();

		if (mMutex != nil) 
			mMutex->Unlock();
			
		mIter  = rhs.mIter;
		mMutex = rhs.mMutex;
	}
	
	return *this;
}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

