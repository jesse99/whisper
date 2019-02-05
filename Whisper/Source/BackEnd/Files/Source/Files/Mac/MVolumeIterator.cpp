/*
 *  File:		MVolumeIterator.cpp
 *  Summary:	A class that iterates over volumes.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MVolumeIterator.cpp,v $
 *		Revision 1.3  2001/04/21 03:29:25  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 09:21:40  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <MVolumeIterator.h>

#include <XFileSystem.h>

namespace Whisper {


// ===================================================================================
//	class MVolumeIterator
// ===================================================================================

//---------------------------------------------------------------
//
// MVolumeIterator::MVolumeIterator ()
//
//---------------------------------------------------------------
MVolumeIterator::MVolumeIterator()
{
	mAllowRemote = false;
	mOnlineOnly  = true;
	
	mLocalOnly  = true;					// do local volumes first
	mRemoteOnly = false;		
	
	mIndex = -1;
	mIterating = false;
}


//---------------------------------------------------------------
//
// MVolumeIterator::MVolumeIterator ()
//
//---------------------------------------------------------------
MVolumeIterator::MVolumeIterator(bool allowRemote, bool onlineOnly)
{
	mAllowRemote = allowRemote;
	mOnlineOnly  = onlineOnly;
	
	mLocalOnly  = true;					// do local volumes first
	mRemoteOnly = false;		
	
	mIndex = 0;
	mIterating = true;
	
	this->Advance();
}


//---------------------------------------------------------------
//
// MVolumeIterator::operator++ ()
//
//---------------------------------------------------------------
MVolumeIterator& MVolumeIterator::operator++()
{
	if (mIterating)
		this->Advance();
		
	return *this;
}


//---------------------------------------------------------------
//
// MVolumeIterator::operator++ (int)
//
//---------------------------------------------------------------
MVolumeIterator MVolumeIterator::operator++(int)
{
	MVolumeIterator temp = *this;
	
	if (mIterating)
		this->Advance();
		
	return temp;
}


//---------------------------------------------------------------
//
// MVolumeIterator::Advance
//
//---------------------------------------------------------------
void MVolumeIterator::Advance()
{
	PRECONDITION(mIterating);
	PRECONDITION(mLocalOnly != mRemoteOnly);
		
	bool found = false;
	while (mIterating && !found) {
		mIndex++;
		
		HParamBlockRec pb;
		
		pb.volumeParam.ioCompletion = nil;
		pb.volumeParam.ioNamePtr    = nil;
		pb.volumeParam.ioVRefNum    = 0;
		pb.volumeParam.ioVolIndex   = mIndex;
		
		OSErr err = PBHGetVInfoSync(&pb);
		if (err == noErr) {
			int16 volume = pb.volumeParam.ioVRefNum;
			
			bool online = pb.volumeParam.ioVDrvInfo > 0;
			bool remote = XFileSystem::IsNetworkVolume(volume);
			
			if ((mOnlineOnly && online) || !mOnlineOnly) {
				if ((mLocalOnly && !remote) || (mRemoteOnly && remote)) {
					mVolume = volume;
					found = true;
				}
			}
			
		} else if (mLocalOnly && mAllowRemote) {
			mIndex = 0;
			
			mLocalOnly  = false;
			mRemoteOnly = true;
		
		} else
			mIterating = false;
	}
	
	if (!found)
		mIterating = false;
		
	if (!mIterating)
		mIndex = -1;
}


}	// namespace Whisper
