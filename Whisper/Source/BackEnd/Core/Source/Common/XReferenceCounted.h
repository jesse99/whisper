/*
 *  File:       XReferenceCounted.h
 *  Summary:    Mixin class to help with reference counting.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XReferenceCounted.h,v $
 *		Revision 1.4  2000/12/15 09:07:47  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/10 08:54:59  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:48:27  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 8/25/00	JDJ		Added XBumpRefCount.
 *		 <2>	 5/27/99	JDJ		Uses XAtomicCounter.
 *		 <1>	11/27/97	JDJ		Created (from Raven)
 */

#pragma once

#include <XAtomicCounter.h>
#include <XDebug.h>
#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XReferenceCountedMixin
//!		Mixin class to help with reference counting.
/*!		Reference counting is basicly a weak form of garbage collection: it allows one 
 *		object to be shared by other objects. When there are no more references to the 
 *		first object it is automatically deleted.
 *
 *		Using reference counting reduces memory requirements and also always heavy weight 
 *		objects to be quickly copied.
 *
 *		Reference counting is typically implemented using the envelope/letter pattern. 
 *		The envelope is the class users see. The envelope has a pointer to the letter 
 *		which does all the work. Letters can belong to more than one envelope. Here's 
 *		an example of how this works: \code
 *			CString::~CString() 
 *			{
 *				mRefPtr->RemoveReference();			// when refCount goes to zero letter deletes itself
 *			}
 *
 *			CString::CString(const char* str) 
 *			{
 *				mRefPtr = new TStringRef(str);
 *			}
 *
 *			CString::CString(const CString& rhs) 
 *			{
 *				mRefPtr = rhs.mRefPtr; 				// note that the pointer is copied -- not the data!
 *				mRefPtr->AddReference();
 *			}
 *
 *			CString& CString::operator=(const CString& rhs)
 *			{
 *				if (mRefPtr != rhs.mRefPtr) {
 *					mRefPtr->RemoveReference();		
 *
 *					mRefPtr = rhs.mRefPtr;
 *					mRefPtr->AddReference();
 *				}
 *				return *this;
 *			} \endcode */
// ===================================================================================
class CORE_EXPORT XReferenceCountedMixin {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XReferenceCountedMixin() = 0;

	explicit 			XReferenceCountedMixin(int32 refCount = 1);
	
//-----------------------------------
//	API
//
public:
	virtual void 		AddReference();
			
	virtual void 		RemoveReference();
						/**< Deletes the object if no one is referencing it. */
			
			int32 		GetRefCount() const						{return mRefCount;}

protected:
	virtual void 		OnFirstReference();
						/**< Defaults to nothing. */
	
	virtual void 		OnLastReference();
						/**< Defaults to deleting the object. */

//-----------------------------------
//	Member Data
//
private:
	XAtomicCounter	mRefCount;
};


// ===================================================================================
//	class XBumpRefCount
//!		Stack based class used to bump an XReferenceCountedMixin's ref count.
// ===================================================================================
class CORE_EXPORT XBumpRefCount {

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XBumpRefCount()  													{mObject->RemoveReference();}

		 				XBumpRefCount(XReferenceCountedMixin* object) : mObject(object) 	{ASSERT(mObject != nil); mObject->AddReference();}
		 				XBumpRefCount(XReferenceCountedMixin& object) : mObject(&object) 	{mObject->AddReference();}
		 				
private:
						XBumpRefCount(const XBumpRefCount& rhs);
		XBumpRefCount& 	operator=(const XBumpRefCount& rhs);
	
//-----------------------------------
//	Member Data
//
private:
	XReferenceCountedMixin*	mObject;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

