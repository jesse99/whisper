/*
 *  File:       MQuickDrawUtils.h
 *  Summary:   	QuickDraw utilities
 *  Written by: Jesse Jones
 *
 *	Classes:	MSavePort			- Stack based class to save and restore the port.
 *				MSetPort			- Stack based class to set and restore the port.
 *				MSetClip			- Stack based class to set and restore the clip region.
 *				MIntersectClip		- Stack based class to intersect the clip region with a rect or region.
 *				MSaveColorState		- Stack based class to save and restore the current port's fg and bk colors.
 *				MSaveColorPenState	- Stack based class to save and restore the current port's colors and pen state.
 *				MSaveTextState		- Stack based class to save and restore the current port's text state.
 *
 *  Copyright � 1997-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MQuickDrawUtils.h,v $
 *		Revision 1.4  2001/02/28 09:57:56  jesjones
 *		Added a flag to PrepCopyBits to disable setting background color (useful with transparent draw mode).
 *		
 *		Revision 1.3  2000/12/10 08:49:06  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:18:46  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <QuickDraw.h>
#include <TextUtils.h>

#include <XRect.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	Misc Functions
// ===================================================================================
GrafPtr		GetWorkPort();

bool 		IsColorPort(GrafPtr port = nil);
			// If port isn't specified the current port will be used.

RGBColor 	GetGrayColor(GDHandle device = nil);
			// If device isn't specified the current device will be used.

int32 		CompareColor(const RGBColor& color1, const RGBColor& color2);

int32 		CompareColorTable(CTabHandle lhs, CTabHandle rhs);

void 		PrepCopyBits(bool keepBackColor = false);
			// Set the fore and background colors to prevent colorization.

void		CopyColorBits(CGrafPtr srcBits, CGrafPtr dstBits, const Rect* srcRect, const Rect* dstRect, int16 mode, RgnHandle maskRgn);
			// CopyBits wrapper that does the ugly casts required to get CopyBits
			// to work with color ports.
					

// ===================================================================================
//	Text Functions
// ===================================================================================
std::wstring TruncateText(const std::wstring& str, int16 pixelWidth);
			// Chops characters off the end of str until it fits within pixelWidth.

std::wstring TruncateText(const std::wstring& str, int16 pixelWidth, TruncCode truncateAt);
			// Uses TruncText to find a substring that will fit within pixelWidth.
			// truncateAt can be truncEnd if characters should be removed from the
			// end of the string or truncMiddle if they should be removed from the
			// middle of the string. If the string is truncated an ellipsis is inserted.

void		TextBox(const std::wstring& text, const XRect& wrapBox, int16 align, int16 lhCode = 0);
			// This word wraps text inside a box like the toolbox's TextBox trap
			// except the box is not erased, align may be kFullJustification, and
			// lhCode can be used to control the line height (<0 based on tallest
			// character in the text, 0 based on GetFontInfo, >1 use lhCode as the
			// line height). The other values for align are teFlushRight, teFlushLeft,
			// teCenter, and teFlushDefault.


// ===================================================================================
//	class MSavePort
//!		Stack based class to save and restore the port.
// ===================================================================================
class MSavePort {

public:
						~MSavePort();

						MSavePort();
						/**< Note that the current port must be valid! */
						
private:
						MSavePort(const MSavePort& rhs);
						
			MSavePort& 	operator=(const MSavePort& rhs);

protected:
	CGrafPtr	mOldPort;
	GDHandle	mOldDevice;
	XPoint		mOldOrigin;
	RgnHandle	mOldClip;
};


// ===================================================================================
//	class MSetPort
//!		Stack based class to set and restore the port.
// ===================================================================================
class MSetPort {

public:
						~MSetPort();
							
#if !TARGET_CARBON
						MSetPort(GrafPtr port, GDHandle device = GetMainDevice());
						/**< If the port is actually a GWorld, device is ignored. */
#endif

						MSetPort(CGrafPtr port, GDHandle device = GetMainDevice());
						
#if TARGET_CARBON
						MSetPort(WindowRef window);
					
						MSetPort(DialogRef dialog);
#endif

private:
						MSetPort(const MSetPort& rhs);
						
			MSetPort& 	operator=(const MSetPort& rhs);

protected:
	CGrafPtr	mOldPort;		
	GDHandle	mOldDevice;
	
	CGrafPtr	mNewPort;		
	GDHandle	mNewDevice;
	XPoint		mNewOrigin;		
	RgnHandle	mNewClip;
};
		

// ===================================================================================
//	class MSetClip
//!		Stack based class to set and restore the clip region.
// ===================================================================================
class MSetClip {

public:
						~MSetClip();
							
						MSetClip(const XRect& clipRect);
						
						MSetClip(RgnHandle clipRgn);

private:
						MSetClip(const MSetClip& rhs);
						
			MSetClip& 	operator=(const MSetClip& rhs);

protected:
	RgnHandle	mOldClip;
};


// ===================================================================================
//	class MIntersectClip
//!		Stack based class to intersect the clip region with a rect or region.
// ===================================================================================
class MIntersectClip {

public:
						~MIntersectClip();
							
						MIntersectClip(const XRect& clipRect);
						
						MIntersectClip(RgnHandle clipRgn);

private:
						MIntersectClip(const MIntersectClip& rhs);
						
			MIntersectClip& operator=(const MIntersectClip& rhs);

protected:
	RgnHandle	mOldClip;
};


// ===================================================================================
//	class MSaveColorState
//!		Stack based class to save and restore the current port's fg and bk colors.
// ===================================================================================
class MSaveColorState {

public:
						~MSaveColorState();
					
						MSaveColorState();
				
			void 		Save();
			
			void 		Restore();
	
	static 	void 		Normalize();
						/**< Sets foreground color to black and background color to white. */

protected:
	RGBColor	mForegroundColor;
	RGBColor	mBackgroundColor;
};


// ===================================================================================
//	class MSaveColorPenState
//!		Stack based class to save and restore the current port's colors and pen state.
// ===================================================================================
class MSaveColorPenState {

public:
						~MSaveColorPenState();
					
						MSaveColorPenState();

			void 		Save();
	
			void 		Restore();
	
	static	void 		Normalize();
						/**< Sets fg color to black and bk color to white and calls PenNormal. */

protected:
	MSaveColorState	mColorState;
	PenState		mPenState;
};


// ===================================================================================
//	class MSaveTextState
//!		Stack based class to save and restore the current port's text state.
// ===================================================================================
class MSaveTextState {

public:
						~MSaveTextState();
					
						MSaveTextState();

			void 		Save();
	
			void 		Restore();
	
	static 	void 		Normalize();
						/**< Sets the text state to the default version of the system font. */

protected:
	int16		mFontNumber;
	int16		mSize;
	int16		mStyle;
	int16		mMode;
};


// ===================================================================================
//	Inlines
// ===================================================================================
inline void CopyColorBits(CGrafPtr srcBits, CGrafPtr dstBits, const Rect* srcRect, const Rect* dstRect, int16 mode, RgnHandle maskRgn)
{
#if TARGET_CARBON
	CopyBits((BitMap*) *GetPortPixMap(srcBits), (BitMap*) *GetPortPixMap(dstBits), srcRect, dstRect, mode, maskRgn);
#else
	CopyBits((BitMap*) &(srcBits->portPixMap), (BitMap*) &(dstBits->portPixMap), srcRect, dstRect, mode, maskRgn);
#endif
}
		

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

