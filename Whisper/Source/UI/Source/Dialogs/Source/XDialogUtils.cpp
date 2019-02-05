/*
 *  File:       XDialogUtils.cpp
 *  Summary:   	Handy dialog manager related routines.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDialogUtils.cpp,v $
 *		Revision 1.3  2001/04/27 09:20:22  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.2  2001/04/27 04:32:09  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.1  2001/01/01 00:29:16  jesjones
 *		Moved from the Windows to the Dialog folder.
 *		
 *		Revision 1.4  2000/12/31 04:47:34  jesjones
 *		Backed out of implementing GetString.
 *		
 *		Revision 1.3  2000/12/10 04:09:44  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:31:57  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	12/21/99	JDJ		Added some simple input dialogs.
 *		 <4>	 7/19/99	JDJ		Tweaked for Carbon.
 *		 <3>	 6/11/99	JDJ		Compiles with const_strings pragma enabled.
 *		 <2>	 6/05/99	JDJ		No longer uses MB_ICONQUESTION
 *		 <1>	12/06/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XDialogUtils.h>

#include <cctype>

#include <IDialogBox.h>
#include <IPoseDialogBox.h>
#include <IReadXML.h>
#include <IRootControl.h>
#include <IText.h>
#include <ITextBox.h>
#include <IWindow.h>
#include <XConstants.h>
#include <XMiscUtils.h>
#include <XStringUtils.h>
#include <XTranscode.h>

#if MAC
	#include <MDialogUtils.h>
	#include <MSystemInfo.h>
#elif WIN
	#include <WSystemInfo.h>
#endif

namespace Whisper {


//-----------------------------------
//	Constants
//
#if MAC
	const ResID kAskID   = 150;
	const ResID kYesID   = 151;
	const ResID kNoID    = 152;
#endif


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// YesNoCancelFilter
//
//---------------------------------------------------------------
#if MAC
static pascal Boolean YesNoCancelFilter(DialogPtr dptr, EventRecord* event, int16* item)
{
	Boolean handled = false;
	
	if (event->what == keyDown) {
		char ch   = (char) std::toupper((int) (event->message & charCodeMask));
		int16 key = (int16) ((event->message & keyCodeMask) >> 8);

		if (ch == 'Y' || key == kReturnKey || ch == kEnterChar) { 
			*item = 1;
			handled = true;
			
		} else if (ch == '~' || key == kEscapeKey || IsCommandPeriod(*event)) {
			*item = 2;
			handled = true;
			
		} else if (ch == 'N') {
			*item = 3;
			handled = true;
		}

		if (handled) 
			FlashButton(dptr, *item);

	} else
		handled = DefaultFilter(dptr, event, item);
	
	return handled;
}
#endif


//---------------------------------------------------------------
//
// YesFilter
//
//---------------------------------------------------------------
#if MAC
static pascal Boolean YesFilter(DialogPtr dptr, EventRecord* event, int16* item)
{
	Boolean handled = false;
	
	if (event->what == keyDown) {
		char ch   = (char) std::toupper((int) (event->message & charCodeMask));
		int16 key = (int16) ((event->message & keyCodeMask) >> 8);

		if (ch == 'Y' || key == kReturnKey || ch == kEnterChar) { 
			*item = 1;
			handled = true;
			
		} else if (ch == 'N' || ch == '~' || key == kEscapeKey || IsCommandPeriod(*event)) {
			*item = 3;
			handled = true;
		}
		
		if (handled) 
			FlashButton(dptr, *item);

	} else
		handled = DefaultFilter(dptr, event, item);
	
	return handled;
}
#endif


//---------------------------------------------------------------
//
// NoFilter
//
//---------------------------------------------------------------
#if MAC
static pascal Boolean NoFilter(DialogPtr dptr, EventRecord* event, int16* item)
{
	Boolean handled = false;
	
	if (event->what == keyDown) {
		char ch   = (char) std::toupper((int) (event->message & charCodeMask));
		int16 key = (int16) ((event->message & keyCodeMask) >> 8);

		if (ch == 'Y') {
			*item = 2;
			handled = true;
		
		} else if (ch == 'N' || key == kReturnKey || ch == kEnterChar) { 
			*item = 1;
			handled = true;
		}

		if (handled) 
			FlashButton(dptr, *item);

	} else
		handled = DefaultFilter(dptr, event, item);
	
	return handled;
}
#endif

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Alerts
// ===================================================================================

//---------------------------------------------------------------
//
// DoNote
//
//---------------------------------------------------------------
void DoNote(const std::wstring& errorStr, const std::wstring& supplementalStr)
{
#if MAC
	Whisper::DoMessageAlert(kAlertNoteAlert, errorStr, supplementalStr);
	
#elif WIN
	std::wstring text = errorStr + L"\n" + supplementalStr;	// $$$ would be nice to handle this more like StandardAlert
	
	uint32 flags = MB_OK +				// just an OK button
				   MB_ICONINFORMATION +	// display the information icon
				   MB_DEFBUTTON1 +		// select the OK button
				   MB_TASKMODAL +		// don't let the user do anything else in the app
				   MB_SETFOREGROUND;	// bring the app to the front

	if (WSystemInfo::HasUnicode())
		(void) MessageBoxW(nil, text.c_str(), L"", flags);
	else
		(void) MessageBoxA(nil, ToPlatformStr(text).c_str(), "", flags);
#endif
}


//---------------------------------------------------------------
//
// DoCaution
//
//---------------------------------------------------------------
bool DoCaution(const std::wstring& errorStr, const std::wstring& supplementalStr)
{
	bool proceed = false;

#if MAC
	proceed = DoConfirmAlert(kAlertCautionAlert, errorStr, supplementalStr);
	
#elif WIN
	std::wstring text = errorStr + L"\n" + supplementalStr;
	
	uint32 flags = MB_OKCANCEL +		// OK and Cancel buttons
				   MB_ICONWARNING +		// display the exclamation mark icon
				   MB_DEFBUTTON2 +		// select the Cancel button
				   MB_TASKMODAL +		// don't let the user do anything else in the app
				   MB_SETFOREGROUND;	// bring the app to the front

	int32 result;
	if (WSystemInfo::HasUnicode())
		result = MessageBoxW(nil, text.c_str(), L"", flags);
	else
		result = MessageBoxA(nil, ToPlatformStr(text).c_str(), "", flags);
	proceed = result == IDCANCEL;
#endif
		
	return proceed;
}

//---------------------------------------------------------------
//
// DoStop
//
//---------------------------------------------------------------
void DoStop(const std::wstring& errorStr, const std::wstring& supplementalStr)
{
#if MAC
	Whisper::DoMessageAlert(kAlertStopAlert, errorStr, supplementalStr);
	
#elif WIN
	std::wstring text = errorStr + L"\n" + supplementalStr;
	
	uint32 flags = MB_OK +				// just an OK button
				   MB_ICONSTOP +		// display the stop sign icon
				   MB_DEFBUTTON1 +		// select the OK button
				   MB_TASKMODAL +		// don't let the user do anything else in the app
				   MB_SETFOREGROUND;	// bring the app to the front

	if (WSystemInfo::HasUnicode())
		(void) MessageBoxW(nil, text.c_str(), L"", flags);
	else
		(void) MessageBoxA(nil, ToPlatformStr(text).c_str(), "", flags);
#endif
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Question Alerts
// ===================================================================================

//---------------------------------------------------------------
//
// Ask
//
//---------------------------------------------------------------
EAnswerType Ask(const std::wstring& prompt)
{	 
	PRECONDITION(prompt.length() > 0);

	EAnswerType result;
	
#if MAC
	int16 item = 1;
	
	InitCursor();

	MCreateModalFilterUPP theFilter(YesNoCancelFilter);

	if (MSystemInfo::HasAppearanceMgr()) {
		MDeativateDesktop deactivate;

		std::wstring defaultText = LoadWhisperString(L"Yes (Button)");	// need to define these up here so that they stick around until StandardAlert returns
		std::wstring cancelText  = LoadWhisperString(L"Cancel (Button)");
		std::wstring otherText   = LoadWhisperString(L"No (Button)");
		
		PascalString defaultStr = ToPascalStr(defaultText);
		PascalString cancelStr = ToPascalStr(cancelText);
		PascalString otherStr = ToPascalStr(otherText);

		AlertStdAlertParamRec params;
		params.movable       = true;
		params.helpButton    = false;
		params.filterProc    = theFilter;
		params.defaultText   = (unsigned char*) defaultStr.c_str();
		params.cancelText    = (unsigned char*) cancelStr.c_str();
		params.otherText     = (unsigned char*) otherStr.c_str();
		params.defaultButton = 1;
		params.cancelButton  = 2;
		params.position      = kWindowAlertPositionParentWindowScreen;

		OSErr err = StandardAlert(kAlertCautionAlert, (unsigned char*) ToPascalStr(prompt).c_str(), (unsigned char*) "\p", &params, &item);
		ASSERT(err == noErr);						// seems kind of pointless to throw

	} else {
		ParamText("\p", ToPascalStr(prompt).c_str(), "\p", "\p");
		item = DoAlert(kAskID, YesNoCancelFilter);
	}
		
	if (item == 1)
		result = kYes;
	else if (item == 2)
		result = kCancel;
	else if (item == 3)
		result = kNo;
		
#elif WIN	
	uint32 flags = MB_YESNOCANCEL +		// Yes, No, and Cancel buttons
				   MB_ICONINFORMATION +	// display the info icon (the question mark icon is no longer recommended)
				   MB_DEFBUTTON1 +		// select the Yes button
				   MB_TASKMODAL +		// don't let the user do anything else in the app
				   MB_SETFOREGROUND;	// bring the app to the front

	int32 item;
	if (WSystemInfo::HasUnicode())
		item = MessageBoxW(nil, prompt.c_str(), L"", flags);
	else
		item = MessageBoxA(nil, ToPlatformStr(prompt).c_str(), "", flags);
	
	if (item == IDYES)
		result = kYes; 
	else if (item == IDCANCEL)
		result = kCancel;
	else 
		result = kNo;
		
#endif

	return result;
}

			
//---------------------------------------------------------------
//
// AnswerYes
//
//---------------------------------------------------------------
bool AnswerYes(const std::wstring& prompt)
{	 
	PRECONDITION(prompt.length() > 0);

#if MAC
	int16 item = 1;
	
	InitCursor();

	MCreateModalFilterUPP theFilter(YesFilter);

	if (MSystemInfo::HasAppearanceMgr()) {
		MDeativateDesktop deactivate;

		std::wstring defaultText = LoadWhisperString(L"Yes (Button)");	// need to define these up here so that they stick around until StandardAlert returns
		std::wstring otherText   = LoadWhisperString(L"No (Button)");
		
		PascalString defaultStr = ToPascalStr(defaultText);
		PascalString otherStr = ToPascalStr(otherText);

		AlertStdAlertParamRec params;
		params.movable       = true;
		params.helpButton    = false;
		params.filterProc    = theFilter;
		params.defaultText   = (unsigned char*) defaultStr.c_str();
		params.cancelText    = nil;				
		params.otherText     = (unsigned char*) otherStr.c_str();
		params.defaultButton = 1;
		params.cancelButton  = 0;
		params.position      = kWindowAlertPositionParentWindowScreen;

		OSErr err = StandardAlert(kAlertCautionAlert, (unsigned char*) ToPascalStr(prompt).c_str(), const_cast<unsigned char*>("\p"), &params, &item);
		ASSERT(err == noErr);						// seems kind of pointless to throw

	} else {
		ParamText("\p", (unsigned char*) ToPascalStr(prompt).c_str(), "\p", "\p");
		item = DoAlert(kYesID, YesFilter);
	}
		
#elif WIN	
	int32 item = 1;
	
	uint32 flags = MB_YESNO +			// Yes and No buttons
				   MB_ICONINFORMATION +	// display the info icon (the question mark icon is no longer recommended)
				   MB_DEFBUTTON1 +		// select the Yes button
				   MB_TASKMODAL +		// don't let the user do anything else in the app
				   MB_SETFOREGROUND;	// bring the app to the front

	int32 result;
	if (WSystemInfo::HasUnicode())
		result = MessageBoxW(nil, prompt.c_str(), L"", flags);
	else
		result = MessageBoxA(nil, ToPlatformStr(prompt).c_str(), "", flags);
	
	item = result == IDYES ? 1 : 0;		
#endif
		
	return item == 1;
}


//---------------------------------------------------------------
//
// AnswerNo
//
//---------------------------------------------------------------
bool AnswerNo(const std::wstring& prompt)
{	 
	PRECONDITION(prompt.length() > 0);

	bool no = false;
	
#if MAC
	InitCursor();

	MCreateModalFilterUPP theFilter(NoFilter);

	int16 item;
	if (MSystemInfo::HasAppearanceMgr()) {
		MDeativateDesktop deactivate;

		std::wstring otherText   = LoadWhisperString(L"Yes (Button)");	// need to define these up here so that they stick around until StandardAlert returns
		std::wstring defaultText = LoadWhisperString(L"No (Button)");
		
		PascalString defaultStr = ToPascalStr(defaultText);
		PascalString otherStr = ToPascalStr(otherText);

		AlertStdAlertParamRec params;
		params.movable       = true;
		params.helpButton    = false;
		params.filterProc    = theFilter;
		params.defaultText   = (unsigned char*) defaultStr.c_str();
		params.cancelText    = nil;
		params.otherText     = (unsigned char*) otherStr.c_str();
		params.defaultButton = 1;
		params.cancelButton  = 0;
		params.position      = kWindowAlertPositionParentWindowScreen;

		OSErr err = StandardAlert(kAlertCautionAlert, (unsigned char*) ToPascalStr(prompt).c_str(), const_cast<unsigned char*>("\p"), &params, &item);
		ASSERT(err == noErr);						// seems kind of pointless to throw
		no = item == 1;		

	} else {
		ParamText("\p", (unsigned char*) ToPascalStr(prompt).c_str(), "\p", "\p");
		item = DoAlert(kNoID, NoFilter);
		no = item == 1;		
	}
		
#elif WIN	
	uint32 flags = MB_YESNO +			// Yes and No buttons
				   MB_ICONINFORMATION +	// display the info icon (the question mark icon is no longer recommended)
				   MB_DEFBUTTON2 +		// select the No button
				   MB_TASKMODAL +		// don't let the user do anything else in the app
				   MB_SETFOREGROUND;	// bring the app to the front

	int32 result;
	if (WSystemInfo::HasUnicode())
		result = MessageBoxW(nil, prompt.c_str(), L"", flags);
	else
		result = MessageBoxA(nil, ToPlatformStr(prompt).c_str(), "", flags);
	
	no = result == IDNO;		
#endif
		
	return no;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Simple Input Dialogs
// ===================================================================================

//---------------------------------------------------------------
//
// GetString
//
//---------------------------------------------------------------
#if 0
bool GetString(const XURI& uri, const std::wstring& id, const IHierarchyPtr& parent, std::wstring* value)
{
	PRECONDITION(value != nil);
	
	bool pressedOK = false;
	
	// Create the dialog
	IReadXMLPtr persist(L"XML Persist");
	IWindowPtr dialog = persist->CreateBoss(uri, id, parent);
	IRootControlPtr root = dialog->GetRootControl();
	
	// Initialize and open the dialog	
	ITextPtr text = root->FindChild(L"Text");
	text->SetText(*value, kDontRedraw);

	IPoseDialogBoxPtr handler(L"Pose Dialog Box");	
	handler->Open(dialog);
	
	// Select the textbox
	ITextBoxPtr textbox(text);
	textbox->Select();

	// Let the dialog process events until the user presses the
	// OK or Cancel button.
	if (handler->Run()) {
		*value = text->GetText();
		pressedOK = true;
	}
		
	handler->Close();

	return pressedOK;
}
#endif


//---------------------------------------------------------------
//
// GetInt16
//
//---------------------------------------------------------------
#if 0
bool GetInt16(const XURI& uri, const std::wstring& id, const IHierarchyPtr& parent, int16* value)
{
	PRECONDITION(value != nil);
	
	int32 temp = *value;
	bool proceed = GetInt32(uri, id, &temp);
	if (proceed)
		*value = numeric_cast<int16>(temp);
		
	return proceed;
}
#endif


//---------------------------------------------------------------
//
// GetInt32
//
//---------------------------------------------------------------
#if 0
bool GetInt32(const XURI& uri, const std::wstring& id, const IHierarchyPtr& parent, int32* value)
{
	PRECONDITION(value != nil);
	
	bool proceed = false;
	
	XDialogBox* dialog = XDialogBox::Create(uri, id, nil);
	
	XRootControl* root = dialog->GetRootControl();
	XTextBoxControl* textBox = dynamic_cast<XTextBoxControl*>(root->FindChild(L"Text"));
	ASSERT(textBox != nil);
	
	textBox->SetValue(*value);
	textBox->SelectAll();
			
	{
	XDialogHandler handler(dialog);
		handler.Pose();
		textBox->SetFocus();
	
		if (handler.HandleEventLoop()) {
			*value = textBox->GetValue();
			proceed = true;
		}
	}
			
	return proceed;
}
#endif


//---------------------------------------------------------------
//
// GetUInt32
//
//---------------------------------------------------------------
#if 0
bool GetUInt32(const XURI& uri, const std::wstring& id, const IHierarchyPtr& parent, uint32* value)
{
	PRECONDITION(value != nil);
	
	bool proceed = false;
	
	XDialogBox* dialog = XDialogBox::Create(uri, id, nil);
	
	XRootControl* root = dialog->GetRootControl();
	XTextBoxControl* textBox = dynamic_cast<XTextBoxControl*>(root->FindChild(L"Text"));
	ASSERT(textBox != nil);
	
	textBox->SetText(UInt32ToStr(*value));
	textBox->SelectAll();
			
	{
	XDialogHandler handler(dialog);
		handler.Pose();
		textBox->SetFocus();
	
		if (handler.HandleEventLoop()) {
			*value = StrToUInt32(textBox->GetText());
			proceed = true;
		}
	}
			
	return proceed;
}
#endif


//---------------------------------------------------------------
//
// GetFloat
//
//---------------------------------------------------------------
#if 0
bool GetFloat(const XURI& uri, const std::wstring& id, const IHierarchyPtr& parent, float* value)
{
	PRECONDITION(value != nil);
	
	double temp = *value;
	
	bool proceed = GetDouble(uri, id, &temp);
	if (proceed)
		*value = (float) temp;
		
	return proceed;
}
#endif
		

//---------------------------------------------------------------
//
// GetDouble
//
//---------------------------------------------------------------
#if 0
bool GetDouble(const XURI& uri, const std::wstring& id, const IHierarchyPtr& parent, double* value)
{
	PRECONDITION(value != nil);
	
	bool proceed = false;
	
	XDialogBox* dialog = XDialogBox::Create(uri, id, nil);
	
	XRootControl* root = dialog->GetRootControl();
	XTextBoxControl* textBox = dynamic_cast<XTextBoxControl*>(root->FindChild(L"Text"));
	ASSERT(textBox != nil);
	
	textBox->SetText(DoubleToStr(*value));
	textBox->SelectAll();
			
	{
	XDialogHandler handler(dialog);
		handler.Pose();
		textBox->SetFocus();
	
		if (handler.HandleEventLoop()) {
			*value = StrToDouble(textBox->GetText());
			proceed = true;
		}
	}
			
	return proceed;
}
#endif


}	// namespace Whisper

