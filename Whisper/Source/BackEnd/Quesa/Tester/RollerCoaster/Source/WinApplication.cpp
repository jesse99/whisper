/*
 *  File:       WinApplication.cpp
 *  Summary:    Main entry point and Win event handling code.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: WinApplication.cpp,v $
 *		Revision 1.2  2000/11/09 12:09:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/15/00	JDJ		Created (from the QD3D 1.6 RollerCoasterSampleMac sample app)
 */

#include <XWhisperHeader.h>

#include <QTML.h>

#include <WSystemInfo.h>
#include <X3DUtils.h>
#include <XError.h>
#include <XExceptions.h>
#include <XTrace.h>
#include <XTraceSinks.h>

#include "CRollerCoaster.h"

using namespace Whisper;


//-----------------------------------
//	Globals
//
static uint32 sTimer;
static CRollerCoaster* sCoaster;


//-----------------------------------
//	Constants
//
const int32 kMillsPerTick = 10;  // milliseconds per timer tick 


// ========================================================================================
//	Internal Functions
// ========================================================================================

//----------------------------------------------------------------
//
// TimerProc	
//
//----------------------------------------------------------------
static VOID CALLBACK TimerProc(HWND, uint32, uint32, uint32)	
{
	sCoaster->Invalidate();
}


//----------------------------------------------------------------
//
// StartTimer	
//
//----------------------------------------------------------------
static void StartTimer()
{
	sTimer = SetTimer(nil, 0, kMillsPerTick, (TIMERPROC) TimerProc);
}


//----------------------------------------------------------------
//
// StopTimer	
//
//----------------------------------------------------------------
static void StopTimer()
{
	KillTimer(nil, sTimer);
}

#if __MWERKS__
#pragma mark -
#endif

// ========================================================================================
//	Main Entry Point
// ========================================================================================

//----------------------------------------------------------------
//
// WinMain	
//
//----------------------------------------------------------------
int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	MSG msg;

	try {
		WSystemInfo::SetPrefsName(L"SOFTWARE\\Whisper\\Roller Coaster\\1.0");

#if DEBUG
		XDebuggerSink sink;
		XTrace::Instance()->AddSink(&sink);
#endif

		OSErr err = InitializeQTML(0);
		ThrowIf(err != noErr);
		
		Init3D();
		sCoaster = new CRollerCoaster;

		StartTimer();
		
		while (GetMessage(&msg, nil, 0, 0)) {
			if (!TranslateAccelerator (msg.hwnd, nil, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		
		StopTimer();
		
		delete sCoaster;
		Terminate3D();
	
#if DEBUG
		XTrace::Instance()->RemoveSink(&sink);
#endif
		
	} catch (const exception& e) {
		XError::Instance()->ReportError(L"Fatal Error", e);
	
	} catch (...) {
		XError::Instance()->ReportError(L"Fatal Error", L"Unknown error");
	}
	
	return (int) msg.wParam; // Returns the value from PostQuitMessage
}


