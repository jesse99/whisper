/*
 *  File:       MTimeMgrTimer.h
 *  Summary:   	Timer that gets time via a Time manager task
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MTimeMgrTimer.h,v $
 *		Revision 1.3  2000/12/10 08:49:21  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:21:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: MTimeMgrTimer.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/10 08:49:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Converted header comments to Doxygen format.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <2>	 2/02/00	JDJ		No longer descends from MExitActionMixIn (this is only
 *									needed for OS 7 and is a bit sticky to implement in a
 *									multi-fragment app).
 *		 <1>	 9/20/98	JDJ		Created (from Raven)
 */

#pragma once

#include <Timer.h>

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class MTimeMgrTimerMixIn
//!		Timer that gets time via a Time manager task
// ===================================================================================
class CORE_EXPORT MTimeMgrTimerMixIn {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual		   		~MTimeMgrTimerMixIn();
						
	explicit			MTimeMgrTimerMixIn(MilliSecond freq, bool running = false);
						/**< Frequency is the interval at which the timer wants to be called. */
					
private:
						MTimeMgrTimerMixIn(const MTimeMgrTimerMixIn& rhs);
					
			MTimeMgrTimerMixIn& operator=(const MTimeMgrTimerMixIn& rhs);

//-----------------------------------
//	API
//
public:
	// ----- Running -----
	virtual void		StartTimer();
						/**< Note that these names are a bit clumsy to minimize the chance
						of a name conflict when the timer is mixed into another class. */
	
	virtual void		StopTimer();
						
			bool		TimerIsRunning() const				{return mTimerIsRunning;}

	// ----- Frequency -----
			MilliSecond	GetTimerFreq() const				{return mTimerFreq;}

	virtual	void		SetTimerFreq(MilliSecond freq);

protected:
	virtual	void		OnTime() = 0;
						/**< Note that this function is called at interrupt time! */

//-----------------------------------
//	Internal API
//
protected:
			void 		DoInstallTimer();
			
			void 		DoRemoveTimer();

	static pascal void 	DoInterrupt(TMTask* taskPtr);

//-----------------------------------
//	Internal Types
//
protected:
 	struct STimeMgrRecord {		
		TMTask				task;
		int32				mMyA5;
		MTimeMgrTimerMixIn*	thisPtr;
		bool				inTask;
 	};

//-----------------------------------
//	Member Data
//
protected:
	bool			mTimerIsRunning;		
	MilliSecond		mTimerFreq;		

	STimeMgrRecord	mTimeMgrInfo;				// Data accessed from within the time manager task.		
 	bool			mTimerInstalled;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
