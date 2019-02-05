/*
 *  File:       XCountedPtr.h
 *  Summary:    Reference counted pointer class (ie the pointer is deleted when the last
 *				XCountedPtr that references it is destroyed).
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XCountedPtr.h,v $
 *		Revision 1.5  2001/05/30 01:18:12  jesjones
 *		Raw pointer comparisons are now done via the operator const void* method.
 *		
 *		Revision 1.4  2001/03/05 05:40:54  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:51:02  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:29:25  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	11/02/00	JDJ		Replaced IsValid with a const void* conversion operator.
 *		 <3>	 1/23/00	JDJ		Made thread safe.
 *		 <2>	 5/21/99	JDJ		Overloaded comparison operators for const T*'s.
 *		 <1>	11/27/97	JDJ		Created (from Raven)
 */

#pragma once

#include <XAtomicCounter.h>
#include <XDebug.h>

namespace Whisper {


// ===================================================================================
//	class XCountedPtr
//!		Reference counted pointer class (ie the pointer is deleted when the last XCountedPtr that references it is destroyed).
// ===================================================================================
template<class T> class XCountedPtr {

//-----------------------------------
//	Types
//
public:
	typedef T value_type;

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XCountedPtr();

	explicit 			XCountedPtr(T* takePtr = nil);
						/**< Ctor is explicit because the pointer is taken. */
	
						XCountedPtr(const XCountedPtr<T>& rhs);
			
			XCountedPtr<T>& operator=(const XCountedPtr<T>& rhs);
			
//-----------------------------------
//	API
//
public:
	// ----- Smart Pointer Operations -----
			T&			operator*() const							{ASSERT(mPtr != nil); return *mPtr;}
			
			T*			operator->() const							{ASSERT(mPtr != nil); return mPtr;}
			
	// ----- Comparisons -----
			bool 		operator==(const XCountedPtr<T>& rhs) const	{return mPtr == rhs.mPtr;}
			bool 		operator!=(const XCountedPtr<T>& rhs) const	{return mPtr != rhs.mPtr;}
			bool 		operator<(const XCountedPtr<T>& rhs) const	{return mPtr < rhs.mPtr;}
			bool 		operator<=(const XCountedPtr<T>& rhs) const	{return mPtr <= rhs.mPtr;}
			bool 		operator>(const XCountedPtr<T>& rhs) const	{return mPtr > rhs.mPtr;}
			bool 		operator>=(const XCountedPtr<T>& rhs) const	{return mPtr >= rhs.mPtr;}

	// ----- Misc -----
			T*			Get() const									{return mPtr;}
						/**< This is a dangerous operation: if you save the pointer and the
						last XCountedPtr goes away you'll be left with a dangling pointer. */
						
						operator const void*() const				{return mPtr;}
						/**< Used for validity tests (this is safer than using bool). Note
						that this is also used for comparisons that use raw pointers. */
			
			T*			Reset(T* takePtr);
						/**< Modifies 'this' to point to takePtr. */
	
			uint32 		GetRefCount() const							{return *mRefCount;}

//-----------------------------------
//	Member Data
//
private:
	T*				mPtr;
	XAtomicCounter*	mRefCount;
};


// ===================================================================================
//	Inlines
// ===================================================================================
template<class T>
XCountedPtr<T>::~XCountedPtr()
{
	if (--(*mRefCount) == 0) {
		delete mPtr;
		delete mRefCount;
	}
}

template<class T>
inline XCountedPtr<T>::XCountedPtr(T* takePtr)
{
	mPtr = takePtr;
	mRefCount = new XAtomicCounter(1);
}

template<class T>
XCountedPtr<T>::XCountedPtr(const XCountedPtr<T>& rhs)
{
	mPtr = rhs.mPtr;				// it's safe to do this assignment before bumping the ref count because our caller is in the same thread and has an outstanding reference
	mRefCount = rhs.mRefCount;
	++(*mRefCount);
}

template<class T>
XCountedPtr<T>& XCountedPtr<T>::operator=(const XCountedPtr<T>& rhs)
{
	if (mPtr != rhs.mPtr) {
		if (--(*mRefCount) == 0) {
			delete mPtr;
			delete mRefCount;
		}
		
		mPtr = rhs.mPtr;
		mRefCount = rhs.mRefCount;
		++(*mRefCount);
	}
	
	return *this;
}

template<class T>
T* XCountedPtr<T>::Reset(T* takePtr)
{
	if (mPtr != takePtr) {
		XAtomicCounter* newCount = new XAtomicCounter(1);
		if (--(*mRefCount) == 0) {
			delete mPtr;
			delete mRefCount;
		}
		
		mPtr = takePtr;
		mRefCount = newCount;
	}
	
	return mPtr;
}


}	// namespace Whisper

