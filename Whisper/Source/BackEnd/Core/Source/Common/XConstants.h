/*
 *  File:       XConstants.h
 *  Summary:   	Various constants.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XConstants.h,v $
 *		Revision 1.7  2001/08/19 22:54:18  jesjones
 *		Added kEnabled and kNotEnabled.
 *		
 *		Revision 1.6  2001/05/06 10:41:41  jesjones
 *		Added kBulletChar.
 *		
 *		Revision 1.5  2001/03/30 04:13:30  jesjones
 *		Added key code constants for the number keys.
 *		
 *		Revision 1.4  2001/02/12 09:48:13  jesjones
 *		Added #pragma export's.
 *		
 *		Revision 1.3  2000/12/10 08:50:55  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:28:46  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <6>	 9/03/00	JDJ		Added kPrefsCmd. Renamed kSaveCopyAsCmd kSaveCopyCmd.
 *		 <5>	 3/14/00	JDJ		Added some Unicode character constants.
 *		 <4>	 6/09/99	JDJ		Fixed spelling of kPathSeparator.
 *		 <3>	 4/02/99	JDJ		Removed kNothingMessage, kOKMessage, and kCancelMessage.
 *		 <2>	12/08/98	JDJ		Replaced kIsBigEndian with BIG_ENDIAN.
 *		 <1>	11/02/97	JDJ		Created (from Raven)
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	Misc Constants
// ===================================================================================
const bool kRedraw     = true;
const bool kDontRedraw = false;

const bool kHandled    = true;
const bool kNotHandled = false;

const bool kEnabled    = true;
const bool kNotEnabled = false;

const bool kPurgeable    = true;
const bool kNonPurgeable = false;

const bool kRecursive    = true;
const bool kNonRecursive = false;

const bool kPersistant    = true;
const bool kNonPersistant = false;

const bool kRaw    = true;
const bool kCooked = false;

const bool kMoveHigh     = true;				//!< only used on the Mac
const bool kDontMoveHigh = false;				//!< only used on the Mac

const uint32 kDontZeroBytes = 0x0000;
const uint32 kZeroBytes	    = 0x0001;
const uint32 kUseAppHeap	= 0x0000;
const uint32 kUseTempHeap	= 0x0002;

const bool kUseCase     = true;
const bool kDontUseCase = false;

#if MAC
	const short kNoFileRefNum = -32767;
#endif

#if WIN
	const OSError noErr = NO_ERROR;
#endif

const uint16 kPlainStyle 		= 0x0000;		// text styles (values are as on the Mac)
const uint16 kBoldStyle 		= 0x0001;
const uint16 kItalicStyle 		= 0x0002;
const uint16 kUnderlineStyle	= 0x0004;

enum ETextAlignment {kLeftBottomAlign,			//!< text goes in left bottom corner of rect
					 kLeftTopAlign,				//!< text goes in left top corner of rect
				
					 kCenterBottomAlign,		//!< text goes in bottom center of rect
					 kCenterTopAlign,			//!< text goes in top center of rect
				
					 kRightBottomAlign,			//!< text goes in right bottom corner of rect
					 kRightTopAlign,			//!< text goes in right top corner of rect
				
					 kDefaultAlign				//!< kLeftTopAlign or kRightTopAlign (depending on the current language)
					 };


// ===================================================================================
//	Special Characters
// ===================================================================================
#if MAC
	const wchar_t kCommandChar 	= 0x11;		//!< the cloverleaf symbol
	
	const wchar_t kNoMarkChar 	= noMark;
	const wchar_t kCheckMarkChar	= 0x12;
	const wchar_t kDiamondChar 	= 0x13;
	const wchar_t kAppleLogoChar	= 0x14;

	const wchar_t kPathSeparator = ':';
	
#elif WIN
	const wchar_t kCheckMarkChar = 0x12;

	const wchar_t kPathSeparator = '\\';
#endif


// ===================================================================================
//	Unicode Characters
// ===================================================================================
const wchar_t kEnterChar 		 	= 0x0003;		//!< END OF TEXT (note that Windows doesn't distinguish between the Enter and Return keys)
const wchar_t kBackSpaceChar		= 0x0008;		//!< BACKSPACE
const wchar_t kTabChar				= 0x0009;		//!< HORIZONTAL TABULATION
const wchar_t kLineFeedChar			= 0x000A;		//!< LINE FEED
const wchar_t kVerticalTabChar		= 0x000B;		//!< VERTICAL TABULATION
const wchar_t kFormFeedChar			= 0x000C;		//!< FORM FEED
const wchar_t kReturnChar		 	= 0x000D;		//!< CARRIAGE RETURN
const wchar_t kEscapeChar		 	= 0x001B;		//!< ESCAPE
const wchar_t kSpaceChar		 	= 0x0020;		//!< SPACE

const wchar_t kNoBreakSpaceChar 	= 0x00A0;		//!< NO-BREAK SPACE
const wchar_t kCentSignChar	 		= 0x00A2;		//!< CENT SIGN
const wchar_t kPoundSignChar	 	= 0x00A3;		//!< POUND SIGN
const wchar_t kYenSignChar	 		= 0x00A5;		//!< YEN SIGN
const wchar_t kCopyrightChar	 	= 0x00A9;		//!< COPYRIGHT SIGN
const wchar_t kSoftHyphenChar	 	= 0x00AD;		//!< SOFT HYPHEN
const wchar_t kRegisteredSignChar 	= 0x00AE;		//!< REGISTERED SIGN
const wchar_t kDegreeSignChar	 	= 0x00B0;		//!< DEGREE SIGN
const wchar_t kPlusMinusSignChar	= 0x00B1;		//!< PLUS-MINUS SIGN

const wchar_t kSixEmSpaceChar		= 0x2006;		//!< SIX-PER-EM SPACE
const wchar_t kZeroWidthSpaceChar	= 0x200B;		//!< ZERO WIDTH SPACE
const wchar_t kBulletChar 			= 0x2022;		//!< BULLET
const wchar_t kLeftQuoteChar 	 	= 0x201C;		//!< LEFT DOUBLE QUOTATION MARK
const wchar_t kRightQuoteChar 		= 0x201D;		//!< RIGHT DOUBLE QUOTATION MARK
const wchar_t kEllipsisChar 		= 0x2026;		//!< HORIZONTAL ELLIPSIS

const wchar_t kEuroSignChar 		= 0x20A0;		//!< EURO-CURRENCY SIGN
const wchar_t kTrademarkSignChar  	= 0x2122;		//!< TRADE MARK SIGN
const wchar_t kNotEqualChar  		= 0x2260;		//!< NOT EQUAL TO
const wchar_t kInfinityChar 		= 0x221E;		//!< INFINITY
const wchar_t kLessOrEqualChar 		= 0x2264;		//!< LESS-THAN OR EQUAL TO
const wchar_t kGreaterOrEqualChar 	= 0x2265;		//!< GREATER-THAN OR EQUAL TO
const wchar_t kIdeographicSpcChar 	= 0x3000;		//!< IDEOGRAPHIC SPACE

const wchar_t kReplacementChar 		= 0xFFFD;		//!< REPLACEMENT CHARACTER


// ===================================================================================
//	Virtual Key Codes
// ===================================================================================
#if MAC
	// Navigation Keys
	const int16 kLeftArrowKey		= 0x7B;
	const int16 kUpArrowKey			= 0x7E;
	const int16 kRightArrowKey		= 0x7C;
	const int16 kDownArrowKey		= 0x7D;

	const int16 kPageUpKey			= 0x74;
	const int16 kPageDownKey		= 0x79;
	const int16 kEndKey				= 0x77;
	const int16 kHomeKey			= 0x73;

	// Deletion Keys
	const int16 kBackspaceKey		= 0x33;
	const int16 kFwdDeleteKey		= 0x75;
	const int16 kClearKey			= 0x47;		

	// Special Keys
	const int16 kReturnKey			= 0x24;
	const int16 kEnterKey			= 0x4C;		//!< Mac only
	const int16 kTabKey				= 0x30;
	const int16 kEscapeKey			= 0x35;

	const int16 kSpaceKey			= 0x31;
	const int16 kHelpKey			= 0x72;

	// Modifier Keys
	const int16 kShiftKey			= 0x38;
	const int16 kControlKey			= 0x3B;
	const int16 kOptionAltKey		= 0x3A;		//!< option on Mac, alt on Windows
	const int16 kCapsLockKey		= 0x39;

	const int16 kLeftShiftKey		= 0x38;		
	const int16 kRightShiftKey		= 0x3C;
	const int16 kLeftControlKey		= 0x3B;
	const int16 kRightControlKey	= 0x3E;
	const int16 kLeftOptionAltKey	= 0x3A;
	const int16 kRightOptionAltKey	= 0x3D;

	// Number Keys
	const int16 k0Key				= 0x1D;
	const int16 k1Key				= 0x12;
	const int16 k2Key				= 0x13;
	const int16 k3Key				= 0x14;
	const int16 k4Key				= 0x15;
	const int16 k5Key				= 0x17;
	const int16 k6Key				= 0x16;
	const int16 k7Key				= 0x1A;
	const int16 k8Key				= 0x1C;
	const int16 k9Key				= 0x19;

	// Numeric Keypad
	const int16 kNumPad0Key			= 0x52;		
	const int16 kNumPad1Key			= 0x53;
	const int16 kNumPad2Key			= 0x54;
	const int16 kNumPad3Key			= 0x55;
	const int16 kNumPad4Key			= 0x56;
	const int16 kNumPad5Key			= 0x57;
	const int16 kNumPad6Key			= 0x58;
	const int16 kNumPad7Key			= 0x59;
	const int16 kNumPad8Key			= 0x5B;
	const int16 kNumPad9Key			= 0x5C;

	const int16 kNumPadMultKey		= 0x43;
	const int16 kNumPadAddKey		= 0x45;
	const int16 kNumPadSubKey		= 0x4E;
	const int16 kNumPadDecimalKey	= 0x41;
	const int16 kNumPadDivideKey	= 0x4B;

	// Function Keys
	const int16 kF1Key				= 0x7A;
	const int16 kF2Key				= 0x78;
	const int16 kF3Key				= 0x63;
	const int16 kF4Key				= 0x76;
	const int16 kF5Key				= 0x60;
	const int16 kF6Key				= 0x61;
	const int16 kF7Key				= 0x62;
	const int16 kF8Key				= 0x64;
	const int16 kF9Key				= 0x65;
	const int16 kF10Key				= 0x6D;
	const int16 kF11Key				= 0x67;			
	const int16 kF12Key				= 0x6F;
	const int16 kF13Key				= 0x69;
	const int16 kF14Key				= 0x6B;
	const int16 kF15Key				= 0x71;

#elif WIN
	// Navigation Keys
	const int16 kLeftArrowKey		= VK_LEFT;
	const int16 kUpArrowKey			= VK_UP;
	const int16 kRightArrowKey		= VK_RIGHT;
	const int16 kDownArrowKey		= VK_DOWN;

	const int16 kPageUpKey			= VK_PRIOR;
	const int16 kPageDownKey		= VK_NEXT;
	const int16 kEndKey				= VK_END;
	const int16 kHomeKey			= VK_HOME;

	// Deletion Keys
	const int16 kBackspaceKey		= VK_BACK;
	const int16 kFwdDeleteKey		= VK_DELETE;
	const int16 kClearKey			= VK_CLEAR;			//!< numeric keypad with num lock off

	// Special Keys
	const int16 kReturnKey			= VK_RETURN;
	const int16 kTabKey				= VK_TAB;
	const int16 kEscapeKey			= VK_ESCAPE;
	const int16 kSpaceKey			= VK_SPACE;
	const int16 kHelpKey			= VK_HELP;

	// Modifier Keys
	const int16 kShiftKey			= VK_SHIFT;
	const int16 kControlKey			= VK_CONTROL;
	const int16 kOptionAltKey		= VK_MENU;			//!< option on Mac, alt on Windows
	const int16 kCapsLockKey		= VK_CAPITAL;

	const int16 kLeftShiftKey		= VK_LSHIFT;		//!< These should only be used with GetAsyncKeyState() and GetKeyState().
	const int16 kRightShiftKey		= VK_RSHIFT;		//!< Note that Win 95 doesn't distinguish between the left and right keys...
	const int16 kLeftControlKey		= VK_LCONTROL;
	const int16 kRightControlKey	= VK_RCONTROL;
	const int16 kLeftOptionAltKey	= VK_LMENU;
	const int16 kRightOptionAltKey	= VK_RMENU;

	// Number Keys
	const int16 k0Key				= '0';
	const int16 k1Key				= '1';
	const int16 k2Key				= '2';
	const int16 k3Key				= '3';
	const int16 k4Key				= '4';
	const int16 k5Key				= '5';
	const int16 k6Key				= '6';
	const int16 k7Key				= '7';
	const int16 k8Key				= '8';
	const int16 k9Key				= '9';

	// Numeric Keypad
	const int16 kNumPad0Key			= VK_NUMPAD0;		//!< numeric keypad with num lock on
	const int16 kNumPad1Key			= VK_NUMPAD1;
	const int16 kNumPad2Key			= VK_NUMPAD2;
	const int16 kNumPad3Key			= VK_NUMPAD3;
	const int16 kNumPad4Key			= VK_NUMPAD4;
	const int16 kNumPad5Key			= VK_NUMPAD5;
	const int16 kNumPad6Key			= VK_NUMPAD6;
	const int16 kNumPad7Key			= VK_NUMPAD7;
	const int16 kNumPad8Key			= VK_NUMPAD8;
	const int16 kNumPad9Key			= VK_NUMPAD9;

	const int16 kNumPadMultKey		= VK_MULTIPLY;
	const int16 kNumPadAddKey		= VK_MULTIPLY;
	const int16 kNumPadSubKey		= VK_SUBTRACT;
	const int16 kNumPadDecimalKey	= VK_DECIMAL;
	const int16 kNumPadDivideKey	= VK_DIVIDE;

	// Function Keys
	const int16 kF1Key				= VK_F1;
	const int16 kF2Key				= VK_F2;
	const int16 kF3Key				= VK_F3;
	const int16 kF4Key				= VK_F4;
	const int16 kF5Key				= VK_F5;
	const int16 kF6Key				= VK_F6;
	const int16 kF7Key				= VK_F7;
	const int16 kF8Key				= VK_F8;
	const int16 kF9Key				= VK_F9;
	const int16 kF10Key				= VK_F10;
	const int16 kF11Key				= VK_F11;			//!< enhanced keyboard
	const int16 kF12Key				= VK_F12;
	const int16 kF13Key				= VK_F13;
	const int16 kF14Key				= VK_F14;
	const int16 kF15Key				= VK_F15;
#endif

									
// ===================================================================================
//	Menu Commands
// 		These are sent up the input target chain when a menu item is selected. Lower
// 		case commands are reserved for Whisper.
// ===================================================================================
extern CORE_EXPORT const XMenuCommand kNothingCmd;
extern CORE_EXPORT const XMenuCommand kAbortCmd;			//!< command-period
extern CORE_EXPORT const XMenuCommand kHelpCmd;

extern CORE_EXPORT const XMenuCommand kAboutCmd;			// About Menu
#if MAC
	extern CORE_EXPORT const XMenuCommand kDACmd;				
#endif

extern CORE_EXPORT const XMenuCommand kNewCmd;				// File Menu
extern CORE_EXPORT const XMenuCommand kOpenCmd;
extern CORE_EXPORT const XMenuCommand kCloseCmd;
extern CORE_EXPORT const XMenuCommand kSaveCmd;
extern CORE_EXPORT const XMenuCommand kSaveAsCmd;
extern CORE_EXPORT const XMenuCommand kRevertCmd;
extern CORE_EXPORT const XMenuCommand kPageSetupCmd;
extern CORE_EXPORT const XMenuCommand kPrintCmd;
extern CORE_EXPORT const XMenuCommand kPrintOneCmd;
extern CORE_EXPORT const XMenuCommand kQuitCmd;
extern CORE_EXPORT const XMenuCommand kSaveCopyCmd;

extern CORE_EXPORT const XMenuCommand kUndoCmd; 			// Edit Menu
extern CORE_EXPORT const XMenuCommand kRedoCmd; 	
extern CORE_EXPORT const XMenuCommand kCutCmd;
extern CORE_EXPORT const XMenuCommand kCopyCmd;
extern CORE_EXPORT const XMenuCommand kPasteCmd;
extern CORE_EXPORT const XMenuCommand kClearCmd;
extern CORE_EXPORT const XMenuCommand kSelectAllCmd;
extern CORE_EXPORT const XMenuCommand kPrefsCmd;
extern CORE_EXPORT const XMenuCommand kShowClipboardCmd;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
