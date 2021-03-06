/*
 *  File:       XEvent.h
 *  Summary:   	Cross platform wrapper around an OS Event.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XEvent.h,v $
 *		Revision 1.4  2001/02/07 04:58:18  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:35:46  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:34:45  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XPoint.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XEvent
//!		Cross platform wrapper around an OS Event.
// ===================================================================================
class UI_EXPORT XEvent {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XEvent();
	
						XEvent();

						XEvent(const OSEvent& event);

//-----------------------------------
//	API
//
public:
	//! @name Event
	//@{
			OSEvent		GetOSEvent() const					{return mEvent;}

			MilliSecond	GetTimeStamp() const				{return mTimeStamp;}
						/**< Returns the time at which the event occured. */
	//@}

	//! @name Mouse Position
	//@{
			XPoint 		GetGlobalPosition() const			{return mGlobalPos;}

			XPoint 		GetPosition() const;

			void 		SetPosition(const XPoint& pos);
	//@}

	//! @name Modifier Keys
	//@{
#if MAC
			bool 		WasCommandKeyDown() const;

			bool 		WasOptionKeyDown()	const;

			bool 		WasShiftKeyDown() const;

			bool 		WasControlKeyDown() const;

			bool 		WasCapsLockDown() const;
#endif
	//@}

//-----------------------------------
//	Member Data
//
protected:
	OSEvent			mEvent;	
	XPoint			mGlobalPos;
	MilliSecond		mTimeStamp;	
};


// ===================================================================================
//	Inlines
// ===================================================================================
#if MAC
	inline XPoint XEvent::GetPosition() const				{return mEvent.where;}

	inline void XEvent::SetPosition(const XPoint& pos)		{mEvent.where = pos.GetOSPoint();}

	inline bool XEvent::WasShiftKeyDown() const				{return (mEvent.modifiers & shiftKey) != 0;}

	inline bool XEvent::WasControlKeyDown() const			{return (mEvent.modifiers & controlKey) != 0;}

	inline bool XEvent::WasCommandKeyDown() const			{return (mEvent.modifiers & cmdKey) != 0;}

	inline bool XEvent::WasCapsLockDown() const				{return (mEvent.modifiers & alphaLock) != 0;}

	inline bool XEvent::WasOptionKeyDown()	const			{return (mEvent.modifiers & optionKey) != 0;}

#elif WIN
	inline XPoint XEvent::GetPosition() const				{return mEvent.pt;}

	inline void XEvent::SetPosition(const XPoint& pos)		{mEvent.pt = pos;}
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
