/*
 *  File:       XAutoPtr.h
 *  Summary:    A class that contains a pointer and deletes it when it goes out of scope.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XAutoPtr.h,v $
 *		Revision 1.3  2000/12/10 08:50:15  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:26:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XAutoPtr.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/10 08:50:15  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Converted header comments to Doxygen format.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <2>	11/02/00	JDJ		Replaced IsValid with a const void* conversion operator.
 *		 <1>	11/27/97	JDJ		Created (from Raven)
 */

#pragma once

#include <XDebug.h>

namespace Whisper {


// ===================================================================================
//	class XAutoPtr
//!		A class that contains a pointer and deletes it when it goes out of scope.
// ===================================================================================
template<class T> class XAutoPtr {

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XAutoPtr();

	explicit 			XAutoPtr(T* takePtr = nil)			{mPtr = takePtr;}
						/**< Ctor is explicit because the pointer is taken. */
	
private:
						XAutoPtr(const XAutoPtr<T>& rhs);
						/**< Use an XCountedPtr if you need a copyable smart pointer. */
			
			XAutoPtr<T>& operator=(const XAutoPtr<T>& rhs);
			
//-----------------------------------
//	API
//
public:
	// ----- Smart Pointer Operations -----
			T&			operator*() const					{ASSERT(mPtr != nil); return *mPtr;}
			
			T*			operator->() const					{ASSERT(mPtr != nil); return mPtr;}
			
	// ----- Misc -----
			T*			Get() const							{return mPtr;}
						/**< This is a dangerous operation: if you save the pointer and the
						last XAutoPtr goes away you'll be left with a dangling pointer. */
						
						operator const void*() const		{return mPtr;}
						/**< Used for validity tests (this is safer than using bool). */
			
			T*			Reset(T* takePtr);
						/**< Modifies 'this' to point to takePtr and returns mPtr. */

			T*			Release();
						/**< Sets mPtr to nil and returns the old pointer. */

//-----------------------------------
//	Member data
//
protected:
	T*		mPtr;
};


// ===================================================================================
//	Inlines
// ===================================================================================
template<class T>
inline XAutoPtr<T>::~XAutoPtr()			// Note that CW Pro 1 won't let you forward declare T if this is in the XAutoPtr declaration						
{
	delete mPtr;
}


template<class T>
T* XAutoPtr<T>::Reset(T* takePtr)
{
	if (mPtr != takePtr) {
		delete mPtr;
		
		mPtr = takePtr;
	}
	
	return mPtr;
}

template<class T>
T* XAutoPtr<T>::Release()
{
	T* oldPtr = mPtr;
	
	mPtr = nil;
	
	return oldPtr;
}


}	// namespace Whisper

