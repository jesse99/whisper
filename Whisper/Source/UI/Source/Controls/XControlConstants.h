/*
 *  File:       XControlConstants.h
 *  Summary:   	Control related constants to ease writing cross-platform XML.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XControlConstants.h,v $
 *		Revision 1.2  2000/11/09 12:29:05  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <2>	 7/31/00	JDJ		Includes ControlDefinitions.h when using the 3.3 Universal Headers.
 *		 <1>	 7/01/00	JDJ		Created
 */

#pragma once

#include <XTypes.h>

#if MAC && (TARGET_CARBON || UNIVERSAL_INTERFACES_VERSION >= 0x0330)
	#include <ControlDefinitions.h>
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Constants
//
#if WIN
	enum {
																	/* Bevel Button Proc IDs */
		kControlBevelButtonSmallBevelProc = 32,
		kControlBevelButtonNormalBevelProc = 33,
		kControlBevelButtonLargeBevelProc = 34
	};
	
	enum {
		kControlBevelButtonMenuOnRight = (1 << 2)
	};
	
	enum {
																	/* Slider proc IDs */
		kControlSliderProc			= 48,
		kControlSliderLiveFeedback	= (1 << 0),
		kControlSliderHasTickMarks	= (1 << 1),
		kControlSliderReverseDirection = (1 << 2),
		kControlSliderNonDirectional = (1 << 3)
	};
	
	enum {
																	/* Triangle proc IDs */
		kControlTriangleProc		= 64,
		kControlTriangleLeftFacingProc = 65,
		kControlTriangleAutoToggleProc = 66,
		kControlTriangleLeftFacingAutoToggleProc = 67
	};

	enum {
																	/* Progress Bar proc IDs */
		kControlProgressBarProc		= 80
	};
	
	enum {
																	/* Little Arrows proc IDs */
		kControlLittleArrowsProc	= 96
	};
	
	enum {
																	/* Chasing Arrows proc IDs */
		kControlChasingArrowsProc	= 112
	};
	
	enum {
																	/* Tabs proc IDs */
		kControlTabLargeProc		= 128,							/* Large tab size, north facing	*/
		kControlTabSmallProc		= 129,							/* Small tab size, north facing	*/
		kControlTabLargeNorthProc	= 128,							/* Large tab size, north facing	*/
		kControlTabSmallNorthProc	= 129,							/* Small tab size, north facing	*/
		kControlTabLargeSouthProc	= 130,							/* Large tab size, south facing	*/
		kControlTabSmallSouthProc	= 131,							/* Small tab size, south facing	*/
		kControlTabLargeEastProc	= 132,							/* Large tab size, east facing	*/
		kControlTabSmallEastProc	= 133,							/* Small tab size, east facing	*/
		kControlTabLargeWestProc	= 134,							/* Large tab size, west facing	*/
		kControlTabSmallWestProc	= 135							/* Small tab size, west facing	*/
	};
	
	enum {
																	/* Visual separator proc IDs */
		kControlSeparatorLineProc	= 144
	};
	
	enum {
																	/* Group Box proc IDs */
		kControlGroupBoxTextTitleProc = 160,
		kControlGroupBoxCheckBoxProc = 161,
		kControlGroupBoxPopupButtonProc = 162,
		kControlGroupBoxSecondaryTextTitleProc = 164,
		kControlGroupBoxSecondaryCheckBoxProc = 165,
		kControlGroupBoxSecondaryPopupButtonProc = 166
	};
	
	enum {
																	/* Image Well proc IDs */
		kControlImageWellProc		= 176
	};
	
	enum {
																	/* Popup Arrow proc IDs */
		kControlPopupArrowEastProc	= 192,
		kControlPopupArrowWestProc	= 193,
		kControlPopupArrowNorthProc	= 194,
		kControlPopupArrowSouthProc	= 195,
		kControlPopupArrowSmallEastProc = 196,
		kControlPopupArrowSmallWestProc = 197,
		kControlPopupArrowSmallNorthProc = 198,
		kControlPopupArrowSmallSouthProc = 199
	};
	
	enum {
																	/* Placard proc IDs */
		kControlPlacardProc			= 224
	};
	
	enum {
																	/* Clock proc IDs */
		kControlClockTimeProc		= 240,
		kControlClockTimeSecondsProc = 241,
		kControlClockDateProc		= 242,
		kControlClockMonthYearProc	= 243
	};
	
	enum {
																	/* User Pane proc IDs */
		kControlUserPaneProc		= 256
	};
	
	enum {
																	/* Edit Text proc IDs */
		kControlEditTextProc		= 272,
		kControlEditTextPasswordProc = 274
	};
	
	enum {
		kControlEditTextInlineInputProc = 276						/* Can't combine with the other variants*/
	};
	
	
	enum {
		kControlStaticTextProc		= 288
	};
	
	/* Tagged data supported by static text */
	enum {
																	/* Picture control proc IDs */
		kControlPictureProc			= 304,
		kControlPictureNoTrackProc	= 305							/* immediately returns kControlPicturePart*/
	};
	
	enum {
		kControlIconProc			= 320,
		kControlIconNoTrackProc		= 321,							/* immediately returns kControlIconPart*/
		kControlIconSuiteProc		= 322,
		kControlIconSuiteNoTrackProc = 323							/* immediately returns kControlIconPart*/
	};
	
	enum {
																	/* icon ref controls may have either an icon, color icon, icon suite, or icon ref.*/
																	/* for data other than icon, you must set the data by passing a*/
																	/* ControlButtonContentInfo to SetControlData*/
		kControlIconRefProc			= 324,
		kControlIconRefNoTrackProc	= 325							/* immediately returns kControlIconPart*/
	};
	
	enum {
																	/* Window Header proc IDs */
		kControlWindowHeaderProc	= 336,							/* normal header*/
		kControlWindowListViewHeaderProc = 337						/* variant for list views - no bottom line*/
	};
	
	enum {
																	/* List Box proc IDs */
		kControlListBoxProc			= 352,
		kControlListBoxAutoSizeProc	= 353
	};
	
	enum {
																	/* Theme Push Button/Check Box/Radio Button proc IDs */
		kControlPushButtonProc		= 368,
		kControlCheckBoxProc		= 369,
		kControlRadioButtonProc		= 370,
		kControlPushButLeftIconProc	= 374,							/* Standard pushbutton with left-side icon*/
		kControlPushButRightIconProc = 375							/* Standard pushbutton with right-side icon*/
	};
		
	enum {
		kControlCheckBoxAutoToggleProc = 371,
		kControlRadioButtonAutoToggleProc = 372
	};
	
	enum {
																	/* Theme Scroll Bar proc IDs */
		kControlScrollBarProc		= 384,							/* normal scroll bar*/
		kControlScrollBarLiveProc	= 386							/* live scrolling variant*/
	};
	
	enum {
																	/* Theme Popup Button proc IDs */
		kControlPopupButtonProc		= 400,
		kControlPopupFixedWidthVariant = 1 << 0,
		kControlPopupVariableWidthVariant = 1 << 1,
		kControlPopupUseAddResMenuVariant = 1 << 2,
		kControlPopupUseWFontVariant = 1 << 3						/* kControlUsesOwningWindowsFontVariant*/
	};
	
	enum {
		kControlRadioGroupProc		= 416
	};
	
	enum {
		kControlScrollTextBoxProc	= 432,
		kControlScrollTextBoxAutoScrollProc = 433
	};

