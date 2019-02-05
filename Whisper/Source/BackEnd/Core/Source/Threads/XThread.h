/*
 *  File:       XThread.h
 *  Summary:   	Abstract base class for threads. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XThread.h,v $
 *		Revision 1.4  2001/03/05 05:44:00  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:56:34  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:56:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <list>

#include <XCallbacks.h>
#include <XCriticalSection.h>
#include <XReferenceCounted.h>
#include <XTypes.h>

#if MAC
	#include <Threads.h>
	#include <Timer.h>
	#if	__profile__
		#include <Profiler.h>
	#endif
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif

//-----------------------------------
//	Types and Constants
//
#if MAC
	typedef ThreadID OSThread;
#elif WIN
	typedef HANDLE OSThread;
	#undef Yield						// Microsoft defines Yield to an empty string!
#endif


// ===================================================================================
//	class XThread
//!		Abstract base class for threads. 
// ===================================================================================
class CORE_EXPORT XThread : public XReferenceCountedMixin {
	
//-----------------------------------
//	Types
//
public:
	typedef std::list<XThread*>		ThreadList;
	typedef ThreadList::iterator	iterator;
	
	typedef XCallback1<void, const std::exception*>	ErrorHandler;	//!< exception pointer may be nil

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XThread();
	
  						XThread(uint32 stackBytes = 0);
  						/**< Threads are preemptive if possible and cooperative otherwise.
  						Threads always start out suspended. stackBytes is ignored on Windows. */
  						
  	static	XThread* 	Create(const XCallback0<void>& callback, const std::wstring& errorText, uint32 stackBytes = 0);
  						/**< Allows you to execute a stand-alone function or a member function
  						in a new thread. ErrorText will be used if an exception is thrown
  						inside the closure. If errorText is empty errors are eaten. */
  	
  	static	XThread* 	Create(const XCallback0<void>& callback, ErrorHandler handler, uint32 stackBytes = 0);
  						/**< Calls an error handler callback instead of popping up an error dialog. */

private:
						XThread(const XThread& rhs);

			XThread& 	operator=(const XThread& rhs);

//-----------------------------------
//	API
//
public:
	//! @name Startup and Shutdown
	//@{
	virtual void 		Start();
						/**< Call this to start the thread up. This will call AddReference and
						when the thread completes RemoveReference will be called so if you 
						call RemoveReference after calling Start the thread will be automatically
						deleted after it completes. */

	virtual void 		Run();
						/**< Called automatically once the thread starts up. */
						
//	virtual void 		Kill();
						// Terminates the thread immediately. This is *very* dangerous:
						// shared data structures may be left in an inconsistent state
						// and critical sections may be left locked. It's generally better 
						// to handle this by providing subclasses with an API for cleanly
						// canceling the thread execution.
	//@}
						
	//! @name Scheduling
	//@{
	virtual void 		Suspend();
						/**< Puts the thread to sleep for an indefinite amount of time. */
						
	virtual void 		Join(MilliSecond timeout = LONG_MAX);
						/**< Causes the calling thread to block until 'this' finishes. */
	
	virtual void 		Wake();
						/**< Makes the thread eligible for execution. Note that calls to
						Suspend/Sleep and Wake may be nested. */
						
	static 	void 		Sleep(MilliSecond delay);
						/**< Puts the current thread to sleep for a specified amount of time. */
	
	static 	void 		Yield();
						/**< Switches to the next eligible thread. This needs to be called
						often when cooperative threads may be used. For preemptive 
						threads this is a no-op. */
	//@}
						
	//! @name Priorities
	//@{
#if WIN
			int32 		GetPriority() const;
						/**< Returns one of the constants listed below in SetPriority. */
			
			void 		SetPriority(int32 priority);
						/**< Priority can be THREAD_PRIORITY_IDLE, THREAD_PRIORITY_LOWEST, 
						THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL, 
						THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_HIGHEST,
						or THREAD_PRIORITY_TIME_CRITICAL. */
#endif
	//@}

	//! @name Info
	//@{
			bool 		IsPreemptive() const				{return mPreemptive;}
			
			bool 		IsRunning() const					{return mRunning;}
						/**< Returns true if OnRun has been called, but hasn't
						yet exited. */
						
			bool 		WasAborted() const					{return mAborted;}
						/**< Returns true if the thread was stopped via Kill or a C++
						exception. */
						
			OSThread 	GetOSThread()						{return mThread;}
						
	static	XThread*	GetActiveThread();
						/**< Returns a pointer to the thread currently being executed.
						May return nil if the active thread isn't an XThread. Call
						RemoveReference when you're done with the thread. */
												
	static	bool		InMainThread();
	//@}

protected:
	//! @name Mandatory Overrides
	//@{
	virtual void 		OnRun() = 0;
	
	virtual void 		OnException(const std::exception* e) = 0;
						/**< Called if an exception is thrown from within Run.
						Typically subclasses will post an XDeferredErrorCommand.
						Note that e may be nil if something not descending from
						std::exception was caught. */
	//@}

//-----------------------------------
//	Internal API
//
protected:
#if MAC
	virtual void 		DoSleep(MilliSecond delay);

	static pascal void* DoEntry(void* refCon);

	static pascal void 	DoWake(TMTask* task);

#if	__profile__
	static pascal void	DoSwapIn(uint32 switchedThread, void* arg);
#endif

#elif WIN
	static unsigned int CALLBACK DoEntry(void* refCon);
#endif

//-----------------------------------
//	Internal Types
//
protected:
#if MAC
	struct SMyTask : public TMTask {
		XThread*	thisPtr;
	};
#endif

//-----------------------------------
//	Member Data
//
protected:
	OSThread				mThread;
	bool					mPreemptive;
	volatile bool			mRunning;
	volatile bool			mAborted;
	
#if MAC
	int32					mSleeping;
	SMyTask					mWakeTask;		
	static TimerUPP			msWakeUPP;	
	
#if TARGET_CARBON
	ThreadEntryUPP			mEntryUPP;
#endif

#if	__profile__
	ProfilerThreadRef		mProfileRef;
#endif

#elif WIN
	unsigned int			mID;	
#endif
	
	static XCriticalSection	msThreadsSection;
	static ThreadList		msThreads;
};


// ===================================================================================
//	Inlines
// ===================================================================================
#if WIN
inline void XThread::Yield()
{
}
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
