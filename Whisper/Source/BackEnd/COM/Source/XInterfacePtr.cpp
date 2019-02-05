/*
 *  File:       XInterfacePtr.cpp
 *  Summary:    Smart pointer class to make interfaces easier to work with.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XInterfacePtr.cpp,v $
 *		Revision 1.6  2001/10/03 02:12:35  jesjones
 *		Added a null type which can be used to avoid ambiguities.
 *		
 *		Revision 1.5  2001/03/17 12:26:06  jesjones
 *		Ctors throw if the implementation class wasn't registered.
 *		
 *		Revision 1.4  2001/03/05 05:42:14  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/11/26 03:26:59  jesjones
 *		Changed the copy ctor and assignment operator so that they no longer do a query when assigning a T to a const T.
 *		
 *		Revision 1.2  2000/11/09 12:39:49  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 8/24/00	JDJ		Added ZBaseInterfacePtr::mBasePtr so that XImplementation::
 *									GetInterface can increment the interface's ref count (so 
 *									query in the first constructed interface doesn't delete 
 *									the boss).
 *		 <4>	 7/14/00	JDJ		Added ZBaseInterfacePtr::Release.
 *		 <3>	 5/09/00	JDJ		Added XInterfacePtr ctors that XBoss::iterator's.
 *		 <2>	 4/28/00	JDJ		Added XInterfacePtr ctors that take raw pointers.
 *		 <1>	11/27/99	JDJ		Created (from XCountedPtr)
 */

#include <XWhisperHeader.h>
#include <XInterfacePtr.h>

#include <XBoss.h>
#include <XObjectModel.h>
#include <XStringUtils.h>

#if GARBAGE_COLLECT_COM
	#include <XGarbageCollector.h>
#endif