#elif MAC
	/*
	 * Edit Control Styles
	 */
	#define ES_LEFT             0x0000L
	#define ES_CENTER           0x0001L
	#define ES_RIGHT            0x0002L
	#define ES_MULTILINE        0x0004L
	#define ES_UPPERCASE        0x0008L
	#define ES_LOWERCASE        0x0010L
	#define ES_PASSWORD         0x0020L
	#define ES_AUTOVSCROLL      0x0040L
	#define ES_AUTOHSCROLL      0x0080L
	#define ES_NOHIDESEL        0x0100L
	#define ES_OEMCONVERT       0x0400L
	#define ES_READONLY         0x0800L
	#define ES_WANTRETURN       0x1000L
	#define ES_NUMBER           0x2000L
	
	/*
	 * Button Control Styles
	 */
	#define BS_PUSHBUTTON       0x00000000L
	#define BS_DEFPUSHBUTTON    0x00000001L
	#define BS_CHECKBOX         0x00000002L
	#define BS_AUTOCHECKBOX     0x00000003L
	#define BS_RADIOBUTTON      0x00000004L
	#define BS_3STATE           0x00000005L
	#define BS_AUTO3STATE       0x00000006L
	#define BS_GROUPBOX         0x00000007L
	#define BS_USERBUTTON       0x00000008L
	#define BS_AUTORADIOBUTTON  0x00000009L
	#define BS_OWNERDRAW        0x0000000BL
	#define BS_LEFTTEXT         0x00000020L
	#define BS_TEXT             0x00000000L
	#define BS_ICON             0x00000040L
	#define BS_BITMAP           0x00000080L
	#define BS_LEFT             0x00000100L
	#define BS_RIGHT            0x00000200L
	#define BS_CENTER           0x00000300L
	#define BS_TOP              0x00000400L
	#define BS_BOTTOM           0x00000800L
	#define BS_VCENTER          0x00000C00L
	#define BS_PUSHLIKE         0x00001000L
	#define BS_MULTILINE        0x00002000L
	#define BS_NOTIFY           0x00004000L
	#define BS_FLAT             0x00008000L
	#define BS_RIGHTBUTTON      BS_LEFTTEXT
	
	/*
	 * Static Control Constants
	 */
	#define SS_LEFT             0x00000000L
	#define SS_CENTER           0x00000001L
	#define SS_RIGHT            0x00000002L
	#define SS_ICON             0x00000003L
	#define SS_BLACKRECT        0x00000004L
	#define SS_GRAYRECT         0x00000005L
	#define SS_WHITERECT        0x00000006L
	#define SS_BLACKFRAME       0x00000007L
	#define SS_GRAYFRAME        0x00000008L
	#define SS_WHITEFRAME       0x00000009L
	#define SS_USERITEM         0x0000000AL
	#define SS_SIMPLE           0x0000000BL
	#define SS_LEFTNOWORDWRAP   0x0000000CL
	#define SS_OWNERDRAW        0x0000000DL
	#define SS_BITMAP           0x0000000EL
	#define SS_ENHMETAFILE      0x0000000FL
	#define SS_ETCHEDHORZ       0x00000010L
	#define SS_ETCHEDVERT       0x00000011L
	#define SS_ETCHEDFRAME      0x00000012L
	#define SS_TYPEMASK         0x0000001FL
	#define SS_NOPREFIX         0x00000080L /* Don't do "&" character translation */
	#define SS_NOTIFY           0x00000100L
	#define SS_CENTERIMAGE      0x00000200L
	#define SS_RIGHTJUST        0x00000400L
	#define SS_REALSIZEIMAGE    0x00000800L
	#define SS_SUNKEN           0x00001000L
	#define SS_ENDELLIPSIS      0x00004000L
	#define SS_PATHELLIPSIS     0x00008000L
	#define SS_WORDELLIPSIS     0x0000C000L
	#define SS_ELLIPSISMASK     0x0000C000L

	/*
	 * Trackbar Control Constants
	 */
	#define TBS_AUTOTICKS           0x0001
	#define TBS_VERT                0x0002
	#define TBS_HORZ                0x0000
	#define TBS_TOP                 0x0004
	#define TBS_BOTTOM              0x0000
	#define TBS_LEFT                0x0004
	#define TBS_RIGHT               0x0000
	#define TBS_BOTH                0x0008
	#define TBS_NOTICKS             0x0010
	#define TBS_ENABLESELRANGE      0x0020
	#define TBS_FIXEDLENGTH         0x0040
	#define TBS_NOTHUMB             0x0080
	#define TBS_TOOLTIPS            0x0100

	/*
	 * Listbox Styles
	 */
	#define LBS_NOTIFY            0x0001L
	#define LBS_SORT              0x0002L
	#define LBS_NOREDRAW          0x0004L
	#define LBS_MULTIPLESEL       0x0008L
	#define LBS_OWNERDRAWFIXED    0x0010L
	#define LBS_OWNERDRAWVARIABLE 0x0020L
	#define LBS_HASSTRINGS        0x0040L
	#define LBS_USETABSTOPS       0x0080L
	#define LBS_NOINTEGRALHEIGHT  0x0100L
	#define LBS_MULTICOLUMN       0x0200L
	#define LBS_WANTKEYBOARDINPUT 0x0400L
	#define LBS_EXTENDEDSEL       0x0800L
	#define LBS_DISABLENOSCROLL   0x1000L
	#define LBS_NODATA            0x2000L
	#define LBS_NOSEL             0x4000L
	#define LBS_STANDARD          (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)
	
	/*
	 * Combo Box styles
	 */
	#define CBS_SIMPLE            0x0001L
	#define CBS_DROPDOWN          0x0002L
	#define CBS_DROPDOWNLIST      0x0003L
	#define CBS_OWNERDRAWFIXED    0x0010L
	#define CBS_OWNERDRAWVARIABLE 0x0020L
	#define CBS_AUTOHSCROLL       0x0040L
	#define CBS_OEMCONVERT        0x0080L
	#define CBS_SORT              0x0100L
	#define CBS_HASSTRINGS        0x0200L
	#define CBS_NOINTEGRALHEIGHT  0x0400L
	#define CBS_DISABLENOSCROLL   0x0800L
	#define CBS_UPPERCASE         0x2000L
	#define CBS_LOWERCASE         0x4000L
	
	/*
	 * Scroll Bar Styles
	 */
	#define SBS_HORZ                    0x0000L
	#define SBS_VERT                    0x0001L
	#define SBS_TOPALIGN                0x0002L
	#define SBS_LEFTALIGN               0x0002L
	#define SBS_BOTTOMALIGN             0x0004L
	#define SBS_RIGHTALIGN              0x0004L
	#define SBS_SIZEBOXTOPLEFTALIGN     0x0002L
	#define SBS_SIZEBOXBOTTOMRIGHTALIGN 0x0004L
	#define SBS_SIZEBOX                 0x0008L
	#define SBS_SIZEGRIP                0x0010L
#endif	


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
