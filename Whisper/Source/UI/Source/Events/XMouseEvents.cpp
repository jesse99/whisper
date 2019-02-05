/*
 *  File:       XMouseEvents.cpp
 *  Summary:   	Cross platform wrapper around mouse events.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XMouseEvents.cpp,v $
 *		Revision 1.3  2001/04/27 04:34:38  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:43:34  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XMouseEvents.h>

#if MAC
	#include <MSystemInfo.h>
#endif

namespace Whisper {


// ========================================================================================
//	class XMouseEvent
// ========================================================================================

//---------------------------------------------------------------
//
// XMouseEvent::~XMouseEvent
//
//---------------------------------------------------------------
XMouseEvent::~XMouseEvent()
{
}


//---------------------------------------------------------------
//
// XMouseEvent::XMouseEvent ()
//
//---------------------------------------------------------------
XMouseEvent::XMouseEvent()
{
}


//---------------------------------------------------------------
//
// XMouseEvent::XMouseEvent (OSEvent)
//
//---------------------------------------------------------------
XMouseEvent::XMouseEvent(const OSEvent& event) : XEvent(event)
{
#if MAC
	PRECONDITION(event.what == osEvt || event.what == mouseDown || event.what == mouseUp);
#elif WIN
	PRECONDITION(event.message == WM_LBUTTONDOWN || event.message == WM_LBUTTONDBLCLK ||
	       event.message == WM_MBUTTONDOWN || event.message == WM_MBUTTONDBLCLK ||
	       event.message == WM_RBUTTONDOWN || event.message == WM_RBUTTONDBLCLK ||
	       event.message == WM_LBUTTONUP   || 
	       event.message == WM_MBUTTONUP   || 
	       event.message == WM_RBUTTONUP   ||
	       event.message == WM_MOUSEMOVE);

	mEvent.pt.x = LOWORD(event.lParam);
	mEvent.pt.y = HIWORD(event.lParam);
#endif
}

#if __MWERKS__
#pragma mark -
#endif

// ========================================================================================
//	class XMouseDownEvent
// ========================================================================================

//---------------------------------------------------------------
//
// XMouseDownEvent::~XMouseDownEvent
//
//---------------------------------------------------------------
XMouseDownEvent::~XMouseDownEvent()
{
}


//---------------------------------------------------------------
//
// XMouseDownEvent::XMouseDownEvent ()
//
//---------------------------------------------------------------
XMouseDownEvent::XMouseDownEvent()
{
	mClickCount = 0;
}


//---------------------------------------------------------------
//
// XMouseDownEvent::XMouseDownEvent (OSEvent, uint32)
//
//---------------------------------------------------------------
XMouseDownEvent::XMouseDownEvent(const OSEvent& event, uint32 clickCount) : XMouseEvent(event)
{
#if MAC
	PRECONDITION(event.what == mouseDown);
#elif WIN
	PRECONDITION(event.message == WM_LBUTTONDOWN || event.message == WM_LBUTTONDBLCLK ||
	       event.message == WM_MBUTTONDOWN || event.message == WM_MBUTTONDBLCLK ||
	       event.message == WM_RBUTTONDOWN || event.message == WM_RBUTTONDBLCLK);
#endif

	PRECONDITION(clickCount > 0);

	mClickCount = clickCount;
}


//---------------------------------------------------------------
//
// XMouseDownEvent::IsRightButton
//
//---------------------------------------------------------------
#if MAC
bool XMouseDownEvent::IsRightButton() const		
{
	bool rightButton = false;
	
#if 0								// $$$ IsShowContextualMenuClick is flaky (with OS 8.5f5)
	if (MSystemInfo::HasContextMenus() && IsShowContextualMenuClick(&mEvent))
		rightButton = true;
	else
#endif
		rightButton = this->WasControlKeyDown();
			
	return rightButton;
}	
#endif

#if __MWERKS__
#pragma mark -
#endif

// ========================================================================================
//	class XMouseMoveEvent
// ========================================================================================

//---------------------------------------------------------------
//
// XMouseMoveEvent::~XMouseMoveEvent
//
//---------------------------------------------------------------
XMouseMoveEvent::~XMouseMoveEvent()
{
}


//---------------------------------------------------------------
//
// XMouseMoveEvent::XMouseMoveEvent ()
//
//---------------------------------------------------------------
XMouseMoveEvent::XMouseMoveEvent()
{
}


//---------------------------------------------------------------
//
// XMouseMoveEvent::XMouseMoveEvent (OSEvent)
//
//---------------------------------------------------------------
XMouseMoveEvent::XMouseMoveEvent(const OSEvent& event) : XMouseEvent(event)
{
#if MAC
	PRECONDITION(event.what == osEvt || event.what == mouseDown);
#elif WIN
	PRECONDITION(event.message == WM_MOUSEMOVE);
#endif
}

#if __MWERKS__
#pragma mark -
#endif

// ========================================================================================
//	class XMouseUpEvent
// ========================================================================================

//---------------------------------------------------------------
//
// XMouseUpEvent::~XMouseUpEvent
//
//---------------------------------------------------------------
XMouseUpEvent::~XMouseUpEvent()
{
}


//---------------------------------------------------------------
//
// XMouseUpEvent::XMouseUpEvent ()
//
//---------------------------------------------------------------
XMouseUpEvent::XMouseUpEvent()
{
}


//---------------------------------------------------------------
//
// XMouseUpEvent::XMouseUpEvent (OSEvent)
//
//---------------------------------------------------------------
XMouseUpEvent::XMouseUpEvent(const OSEvent& event) : XMouseEvent(event)
{
#if MAC
	PRECONDITION(event.what == mouseUp);
#elif WIN
	PRECONDITION(event.message == WM_LBUTTONUP || event.message == WM_MBUTTONUP || event.message == WM_RBUTTONUP);
#endif
}


}	// namespace Whisper

