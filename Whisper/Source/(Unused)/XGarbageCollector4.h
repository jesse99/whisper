/*
 *  File:       XGarbageCollector.h
 *  Summary:    Mark and sweep collector that deletes unused bosses.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XGarbageCollector4.h,v $
 *		Revision 1.3  2001/03/03 11:15:55  jesjones
 *		Instead of specializing selected algorithms for POD types, we use the new MSL mechanism to declare POD types.
 *		
 *		Revision 1.2  2000/11/09 12:37:42  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XAutoPtr.h>
#include <XGeneric.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif

#if GARBAGE_COLLECT_COM


//-----------------------------------
//	Forward References
//
class ZBaseInterfacePtr;


// ===================================================================================
//	class XGarbageCollector
// ===================================================================================
class COM_EXPORT XGarbageCollector {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XGarbageCollector();

						XGarbageCollector();
						
	static XGarbageCollector* Instance();

private:
						XGarbageCollector(const XGarbageCollector& rhs);
			
			XGarbageCollector& operator=(const XGarbageCollector& rhs);
			
//-----------------------------------
//	API
//
public:
			void 		AddInterfacePtr(ZBaseInterfacePtr* ptr);
			void 		RemoveInterfacePtr(ZBaseInterfacePtr* ptr);
						// Called by the ZBaseInterfacePtr ctors and the dtor.
						
			void 		ConstructedInterface(XBoss* boss, const XUnknown* imp, uint32 bytes);
						// Called as the boss instantiates interfaces (this is used to
						// determine which boss XInterfacePtr's are embedded in).
						
			void 		Collect(bool shuttingDown = false);
						// Marks all bosses that are referenced by singleton bosses and
						// XInterfacePtr's on the stack and deletes the unmarked bosses.
						// Unlike reference counting schemes this will transparently handle
						// unused bosses that point to each other.
			
//-----------------------------------
//	Internal Types
//
public:
	struct SInterface {
		XBoss*						boss;					// will be nil for stack based interface ptrs
		const ZBaseInterfacePtr*	ptr;
		
					SInterface(ZBaseInterfacePtr* p) : boss(nil), ptr(p)	{}
		
			bool 	operator==(const SInterface& rhs) const		{return ptr == rhs.ptr;}
			bool 	operator<(const SInterface& rhs) const		{return ptr < rhs.ptr;}
		
			bool 	operator==(const void* rhs) const			{return ptr == rhs;}
			bool 	operator<(const void* rhs) const			{return ptr < rhs;}
	};
	
	struct SBoss {
		XBoss*									boss;
		mutable uint32							generation;
		mutable std::vector<const XUnknown*>	ptrs;		// interfaces the boss references (usually via XInterfacePtr's as member data of one of the boss's interfaces)
		
					SBoss(XBoss* b) : boss(b), generation(0)	{}
		
			bool 	operator==(const SBoss& rhs) const			{return boss == rhs.boss;}
			bool 	operator<(const SBoss& rhs) const			{return boss < rhs.boss;}
		
			bool 	operator==(const XBoss* rhs) const			{return boss == rhs;}
			bool 	operator<(const XBoss* rhs) const			{return boss < rhs;}
	};
	
	typedef std::vector<SInterface>	Interfaces;				// note that we can't use a set because ConstructedInterface needs to search for pointers that aren't exact matches
	typedef std::set<SBoss> Bosses;

//-----------------------------------
//	Internal API
//
protected:
			void 		DoMarkBoss(const SBoss& boss);
	
//-----------------------------------
//	Member Data
//
protected:
	Interfaces							mInterfaces;
	Bosses								mBosses;
	
	uint32								mGeneration;

	static XAutoPtr<XGarbageCollector>	msInstance;			
	static XCriticalSection				msMutex;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

DECLARE_POD(Whisper::XGarbageCollector::SInterface)

#endif	// GARBAGE_COLLECT_COM