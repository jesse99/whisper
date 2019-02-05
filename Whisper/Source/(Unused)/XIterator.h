/*
 *  File:       XIterator.h
 *  Summary:   	Inheritance based iterator classes for use with interface classes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XIterator.h,v $
 *		Revision 1.2  2000/11/09 12:40:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 1/12/00	JDJ		Created
 */

#pragma once

#include <XCountedPtr.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XForwardIteratorHelper
// ===================================================================================
template <class T>
class XForwardIteratorHelper {

public:		 			
	virtual T& 			Get() const = 0;
	
	virtual void 		Advance() = 0;
	
	virtual int32 		Compare(const XForwardIteratorHelper<T>& rhs) const = 0;
};


// ===================================================================================
//	class XForwardIterator
// ===================================================================================
template <class T>
class XForwardIterator : public std::iterator<std::forward_iterator_tag, T> {

public:
	typedef XCountedPtr<XForwardIteratorHelper<T> > Iterator;

public:								
								XForwardIterator()			{}
								XForwardIterator(const Iterator& helper) : mIterator(takeHelper)	{ASSERT(takeHelper.Get() != nil);}
								
			reference			operator*() const 			{ASSERT(takeHelper.Get() != nil); return mIterator->Get();}
			pointer 			operator->() const 			{ASSERT(takeHelper.Get() != nil); return &mIterator->Get();}

			XForwardIterator& 	operator++() 				{ASSERT(takeHelper.Get() != nil); mIterator->Advance(); return *this;}
			XForwardIterator 	operator++(int) 			{ASSERT(takeHelper.Get() != nil); XForwardIterator temp = *this; mIterator->Advance(); return temp;}

			bool 				operator==(const IExtensibleIterator<T>& rhs) const	{return mIterator->Compare(*rhs.mIterator) == 0;}
			bool 				operator!=(const IExtensibleIterator<T>& rhs) const	{return mIterator->Compare(*rhs.mIterator) != 0;}
	
private:
	Iterator	mIterator;
};



#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
