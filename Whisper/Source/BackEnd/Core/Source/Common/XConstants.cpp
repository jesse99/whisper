/*
 *  File:       XConstants.cpp
 *  Summary:   	Various constants.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XConstants.cpp,v $
 *		Revision 1.2  2000/11/09 12:28:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <3>	 9/03/00	JDJ		Added kPrefsCmd. Renamed kSaveCopyAsCmd kSaveCopyCmd.
 *		 <2>	 4/02/99	JDJ		Removed kNothingMessage, kOKMessage, and kCancelMessage.
 *		 <1>	11/02/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XConstants.h>

#if MULTI_FRAGMENT_APP && MAC && 0
	namespace std {
		basic_string<char, char_traits<char>, allocator<char> >::CharArray basic_string<char, char_traits<char>, allocator<char> >::null_;
	}
#endif

namespace Whisper {


// ===================================================================================
//	Menu Commands
// ===================================================================================
const XMenuCommand kNothingCmd			= L"nothing";
const XMenuCommand kAbortCmd			= L"abort";				// Command-period/Control-Break
const XMenuCommand kHelpCmd				= L"help";

const XMenuCommand kAboutCmd			= L"about";				// About Menu
#if MAC
	const XMenuCommand kDACmd			= L"da";		
#endif

const XMenuCommand kNewCmd				= L"new";				// File Menu
const XMenuCommand kOpenCmd				= L"open";
const XMenuCommand kCloseCmd			= L"close";
const XMenuCommand kSaveCmd				= L"save";
const XMenuCommand kSaveAsCmd			= L"save as";
const XMenuCommand kRevertCmd			= L"revert";
const XMenuCommand kPageSetupCmd		= L"page setup";
const XMenuCommand kPrintCmd			= L"print";
const XMenuCommand kPrintOneCmd			= L"print one";
const XMenuCommand kQuitCmd				= L"quit";
const XMenuCommand kSaveCopyCmd			= L"save copy";

const XMenuCommand kUndoCmd				= L"undo"; 				// Edit Menu
const XMenuCommand kRedoCmd				= L"redo"; 	
const XMenuCommand kCutCmd				= L"cut";
const XMenuCommand kCopyCmd				= L"copy";
const XMenuCommand kPasteCmd			= L"paste";
const XMenuCommand kClearCmd			= L"clear";
const XMenuCommand kSelectAllCmd		= L"select all";
const XMenuCommand kPrefsCmd			= L"preferences";
const XMenuCommand kShowClipboardCmd	= L"show clipboard";


}	// namespace Whisper

