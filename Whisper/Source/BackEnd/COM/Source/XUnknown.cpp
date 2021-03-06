/*
 *  File:       XUnknown.cpp
 *  Summary:    Base class for all interface classes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XUnknown.cpp,v $
 *		Revision 1.5  2001/04/27 04:37:55  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/04/17 01:45:14  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.3  2001/04/01 05:01:00  jesjones
 *		Added a comment.
 *		
 *		Revision 1.2  2000/11/09 12:58:39  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XUnknown.h>

#include <XBoss.h>

namespace Whisper {


// ===================================================================================
//	class XUnknown
// ===================================================================================

//---------------------------------------------------------------
//
// XUnknown::~XUnknown
//
//---------------------------------------------------------------
XUnknown::~XUnknown()
{
	CALL_INVARIANT;		
		
#if !GARBAGE_COLLECT_COM
	PRECONDITION(mRefCount == 0);
#endif
}


//---------------------------------------------------------------
//
// XUnknown::XUnknown
//
//---------------------------------------------------------------
XUnknown::XUnknown()
{		
	mBoss = nil;
	
#if DEBUG && !GARBAGE_COLLECT_COM
	mRefCount = 0;
	mBreakOnRefs = false;
#endif

//	CALL_INVARIANT;		// can't do this yet because the boss hasn't been set		
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XUnknown::Invariant
//
//---------------------------------------------------------------
void XUnknown::Invariant() const
{	
	ASSERT(mBoss != nil);
#if !GARBAGE_COLLECT_COM
	ASSERT(mRefCount >= 0);
#endif
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XUnknown::DoAddReference
//
//---------------------------------------------------------------
#if !GARBAGE_COLLECT_COM
void XUnknown::DoAddReference()
{
#if DEBUG
	++mRefCount;
	ASSERT(mRefCount > 0);
	ASSERT(mBoss != nil);			// forgot to call DoSetBoss?
	DEBUGSTR_IF(mBreakOnRefs, "Added a reference to ", mBoss->GetName());
#endif

	mBoss->AddReference();
}
#endif


//---------------------------------------------------------------
//
// XUnknown::DoRemoveReference
//
// Note that 'this' will not be deleted until the boss ref count
// goes to zero. This is what we want: interfaces are conceptually
// parts of a larger whole (the boss) so we keep them around even 
// if clients no longer reference them. 
//
//---------------------------------------------------------------
#if !GARBAGE_COLLECT_COM
void XUnknown::DoRemoveReference()
{
#if DEBUG
	--mRefCount;
	ASSERT(mRefCount >= 0);
	ASSERT(mBoss != nil);
	DEBUGSTR_IF(mBreakOnRefs, "Released a reference to ", mBoss->GetName());
#endif

	mBoss->RemoveReference();		// we may be deleted after this executes
}
#endif


}	// namespace Whisper

