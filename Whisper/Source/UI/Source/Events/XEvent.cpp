/*
 *  File:       XEvent.cpp
 *  Summary:   	Cross platform wrapper around an OS Event.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XEvent.cpp,v $
 *		Revision 1.2  2000/11/09 12:34:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	10/08/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XEvent.h>

#include <XMiscUtils.h>
#include <XNumbers.h>

namespace Whisper {


// ===================================================================================
//	class XEvent
// ===================================================================================

//---------------------------------------------------------------
//
// XEvent::~XEvent
//
//---------------------------------------------------------------
XEvent::~XEvent()
{
}


//---------------------------------------------------------------
//
// XEvent::XEvent ()
//
//---------------------------------------------------------------
XEvent::XEvent()
{
#if MAC
	mEvent.what      = nullEvent;
	mEvent.message   = 0;
	mEvent.when      = TickCount();
	mEvent.where.h   = 0;
	mEvent.where.v   = 0;
	mEvent.modifiers = 0;

	mTimeStamp = GetMilliSeconds();
	
#elif WIN
	mEvent.hwnd    = nil;	   
   	mEvent.message = WM_NULL;
   	mEvent.wParam  = 0;
   	mEvent.lParam  = 0;
   	mEvent.time    = (uint32) GetMilliSeconds();
   	mEvent.pt.x    = 0;
   	mEvent.pt.y    = 0;
    
	mTimeStamp = (int32) mEvent.time;
#endif
}


//---------------------------------------------------------------
//
// XEvent::XEvent (OSEvent)
//
//---------------------------------------------------------------
XEvent::XEvent(const OSEvent& event)
{
	mEvent = event;
	
#if MAC
	mGlobalPos = event.where;
#elif WIN
	mGlobalPos = event.pt;
#endif

	mTimeStamp = GetMilliSeconds();
}


}	// namespace Whisper

