/*
 *  File:       XGarbageCollector.cpp
 *  Summary:    Mark and sweep collector that deletes unused bosses.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XGarbageCollector3.cpp,v $
 *		Revision 1.3  2001/04/27 04:32:55  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:37:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XGarbageCollector.h>

#include <XInterfacePtr.h>

namespace Whisper {
#if GARBAGE_COLLECT_COM


// ===================================================================================
//	class XGarbageCollector
// ===================================================================================

XAutoPtr<XGarbageCollector>	XGarbageCollector::msInstance;			
XCriticalSection			XGarbageCollector::msMutex;

//---------------------------------------------------------------
//
// XGarbageCollector::~XGarbageCollector
//
//---------------------------------------------------------------
XGarbageCollector::~XGarbageCollector()
{
	PRECONDITION(mInterfaces.size() == 0);
	PRECONDITION(mBosses.size() == 0);
}


//---------------------------------------------------------------
//
// XGarbageCollector::XGarbageCollector
//
//---------------------------------------------------------------
XGarbageCollector::XGarbageCollector()
{
	mInterfaces.reserve(128);
	
	mGeneration = 0;

	mStackBosses = mBosses.insert(nil).first;		// for interfaces referenced by XInterfacePtr's on the stack
}


//---------------------------------------------------------------
//
// XGarbageCollector::Instance							[static]
//
//---------------------------------------------------------------
XGarbageCollector* XGarbageCollector::Instance()
{
	if (msInstance.Get() == nil) {
		XEnterCriticalSection enter(msMutex);

		if (msInstance.Get() == nil) 
			msInstance.Reset(new XGarbageCollector);
	}
	
	return msInstance.Get();
}


//---------------------------------------------------------------
//
// XGarbageCollector::AddInterfacePtr
//
//---------------------------------------------------------------
void XGarbageCollector::AddInterfacePtr(ZBaseInterfacePtr* ptr)
{
	PRECONDITION(ptr != nil);
	
	XEnterCriticalSection enter(msMutex);

	Interfaces::iterator iter = std::lower_bound(mInterfaces.begin(), mInterfaces.end(), ptr);
	ASSERT(iter == mInterfaces.end() || iter->ptr != ptr);
	
	mInterfaces.insert(iter, ptr);

	if (ptr->GetPtr() != nil)
		mStackBosses->ptrs.insert(ptr->GetPtr());	// assume we're stack based until told otherwise
}


//---------------------------------------------------------------
//
// XGarbageCollector::RemoveInterfacePtr
//
//---------------------------------------------------------------
void XGarbageCollector::RemoveInterfacePtr(ZBaseInterfacePtr* ptr)
{
	PRECONDITION(ptr != nil);
	
	XEnterCriticalSection enter(msMutex);

	Interfaces::iterator iter = std::lower_bound(mInterfaces.begin(), mInterfaces.end(), ptr);
	ASSERT(iter != mInterfaces.end() && iter->ptr == ptr);

	if (ptr->GetPtr() != nil)
		if (iter->boss == nil)		// if we're embedded in a boss it must be in the process of being deleted so the SBoss entry has already been deleted	
			mStackBosses->ptrs.erase(ptr->GetPtr());
		else
			ASSERT(mBosses.find(iter->boss) == mBosses.end());
	
	mInterfaces.erase(iter);
}


//---------------------------------------------------------------
//
// XGarbageCollector::ConstructedInterface
//
//---------------------------------------------------------------
void XGarbageCollector::ConstructedInterface(XBoss* boss, const XUnknown* imp, uint32 bytes)
{
	PRECONDITION(boss != nil);
	PRECONDITION(imp != nil);
	PRECONDITION(bytes >= sizeof(XUnknown));	
	
	const uint8* impBegin = (uint8*) imp;
	const uint8* impEnd   = impBegin + bytes;			

	XEnterCriticalSection enter(msMutex);

	Bosses::iterator iter2 = mBosses.find(boss);
	if (iter2 == mBosses.end())
		iter2 = mBosses.insert(boss).first;
		
	Interfaces::iterator iter = std::lower_bound(mInterfaces.begin(), mInterfaces.end(), impBegin);
	while (iter != mInterfaces.end() && *iter < impEnd) {	// iterate over all the XInterfacePtr's embedded in the implementation class
		ASSERT(iter->boss == nil);
		iter->boss = boss;

		if (iter->ptr->GetPtr() != nil) {
			mStackBosses->ptrs.erase(iter->ptr->GetPtr());
			iter2->ptrs.insert(iter->ptr->GetPtr());
		}
		
		++iter;
	}
}


//---------------------------------------------------------------
//
// XGarbageCollector::AssignInterface
//
//---------------------------------------------------------------
void XGarbageCollector::AssignInterface(XBoss* boss, const XUnknown* imp)
{
	PRECONDITION(boss != nil);

	if (imp != nil) {	
		XEnterCriticalSection enter(msMutex);
	
		Bosses::iterator iter2 = mBosses.find(boss);
		if (iter2 == mBosses.end())
			iter2 = mBosses.insert(boss).first;

		mStackBosses->ptrs.erase(imp);	// $$$ RemoveInterfacePtr will do another remove...
		iter2->ptrs.insert(imp);
	}
}


//---------------------------------------------------------------
//
// XGarbageCollector::RemoveInterface
//
//---------------------------------------------------------------
void XGarbageCollector::RemoveInterface(XBoss* boss, const XUnknown* imp)
{
	PRECONDITION(boss != nil);

	if (imp != nil) {	
		XEnterCriticalSection enter(msMutex);
	
		Bosses::iterator iter = mBosses.find(boss);
		ASSERT(iter != mBosses.end());
	
		std::multiset<const XUnknown*>::iterator iter2 = iter->ptrs.find(imp);
		ASSERT(iter2 != iter->ptrs.end());
		iter->ptrs.erase(iter2);

		mStackBosses->ptrs.insert(imp);
	}
}


//---------------------------------------------------------------
//
// XGarbageCollector::InterfacePtrChanged
//
//---------------------------------------------------------------
void XGarbageCollector::InterfacePtrChanged(const ZBaseInterfacePtr* ptr, const XUnknown* oldImp)
{
	PRECONDITION(ptr != nil);

	XEnterCriticalSection enter(msMutex);
	
	Interfaces::iterator iter = std::lower_bound(mInterfaces.begin(), mInterfaces.end(), ptr);
	ASSERT(iter != mInterfaces.end() && *iter == ptr);

	Bosses::iterator iter2 = mBosses.find(iter->boss);
	ASSERT(iter2 != mBosses.end());

	if (oldImp != nil) {
		std::multiset<const XUnknown*>::iterator iter3 = iter2->ptrs.find(oldImp);
		ASSERT(iter3 != iter2->ptrs.end());

		iter2->ptrs.erase(iter3);
	}

	const XUnknown* newImp = ptr->GetPtr();
	if (newImp != nil)
		iter2->ptrs.insert(newImp);
}


//---------------------------------------------------------------
//
// XGarbageCollector::Collect
//
//---------------------------------------------------------------
void XGarbageCollector::Collect(bool shuttingDown)
{
	XEnterCriticalSection enter(msMutex);

	mGeneration = std::max(mGeneration+1, 1UL);
	
	uint32 numBosses = mBosses.size();
	uint32 numInterfaces = mInterfaces.size();
	
	TRACEFLOW("XGarbage", "Running garbage collector (generation ", mGeneration, ")\n");
	TRACEFLOW("XGarbage", "   number of bosses = ", numBosses, "\n");
	TRACEFLOW("XGarbage", "   number of interface ptrs = ", numInterfaces, "\n\n");

	// Mark all bosses that are referenced directly or indirectly 
	// by singleton bosses and XInterfacePtr's on the stack.
	Bosses::iterator iter = mBosses.begin();
	while (iter != mBosses.end()) {
		const SBoss& boss = *iter;
		++iter;
		
		if (boss.boss == nil || (boss.boss->IsSingleton() && !shuttingDown)) {
			if (boss.ptrs.size() > 0) {
				if (boss.generation != mGeneration) {	
					if (boss.boss == nil)
						TRACEFLOW("XGarbage", "   marking stack based bosses\n");
					else
						TRACEFLOW("XGarbage", "   marking bosses referenced by '", boss.boss->GetName(), "'\n");
				}

				this->DoMarkBoss(boss);						
			}
		}
	}
	
	TRACEFLOW("XGarbage", "\n");
	
	// Delete any unmarked bosses.
	Bosses::iterator iter2 = mBosses.begin();
	while (iter2 != mBosses.end()) {
		XBoss* boss = iter2->boss;
		Bosses::iterator temp = iter2;
		++iter2;
		
		if (boss != nil && !boss->IsSingleton()) {			// object model will delete singletons
			if (temp->generation != mGeneration) {
				mBosses.erase(temp);

				TRACEFLOW("XGarbage", "   deleting '", boss->GetName(), "'\n");
				delete boss;
			}
		}
	}
		
	TRACEFLOW("XGarbage", "   deleted ", numBosses - mBosses.size(), " bosses\n");
	TRACEFLOW("XGarbage", "   deleted ", numInterfaces - mInterfaces.size(), " interface ptrs\n\n");
}


//---------------------------------------------------------------
//
// XGarbageCollector::DoMarkBoss
//
//---------------------------------------------------------------
void XGarbageCollector::DoMarkBoss(const SBoss& boss)
{
	if (boss.generation != mGeneration) {		// dependecies can be cyclic so we need to check to see if we've already marked the boss
		if (boss.boss != nil)
			TRACEFLOW("XGarbage", "      '", boss.boss->GetName(), "'\n");
	
		boss.generation = mGeneration;

		std::multiset<const XUnknown*>::iterator iter = boss.ptrs.begin();
		while (iter != boss.ptrs.end()) {
			const XUnknown* ptr = *iter;	
			ASSERT(ptr != nil);
			++iter;
			
			Bosses::iterator iter2 = mBosses.find(ptr->GetBoss());
			ASSERT(iter2 != mBosses.end());
			
			this->DoMarkBoss(*iter2);						
		}
	}
}


#endif	// GARBAGE_COLLECT_COM
}		// namespace Whisper

