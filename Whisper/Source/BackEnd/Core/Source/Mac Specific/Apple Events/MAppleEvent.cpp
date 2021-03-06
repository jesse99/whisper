/*
 *  File:       MAppleEvent.cpp
 *  Summary:   	Wrapper around an AppleEvent.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MAppleEvent.cpp,v $
 *		Revision 1.3  2001/04/21 03:23:50  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 09:09:42  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <MAppleEvent.h>

#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateAppleEvent
//
//---------------------------------------------------------------
static AERecord CreateAppleEvent(AEEventClass eventClass, AEEventID id, const MAEAddress& target)
{
	AEDesc desc = {typeNull, nil};
	
	OSErr err = AECreateAppleEvent(eventClass, id, &target, kAutoGenerateReturnID, kAnyTransactionID, &desc);
	ThrowIfOSErr(err);

	ASSERT(desc.descriptorType == typeAppleEvent);
		
	return desc;
}

#pragma mark -

// ===================================================================================
//	class MAppleEvent
// ===================================================================================

AEIdleUPP	MAppleEvent::msIdleProc   = nil;
AEFilterUPP	MAppleEvent::msFilterProc = nil;

//---------------------------------------------------------------
//
// MAppleEvent::~MAppleEvent
//
//---------------------------------------------------------------
MAppleEvent::~MAppleEvent()
{
}


//---------------------------------------------------------------
//
// MAppleEvent::MAppleEvent ()
//
//---------------------------------------------------------------
MAppleEvent::MAppleEvent() : MAERecord(CreateAppleEvent(typeNull, typeNull, '????'), false)
{	
	mSendingMode = kAENoReply;
	mTimeout     = kAEDefaultTimeout;
	mPriority    = kAENormalPriority;
}


//---------------------------------------------------------------
//
// MAppleEvent::MAppleEvent (AEEventClass, AEEventID, MAEAddress, int32)
//
//---------------------------------------------------------------
MAppleEvent::MAppleEvent(AEEventClass eventClass, AEEventID id, const MAEAddress& target, int32 sendMode) : MAERecord(CreateAppleEvent(eventClass, id, target), false)
{	
	mSendingMode = sendMode;
	mTimeout     = kAEDefaultTimeout;
	mPriority    = kAENormalPriority;
}


//---------------------------------------------------------------
//
// MAppleEvent::MAppleEvent (AppleEvent)
//
//---------------------------------------------------------------
MAppleEvent::MAppleEvent(const AppleEvent& desc, bool copyDesc) : MAERecord(desc, copyDesc)
{
	PRECONDITION(descriptorType == typeAppleEvent);
}


//---------------------------------------------------------------
//
// MAppleEvent::Send
//
//---------------------------------------------------------------
void MAppleEvent::Send()	
{
	PRECONDITION((mSendingMode & kAEWaitReply) != kAEWaitReply);

	MAEDesc reply;
	OSErr err = AESend(this, &reply, mSendingMode, mPriority, mTimeout, msIdleProc, msFilterProc);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAppleEvent::Send
//
//---------------------------------------------------------------
void MAppleEvent::Send(MAppleEvent* reply)	
{
	PRECONDITION(reply != nil);
	PRECONDITION((mSendingMode & kAEWaitReply) == kAEWaitReply);

	OSErr err = AESend(this, reply, mSendingMode, mPriority, mTimeout, msIdleProc, msFilterProc);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAppleEvent::GetPSN
//
//---------------------------------------------------------------
ProcessSerialNumber MAppleEvent::GetPSN() const
{
	ProcessSerialNumber psn;
	
	DescType actualType;
	int32 actualSize;
	OSErr err = AEGetAttributePtr(this, keyAddressAttr, typeProcessSerialNumber, &actualType, &psn, sizeof(psn), &actualSize);
	ThrowIfOSErr(err);
	
	return psn;
}


//---------------------------------------------------------------
//
// MAppleEvent::GetAddress
//
//---------------------------------------------------------------
MAEAddress MAppleEvent::GetAddress() const
{
	MAEAddress address;
	
	OSErr err = AEGetAttributeDesc(this, keyAddressAttr, typeWildCard, &address);
	ThrowIfOSErr(err);
	
	return address;
}


//---------------------------------------------------------------
//
// MAppleEvent::SetAddress
//
//---------------------------------------------------------------
void MAppleEvent::SetAddress(const MAEAddress& address)
{
	OSErr err = AEPutAttributeDesc(this, keyAddressAttr, &address);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAppleEvent::GetEventClass
//
//---------------------------------------------------------------
AEEventClass MAppleEvent::GetEventClass() const
{
	AEEventClass eventClass;

	DescType typeCode;
	int32 actualSize;

	OSErr err = AEGetAttributePtr(this, keyEventClassAttr, typeType, &typeCode, &eventClass, sizeof(eventClass), &actualSize);
	ThrowIfOSErr(err);

	return eventClass;
}


//---------------------------------------------------------------
//
// MAppleEvent::GetEventID
//
//---------------------------------------------------------------
AEEventID MAppleEvent::GetEventID() const
{
	AEEventID eventID;

	DescType typeCode;
	int32 actualSize;

	OSErr err = AEGetAttributePtr(this, keyEventIDAttr, typeType, &typeCode, &eventID, sizeof(eventID), &actualSize);
	ThrowIfOSErr(err);

	return eventID;
}


//---------------------------------------------------------------
//
// MAppleEvent::GetReturnID
//
//---------------------------------------------------------------
int32 MAppleEvent::GetReturnID() const
{
	int32 id;

	DescType actualType;
	int32 actualSize;

	OSErr err = AEGetAttributePtr(this, keyReturnIDAttr, typeLongInteger, &actualType, &id, sizeof(id), &actualSize);
	ThrowIfOSErr(err);

	return id;
}

		
//---------------------------------------------------------------
//
// MAppleEvent::SetReturnID
//
//---------------------------------------------------------------
void MAppleEvent::SetReturnID(int32 id)
{
	OSErr err = AEPutAttributePtr(this, keyReturnIDAttr, typeLongInteger, &id, sizeof(id));
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAppleEvent::GetTransactionID
//
//---------------------------------------------------------------
int32 MAppleEvent::GetTransactionID() const
{
	int32 id;
	
	DescType actualType;
	int32 actualSize;

	OSErr err = AEGetAttributePtr(this, keyTransactionIDAttr, typeLongInteger, &actualType, &id, sizeof(id), &actualSize);
	if (err != noErr)
		id = 0;

	return id;
}


//---------------------------------------------------------------
//
// MAppleEvent::SetTransactionID
//
//---------------------------------------------------------------
void MAppleEvent::SetTransactionID(int32 id)
{
	OSErr err = AEPutAttributePtr(this, keyTransactionIDAttr, typeLongInteger, &id, sizeof(id));
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAppleEvent::CheckRequiredParameters
//
//---------------------------------------------------------------
void MAppleEvent::CheckRequiredParameters()
{
	DescType returnedType;
	int32 actualSize;
	OSErr err = AEGetAttributePtr(this, keyMissedKeywordAttr, typeWildCard, &returnedType, nil, 0, &actualSize);

	if (err == noErr) 
		ThrowErr(errAEParamMissed);			// if keyMissedKeywordAttr is still present then we missed a required parameter

	else if (err != errAEDescNotFound) 		// if keyMissedKeywordAttr is missing then we got everything
		ThrowIfOSErr(err);
}


}	// namespace Whisper
