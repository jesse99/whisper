/*
 *  File:       XXMLControls.cpp
 *  Summary:   	Functions uses to stream the control classes into and out of XML.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLControls.cpp,v $
 *		Revision 1.12  2001/04/27 04:38:38  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.11  2001/04/01 04:59:14  jesjones
 *		Added listbox control.
 *		
 *		Revision 1.10  2001/03/23 10:59:53  jesjones
 *		Added ISeperator support.
 *		
 *		Revision 1.9  2001/03/20 12:02:28  jesjones
 *		Added bevel button.
 *		
 *		Revision 1.8  2001/03/09 10:23:05  jesjones
 *		Nuked help_id.
 *		
 *		Revision 1.7  2001/03/01 11:40:27  jesjones
 *		Tweaked the TRACEFLOW statements so less code is generated.
 *		
 *		Revision 1.6  2000/12/25 02:49:23  jesjones
 *		Textboxes default to a slightly larger size.
 *		
 *		Revision 1.5  2000/12/19 08:00:51  jesjones
 *		Added ITextBox.
 *		
 *		Revision 1.4  2000/12/17 03:51:22  jesjones
 *		Added a color swatch control.
 *		
 *		Revision 1.3  2000/12/05 09:20:49  jesjones
 *		Added support for checkboxes and radio buttons.
 *		
 *		Revision 1.2  2000/11/09 13:00:42  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XXMLControls.h>

#include <IBevelButton.h>
#include <ICheckBox.h>
#include <IColorSwatch.h>
#include <IControl.h>
#include <IGroupBox.h>
#include <IGroupControl.h>
#include <IListBox.h>
#include <IProgressBar.h>
#include <IPushButton.h>
#include <IRadioButton.h>
#include <IRootControl.h>
#include <IScrollableControl.h>
#include <IScrollBar.h>
#include <IScrollerControl.h>
#include <ISeperator.h>
#include <IStaticText.h>
#include <ITextBox.h>
#include <IUserControl.h>
#include <IValueControl.h>
#include <XAutoPtr.h>
#include <XImageImporter.h>
#include <XPixMap.h>
#include <XStringUtils.h>
#include <XURI.h>
#include <XXMLDoc.h>
#include <XXMLItems.h>

#if WIN
	#include <commctrl.h>
#endif

namespace Whisper {

#if __MWERKS__ > 0x2300				
	#pragma warn_no_side_effect off		// doesn't like "style |= SBS_HORZ"...
#endif


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// NameToProcID
//
//---------------------------------------------------------------
static uint32 NameToProcID(const std::wstring& name)
{	
	uint32 procID = 0;
	
	// Bevel button
	if (name.find(L"kControlBevelButtonSmallBevelProc") != std::wstring::npos) {
		if (name.find(L"kControlBevelButtonMenuOnRight") != std::wstring::npos) 
			procID = kControlBevelButtonSmallBevelProc + kControlBevelButtonMenuOnRight;
		else
			procID = kControlBevelButtonSmallBevelProc;
	
	} else if (name.find(L"kControlBevelButtonNormalBevelProc") != std::wstring::npos) {
		if (name.find(L"kControlBevelButtonMenuOnRight") != std::wstring::npos) 
			procID = kControlBevelButtonNormalBevelProc + kControlBevelButtonMenuOnRight;
		else
			procID = kControlBevelButtonNormalBevelProc;

	} else if (name.find(L"kControlBevelButtonLargeBevelProc") != std::wstring::npos) {
		if (name.find(L"kControlBevelButtonMenuOnRight") != std::wstring::npos) 
			procID = kControlBevelButtonLargeBevelProc + kControlBevelButtonMenuOnRight;
		else
			procID = kControlBevelButtonLargeBevelProc;
			
	// Slider
	} else if (name.find(L"kControlSliderProc") != std::wstring::npos) {
		procID = kControlSliderProc;
		
		if (name.find(L"kControlSliderLiveFeedback") != std::wstring::npos) 
			procID |= kControlSliderLiveFeedback;
		if (name.find(L"kControlSliderHasTickMarks") != std::wstring::npos) 
			procID |= kControlSliderHasTickMarks;
		if (name.find(L"kControlSliderReverseDirection") != std::wstring::npos) 
			procID |= kControlSliderReverseDirection;
		if (name.find(L"kControlSliderNonDirectional") != std::wstring::npos) 
			procID |= kControlSliderNonDirectional;
			
	// Triangle
	} else if (name.find(L"kControlTriangleLeftFacingProc") != std::wstring::npos) {
		procID = kControlTriangleLeftFacingProc;
		
		if (name.find(L"kControlTriangleAutoToggleProc") != std::wstring::npos) 
			procID |= kControlTriangleAutoToggleProc;
		if (name.find(L"kControlTriangleLeftFacingAutoToggleProc") != std::wstring::npos) 
			procID |= kControlTriangleLeftFacingAutoToggleProc;
			
	// Progress Bar
	} else if (name == L"kControlProgressBarProc") {
		procID = kControlProgressBarProc;
			
	// Little Arrows
	} else if (name == L"kControlLittleArrowsProc") {
		procID = kControlLittleArrowsProc;
			
	// Chasing Arrows
	} else if (name == L"kControlChasingArrowsProc") {
		procID = kControlChasingArrowsProc;
		
	// Tabs
	} else if (name == L"kControlTabLargeProc") {
		procID = kControlTabLargeProc;

	} else if (name == L"kControlTabSmallProc") {
		procID = kControlTabSmallProc;
	
	} else if (name == L"kControlTabLargeNorthProc") {
		procID = kControlTabLargeNorthProc;
	
	} else if (name == L"kControlTabSmallNorthProc") {
		procID = kControlTabSmallNorthProc;
	
	} else if (name == L"kControlTabLargeSouthProc") {
		procID = kControlTabLargeSouthProc;
	
	} else if (name == L"kControlTabSmallSouthProc") {
		procID = kControlTabSmallSouthProc;
	
	} else if (name == L"kControlTabLargeEastProc") {
		procID = kControlTabLargeEastProc;
	
	} else if (name == L"kControlTabSmallEastProc") {
		procID = kControlTabSmallEastProc;
		
	} else if (name == L"kControlTabLargeWestProc") {
		procID = kControlTabLargeWestProc;
		
	} else if (name == L"kControlTabSmallWestProc") {
		procID = kControlTabSmallWestProc;
			
	// Visual separator
	} else if (name == L"kControlSeparatorLineProc") {
		procID = kControlSeparatorLineProc;
			
	// Group Box
	} else if (name == L"kControlGroupBoxTextTitleProc") {
		procID = kControlGroupBoxTextTitleProc;

	} else if (name == L"kControlGroupBoxCheckBoxProc") {
		procID = kControlGroupBoxCheckBoxProc;

	} else if (name == L"kControlGroupBoxPopupButtonProc") {
		procID = kControlGroupBoxPopupButtonProc;

	} else if (name == L"kControlGroupBoxSecondaryTextTitleProc") {
		procID = kControlGroupBoxSecondaryTextTitleProc;

	} else if (name == L"kControlGroupBoxSecondaryCheckBoxProc") {
		procID = kControlGroupBoxSecondaryCheckBoxProc;

	} else if (name == L"kControlGroupBoxSecondaryPopupButtonProc") {
		procID = kControlGroupBoxSecondaryPopupButtonProc;
			
	// Image Well
	} else if (name == L"kControlImageWellProc") {
		procID = kControlImageWellProc;
			
	// Popup Arrow
	} else if (name == L"kControlPopupArrowEastProc") {
		procID = kControlPopupArrowEastProc;

	} else if (name == L"kControlPopupArrowWestProc") {
		procID = kControlPopupArrowWestProc;

	} else if (name == L"kControlPopupArrowNorthProc") {
		procID = kControlPopupArrowNorthProc;

	} else if (name == L"kControlPopupArrowSouthProc") {
		procID = kControlPopupArrowSouthProc;

	} else if (name == L"kControlPopupArrowSmallEastProc") {
		procID = kControlPopupArrowSmallEastProc;

	} else if (name == L"kControlPopupArrowSmallWestProc") {
		procID = kControlPopupArrowSmallWestProc;

	} else if (name == L"kControlPopupArrowSmallNorthProc") {
		procID = kControlPopupArrowSmallNorthProc;

	} else if (name == L"kControlPopupArrowSmallSouthProc") {
		procID = kControlPopupArrowSmallSouthProc;
			
	// Placard 
	} else if (name == L"kControlPlacardProc") {
		procID = kControlPlacardProc;
			
	// Clock
	} else if (name == L"kControlClockTimeProc") {
		procID = kControlClockTimeProc;

	} else if (name == L"kControlClockTimeSecondsProc") {
		procID = kControlClockTimeSecondsProc;

	} else if (name == L"kControlClockDateProc") {
		procID = kControlClockDateProc;

	} else if (name == L"kControlClockMonthYearProc") {
		procID = kControlClockMonthYearProc;
			
	// User Pane
	} else if (name == L"kControlUserPaneProc") {
		procID = kControlUserPaneProc;
			
	// Edit Text
	} else if (name == L"kControlEditTextProc") {
		procID = kControlEditTextProc;

	} else if (name == L"kControlEditTextPasswordProc") {
		procID = kControlEditTextPasswordProc;

	} else if (name == L"kControlEditTextInlineInputProc") {
		procID = kControlEditTextInlineInputProc;

	// Static Text
	} else if (name == L"kControlStaticTextProc") {
		procID = kControlStaticTextProc;
			
	// Picture
	} else if (name == L"kControlPictureProc") {
		procID = kControlPictureProc;

	} else if (name == L"kControlPictureNoTrackProc") {
		procID = kControlPictureNoTrackProc;
			
	// Icon
	} else if (name == L"kControlIconProc") {
		procID = kControlIconProc;

	} else if (name == L"kControlIconNoTrackProc") {
		procID = kControlIconNoTrackProc;

	} else if (name == L"kControlIconSuiteProc") {
		procID = kControlIconSuiteProc;

	} else if (name == L"kControlIconSuiteNoTrackProc") {
		procID = kControlIconSuiteNoTrackProc;

	} else if (name == L"kControlIconRefProc") {
		procID = kControlIconRefProc;

	} else if (name == L"kControlIconRefNoTrackProc") {
		procID = kControlIconRefNoTrackProc;
			
	// Window Header
	} else if (name == L"kControlWindowHeaderProc") {
		procID = kControlWindowHeaderProc;

	} else if (name == L"kControlWindowListViewHeaderProc") {
		procID = kControlWindowListViewHeaderProc;
			
	// List Box
	} else if (name == L"kControlListBoxProc") {
		procID = kControlListBoxProc;

	} else if (name == L"kControlListBoxAutoSizeProc") {
		procID = kControlListBoxAutoSizeProc;
			
	// Theme Push Button/Check Box/Radio Button
	} else if (name == L"kControlPushButtonProc") {
		procID = kControlPushButtonProc;

	} else if (name == L"kControlCheckBoxProc") {
		procID = kControlCheckBoxProc;

	} else if (name == L"kControlRadioButtonProc") {
		procID = kControlRadioButtonProc;

	} else if (name == L"kControlPushButLeftIconProc") {
		procID = kControlPushButLeftIconProc;

	} else if (name == L"kControlPushButRightIconProc") {
		procID = kControlPushButRightIconProc;

	} else if (name == L"kControlCheckBoxAutoToggleProc") {
		procID = kControlCheckBoxAutoToggleProc;

	} else if (name == L"kControlRadioButtonAutoToggleProc") {
		procID = kControlRadioButtonAutoToggleProc;
			
	// Theme Scroll Bar
	} else if (name == L"kControlScrollBarProc") {
		procID = kControlScrollBarProc;

	} else if (name == L"kControlScrollBarLiveProc") {
		procID = kControlScrollBarLiveProc;
			
	// Theme Popup Button
	} else if (name.find(L"kControlPopupButtonProc") != std::wstring::npos) {
		procID = kControlPopupButtonProc;
		
		if (name.find(L"kControlPopupFixedWidthVariant") != std::wstring::npos) 
			procID |= kControlPopupFixedWidthVariant;
		if (name.find(L"kControlPopupVariableWidthVariant") != std::wstring::npos) 
			procID |= kControlPopupVariableWidthVariant;
		if (name.find(L"kControlPopupUseAddResMenuVariant") != std::wstring::npos) 
			procID |= kControlPopupUseAddResMenuVariant;
		if (name.find(L"kControlPopupUseWFontVariant") != std::wstring::npos) 
			procID |= kControlPopupUseWFontVariant;
			
	// Radio Group
	} else if (name == L"kControlRadioGroupProc") {
		procID = kControlRadioGroupProc;
			
	// Scrolling Textbox
	} else if (name == L"kControlScrollTextBoxProc") {
		procID = kControlScrollTextBoxProc;

	} else if (name == L"kControlScrollTextBoxAutoScrollProc") {
		procID = kControlScrollTextBoxAutoScrollProc;
		
	} else
		DEBUGSTR("Bad name in NameToProcID");
	
	return procID;
}


//---------------------------------------------------------------
//
// NameToStyle
//
//---------------------------------------------------------------
static uint32 NameToStyle(const std::wstring& name)
{	
	uint32 style = 0;
	
	std::wstring str = name;
	while (str.length() > 0) {
		std::wstring temp = Whisper::Parse(str, L" +");

		bool found1 = true;
		bool found2 = true;
	
		// Window Styles
		if (temp == L"WS_CHILD")
			style |= WS_CHILD;
		else if (temp == L"WS_VISIBLE")
			style |= WS_VISIBLE;
		else if (temp == L"WS_DISABLED")
			style |= WS_DISABLED;
		else if (temp == L"WS_CLIPSIBLINGS")
			style |= WS_CLIPSIBLINGS;
		else if (temp == L"WS_CLIPCHILDREN")
			style |= WS_CLIPCHILDREN;
		else if (temp == L"WS_CAPTION")
			style |= WS_CAPTION;
		else if (temp == L"WS_BORDER")
			style |= WS_BORDER;
		else if (temp == L"WS_THICKFRAME")
			style |= WS_THICKFRAME;
		else if (temp == L"WS_GROUP")
			style |= WS_GROUP;
		else if (temp == L"WS_TABSTOP")
			style |= WS_TABSTOP;
			
		// Scroll Bar Styles
		else if (temp == L"SBS_HORZ")
			style |= SBS_HORZ;
		else if (temp == L"SBS_VERT")
			style |= SBS_VERT;
		else if (temp == L"SBS_TOPALIGN")
			style |= SBS_TOPALIGN;
		else if (temp == L"SBS_LEFTALIGN")
			style |= SBS_LEFTALIGN;
		else if (temp == L"SBS_BOTTOMALIGN")
			style |= SBS_BOTTOMALIGN;
		else if (temp == L"SBS_RIGHTALIGN")
			style |= SBS_RIGHTALIGN;
		else if (temp == L"SBS_SIZEBOXTOPLEFTALIGN")
			style |= SBS_SIZEBOXTOPLEFTALIGN;
		else if (temp == L"SBS_SIZEBOXBOTTOMRIGHTALIGN")
			style |= SBS_SIZEBOXBOTTOMRIGHTALIGN;
		else if (temp == L"SBS_SIZEBOX")
			style |= SBS_SIZEBOX;
		else if (temp == L"SBS_SIZEGRIP")
			style |= SBS_SIZEGRIP;

		// Combo Box styles
		else if (temp == L"CBS_SIMPLE")
			style |= CBS_SIMPLE;
		else if (temp == L"CBS_DROPDOWN")
			style |= CBS_DROPDOWN;
		else if (temp == L"CBS_DROPDOWNLIST")
			style |= CBS_DROPDOWNLIST;
		else if (temp == L"CBS_OWNERDRAWFIXED")
			style |= CBS_OWNERDRAWFIXED;
		else if (temp == L"CBS_OWNERDRAWVARIABLE")
			style |= CBS_OWNERDRAWVARIABLE;
		else if (temp == L"CBS_AUTOHSCROLL")
			style |= CBS_AUTOHSCROLL;
		else if (temp == L"CBS_OEMCONVERT")
			style |= CBS_OEMCONVERT;
		else if (temp == L"CBS_SORT")
			style |= CBS_SORT;
		else if (temp == L"CBS_HASSTRINGS")
			style |= CBS_HASSTRINGS;
		else if (temp == L"CBS_NOINTEGRALHEIGHT")
			style |= CBS_NOINTEGRALHEIGHT;
		else if (temp == L"CBS_DISABLENOSCROLL")
			style |= CBS_DISABLENOSCROLL;
		else if (temp == L"CBS_UPPERCASE")
			style |= CBS_UPPERCASE;
		else if (temp == L"CBS_LOWERCASE")
			style |= CBS_LOWERCASE;

		// Listbox Styles
		else if (temp == L"LBS_NOTIFY")
			style |= LBS_NOTIFY;
		else if (temp == L"LBS_SORT")
			style |= LBS_SORT;
		else if (temp == L"LBS_NOREDRAW")
			style |= LBS_NOREDRAW;
		else if (temp == L"LBS_MULTIPLESEL")
			style |= LBS_MULTIPLESEL;
		else if (temp == L"LBS_OWNERDRAWFIXED")
			style |= LBS_OWNERDRAWFIXED;
		else if (temp == L"LBS_OWNERDRAWVARIABLE")
			style |= LBS_OWNERDRAWVARIABLE;
		else if (temp == L"LBS_HASSTRINGS")
			style |= LBS_HASSTRINGS;
		else if (temp == L"LBS_USETABSTOPS")
			style |= LBS_USETABSTOPS;
		else if (temp == L"LBS_NOINTEGRALHEIGHT")
			style |= LBS_NOINTEGRALHEIGHT;
		else if (temp == L"LBS_MULTICOLUMN")
			style |= LBS_MULTICOLUMN;
		else if (temp == L"LBS_WANTKEYBOARDINPUT")
			style |= LBS_WANTKEYBOARDINPUT;
		else if (temp == L"LBS_EXTENDEDSEL")
			style |= LBS_EXTENDEDSEL;
		else if (temp == L"LBS_DISABLENOSCROLL")
			style |= LBS_DISABLENOSCROLL;
		else if (temp == L"LBS_NODATA")
			style |= LBS_NODATA;
		else if (temp == L"LBS_NOSEL")
			style |= LBS_NOSEL;

		// Button Control Styles
		else if (temp == L"BS_PUSHBUTTON")
			style |= BS_PUSHBUTTON;
		else if (temp == L"BS_DEFPUSHBUTTON")
			style |= BS_DEFPUSHBUTTON;
		else if (temp == L"BS_CHECKBOX")
			style |= BS_CHECKBOX;
		else if (temp == L"BS_AUTOCHECKBOX")
			style |= BS_AUTOCHECKBOX;
		else if (temp == L"BS_RADIOBUTTON")
			style |= BS_RADIOBUTTON;
		else if (temp == L"BS_3STATE")
			style |= BS_3STATE;
		else if (temp == L"BS_AUTO3STATE")
			style |= BS_AUTO3STATE;
		else if (temp == L"BS_GROUPBOX")
			style |= BS_GROUPBOX;
		else if (temp == L"BS_USERBUTTON")
			style |= BS_USERBUTTON;
		else if (temp == L"BS_AUTORADIOBUTTON")
			style |= BS_AUTORADIOBUTTON;
		else if (temp == L"BS_OWNERDRAW")
			style |= BS_OWNERDRAW;
		else if (temp == L"BS_LEFTTEXT")
			style |= BS_LEFTTEXT;
		else if (temp == L"BS_TEXT")
			style |= BS_TEXT;
		else if (temp == L"BS_ICON")
			style |= BS_ICON;
		else if (temp == L"BS_BITMAP")
			style |= BS_BITMAP;
		else if (temp == L"BS_LEFT")
			style |= BS_LEFT;
		else if (temp == L"BS_RIGHT")
			style |= BS_RIGHT;
		else if (temp == L"BS_CENTER")
			style |= BS_CENTER;
		else if (temp == L"BS_TOP")
			style |= BS_TOP;
		else if (temp == L"BS_BOTTOM")
			style |= BS_BOTTOM;
		else if (temp == L"BS_VCENTER")
			style |= BS_VCENTER;
		else if (temp == L"BS_PUSHLIKE")
			style |= BS_PUSHLIKE;
		else if (temp == L"BS_MULTILINE")
			style |= BS_MULTILINE;
		else if (temp == L"BS_NOTIFY")
			style |= BS_NOTIFY;
		else if (temp == L"BS_FLAT")
			style |= BS_FLAT;
		else if (temp == L"BS_RIGHTBUTTON")
			style |= BS_RIGHTBUTTON;

		// Static Control Styles
		else if (temp == L"SS_LEFT")
			style |= SS_LEFT;
		else if (temp == L"SS_CENTER")
			style |= SS_CENTER;
		else if (temp == L"SS_RIGHT")
			style |= SS_RIGHT;
		else if (temp == L"SS_ICON")
			style |= SS_ICON;
		else if (temp == L"SS_BLACKRECT")
			style |= SS_BLACKRECT;
		else if (temp == L"SS_GRAYRECT")
			style |= SS_GRAYRECT;
		else if (temp == L"SS_WHITERECT")
			style |= SS_WHITERECT;
		else if (temp == L"SS_BLACKFRAME")
			style |= SS_BLACKFRAME;
		else if (temp == L"SS_GRAYFRAME")
			style |= SS_GRAYFRAME;
		else if (temp == L"SS_WHITEFRAME")
			style |= SS_WHITEFRAME;
		else if (temp == L"SS_USERITEM")
			style |= SS_USERITEM;
		else if (temp == L"SS_SIMPLE")
			style |= SS_SIMPLE;
		else if (temp == L"SS_LEFTNOWORDWRAP")
			style |= SS_LEFTNOWORDWRAP;
		else if (temp == L"SS_OWNERDRAW")
			style |= SS_OWNERDRAW;
		else if (temp == L"SS_BITMAP")
			style |= SS_BITMAP;
		else if (temp == L"SS_ENHMETAFILE")
			style |= SS_ENHMETAFILE;
		else if (temp == L"SS_ETCHEDHORZ")
			style |= SS_ETCHEDHORZ;
		else if (temp == L"SS_ETCHEDVERT")
			style |= SS_ETCHEDVERT;
		else if (temp == L"SS_ETCHEDFRAME")
			style |= SS_ETCHEDFRAME;
		else if (temp == L"SS_TYPEMASK")
			style |= SS_TYPEMASK;
		else if (temp == L"SS_NOPREFIX")
			style |= SS_NOPREFIX;
		else if (temp == L"SS_NOTIFY")
			style |= SS_NOTIFY;
		else if (temp == L"SS_CENTERIMAGE")
			style |= SS_CENTERIMAGE;
		else if (temp == L"SS_RIGHTJUST")
			style |= SS_RIGHTJUST;
		else if (temp == L"SS_REALSIZEIMAGE")
			style |= SS_REALSIZEIMAGE;
		else if (temp == L"SS_SUNKEN")
			style |= SS_SUNKEN;
		else if (temp == L"SS_ENDELLIPSIS")
			style |= SS_ENDELLIPSIS;
		else if (temp == L"SS_PATHELLIPSIS")
			style |= SS_PATHELLIPSIS;
		else if (temp == L"SS_WORDELLIPSIS")
			style |= SS_WORDELLIPSIS;
		else if (temp == L"SS_ELLIPSISMASK")
			style |= SS_ELLIPSISMASK;

		// Edit Control Styles
		else if (temp == L"ES_LEFT")
			style |= ES_LEFT;
		else if (temp == L"ES_CENTER")
			style |= ES_CENTER;
		else if (temp == L"ES_RIGHT")
			style |= ES_RIGHT;
		else if (temp == L"ES_MULTILINE")
			style |= ES_MULTILINE;
		else if (temp == L"ES_UPPERCASE")
			style |= ES_UPPERCASE;
		else if (temp == L"ES_LOWERCASE")
			style |= ES_LOWERCASE;
		else if (temp == L"ES_PASSWORD")
			style |= ES_PASSWORD;
		else if (temp == L"ES_AUTOVSCROLL")
			style |= ES_AUTOVSCROLL;
		else if (temp == L"ES_AUTOHSCROLL")
			style |= ES_AUTOHSCROLL;
		else if (temp == L"ES_NOHIDESEL")
			style |= ES_NOHIDESEL;
		else if (temp == L"ES_OEMCONVERT")
			style |= ES_OEMCONVERT;
		else if (temp == L"ES_READONLY")
			style |= ES_READONLY;
		else if (temp == L"ES_WANTRETURN")
			style |= ES_WANTRETURN;
		else if (temp == L"ES_NUMBER")
			style |= ES_NUMBER;
		else
			found1 = false;

		// Trackbar Control Styles
		if (temp == L"TBS_AUTOTICKS")		// $$ should be an else here, but MSVC 5 emits a compile error when there are this many else if's...
			style |= TBS_AUTOTICKS;
		else if (temp == L"TBS_VERT")
			style |= TBS_VERT;
		else if (temp == L"TBS_HORZ")
			style |= TBS_HORZ;
		else if (temp == L"TBS_TOP")
			style |= TBS_TOP;
		else if (temp == L"TBS_BOTTOM")
			style |= TBS_BOTTOM;
		else if (temp == L"TBS_LEFT")
			style |= TBS_LEFT;
		else if (temp == L"TBS_RIGHT")
			style |= TBS_RIGHT;
		else if (temp == L"TBS_BOTH")
			style |= TBS_BOTH;
		else if (temp == L"TBS_NOTICKS")
			style |= TBS_NOTICKS;
		else if (temp == L"TBS_ENABLESELRANGE")
			style |= TBS_ENABLESELRANGE;
		else if (temp == L"TBS_FIXEDLENGTH")
			style |= TBS_FIXEDLENGTH;
		else if (temp == L"TBS_NOTHUMB")
			style |= TBS_NOTHUMB;
		else if (temp == L"TBS_TOOLTIPS")
			style |= TBS_TOOLTIPS;
		else
			found2 = false;

		if (!found1 && !found2) 
			DEBUGSTR("Bad name in NameToStyle");
	}
			
	return style;
}

#if __MWERKS__
#pragma mark -
#endif
	
// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// LoadControl
//
//---------------------------------------------------------------
void LoadControl(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
				
	if (pass == kLoadedBoss) {
		IControlPtr control(boss, interface);				// this should always be valid because IPersistXML checks to make sure the names match
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
		
			if (name == L"name") {			
				control->SetName(attr.GetValue());	
							  												  					
			} else if (name == L"visible") {	
				if (attr.Get<bool>())
					control->Show(kDontRedraw);	
				else		
					control->Hide(kDontRedraw);	
							  												  					
			} else if (name == L"enabled") {			
				control->Enable(attr.Get<bool>(), kDontRedraw);	
											  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IControl!");
		}		
	}
}


//---------------------------------------------------------------
//
// LoadBevelButton
//
//---------------------------------------------------------------
void LoadBevelButton(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadingBoss) {
		uint32 style, procID;
		std::wstring text;
		EBevelTextAlignment alignment;
		EBevelBehavior behavior;
		ResID iconID = 0;
		std::wstring imageURI;
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"style") {			
				style = NameToStyle(value);	
							  												  					
			} else if (name == L"proc_id") {			
				procID = NameToProcID(value);	
							  															  					
			} else if (name == L"text") {			
				text = value;
							  															  					
			} else if (name == L"text_align") {			
				if (value == L"left")
					alignment = kAlignBevelTextLeft;
				
				else if (value == L"center")
					alignment = kAlignBevelTextRight;
				
				else if (value == L"right")
					alignment = kAlignBevelTextCenter;
				
				else if (value == L"system")
					alignment = kAlignBevelTextSystem;
				
				else
					DEBUGSTR("Bad alignment in LoadCheckBox");
							  															  					
			} else if (name == L"icon_id") {			
				iconID = attr.Get<int16>();
							  															  					
			} else if (name == L"image_uri") {			
				imageURI = value;
							  															  					
			} else if (name == L"behavior") {			
				if (value == L"resets")
					behavior = kBevelResets;
				
				else if (value == L"toggles")
					behavior = kBevelToggles;
				
				else if (value == L"sticks")
					behavior = kBevelSticks;
				
				else
					DEBUGSTR("Bad behavior in LoadCheckBox");
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IBevelButton!");
		}
		
		IBevelButtonPtr control(boss, interface);
		if (iconID != 0) {
			control->Init(parent, L"dummy name", XRect(0, 0, 1, 1), iconID, behavior, style, procID);	// name is set by LoadControl, frame is set by LoadGeometry
		
		} else if (imageURI.length() > 0) {
			XAutoPtr<XImageImporter> image(XImageImporter::Create(XURI(imageURI)));
			XPixMap pixmap(*image);
			control->Init(parent, L"dummy name", XRect(0, 0, 1, 1), pixmap, behavior, style, procID);	
		
		} else
			control->Init(parent, L"dummy name", XRect(0, 0, 1, 1), text, behavior, alignment, style, procID);	
	}
}


//---------------------------------------------------------------
//
// LoadCheckBox
//
//---------------------------------------------------------------
void LoadCheckBox(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadingBoss) {
		uint32 style, procID;
		std::wstring text;
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"style") {			
				style = NameToStyle(value);	
							  												  					
			} else if (name == L"proc_id") {			
				procID = NameToProcID(value);	
							  															  					
			} else if (name == L"text") {			
				text = value;
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of ICheckBox!");
		}
		
		ICheckBoxPtr control(boss, interface);
		control->Init(parent, L"dummy name", XRect(0, 0, 1, 1), text, style, procID);	// name is set by LoadControl, frame is set by LoadGeometry
	}
}


//---------------------------------------------------------------
//
// LoadColorSwatch
//
//---------------------------------------------------------------
void LoadColorSwatch(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadedBoss) {
		IColorSwatchPtr control(boss, interface);
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
		
			if (name == L"color") {		
				XRGBColor color = attr.Get<XRGBColor>();
				control->SetColor(color, kDontRedraw);
							  												  												  															  					
			} else if (name == L"prompt") {			
				control->SetPrompt(attr.GetValue());
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IColorSwatch!");
		}
	}
}


//---------------------------------------------------------------
//
// LoadGroupBox
//
//---------------------------------------------------------------
void LoadGroupBox(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadingBoss) {
		uint32 style, procID;
		std::wstring text;
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"style") {			
				style = NameToStyle(value);	
							  												  					
			} else if (name == L"proc_id") {			
				procID = NameToProcID(value);	
							  															  					
			} else if (name == L"text") {			
				text = value;
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IGroupBox!");
		}
		
		IGroupBoxPtr control(boss, interface);
		control->Init(parent, L"dummy name", XRect(0, 0, 1, 1), text, style, procID);	// name is set by LoadControl, frame is set by LoadGeometry
	}
}


//---------------------------------------------------------------
//
// LoadGroupControl
//
//---------------------------------------------------------------
void LoadGroupControl(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadingBoss) {
		uint32 style, procID;
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"style") {			
				style = NameToStyle(value);	
							  												  					
			} else if (name == L"proc_id") {			
				procID = NameToProcID(value);	
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IGroupControl!");
		}
		
		IGroupControlPtr control(boss, interface);
		control->Init(parent, L"dummy name", XRect(0, 0, 1, 1), style, procID);	// name is set by LoadControl, frame is set by LoadGeometry
	}
}


//---------------------------------------------------------------
//
// LoadListBox
//
//---------------------------------------------------------------
void LoadListBox(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadingBoss) {
		uint32 style, procID;
		SListBoxInfo info;
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"style") {			
				style = NameToStyle(value);	
							  												  					
			} else if (name == L"proc_id") {			
				procID = NameToProcID(value);	
							  															  					
			} else if (name == L"has_vert_scrollbar") {			
				info.hasVertScrollBar = attr.Get<bool>();
							  															  					
			} else if (name == L"has_horz_scrollbar") {			
				info.hasHorzScrollBar = attr.Get<bool>();
							  															  					
			} else if (name == L"multiple_selections") {			
				info.hasMultipleSelections = attr.Get<bool>();
							  															  					
			} else if (name == L"use_type_ahead") {			
				info.useTypeAhead = attr.Get<bool>();
							  															  												  															  					
			} else if (name == L"has_grow_box") {			
				info.hasGrowBox = attr.Get<bool>();
							  															  												  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IListBox!");
		}
		
		IListBoxPtr control(boss, interface);
		control->Init(parent, L"dummy name", XRect(0, 0, 1, 1), info, style, procID);	// name is set by LoadControl, frame is set by LoadGeometry
			
		XXMLElement::const_item_iterator iter2 = element.item_begin();	
		while (iter2 != element.item_end()) {
			const XXMLItem* item = *iter2;
			++iter2;
		
			const XXMLElement* entry = dynamic_cast<const XXMLElement*>(item);
			ASSERT(entry != nil);
			
			item = entry->item_front();
			const XXMLCharData* data = dynamic_cast<const XXMLCharData*>(item);
			ASSERT(data != nil);
			
			control->AppendItem(LoadAppString(data->GetText()), kDontRedraw);
		}	
	}
}


//---------------------------------------------------------------
//
// LoadProgressBar
//
//---------------------------------------------------------------
void LoadProgressBar(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadingBoss) {
		uint32 style, procID;
		bool indeterminate;
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"style") {			
				style = NameToStyle(value);	
							  												  					
			} else if (name == L"proc_id") {			
				procID = NameToProcID(value);	
							  															  					
			} else if (name == L"indeterminate") {			
				indeterminate = attr.Get<bool>();
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IProgressBar!");
		}
		
		IProgressBarPtr control(boss, interface);
		control->Init(parent, L"dummy name", XRect(0, 0, 1, 1), 0, 0, style, procID);	// name is set by LoadControl, frame is set by LoadGeometry
	
		if (indeterminate)
			control->MakeIndeterminate();
	}
}


//---------------------------------------------------------------
//
// LoadPushButton
//
//---------------------------------------------------------------
void LoadPushButton(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadingBoss) {
		uint32 style, procID;
		std::wstring text;
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"style") {			
				style = NameToStyle(value);	
							  												  					
			} else if (name == L"proc_id") {			
				procID = NameToProcID(value);	
							  															  					
			} else if (name == L"text") {			
				text = value;
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IPushButton!");
		}
		
		IPushButtonPtr control(boss, interface);
		control->Init(parent, L"dummy name", XRect(0, 0, 1, 1), text, style, procID);	// name is set by LoadControl, frame is set by LoadGeometry
	}
}


//---------------------------------------------------------------
//
// LoadRadioButton
//
//---------------------------------------------------------------
void LoadRadioButton(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadingBoss) {
		uint32 style, procID;
		std::wstring text, family;
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"style") {			
				style = NameToStyle(value);	
							  												  					
			} else if (name == L"proc_id") {			
				procID = NameToProcID(value);	
							  															  					
			} else if (name == L"text") {			
				text = value;
							  															  					
			} else if (name == L"family") {			
				family = value;
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IRadioButton!");
		}
		
		IRadioButtonPtr control(boss, interface);
		control->Init(parent, L"dummy name", XRect(0, 0, 1, 1), text, family, style, procID);	// name is set by LoadControl, frame is set by LoadGeometry
	}
}


//---------------------------------------------------------------
//
// LoadRootControl
//
//---------------------------------------------------------------
void LoadRootControl(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() == element.attr_end());		// we have no attributes
	
	if (pass == kLoadingBoss) {
		IRootControlPtr control(boss, interface);
		control->Init(parent, L"dummy name");				// name is set by LoadControl
	}
}


//---------------------------------------------------------------
//
// LoadScrollableControl
//
//---------------------------------------------------------------
void LoadScrollableControl(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadedBoss) {
		IScrollableControlPtr control(boss, interface);
		control->Init();	
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			bool value = attr.Get<bool>();
		
			if (name == L"adjust_origin") {			
				control->SetAdjustOrigin(value);	
							  												  					
			} else if (name == L"reconcile_overhang") {			
				control->SetReconcileOverhang(value);	
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IScrollableControl!");
		}
	}
}


//---------------------------------------------------------------
//
// LoadScrollBar
//
//---------------------------------------------------------------
void LoadScrollBar(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadingBoss) {
		uint32 style, procID;
		int32 arrowDelta, pageDelta;
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
		
			if (name == L"style") {			
				style = NameToStyle(attr.GetValue());	
							  												  					
			} else if (name == L"proc_id") {			
				procID = NameToProcID(attr.GetValue());	
							  															  					
			} else if (name == L"page_delta") {			
				pageDelta = attr.Get<int32>();	
							  															  					
			} else if (name == L"arrow_delta") {			
				arrowDelta = attr.Get<int32>();	
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IScrollBar!");
		}
		
		IScrollBarPtr control(boss, interface);
		control->Init(parent, L"dummy name", XRect(0, 0, 1, 1), 0, 0, style, procID);	// name is set by LoadControl, frame is set by LoadGeometry, min and max value are set by IValueControl
	
		control->SetPageDelta(pageDelta);
		control->SetArrowDelta(arrowDelta);
	}
}


//---------------------------------------------------------------
//
// LoadScrollerControl
//
//---------------------------------------------------------------
void LoadScrollerControl(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadedBoss) {
		bool hasHorz, hasVert, fillsWindow;
		XRect indent;
		XSize scrollUnits;
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
		
			if (name == L"has_horz_bar") {			
				hasHorz = attr.Get<bool>();
							  												  					
			} else if (name == L"has_vert_bar") {			
				hasVert = attr.Get<bool>();
							  															  					
			} else if (name == L"left_indent") {			
				indent.left = attr.Get<int32>();
							  															  					
			} else if (name == L"right_indent") {			
				indent.right = attr.Get<int32>();
							  															  					
			} else if (name == L"top_indent") {			
				indent.top = attr.Get<int32>();
							  															  					
			} else if (name == L"bottom_indent") {			
				indent.bottom = attr.Get<int32>();
							  															  					
			} else if (name == L"fills_window") {			
				fillsWindow = attr.Get<bool>();
							  															  					
			} else if (name == L"scroll_units") {			
				scrollUnits = attr.Get<XSize>();
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IScrollerControl!");
		}

		IScrollerControlPtr control(boss, interface);
		control->Init(hasVert, hasHorz, indent, fillsWindow);
		
		control->SetScrollUnits(scrollUnits);
	}
}


//---------------------------------------------------------------
//
// LoadSeperator
//
//---------------------------------------------------------------
void LoadSeperator(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadingBoss) {
		uint32 style, procID;
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"style") {			
				style = NameToStyle(value);	
							  												  					
			} else if (name == L"proc_id") {			
				procID = NameToProcID(value);	
							  															  												  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of ISeperator!");
		}
		
		ISeperatorPtr control(boss, interface);
		control->Init(parent, L"dummy name", XRect(0, 0, 1, 1), style, procID);	// name is set by LoadControl, frame is set by LoadGeometry
	}
}


//---------------------------------------------------------------
//
// LoadStaticText
//
//---------------------------------------------------------------
void LoadStaticText(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadingBoss) {
		uint32 style, procID;
		std::wstring text;
		ETextAlignment alignment = kDefaultAlign;
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"style") {			
				style = NameToStyle(value);	
							  												  					
			} else if (name == L"proc_id") {			
				procID = NameToProcID(value);	
							  															  					
			} else if (name == L"text") {			
				text = value;	
							  															  					
			} else if (name == L"alignment") {			
				if (value == L"left_top")
					alignment = kLeftTopAlign;
				else if (value == L"center_top")
					alignment = kCenterTopAlign;
				else if (value == L"right_top")
					alignment = kRightTopAlign;
				else if (value == L"default")
					alignment = kDefaultAlign;
				else 
					DEBUGSTR(value + L" isn't a valid IStaticText alignment attribute!");
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IStaticText!");
		}
		
		XFont font(kSystemFont);
		if (element.item_begin() != element.item_end()) {
			const XXMLItem* item = element.item_front();
			const XXMLElement* child = dynamic_cast<const XXMLElement*>(item);
			ASSERT(child != nil);
			ASSERT(child->GetName() == L"XFont");
			
			XXMLDoc doc;								// yuck
			font.HandleStreamIn(doc, *child);
		}

		IStaticTextPtr control(boss, interface);
		control->Init(parent, L"dummy name", XRect(0, 0, 1, 1), text, font, style, procID);	// name is set by LoadControl, frame is set by LoadGeometry
		
		control->SetAlignment(alignment, kDontRedraw);
	}
}


//---------------------------------------------------------------
//
// LoadTextBox
//
//---------------------------------------------------------------
void LoadTextBox(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadingBoss) {
		uint32 style, procID;
		std::wstring text;
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"style") {			
				style = NameToStyle(value);	
							  												  					
			} else if (name == L"proc_id") {			
				procID = NameToProcID(value);	
							  															  					
			} else if (name == L"text") {			
				text = value;	
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of ITextBox!");
		}
		
		ITextBoxPtr control(boss, interface);
		control->Init(parent, L"dummy name", XRect(0, 0, 16, 16), text, style, procID);	// name is set by LoadControl, frame is set by LoadGeometry
	}
}


//---------------------------------------------------------------
//
// LoadUserControl
//
//---------------------------------------------------------------
void LoadUserControl(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadingBoss) {
		uint32 style, procID;
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"style") {			
				style = NameToStyle(value);	
							  												  					
			} else if (name == L"proc_id") {			
				procID = NameToProcID(value);	
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IUserControl!");
		}
		
		IUserControlPtr control(boss, interface);
		control->Init(parent, L"dummy name", XRect(0, 0, 1, 1), style, procID);	// name is set by LoadControl, frame is set by LoadGeometry
	}
}


//---------------------------------------------------------------
//
// LoadValueControl
//
//---------------------------------------------------------------
void LoadValueControl(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);
	
	ASSERT(element.item_begin() == element.item_end());		// we have no child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
	
	if (pass == kLoadedBoss) {
		int32 value, minValue, maxValue;
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			int32 n = attr.Get<int32>();
		
			if (name == L"value") {			
				value = n;
							  												  					
			} else if (name == L"min_value") {			
				minValue = n;
							  															  					
			} else if (name == L"max_value") {			
				maxValue = n;
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IValueControl!");
		}
		
		IValueControlPtr control(boss, interface);
		control->SetMinMaxValue(minValue, maxValue, kDontRedraw);	
		control->SetValue(value, kDontRedraw);	
	}
}


}	// namespace Whisper
