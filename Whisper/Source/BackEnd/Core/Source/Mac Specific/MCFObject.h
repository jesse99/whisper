/*
 *  File:       MCFObject.h
 *  Summary:    Wrapper around Core Foundation objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MCFObject.h,v $
 *		Revision 1.5  2001/04/19 04:10:36  jesjones
 *		Added a const void* conversion operator.
 *		
 *		Revision 1.4  2001/04/13 07:48:56  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2000/12/10 08:48:25  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:10:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <CFBase.h>

#include <XDebug.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class MCFObject
//!		Wrapper around Core Foundation objects.
// ===================================================================================
template<class T> class MCFObject {

//-----------------------------------
//	Types
//
public:
	typedef T value_type;

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~MCFObject();

			 			MCFObject();

	explicit 			MCFObject(T takeObject);
						/**< Ctor is explicit because the pointer is taken. */
	
						MCFObject(const MCFObject<T>& rhs);
			
			MCFObject<T>& operator=(const MCFObject<T>& rhs);
			
//-----------------------------------
//	API
//
public:
	// ----- Smart Pointer Operations -----
			T			operator*() const							{ASSERT(mObject != nil); return mObject;}
			
			T*			operator->() const							{ASSERT(mObject != nil); return &mObject;}
			
	// ----- Comparisons -----
			bool 		operator==(const MCFObject<T>& rhs) const	{return mObject == rhs.mObject;}
			bool 		operator!=(const MCFObject<T>& rhs) const	{return mObject != rhs.mObject;}
			bool 		operator<(const MCFObject<T>& rhs) const	{return mObject < rhs.mObject;}
			bool 		operator<=(const MCFObject<T>& rhs) const	{return mObject <= rhs.mObject;}
			bool 		operator>(const MCFObject<T>& rhs) const	{return mObject > rhs.mObject;}
			bool 		operator>=(const MCFObject<T>& rhs) const	{return mObject >= rhs.mObject;}

			bool 		operator==(T rhs) const						{return mObject == rhs;}
			bool 		operator!=(T rhs) const						{return mObject != rhs;}
			bool 		operator<(T rhs) const						{return mObject < rhs;}
			bool 		operator<=(T rhs) const						{return mObject <= rhs;}
			bool 		operator>(T rhs) const						{return mObject > rhs;}
			bool 		operator>=(T rhs) const						{return mObject >= rhs;}

	// ----- Misc -----
						operator const void*() const				{return mObject;}
						/**< Used for validity tests (this is safer than using bool). */
			
			T			Get() const									{return mObject;}
						/**< This is a dangerous operation: if you save the result and the
						last MCFObject goes away you'll be left with a dangling pointer. */
			
//-----------------------------------
//	Member Data
//
protected:
	T	mObject;
};


// ===================================================================================
//	Inlines
// ===================================================================================
CORE_EXPORT void ThrowIfNil(const void* ptr);	// get a cycle if we include <XExceptions.h>

template<class T>
MCFObject<T>::~MCFObject()
{
	if (mObject != nil)
		CFRelease(mObject);
}

template<class T>
MCFObject<T>::MCFObject()
{
	mObject = nil;
}

template<class T>
MCFObject<T>::MCFObject(T takeObject)
{
	mObject = takeObject;
	ThrowIfNil(mObject);
}

template<class T>
MCFObject<T>::MCFObject(const MCFObject<T>& rhs)
{
	mObject = rhs.mObject;
	if (mObject != nil)
		CFRetain(mObject);
}

template<class T>
MCFObject<T>& MCFObject<T>::operator=(const MCFObject<T>& rhs)
{
	if (mObject != rhs.mObject) {
		if (mObject != nil)
			CFRelease(mObject);
		
		mObject = rhs.mObject;
		if (mObject != nil)
			CFRetain(mObject);
	}
	
	return *this;
}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

