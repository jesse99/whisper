/*
 *  File:       WTimer.cpp
 *  Summary:   	ITimer implementation for Windows.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WTimer.cpp,v $
 *		Revision 1.1  2001/04/27 09:25:59  jesjones
 *		Checked in
 *		
 */

#include <XWhisperHeader.h>
#include <ITimer.h>

#include <list>

#include <XExceptions.h>
#include <XInterfaceMacros.h>
#include <XMiscUtils.h>
#include <XObjectModel.h>

namespace Whisper {


// ===================================================================================
//	class XTimer
//!		ITimer implementation for Windows.
// ===================================================================================
class XTimer : public ITimer {	

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XTimer();
	
						XTimer(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		AddTimer(XCallback0<void> callback, MilliSecond interval);
	virtual void 		AddTimer(XCallback0<void> callback, MilliSecond interval, MilliSecond delay);
	virtual void 		AddOneShotTimer(XCallback0<void> callback, MilliSecond delay);

	virtual void 		SetInterval(XCallback0<void> callback, MilliSecond interval);
	virtual void 		SetDelay(XCallback0<void> callback, MilliSecond delay);

	virtual void 		RemoveTimer(XCallback0<void> callback);
	
	virtual void 		Tickle();

//-----------------------------------
//	Internal API
//
private:	
			void 		DoUpdate();

//-----------------------------------
//	Internal Types
//
private:	
	struct SEntry {
		XCallback0<void>	callback;
		MilliSecond			interval;
		MilliSecond			callTime;		// next time at which we should call the callback
		
					SEntry()	{}
					SEntry(XCallback0<void> c, MilliSecond i, MilliSecond d) : callback(c), interval(i), callTime(GetMilliSeconds() + d) {}
					
			bool 	operator==(const SEntry& rhs) const				{return callTime == rhs.callTime;}
			bool 	operator<(const SEntry& rhs) const				{return callTime < rhs.callTime;}

			bool 	operator==(const XCallback0<void>& rhs) const	{return callback == rhs;}
	};

	typedef std::list<SEntry> Callbacks;		// use a list so clients can remove their timers within the callback

//-----------------------------------
//	Member Data
//
private:
	bool			mDirty;
	Callbacks		mCallbacks;	
	MilliSecond		mMinDelay;
	
	unsigned int	mTimerID;
};

DEFINE_INTERFACE_FACTORY(XTimer)

//---------------------------------------------------------------
//
// XTimer::~XTimer
//
//---------------------------------------------------------------
XTimer::~XTimer()
{
	PRECONDITION(mCallbacks.empty());
	
	if (mTimerID != nil)
		VERIFY(::KillTimer(nil, mTimerID));
}


//---------------------------------------------------------------
//
// XTimer::XTimer
//
//---------------------------------------------------------------
XTimer::XTimer(XBoss* boss) 
{	
	this->DoSetBoss(boss);
	
	mMinDelay = 10*1000;
	mDirty = true;
	
	mTimerID = nil;
}


//---------------------------------------------------------------
//
// XTimer::Tickle
//
//---------------------------------------------------------------
void XTimer::Tickle()
{
	if (!mCallbacks.empty()) {
		MilliSecond time = GetMilliSeconds();
		
		if (mDirty)
			mCallbacks.sort();
		
		Callbacks::iterator iter = mCallbacks.begin();
		while (iter != mCallbacks.end() && iter->callTime < time) {	// "< time" to be safe with one shot timers
			Callbacks::iterator temp = iter;						// use a temporary in case the timer calls RemoveTimer
			++temp;

			SEntry& entry = *iter;
			entry.callTime = entry.interval == LONG_MAX ? LONG_MAX : (time + entry.interval);
			entry.callback();

			iter = temp;
			mDirty = true;
		}
	}
}


//---------------------------------------------------------------
//
// XTimer::AddTimer (XCallback0<void>, MilliSecond)
//
//---------------------------------------------------------------
void XTimer::AddTimer(XCallback0<void> callback, MilliSecond interval)
{
	this->AddTimer(callback, interval, interval);
}


//---------------------------------------------------------------
//
// XTimer::AddTimer (XCallback0<void>, MilliSecond, MilliSecond)
//
//---------------------------------------------------------------
void XTimer::AddTimer(XCallback0<void> callback, MilliSecond interval, MilliSecond delay)
{
	PRECONDITION(callback.IsValid());
	PRECONDITION(interval >= 0);
	PRECONDITION(delay >= 0);
	PRECONDITION(std::find(mCallbacks.begin(), mCallbacks.end(), callback) == mCallbacks.end());
	
	mCallbacks.push_back(SEntry(callback, interval, delay));
	
	if (interval < mMinDelay) {
		mMinDelay = interval;
		this->DoUpdate();
	}

	mDirty = true;

	POSTCONDITION(std::find(mCallbacks.begin(), mCallbacks.end(), callback) != mCallbacks.end());
}


//---------------------------------------------------------------
//
// XTimer::AddOneShotTimer
//
//---------------------------------------------------------------
void XTimer::AddOneShotTimer(XCallback0<void> callback, MilliSecond delay)
{
	PRECONDITION(callback.IsValid());
	PRECONDITION(delay >= 0);
	PRECONDITION(std::find(mCallbacks.begin(), mCallbacks.end(), callback) == mCallbacks.end());
	
	mCallbacks.push_back(SEntry(callback, LONG_MAX, delay));

	mDirty = true;

	POSTCONDITION(std::find(mCallbacks.begin(), mCallbacks.end(), callback) != mCallbacks.end());
}


//---------------------------------------------------------------
//
// XTimer::SetInterval
//
//---------------------------------------------------------------
void XTimer::SetInterval(XCallback0<void> callback, MilliSecond interval)
{
	PRECONDITION(callback.IsValid());
	PRECONDITION(interval >= 0);

	Callbacks::iterator iter = std::find(mCallbacks.begin(), mCallbacks.end(), callback);
	PRECONDITION(iter != mCallbacks.end());
	
	if (interval != iter->interval) {
		iter->interval = interval;
		
		if (interval < mMinDelay) {
			mMinDelay = interval;
			this->DoUpdate();
		}
	}
}


//---------------------------------------------------------------
//
// XTimer::SetDelay
//
//---------------------------------------------------------------
void XTimer::SetDelay(XCallback0<void> callback, MilliSecond delay)
{
	PRECONDITION(callback.IsValid());
	PRECONDITION(delay >= 0);

	Callbacks::iterator iter = std::find(mCallbacks.begin(), mCallbacks.end(), callback);
	PRECONDITION(iter != mCallbacks.end());
	
	iter->callTime = GetMilliSeconds() + delay;

	mDirty = true;
}


//---------------------------------------------------------------
//
// XTimer::RemoveTimer
//
// Remove doesn't affect the sort order so we don't need to set
// mDirty.
//
//---------------------------------------------------------------
void XTimer::RemoveTimer(XCallback0<void> callback)
{
	PRECONDITION(callback.IsValid());
	
	Callbacks::iterator iter = std::find(mCallbacks.begin(), mCallbacks.end(), callback);
	if (iter != mCallbacks.end()) {
		MilliSecond interval = iter->interval;
		mCallbacks.erase(iter);
	
		if (interval == mMinDelay) {
			mMinDelay = 10*1000;

			Callbacks::const_iterator iter = mCallbacks.begin();
			while (iter != mCallbacks.end()) {
				const SEntry& entry = *iter;
				++iter;
		
				if (entry.interval < mMinDelay)
					mMinDelay = entry.interval;
			}

			this->DoUpdate();
		}
	}

	POSTCONDITION(std::find(mCallbacks.begin(), mCallbacks.end(), callback) == mCallbacks.end());
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XTimer::DoUpdate
//
//---------------------------------------------------------------
void XTimer::DoUpdate()
{
	if (mTimerID != nil) {
		VERIFY(::KillTimer(nil, mTimerID));
		mTimerID = nil;
	}

	if (mMinDelay < 10*1000) {
		mTimerID = ::SetTimer(nil,						// window
							  nil,						// id
							  (unsigned int) mMinDelay,	// interval
							  nil);						// callback (nil => post an event)
		ThrowIf(mTimerID == nil);
	}
}


}	// namespace Whisper

