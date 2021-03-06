/*
 *  File:       XStateBroadcaster.h
 *  Summary:   	Singleton used to broadcast state changes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XStateBroadcaster.h,v $
 *		Revision 1.5  2001/04/13 08:03:48  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.4  2001/03/05 05:43:31  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:56:01  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:52:29  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XAnyValue.h>
#include <XAutoPtr.h>
#include <XBroadcaster.h>
#include <XCriticalSection.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Constants
//
const uint32 kStartingApp   	 = 'StAp';
const uint32 kQuittingApp   	 = 'QtAp';
const uint32 kSuspendingApp 	 = 'SpAp';
const uint32 kResumingApp   	 = 'RsAp';

const uint32 kOpeningWindow  	 = 'OpWn';		//!< data will have an const IWindow*			
const uint32 kOpenedWindow  	 = 'OdWn';		
const uint32 kShowingWindow  	 = 'ShWn';
const uint32 kShowedWindow  	 = 'SdWn';
const uint32 kHidingWindow  	 = 'HdWn';
const uint32 kHidWindow  		 = 'HDWn';
const uint32 kActivatingWindow   = 'AcWn';
const uint32 kDeactivatingWindow = 'DcWn';
const uint32 kRenamedWindow	 	= 'RnWn';			
const uint32 kClosingWindow  	 = 'ClWn';	
		
const uint32 kClosedWindow  	 = 'CdWn';	

const uint32 kMovedWindow		 = 'MvWn';
const uint32 kResizedWindow	 	 = 'RzWn';

const uint32 kDeletingControl 	 = 'DlCt';		//!< data has an const IControl*

#if MAC
	const uint32 kDeviceChanged	 = 'GDHc';		//!< data will have the GDHandle that's been changed (eg via the Monitors control panel)
	const uint32 kDesktopChanged = 'Dskc';		//!< one or more monitors has changed (this is broadcast after possibly multiple kDeviceChanged broadcasts)

	const uint32 kMemoryWentLow	 = 'LoMm';		//!< broadcast by XApplication when the reserve block is purged and reallocated
	const uint32 kMemoryWentHigh = 'HiMm';		//!< $$$ implement this
#endif


// ===================================================================================
//	struct SStateMessage
//!		The struct broadcast by XStateBroadcaster.
// ===================================================================================
struct CORE_EXPORT SStateMessage {
	uint32		mesg;
	XAnyValue	data;						
	
	SStateMessage(uint32 message)						{mesg = message;}
	SStateMessage(uint32 message, const XAnyValue& d)	{mesg = message; data = d;}
};


// ===================================================================================
//	class XStateBroadcaster
//!		Singleton used to broadcast state changes.
// ===================================================================================
class CORE_EXPORT XStateBroadcaster : public XBroadcasterMixin<SStateMessage> {

	typedef XBroadcasterMixin<SStateMessage> Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XStateBroadcaster();

						XStateBroadcaster();

	static XStateBroadcaster* Instance();

private:
						XStateBroadcaster(const XStateBroadcaster& rhs);
						
			XStateBroadcaster& operator=(const XStateBroadcaster& rhs);

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Broadcast(const SStateMessage& message) const;
	
//-----------------------------------
//	New API
//
public:
			void 		Broadcast(uint32 mesg, const XAnyValue& d) const		{this->Broadcast(SStateMessage(mesg, d));}

//-----------------------------------
//	Internal API
//
protected:
#if DEBUG
	virtual void		DoTraceMessage(const SStateMessage& message) const;
#endif

#if MAC
	static 	pascal OSErr DoAEDeviceChanged(const AppleEvent* event, AppleEvent* reply, int32 refCon);
#endif

//-----------------------------------
//	Member data
//
private:
	static XAutoPtr<XStateBroadcaster>	msInstance;			
	static XCriticalSection				msCreateMutex;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

