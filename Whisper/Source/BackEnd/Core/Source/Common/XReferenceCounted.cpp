/*
 *  File:       XReferenceCounted.h
 *  Summary:    Mixin class to help with reference counting.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XReferenceCounted.cpp,v $
 *		Revision 1.3  2001/04/27 04:35:38  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:48:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XReferenceCounted.h>

namespace Whisper {


// ===================================================================================
//	class XReferenceCountedMixin
// ===================================================================================

//---------------------------------------------------------------
//
// XReferenceCountedMixin::~XReferenceCountedMixin
//
//---------------------------------------------------------------
XReferenceCountedMixin::~XReferenceCountedMixin()
{
	// It's legal to delete an object with a ref count of one so
	// that XReferenceCountedMixin objects can be used on the stack.
	PRECONDITION(mRefCount <= 1);				
}


//---------------------------------------------------------------
//
// XReferenceCountedMixin::XReferenceCountedMixin
//
//---------------------------------------------------------------
XReferenceCountedMixin::XReferenceCountedMixin(int32 refCount) : mRefCount(refCount)
{
	PRECONDITION(refCount >= 0);
}


//---------------------------------------------------------------
//
// XReferenceCountedMixin::AddReference
//
//---------------------------------------------------------------
void XReferenceCountedMixin::AddReference()
{
	PRECONDITION(mRefCount >= 0);
		
	if (++mRefCount == 1)				
		this->OnFirstReference();
}


//---------------------------------------------------------------
//
// XReferenceCountedMixin::RemoveReference
//
//---------------------------------------------------------------
void XReferenceCountedMixin::RemoveReference()
{
	PRECONDITION(mRefCount >= 1);
		
	if (--mRefCount == 0)				
		this->OnLastReference();
}


//---------------------------------------------------------------
//
// XReferenceCountedMixin::OnFirstReference
//
//---------------------------------------------------------------
void XReferenceCountedMixin::OnFirstReference()
{
	PRECONDITION(mRefCount == 1);
}


//---------------------------------------------------------------
//
// XReferenceCountedMixin::OnLastReference
//
//---------------------------------------------------------------
void XReferenceCountedMixin::OnLastReference()
{
	PRECONDITION(mRefCount == 0);

	delete this;
}


}	// namespace Whisper

