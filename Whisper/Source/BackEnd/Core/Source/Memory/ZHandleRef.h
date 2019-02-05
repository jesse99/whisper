/*
 *  File:		ZHandleRef.h
 *  Summary:	Letter used by XHandle.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: ZHandleRef.h,v $
 *		Revision 1.5  2001/04/17 01:45:46  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/03/05 05:44:50  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:57:38  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/10 04:49:38  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XConstants.h>
#include <XInvariant.h>
#include <XLockable.h>
#include <XReferenceCounted.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class ZHandleRef
//!		Letter used by XHandle.
// ===================================================================================
class CORE_EXPORT ZHandleRef : public XLockableMixin, public XReferenceCountedMixin {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~ZHandleRef();

						ZHandleRef(uint32 bytes, uint32 options);

#if MAC
						ZHandleRef(Handle macHandle);
#endif

private:
						ZHandleRef(const ZHandleRef& rhs);

			ZHandleRef&	operator=(const ZHandleRef& rhs);

//-----------------------------------
//	New API
//
public:
	// ----- Access -----
			const uint8* GetPtr() const;
						// Note that the handle may be unlocked (XHandle::GetPtr does the checking).

			uint8* 		GetPtr();

#if MAC
			Handle 		GetHandle();
#endif

	// ----- Sizing -----
			uint32 		GetSize() const							{return mBytes;}

			void 		SetSize(uint32 bytes, bool zeroBytes);

	// ----- Misc -----
#if MAC && DEBUG

			void 		ResetTail();
#endif

//-----------------------------------
//	Inherited API
//
protected:
			void 		Invariant() const;
	
	virtual void 		OnLock(bool moveHigh);

	virtual void 		OnUnlock();

//-----------------------------------
//	Member Data
//
private:
#if MAC
	Handle	mHandle;
#elif WIN
	uint8*	mHandle;
#endif

	uint32	mBytes;				// doesn't include tail
	
#if MAC && DEBUG				// on Windows we'll rely on the debugging support built into operator new
	bool	mHasTail;
#endif
};


// ===================================================================================
//	Inlines
// ===================================================================================
#if MAC
	inline const uint8* ZHandleRef::GetPtr() const
	{
		return (uint8*) *mHandle;
	}

	inline uint8* ZHandleRef::GetPtr()
	{
		return (uint8*) *mHandle;
	}

	inline Handle ZHandleRef::GetHandle()
	{	
		return mHandle;
	}

#elif WIN
	inline const uint8* ZHandleRef::GetPtr() const
	{
		return mHandle;
	}

	inline uint8* ZHandleRef::GetPtr()
	{
		return mHandle;
	}
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
