/*
 *  File:       MQuickDrawUtils.cpp
 *  Summary:   	QuickDraw utilities
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MQuickDrawUtils.cpp,v $
 *		Revision 1.7  2001/04/27 04:22:08  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.6  2001/04/21 03:28:01  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/13 07:53:09  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.4  2001/02/28 09:57:19  jesjones
 *		Added a flag to PrepCopyBits to prevent changing the background color (useful for transparent draw mode).
 *		
 *		Revision 1.3  2000/12/10 04:05:03  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:18:40  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 2/06/00	JDJ		Nuked SetIfColor and SetIfBkColor. Renamed GetGray GetGrayColor.
 *		 <4>	 7/19/99	JDJ		Tweaked for Carbon.
 *		 <3>	 6/11/99	JDJ		SetIfColor and SetIfBkColor no longer check for color ports (RGBForeColor
 *									will do the right thing if the port is b/w) and no longer check the port's
 *									current color (this code was swiped from MacApp and didn't work because
 *									XRGBColor uses floats).
 *		 <2>	 6/10/99	JDJ		Added TARGET_CARBON MSetPort overloads for WindowRef and DialogRef.
 *		 <1>	11/30/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <MQuickDrawUtils.h>

#include <Fonts.h>
#include <Palettes.h>
#include <QDOffscreen.h>
#include <Script.h>
#include <TextEdit.h>
#include <TextUtils.h>

#include <MSystemInfo.h>
#include <XConstants.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XNumbers.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const int16 kFullJustification = 128;			


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// LineHeight
//
// Based on NeoTextBox by Bryan K. Ressler (except that startY no
// longer includes leading).
//
//---------------------------------------------------------------
static int32 LineHeight(const char* theText, uint32 textLen, const XRect& wrapBox, int16 lhCode, int32 *startY)
{
	int32 lineHeight;	

	FontInfo fontInfo;		
	::GetFontInfo(&fontInfo);
	
	if (lhCode < 0) {
		XPoint scaling(1, 1);		
		if (MSystemInfo::HasTrueType() && IsOutline(scaling.GetOSPoint(), scaling.GetOSPoint())) {
			int16 ascent, descent;
			
			OutlineMetrics(numeric_cast<int16>(textLen), theText, scaling.GetOSPoint(), scaling.GetOSPoint(), &ascent, &descent, nil, nil, nil);
			lineHeight = Max(fontInfo.ascent, ascent) + Max(fontInfo.descent, numeric_cast<int16>(-descent)) + fontInfo.leading;
			
			*startY = wrapBox.top + Max(fontInfo.ascent, ascent);
			
		} else {
			lineHeight = fontInfo.ascent + fontInfo.descent + fontInfo.leading;
			*startY = wrapBox.top + fontInfo.ascent;
		}
		
	} else if (lhCode == 0) {
		lineHeight = fontInfo.ascent + fontInfo.descent + fontInfo.leading;
		*startY = wrapBox.top + fontInfo.ascent;
		
	} else {
		lineHeight = lhCode;
		*startY = wrapBox.top + lhCode;
	}

	return lineHeight;
}


//---------------------------------------------------------------
//
// DoDrawText
//
// Based on NeoTextBox by Bryan K. Ressler, $$$ better to use MLTE
//
//---------------------------------------------------------------
static void DoDrawText(StyledLineBreakCode breakCode, const char* lineStart, int32 lineBytes, const XRect& wrapBox, int16 align, int16 curY, int16 boxWidth)
{
	int16 blackLen = numeric_cast<int16>(::VisibleLength((char *) lineStart, (int16) lineBytes));
	
	if (align == kFullJustification) {
		const Point kNoScaling = {1, 1};
		int16 slop = numeric_cast<int16>(boxWidth - TextWidth(lineStart, 0, blackLen));
		MoveTo(numeric_cast<int16>(wrapBox.left), curY);
		
		if (breakCode == smBreakOverflow || *(lineStart + (lineBytes - 1)) == kReturnCharCode)
			align = GetSysDirection();
		else
			DrawJustified((char *) lineStart, blackLen, slop, onlyStyleRun, kNoScaling, kNoScaling);
	}

	switch(align) {
		case teForceLeft:
		case teJustLeft:
			MoveTo(numeric_cast<int16>(wrapBox.left), curY);
			break;
			
		case teJustRight:
			MoveTo(numeric_cast<int16>(wrapBox.right - TextWidth(lineStart, 0, blackLen)), curY);
			break;
			
		case teJustCenter:
			MoveTo(numeric_cast<int16>(wrapBox.left + (boxWidth - ::TextWidth(lineStart, 0, blackLen))/2), curY);
			break;
	}
	
	if (align != kFullJustification)
		::DrawText(lineStart, 0, numeric_cast<int16>(lineBytes));
}


//---------------------------------------------------------------
//
// ColorToLong
//
//---------------------------------------------------------------
static int32 ColorToLong(const RGBColor& rgb)
{
	int32 pixel;

	pixel  = (rgb.red & 0xFF00L) << 8;
	pixel |= rgb.green & 0xFF00;
	pixel |= (rgb.blue >> 8) & 0xFF;
	
	return pixel;
}

#pragma mark -

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetWorkPort
//
//---------------------------------------------------------------
GrafPtr GetWorkPort()
{
	static CGrafPtr port = nil;
	
	if (port == nil) {
		Rect bounds = {-10000, -10000, -10000, -10000};
		if (MSystemInfo::HasColorQuickDraw())
			port = GetWindowPort(NewCWindow(nil, &bounds, "\pWork Port", false, documentProc, nil, false, 0));
		else
			port = GetWindowPort(NewWindow(nil, &bounds, "\pWork Port", false, documentProc, nil, false, 0));
	}
	
	return port;
}


//---------------------------------------------------------------
//
// IsColorPort
//
//---------------------------------------------------------------
bool IsColorPort(GrafPtr port)
{
	return IsPortColor(port);
} 


//---------------------------------------------------------------
//
// GetGrayColor
//
//---------------------------------------------------------------
RGBColor GetGrayColor(GDHandle device)
{
	if (device == nil) {
		device = GetGDevice();
		ASSERT(device != nil);
	}
		
	RGBColor back = {0, 0, 0};
	RGBColor gray = {65535, 65535, 65535};
	
	(void) ::GetGray(device, &back, &gray);
	
	return gray;
}


//---------------------------------------------------------------
//
// CompareColor
//
//---------------------------------------------------------------
int32 CompareColor(const RGBColor& color1, const RGBColor& color2)
{
	int32 result;
	
	int32 pixel1 = ColorToLong(color1);
	int32 pixel2 = ColorToLong(color2);
	
	if (pixel1 < pixel2)
		result = -1;
		
	else if (pixel1 > pixel2)
		result = 1;
		
	else
		result = 0;
		
	return result;
}


//---------------------------------------------------------------
//
// CompareColorTable
//
//---------------------------------------------------------------
int32 CompareColorTable(CTabHandle lhs, CTabHandle rhs)
{
	int32 result;
	
	if ((**lhs).ctSize > (**rhs).ctSize)		
		result = 1;
		
	else if ((**lhs).ctSize < (**rhs).ctSize)
		result = -1;
		
	else if ((**lhs).ctSeed == (**rhs).ctSeed)
		result = 0;
	
	else {
		result = 0;
		
		for (uint16 index = 0; index <= (**lhs).ctSize && result == 0; ++index) {
			RGBColor lhsColor = (**lhs).ctTable[index].rgb;
			RGBColor rhsColor = (**rhs).ctTable[index].rgb;
			
			result = CompareColor(lhsColor, rhsColor);
		}
	}
	
	return result;
}


//---------------------------------------------------------------
//
// PrepCopyBits
//
// Prepare the current port for bit copy operation.
//
//---------------------------------------------------------------
void PrepCopyBits(bool keepBackColor)
{
	RGBColor black = {0, 0, 0};
	RGBColor white = {65535, 65535, 65535};
		
#if TARGET_CARBON
	RGBForeColor(&black);
	if (!keepBackColor)
		RGBBackColor(&white);
#else
	if (IsColorPort(qd.thePort)) {
		RGBForeColor(&black);
		if (!keepBackColor)
			RGBBackColor(&white);
			
	} else {
		if (qd.thePort->fgColor != blackColor)										// Set old-style fore/background colors if necessary
			ForeColor(blackColor);
		if (qd.thePort->bkColor != whiteColor && !keepBackColor)
			BackColor(whiteColor);
	}
#endif
}

#pragma mark -

//---------------------------------------------------------------
//
// TruncateText (string, int16)
//
//---------------------------------------------------------------
std::wstring TruncateText(const std::wstring& inStr, int16 pixelWidth)
{
	PRECONDITION(pixelWidth >= 0);
	
	std::wstring outStr;

	uint32 len = inStr.length();
	
	if (len > 0) {
		MSetPort setter(GetWorkPort());
		
		outStr = inStr;
		
		if (::TextWidth(ToPlatformStr(inStr).c_str(), 0, numeric_cast<int16>(len)) > pixelWidth) {
			while (--len > 0 && ::TextWidth(ToPlatformStr(inStr).c_str(), 0, numeric_cast<int16>(len)) > pixelWidth)
				;

			outStr = std::wstring(inStr.c_str(), len);
		}
	}
	
	return outStr;
}


//---------------------------------------------------------------
//
// TruncateText (string, int16, TruncCode)
//
//---------------------------------------------------------------
std::wstring TruncateText(const std::wstring& inStr, int16 pixelWidth, TruncCode truncateAt)
{
	PRECONDITION(pixelWidth >= 0);

	std::wstring outStr;

	int16 len = numeric_cast<int16>(inStr.length());
	
	if (len > 0) {
		MSetPort setter(GetWorkPort());

		outStr = inStr;
		
		if (::TextWidth(ToPlatformStr(inStr).c_str(), 0, len) > pixelWidth) {
			char* str = new char[len+1];
			BlockMoveData(ToPlatformStr(inStr).c_str(), str, len+1UL);

			(void) ::TruncText(pixelWidth, str, &len, truncateAt);

			outStr = FromPlatformStr(str, len);
			
			delete [] str;
		}
	}

	return outStr;
}


//---------------------------------------------------------------
//
// TextBox
//
// Based on NeoTextBox by Bryan K. Ressler
//
//---------------------------------------------------------------
void TextBox(const std::wstring& text, const XRect& wrapBox, int16 align, int16 lhCode)
{
	MIntersectClip clipTo(wrapBox);
	
	PlatformString str = ToPlatformStr(text);

	int32 boxWidth = wrapBox.right - wrapBox.left;
	if (align == teFlushDefault)
		align = ::GetSysDirection();

	int32 curY;
	int32 lineHeight = LineHeight(str.c_str(), text.length(), wrapBox, lhCode, &curY);
	uint16 lineCount = 0;
	const char* lineStart = str.c_str();
	int32 textLeft = (int32) text.length();
	
	do {
		int32 lineBytes = 1;
		Fixed wrapWidth = boxWidth << 16;

		StyledLineBreakCode breakCode = ::StyledLineBreak((char *) lineStart, textLeft, 0, textLeft, 0,
									                      &wrapWidth, &lineBytes);
		
		DoDrawText(breakCode, lineStart, lineBytes, wrapBox, align, numeric_cast<int16>(curY), numeric_cast<int16>(boxWidth));
		
		curY += lineHeight;
		lineStart += lineBytes;
		textLeft -= lineBytes;
		lineCount++;
	} while (lineStart < str.c_str() + text.length() & curY <= (wrapBox.bottom + lineHeight));
}

#pragma mark -

// ===================================================================================
//	class MSavePort
// ===================================================================================

//---------------------------------------------------------------
//
//	MSavePort::~MSavePort
//
//---------------------------------------------------------------
MSavePort::~MSavePort()
{
	SetGWorld(mOldPort, mOldDevice);		
	
	if (mOldPort != nil) {
		::SetOrigin(numeric_cast<int16>(mOldOrigin.x), numeric_cast<int16>(mOldOrigin.y));
		::SetClip(mOldClip);
	}
	
	DisposeRgn(mOldClip);
}


//---------------------------------------------------------------
//
// MSavePort::MSavePort
//
//---------------------------------------------------------------
MSavePort::MSavePort()
{
	GetGWorld(&mOldPort, &mOldDevice);
	
	if (mOldPort != nil) {
#if TARGET_CARBON
		Rect bounds;
		::GetPortBounds(mOldPort, &bounds);
		mOldOrigin.x = bounds.left;
		mOldOrigin.y = bounds.top;
#else
		XRect portRect = mOldPort->portRect;
		mOldOrigin = portRect[topLeft];
#endif
		
		mOldClip = NewRgn();
		ThrowIfNil(mOldClip);
		
		::GetClip(mOldClip);
	}
}

#pragma mark -

// ===================================================================================
//	class MSetPort
// ===================================================================================

//---------------------------------------------------------------
//
// MSetPort::~MSetPort
//
//---------------------------------------------------------------
MSetPort::~MSetPort()
{
	// Restore port's origin and clip region.
	SetGWorld(mNewPort, mNewDevice);		
	
	SetOrigin(numeric_cast<int16>(mNewOrigin.x), numeric_cast<int16>(mNewOrigin.y));
	SetClip(mNewClip);

	// Set the current port to the original port.
	SetGWorld(mOldPort, mOldDevice);		

	DisposeRgn(mNewClip);
}


//---------------------------------------------------------------
//
// MSetPort::MSetPort (CGrafPtr, GDHandle)
//
//---------------------------------------------------------------
MSetPort::MSetPort(CGrafPtr port, GDHandle device)
{
	PRECONDITION(port != nil);
	PRECONDITION(device != nil);
	
	GetGWorld(&mOldPort, &mOldDevice);
	
	SetGWorld(port, device);
	GetGWorld(&mNewPort, &mNewDevice);
	
#if TARGET_CARBON
	Rect bounds;
	::GetPortBounds(mNewPort, &bounds);
	mNewOrigin.x = bounds.left;
	mNewOrigin.y = bounds.top;
#else
	XRect portRect = mNewPort->portRect;
	mNewOrigin = portRect[topLeft];
#endif

	mNewClip = NewRgn();
	ThrowIfNil(mNewClip);

	GetClip(mNewClip);
}


//---------------------------------------------------------------
//
// MSetPort::MSetPort (WindowRef)
//
//---------------------------------------------------------------
MSetPort:: MSetPort(WindowRef window)
{
	PRECONDITION(window != nil);

	GetGWorld(&mOldPort, &mOldDevice);
	
	SetPortWindowPort(window);
	GetGWorld(&mNewPort, &mNewDevice);
	
	Rect bounds;
	::GetPortBounds(mNewPort, &bounds);
	mNewOrigin.x = bounds.left;
	mNewOrigin.y = bounds.top;

	mNewClip = NewRgn();
	ThrowIfNil(mNewClip);

	GetClip(mNewClip);
}


//---------------------------------------------------------------
//
// MSetPort::MSetPort (DialogRef)
//
//---------------------------------------------------------------
MSetPort:: MSetPort(DialogRef dialog)
{
	PRECONDITION(dialog != nil);

	GetGWorld(&mOldPort, &mOldDevice);
	
	SetPortDialogPort(dialog);
	GetGWorld(&mNewPort, &mNewDevice);
	
	Rect bounds;
	::GetPortBounds(mNewPort, &bounds);
	mNewOrigin.x = bounds.left;
	mNewOrigin.y = bounds.top;

	mNewClip = NewRgn();
	ThrowIfNil(mNewClip);

	GetClip(mNewClip);
}

#pragma mark -

// ===================================================================================
//	class MSetClip
// ===================================================================================

//---------------------------------------------------------------
//
// MSetClip::~MSetClip
//
//---------------------------------------------------------------
MSetClip::~MSetClip()
{
	SetClip(mOldClip);

	DisposeRgn(mOldClip);
}

	
//---------------------------------------------------------------
//
// MSetClip::MSetClip (XRect)
//
//---------------------------------------------------------------
MSetClip::MSetClip(const XRect& rect)
{
	mOldClip = NewRgn();
	ThrowIfNil(mOldClip);
		
	GetClip(mOldClip);
	
	Rect temp = rect.GetOSRect();
	ClipRect(&temp);
}


//---------------------------------------------------------------
//
// MSetClip::MSetClip (RgnHandle)
//
//---------------------------------------------------------------
MSetClip::MSetClip(RgnHandle rgn)
{
	PRECONDITION(rgn != nil);
	
	mOldClip = NewRgn();
	ThrowIfNil(mOldClip);
		
	GetClip(mOldClip);
	
	SetClip(rgn);
}

#pragma mark -

// ===================================================================================
//	class MIntersectClip
// ===================================================================================

//---------------------------------------------------------------
//
// MIntersectClip::~MIntersectClip
//
//---------------------------------------------------------------
MIntersectClip::~MIntersectClip()
{
	SetClip(mOldClip);

	DisposeRgn(mOldClip);
}

	
//---------------------------------------------------------------
//
// MIntersectClip::MIntersectClip (XRect)
//
//---------------------------------------------------------------
MIntersectClip::MIntersectClip(const XRect& rect)
{
	mOldClip = NewRgn();
	ThrowIfNil(mOldClip);
		
	GetClip(mOldClip);
		
	RgnHandle newClip = NewRgn();
	ThrowIfNil(newClip);

	Rect temp = rect.GetOSRect();
	RectRgn(newClip, &temp);
	
	SectRgn(mOldClip, newClip, newClip);
	ThrowIfQDError();

	SetClip(newClip);
	DisposeRgn(newClip);
}


//---------------------------------------------------------------
//
// MIntersectClip::MIntersectClip (RgnHandle)
//
//---------------------------------------------------------------
MIntersectClip::MIntersectClip(RgnHandle rgn)
{
	PRECONDITION(rgn != nil);
	
	mOldClip = NewRgn();
	ThrowIfNil(mOldClip);
		
	GetClip(mOldClip);
	
	RgnHandle newClip = NewRgn();
	ThrowIfNil(newClip);

	SectRgn(mOldClip, rgn, newClip);
	ThrowIfQDError();
	
	SetClip(newClip);
	DisposeRgn(newClip);
}

#pragma mark -

// ===================================================================================
//	class MSaveColorState
// ===================================================================================

//---------------------------------------------------------------
//
// MSaveColorState::~MSaveColorState
//
//---------------------------------------------------------------
MSaveColorState::~MSaveColorState()
{
	this->Restore();
}


//---------------------------------------------------------------
//
// MSaveColorState::MSaveColorState
//
//---------------------------------------------------------------
MSaveColorState::MSaveColorState()
{
	this->Save();
}

			
//---------------------------------------------------------------
//
// MSaveColorState::Save
//
//---------------------------------------------------------------
void MSaveColorState::Save()
{
	::GetForeColor(&mForegroundColor);
	::GetBackColor(&mBackgroundColor);
}


//---------------------------------------------------------------
//
// MSaveColorState::Restore
//
//---------------------------------------------------------------
void MSaveColorState::Restore()
{
	::RGBForeColor(&mForegroundColor);
	::RGBBackColor(&mBackgroundColor);
}


//---------------------------------------------------------------
//
// MSaveColorState::Normalize							[static]
//
//---------------------------------------------------------------
void MSaveColorState::Normalize()
{
	RGBColor black = {0, 0, 0};	
	::RGBForeColor(&black);

	RGBColor white = {65535, 65535, 65535};	
	::RGBBackColor(&white);
}

#pragma mark -

// ===================================================================================
//	class MSaveColorPenState
// ===================================================================================

//---------------------------------------------------------------
//
// MSaveColorPenState::~MSaveColorPenState
//
//---------------------------------------------------------------
MSaveColorPenState::~MSaveColorPenState()
{
	::SetPenState(&mPenState);
}


//---------------------------------------------------------------
//
// MSaveColorPenState::MSaveColorPenState
//
//---------------------------------------------------------------
MSaveColorPenState::MSaveColorPenState()
{
	::GetPenState(&mPenState);
}

	
//---------------------------------------------------------------
//
// MSaveColorPenState::Save
//
//---------------------------------------------------------------
void MSaveColorPenState::Save()
{
	mColorState.Save();
	::GetPenState(&mPenState);
}


//---------------------------------------------------------------
//
// MSaveColorPenState::Restore
//
//---------------------------------------------------------------
void MSaveColorPenState::Restore()
{
	mColorState.Restore();
	::SetPenState(&mPenState);
}


//---------------------------------------------------------------
//
// MSaveColorPenState::Normalize						[static]
//
//---------------------------------------------------------------
void MSaveColorPenState::Normalize()
{
	MSaveColorState::Normalize();
	::PenNormal();
}

#pragma mark -

// ===================================================================================
//	class MSaveTextState
// ===================================================================================

//---------------------------------------------------------------
//
// MSaveTextState::~MSaveTextState
//
//---------------------------------------------------------------
MSaveTextState::~MSaveTextState()
{
	this->Restore();
}


//---------------------------------------------------------------
//
// MSaveTextState::MSaveTextState
//
//---------------------------------------------------------------
MSaveTextState::MSaveTextState()
{
	this->Save();
}

		
//---------------------------------------------------------------
//
// MSaveTextState::Save
//
//---------------------------------------------------------------
void MSaveTextState::Save()
{
#if TARGET_CARBON
	CGrafPtr port = ::GetQDGlobalsThePort();
	ASSERT(port != nil);
	
	mFontNumber = ::GetPortTextFont(port);
	mSize       = ::GetPortTextSize(port);
	mStyle      = ::GetPortTextFace(port);
	mMode       = ::GetPortTextMode(port);
#else
	ASSERT(qd.thePort != nil);

	mFontNumber = qd.thePort->txFont;
	mSize       = qd.thePort->txSize;
	mStyle      = qd.thePort->txFace;
	mMode       = qd.thePort->txMode;
#endif
}


//---------------------------------------------------------------
//
// MSaveTextState::Restore
//
//---------------------------------------------------------------
void MSaveTextState::Restore()
{
	::TextFont(mFontNumber);
	::TextSize(mSize);
	::TextFace(mStyle);
	::TextMode(mMode);
}


//---------------------------------------------------------------
//
// MSaveTextState::Normalize							[static]
//
//---------------------------------------------------------------
void MSaveTextState::Normalize()
{
	::TextFont(systemFont);
	::TextSize(0);
	::TextFace(0);
	::TextMode(srcOr);
}


}	// namespace Whisper

