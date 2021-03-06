/*
 *  File:		MProcess.cpp
 *  Summary:	Some handy functions dealing with the Process Manager.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MProcess.cpp,v $
 *		Revision 1.2  2000/11/09 09:18:08  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <2>	 2/01/00	JDJ		Moved the LaunchApp functions to MDesktopMgr.
 *		 <1>	 1/17/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <MProcess.h>

#include <AppleEvents.h>
#include <Errors.h>

#include <XDebug.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class MProcess
// ===================================================================================

//---------------------------------------------------------------
//
// MProcess::InFront ()									[static]
//
//---------------------------------------------------------------
bool MProcess::InFront()
{
	ProcessSerialNumber	psn = {0, kCurrentProcess};

	return MProcess::InFront(psn);
}


//---------------------------------------------------------------
//
// MProcess::PullToFront ()								[static]
//
//---------------------------------------------------------------
void MProcess::PullToFront()
{
	ProcessSerialNumber	psn = {0, kCurrentProcess};

	MProcess::PullToFront(psn);
}


//---------------------------------------------------------------
//
// MProcess::Kill ()									[static]
//
//---------------------------------------------------------------
void MProcess::Kill()
{
	ProcessSerialNumber	psn = {0, kCurrentProcess};

	MProcess::Kill(psn);
}


//---------------------------------------------------------------
//
// MProcess::InFront (ProcessSerialNumber)				[static]
//
//---------------------------------------------------------------
bool MProcess::InFront(const ProcessSerialNumber& psn)
{
	Boolean inFront = true;

	ProcessSerialNumber	frontPSN;
	OSErr err = GetFrontProcess(&frontPSN);
	if (err == noErr)
		err = SameProcess(&psn, &frontPSN, &inFront);
	
	ASSERT(err == noErr);
	
	return inFront;
}


//---------------------------------------------------------------
//
// MProcess::PullToFront (ProcessSerialNumber)			[static]
//
//---------------------------------------------------------------
void MProcess::PullToFront(const ProcessSerialNumber& psn)
{
	OSErr err = SetFrontProcess(&psn);
	ASSERT(err == noErr);
}


//---------------------------------------------------------------
//
// MProcess::IsRunning (OSType)							[static]
//
//---------------------------------------------------------------
bool MProcess::IsRunning(OSType signature)
{
	ProcessSerialNumber psn = {0, kNoProcess};

	bool found = false;	
	OSErr err = noErr;
	
	ProcessInfoRec info;		
	info.processInfoLength = sizeof(ProcessInfoRec);
	info.processName       = nil;
	info.processAppSpec    = nil;
	info.processLocation   = nil;
	
	while (!found && err == noErr) {
		err = GetNextProcess(&psn);
		if (psn.highLongOfPSN == 0 && psn.lowLongOfPSN == kNoProcess)
			err = procNotFound;
			
		if (err == noErr) {
			err = GetProcessInformation(&psn, &info);
			
			found = err == noErr && info.processSignature == signature;
		}
	}
	
	return found;	
}


//---------------------------------------------------------------
//
// MProcess::IsRunning (OSType, OSType)					[static]
//
//---------------------------------------------------------------
bool MProcess::IsRunning(OSType type, OSType creator)
{
	ProcessSerialNumber psn = {0, kNoProcess};

	bool found = false;	
	OSErr err = noErr;
	
	ProcessInfoRec info;		
	info.processInfoLength = sizeof(ProcessInfoRec);
	info.processName       = nil;
	info.processAppSpec    = nil;
	info.processLocation   = nil;
	
	while (!found && err == noErr) {
		err = GetNextProcess(&psn);
		if (psn.highLongOfPSN == 0 && psn.lowLongOfPSN == kNoProcess)
			err = procNotFound;
			
		if (err == noErr) {
			err = GetProcessInformation(&psn, &info);
			
			found = err == noErr && info.processType == type && info.processSignature == creator;
		}
	}
	
	return found;	
}


//---------------------------------------------------------------
//
// MProcess::Kill (ProcessSerialNumber)					[static]
//
//---------------------------------------------------------------
void MProcess::Kill(const ProcessSerialNumber& psn)
{
	AEDesc 		target = {typeNull, nil};
	AppleEvent	event  = {typeNull, nil};

	try {
		OSErr err = AECreateDesc(typeProcessSerialNumber, &psn, sizeof(ProcessSerialNumber), &target);
		ThrowIfOSErr(err);

		err = AECreateAppleEvent(kCoreEventClass, kAEQuitApplication, &target, kAutoGenerateReturnID, kAnyTransactionID, &event);
		ThrowIfOSErr(err);

		AppleEvent reply = {typeNull, nil};
		err = AESend(&event, &reply, kAENoReply, kAENormalPriority, kAEDefaultTimeout, nil, nil);
		ThrowIfOSErr(err);

		AEDisposeDesc(&target);
		AEDisposeDesc(&event);
		
	} catch (...) {
		AEDisposeDesc(&target);
		AEDisposeDesc(&event);
		
		throw;
	}
}


//---------------------------------------------------------------
//
// MProcess::GetFrontSerialNumber						[static]
//
//---------------------------------------------------------------
ProcessSerialNumber MProcess::GetFrontSerialNumber()
{
	ProcessSerialNumber psn;
	
	OSErr err = GetFrontProcess(&psn);
	ThrowIfOSErr(err);
	
	return psn;
}


//---------------------------------------------------------------
//
// MProcess::GetSerialNumber ()							[static]
//
//---------------------------------------------------------------
ProcessSerialNumber MProcess::GetSerialNumber()
{
	ProcessSerialNumber psn;
	
	OSErr err = GetCurrentProcess(&psn);
	ThrowIfOSErr(err);
	
	return psn;
}


//---------------------------------------------------------------
//
// MProcess::GetSerialNumber (OSType)					[static]
//
//---------------------------------------------------------------
ProcessSerialNumber MProcess::GetSerialNumber(OSType signature)
{
	ProcessSerialNumber psn = {0, kNoProcess};

	bool found = false;	
	OSErr err = noErr;
	
	ProcessInfoRec info;		
	info.processInfoLength = sizeof(ProcessInfoRec);
	info.processName       = nil;
	info.processAppSpec    = nil;
	info.processLocation   = nil;
	
	while (!found && err == noErr) {
		err = GetNextProcess(&psn);
		if (psn.highLongOfPSN == 0 && psn.lowLongOfPSN == kNoProcess)
			err = procNotFound;
			
		if (err == noErr) {
			err = GetProcessInformation(&psn, &info);
			
			found = err == noErr && info.processSignature == signature;
		}
	}
	
	if (!found)
		ThrowErr(procNotFound);
	
	return psn;	
}


//---------------------------------------------------------------
//
// MProcess::GetSerialNumber (OSType, OSType)			[static]
//
//---------------------------------------------------------------
ProcessSerialNumber MProcess::GetSerialNumber(OSType type, OSType creator)
{
	ProcessSerialNumber psn = {0, kNoProcess};

	bool found = false;	
	OSErr err = noErr;
	
	ProcessInfoRec info;		
	info.processInfoLength = sizeof(ProcessInfoRec);
	info.processName       = nil;
	info.processAppSpec    = nil;
	info.processLocation   = nil;
	
	while (!found && err == noErr) {
		err = GetNextProcess(&psn);
		if (psn.highLongOfPSN == 0 && psn.lowLongOfPSN == kNoProcess)
			err = procNotFound;
			
		if (err == noErr) {
			err = GetProcessInformation(&psn, &info);
			
			found = err == noErr && info.processType == type && info.processSignature == creator;
		}
	}
	
	if (!found)
		ThrowErr(procNotFound);
	
	return psn;	
}


//---------------------------------------------------------------
//
// MProcess::GetInfo									[static]
//
//---------------------------------------------------------------
ProcessInfoRec MProcess::GetInfo(const ProcessSerialNumber& psn)
{
	ProcessInfoRec info;
	
	info.processInfoLength = sizeof(ProcessInfoRec);
	info.processName       = nil;
	info.processAppSpec    = nil;
	info.processLocation   = nil;
	
	OSErr err = GetProcessInformation(&psn, &info);
	ThrowIfOSErr(err);
	
	return info;
}


}	// namespace Whisper
