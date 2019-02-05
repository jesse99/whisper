/*
 *  File:       MPurgeable.cpp
 *  Summary:   	Mixin for objects that want to get purged from within the grow zone proc.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MPurgeable.cpp,v $
 *		Revision 1.5  2001/04/21 03:27:48  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/03/06 07:31:47  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.3  2001/03/02 11:22:49  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.2  2000/11/09 09:18:23  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <MPurgeable.h>

#include <iomanip>
#include <vector>

#include <MGrowZone.h>
#include <XAutoPtr.h>
#include <XCriticalSection.h>
#include <XDebug.h>
#include <XMiscUtils.h>

namespace Whisper {


// ===================================================================================
//	class ZPurgeableList
// ===================================================================================
class ZPurgeableList {

//-----------------------------------
//	Types
//
public:
	typedef std::vector<MPurgeableMixin*> 	PurgeableList;
	typedef PurgeableList::iterator			iterator;

//-----------------------------------
//	Initialization/Destruction
//
public:
						~ZPurgeableList();

						ZPurgeableList();
						
	static ZPurgeableList* Instance();

private:
						ZPurgeableList(const ZPurgeableList& rhs);
						
			ZPurgeableList& operator=(const ZPurgeableList& rhs);

//-----------------------------------
//	API
//
public:
	// ----- Adding and Removing -----
			void 		Add(MPurgeableMixin* object);
			
			void 		Remove(MPurgeableMixin* object);
			
	// ----- Iterating -----
			iterator 	begin();
						// Iterates over all the purgeable objects with the highest
						// priority objects returned first.
			
			iterator 	end()								{return mObjects.end();}
			
			void 		SetNeedsSorting()					{mNeedsSorting = true;}
	
//-----------------------------------
//	Internal API
//
protected:
	static	bool 		DoCompare(MPurgeableMixin* lhs, MPurgeableMixin* rhs);
						
//-----------------------------------
//	Member Data
//
public:
	static XCriticalSection			msListMutex;

protected:
	PurgeableList					mObjects;
	bool							mNeedsSorting;

private:
	static XAutoPtr<ZPurgeableList>	msInstance;			
	static XCriticalSection			msCreateMutex;
};

XCriticalSection 			ZPurgeableList::msListMutex;
XAutoPtr<ZPurgeableList>	ZPurgeableList::msInstance;		
XCriticalSection 			ZPurgeableList::msCreateMutex;

//---------------------------------------------------------------
//
// ZPurgeableList::~ZPurgeableList
//
//---------------------------------------------------------------
ZPurgeableList::~ZPurgeableList()
{
	PRECONDITION(mObjects.empty());
}


//---------------------------------------------------------------
//
// ZPurgeableList::ZPurgeableList
//
//---------------------------------------------------------------
ZPurgeableList::ZPurgeableList()
{
	mObjects.reserve(64);
	
	mNeedsSorting = false;			// nothing to sort...
	
	MGrowZone::Instance()->AddToBack(MPurgeableMixin::PurgeObjects);		// kind of lame to do this here...
}


//---------------------------------------------------------------
//
// ZPurgeableList::Instance								[static]
//
//---------------------------------------------------------------
ZPurgeableList* ZPurgeableList::Instance()
{
	if (msInstance.Get() == nil) {
		XEnterCriticalSection enter(msCreateMutex);

		if (msInstance.Get() == nil) 
			msInstance.Reset(new ZPurgeableList);
	}
	
	return msInstance.Get();
}


//---------------------------------------------------------------
//
// ZPurgeableList::Add
//
//---------------------------------------------------------------
void ZPurgeableList::Add(MPurgeableMixin* object)
{
	PRECONDITION(object != nil);

	XEnterCriticalSection enter(msListMutex);

#if DEBUG						// otherwise we'll get a compile error if ASSERTS_THROW is defined
	PRECONDITION(!gIntenseDebugging || std::find(mObjects.begin(), mObjects.end(), object) == mObjects.end());
#endif

	mObjects.push_back(object);
	mNeedsSorting = true;
}


//---------------------------------------------------------------
//
// ZPurgeableList::Remove
//
//---------------------------------------------------------------
void ZPurgeableList::Remove(MPurgeableMixin* object)
{
	PRECONDITION(object != nil);
	
	XEnterCriticalSection enter(msListMutex);

	iterator iter = std::find(mObjects.begin(), mObjects.end(), object);
	PRECONDITION(iter != mObjects.end());
	
	mObjects.erase(iter);
}


//---------------------------------------------------------------
//
// ZPurgeableList::begin
//
// $$ÊCalling sort is a bit dicy because we'll usually be inside
// $$Êthe grow zone proc. However MSL's implementation doesn't
// $$Êallocate any memory and if it ever changes our ASSERT in
// $$ÊWhisperOperatorNew will hopefully catch it.
//
//---------------------------------------------------------------
ZPurgeableList::iterator ZPurgeableList::begin()
{
	if (mNeedsSorting) {
		std::sort(mObjects.begin(), mObjects.end(), DoCompare);
		
		mNeedsSorting = false;
	}
	
	return mObjects.begin();
}

#pragma mark Ê

//---------------------------------------------------------------
//
// ZPurgeableList::DoCompare							[static]
//
// This is written to return true if the lhs's purge level is
// greater then the rhs's so that the objects are sorted into
// descending order.
//
//---------------------------------------------------------------
bool ZPurgeableList::DoCompare(MPurgeableMixin* lhs, MPurgeableMixin* rhs)
{
	SAFE_ASSERT(lhs != nil);
	SAFE_ASSERT(rhs != nil);
	
	int32 lhsLevel = lhs->GetPurgeLevel();
	int32 rhsLevel = rhs->GetPurgeLevel();
	
	bool greater = lhsLevel > rhsLevel;
	
	if (lhsLevel == rhsLevel)
		greater = lhs->GetTimeLastUsed() < rhs->GetTimeLastUsed();	// is rhs has been used recently it should be placed later in the list
	
	return greater;
}

#pragma mark -

// ===================================================================================
//	class MDisablePurging
// ===================================================================================

//---------------------------------------------------------------
//
// MDisablePurging::~MDisablePurging
//
//---------------------------------------------------------------
MDisablePurging::~MDisablePurging()
{
	mObject->mDisablePurging--;
}


//---------------------------------------------------------------
//
// MDisablePurging::MDisablePurging
//
//---------------------------------------------------------------
MDisablePurging::MDisablePurging(MPurgeableMixin* object)
{
	PRECONDITION(object != nil);
	
	mObject = object;
	mObject->mDisablePurging++;
}

#pragma mark -

// ===================================================================================
//	class MPurgeableMixin
// ===================================================================================

#if DEBUG
	uint32				MPurgeableMixin::msPurgeCount = 0;
	MPurgeableMixin*	MPurgeableMixin::msPurgedObjects[kMaxPurgeCount];
	uint32				MPurgeableMixin::msPurgedBytes[kMaxPurgeCount];
	XCriticalSection	MPurgeableMixin::msStatisticsSection;
#endif

//---------------------------------------------------------------
//
// MPurgeableMixin::~MPurgeableMixin
//
//---------------------------------------------------------------
MPurgeableMixin::~MPurgeableMixin()
{
#if DEBUG
	MPurgeableMixin::DumpPurgedObjects(this);
#endif

	ZPurgeableList::Instance()->Remove(this);
}


//---------------------------------------------------------------
//
// MPurgeableMixin::MPurgeableMixin (int32, bool)
//
//---------------------------------------------------------------
MPurgeableMixin::MPurgeableMixin(int32 purgeLevel, bool purged)
{
	PRECONDITION(purgeLevel >= kDontPurge);
	PRECONDITION(!MGrowZone::Instance()->IsExecuting());
	
	mPurgeLevel     = purgeLevel;
	mDisablePurging = 0;
	mPurged         = purged;

	ZPurgeableList::Instance()->Add(this);

	this->Touch();
}


//---------------------------------------------------------------
//
// MPurgeableMixin::MPurgeableMixin (MPurgeableMixin)
//
//---------------------------------------------------------------
MPurgeableMixin::MPurgeableMixin(const MPurgeableMixin& rhs)
{
	PRECONDITION(!MGrowZone::Instance()->IsExecuting());

	mPurgeLevel     = rhs.mPurgeLevel;
	mDisablePurging = 0;
	mPurged         = rhs.mPurged;

	ZPurgeableList::Instance()->Add(this);

	this->Touch();
}


//---------------------------------------------------------------
//
// MPurgeableMixin::operator=
//
//---------------------------------------------------------------
MPurgeableMixin& MPurgeableMixin::operator=(const MPurgeableMixin& rhs)
{
	if (this != &rhs) {
		mPurgeLevel = rhs.mPurgeLevel;
		mPurged     = rhs.mPurged;
	
		ZPurgeableList::Instance()->SetNeedsSorting();
	}
	
	this->Touch();

	return *this;
}


//---------------------------------------------------------------
//
// MPurgeableMixin::SetPurgeLevel
//
//---------------------------------------------------------------
void MPurgeableMixin::SetPurgeLevel(int32 level)
{
	PRECONDITION(level >= kDontPurge);
	
	if (level != mPurgeLevel) {
		if (level < mPurgeLevel)
			this->Touch();
			
		mPurgeLevel = level;	

		ZPurgeableList::Instance()->SetNeedsSorting();
	}
}


//---------------------------------------------------------------
//
// MPurgeableMixin::HandlePurge
//
//---------------------------------------------------------------
uint32 MPurgeableMixin::HandlePurge(Handle gzHandle)
{
	PRECONDITION(mDisablePurging >= 0);
	
	uint32 bytes = 0;
		
	if (this->IsPurgeable(gzHandle)) {
		bytes = this->OnGetPurgeBytes();
		ASSERT(bytes > 0);

		this->OnPurge();
		
		mPurged = true;
	}
	
	return bytes;
}


//---------------------------------------------------------------
//
// MPurgeableMixin::HandleLoad
//
//---------------------------------------------------------------
void MPurgeableMixin::HandleLoad() const
{
	MPurgeableMixin* thisPtr = const_cast<MPurgeableMixin*>(this);
	
	if (!this->IsLoaded()) {
		MDisablePurging disable(thisPtr);	// don't want to get purged while we're loading!
		
		thisPtr->OnLoad();
		
		thisPtr->mPurged = false;
	}
	
	thisPtr->Touch();
}


//---------------------------------------------------------------
//
// MPurgeableMixin::Touch
//
//---------------------------------------------------------------
void MPurgeableMixin::Touch()
{
	mTimeLastUsed = GetMilliSeconds();

	ZPurgeableList::Instance()->SetNeedsSorting();

#if DEBUG
	MPurgeableMixin::DumpPurgedObjects(nil);
#endif
}


//---------------------------------------------------------------
//
// MPurgeableMixin::GetPurgeableBytes					[static]
//
//---------------------------------------------------------------
uint32 MPurgeableMixin::GetPurgeableBytes()
{
	uint32 bytes = 0;
		
	XEnterCriticalSection enter(ZPurgeableList::msListMutex);

	ZPurgeableList::iterator iter = ZPurgeableList::Instance()->begin();
	while (iter != ZPurgeableList::Instance()->end()) {	
		const MPurgeableMixin* object = *iter;
		++iter;
		
		if (object->IsPurgeable())
			bytes += object->OnGetPurgeBytes();
	}
	
	return bytes;
}


//---------------------------------------------------------------
//
// MPurgeableMixin::DumpPurgedObjects					[static]
//
// It's not safe to print this stuff from the grow zone proc so
// we'll do it here.
//
//---------------------------------------------------------------
#if DEBUG
void MPurgeableMixin::DumpPurgedObjects(MPurgeableMixin* badObject)
{
	XEnterCriticalSection enter(msStatisticsSection);
	
	if (msPurgeCount > 0) {
		uint32 totalBytes = 0;
		
		TRACEFLOW("XPurging", "Purged objects:\n");
	
		for (uint32 index = 0; index < msPurgeCount; ++index) {	
			MPurgeableMixin* object = msPurgedObjects[index];
			
			if (object != badObject) {						// if badObject isn't nil then it's partially destroyed and calling OnGetName could destroy the cosmos
				std::wstring name = object->OnGetName();
				uint32 bytes     = msPurgedBytes[index];
				
				totalBytes += bytes;
				
				TRACEFLOW("XPurging", "   '", name, "' for ", ToStr(bytes/1024.0, 1), "K bytes\n");
			}
		}
		
		if (msPurgeCount == kMaxPurgeCount)
			TRACEFLOW("XPurging", "   ...overflowed history buffer...\n");
		
		if (msPurgeCount > 1)
			TRACEFLOW("XPurging", "Num Objects = ", msPurgeCount, ". Total bytes = ", ToStr(totalBytes/1024.0, 1), "K\n\n");
 
		msPurgeCount = 0;
	}
}
#endif	// DEBUG


//---------------------------------------------------------------
//
// MPurgeableMixin::PurgeObjects						[static]
//
//---------------------------------------------------------------
uint32 MPurgeableMixin::PurgeObjects(uint32 needed, Handle gzHandle)
{
	uint32 purged = 0;
	
	bool done = false;
	
	XEnterCriticalSection enter(ZPurgeableList::msListMutex);

	ZPurgeableList::iterator iter = ZPurgeableList::Instance()->begin();
	while (iter != ZPurgeableList::Instance()->end() && !done) {	// note that MGrowZone will have added some padding onto needed
		MPurgeableMixin* object = *iter;
		++iter;
		
		uint32 bytes = object->HandlePurge(gzHandle);
		purged += bytes;

#if DEBUG
		XEnterCriticalSection enter(msStatisticsSection);
	
		if (bytes > 0 && msPurgeCount < kMaxPurgeCount) {
			msPurgedObjects[msPurgeCount] = object;
			msPurgedBytes[msPurgeCount]   = bytes;
			
			++msPurgeCount;
		}
#endif
			
		done = object->GetPurgeLevel() <= kDontPurge || purged >= needed;
	}
	
	return purged;
}

#pragma mark Ê

//---------------------------------------------------------------
//
// MPurgeableMixin::OnCanPurge
//
//---------------------------------------------------------------
bool MPurgeableMixin::OnCanPurge(Handle gzHandle) const
{
	#pragma unused(gzHandle)
	
	bool purgeable = false;
	
	if (mPurgeLevel > kDontPurge)
		if (!mPurged)
			if (mDisablePurging == 0)
				purgeable = true;
	
	return purgeable;
}


}	// namespace Whisper
