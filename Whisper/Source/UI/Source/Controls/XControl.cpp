/*
 *  File:       XControl.cpp
 *  Summary:   	Base class for both OS and user widgets.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XControl.cpp,v $
 *		Revision 1.3  2001/04/27 04:31:25  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:28:55  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XControl.h>

#include <XAutoPtr.h>
#include <XExtension.h>
#include <XStringUtils.h>
#include <XXMLDoc.h>

#if WIN
	#include <commctrl.h>
#endif

namespace Whisper {


// ===================================================================================
//	class ZAtributeMatcher
// ===================================================================================
class ZAtributeMatcher {

public:
  						ZAtributeMatcher(const std::wstring& name) : mName(name)	{}
  						
			bool		operator()(const XXMLAttribute& value) const	{return value.GetName() == mName;}
			
private:
	std::wstring		mName;
};


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
// ProcIDToName
//
//---------------------------------------------------------------
static std::wstring ProcIDToName(uint32 procID)
{	
	std::wstring name;
	
	// Bevel button
	if (procID == kControlBevelButtonSmallBevelProc)
		name = L"kControlBevelButtonSmallBevelProc";
		
	else if (procID == kControlBevelButtonSmallBevelProc+kControlBevelButtonMenuOnRight)
		name = L"kControlBevelButtonSmallBevelProc+kControlBevelButtonMenuOnRight";
		
	else if (procID == kControlBevelButtonNormalBevelProc)
		name = L"kControlBevelButtonNormalBevelProc";
		
	else if (procID == kControlBevelButtonNormalBevelProc+kControlBevelButtonMenuOnRight)
		name = L"kControlBevelButtonNormalBevelProc+kControlBevelButtonMenuOnRight";
		
	else if (procID == kControlBevelButtonLargeBevelProc)
		name = L"kControlBevelButtonLargeBevelProc";
		
	else if (procID == kControlBevelButtonLargeBevelProc+kControlBevelButtonMenuOnRight)
		name = L"kControlBevelButtonLargeBevelProc+kControlBevelButtonMenuOnRight";
						
	// Progress Bar
	else if (procID == kControlProgressBarProc) {
		name = L"kControlProgressBarProc";
			
	// Little Arrows
	} else if (procID == kControlLittleArrowsProc) {
		name = L"kControlLittleArrowsProc";
			
	// Chasing Arrows
	} else if (procID == kControlChasingArrowsProc) {
		name = L"kControlChasingArrowsProc";
		
	// Tabs
	} else if (procID == kControlTabLargeProc) {
		name = L"kControlTabLargeProc";

	} else if (procID == kControlTabSmallProc) {
		name = L"kControlTabSmallProc";
	
	} else if (procID == kControlTabLargeNorthProc) {
		name = L"kControlTabLargeNorthProc";
	
	} else if (procID == kControlTabSmallNorthProc) {
		name = L"kControlTabSmallNorthProc";
	
	} else if (procID == kControlTabLargeSouthProc) {
		name = L"kControlTabLargeSouthProc";
	
	} else if (procID == kControlTabSmallSouthProc) {
		name = L"kControlTabSmallSouthProc";
	
	} else if (procID == kControlTabLargeEastProc) {
		name = L"kControlTabLargeEastProc";
	
	} else if (procID == kControlTabSmallEastProc) {
		name = L"kControlTabSmallEastProc";
		
	} else if (procID == kControlTabLargeWestProc) {
		name = L"kControlTabLargeWestProc";
		
	} else if (procID == kControlTabSmallWestProc) {
		name = L"kControlTabSmallWestProc";
			
	// Visual separator
	} else if (procID == kControlSeparatorLineProc) {
		name = L"kControlSeparatorLineProc";
			
	// Group Box
	} else if (procID == kControlGroupBoxTextTitleProc) {
		name = L"kControlGroupBoxTextTitleProc";

	} else if (procID == kControlGroupBoxCheckBoxProc) {
		name = L"kControlGroupBoxCheckBoxProc";

	} else if (procID == kControlGroupBoxPopupButtonProc) {
		name = L"kControlGroupBoxPopupButtonProc";

	} else if (procID == kControlGroupBoxSecondaryTextTitleProc) {
		name = L"kControlGroupBoxSecondaryTextTitleProc";

	} else if (procID == kControlGroupBoxSecondaryCheckBoxProc) {
		name = L"kControlGroupBoxSecondaryCheckBoxProc";

	} else if (procID == kControlGroupBoxSecondaryPopupButtonProc) {
		name = L"kControlGroupBoxSecondaryPopupButtonProc";
			
	// Image Well
	} else if (procID == kControlImageWellProc) {
		name = L"kControlImageWellProc";
			
	// Popup Arrow
	} else if (procID == kControlPopupArrowEastProc) {
		name = L"kControlPopupArrowEastProc";

	} else if (procID == kControlPopupArrowWestProc) {
		name = L"kControlPopupArrowWestProc";

	} else if (procID == kControlPopupArrowNorthProc) {
		name = L"kControlPopupArrowNorthProc";

	} else if (procID == kControlPopupArrowSouthProc) {
		name = L"kControlPopupArrowSouthProc";

	} else if (procID == kControlPopupArrowSmallEastProc) {
		name = L"kControlPopupArrowSmallEastProc";

	} else if (procID == kControlPopupArrowSmallWestProc) {
		name = L"kControlPopupArrowSmallWestProc";

	} else if (procID == kControlPopupArrowSmallNorthProc) {
		name = L"kControlPopupArrowSmallNorthProc";

	} else if (procID == kControlPopupArrowSmallSouthProc) {
		name = L"kControlPopupArrowSmallSouthProc";
			
	// Placard 
	} else if (procID == kControlPlacardProc) {
		name = L"kControlPlacardProc";
			
	// Clock
	} else if (procID == kControlClockTimeProc) {
		name = L"kControlClockTimeProc";

	} else if (procID == kControlClockTimeSecondsProc) {
		name = L"kControlClockTimeSecondsProc";

	} else if (procID == kControlClockDateProc) {
		name = L"kControlClockDateProc";

	} else if (procID == kControlClockMonthYearProc) {
		name = L"kControlClockMonthYearProc";
			
	// User Pane
	} else if (procID == kControlUserPaneProc) {
		name = L"kControlUserPaneProc";
			
	// Edit Text
	} else if (procID == kControlEditTextProc) {
		name = L"kControlEditTextProc";

	} else if (procID == kControlEditTextPasswordProc) {
		name = L"kControlEditTextPasswordProc";

	} else if (procID == kControlEditTextInlineInputProc) {
		name = L"kControlEditTextInlineInputProc";

	// Static Text
	} else if (procID == kControlStaticTextProc) {
		name = L"kControlStaticTextProc";
			
	// Picture
	} else if (procID == kControlPictureProc) {
		name = L"kControlPictureProc";

	} else if (procID == kControlPictureNoTrackProc) {
		name = L"kControlPictureNoTrackProc";
			
	// Icon
	} else if (procID == kControlIconProc) {
		name = L"kControlIconProc";

	} else if (procID == kControlIconNoTrackProc) {
		name = L"kControlIconNoTrackProc";

	} else if (procID == kControlIconSuiteProc) {
		name = L"kControlIconSuiteProc";

	} else if (procID == kControlIconSuiteNoTrackProc) {
		name = L"kControlIconSuiteNoTrackProc";

	} else if (procID == kControlIconRefProc) {
		name = L"kControlIconRefProc";

	} else if (procID == kControlIconRefNoTrackProc) {
		name = L"kControlIconRefNoTrackProc";
			
	// Window Header
	} else if (procID == kControlWindowHeaderProc) {
		name = L"kControlWindowHeaderProc";

	} else if (procID == kControlWindowListViewHeaderProc) {
		name = L"kControlWindowListViewHeaderProc";
			
	// List Box
	} else if (procID == kControlListBoxProc) {
		name = L"kControlListBoxProc";

	} else if (procID == kControlListBoxAutoSizeProc) {
		name = L"kControlListBoxAutoSizeProc";
			
	// Theme Push Button/Check Box/Radio Button
	} else if (procID == kControlPushButtonProc) {
		name = L"kControlPushButtonProc";

	} else if (procID == kControlCheckBoxProc) {
		name = L"kControlCheckBoxProc";

	} else if (procID == kControlRadioButtonProc) {
		name = L"kControlRadioButtonProc";

	} else if (procID == kControlPushButLeftIconProc) {
		name = L"kControlPushButLeftIconProc";

	} else if (procID == kControlPushButRightIconProc) {
		name = L"kControlPushButRightIconProc";

	} else if (procID == kControlCheckBoxAutoToggleProc) {
		name = L"kControlCheckBoxAutoToggleProc";

	} else if (procID == kControlRadioButtonAutoToggleProc) {
		name = L"kControlRadioButtonAutoToggleProc";
			
	// Theme Scroll Bar
	} else if (procID == kControlScrollBarProc) {
		name = L"kControlScrollBarProc";

	} else if (procID == kControlScrollBarLiveProc) {
		name = L"kControlScrollBarLiveProc";

	// Radio Group
	} else if (procID == kControlRadioGroupProc) {
		name = L"kControlRadioGroupProc";
			
	// Scrolling Textbox
	} else if (procID == kControlScrollTextBoxProc) {
		name = L"kControlScrollTextBoxProc";

	} else if (procID == kControlScrollTextBoxAutoScrollProc) {
		name = L"kControlScrollTextBoxAutoScrollProc";
			
	// Theme Popup Button
	} else if ((procID & kControlPopupButtonProc) == kControlPopupButtonProc) {
		name = L"kControlPopupButtonProc";
	
		if ((procID & kControlPopupFixedWidthVariant) == kControlPopupFixedWidthVariant) 
			name += L"+kControlPopupFixedWidthVariant";
		if ((procID & kControlPopupVariableWidthVariant) == kControlPopupVariableWidthVariant) 
			name += L"+kControlPopupVariableWidthVariant";
		if ((procID & kControlPopupUseAddResMenuVariant) == kControlPopupUseAddResMenuVariant) 
			name += L"+kControlPopupUseAddResMenuVariant";
		if ((procID & kControlPopupUseWFontVariant) == kControlPopupUseWFontVariant) 
			name += L"+kControlPopupUseWFontVariant";
			
	// Triangle
	} else if ((procID & kControlTriangleLeftFacingProc) == kControlTriangleLeftFacingProc) {
		name = L"kControlTriangleLeftFacingProc";
	
		if ((procID & kControlTriangleAutoToggleProc) == kControlTriangleAutoToggleProc) 
			name += L"+kControlTriangleAutoToggleProc";
		if ((procID & kControlTriangleLeftFacingAutoToggleProc) == kControlTriangleLeftFacingAutoToggleProc) 
			name += L"+kControlTriangleLeftFacingAutoToggleProc";
					
	// Slider	
	} else if ((procID & kControlSliderProc) == kControlSliderProc) {
		name = L"kControlSliderProc";
	
		if ((procID & kControlSliderLiveFeedback) == kControlSliderLiveFeedback) 
			name += L"+kControlSliderLiveFeedback";
		if ((procID & kControlSliderHasTickMarks) == kControlSliderHasTickMarks) 
			name += L"+kControlSliderHasTickMarks";
		if ((procID & kControlSliderReverseDirection) == kControlSliderReverseDirection) 
			name += L"+kControlSliderReverseDirection";
		if ((procID & kControlSliderNonDirectional) == kControlSliderNonDirectional) 
			name += L"+kControlSliderNonDirectional";
		
	} else
		DEBUGSTR("Bad name in ProcIDToName");
	
	return name;
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


//---------------------------------------------------------------
//
// StyleToName
//
//---------------------------------------------------------------
static std::wstring StyleToName(const std::wstring& className, uint32 style)
{	
	std::wstring name;
	
	// Window Styles
	if ((style & WS_CHILD) == WS_CHILD)
		name += L"WS_CHILD+";
	if ((style & WS_VISIBLE) == WS_VISIBLE)
		name += L"WS_VISIBLE+";
	if ((style & WS_DISABLED) == WS_DISABLED)
		name += L"WS_DISABLED+";
	if ((style & WS_CLIPSIBLINGS) == WS_CLIPSIBLINGS)
		name += L"WS_CLIPSIBLINGS+";
	if ((style & WS_CLIPCHILDREN) == WS_CLIPCHILDREN)
		name += L"WS_CLIPCHILDREN+";
	if ((style & WS_CAPTION) == WS_CAPTION)
		name += L"WS_CAPTION+";
	if ((style & WS_BORDER) == WS_BORDER)
		name += L"WS_BORDER+";
	if ((style & WS_THICKFRAME) == WS_THICKFRAME)
		name += L"WS_THICKFRAME+";
	if ((style & WS_GROUP) == WS_GROUP)
		name += L"WS_GROUP+";
	if ((style & WS_TABSTOP) == WS_TABSTOP)
		name += L"WS_TABSTOP+";
		
	// Button Control Styles
	if (className == L"BUTTON") {
		if ((style & BS_OWNERDRAW) == BS_OWNERDRAW)
			name += L"BS_OWNERDRAW+";
		else if ((style & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON)
			name += L"BS_AUTORADIOBUTTON+";
		else if ((style & BS_USERBUTTON) == BS_USERBUTTON)
			name += L"BS_USERBUTTON+";
		else if ((style & BS_GROUPBOX) == BS_GROUPBOX)
			name += L"BS_GROUPBOX+";
		else if ((style & BS_AUTO3STATE) == BS_AUTO3STATE)
			name += L"BS_AUTO3STATE+";
		else if ((style & BS_3STATE) == BS_3STATE)
			name += L"BS_3STATE+";
		else if ((style & BS_RADIOBUTTON) == BS_RADIOBUTTON)
			name += L"BS_RADIOBUTTON+";
		else if ((style & BS_AUTOCHECKBOX) == BS_AUTOCHECKBOX)
			name += L"BS_AUTOCHECKBOX+";
		else if ((style & BS_CHECKBOX) == BS_CHECKBOX)
			name += L"BS_CHECKBOX+";
		else if ((style & BS_DEFPUSHBUTTON) == BS_DEFPUSHBUTTON)
			name += L"BS_DEFPUSHBUTTON+";
			
		if ((style & BS_LEFTTEXT) == BS_LEFTTEXT)
			name += L"BS_LEFTTEXT+";
		if ((style & BS_ICON) == BS_ICON)
			name += L"BS_ICON+";
		if ((style & BS_BITMAP) == BS_BITMAP)
			name += L"BS_BITMAP+";
		if ((style & BS_LEFT) == BS_LEFT)
			name += L"BS_LEFT+";
		if ((style & BS_RIGHT) == BS_RIGHT)
			name += L"BS_RIGHT+";
		if ((style & BS_CENTER) == BS_CENTER)
			name += L"BS_CENTER+";
		if ((style & BS_TOP) == BS_TOP)
			name += L"BS_TOP+";
		if ((style & BS_BOTTOM) == BS_BOTTOM)
			name += L"BS_BOTTOM+";
		if ((style & BS_VCENTER) == BS_VCENTER)
			name += L"BS_VCENTER+";
		if ((style & BS_PUSHLIKE) == BS_PUSHLIKE)
			name += L"BS_PUSHLIKE+";
		if ((style & BS_MULTILINE) == BS_MULTILINE)
			name += L"BS_MULTILINE+";
		if ((style & BS_NOTIFY) == BS_NOTIFY)
			name += L"BS_NOTIFY+";
		if ((style & BS_FLAT) == BS_FLAT)
			name += L"BS_FLAT+";

	// Scroll Bar Styles
	} else if (className == L"SCROLLBAR") {
		if ((style & SBS_VERT) == SBS_VERT)
			name += L"SBS_VERT+";
		if ((style & SBS_LEFTALIGN) == SBS_LEFTALIGN)
			name += L"SBS_LEFTALIGN+";
		if ((style & SBS_RIGHTALIGN) == SBS_RIGHTALIGN)
			name += L"SBS_RIGHTALIGN+";
		if ((style & SBS_SIZEBOX) == SBS_SIZEBOX)
			name += L"SBS_SIZEBOX+";
		if ((style & SBS_SIZEGRIP) == SBS_SIZEGRIP)
			name += L"SBS_SIZEGRIP+";

	// Combo Box styles
	} else if (className == L"COMBOBOX") {
		if ((style & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST)
			name += L"CBS_DROPDOWNLIST+";
		else if ((style & CBS_DROPDOWN) == CBS_DROPDOWN)
			name += L"CBS_DROPDOWN+";
		else if ((style & CBS_SIMPLE) == CBS_SIMPLE)
			name += L"CBS_SIMPLE+";
			
		if ((style & CBS_OWNERDRAWFIXED) == CBS_OWNERDRAWFIXED)
			name += L"CBS_OWNERDRAWFIXED+";
		if ((style & CBS_OWNERDRAWVARIABLE) == CBS_OWNERDRAWVARIABLE)
			name += L"CBS_OWNERDRAWVARIABLE+";
		if ((style & CBS_AUTOHSCROLL) == CBS_AUTOHSCROLL)
			name += L"CBS_AUTOHSCROLL+";
		if ((style & CBS_OEMCONVERT) == CBS_OEMCONVERT)
			name += L"CBS_OEMCONVERT+";
		if ((style & CBS_SORT) == CBS_SORT)
			name += L"CBS_SORT+";
		if ((style & CBS_HASSTRINGS) == CBS_HASSTRINGS)
			name += L"CBS_HASSTRINGS+";
		if ((style & CBS_NOINTEGRALHEIGHT) == CBS_NOINTEGRALHEIGHT)
			name += L"CBS_NOINTEGRALHEIGHT+";
		if ((style & CBS_DISABLENOSCROLL) == CBS_DISABLENOSCROLL)
			name += L"CBS_DISABLENOSCROLL+";
		if ((style & CBS_UPPERCASE) == CBS_UPPERCASE)
			name += L"CBS_UPPERCASE+";
		if ((style & CBS_LOWERCASE) == CBS_LOWERCASE)
			name += L"CBS_LOWERCASE+";
	
	// Listbox Styles
	} else if (className == L"LISTBOX") {
		if ((style & LBS_NOTIFY) == LBS_NOTIFY)
			name += L"LBS_NOTIFY+";
		if ((style & LBS_SORT) == LBS_SORT)
			name += L"LBS_SORT+";
		if ((style & LBS_NOREDRAW) == LBS_NOREDRAW)
			name += L"LBS_NOREDRAW+";
		if ((style & LBS_MULTIPLESEL) == LBS_MULTIPLESEL)
			name += L"LBS_MULTIPLESEL+";
		if ((style & LBS_OWNERDRAWFIXED) == LBS_OWNERDRAWFIXED)
			name += L"LBS_OWNERDRAWFIXED+";
		if ((style & LBS_OWNERDRAWVARIABLE) == LBS_OWNERDRAWVARIABLE)
			name += L"LBS_OWNERDRAWVARIABLE+";
		if ((style & LBS_HASSTRINGS) == LBS_HASSTRINGS)
			name += L"LBS_HASSTRINGS+";
		if ((style & LBS_USETABSTOPS) == LBS_USETABSTOPS)
			name += L"LBS_USETABSTOPS+";
		if ((style & LBS_NOINTEGRALHEIGHT) == LBS_NOINTEGRALHEIGHT)
			name += L"LBS_NOINTEGRALHEIGHT+";
		if ((style & LBS_MULTICOLUMN) == LBS_MULTICOLUMN)
			name += L"LBS_MULTICOLUMN+";
		if ((style & LBS_WANTKEYBOARDINPUT) == LBS_WANTKEYBOARDINPUT)
			name += L"LBS_WANTKEYBOARDINPUT+";
		if ((style & LBS_EXTENDEDSEL) == LBS_EXTENDEDSEL)
			name += L"LBS_EXTENDEDSEL+";
		if ((style & LBS_DISABLENOSCROLL) == LBS_DISABLENOSCROLL)
			name += L"LBS_DISABLENOSCROLL+";
		if ((style & LBS_NODATA) == LBS_NODATA)
			name += L"LBS_NODATA+";
		if ((style & LBS_NOSEL) == LBS_NOSEL)
			name += L"LBS_NOSEL+";

	// Static Control Styles
	} else if (className == L"STATIC") {
		if ((style & SS_TYPEMASK) == SS_TYPEMASK)
			name += L"SS_TYPEMASK+";
		else if ((style & SS_ETCHEDFRAME) == SS_ETCHEDFRAME)
			name += L"SS_ETCHEDFRAME+";
		else if ((style & SS_ETCHEDVERT) == SS_ETCHEDVERT)
			name += L"SS_ETCHEDVERT+";
		else if ((style & SS_ETCHEDHORZ) == SS_ETCHEDHORZ)
			name += L"SS_ETCHEDHORZ+";
		else if ((style & SS_ENHMETAFILE) == SS_ENHMETAFILE)
			name += L"SS_ENHMETAFILE+";
		else if ((style & SS_BITMAP) == SS_BITMAP)
			name += L"SS_BITMAP+";
		else if ((style & SS_OWNERDRAW) == SS_OWNERDRAW)
			name += L"SS_OWNERDRAW+";
		else if ((style & SS_LEFTNOWORDWRAP) == SS_LEFTNOWORDWRAP)
			name += L"SS_LEFTNOWORDWRAP+";
		else if ((style & SS_SIMPLE) == SS_SIMPLE)
			name += L"SS_SIMPLE+";
		else if ((style & SS_USERITEM) == SS_USERITEM)
			name += L"SS_USERITEM+";
		else if ((style & SS_WHITEFRAME) == SS_WHITEFRAME)
			name += L"SS_WHITEFRAME+";
		else if ((style & SS_GRAYFRAME) == SS_GRAYFRAME)
			name += L"SS_GRAYFRAME+";
		else if ((style & SS_BLACKFRAME) == SS_BLACKFRAME)
			name += L"SS_BLACKFRAME+";
		else if ((style & SS_WHITERECT) == SS_WHITERECT)
			name += L"SS_WHITERECT+";
		else if ((style & SS_GRAYRECT) == SS_GRAYRECT)
			name += L"SS_GRAYRECT+";
		else if ((style & SS_BLACKRECT) == SS_BLACKRECT)
			name += L"SS_BLACKRECT+";
		else if ((style & SS_ICON) == SS_ICON)
			name += L"SS_ICON+";
		else if ((style & SS_CENTER) == SS_CENTER)
			name += L"SS_CENTER+";
		else if ((style & SS_CENTER) == SS_CENTER)
			name += L"SS_CENTER+";
			
		if ((style & SS_NOPREFIX) == SS_NOPREFIX)
			name += L"SS_NOPREFIX+";
		if ((style & SS_NOTIFY) == SS_NOTIFY)
			name += L"SS_NOTIFY+";
		if ((style & SS_CENTERIMAGE) == SS_CENTERIMAGE)
			name += L"SS_CENTERIMAGE+";
		if ((style & SS_RIGHTJUST) == SS_RIGHTJUST)
			name += L"SS_RIGHTJUST+";
		if ((style & SS_REALSIZEIMAGE) == SS_REALSIZEIMAGE)
			name += L"SS_REALSIZEIMAGE+";
		if ((style & SS_SUNKEN) == SS_SUNKEN)
			name += L"SS_SUNKEN+";
		if ((style & SS_ENDELLIPSIS) == SS_ENDELLIPSIS)
			name += L"SS_ENDELLIPSIS+";
		if ((style & SS_PATHELLIPSIS) == SS_PATHELLIPSIS)
			name += L"SS_PATHELLIPSIS+";
		if ((style & SS_WORDELLIPSIS) == SS_WORDELLIPSIS)
			name += L"SS_WORDELLIPSIS+";
		if ((style & SS_ELLIPSISMASK) == SS_ELLIPSISMASK)
			name += L"SS_ELLIPSISMASK+";

	// Edit Control Styles
	} else if (className == L"EDIT") {
//		if ((style & ES_LEFT) == ES_LEFT)			// ES_LEFT == 0
//			name += L"ES_LEFT+";
		if ((style & ES_CENTER) == ES_CENTER)
			name += L"ES_CENTER+";
		if ((style & ES_RIGHT) == ES_RIGHT)
			name += L"ES_RIGHT+";
		if ((style & ES_MULTILINE) == ES_MULTILINE)
			name += L"ES_MULTILINE+";
		if ((style & ES_UPPERCASE) == ES_UPPERCASE)
			name += L"ES_UPPERCASE+";
		if ((style & ES_LOWERCASE) == ES_LOWERCASE)
			name += L"ES_LOWERCASE+";
		if ((style & ES_PASSWORD) == ES_PASSWORD)
			name += L"ES_PASSWORD+";
		if ((style & ES_AUTOVSCROLL) == ES_AUTOVSCROLL)
			name += L"ES_AUTOVSCROLL+";
		if ((style & ES_AUTOHSCROLL) == ES_AUTOHSCROLL)
			name += L"ES_AUTOHSCROLL+";
		if ((style & ES_NOHIDESEL) == ES_NOHIDESEL)
			name += L"ES_NOHIDESEL+";
		if ((style & ES_OEMCONVERT) == ES_OEMCONVERT)
			name += L"ES_OEMCONVERT+";
		if ((style & ES_READONLY) == ES_READONLY)
			name += L"ES_READONLY+";
		if ((style & ES_WANTRETURN) == ES_WANTRETURN)
			name += L"ES_WANTRETURN+";
		if ((style & ES_NUMBER) == ES_NUMBER)
			name += L"ES_NUMBER+";
		
	// Trackbar Control Styles
	} else if (className == L"msctls_trackbar32") {
		if ((style & TBS_AUTOTICKS) == TBS_AUTOTICKS)
			name += L"TBS_AUTOTICKS+";
		if ((style & TBS_VERT) == TBS_VERT)
			name += L"TBS_VERT+";
		if ((style & TBS_LEFT) == TBS_LEFT)
			name += L"TBS_LEFT+";
		if ((style & TBS_BOTH) == TBS_BOTH)
			name += L"TBS_BOTH+";
		if ((style & TBS_NOTICKS) == TBS_NOTICKS)
			name += L"TBS_NOTICKS+";
		if ((style & TBS_ENABLESELRANGE) == TBS_ENABLESELRANGE)
			name += L"TBS_ENABLESELRANGE+";
		if ((style & TBS_FIXEDLENGTH) == TBS_FIXEDLENGTH)
			name += L"TBS_FIXEDLENGTH+";
		if ((style & TBS_NOTHUMB) == TBS_NOTHUMB)
			name += L"TBS_NOTHUMB+";
		if ((style & TBS_TOOLTIPS) == TBS_TOOLTIPS)
			name += L"TBS_TOOLTIPS+";
			
	// Progress Bar Control Styles
	} else if (className == L"msctls_progress32") {
			
	// Tab Control Styles
	} else if (className == L"SysTabControl32") {
	}
	
	POSTCONDITION(name.length() > 0);
	name.resize(name.length() - 1);
			
	return name;
}

#if __MWERKS__
#pragma mark -
#endif
	
// ===================================================================================
//	class XControl
// ===================================================================================

//---------------------------------------------------------------
//
// XControl::Create (XURI, wstring, XCreateByNameMixin*) [static]
//
//---------------------------------------------------------------
XControl* XControl::Create(const XURI& uri, const std::wstring& id, XCreateByNameMixin* parent)
{
	XXMLDoc doc;
	doc.Load(uri);
	
	XAutoPtr<XControl> control(XControl::Create(doc, id, parent));
	control->HandleUnflatten();
		
	return control.Release();
}


//---------------------------------------------------------------
//
// XControl::Create (XXMLDoc, wstring, XCreateByNameMixin*) [static]
//
//---------------------------------------------------------------
XControl* XControl::Create(const XXMLDoc& doc, const std::wstring& id, XCreateByNameMixin* parent)
{
	const XXMLElement* element = XBaseWindow::FindLocalizedElement(doc, id);		// will throw if id isn't found
	ASSERT(element != nil);

	XAutoPtr<XControl> control(XControl::Create(doc, *element, parent));
	control->HandleUnflatten();
	
	return control.Release();
}


//---------------------------------------------------------------
//
// XControl::Create (XXMLDoc, XXMLElement, XCreateByNameMixin*) [static]
//
// This is called as a nestable control streams in it's children
// so we don't want to call HandleUnflatten until the entire
// control hierachy is present.
//
//---------------------------------------------------------------
XControl* XControl::Create(const XXMLDoc& doc, const XXMLElement& element, XCreateByNameMixin* parent)
{
	XXMLPersistentMixin* object = XXMLPersistentMixin::Unflatten(doc, element, parent);
	XControl* control = dynamic_cast<XControl*>(object);
	
	if (control == nil) {
		delete object;
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: XControl::Create created an object that wasn't a control.")));
	}
		
	return control;
}


//---------------------------------------------------------------
//
// XControl::HandleUnflatten
//
//---------------------------------------------------------------
void XControl::HandleUnflatten()
{
	XDisableInvariant disabler(this);			// object isn't fully inited so we'll disable invariant checks
	
	this->OnUnflatten();
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XControl::OnStreamOut
//
//---------------------------------------------------------------
void XControl::OnStreamOut(XXMLDoc& doc, XXMLElement& parent) const
{
	UNUSED(doc);
	
	std::wstring ids;
	
	const_iterator iter = this->begin();
	while (iter != this->end()) {
		const XExtension* extension = *iter;
		++iter;
		
		if (extension->IsPersistant()) {
			XXMLPersistentMixin::Flatten(doc, *doc.GetRootElement(), extension);
			ids += extension->GetName() + L" ";
		}
	}
	
	parent.AppendAttribute(XXMLAttribute(L"style", StyleToName(mClass, mStyle)));		
	parent.AppendAttribute(XXMLAttribute(L"proc_id", ProcIDToName(mProcID)));		
	
	XAutoPtr<XXMLElement> info(new XXMLElement(L"XControlInfo"));
	
	info->AppendAttribute(XXMLAttribute(L"name", this->GetName()));	
	info->AppendAttribute(XXMLAttribute(L"location", this->GetLocation()));	
	info->AppendAttribute(XXMLAttribute(L"size", this->GetSize()));	

	info->AppendAttribute(XXMLAttribute(L"bind_left", mBinding.left));	
	info->AppendAttribute(XXMLAttribute(L"bind_right", mBinding.right));	
	info->AppendAttribute(XXMLAttribute(L"bind_top", mBinding.top));	
	info->AppendAttribute(XXMLAttribute(L"bind_bottom", mBinding.bottom));	
	
	info->AppendAttribute(XXMLAttribute(L"visible", this->IsVisible() || mWasVisible));	
	info->AppendAttribute(XXMLAttribute(L"enabled", this->IsEnabled()));	

	info->AppendAttribute(XXMLAttribute(L"help_mesg", mHelpMesg));	
	info->AppendAttribute(XXMLAttribute(L"disabled_help", mDisabledHelpMesg));	

	info->AppendAttribute(XXMLAttribute(L"draw_outset", mDrawOutset));	
	info->AppendAttribute(XXMLAttribute(L"help_id", mHelpID));	

	if (ids.length() > 0)	
		info->AppendAttribute(XXMLAttribute(L"extensions", ids));	

	parent.AppendItem(info.Release());
}


//---------------------------------------------------------------
//
// XControl::OnStreamIn
//
//---------------------------------------------------------------
void XControl::OnStreamIn(const XXMLDoc& doc, const XXMLElement& parent, const XXMLElement& element)
{
	UNUSED(doc);
	
	if (&parent == &element) {
		XXMLElement::const_attr_iterator iter = std::find_if(parent.attr_begin(), parent.attr_end(), ZAtributeMatcher(L"proc_id"));
		ASSERT(iter != parent.attr_end());		
		uint32 procID = NameToProcID(iter->GetValue());

		iter = std::find_if(parent.attr_begin(), parent.attr_end(), ZAtributeMatcher(L"style"));
		ASSERT(iter != parent.attr_end());		
		uint32 style = NameToStyle(iter->GetValue());

		OSControl control = nil;
#if MAC
		control = this->DoCreateControl(doc, parent, procID);
#elif WIN
		control = this->DoCreateControl(doc, parent, style);
#endif
		this->DoResetControl(control);
		
		mProcID = procID;
		mStyle = style;
	}
	
	if (element.GetName() == L"XControlInfo") {
		ASSERT(element.item_begin() == element.item_end());		// we don't have any child elements
		ASSERT(element.attr_begin() != element.attr_end());		// we do have required attributes
		
		this->DeleteExtensions();
		
		XPoint loc = kZeroPt;
		XSize size = kZeroSize;
			
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
		
			if (name == L"name") {			
				this->SetName(attr.GetValue());
							  
			} else if (name == L"location") {			
				loc += attr.GetPointValue();
	
			} else if (name == L"mac_delta_loc") {			
#if MAC
				loc += attr.GetPointValue();
#endif
			} else if (name == L"win_delta_loc") {			
#if WIN
				loc += attr.GetPointValue();
#endif
	
			} else if (name == L"size") {			
				size += attr.GetSizeValue();
							  					
			} else if (name == L"mac_delta_size") {			
#if MAC
				size += attr.GetSizeValue();
#endif
			} else if (name == L"win_delta_size") {			
#if WIN
				size += attr.GetSizeValue();
#endif
	
			} else if (name == L"bind_left") {			
				mBinding.left = attr.GetBoolValue();
							  					
			} else if (name == L"bind_right") {			
				mBinding.right = attr.GetBoolValue();

			} else if (name == L"bind_top") {			
				mBinding.top = attr.GetBoolValue();

			} else if (name == L"bind_bottom") {			
				mBinding.bottom = attr.GetBoolValue();
							  					
			} else if (name == L"visible") {			
				if (attr.GetBoolValue())
					this->HandleShow();
				else
					this->HandleHide();
							  					
			} else if (name == L"enabled") {			
				this->HandleEnable(attr.GetBoolValue(), kDontRedraw);
							  					
			} else if (name == L"help_mesg") {			
				mHelpMesg = LoadAppString(attr.GetValue());
							  					
			} else if (name == L"disabled_help") {			
				mDisabledHelpMesg = LoadAppString(attr.GetValue());
			
			} else if (name == L"draw_outset") {			
				mDrawOutset = attr.GetInt32Value();
							  					
			} else if (name == L"help_id") {			
				mHelpID = attr.GetUInt32Value();
							  					
			} else if (name == L"extensions") {			
				if (!attr.IsImplied()) {	
					std::wstring temp = attr.GetValue();
					std::wstring id = Whisper::Parse(temp, L" \t");
					while (id.length() > 0) {
						XExtension* extension = XExtension::Create(doc, id);
						this->AddExtension(extension);
						
						id = Whisper::Parse(temp, L" \t");
					}
				}
											  					
			} else
				DEBUGSTR(name, " isn't a valid attribute of XControlInfo!");
		}	
		
		this->SetLocation(loc);
		this->SetSize(size);
		this->Invalidate();	
	}
}


}	// namespace Whisper
