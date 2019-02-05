/*
 *  File:       XStateBroadcaster.cpp
 *  Summary:   	Singleton used to broadcast state changes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XStateBroadcaster.cpp,v $
 *		Revision 1.5  2001/04/27 04:36:28  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/04/13 08:03:45  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2001/03/03 23:58:52  jesjones
 *		Added calls to DEBUGSTR in catch (...) blocks.
 *		
 *		Revision 1.2  2000/11/09 12:52:24  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 7/23/99	JDJ		Fixed some bugs in DoAEDeviceChanged.
 *		 <3>	 7/19/99	JDJ		DoAEDeviceChanged is now a pascal routine.
 *		 <2>	 5/27/99	JDJ		Made Instance method thread safe.
 *		 <1>	10/10/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XStateBroadcaster.h>

#include <XExceptions.h>

#if MAC
	#include <Displays.h>
#endif

namespace Whisper {


// ===================================================================================
//	class XStateBroadcaster
// ===================================================================================

XAutoPtr<XStateBroadcaster>	XStateBroadcaster::msInstance;		
XCriticalSection 			XStateBroadcaster::msCreateMutex;

//---------------------------------------------------------------
//
// XStateBroadcaster::~XStateBroadcaster
//
//---------------------------------------------------------------
XStateBroadcaster::~XStateBroadcaster()
{
}


//---------------------------------------------------------------
//
// XStateBroadcaster::XStateBroadcaster
//
//---------------------------------------------------------------
XStateBroadcaster::XStateBroadcaster()
{
#if MAC
	AEEventHandlerUPP handler = NewAEEventHandlerUPP(XStateBroadcaster::DoAEDeviceChanged);
	
	OSErr err = AEInstallEventHandler(kCoreEventClass, kAESystemConfigNotice, handler, 0, false);
	ASSERT(err == noErr);		// don't throw for this!
#endif
}


//---------------------------------------------------------------
//
// XStateBroadcaster::Instance							[static]
//
//---------------------------------------------------------------
XStateBroadcaster* XStateBroadcaster::Instance()
{
	if (msInstance.Get() == nil) {
		XEnterCriticalSection enter(msCreateMutex);

		if (msInstance.Get() == nil) 
			msInstance.Reset(new XStateBroadcaster);
	}
	
	return msInstance.Get();
}


//---------------------------------------------------------------
//
// XStateBroadcaster::Broadcast
//
//---------------------------------------------------------------
void XStateBroadcaster::Broadcast(const SStateMessage& message) const
{
#if DEBUG
	this->DoTraceMessage(message);
#endif

	Inherited::Broadcast(message);
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XStateBroadcaster::DoTraceMessage
//
// If you want to TRACEFLOW your messages you need to create a
// subclass and initialize msInstance to point to your subclass.
//
//---------------------------------------------------------------	
#if DEBUG
void XStateBroadcaster::DoTraceMessage(const SStateMessage& message) const
{
	switch (message.mesg) {
		case kStartingApp:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kStartingApp\n");
			break;

		case kQuittingApp:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kQuittingApp\n");
			break;

		case kSuspendingApp:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kSuspendingApp\n");
			break;

		case kResumingApp:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kResumingApp\n");
			break;

		case kOpeningWindow:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kOpeningWindow\n");
			break;

		case kOpenedWindow:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kOpenedWindow\n");
			break;

		case kClosingWindow:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kClosingWindow\n");
			break;

		case kClosedWindow:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kClosedWindow\n");
			break;

		case kShowingWindow:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kShowingWindow\n");
			break;

		case kShowedWindow:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kShowedWindow\n");
			break;

		case kHidWindow:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kHidWindow\n");
			break;

		case kHidingWindow:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kHidingWindow\n");
			break;

		case kActivatingWindow:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kActivatingWindow\n");
			break;

		case kDeactivatingWindow:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kDeactivatingWindow\n");
			break;

		case kRenamedWindow:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kRenamedWindow\n");
			break;

		case kMovedWindow:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kMovedWindow\n");
			break;

		case kResizedWindow:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kResizedWindow\n");
			break;

		case kDeletingControl:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kDeletingControl\n");
			break;

#if MAC
		case kDeviceChanged:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kDeviceChanged\n");
			break;

		case kDesktopChanged:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kDesktopChanged\n");
			break;

		case kMemoryWentLow:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kMemoryWentLow\n");
			break;

		case kMemoryWentHigh:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting kMemoryWentHigh\n");
			break;
#endif

		default:
			TRACEFLOW("XStateBroadcaster", "XStateBroadcaster is broadcasting ", message.mesg, "\n");
	}
}
#endif	// DEBUG


//---------------------------------------------------------------
//
// XStateBroadcaster::DoAEDeviceChanged					[static]
//
//---------------------------------------------------------------
#if MAC
pascal OSErr XStateBroadcaster::DoAEDeviceChanged(const AppleEvent* inEvent, AppleEvent* reply, int32 refCon)
{
	PRECONDITION(inEvent != nil);
	#pragma unused(reply, refCon)

	AEDescList displayList = {nil, nil};		// note that app's only receive this event if they have the isDisplayManagerAware SIZE flag set

	try {		
		OSErr err = AEGetParamDesc(inEvent, kAEDisplayNotice, typeWildCard, &displayList);
		ThrowIfOSErr(err);

		int32 count;
		err = AECountItems(&displayList, &count);		
		ThrowIfOSErr(err);
		
		while (count > 0) {
			AEDescList displayID;
			AEKeyword tempWord;
			err = AEGetNthDesc(&displayList, count--, typeWildCard, &tempWord, &displayID);
			ThrowIfOSErr(err);

			AERecord newConfig;
			err = AEGetNthDesc(&displayID, 2, typeWildCard, &tempWord, &newConfig);
			if (err == noErr) {
				GDHandle device;
				DescType returnType;
				err = AEGetKeyPtr(&newConfig, keyDisplayDevice, typeWildCard, &returnType, &device, sizeof(device), nil);
				if (err == noErr) {
					XStateBroadcaster::Instance()->Broadcast(SStateMessage(kDeviceChanged, XAnyValue(device)));
				}
				
				(void) AEDisposeDesc(&newConfig);
			}
			
			(void) AEDisposeDesc(&displayID);	
		}
				
	} catch (...) {
		DEBUGSTR("Got an error in XStateBroadcaster::DoAEDeviceChanged");	
	}
	
	if (displayList.dataHandle != nil)
		(void) AEDisposeDesc(&displayList);

	XStateBroadcaster::Instance()->Broadcast(kDesktopChanged);

	return noErr;
}
#endif


}	// namespace Whisper

