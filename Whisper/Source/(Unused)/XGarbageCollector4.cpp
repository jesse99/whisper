/*
 *  File:       XGarbageCollector.cpp
 *  Summary:    Mark and sweep collector that deletes unused bosses.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XGarbageCollector4.cpp,v $
 *		Revision 1.3  2001/04/27 04:32:59  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:37:37  jesjones
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
	
	(void) mBosses.insert(nil);			// for interfaces referenced by XInterfacePtr's on the stack
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
	PRECONDITION(iter == mInterfaces.end() || iter->ptr != ptr);
	
	mInterfaces.insert(iter, ptr);
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
	PRECONDITION(iter != mInterfaces.end() && *iter == ptr);
	
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
	while (iter != mInterfaces.end() && *iter < impEnd) {	// iterate over all XInterfacePtr's inside the implementation object
		ASSERT(iter->boss == nil);
		iter->boss = boss;

		++iter;
	}
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

	// Figure out which interfaces each boss references.
	Interfaces::iterator iter = mInterfaces.begin();
	while (iter != mInterfaces.end()) {
		const SInterface& interface = *iter;
		++iter;
		
		if (interface.ptr->GetPtr() != nil) {
			XBoss* owner = interface.boss;
			Bosses::iterator iter2 = mBosses.find(owner);
			ASSERT(iter2 != mBosses.end());
			
			iter2->ptrs.push_back(interface.ptr->GetPtr());		// may have duplicates, but that's OK
		}
	}
	
	// Mark all bosses that are referenced directly or indirectly 
	// by singleton bosses and XInterfacePtr's on the stack.
	Bosses::iterator iter2 = mBosses.begin();
	while (iter2 != mBosses.end()) {
		const SBoss& boss = *iter2;
		++iter2;
		
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
		
		boss.ptrs.clear();
	}
	
	TRACEFLOW("XGarbage", "\n");
	
	// Delete any unmarked bosses.
	iter2 = mBosses.begin();
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
		TRACEFLOW("XGarbage", "      '", boss.boss->GetName(), "'\n");
		boss.generation = mGeneration;

		std::vector<const XUnknown*>::iterator iter = boss.ptrs.begin();
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

