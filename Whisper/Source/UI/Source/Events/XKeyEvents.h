/*
 *  File:       XKeyEvents.h
 *  Summary:   	Cross platform wrapper around keystroke events.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XKeyEvents.h,v $
 *		Revision 1.4  2001/02/07 04:58:24  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:36:06  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:40:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 8/01/99	JDJ		Fixed some silly bugs in XKeyDownEvent::WasRepeating and XCharEvent::WasRepeating (Win).
 *		 <3>	 8/01/99	JDJ		Added XCharEvent::WasAltKeyDown (Win).
 *		 <2>	 4/03/99	JDJ		Added GetRepeatDelay. Implemented GetRepeatCount on Windows.
 *		 <1>	10/08/98	JDJ		Created
 */

#pragma once

#include <XEvent.h>
#include <XNumbers.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ========================================================================================
//	class XKeyDownEvent
//!		Cross platform wrapper around keydown events that returns the key code.
// ========================================================================================
class UI_EXPORT XKeyDownEvent : public XEvent {

	typedef XEvent Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XKeyDownEvent();

						XKeyDownEvent();

						XKeyDownEvent(const OSEvent& event);

//-----------------------------------
//	New API
//
public:
	// ----- Key -----
			int16		GetKey() const;
			
			wchar_t 	ComputeChar() const;
						/**< On the Mac this will return the character. On Windows it will
						return an unshifted character code or zero if there's no translation. */

	// ----- Event Info -----
			bool 		WasRepeating() const;
						/**< True if the user is holding the key down and a key down event has
						already been processed. */
			
			uint32 		GetRepeatCount() const;
						/**< Windows will merge consecutive key down messages together and
						bump the repeat count (on the Mac this will one). */

	// ----- Misc -----
	static	MilliSecond	GetRepeatDelay();
						/**< The amount of time the user must hold down a key before it starts to repeat. */

	static	MilliSecond	GetRepeatFreq();
						/**< The rate at which repeated key down events are placed in the event queue. */

//-----------------------------------
//	Member Data
//
protected:
	mutable wchar_t 	mChar;
};


// ========================================================================================
//	class XCharEvent
//!		Cross platform wrapper around keydown events that returns the character code.
// ========================================================================================
class UI_EXPORT XCharEvent : public XEvent {

	typedef XEvent Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XCharEvent();

						XCharEvent();

						XCharEvent(const OSEvent& event);

//-----------------------------------
//	New API
//
public:
	// ----- Character Code -----
			wchar_t 	GetChar() const;
			
			int16		ComputeKey() const;
						/**< On the Mac this will return the key code. On Windows it will
						usually return the virtual key code, but may return -1 if a
						translation cannot be found. */

	// ----- Event Info -----
			bool 		WasRepeating() const;
						/**< True if the user is holding the key down and a key down event has
						already been processed. */
			
			uint32 		GetRepeatCount() const;
						/**< Windows will merge consecutive key down messages together and
						bump the repeat count (on the Mac this will be one). */

	// ----- Modifier Keys -----
#if WIN
			bool 		WasAltKeyDown() const;
#endif

//-----------------------------------
//	Member Data
//
protected:
	mutable int16 	mKey;
};


// ========================================================================================
//	class XKeyUpEvent
//!		Cross platform wrapper around keyup events.
// ========================================================================================
class UI_EXPORT XKeyUpEvent : public XEvent {

	typedef XEvent Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XKeyUpEvent();

						XKeyUpEvent();

						XKeyUpEvent(const OSEvent& event);

//-----------------------------------
//	New API
//
public:
	// ----- Virtual Key Code -----
			int16		GetKey() const;
};


// ========================================================================================
//	Inlines
// ========================================================================================
#if MAC
	inline int16 XKeyDownEvent::GetKey() const				{return (int16) ((mEvent.message & keyCodeMask) >> 8);}
						
	inline bool XKeyDownEvent::WasRepeating() const			{return mEvent.what == autoKey;}

	inline uint32 XKeyDownEvent::GetRepeatCount() const		{return 1;}

#elif WIN
	inline int16 XKeyDownEvent::GetKey() const				{return (int16) mEvent.wParam;}
						
	inline bool XKeyDownEvent::WasRepeating() const			{return (mEvent.lParam & 0x40000000) != 0;}

	inline uint32 XKeyDownEvent::GetRepeatCount() const		{return Max((uint32) (mEvent.lParam & 0xFFFF), 1UL);}
#endif


#if MAC
	inline wchar_t XCharEvent::GetChar() const				{return (wchar_t) (mEvent.message & charCodeMask);}
			
	inline bool XCharEvent::WasRepeating() const			{return mEvent.what == autoKey;}

	inline uint32 XCharEvent::GetRepeatCount() const		{return 1;}

#elif WIN
	inline wchar_t XCharEvent::GetChar() const				{return (wchar_t) mEvent.wParam;}
			
	inline bool XCharEvent::WasRepeating() const			{return (mEvent.lParam & 0x40000000) != 0;}

	inline uint32 XCharEvent::GetRepeatCount() const		{return Max((uint32) (mEvent.lParam & 0xFFFF), 1UL);}

	inline bool XCharEvent::WasAltKeyDown() const			{return (mEvent.lParam & 0x20000000) != 0;}
#endif


#if MAC
	inline int16 XKeyUpEvent::GetKey() const				{return (int16) ((mEvent.message & keyCodeMask) >> 8);}

#elif WIN
	inline int16 XKeyUpEvent::GetKey() const				{return (int16) mEvent.wParam;}
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
