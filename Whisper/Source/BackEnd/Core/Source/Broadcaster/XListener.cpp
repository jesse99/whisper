/*
 *  File:       XListener.cpp
 *  Summary:    Mixin class that listens to a broadcaster.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XListener.cpp,v $
 *		Revision 1.3  2001/04/27 04:34:02  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:41:40  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */
 
#include <XWhisperHeader.h>
#include <XListener.h>

#include <XBroadcaster.h>
#include <XDebug.h>
#include <XMiscUtils.h>

namespace Whisper {


// ===================================================================================
//	class XBaseListenerMixin
// ===================================================================================

//---------------------------------------------------------------
//
// XBaseListenerMixin::~XBaseListenerMixin
//
//---------------------------------------------------------------
XBaseListenerMixin::~XBaseListenerMixin()
{
	this->DoRemoveAllBroadcasters();
	
	delete mBroadcasters;
}

	
//---------------------------------------------------------------
//
// XBaseListenerMixin::XBaseListenerMixin ()
//
//---------------------------------------------------------------
XBaseListenerMixin::XBaseListenerMixin()
{
	mListenerEnabled = true;
	
	mBroadcasters = new std::list<XBaseBroadcasterMixin*>;
}


//---------------------------------------------------------------
//
// XBaseListenerMixin::XBaseListenerMixin (XBaseListenerMixin)
//
//---------------------------------------------------------------
XBaseListenerMixin::XBaseListenerMixin(const XBaseListenerMixin& rhs)
{
	mListenerEnabled = rhs.mListenerEnabled;

	mBroadcasters = new std::list<XBaseBroadcasterMixin*>;
}

	
//---------------------------------------------------------------
//
// XBaseListenerMixin::operator=
//
//---------------------------------------------------------------
XBaseListenerMixin& XBaseListenerMixin::operator=(const XBaseListenerMixin& rhs)
{
	if (this != &rhs) {
		XEnterCriticalSection enter(mListenerMutex);

		mListenerEnabled = rhs.mListenerEnabled;

		mBroadcasters->erase(mBroadcasters->begin(), mBroadcasters->end());
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XBaseListenerMixin::IsListening
//
//---------------------------------------------------------------
bool XBaseListenerMixin::IsListening() const
{
	return mListenerEnabled;
}


//---------------------------------------------------------------
//
// XBaseListenerMixin::DoAddBroadcaster
//
//---------------------------------------------------------------
void XBaseListenerMixin::DoAddBroadcaster(XBaseBroadcasterMixin* broadcaster)
{
	PRECONDITION(broadcaster != nil);

	XEnterCriticalSection enter(mListenerMutex);
	PRECONDITION(std::find(mBroadcasters->begin(), mBroadcasters->end(), broadcaster) == mBroadcasters->end());
	
	mBroadcasters->push_back(broadcaster);		
}


//---------------------------------------------------------------
//
// XBaseListenerMixin::DoRemoveBroadcaster
//
//---------------------------------------------------------------
void XBaseListenerMixin::DoRemoveBroadcaster(XBaseBroadcasterMixin* broadcaster)
{
	PRECONDITION(broadcaster != nil);

	XEnterCriticalSection enter(mListenerMutex);
	PRECONDITION(std::find(mBroadcasters->begin(), mBroadcasters->end(), broadcaster) != mBroadcasters->end());
	
	mBroadcasters->remove(broadcaster);		
}


//---------------------------------------------------------------
//
// XBaseListenerMixin::DoRemoveAllBroadcasters
//
//---------------------------------------------------------------
void XBaseListenerMixin::DoRemoveAllBroadcasters()
{
	XEnterCriticalSection enter(mListenerMutex);

	while (!mBroadcasters->empty()) {
		XBaseBroadcasterMixin* broadcaster = mBroadcasters->front();

		broadcaster->DoRemoveListener(this);
	}
}


}	// namespace Whisper
