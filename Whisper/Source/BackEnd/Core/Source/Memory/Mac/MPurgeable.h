/*
 *  File:       MPurgeable.h
 *  Summary:   	Mixin for objects that want to get purged from within the grow zone proc.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MPurgeable.h,v $
 *		Revision 1.4  2001/03/05 05:39:36  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:49:01  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:18:29  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XCriticalSection.h>
#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class MPurgeableMixin;


//-----------------------------------
//	Constants
//
const int32 kDontPurge 			= 0;
const int32 kPurgeRarely		= 100;
const int32 kPurgeSeldom		= 200;
const int32 kNormalPurgeLevel	= 500;		
const int32 kPurgeFrequently	= 700;
const int32 kPurgeImmediately	= 1000;

const bool kStartsPurged = true;
const bool kStartsLoaded = false;


// ===================================================================================
//	class MDisablePurging
//!		Stack based class that will prevent object from being purged while it's alive. Note that these can be nested.
// ===================================================================================
class CORE_EXPORT MDisablePurging {

public:
						~MDisablePurging();

						MDisablePurging(MPurgeableMixin* object);
	
protected:
	MPurgeableMixin*	mObject;
};


// ===================================================================================
//	class MPurgeableMixin
//!		Mixin for objects that want to get purged from within the grow zone proc.
// ===================================================================================
class CORE_EXPORT MPurgeableMixin {

	friend class CORE_EXPORT MDisablePurging;
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~MPurgeableMixin();

						MPurgeableMixin(int32 purgeLevel, bool startsPurged);
						
						MPurgeableMixin(const MPurgeableMixin& rhs);
						
			MPurgeableMixin& operator=(const MPurgeableMixin& rhs);
						
//-----------------------------------
//	API
//
public:
	//! @name Purge Levels
	//@{
	virtual	bool 		IsPurgeable(Handle gzHandle = nil) const	{return this->OnCanPurge(gzHandle);}
			
			int32 		GetPurgeLevel() const						{return mPurgeLevel;}
						/**< Objects with larger purge levels are purged first. */
			
	virtual void 		SetPurgeLevel(int32 level);
						/**< When you have lot's of purgeable objects it's generally
						best if most of them have kNormalPurgeLevel. Otherwise
						objects with relatively low purge priorities will never
						get purged even if it's been a very long time since they
						were last used. */
	//@}
	
	//! @name Purging
	//@{
			bool 		IsLoaded() const							{return !mPurged;}

	virtual uint32 		HandlePurge(Handle gzHandle);
						/**< This will be called by PurgeObjects: it should rarely
						be called elsewhere. */
						
	virtual void 		HandleLoad() const;
						/**< Call this to ensure that the object is loaded and ready
						for use. (This is const because it essentially affects
						only the internal state of the object). */
	//@}
	
	//! @name Misc
	//@{
			void 		Touch();
						/**< Call this when the object is used. (Objects that haven't
						been touched recently are purged first). */

			MilliSecond GetTimeLastUsed() const						{return mTimeLastUsed;}
			
	static	uint32 		PurgeObjects(uint32 needed, Handle gzHandle);
						/**< Called by MGrowZone. */
						
	static	uint32 		GetPurgeableBytes();
						/**< Returns the (approximate) amount of memory that would be 
						freed if all purgeable objects were purged. */
#if DEBUG
	static 	void 		DumpPurgedObjects(MPurgeableMixin* badObject);
						/**< Prints any objects that have been recently purged using
						OnGetName. You shouldn't normally have to call this. */
#endif
	//@}

//-----------------------------------
//	Internal API
//
protected:
	virtual uint32 		OnGetPurgeBytes() const = 0;
						/**< Returns the amount of memory freed (this can be an approximate 
						value). This is called from the grow zone so it has the same 
						restrictions as OnPurge. */

	virtual void 		OnPurge() = 0;
						/**< Override and free up some memory. Note that you have to very careful 
						about what you call from this function. In particular you can't make 
						synchronous File Manager calls and the only legal Memory Manager calls 
						are SetHandleSize/SetPtrSize (shrinking), DisposeHandle/DisposePtr, 
						EmptyHandle, HPurge, HSetState, and UnloadSeg (this is according to TN 
						1042). */
						
	virtual void 		OnLoad() = 0;
						/**< Note that purging for 'this' is disabled while HandleLoad
						executes. */
	
	virtual bool 		OnCanPurge(Handle gzHandle) const;
						/**< If you delete handles from within OnPurge you should override
						this and return false if gzHandle matches one of your handles. */
						
#if DEBUG
	virtual std::wstring OnGetName() const = 0;
						/**< Override and return an intelligble name (see TCachedView for
						an example). */
#endif
						
//-----------------------------------
//	Internal Constants
//
protected:
	enum {kMaxPurgeCount = 64};
	
//-----------------------------------
//	Member Data
//
protected:
	MilliSecond				mTimeLastUsed;
	int32					mPurgeLevel;
	int32					mDisablePurging;
	bool					mPurged;
	
#if DEBUG
	static uint32			msPurgeCount;
	static MPurgeableMixin*	msPurgedObjects[kMaxPurgeCount];
	static uint32			msPurgedBytes[kMaxPurgeCount];
	static XCriticalSection	msStatisticsSection;
#endif
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
