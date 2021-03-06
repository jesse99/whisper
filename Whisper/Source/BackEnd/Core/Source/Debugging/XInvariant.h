/*
 *  File:       XInvariant.h
 *  Summary:    Classes and macros used to check object invariants.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XInvariant.h,v $
 *		Revision 1.8  2001/04/21 01:44:31  jesjones
 *		CHECK_INVARIANT now creates a stack based class that calls the invariant when it's constructed and destructed. Added CALL_INVARIANT. Moved PRECONDITION and POSTCONDITION to XDebug.h
 *		
 *		Revision 1.7  2001/04/17 01:44:03  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.6  2001/03/08 08:09:57  jesjones
 *		Outlined the methods in release (to avoid linker warnings when building DLL targets).
 *		
 *		Revision 1.5  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.4  2000/12/10 08:53:31  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.3  2000/11/09 12:40:15  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XDebug.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class ZCheckInvariant
//!		Used by CHECK_INVARIANT to call Invariant methods.
// ===================================================================================
#if DEBUG
class CORE_EXPORT ZCheckInvariant {

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~ZCheckInvariant()		{LeavingObject(mObject); if (mInvoker) mInvoker(mObject);}

						template <class T>
						ZCheckInvariant(const T* object) : mObject(object)	{mInvoker = nil; if (EnteringObject(mObject)) {mInvoker = InvokeInvariant<T>; mInvoker(mObject);}}

//-----------------------------------
//	Internal API
//
public:
	static	bool 		EnteringObject(const void* object);
	static	void 		LeavingObject(const void* object);

private:					
						template <class T>
	static	void 		InvokeInvariant(const void* object)	{void (T::*method)() const = &T::Invariant; (static_cast<const T*>(object)->*method)();}

//-----------------------------------
//	Member Data
//
private:					
	void (*mInvoker)(const void*);
	const void*	mObject;
};
#endif


// ===================================================================================
//	class XDisableInvariant
//!		Stack based class used to disable calls to Invariant methods.
// ===================================================================================
#if DEBUG
class CORE_EXPORT XDisableInvariant {

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XDisableInvariant()									{ZCheckInvariant::LeavingObject(mObject);}

						template <class T>
						XDisableInvariant(const T* object) : mObject(object)	{(void) ZCheckInvariant::EnteringObject(mObject);}

//-----------------------------------
//	Member Data
//
private:					
	const void*	mObject;
};
#endif

}	// namespace Whisper


// ===================================================================================
//	Macros
//		CHECK_INVARIANT
//		This is typically placed after the PRECONDITION statements. It creates a stack 
//		based class to call the invariant. The invariant is a method that clients write 
//		and is used to verify that the object's state is correct. Note that you should 
//		not check invariants in protected or public methods: the object's state may become 
//		temporarily invalid as a method executes. 
//
//		CALL_INVARIANT
//		Makes a direct call to the invariant. This is useful in constructors and destructors
//		(the stack based class CHECK_INVARIANT creates makes two calls to the invariant
//		which is inefficient in ctors and often erroneous in dtors).
// ===================================================================================
#if DEBUG
 	#define CHECK_INVARIANT			Whisper::ZCheckInvariant _checker(this)
	#define CALL_INVARIANT			this->Invariant()

#else	
	#define CHECK_INVARIANT			((void) 0)
	#define CALL_INVARIANT			((void) 0)
#endif	


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif


