/*
 *  File:       XThread.cpp
 *  Summary:   	Abstract base class for threads. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XThread.cpp,v $
 *		Revision 1.4  2001/04/27 04:37:14  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2000/11/20 05:36:15  jesjones
 *		XCallback again uses IsValid.
 *		
 *		Revision 1.2  2000/11/09 12:56:16  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XThread.h>

#include <XError.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class ZCallbackThread
// ===================================================================================
class ZCallbackThread : public XThread {
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~ZCallbackThread();
	
  						ZCallbackThread(const XCallback0<void>& callback, const std::wstring& errorText, uint32 stackBytes);

  						ZCallbackThread(const XCallback0<void>& callback, ErrorHandler handler, uint32 stackBytes);
  						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnRun();
	
	virtual void 		OnException(const std::exception* e);

//-----------------------------------
//	Member Data
//
protected:
	XCallback0<void>	mCallback;
	ErrorHandler		mErrorHandler;
	std::wstring		mErrorText;
};


//---------------------------------------------------------------
//
// ZCallbackThread::~ZCallbackThread
//
//---------------------------------------------------------------
ZCallbackThread::~ZCallbackThread()
{
}


//---------------------------------------------------------------
//
// ZCallbackThread::ZCallbackThread (XCallback0<void>, wstring, uint32)
//
//---------------------------------------------------------------
ZCallbackThread::ZCallbackThread(const XCallback0<void>& callback, const std::wstring& errorText, uint32 stackBytes) : XThread(stackBytes), mCallback(callback), mErrorText(errorText)
{
	PRECONDITION(XError::Instance()->HasDeferredErrorHandler());
}


//---------------------------------------------------------------
//
// ZCallbackThread::ZCallbackThread (XCallback0<void>, ErrorHandler, uint32)
//
//---------------------------------------------------------------
ZCallbackThread::ZCallbackThread(const XCallback0<void>& callback, ErrorHandler handler, uint32 stackBytes) : XThread(stackBytes), mCallback(callback), mErrorHandler(handler)
{
}


//---------------------------------------------------------------
//
// ZCallbackThread::OnRun
//
//---------------------------------------------------------------
void ZCallbackThread::OnRun()
{
	mCallback();
}


//---------------------------------------------------------------
//
// ZCallbackThread::OnException
//
//---------------------------------------------------------------
void ZCallbackThread::OnException(const std::exception* e)
{
	if (mErrorHandler.IsValid()) {
		mErrorHandler(e);
	
	} else if (mErrorText.length() > 0) {
		std::wstring narrative = XError::Instance()->GetText(e);
		XError::Instance()->DeferError(mErrorText, narrative);

	} else
		DEBUGSTR("There was an error executing the callback thread!");	// since the client hasn't provided a way to notify the user we'll assume he wants to eat errors...
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XThread
// ===================================================================================

//---------------------------------------------------------------
//
// XThread::Create (XCallback0<void>, wstring, uint32)	[static]
//
//---------------------------------------------------------------
XThread* XThread::Create(const XCallback0<void>& callback, const std::wstring& errorText, uint32 stackBytes)
{
	XThread* thread = new ZCallbackThread(callback, errorText, stackBytes);
	
	return thread;
}


//---------------------------------------------------------------
//
// XThread::Create (XCallback0<void>, ErrorHandler, uint32)	[static]
//
//---------------------------------------------------------------
XThread* XThread::Create(const XCallback0<void>& callback, ErrorHandler handler, uint32 stackBytes)
{
	XThread* thread = new ZCallbackThread(callback, handler, stackBytes);
	
	return thread;
}


//---------------------------------------------------------------
//
// XThread::Start
//
//---------------------------------------------------------------
void XThread::Start()
{
	PRECONDITION(!mRunning);
	
	this->AddReference();

	{
	XEnterCriticalSection enter(msThreadsSection);	// can't do this in Run because the client's reference may go away before Run starts up
		msThreads.push_back(this);
	}
		
	mRunning = true;
	
	this->Wake();
}


//---------------------------------------------------------------
//
// XThread::Run
//
//---------------------------------------------------------------
void XThread::Run()
{
	try {
		mRunning = true;
				
		this->OnRun();
		
	} catch (const std::exception& e) {
		mAborted = true;
		this->OnException(&e);
	
	} catch (...) {
		mAborted = true;
		this->OnException(nil);
	}
	
	{
	XEnterCriticalSection enter(msThreadsSection);
		mRunning = false;

		iterator iter = std::find(msThreads.begin(), msThreads.end(), this);	// other threads may want to write to msThreads so find has to be protected...
		ASSERT(iter != msThreads.end());
		(void) msThreads.erase(iter);											// erase will delete 'this' if no-one else has a reference
	}

	this->RemoveReference();			// may be deleted after this line executes!
}


}	// namespace Whisper
