/*
 *  File:       MacApplication.cpp
 *  Summary:    Main entry point and Mac event handling code.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: MacApplication.cpp,v $
 *		Revision 1.5  2001/04/14 07:34:37  jesjones
 *		Ported to Carbon.
 *		
 *		Revision 1.4  2001/04/13 07:48:16  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2001/03/17 08:56:20  jesjones
 *		Added std:: to one spot.
 *		
 *		Revision 1.2  2000/11/09 09:08:35  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>

#include <X3DUtils.h>
#include <XError.h>
#include <XTrace.h>
#include <XTraceSinks.h>

#include "CRollerCoaster.h"

using namespace Whisper;

namespace Whisper {
	extern bool gExitingNormally;
}


//-----------------------------------
//	Internal Variables
//
static CRollerCoaster* sCoaster;


// ========================================================================================
//	Internal Functions
// ========================================================================================

//----------------------------------------------------------------
//
// InitToolbox	
//
//----------------------------------------------------------------
static void InitToolbox()
{
	MoreMasterPointers(3);

	FlushEvents(everyEvent, 0);
}


//----------------------------------------------------------------
//
// HandleKeyPress	
//
//----------------------------------------------------------------
static bool HandleKeyPress(const EventRecord* event)
{
	char ch = (char) (event->message & charCodeMask);
	bool quitFlag = ch == 'Q' || ch == 'q';
	
	return quitFlag;
}


//----------------------------------------------------------------
//
// MainEventLoop	
//
//----------------------------------------------------------------
static void MainEventLoop()
{
	EventRecord 	event;
	WindowPtr   	window;
	int16       	thePart;
	Rect        	screenRect;
	Point			aPoint = {100, 100};
	bool	 		quitFlag = false;

	while (!quitFlag) {
		if (WaitNextEvent(everyEvent, &event, 0, nil)) {
			switch (event.what) {
				case mouseDown:
					thePart = FindWindow(event.where, &window);
					
					switch(thePart) {
						case inMenuBar: 
							break;
						
						case inDrag:
							GetRegionBounds(GetGrayRgn(), &screenRect);
							DragWindow(window, event.where, &screenRect);
							break;
					
						case inContent:
							if (window != FrontWindow())
								SelectWindow(window);
							break;
					
						case inGoAway:
							if (TrackGoAway(window, event.where)) {
								DisposeWindow (window);
								quitFlag = true;
							}
							break;
							
						default:
							break;
					}
					break;
							
				case updateEvt:	
					window = (WindowPtr)event.message; 
					sCoaster->Update();
					break;
					
				case keyDown:
				case autoKey:
					quitFlag = HandleKeyPress(&event);
					if (quitFlag)
						DisposeWindow (window);
					break;
										
				case osEvt:
				case activateEvt:
					break;
			}

		} else {
			// we received a null event 
			sCoaster->Invalidate();
		}
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ========================================================================================
//	Main Entry Point
// ========================================================================================
//----------------------------------------------------------------
//
// main	
//
//----------------------------------------------------------------
void main()
{
	try {
		gExitingNormally = false;

		InitToolbox();
		Init3D();
	
#if DEBUG
		XDebuggerSink sink;
		XTrace::Instance()->AddSink(&sink);
#endif

		sCoaster = new CRollerCoaster;

		MainEventLoop();
		
#if DEBUG
		XTrace::Instance()->RemoveSink(&sink);
#endif

		delete sCoaster;
		Terminate3D();

		gExitingNormally = true;
		
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(L"Fatal Error", e);
	
	} catch (...) {
		XError::Instance()->ReportError(L"Fatal Error", L"Unknown error");
	}
}


