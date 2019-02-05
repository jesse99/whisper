/*
 *  File:       MStartup.h
 *  Summary:   	Initialization and termination routines for the app.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MStartup.cpp,v $
 *		Revision 1.5  2001/04/13 07:53:28  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.4  2001/02/12 10:35:19  jesjones
 *		Fixed the FragInit prototype.
 *		
 *		Revision 1.3  2000/11/10 06:47:28  jesjones
 *		gExitingNormally defaults to false instead of true.
 *		
 *		Revision 1.2  2000/11/09 09:20:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>

#include <Appearance.h>
#include <CodeFragments.h>
#include <Dialogs.h>
#include <Fonts.h>
#include <List>
#include <TextEdit.h>

#include <XDebug.h>
#include <XDebugMalloc.h>
#include <XDebugNew.h>

extern pascal OSErr __initialize(const CFragInitBlock* block);
extern pascal void __terminate();

namespace Whisper {

bool gInitedApp = false;
bool gExitingNormally = false;


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetSystemVersion
//
//---------------------------------------------------------------
static uint32 GetSystemVersion()
{
	uint32	version = 0;
	
	int32 result;
	if (Gestalt(gestaltSystemVersion, &result) == noErr)
		version = (uint32) (result & 0x0FFF);
			
	return version;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Entry Points
// ===================================================================================

//---------------------------------------------------------------
//
// FragInit
//
// Entry point for CFM apps (or the Runtime shared library). 
//
//---------------------------------------------------------------
#pragma profile off
extern "C" OSErr FragInit(const CFragInitBlock*);
OSErr FragInit(const CFragInitBlock* block)
{		
	(void) block;						
	
	OSStatus err = noErr;
			
	MoreMasterPointers(128);
			
#if MULTI_FRAGMENT_APP
	err = __initialize(block);			// if an exception is thrown by a static ctor the app will terminate	
#endif
	gInitedApp = err == noErr;

	return (OSErr) err;
}
#pragma profile reset


//---------------------------------------------------------------
//
// FragTerm
//
// Exit point for CFM apps (or the Runtime shared library).
//
//---------------------------------------------------------------
#pragma profile off
extern "C" void FragTerm();
void FragTerm()
{		
	if (gExitingNormally) {			// don't bother with leaks if we're terminating abnormally	
#if DEBUG && !MULTI_FRAGMENT_APP	// $$$ enable this for multi-fragment apps?
#if WHISPER_OPERATOR_NEW
		uint32 leaks = gDebugMalloc->GetLeakCount();
		if (leaks > 0)
			gDebugMalloc->DumpLeaks();
	
#elif DEBUG_NEW > DEBUG_NEW_BASIC	
		uint32 leaks = gDebugNewAllocCount;
		if (leaks > 0)
			DebugNewReportLeaks();
#endif
		
		if (leaks == 1)
			DEBUGSTR("You have one memory leak: check the 'Leaks Log.txt' file.");
	
		else if (leaks > 0) {
			char buffer[1024];
			std::sprintf(buffer, "You have %d memory leaks: check the 'Leaks Log.txt' file.", leaks);
			DEBUGSTR(buffer);			// can't use iostreams at static dtor time...
		}
#endif

		__terminate();						

	} 
//	else
//		MExitActionMixIn::DoExitActions();	// terminating abnormally so call exit actions
}
#pragma profile reset


}	// namespace Whisper

