/*
 *  File:       MAppleEvent.h
 *  Summary:   	Wrapper around an AppleEvent.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MAppleEvent.h,v $
 *		Revision 1.4  2000/12/11 07:20:09  jesjones
 *		Fixed a doxygen bug.
 *		
 *		Revision 1.3  2000/12/10 08:48:21  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:09:46  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: MAppleEvent.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.4  2000/12/11 07:20:09  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Fixed a doxygen bug.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/10 08:48:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Converted header comments to Doxygen format.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 9/15/98	JDJ		Created
 */

#pragma once

#include <MAEAddress.h>
#include <MAERecord.h>
#include <XDebug.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class MAppleEvent
//!		Wrapper around an AppleEvent.
// ===================================================================================
class CORE_EXPORT MAppleEvent : public MAERecord {

	typedef MAERecord Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual		   		~MAppleEvent();
						
						MAppleEvent();

						MAppleEvent(AEEventClass eventClass, AEEventID id, const MAEAddress& target, int32 sendMode = kAENoReply);
					
						MAppleEvent(const AppleEvent& desc, bool copyDesc = true);

//-----------------------------------
//	New API
//
public:
	//! @name Sending
	//@{
			void 		Send();
			
			void 		Send(MAppleEvent* reply);
						/**< Send the apple event and wait for a reply event. */
	//@}

	//! @name Sender
	//@{
			ProcessSerialNumber GetPSN() const;
						/**< Return the psn for the sender of the message. */
	//@}
			
	//! @name Addressing
	//@{
			MAEAddress 	GetAddress() const;
						/**< Return the target address of the message. */
						
			void 		SetAddress(const MAEAddress& desc);
	//@}
						
	//! @name Event Type
	//@{
			AEEventClass GetEventClass() const;
		
			AEEventID 	GetEventID() const;
	//@}

	//! @name Sending Mode
	//@{
			int32 		GetSendingMode() const					{return mSendingMode;}
			
			void 		SetSendingMode(int32 mode)				{mSendingMode = mode;}
						/**< Mode is a bit mask that includes the following:
						the reply mode: kAENoReply, kAEQueueReply, or kAEWaitReply
						the interaction level: kAENeverInteract, kAECanInteract, or kAEAlwaysInteract
						the application switch mode: kAECanSwitchLayer
						the reconnection receipt mode: kAEWantReceipt */
	//@}
			
	//! @name Timeout
	//@{
			MilliSecond GetTimeout() const						{return 50*mTimeout/3;}
						/**<Returns the amount of time to wait for a reply.
						Default is about a minute. You can use kNoTimeOut
						if you don't want to timeout. */
			
			void 		SetTimeout(MilliSecond timeout)			{ASSERT(timeout > 0); mTimeout = 3*timeout/50;}
	//@}
			
	//! @name Priority
	//@{
			int16 		GetPriority() const						{return mPriority;}
			
			void 		SetPriority(int16 priority)				{mPriority = priority;}
						/**<Priority can be kAENormalPriority or kAEHighPriority.
						Defaults to kAENormalPriority. */
	//@}
			
	//! @name Return ID
	//@{
			int32 		GetReturnID() const;
									
			void 		SetReturnID(int32 id);
						/**<Sets the ID value of the reply Apple Event. This is
						accessed using keyReturnIDAttr and defaults to 
						kAutoGenerateReturnID. */
	//@}
			
	//! @name Transaction ID
	//@{
			int32 		GetTransactionID() const;
			
			void 		SetTransactionID(int32 id);
						/**<You can use this to mark a sequence of Apple Events as
						belonging to the same transaction. */
	//@}
			
	//! @name Misc
	//@{
			void 		CheckRequiredParameters();
						/**<Throws an exception if all required parameters weren't
						retrieved. Every Apple Event handler should call this. */
	//@}

//-----------------------------------
//	Member Data
//
public:
	static AEIdleUPP	msIdleProc;			//!< XApplication ctor sets these
	static AEFilterUPP	msFilterProc;
	
protected:
	int16				mPriority;
	int32 				mSendingMode;
	int32				mTimeout;			// in ticks
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
