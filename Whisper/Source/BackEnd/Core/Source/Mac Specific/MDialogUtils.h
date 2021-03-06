/*
 *  File:       MDialogUtils.h
 *  Summary:   	Handy dialog manager related routines.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MDialogUtils.h,v $
 *		Revision 1.4  2001/04/13 07:49:37  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2000/12/10 08:48:30  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:12:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Dialogs.h>
#include <Resources.h>

#include <XAtomicCounter.h>
#include <XRect.h>
#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Constants
//
const ResID kMessageAlert   = 130;
const ResID kConfirmAlert	= 134;


//-----------------------------------
//	Types
//
typedef bool (*EventProc) (EventRecord* event);


// ===================================================================================
//	class MDeativateDesktop
//!		You should create one of these whenever you popup a native Mac dialog.
// ===================================================================================
class MDeativateDesktop {

//-----------------------------------
//	Types
//
public:
	typedef void (*Callback)();
	
//-----------------------------------
//	Initialization/Destruction
//
public:
						~MDeativateDesktop();
						
						MDeativateDesktop();
						
private:
						MDeativateDesktop(const MDeativateDesktop& rhs);
						
			MDeativateDesktop& operator=(const MDeativateDesktop& rhs);
						
//-----------------------------------
//	Internal API
//
public:
	static	bool 		IsDeactivated()						{return msNestCount > 0;}
			
	static	void 		SetActivateProc(Callback proc)		{msActivateProc = proc;}
						/**< MWindowMgr calls this. */

	static	void 		SetDeactivateProc(Callback proc)	{msDeactivateProc = proc;}
						/**< MWindowMgr calls this. */

//-----------------------------------
//	Member Data
//
protected:
	static XAtomicCounter	msNestCount;
	static Callback			msActivateProc;
	static Callback 		msDeactivateProc;
};


// ===================================================================================
//	class MCreateModalFilterUPP
//!		Efficient, exception safe class for creating modal dialog filter UPPs.
// ===================================================================================
class MCreateModalFilterUPP {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~MCreateModalFilterUPP();
						
						MCreateModalFilterUPP(ModalFilterProcPtr proc);
						
						operator ModalFilterUPP() const					{return mUPP;}

//-----------------------------------
//	Member Data
//
protected:
	ModalFilterUPP		mUPP;
};


// ===================================================================================
//	Event Procs
// ===================================================================================
void			AddEventProc(EventProc proc);
				// Adds a procedure that can respond to events. Called by higher level 
				// components of Whisper to, for example, allow windows to be updated 
				// as a movable modal is dragged.
				
pascal Boolean 	HandleEvents(EventRecord* event);
				// Runs through all the event procs until one handles the event.

void			RemoveEventProc(EventProc proc);


// ===================================================================================
//	Dialog Filter Procs
// ===================================================================================
pascal Boolean 	DefaultFilter(DialogPtr dptr, EventRecord* event, int16* item);	
				// Default dialog filter. Calls DefaultFilterProc and then event procs.

pascal Boolean 	DefaultYDFilter(DialogPtr dptr, EventRecord* event, int16* item, void* myData);	
				// Like DefaultFilter except for the extra argument.

pascal Boolean 	SaveChangesFilter(DialogPtr dptr, EventRecord* event, int16* item);
				// Like DefaultFilter except 'S' and 'D' keys are special cased.

pascal Boolean 	DefaultFilterProc(DialogPtr dptr, EventRecord* event, int16* item);	
				// The dialog filter DefaultFilter starts with. Outlines the default
				// button, handles cut/copy/paste, changes the cursor to an i-beam,
				// and handles return/escape/etc.
				
	
// ===================================================================================
//	Alerts
// ===================================================================================
int16 		DoAlert(ResID id, ModalFilterProcPtr filter = DefaultFilter);

void 		DoMessageAlert(AlertType type, const std::wstring& mainStr, const std::wstring& supplementalStr, ResID id = kMessageAlert, ModalFilterProcPtr filter = DefaultFilter);
			// Displays an alert using StandardAlert (if the Appearance Manager is
			// available) or the Dialog Manager using the specified id.
			
bool 		DoConfirmAlert(AlertType type, const std::wstring& mainStr, const std::wstring& supplementalStr, ResID id = kConfirmAlert, ModalFilterProcPtr filter = DefaultFilter);
			// Displays an alert with a cancel button. Returns true if OK was pressed.
					
			
// ===================================================================================
//	Accessors
// ===================================================================================
XRect 		GetItemBounds(DialogPtr dialog, int16 item);
			
int16 		GetItemType(DialogPtr dialog, int16 item);
			
bool 		ItemIsEnabled(DialogPtr dialog, int16 item);

ControlHandle GetControl(DialogPtr dialog, int16 item);

std::wstring GetDStr(DialogPtr dialog, int16 item);


// ===================================================================================
//	Misc
// ===================================================================================
void 		SetDStr(DialogPtr dialog, int16 item, const std::wstring& str);
			
void 		FlashButton(DialogPtr dialog, int16 item);
			// Call this when a button is pressed via a keystroke.


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

