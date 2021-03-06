/*
 *  File:       MBootStrap.cpp
 *  Summary:   	Functions used to intialize and terminate UI apps.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MBootStrap.cpp,v $
 *		Revision 1.9  2001/10/04 06:12:11  jesjones
 *		DefaultSystemCheck no longer requires System 7 (Whisper is Carbon only).
 *		
 *		Revision 1.8  2001/04/27 04:18:57  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.7  2001/04/21 03:24:02  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/03/06 07:31:22  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.5  2001/02/12 09:38:51  jesjones
 *		Disabled leak checking for DLL targets.
 *		
 *		Revision 1.4  2001/01/05 06:18:30  jesjones
 *		Big time refactoring.
 *		
 *		Revision 1.3  2000/12/10 04:02:33  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:10:11  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <8>	11/20/99	JDJ		Calls to _prealloc_newpool and co are now properly scoped.
 *		 <7>	11/16/99	JDJ		Added DoInitObjectHeap (for multi-fragment apps).
 *		 <6>	10/31/99	JDJ		Moved FragInit and FragTerm into MAppStartup.cpp
 *		 <5>	 7/20/99	JDJ		Tweaked for Carbon.
 *		 <4>	 6/19/99	JDJ		FragInit calls __initialize if MULTI_FRAGMENT_APP is 1. FragInit only calls InitializeToolbox if
 *									APP_TARGET is non-zero.
 *		 <3>	 3/22/99	JDJ		InitializeToolbox no longer calls InitFloatingWindows
 *									if running under 8.5.
 *		 <2>	 1/05/99	JDJ		FragTerm calls __terminate.
 *		 <1>	 9/20/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XBootStrap.h>

#include <Appearance.h>
#include <Sioux.h>

#include <MQuickDrawUtils.h>
#include <MSystemInfo.h>
#include <XDebugMalloc.h>
#include <XDebugNew.h>
#include <XDesktop.h>
#include <XNumbers.h>
#include <XObjectModel.h>
#include <XStringUtils.h>
#include <XTranscode.h>

namespace Whisper {

extern bool gExitingNormally;


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// DoPositionSioux
//
// Moves the SIOUX window to the bottom right corner of the specified
// device. The window will cover width/height percent of the device.
//
//---------------------------------------------------------------
static void DoPositionSioux(GDHandle device, int32 width, int32 height)
{
	PRECONDITION(device != nil);
	PRECONDITION(width > 0 && width <= 100);
	PRECONDITION(height > 0 && height <= 100);
	
	// Find out what the SIOUX window character width and height will be.
	int charWidth, charHite;
	{
	MSetPort       setPort(GetWorkPort());
	MSaveTextState saveText();

		TextFont(SIOUXSettings.fontid);
		TextSize(SIOUXSettings.fontsize);
		TextFace(SIOUXSettings.fontface);
		{
			FontInfo	fontInfo;
			GetFontInfo(&fontInfo);
			charWidth = fontInfo.widMax;
			charHite = fontInfo.ascent + fontInfo.descent + fontInfo.leading;
		}
	}

	// Find out where the window will be positioned.
	XRect screen = (**device).gdRect;
	
	SIOUXSettings.leftpixel = numeric_cast<int16>(screen.right - MulDiv(screen.GetWidth(), width, 100));
	SIOUXSettings.toppixel  = numeric_cast<int16>(screen.bottom - MulDiv(screen.GetHeight(), height, 100));
			
	// Leave room for the menubar and the window's title bar.
	int16 padding = numeric_cast<int16>(20 + (device == GetMainDevice() ? GetMBarHeight() : 0));
	if (SIOUXSettings.toppixel - screen.top < padding)
		SIOUXSettings.toppixel += padding;
	
	// Figure out how many rows and columns the window will have.
	SIOUXSettings.columns = numeric_cast<int16>((screen.right - SIOUXSettings.leftpixel - 23)/charWidth - 1);
	SIOUXSettings.rows    = numeric_cast<int16>((screen.bottom - SIOUXSettings.toppixel - 23)/charHite - 1);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// InitUI
//
//---------------------------------------------------------------
void InitUI()
{
	gExitingNormally = false;
	
#if !MULTI_FRAGMENT_APP && WHISPER_OPERATOR_NEW && DEBUG // $$$ get rid of MULTI_FRAGMENT_APP
	gDebugMalloc->EnableLeakChecking();		// we don't leak check static ctors
#endif

	InitCursor();
	FlushEvents(everyEvent, 0);
		
	if (MSystemInfo::HasAppearanceMgr())
		RegisterAppearanceClient();

	if (MSystemInfo::HasContextMenus()) {
		OSStatus err = InitContextualMenus();
		if (err != noErr) {
			MSystemInfo::SetHasContextMenus(false);
			ASSERT(err == noErr);
		}
	}
}


//---------------------------------------------------------------
//
// ShutDownUI
//
//---------------------------------------------------------------
void ShutDownUI()
{
	gExitingNormally = true;

	XObjectModel::Instance()->Shutdown();
}


//---------------------------------------------------------------
//
// DefaultSystemCheck
//
//---------------------------------------------------------------
void DefaultSystemCheck(std::list<std::wstring>& needs)
{
	if (!MSystemInfo::HasColorQuickDraw())
		needs.push_back(LoadWhisperString(L"Color QuickDraw"));
		
	if (!MSystemInfo::HasAppearanceMgr() || MSystemInfo::GetAppearanceVersion() < 0x0101)
		needs.push_back(LoadWhisperString(L"Appearance Manager 1.0.1"));
}


//---------------------------------------------------------------
//
// HandleBadSystem
//
//---------------------------------------------------------------
void HandleBadSystem(const std::list<std::wstring>& needs)
{
	PRECONDITION(!needs.empty());
	
	// Build a string containing all of the reasons the app won't run.
	std::wstring mesg;
	if (needs.size() == 1)
		mesg += needs.front();
	
	else if (needs.size() == 2)
		mesg += needs.front() + LoadWhisperString(L" and ") + needs.back();
	
	else {
		std::list<std::wstring>::const_iterator iter = needs.begin();
		while (iter != needs.end()) {
			std::wstring item = *iter++;
			
			if (iter == needs.end())
				mesg += LoadWhisperString(L"and ");
			mesg += item;
			if (iter != needs.end())
				mesg += LoadWhisperString(L", ");
		}
	}

	// Put up an alert to let the user know how badly their Mac sucks.
	InitCursor();

	if (MSystemInfo::HasAppearanceMgr()) {
		std::wstring defaultText = LoadWhisperString(L"Quit Button");	// local so it sticks around until StandardAlert exits
		PascalString str = ToPascalStr(defaultText);
		
		AlertStdAlertParamRec params;
		params.movable       = false;
		params.helpButton    = false;
		params.filterProc    = nil;
		params.defaultText   = (unsigned char*) str.c_str();
		params.cancelText    = nil;
		params.otherText     = nil;
		params.defaultButton = ok;
		params.cancelButton  = 0;
		params.position      = kWindowDefaultPosition;
		
		std::wstring errorStr = LoadWhisperString(L"Unable to run the app because the system components below are missing:");

		short item;
		OSErr err = StandardAlert(kAlertStopAlert, (unsigned char*) ToPascalStr(errorStr).c_str(), (unsigned char*) ToPascalStr(mesg).c_str(), &params, &item);
		ASSERT(err == noErr);						// seems kind of pointless to throw
	
	} else {
		ParamText(ToPascalStr(mesg).c_str(), "\p", "\p", "\p");
		(void) ::Alert(129, nil);
	}
	
	// Bail
	::ExitToShell();
}

				
//---------------------------------------------------------------
//
// InitSIOUX
//
//---------------------------------------------------------------
#if MAC && DEBUG
void InitSIOUX()
{
	SIOUXSettings.initializeTB     = false;
	SIOUXSettings.standalone       = false;
	SIOUXSettings.setupmenus       = false;
	SIOUXSettings.asktosaveonclose = false;
	
	// Place the SIOUX window on the largest secondary monitor.
	XDisplay display = XDesktop::GetMainDisplay();
	
	XDesktop::iterator iter = XDesktop::begin();
	while (iter != XDesktop::end()) {
		XDisplay candidate = *iter;
		++iter;
		
		if (candidate != XDesktop::GetMainDisplay()) {
			if (display == XDesktop::GetMainDisplay())
				display = candidate;
			else if (candidate.GetResolution().GetArea() > display.GetResolution().GetArea())
				display = candidate;
		}

		if (display.GetResolution().width > 640)
			DoPositionSioux(display, 45, 100);		// use right 45% of device
		else
			DoPositionSioux(display, 33, 100);		// use right third of device
	}
}
#endif


}	// namespace Whisper

