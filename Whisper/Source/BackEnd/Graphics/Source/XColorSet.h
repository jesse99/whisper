/*
 *  File:       XColorSet.h
 *  Summary:	List of named XRGBColor's.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XColorSet.h,v $
 *		Revision 1.4  2001/02/12 09:48:06  jesjones
 *		Added #pragma export's.
 *		
 *		Revision 1.3  2000/12/10 08:50:38  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:28:07  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XRGBColor.h>

#include <vector>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	struct SNamedColor
// ===================================================================================
struct GRAPHICS_EXPORT SNamedColor {
	const wchar_t*	name;			// can't use an std::wstring because tables wind up larger than 32K and CW 5 refuses to compile the cpp
	XRGBColor		color;
	
			SNamedColor() : name(L"White"), color(kRGBWhite) {}
			SNamedColor(const wchar_t* n, const XRGBColor& c);
};

typedef std::vector<SNamedColor> XColorSet;


// ===================================================================================
//	Constants
// ===================================================================================
extern GRAPHICS_EXPORT const XColorSet kWhisperColors;		//!< the standard 12 Whisper/ODF colors (eg kRGBBlue, kRGBBlack, etc)
extern GRAPHICS_EXPORT const XColorSet kCrayolaColors;		//!< the 60 colors from Apple's Crayola color picker
extern GRAPHICS_EXPORT const XColorSet kHTMLColors;			//!< the 16 HTML 4.0 colors
extern GRAPHICS_EXPORT const XColorSet kNetscapeColors;		//!< the 140 Netscape colors (IE 3 supports almost all of these names)


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper


