/*
 *  File:       XBroadcaster.cpp
 *  Summary:    Mixin class allowing an object to broadcast to one or more listeners.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XBroadcaster.cpp,v $
 *		Revision 1.3  2001/04/21 03:39:48  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 12:26:50  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XBroadcaster.h>

#include <XDebug.h>
#include <XMiscUtils.h>

namespace Whisper {

 
// ===================================================================================
//	class XBaseBroadcasterMixin
// ===================================================================================

//---------------------------------------------------------------
//
// XBaseBroadcasterMixin::~XBaseBroadcasterMixin
//
//---------------------------------------------------------------
XBaseBroadcasterMixin::~XBaseBroadcasterMixin()
{
	this->RemoveAllListeners();
	
	delete mListeners;
}

	
//---------------------------------------------------------------
//
// XBaseBroadcasterMixin::XBaseBroadcasterMixin ()
//
//---------------------------------------------------------------
XBaseBroadcasterMixin::XBaseBroadcasterMixin()
{
	mBroadcasterEnabled = true;
	
	mListeners = new std::list<XBaseListenerMixin*>;
}


//---------------------------------------------------------------
//
// XBaseBroadcasterMixin::XBaseBroadcasterMixin (XBaseBroadcasterMixin)
//
//---------------------------------------------------------------
XBaseBroadcasterMixin::XBaseBroadcasterMixin(const XBaseBroadcasterMixin& rhs)
{
	UNUSED(rhs);
	
	XEnterCriticalSection enter(mBroadcasterMutex);

	mBroadcasterEnabled = rhs.mBroadcasterEnabled;
	
	mListeners = new std::list<XBaseListenerMixin*>;
}

	
//---------------------------------------------------------------
//
// XBaseBroadcasterMixin::operator=
//
//---------------------------------------------------------------
XBaseBroadcasterMixin& XBaseBroadcasterMixin::operator=(const XBaseBroadcasterMixin& rhs)
{
	if (this != &rhs) {
		XEnterCriticalSection enter(mBroadcasterMutex);

		mBroadcasterEnabled = rhs.mBroadcasterEnabled;
		
		mListeners->erase(mListeners->begin(), mListeners->end());
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XBaseBroadcasterMixin::IsBroadcasting
//
//---------------------------------------------------------------
bool XBaseBroadcasterMixin::IsBroadcasting() const
{
	return mBroadcasterEnabled;
}


//---------------------------------------------------------------
//
// XBaseBroadcasterMixin::HasListeners
//
//---------------------------------------------------------------
bool XBaseBroadcasterMixin::HasListeners() const					
{
	XEnterCriticalSection enter(mBroadcasterMutex);

	bool has = !mListeners->empty();
	
	return has;
}


//---------------------------------------------------------------
//
// XBaseBroadcasterMixin::RemoveAllListeners
//
//---------------------------------------------------------------
void XBaseBroadcasterMixin::RemoveAllListeners()
{
	XEnterCriticalSection enter(mBroadcasterMutex);

	std::list<XBaseListenerMixin*>::iterator iter = mListeners->begin();
	while (iter != mListeners->end()) {
		XBaseListenerMixin* listener = *iter++;
	
		listener->DoRemoveBroadcaster(this);
	}

	mListeners->erase(mListeners->begin(), mListeners->end());
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XBaseBroadcasterMixin::DoAddListener
//
//---------------------------------------------------------------
void XBaseBroadcasterMixin::DoAddListener(XBaseListenerMixin* listener)
{
	PRECONDITION(listener != nil);

	XEnterCriticalSection enter(mBroadcasterMutex);
	PRECONDITION(std::find(mListeners->begin(), mListeners->end(), listener) == mListeners->end());
	
	listener->DoAddBroadcaster(this);

	try {
		mListeners->push_back(listener);
		
	} catch (...) {
		listener->DoRemoveBroadcaster(this);
		throw;
	}
}


//---------------------------------------------------------------
//
// XBaseBroadcasterMixin::DoRemoveListener
//
//---------------------------------------------------------------
void XBaseBroadcasterMixin::DoRemoveListener(XBaseListenerMixin* listener)
{
	PRECONDITION(listener != nil);

	XEnterCriticalSection enter(mBroadcasterMutex);

	PRECONDITION(std::find(mListeners->begin(), mListeners->end(), listener) != mListeners->end());
	
	mListeners->remove(listener);
	
	listener->DoRemoveBroadcaster(this);
}


//---------------------------------------------------------------
//
// XBaseBroadcasterMixin::DoBroadcasting
//
//---------------------------------------------------------------
void XBaseBroadcasterMixin::DoBroadcasting(const void* message) const
{
	if (this->IsBroadcasting()) {
		XEnterCriticalSection enter(mBroadcasterMutex);

		std::list<XBaseListenerMixin*>::const_iterator iter = mListeners->begin();
		while (iter != mListeners->end()) {
			XBaseListenerMixin* listener = *iter++;
			
			if (listener->IsListening())
				this->DoBroadcast(listener, message);
		}
	}
}


}	// namespace Whisper
