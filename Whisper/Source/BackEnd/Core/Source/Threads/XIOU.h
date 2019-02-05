/*
 *  File:       XIOU.h
 *  Summary:   	A class used to return the result of a threaded computation (also
 *				known as a future). 
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XIOU.h,v $
 *		Revision 1.5  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.4  2000/12/10 08:53:37  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.3  2000/11/20 05:36:15  jesjones
 *		XCallback again uses IsValid.
 *		
 *		Revision 1.2  2000/11/09 12:40:24  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	 4/02/00	JDJ		Fulfill, Cancel, and Abort now unlock the semaphore after
 *									calling the callback object.
 *		 <1>	 5/31/99	JDJ		Created
 */

#pragma once

#include <XCallbacks.h>
#include <XCriticalSection.h>
#include <XError.h>
#include <XExceptions.h>
#include <XReferenceCounted.h>
#include <XStringUtils.h>
#include <XSyncObjects.h>

namespace Whisper {


// ===================================================================================
//	class ZIOURef
// ===================================================================================
template <class T>
class ZIOURef : public XReferenceCountedMixin {
		
//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~ZIOURef();
	
  						ZIOURef();

  						ZIOURef(XCallback0<void> callback);

private:
						ZIOURef(const ZIOURef& rhs);

			ZIOURef& 	operator=(const ZIOURef& rhs);

//-----------------------------------
//	Member Data
//
public:
	volatile bool		mComputed;
	volatile bool 		mAborted;
	std::wstring		mAbortText;
	T					mResult;
	
	XCallback0<void>	mCallback;
	XSemaphore			mSemaphore;			// need to use a semaphore since we unlock from a different thread
	XCriticalSection	mResultMutex;		// need this on weak ordering machines (see below)
};


// ===================================================================================
//	class XIOU
//!		A class used to return the result of a threaded computation (also known as a future).
/*!		IOUs are used like this: \code
 *			static void ComputePi(XIOU<double>& result, double epsilon);
 *			
 *			static void Compute()
 *			{
 *				const double kEpsilon = 0.000001;
 *				
 *				// Create a callback to be executed inside a thread and bind
 *				// all its arguments.
 *				XIOU<double> result;
 *				XCallback2<void, XIOU<double>&, double> temp(ComputePi);
 *				XCallback0<void> function = Adapt0(temp, result, kEpsilon);
 *				
 *				// Create a callback object to handle exceptions in the function.
 *				// It would also be possible to add a catch block to the function
 *				// and explicitly call XIOU::Abort, but it's generally easier to
 *				// use a callback. 
 *				XThread::ErrorHandler errors(&result, &XIOU<double>::Abort);
 *				
 *				// Spin off a thread to execute the funtion. Callbacks and IOUs are
 *				// handles around reference counted bodies so we can release our
 *				// reference to the thread as soon as it starts up (this means the
 *				// thread will be deleted as soon as it finishes).
 *				XThread* thread = XThread::Create(function, errors);
 *				thread->Start();
 *				thread->RemoveReference();				
 *				
 *				// Display progress info while we wait for the thread to finish.
 *				while (!result.Redeemable() && !result.Aborted()) {
 *					TRACE(".");
 *					MilliSecondDelay(10);
 *				}
 *				TRACE("\n");
 *				
 *				// TRACE out the result.
 *				if (result.Redeemable())
 *					TRACE("pi = ", result.Redeem(), "\n");
 *				else
 *					TRACE("Couldn't compute pi. ", result.GetAbortText(), "\n");		
 *			} \endcode */
// ===================================================================================
template <class T>
class XIOU {
	
//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XIOU();
	
  						XIOU();

  						XIOU(XCallback0<void> callback);
  						/**< Callback will be called when the IOU is fulfilled (or aborted). */

						XIOU(const XIOU& rhs);

			XIOU& 		operator=(const XIOU& rhs);
			
//-----------------------------------
//	API
//
public:
	//! @name Debtee
	//@{
			bool 		Redeemable() const							{return mRef->mComputed;}
						/**< Returns true if the thread has computed the result. */
												
			bool 		Aborted() const								{return mRef->mAborted;}
						/**< Returns true if the thread was aborted (eg user canceled 
						or an exception was thrown). */
						
			void 		Wait(MilliSecond timeout = LONG_MAX);
						/**< Blocks until the IOU has been fulfilled or aborted. Note 
						that you should use one of the Wait functions below if you 
						want to wait on multiple IOUs. */
						
			T 			Redeem() const;
						/**< Returns the result. Note that it's an error to call this
						if Redeemable returns false; */
						
			std::wstring GetAbortText() const;
						/**< Returns the reason for the abort (may be empty). */
	//@}
						
	//! @name Debtor
	//@{
			void 		Fulfill(const T& result);
						/**< Called by the thread once the result has been computed. */
						
			void 		Abort(const std::exception* e);	
						/**< Called by the thread if the result cannot be computed.
						(Usually this will be called automatically via the thread's
						error handler). */

			void 		Cancel(const std::wstring& mesg);	
						/**< Just like Abort except that it's not called automatically
						and allows for an empty abort text message. (I'd call this
						function Abort, but <callback.hpp> doesn't work too well
						with overloaded member functions). */
	//@}
								
	//! @name Misc
	//@{
			bool 		operator==(const XIOU& rhs) const			{return mRef == rhs.mRef;}

