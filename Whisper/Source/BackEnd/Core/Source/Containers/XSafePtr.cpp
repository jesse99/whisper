/*
 *  File:       XSafePtr.cpp
 *  Summary:    Smart pointer class that is notified when it's target is destroyed.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XSafePtr.cpp,v $
 *		Revision 1.3  2001/04/27 04:35:56  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:50:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 1/25/00	JDJ		Changed sSafePtrs from a std::set to a std::list.
 *		 <3>	 6/21/99	JDJ		Renamed XSafePtrMixin (from XSafePtrMixIn).
 *		 <2>	 5/28/99	JDJ		Made thread safe.
 *		 <1>	11/27/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XSafePtr.h>

#include <list>
#include <set>

#include <XCriticalSection.h>

namespace Whisper {


//-----------------------------------
//	Types
//
typedef std::list<ZBaseSafePtr*> SafePtrList;	// Unlike std::set std::list is specialized for pointers so we can save well over 15K by using a std::list. This seems like a good trade-off since it seems unlikely that there will be a lot of safe pointers active at once.
	
	
//-----------------------------------
//	Static Globals
//
static SafePtrList      sSafePtrs;
static XCriticalSection	sPtrMutex;


// ========================================================================================
//	class XSafePtrMixin
// ========================================================================================

//---------------------------------------------------------------
//
// XSafePtrMixin::~XSafePtrMixin
//
//---------------------------------------------------------------
XSafePtrMixin::~XSafePtrMixin()
{
	XEnterCriticalSection enter(sPtrMutex);

	SafePtrList::iterator iter = sSafePtrs.begin();
	while (iter != sSafePtrs.end()) {
		SafePtrList::iterator temp = iter;
		ZBaseSafePtr* ptr = *iter++;
		
		if (ptr->GetTarget() == this) {
			ptr->HandleLostTarget();
			sSafePtrs.erase(temp);
		}
	}
}


//---------------------------------------------------------------
//
// XSafePtrMixin::XSafePtrMixin
//
//---------------------------------------------------------------
XSafePtrMixin::XSafePtrMixin()
{
}


//---------------------------------------------------------------
//
// XSafePtrMixin::AddSafePtr							[static]
//
//---------------------------------------------------------------
void XSafePtrMixin::AddSafePtr(ZBaseSafePtr* ptr)
{
	PRECONDITION(ptr != nil);

	XEnterCriticalSection enter(sPtrMutex);
	PRECONDITION(find(sSafePtrs.begin(), sSafePtrs.end(), ptr) == sSafePtrs.end());
	
	sSafePtrs.push_back(ptr);
}


//---------------------------------------------------------------
//
// XSafePtrMixin::RemoveSafePtr							[static]
//
//---------------------------------------------------------------
void XSafePtrMixin::RemoveSafePtr(ZBaseSafePtr* ptr)
{
	PRECONDITION(ptr != nil);

	XEnterCriticalSection enter(sPtrMutex);
	PRECONDITION(find(sSafePtrs.begin(), sSafePtrs.end(), ptr) != sSafePtrs.end());
	
	sSafePtrs.remove(ptr);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZBaseSafePtr
// ===================================================================================

//---------------------------------------------------------------
//
// ZBaseSafePtr::~ZBaseSafePtr
//
//---------------------------------------------------------------
ZBaseSafePtr::~ZBaseSafePtr()
{
	if (mTarget != nil)
		XSafePtrMixin::RemoveSafePtr(this);

	mTarget = nil;
}


//---------------------------------------------------------------
//
// ZBaseSafePtr::ZBaseSafePtr (XSafePtrMixin*)
//
//---------------------------------------------------------------
ZBaseSafePtr::ZBaseSafePtr(XSafePtrMixin* target)
{
	mTarget = target;

	if (mTarget != nil)
		XSafePtrMixin::AddSafePtr(this);
}


//---------------------------------------------------------------
//
// ZBaseSafePtr::ZBaseSafePtr (ZBaseSafePtr)
//
//---------------------------------------------------------------
ZBaseSafePtr::ZBaseSafePtr(const ZBaseSafePtr& rhs)
{
	mTarget = rhs.mTarget;

	if (mTarget != nil)
		XSafePtrMixin::AddSafePtr(this);
}


//---------------------------------------------------------------
//
// ZBaseSafePtr::operator=
//
//---------------------------------------------------------------
ZBaseSafePtr& ZBaseSafePtr::operator=(const ZBaseSafePtr& rhs)
{
	if (this != &rhs)
		this->Set(rhs.mTarget);
	
	return *this;
}


//---------------------------------------------------------------
//
// ZBaseSafePtr::Set
//
//---------------------------------------------------------------
void ZBaseSafePtr::Set(XSafePtrMixin* target)
{
	if (mTarget != nil) {
		XSafePtrMixin::RemoveSafePtr(this);
		mTarget = nil;
	}
	
	if (target != nil)
		XSafePtrMixin::AddSafePtr(this);

	mTarget = target;
}


//---------------------------------------------------------------
//
// ZBaseSafePtr::HandleLostTarget
//
//---------------------------------------------------------------
void ZBaseSafePtr::HandleLostTarget()
{
	PRECONDITION(mTarget != nil);				// can't lose target twice!
	
	mTarget = nil;
}


}	// namespace Whisper


