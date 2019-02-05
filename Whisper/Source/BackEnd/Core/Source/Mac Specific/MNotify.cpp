/*
 *  File:       MNotify.cpp
 *  Summary:   	A wrapper around the Notification Manager.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1996-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MNotify.cpp,v $
 *		Revision 1.6  2001/04/27 04:22:03  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.5  2001/04/21 03:27:19  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/04/13 07:51:23  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2001/03/03 23:57:49  jesjones
 *		Added calls to DEBUGSTR in catch (...) blocks.
 *		
 *		Revision 1.2  2000/11/09 09:16:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <MNotify.h>

#include <Icons.h>
#include <Resources.h>

#include <MProcess.h>
#include <XConstants.h>
#include <XExceptions.h>
#include <XMiscUtils.h>

namespace Whisper {


// ===================================================================================
//	class MNotify
// ===================================================================================

//----------------------------------------------------------------
//
// MNotify::~MNotify
//
//----------------------------------------------------------------
MNotify::~MNotify()
{
	if (mInstalled)
		NMRemove(&mRecord);

	if (mRecord.nmIcon != nil)
		DisposeIconSuite(mRecord.nmIcon, false);
		
	if (mRecord.nmSound != nil && mRecord.nmSound != (Handle) -1)
		ReleaseResource(mRecord.nmSound);

	if (mRecord.nmResp != nil)
		DisposeNMUPP(mRecord.nmResp);
}


//----------------------------------------------------------------
//
// MNotify::MNotify (ResID, ResID, bool)
//
//----------------------------------------------------------------
MNotify::MNotify(ResID iconID, ResID soundID, bool useMark)
{
	this->DoInit(L"", iconID, soundID, useMark);
}


//----------------------------------------------------------------
//
// MNotify::MNotify (wstring, ResID, ResID, bool)
//
//----------------------------------------------------------------
MNotify::MNotify(const std::wstring& mesg, ResID iconID, ResID soundID, bool useMark)
{
	this->DoInit(mesg, iconID, soundID, useMark);
}


//----------------------------------------------------------------
//
// MNotify::MNotify (wstring, ResID)
//
//----------------------------------------------------------------
MNotify::MNotify(const std::wstring& mesg, ResID soundID)
{
	this->DoInit(mesg, 0, soundID, false);
}

					
//----------------------------------------------------------------
//
// MNotify::Post
//
//----------------------------------------------------------------
void MNotify::Post()
{
	PRECONDITION(!mInstalled);
	
	OSErr err = NMInstall(&mRecord);
	ASSERT(err == noErr);
	
	mInstalled = err == noErr;
}


//----------------------------------------------------------------
//
// MNotify::HandleCallback
//
//----------------------------------------------------------------
void MNotify::HandleCallback()
{
	this->OnCallback();
	
	if (mCalledFromForeground)
		delete this;
}


//----------------------------------------------------------------
//
// MNotify::OnBroadcast
//
//----------------------------------------------------------------
void MNotify::OnBroadcast(const SStateMessage& message)
{
	PRECONDITION(!mCalledFromForeground);

	if (message.mesg == kResumingApp)
		delete this;
}


//----------------------------------------------------------------
//
// MNotify::DoInit
//
//----------------------------------------------------------------
void MNotify::DoInit(const std::wstring& mesg, ResID iconID, ResID soundID, bool useMark)
{	
	mRecord.nmIcon  = nil;
	mRecord.nmSound = nil;
	mRecord.nmResp  = nil;

	try {
		mText = mesg;
		
		mInstalled = false;
		mCalledFromForeground = MProcess::InFront();
		
		// Initialize the notification manager record.
		PascalString text = ToPascalStr(mText);
		
		mRecord.qLink    = nil;
		mRecord.qType    = nmType;
		mRecord.nmMark   = useMark;
		mRecord.nmStr    = (unsigned char*) text.c_str();
		mRecord.nmRefCon = reinterpret_cast<int32>(this);

		// Create a handle for the icon.
		if (iconID > 0) {
			(void) GetIconSuite(&mRecord.nmIcon, iconID, svAllSmallData);
		
			ASSERT(mRecord.nmIcon != nil);

			if (mRecord.nmIcon != nil)						// not worth throwing an exception for
				HNoPurge(mRecord.nmIcon);
		}
		
		// Create a handle for the sound.
		if (soundID > 0) {
			mRecord.nmSound = GetResource('snd ', soundID);
			ASSERT(mRecord.nmSound != nil);
			
			if (mRecord.nmSound != nil)						// not worth throwing an exception for
				HNoPurge(mRecord.nmSound);
		
		} else if (soundID < 0)
			mRecord.nmSound = (Handle) -1;

		// Create a upp for the notification callback.
		mRecord.nmResp = NewNMUPP(MNotify::DoCallback);
		ThrowIfMemFail(mRecord.nmResp);
		
		// Register with the state broadcaster so we can find out when we're
		// switched back in (we have to wait until we're switched to the front
		// before we remove the notification since we want the icon to keep
		// blinking).
		if (!mCalledFromForeground)
			XStateBroadcaster::Instance()->AddListener(this);

	} catch (...) {
		// this can be constructed from dicy places like Drag Manager callbacks
		// so we'll eat all exceptions
		DEBUGSTR("Got an exception in MNotify::DoInit");	
	}
}


//----------------------------------------------------------------
//
// MNotify::DoCallback									[static]
//
//----------------------------------------------------------------
pascal void MNotify::DoCallback(NMRec* record)
{
	try {
		MNotify* thisPtr = reinterpret_cast<MNotify*>(record->nmRefCon);
		
		thisPtr->HandleCallback();
	
	} catch (...) {
		DEBUGSTR("Got an exception in MNotify::DoCallback");	// can't throw in a callback!
	}
}


}	// namespace Whisper
