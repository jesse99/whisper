/*
 *  File:       MTimer.cpp
 *  Summary:   	ITimer implementation based on Carbon Event timers.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MTimer.cpp,v $
 *		Revision 1.2  2001/05/14 10:54:49  jesjones
 *		DoTimer traps exceptions.
 *		
 *		Revision 1.1  2001/04/27 09:25:55  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <ITimer.h>

#include <list>

#include <XError.h>
#include <XExceptions.h>
#include <XInterfaceMacros.h>
#include <XMiscUtils.h>
#include <XObjectModel.h>

namespace Whisper {


// ===================================================================================
//	class XTimer
//!		ITimer implementation based on Carbon Event timers.
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
	
//-----------------------------------
//	Internal Types
//
private:	
	struct SEntry {
		XCallback0<void>	callback;
		EventLoopTimerRef	timer;
		
					SEntry()	{}
					SEntry(XCallback0<void> c) : callback(c), timer(nil) {}
			bool 	operator==(const XCallback0<void>& rhs) const	{return callback == rhs;}
	};

	typedef std::list<SEntry> Callbacks;		// use a list so clients can remove their timers within the callback

//-----------------------------------
//	Internal API
//
private:	
	static pascal void 	DoTimer(EventLoopTimerRef timer, void* refCon);

//-----------------------------------
//	Member Data
//
private:
	Callbacks			mCallbacks;	
	EventLoopTimerUPP	mTimerUPP;
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
	
	if (mTimerUPP != nil)
		DisposeEventLoopTimerUPP(mTimerUPP);
}


//---------------------------------------------------------------
//
// XTimer::XTimer
//
//---------------------------------------------------------------
XTimer::XTimer(XBoss* boss) 
{	
	ITimer::DoSetBoss(boss);
		
	mTimerUPP = NewEventLoopTimerUPP(XTimer::DoTimer);
	ThrowIfNil(mTimerUPP);
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
	
	interval = std::max(interval, 1L);		// 0 means one-shot

	mCallbacks.push_back(callback);			// have to do this ahead of time so we can get a pointer to the callback that will stay good
	
	EventLoopTimerRef timer = nil;
	OSStatus err = InstallEventLoopTimer(GetMainEventLoop(), 	
										 delay*kEventDurationMillisecond, 
										 interval*kEventDurationMillisecond,
										 mTimerUPP,
										 &(mCallbacks.back().callback),
										 &timer);
	if (err != noErr) {	
		mCallbacks.pop_back();
		ThrowIfOSErr(err);
	}
	
	mCallbacks.back().timer = timer;

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
	
	mCallbacks.push_back(callback);			// have to do this ahead of time so we can get a pointer to the callback that will stay good
	
	EventLoopTimerRef timer = nil;
	OSStatus err = InstallEventLoopTimer(GetMainEventLoop(), 	
										 delay*kEventDurationMillisecond, 
										 0,
										 mTimerUPP,
										 &(mCallbacks.back().callback),
										 &timer);
	if (err != noErr) {	
		mCallbacks.pop_back();
		ThrowIfOSErr(err);
	}
	
	mCallbacks.back().timer = timer;

	POSTCONDITION(std::find(mCallbacks.begin(), mCallbacks.end(), callback) != mCallbacks.end());
}


//---------------------------------------------------------------
//
// XTimer::SetInterval
//
//---------------------------------------------------------------
void XTimer::SetInterval(XCallback0<void> callback, MilliSecond interval)
{
	this->RemoveTimer(callback);
	this->AddTimer(callback, interval);
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
	
	OSStatus err = SetEventLoopTimerNextFireTime(iter->timer, delay*kEventDurationMillisecond);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// XTimer::RemoveTimer
//
//---------------------------------------------------------------
void XTimer::RemoveTimer(XCallback0<void> callback)
{
	PRECONDITION(callback.IsValid());
	
	Callbacks::iterator iter = std::find(mCallbacks.begin(), mCallbacks.end(), callback);
	if (iter != mCallbacks.end()) {
		OSStatus err = RemoveEventLoopTimer(iter->timer);
		ASSERT(err == noErr);
		
		mCallbacks.erase(iter);
	}

	POSTCONDITION(std::find(mCallbacks.begin(), mCallbacks.end(), callback) == mCallbacks.end());
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XTimer::DoTimer										[static]
//
//---------------------------------------------------------------
pascal void XTimer::DoTimer(EventLoopTimerRef timer, void* refCon)
{
	PRECONDITION(refCon != nil);
	UNUSED(timer);
	
	try {
		XCallback0<void>* callback = static_cast<XCallback0<void>*>(refCon);
		(*callback)();
				
	} catch (const std::exception& e) {
		DEBUGSTR("Caught an exception in XTimer::DoTimer: ", XError::Instance()->GetText(&e));

	} catch (...) {
		DEBUGSTR("Caught an exception in XTimer::DoTimer.");	// $$$ remove the timer?
	}		
}


}	// namespace Whisper

