/*
 *  File:       ITimer.h
 *  Summary:   	An object that periodically calls callbacks.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ITimer.h,v $
 *		Revision 1.6  2001/05/12 11:14:11  jesjones
 *		Tweaked a comment.
 *		
 *		Revision 1.5  2001/04/27 09:10:08  jesjones
 *		Renamed the methods, added support for one shot timers, and for an initial delay.
 *		
 *		Revision 1.4  2000/12/15 09:07:49  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/14 08:25:33  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:07:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XCallbacks.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class ITimer
//!		An object that periodically calls callbacks.
/*!		You install a timer using code that looks something like this: \code
 *			XInterfacePtr<ITimer> timer(L"Application");
 *			XCallback0<void> callback(this, &XTraceExtension::DoUpdate);
 *			timer->v(callback, kUpdateInterval); \endcode
 *
 *      Note that you're responsible for removing the timer when your object is destroyed 
 *		(the remove code looks exactly like the add code except that you call RemoveTimer 
 *		instead of AddTimer). */
// ===================================================================================
class ITimer : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Adding -----
	virtual void 		AddTimer(XCallback0<void> callback, MilliSecond interval) = 0;
						/**< Interval is the amount of time you want to wait before calls
						to the callback. Note that the actual interval may occasionally be 
						larger than the specified interval (but it won't be smaller). */

	virtual void 		AddTimer(XCallback0<void> callback, MilliSecond interval, MilliSecond delay) = 0;
						/**< Allows you to specify the amount of time to wait before the
						timer is called the very first time. Defaults to the interval time. */

	virtual void 		AddOneShotTimer(XCallback0<void> callback, MilliSecond delay) = 0;
						/**< Installs a timer that will only be called once (you can use
						SetInterval to switch over to periodic time). */

	// ----- Mutators -----
	virtual void 		SetInterval(XCallback0<void> callback, MilliSecond interval) = 0;
						/**< Updates the interval of a previously added callback. */

	virtual void 		SetDelay(XCallback0<void> callback, MilliSecond delay) = 0;
						/**< The timer won't fire until delay msecs elapse at which time
						it will fire every interval msecs (this is just like calling
						RemoveTimer and then AddTimer with a new delay). */

	// ----- Removing -----
	virtual void 		RemoveTimer(XCallback0<void> callback) = 0;
						/**< Note that it's ok to call this if the callback hasn't been added. */
	
//-----------------------------------
//	Internal API
//
public: 
#if WIN
	virtual void 		Tickle() = 0;
						/**< Called by the app. */
#endif
};

typedef XInterfacePtr<ITimer> ITimerPtr;
typedef XInterfacePtr<const ITimer> IConstTimerPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

