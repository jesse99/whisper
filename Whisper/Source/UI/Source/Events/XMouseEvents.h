/*
 *  File:       XMouseEvents.h
 *  Summary:   	Cross platform wrapper around mouse events.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XMouseEvents.h,v $
 *		Revision 1.4  2001/02/07 04:58:38  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:36:33  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:43:40  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	 3/31/99	JDJ		Moved WasCapsLockDown, WasCommandKeyDown, and WasOptionKeyDown
 *									into XEvent.
 *		 <1>	10/08/98	JDJ		Created
 */

#pragma once

#include <XEvent.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ========================================================================================
//	class XMouseEvent
//!		Base class for mouse event classes.
// ========================================================================================
class UI_EXPORT XMouseEvent : public XEvent {

	typedef XEvent Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XMouseEvent();

						XMouseEvent();

						XMouseEvent(const OSEvent& event);
			
//-----------------------------------
//	New API
//
public:
#if WIN
	// ----- Modifier Keys -----
			bool 		WasShiftKeyDown() const;
						/**< On the Mac these are defined in XEvent. */

			bool 		WasControlKeyDown() const;
#endif
};


// ========================================================================================
//	class XMouseDownEvent
//!		Cross platform wrapper around mouse down events.
// ========================================================================================
class UI_EXPORT XMouseDownEvent : public XMouseEvent {

	typedef XMouseEvent Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XMouseDownEvent();

						XMouseDownEvent();

						XMouseDownEvent(const OSEvent& event, uint32 clickCount = 1);

//-----------------------------------
//	New API
//
public:
	// ----- Mouse Button -----
			bool 		IsLeftButton() const;

			bool 		IsMiddleButton() const;
						/**< Always false on Macs. */

			bool 		IsRightButton() const;
						/**< Will return true on a Mac if IsShowContextualMenuClick returns true
						(or the control key is down if CMM isn't installed). */

	// ----- Click Count -----
			uint32		GetClickCount() const				{return mClickCount;}

			void		SetClickCount(uint32 count)			{ASSERT(count >= 1); mClickCount = count;}

//-----------------------------------
//	Member Data
//
protected:
	uint32	mClickCount;
};


// ========================================================================================
//	class XMouseMoveEvent
//!		Cross platform wrapper around mouse move events.
// ========================================================================================
class UI_EXPORT XMouseMoveEvent : public XMouseEvent {

	typedef XMouseEvent Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XMouseMoveEvent();

						XMouseMoveEvent();

						XMouseMoveEvent(const OSEvent& event);
};


// ========================================================================================
//	class XMouseUpEvent
//!		Cross platform wrapper around mouse up events.
// ========================================================================================
class UI_EXPORT XMouseUpEvent : public XMouseEvent {

	typedef XMouseEvent Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XMouseUpEvent();

						XMouseUpEvent();

						XMouseUpEvent(const OSEvent& event);
};


// ========================================================================================
//	Inlines
// ========================================================================================
#if MAC
	inline bool XMouseDownEvent::IsLeftButton() const		{return !this->IsRightButton();}

	inline bool XMouseDownEvent::IsMiddleButton() const		{return false;}

#elif WIN
	inline bool XMouseEvent::WasShiftKeyDown() const		{return (mEvent.wParam & MK_SHIFT) != 0;}

	inline bool XMouseEvent::WasControlKeyDown() const		{return (mEvent.wParam & MK_CONTROL) != 0;}

	inline bool XMouseDownEvent::IsLeftButton() const		{return mEvent.message == WM_LBUTTONDOWN || mEvent.message == WM_LBUTTONDBLCLK;}

	inline bool XMouseDownEvent::IsMiddleButton() const		{return mEvent.message == WM_MBUTTONDOWN || mEvent.message == WM_MBUTTONDBLCLK;}

	inline bool XMouseDownEvent::IsRightButton() const		{return mEvent.message == WM_RBUTTONDOWN || mEvent.message == WM_RBUTTONDBLCLK;}
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