namespace Whisper {

const null_t null = null_t();

// ===================================================================================
//	class ZBaseInterfacePtr
// ===================================================================================
	
//---------------------------------------------------------------
//
// ZBaseInterfacePtr::~XInterfacePtr
//
//---------------------------------------------------------------
ZBaseInterfacePtr::~ZBaseInterfacePtr()
{
#if !GARBAGE_COLLECT_COM
	if (mBasePtr != nil)
		mBasePtr->DoRemoveReference();	

	if (mPtr != nil)
		mPtr->DoRemoveReference();
#else
	XGarbageCollector::Instance()->RemoveInterfacePtr(this);
#endif
}


//---------------------------------------------------------------
//
// ZBaseInterfacePtr::ZBaseInterfacePtr ()
//
//---------------------------------------------------------------
ZBaseInterfacePtr::ZBaseInterfacePtr()
{		
	mBasePtr = nil;
	mPtr = nil;
	mAggregated = false;

#if GARBAGE_COLLECT_COM
	XGarbageCollector::Instance()->AddInterfacePtr(this);
#endif
}


//---------------------------------------------------------------
//
// ZBaseInterfacePtr::ZBaseInterfacePtr (XUnknown*)
//
//---------------------------------------------------------------
ZBaseInterfacePtr::ZBaseInterfacePtr(const XUnknown* unknown)
{
	mBasePtr = const_cast<XUnknown*>(unknown);
	mAggregated = true;					// $$$ we can't tell if we're aggregated so we'll err on the side of safety (note that this ctor is only used by XInterfacePtr<XUnknown>)

#if GARBAGE_COLLECT_COM
	XGarbageCollector::Instance()->AddInterfacePtr(this);
#else
	if (mBasePtr != nil)
		mBasePtr->DoAddReference();
#endif

	mPtr = mBasePtr;
}


//---------------------------------------------------------------
//
// ZBaseInterfacePtr::ZBaseInterfacePtr (XUnknown*, const char*)
//
//---------------------------------------------------------------
ZBaseInterfacePtr::ZBaseInterfacePtr(const XUnknown* unknown, const char* encodedName)
{	
	mBasePtr = nil;
	mAggregated = false;
	
	if (unknown != nil) {
		XBoss* boss = unknown->GetBoss();									
		
		XImplementation* implementation = boss->GetImplementation(encodedName);	// all these functions are constant time except for this which is O(log N)
		if (implementation != nil) {	
			if (implementation->IsAbstract())					// can't use an assert since we need to allow clients to tune REGISTER_INTERFACE_FACTORY for release builds
				throw std::logic_error(ToUTF8Str(LoadWhisperString(L"#1 hasn't been registered with REGISTER_INTERFACE_FACTORY", implementation->GetName())));
		
			mBasePtr = implementation->GetInterface(boss);		// bumps the ref count					 
			ASSERT(mBasePtr != nil);
			
			mAggregated = implementation->IsAggregated();
		}
	}

#if GARBAGE_COLLECT_COM
	XGarbageCollector::Instance()->AddInterfacePtr(this);
#endif

	mPtr = mBasePtr;
}


//---------------------------------------------------------------
//
// ZBaseInterfacePtr::ZBaseInterfacePtr (wstring)
//
//---------------------------------------------------------------
ZBaseInterfacePtr::ZBaseInterfacePtr(const std::wstring& bossName)
{
	XBoss* boss = XObjectModel::Instance()->CreateBoss(bossName);		// will be deleted when the last interface is released
		
	XImplementation* implementation = boss->GetImplementation();
	ASSERT(implementation != nil);
	
	mBasePtr = implementation->GetInterface(boss);		// bumps the ref count
	ASSERT(mBasePtr != nil);

	mAggregated = implementation->IsAggregated();

#if GARBAGE_COLLECT_COM
	XGarbageCollector::Instance()->AddInterfacePtr(this);
#endif

	mPtr = mBasePtr;
}


//---------------------------------------------------------------
//
// ZBaseInterfacePtr::ZBaseInterfacePtr (wstring, const char*)
//
//---------------------------------------------------------------
ZBaseInterfacePtr::ZBaseInterfacePtr(const std::wstring& bossName, const char* encodedName)
{
	mBasePtr = nil;
	mAggregated = false;

	XBoss* boss = XObjectModel::Instance()->CreateBoss(bossName);		// will be deleted when the last interface is released
		
	XImplementation* implementation = boss->GetImplementation(encodedName);
	if (implementation != nil) {
		if (implementation->IsAbstract())					// can't use an assert since we need to allow clients to tune REGISTER_INTERFACE_FACTORY for release builds
			throw std::logic_error(ToUTF8Str(LoadWhisperString(L"#1 hasn't been registered with REGISTER_INTERFACE_FACTORY", implementation->GetName())));
		
		mBasePtr = implementation->GetInterface(boss);		// bumps the ref count
		ASSERT(mBasePtr != nil);

		mAggregated = implementation->IsAggregated();
	}

#if GARBAGE_COLLECT_COM
	XGarbageCollector::Instance()->AddInterfacePtr(this);
#endif

	mPtr = mBasePtr;
}


//---------------------------------------------------------------
//
// ZBaseInterfacePtr::ZBaseInterfacePtr (const XBoss*, XBoss::const_iterator)
//
//---------------------------------------------------------------
ZBaseInterfacePtr::ZBaseInterfacePtr(const XBoss* inBoss, const XBoss::const_iterator& iter)
{	
	XBoss* boss = const_cast<XBoss*>(inBoss);
		
	XImplementation* implementation = const_cast<XImplementation*>(iter->GetImplementation());
	ASSERT(implementation != nil);
	
	mBasePtr = implementation->GetInterface(boss);		// bumps the ref count
	ASSERT(mBasePtr != nil);

	mAggregated = implementation->IsAggregated();

#if GARBAGE_COLLECT_COM
	XGarbageCollector::Instance()->AddInterfacePtr(this);
#endif

	mPtr = mBasePtr;
}


//---------------------------------------------------------------
//
// ZBaseInterfacePtr::ZBaseInterfacePtr (const XBoss*, XBoss::const_iterator, const char*)
//
//---------------------------------------------------------------
ZBaseInterfacePtr::ZBaseInterfacePtr(const XBoss* inBoss, const XBoss::const_iterator& iter, const char* encodedName)
{	
	mBasePtr = nil;
	mAggregated = false;

	XBoss* boss = const_cast<XBoss*>(inBoss);		
	
	if (*iter == encodedName) {
		XImplementation* implementation = const_cast<XImplementation*>(iter->GetImplementation());
		ASSERT(implementation != nil);
		
		if (implementation->IsAbstract())					// can't use an assert since we need to allow clients to tune REGISTER_INTERFACE_FACTORY for release builds
			throw std::logic_error(ToUTF8Str(LoadWhisperString(L"#1 hasn't been registered with REGISTER_INTERFACE_FACTORY", implementation->GetName())));
		
		mBasePtr = implementation->GetInterface(boss);		// bumps the ref count
		ASSERT(mBasePtr != nil);

		mAggregated = implementation->IsAggregated();
	}

#if GARBAGE_COLLECT_COM
	XGarbageCollector::Instance()->AddInterfacePtr(this);
#endif

	mPtr = mBasePtr;
}


//---------------------------------------------------------------
//
// ZBaseInterfacePtr::ZBaseInterfacePtr (const XBoss*, const XImplementation*, const char*)
//
//---------------------------------------------------------------
ZBaseInterfacePtr::ZBaseInterfacePtr(const XBoss* inBoss, const XImplementation* implementation, const char* encodedName)
{	
	UNUSED(encodedName);								// rely on the dynamic_cast
	
	mBasePtr = nil;
	mAggregated = false;

	XBoss* boss = const_cast<XBoss*>(inBoss);
			
	if (implementation->IsAggregated()) {
		mBasePtr = const_cast<XImplementation*>(implementation)->GetInterface(boss);		// bumps the ref count
		ASSERT(mBasePtr != nil);

		mAggregated = true;
	}

#if GARBAGE_COLLECT_COM
	XGarbageCollector::Instance()->AddInterfacePtr(this);
#endif

	mPtr = mBasePtr;
}


//---------------------------------------------------------------
//
// ZBaseInterfacePtr::ZBaseInterfacePtr (ZBaseInterfacePtr)
//
//---------------------------------------------------------------
ZBaseInterfacePtr::ZBaseInterfacePtr(const ZBaseInterfacePtr& rhs)
{
	mBasePtr = rhs.mBasePtr;
	mPtr = rhs.mPtr;
	mAggregated = rhs.mAggregated;

#if !GARBAGE_COLLECT_COM
	if (mBasePtr != nil)
		mBasePtr->DoAddReference();
	if (mPtr != nil)
		mPtr->DoAddReference();
#endif

#if GARBAGE_COLLECT_COM
	XGarbageCollector::Instance()->AddInterfacePtr(this);
#endif
}


//---------------------------------------------------------------
//
// ZBaseInterfacePtr::ZBaseInterfacePtr (ZBaseInterfacePtr, const char*)
//
//---------------------------------------------------------------
ZBaseInterfacePtr::ZBaseInterfacePtr(const ZBaseInterfacePtr& unknown, const char* encodedName)
{	
	mBasePtr = nil;
	mAggregated = false;
	
	if (unknown.mPtr != nil) {
		XBoss* boss = unknown.mPtr->GetBoss();									
		
		XImplementation* implementation = boss->GetImplementation(encodedName);	// all these functions are constant time except for this which is O(log N)
		if (implementation != nil) {	
			if (implementation->IsAbstract())					// can't use an assert since we need to allow clients to tune REGISTER_INTERFACE_FACTORY for release builds
				throw std::logic_error(ToUTF8Str(LoadWhisperString(L"#1 hasn't been registered with REGISTER_INTERFACE_FACTORY", implementation->GetName())));
		
			mBasePtr = implementation->GetInterface(boss);		// bumps the ref count						 
			ASSERT(mBasePtr != nil);
			
			mAggregated = implementation->IsAggregated();
		}
	}

#if GARBAGE_COLLECT_COM
	XGarbageCollector::Instance()->AddInterfacePtr(this);
#endif

	mPtr = mBasePtr;
}


//---------------------------------------------------------------
//
// ZBaseInterfacePtr::operator=
//
//---------------------------------------------------------------
ZBaseInterfacePtr& ZBaseInterfacePtr::operator=(const ZBaseInterfacePtr& rhs)
{
	if (mPtr != rhs.mPtr) {
		XUnknown* oldBase = mBasePtr;
		XUnknown* oldImp = mPtr;
		
#if !GARBAGE_COLLECT_COM
		if (mBasePtr != nil)
			mBasePtr->DoRemoveReference();
		if (mPtr != nil)
			mPtr->DoRemoveReference();
#endif
		
		mBasePtr = rhs.mBasePtr;
		mPtr = rhs.mPtr;
		mAggregated = rhs.mAggregated;
	
#if !GARBAGE_COLLECT_COM
		if (mBasePtr != nil)
			mBasePtr->DoAddReference();
		if (mPtr != nil)
			mPtr->DoAddReference();
#endif

#if GARBAGE_COLLECT_COM
		XGarbageCollector::Instance()->InterfacePtrChanged(this, oldImp);
#endif
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// ZBaseInterfacePtr::Release
//
//---------------------------------------------------------------
void ZBaseInterfacePtr::Release()
{
	if (mPtr != nil) {
		XUnknown* oldImp = mPtr;

#if !GARBAGE_COLLECT_COM
		mPtr->DoRemoveReference();
#endif
		
		mPtr = nil;

#if GARBAGE_COLLECT_COM
		XGarbageCollector::Instance()->InterfacePtrChanged(this, oldImp);
#endif
	}

	if (mBasePtr != nil) {
#if !GARBAGE_COLLECT_COM
		mBasePtr->DoRemoveReference();
#endif
		
		mBasePtr = nil;
	}
}


}	// namespace Whisper