			bool 		operator!=(const XIOU& rhs) const			{return mRef != rhs.mRef;}
	//@}

//-----------------------------------
//	Internal API
//
public:
			XSemaphore& GetSem()									{return mRef->mSemaphore;}

//-----------------------------------
//	Member Data
//
protected:
	ZIOURef<T>*	mRef;
};


// ===================================================================================
//	Global Functions
// ===================================================================================
template <class T, class U>
void 		Wait(XIOU<T>& i1, XIOU<U>& i2, MilliSecond timeout = LONG_MAX);

template <class T, class U, class V>
void 		Wait(XIOU<T>& i1, XIOU<U>& i2, XIOU<V>& i3, MilliSecond timeout = LONG_MAX);

template <class T, class U, class V, class W>
void 		Wait(XIOU<T>& i1, XIOU<U>& i2, XIOU<V>& i3, XIOU<W>& i4, MilliSecond timeout = LONG_MAX);
			// Blocks until all the IOUs are fulfilled (use these to avoid deadlocks).


// ===================================================================================
//	Methods
// ===================================================================================
template <class T>
ZIOURef<T>::~ZIOURef()
{
}
	
template <class T>
ZIOURef<T>::ZIOURef() : mSemaphore(0, 1)
{
	mComputed = false;
	mAborted = false;
}

template <class T>
ZIOURef<T>::ZIOURef(XCallback0<void> callback) : mSemaphore(0, 1), mCallback(callback)
{
	mComputed = false;
	mAborted = false;
}

// -----------------------------------------------------------------------------------

template <class T>
XIOU<T>::~XIOU()
{
	mRef->RemoveReference();
}

template <class T>
XIOU<T>::XIOU()
{
	mRef = new ZIOURef<T>;
}

template <class T>
XIOU<T>::XIOU(XCallback0<void> callback)
{
	mRef = new ZIOURef(callback);
}

template <class T>
XIOU<T>::XIOU(const XIOU& rhs)
{
	rhs.mRef->AddReference();
	mRef = rhs.mRef;
}

template <class T>
XIOU<T>& XIOU<T>::operator=(const XIOU& rhs)
{
	if (this != &rhs) {
		rhs.mRef->AddReference();
		mRef->RemoveReference();
		
		mRef = rhs.mRef;
	}
	
	return *this;
}

template <class T>
void XIOU<T>::Wait(MilliSecond timeout)
{
	XSingleLocker lock(mRef->mSemaphore, timeout);
}

template <class T>
T XIOU<T>::Redeem() const								
{
	ASSERT(mRef->mComputed); 
	
	// Modern processors often relax the order in which reads and writes to
	// distinct addresses are executed. For example, the Pentium II uses total 
	// store order which means that it may re-order instructions so that reads
	// occur before writes. OTOH the PowerPC uses weak ordering which means that
	// there are no guarantees on the order of reads and writes in the absence
	// of synchronization primitives. In particular this means that Fulfill may
	// set mComputed before mResult which requires us to serialize access to
	// mResult.
	T result;
	{
	XEnterCriticalSection enter(mRef->mResultMutex);	
		result = mRef->mResult;
	}
		
	return result;
}

template <class T>
std::wstring XIOU<T>::GetAbortText() const								
{
	ASSERT(mRef->mAborted); 		
		
	std::wstring text;
	{
	XEnterCriticalSection enter(mRef->mResultMutex);	
		text = mRef->mAbortText;
	}
		
	return text;
}

template <class T>
void XIOU<T>::Fulfill(const T& result)
{
	{
	XEnterCriticalSection enter(mRef->mResultMutex);
		mRef->mResult = result;
		mRef->mComputed = true;
	}
	
	if (mRef->mCallback.IsValid())
		mRef->mCallback();
	
	mRef->mSemaphore.Unlock();
}

template <class T>
void XIOU<T>::Abort(const std::exception* e)
{
	std::wstring text;
	if (e == nil)						// we can't copy the exception without slicing so we do the next best thing and convert the exception into a string
		text = LoadWhisperString(L"Unknown error.");
	
	else if (const XSystemException* sysErr = dynamic_cast<const XSystemException*>(e))
		text = XError::Instance()->GetText(sysErr->mError);
		
	else if (const std::bad_alloc* allocErr = dynamic_cast<const std::bad_alloc*>(e))
#if MAC
		text = XError::Instance()->GetText(memFullErr);
#elif WIN
		text = XError::Instance()->GetText(ERROR_NOT_ENOUGH_MEMORY);
#endif

	else
		text = FromUTF8Str(e->what());

	{
	XEnterCriticalSection enter(mRef->mResultMutex);	
		mRef->mAbortText = text;
		mRef->mAborted = true;
	}
	
	if (mRef->mCallback.IsValid())
		mRef->mCallback();
	
	mRef->mSemaphore.Unlock();
}	

template <class T>
void XIOU<T>::Cancel(const std::wstring& mesg)
{
	{
	XEnterCriticalSection enter(mRef->mResultMutex);	
		mRef->mAbortText = mesg;
		mRef->mAborted = true;
	}
	
	if (mRef->mCallback)
		mRef->mCallback();
	
	mRef->mSemaphore.Unlock();
}	

// -----------------------------------------------------------------------------------

template <class T, class U>
inline void Wait(XIOU<T>& i1, XIOU<U>& i2, MilliSecond timeout)
{
	XMultiLocker lock(i1.GetSem(), i2.GetSem(), timeout);
}

template <class T, class U, class V>
inline void Wait(XIOU<T>& i1, XIOU<U>& i2, XIOU<V>& i3, MilliSecond timeout)
{
	XMultiLocker lock(i1.GetSem(), i2.GetSem(), i3.GetSem(), timeout);
}

template <class T, class U, class V, class W>
inline void Wait(XIOU<T>& i1, XIOU<U>& i2, XIOU<V>& i3, XIOU<W>& i4, MilliSecond timeout)
{
	XMultiLocker lock(i1.GetSem(), i2.GetSem(), i3.GetSem(), i4.GetSem(), timeout);
}


}	// namespace Whisper
