/*
 *  File:       MDialogUtils.cpp
 *  Summary:   	Handy dialog manager related routines.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MDialogUtils.cpp,v $
 *		Revision 1.7  2001/05/14 10:53:20  jesjones
 *		HandleEvents traps exceptions.
 *		
 *		Revision 1.6  2001/04/27 04:19:54  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.5  2001/04/21 03:25:09  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/04/13 07:49:32  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2000/12/10 04:03:20  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:12:20  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 7/19/99	JDJ		Added MCreateModalFilterUPP. Tweaked for Carbon.
 *		 <3>	 6/11/99	JDJ		Tweaked for CW Pro 5fc3.
 *		 <2>	 5/27/99	JDJ		Removed SetUserProc, SetDashedLineProc, and SetDrawPictProc.
 *									Made sEventProcs thread safe. MDeativateDesktop uses an XAtomicCounter.
 *		 <1>	12/06/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <MDialogUtils.h>

#include <Appearance.h>
#include <Controls.h>
#include <Palettes.h>
#include <Scrap.h>
#include <Sound.h>

#include <cctype>

#include <MQuickDrawUtils.h>
#include <MSystemInfo.h>
#include <XConstants.h>
#include <XCriticalSection.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XMiscUtils.h>
#include <XNumbers.h>
#include <XStringUtils.h>

namespace Whisper {


//-----------------------------------
// Constants 
//
const int32 kMaxEventProcs = 16;


//-----------------------------------
// Static Variables
//
static EventProc sEventProcs[kMaxEventProcs];
static XCriticalSection sEventSection;


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// IsPushButton
//
//---------------------------------------------------------------
static bool IsPushButton(DialogPtr dialog, int16 item)
{
	PRECONDITION(dialog != nil);
	
	bool isBtn = false;
	
	if (item >= 1 && item <= CountDITL(dialog)) {
		int16	itemType;
		Handle	itemHandle;
		Rect	itemRect;
	
		GetDialogItem(dialog, item, &itemType, &itemHandle, &itemRect);
		if (itemType > itemDisable)
			 itemType -= itemDisable;
			 
		isBtn = itemType == ctrlItem + btnCtrl;
	}
		
	return isBtn;
}


//---------------------------------------------------------------
//
// DashedLineProc	
//
//---------------------------------------------------------------
static UserItemUPP sDashLineProc = nil;

static pascal void DashedLineProc(DialogPtr dialog, int16 item)
{
	XRect bounds = GetItemBounds(dialog, item);
	
	Pattern pat;
	GetIndPattern(&pat, sysPatListID, 4);
	
	Rect temp = bounds.GetOSRect();
	FillRect(&temp, &pat);
}


//---------------------------------------------------------------
//
// DrawPictureAt
//
// Draws specified picture at specified point without scaling.
//
//---------------------------------------------------------------
static void DrawPictureAt(PicHandle picHand, XPoint dstPoint)
{
	XRect dstRect;

	dstRect[topLeft] = dstPoint;
	dstRect[botRight] = dstPoint + XRect((**picHand).picFrame).GetSize();

	Rect temp = dstRect.GetOSRect();
	DrawPicture(picHand, &temp);
}


//---------------------------------------------------------------
//
// DrawDefaultOutline
//
//---------------------------------------------------------------
static void DrawDefaultOutline(DialogPtr dptr)
{
	int16 item = GetDialogDefaultItem(dptr);
	
	if (item > 0 && (GetItemType(dptr, item) & (ctrlItem+btnCtrl)) != 0) {
		PenState oldState;
		GetPenState(&oldState);
		
		PenNormal();
		PenSize(3, 3);
		PenMode(srcCopy);

		XRect bounds = GetItemBounds(dptr, item);
		bounds.Inset(-4, -4);
		
		if (!ItemIsEnabled(dptr, item))
			if (IsColorPort()) {
				RGBColor color = GetGrayColor();
				::RGBForeColor(&color);
				
			} else {
#if TARGET_CARBON
				Pattern pat;
				PenPat(GetQDGlobalsGray(&pat));
#else
				PenPat(&qd.gray);
#endif
			}
			
		int16 buttonOval = numeric_cast<int16>(2 + (bounds.bottom - bounds.top)/2);

		Rect temp = bounds.GetOSRect();
		FrameRoundRect(&temp, buttonOval, buttonOval);
			
		SetPenState(&oldState);
	}
}  

#pragma mark -

// ===================================================================================
//	class MDeativateDesktop
// ===================================================================================

XAtomicCounter				MDeativateDesktop::msNestCount      = 0;
MDeativateDesktop::Callback MDeativateDesktop::msActivateProc   = nil;
MDeativateDesktop::Callback MDeativateDesktop::msDeactivateProc = nil;

//---------------------------------------------------------------
//
// MDeativateDesktop::~MDeativateDesktop
//
//---------------------------------------------------------------
MDeativateDesktop::~MDeativateDesktop()				
{
	PRECONDITION(msNestCount > 0);
	
	--msNestCount;
	
	if (msNestCount == 0 && msActivateProc != nil) 
		(*msActivateProc)();
}
						

//---------------------------------------------------------------
//
// MDeativateDesktop::MDeativateDesktop
//
//---------------------------------------------------------------
MDeativateDesktop::MDeativateDesktop()					
{
	PRECONDITION(msNestCount >= 0);
	
	++msNestCount;
	
	if (msNestCount == 1 && msDeactivateProc != nil) 
		(*msDeactivateProc)();
}

#pragma mark -

// ===================================================================================
//	class MCreateModalFilterUPP
// ===================================================================================

//---------------------------------------------------------------
//
// MCreateModalFilterUPP::~MCreateModalFilterUPP
//
//---------------------------------------------------------------
MCreateModalFilterUPP::~MCreateModalFilterUPP()
{
	DisposeModalFilterUPP(mUPP);
}

						
//---------------------------------------------------------------
//
// MCreateModalFilterUPP::MCreateModalFilterUPP
//
//---------------------------------------------------------------
MCreateModalFilterUPP::MCreateModalFilterUPP(ModalFilterProcPtr proc)
{
	PRECONDITION(proc != nil);

	mUPP = NewModalFilterUPP(proc);
	ThrowIfNil(mUPP);
}

#pragma mark -

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// AddEventProc
//
//---------------------------------------------------------------
void AddEventProc(EventProc proc)
{
	PRECONDITION(proc != nil);
	
	XEnterCriticalSection enter(sEventSection);
	
	for (int32 index = 0; index < kMaxEventProcs; ++index) {
		if (sEventProcs[index] == nil) {
			sEventProcs[index] = proc;
			break;
		}
	}
}

				
//---------------------------------------------------------------
//
// HandleEvents
//
//---------------------------------------------------------------
pascal Boolean HandleEvents(EventRecord* event)
{
	PRECONDITION(event != nil);
	
	bool handled = false;
	
	try {
		XEnterCriticalSection enter(sEventSection);
		
		for (int32 index = 0; index < kMaxEventProcs && !handled; ++index) {
			if (sEventProcs[index] != nil)
				handled = (sEventProcs[index])(event);
		}
	
	} catch (...) {
		DEBUGSTR("Caught an exception in HandleEvents");
	}
	
	return handled;
}


//---------------------------------------------------------------
//
// RemoveEventProc
//
//---------------------------------------------------------------
void RemoveEventProc(EventProc proc)
{
	PRECONDITION(proc != nil);
	
	XEnterCriticalSection enter(sEventSection);
	
	for (int32 index = 0; index < kMaxEventProcs; ++index) {
		if (sEventProcs[index] == proc) {
			sEventProcs[index] = nil;
			break;
		}
	}
}

#pragma mark -

//---------------------------------------------------------------
//
// DefaultFilterProc
//
//---------------------------------------------------------------
pascal Boolean DefaultFilterProc(DialogPtr dptr, EventRecord* event, int16* item)
{
	bool handled = false;
	
	GrafPtr oldPort;
	GetPort(&oldPort);
	
	SetPortDialogPort(dptr);

	// Set the cursor to the ibeam if we're over an editBox.
	Point mouse;
	GetMouse(&mouse);
	int16 itemType = statText;
	int16 itemNum = numeric_cast<int16>(FindDialogItem(dptr, mouse) + 1);
	if (itemNum > 0)
		itemType = GetItemType(dptr, itemNum);

	Cursor arrow;
	SetCursor(itemType == editText ? *GetCursor(iBeamCursor) : GetQDGlobalsArrow(&arrow));

	int16 defaultItem = GetDialogDefaultItem(dptr);
	int16 cancelItem  = 2;								// GetDialogCancelItem isn't working...
		
	// If it's an update event draw an outline around the default button.
	if (event->what == updateEvt && defaultItem > 0)
		if ((DialogPtr) event->message == dptr)	
			DrawDefaultOutline(dptr);

	// Handle keystrokes.
	if (event->what == keyDown) {
		char ch   = (char) (event->message & charCodeMask);
		int16 key = (int16) ((event->message & keyCodeMask) >> 8);
		
		bool defaultEnabled = IsPushButton(dptr, defaultItem) && ItemIsEnabled(dptr, defaultItem);
		bool cancelEnabled  = IsPushButton(dptr, cancelItem) && ItemIsEnabled(dptr, cancelItem);
		
		// Map F2, F3, and F4 function keys to cmd X, C, and V.
		switch (key) {
			case kF2Key:
				event->modifiers |= cmdKey;
				key = 'x';
				break;
			
			case kF3Key:
				event->modifiers |= cmdKey;
				key = 'c';
				break;
			
			case kF4Key:
				event->modifiers |= cmdKey;
				key = 'v';
				break;
		}			
		
		// Handle cmd X, C, and V.
		if ((event->modifiers & cmdKey) != 0) {
			switch (std::toupper(ch)) {
				case 'X':
					ClearCurrentScrap();
					DialogCut(dptr);				// Note that these do nothing if the dialog has no edit box.
					if (TEToScrap() != noErr)
						SysBeep(1);
					handled = true;
					break;
										
				case 'C':
					ClearCurrentScrap();
					DialogCopy(dptr);
					if (TEToScrap() != noErr)
						SysBeep(1);
					handled = true;
					break;
										
				case 'V':
					if (TEFromScrap() == noErr)
						DialogPaste(dptr);
					handled = true;
					break;
			}	
			
		// Handle return and escape keys.
		} else if ((key == kReturnKey || ch == kEnterChar) && defaultEnabled) {
			*item = defaultItem;
			FlashButton(dptr, *item);
			handled = true;
		
		} else if ((ch == '~' || key == kEscapeKey) && cancelEnabled) { 
			*item = cancelItem;
			FlashButton(dptr, *item);
			handled = true;
		}
		
		// Handle command-period.
		if (!handled && cancelEnabled && IsCommandPeriod(*event)) {
			*item = cancelItem;
			FlashButton(dptr, *item);
			handled = true;
		}
	}
	
	SetPort(oldPort);

	return handled;
}


//---------------------------------------------------------------
//
// DefaultFilter
//
//---------------------------------------------------------------
pascal Boolean DefaultFilter(DialogPtr dptr, EventRecord* event, int16* item)
{	
	PRECONDITION(MDeativateDesktop::IsDeactivated());

	GrafPtr oPort;
	GetPort(&oPort);
	
	Boolean handled = Whisper::DefaultFilterProc(dptr, event, item);
	if (!handled)
		handled = Whisper::HandleEvents(event);
		
	SetPort(oPort);
	
	return handled;
}


//---------------------------------------------------------------
//
// SaveChangesFilter
//
//---------------------------------------------------------------
pascal Boolean SaveChangesFilter(DialogPtr dptr, EventRecord* event, int16* item)
{
	bool handled = false;
		
	if (event->what == keyDown) {
		char ch = (char) std::toupper((char) (event->message & charCodeMask));
		if (ch == 'S') { 
			*item = 1;
			handled = true;
		} else if (ch == 'D') { 
			*item = 3;
			handled = true;
		}
		
		if (handled) 
			FlashButton(dptr, *item);
	}
	
	if (!handled)
		handled = DefaultFilter(dptr, event, item);
	
	return handled;
}

//---------------------------------------------------------------
//
// DefaultYDFilter
//
//---------------------------------------------------------------
pascal Boolean DefaultYDFilter(DialogPtr dptr, EventRecord* event, int16* item, void* myData)
{
	#pragma unused(myData)
		
	GrafPtr oPort;
	GetPort(&oPort);
	
	Boolean handled = DefaultFilter(dptr, event, item);
	if (!handled)
		handled = HandleEvents(event);

	SetPort(oPort);
	
	return handled;
}

#pragma mark -

//---------------------------------------------------------------
//
// DoAlert
//
//---------------------------------------------------------------
int16 DoAlert(ResID id, ModalFilterProcPtr filter)
{
	MCreateModalFilterUPP theFilter(filter);

	InitCursor();
	MDeativateDesktop deactivate;

	int16 item = Alert(id, theFilter);
	
	return item;
}


//---------------------------------------------------------------
//
// DoMessageAlert
//
// $$$ Use notification manager if in background?
//
//---------------------------------------------------------------
void DoMessageAlert(AlertType type, const std::wstring& errorStr, const std::wstring& supplementalStr, ResID id, ModalFilterProcPtr filter)
{
	PRECONDITION(filter != nil);
	 	
	std::wstring text = errorStr + L"\r\r" + supplementalStr;

	InitCursor();
	MDeativateDesktop deactivate;

	MCreateModalFilterUPP theFilter(filter);

	if (MSystemInfo::HasAppearanceMgr()) {
		AlertStdAlertParamRec params;
		params.movable       = true;
		params.helpButton    = false;
		params.filterProc    = theFilter;
		params.filterProc    = nil;
		params.defaultText   = (StringPtr) -1L;		// use default (ie "OK")
		params.cancelText    = nil;
		params.otherText     = nil;
		params.defaultButton = 1;
		params.cancelButton  = 0;
		params.position      = kWindowAlertPositionParentWindowScreen;

		int16 item = ok;
		OSErr err = StandardAlert(type, (unsigned char*) ToPascalStr(errorStr).c_str(), (unsigned char*) ToPascalStr(supplementalStr).c_str(), &params, &item);
		ASSERT(err == noErr);						// seems kind of pointless to throw

	} else {
		ParamText(ToPascalStr(text).c_str(), "\p", "\p", "\p");

		if (type == kAlertStopAlert)
			(void) StopAlert(id, theFilter);
			
		else if (type == kAlertNoteAlert)
			(void) NoteAlert(id, theFilter);
			
		else 
			(void) CautionAlert(id, theFilter);
	}
}


//---------------------------------------------------------------
//
// DoConfirmAlert
//
// $$$ Use notification manager if in background?
//
//---------------------------------------------------------------
bool DoConfirmAlert(AlertType type, const std::wstring& errorStr, const std::wstring& supplementalStr, ResID id, ModalFilterProcPtr filter)
{
	PRECONDITION(filter != nil);
	 
	int16 item = cancel;
	
	std::wstring text = errorStr + L"\r\r" + supplementalStr;

	InitCursor();
	MDeativateDesktop deactivate;

	MCreateModalFilterUPP theFilter(filter);

	if (MSystemInfo::HasAppearanceMgr()) {
		AlertStdAlertParamRec params;
		params.movable       = true;
		params.helpButton    = false;
		params.filterProc    = theFilter;
		params.defaultText   = (StringPtr) -1L;		// use default (ie "OK")
		params.cancelText    = (StringPtr) -1L;		// use default (ie "Cancel")
		params.otherText     = nil;
		params.defaultButton = cancel;
		params.cancelButton  = 0;
		params.position      = kWindowAlertPositionParentWindowScreen;

		OSErr err = StandardAlert(type, (unsigned char*) ToPascalStr(errorStr).c_str(), (unsigned char*) ToPascalStr(supplementalStr).c_str(), &params, &item);
		ASSERT(err == noErr);						// seems kind of pointless to throw

	} else {
		ParamText(ToPascalStr(text).c_str(), "\p", "\p", "\p");

		if (type == kAlertStopAlert)
			item = StopAlert(id, theFilter);
			
		else if (type == kAlertNoteAlert)
			item = NoteAlert(id, theFilter);
			
		else 
			item = CautionAlert(id, theFilter);
	}
			
	return item == ok;
}

#pragma mark -

//---------------------------------------------------------------
//
// GetItemBounds
//
//---------------------------------------------------------------
XRect GetItemBounds(DialogPtr dialog, int16 item)
{
	int16	itemType;
	Handle	itemHandle;
	Rect	itemRect;

	GetDialogItem(dialog, item, &itemType, &itemHandle, &itemRect);
	
	return itemRect;
}


//---------------------------------------------------------------
//
// GetItemType
//
//---------------------------------------------------------------
int16 GetItemType(DialogPtr dialog, int16 item)
{
	int16	itemType;
	Handle	itemHandle;
	Rect	itemRect;

	GetDialogItem(dialog, item, &itemType, &itemHandle, &itemRect);
	
	return itemType;
}


//---------------------------------------------------------------
//
// ItemIsEnabled
//
//---------------------------------------------------------------
bool ItemIsEnabled(DialogPtr dialog, int16 item)
{
	int16	itemType;
	Handle	itemHandle;
	Rect	itemRect;

	GetDialogItem(dialog, item, &itemType, &itemHandle, &itemRect);
	
	return (itemType & itemDisable) == false;
}


//---------------------------------------------------------------
//
// GetControl
//
//---------------------------------------------------------------
ControlHandle GetControl(DialogPtr dialog, int16 item)
{
	PRECONDITION(dialog != nil);
	PRECONDITION(item >= 1 && item <= CountDITL(dialog));

	int16	itemType;
	Handle	itemHandle;
	Rect	itemRect;

	GetDialogItem(dialog, item, &itemType, &itemHandle, &itemRect);
	if (itemType > itemDisable)
		 itemType -= itemDisable;
	ASSERT(itemType >= ctrlItem + btnCtrl && itemType <= ctrlItem + resCtrl);
	
	return (ControlHandle) itemHandle;
}


//---------------------------------------------------------------
//
// GetDStr
//
//---------------------------------------------------------------
std::wstring GetDStr(DialogPtr dialog, int16 item)
{
	PRECONDITION(dialog != nil);
	PRECONDITION(item >= 1 && item <= CountDITL(dialog));

	int16	itemType;
	Handle	itemHandle;
	Rect	itemRect;
	Str255	itemString;

	GetDialogItem(dialog, item, &itemType, &itemHandle, &itemRect);
	if (itemType > itemDisable)
		 itemType -= itemDisable;
	ASSERT(itemType == statText || itemType == editText);
	GetDialogItemText(itemHandle, itemString);
	
	return FromPascalStr(itemString);
}

#pragma mark -

//---------------------------------------------------------------
//
// SetDStr
//
//---------------------------------------------------------------
void SetDStr(DialogPtr dialog, int16 item, const std::wstring& str)
{
	PRECONDITION(dialog != nil);
	PRECONDITION(item >= 1 && item <= CountDITL(dialog));
	
	int16	itemType;
	Handle	itemHandle;
	Rect	itemRect;

	GetDialogItem(dialog, item, &itemType, &itemHandle, &itemRect);
	if (itemType > itemDisable)
		 itemType -= itemDisable;
	ASSERT(itemType == statText || itemType == editText);
	SetDialogItemText(itemHandle, ToPascalStr(str).c_str());
}


//---------------------------------------------------------------
//
// FlashButton
//
//---------------------------------------------------------------
void FlashButton(DialogPtr dialog, int16 item)
{
	PRECONDITION(dialog != nil);
	PRECONDITION(item >= 1 && item <= CountDITL(dialog));
	PRECONDITION(IsPushButton(dialog, item));
	
	uint32 ticks;
	
	ControlHandle control = GetControl(dialog, item);
	HiliteControl(control, kControlEntireControl);
	Delay(8, &ticks);
	HiliteControl(control, 0);
}


}	// namespace Whisper

