/*
 *  File:       XColorSet.cpp
 *  Summary:	List of named XRGBColor's.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XColorSet.cpp,v $
 *		Revision 1.3  2001/04/27 04:31:03  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:28:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	  5/15/00	JDJ		Fixed a couple of names in kCrayolaTable: "Red" wasn't 
 *									capitalized and "Sky Blue" was missing the "S".
 *									to "Sky Blue".
 *		 <1>	  8/11/99	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <XColorSet.h>

#include <XARGBColor.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XHSVColor.h>
#include <XNumbers.h>

#if MAC
	#include <MDialogUtils.h>
	#include <MSystemInfo.h>
#endif

namespace Whisper {


SNamedColor::SNamedColor(const wchar_t* n, const XRGBColor& c) : name(n), color(c) {}

// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// MakeColor
//
//---------------------------------------------------------------
static XRGBColor MakeColor(uint32 red, uint32 green, uint32 blue) 
{
	PRECONDITION(red <= 255);
	PRECONDITION(green <= 255);
	PRECONDITION(blue <= 255);
	
	return XRGBColor(red/255.0, green/255.0, blue/255.0);
}


// ===================================================================================
//	Internal Tables
// ===================================================================================
const SNamedColor kWhisperTable[12] = {						// hard-code values to avoid order of initialization problems
	SNamedColor(L"Black",	 	XRGBColor(0.0,  0.0,  0.0)), 		
	SNamedColor(L"White",	 	XRGBColor(1.0,  1.0,  1.0)), 		
	SNamedColor(L"Red",		 	XRGBColor(0.87, 0.0,  0.0)), 
	SNamedColor(L"Yellow",	 	XRGBColor(1.0,  1.0,  0.0)), 
	SNamedColor(L"Blue",	 	XRGBColor(0.0,  0.0,  0.87)), 
	SNamedColor(L"Green",	 	XRGBColor(0.0,  0.93, 0.0)), 
	SNamedColor(L"Brown",	 	XRGBColor(0.6,  0.4,  0.0)), 
	SNamedColor(L"Purple",	 	XRGBColor(0.4,  0.0,  0.6)), 
	SNamedColor(L"Light Blue", 	XRGBColor(0.0,  0.6,  1.0)), 
	SNamedColor(L"Gray",     	XRGBColor(0.53, 0.53, 0.53)), 
	SNamedColor(L"Dark Gray", 	XRGBColor(0.25, 0.25, 0.25)), 
	SNamedColor(L"Light Gray",	XRGBColor(0.75, 0.75, 0.75))
};

const SNamedColor kCrayolaTable[60] = {
	SNamedColor(L"Chalk",			XRGBColor(1.00, 1.00, 1.00)),		
	SNamedColor(L"Marble",			XRGBColor(0.93, 0.93, 0.93)),
	SNamedColor(L"Soapstone",		XRGBColor(0.80, 0.80, 0.80)),
	SNamedColor(L"Concrete",		XRGBColor(0.67, 0.67, 0.67)),
	SNamedColor(L"Granite",			XRGBColor(0.53, 0.53, 0.53)),
	SNamedColor(L"Flint",			XRGBColor(0.47, 0.47, 0.47)),
	SNamedColor(L"Shale",			XRGBColor(0.33, 0.33, 0.33)),
	SNamedColor(L"Gabbro",			XRGBColor(0.20, 0.20, 0.20)),
	SNamedColor(L"Basalt",			XRGBColor(0.13, 0.13, 0.13)),
	SNamedColor(L"Obsidian",		XRGBColor(0.00, 0.00, 0.00)),

	SNamedColor(L"Violet Dusk",		XRGBColor(0.45, 0.40, 0.86)),
	SNamedColor(L"Sky Blue",		XRGBColor(0.36, 0.53, 0.95)),
	SNamedColor(L"Ocean Green",		XRGBColor(0.38, 0.84, 0.67)),
	SNamedColor(L"Spring Frost",	XRGBColor(0.37, 0.74, 0.44)),
	SNamedColor(L"Mildew",			XRGBColor(0.49, 0.51, 0.36)),
	SNamedColor(L"Mustard",			XRGBColor(0.90, 0.90, 0.35)),
	SNamedColor(L"Canteloupe",		XRGBColor(1.00, 0.75, 0.34)),
	SNamedColor(L"Tulip",			XRGBColor(0.97, 0.48, 0.34)),
	SNamedColor(L"Carnation",		XRGBColor(0.85, 0.27, 0.42)),
	SNamedColor(L"Orchid",			XRGBColor(0.73, 0.34, 0.76)),

	SNamedColor(L"Pale Violet",		XRGBColor(0.51, 0.33, 0.82)),
	SNamedColor(L"Evening Blue",	XRGBColor(0.41, 0.46, 0.91)),
	SNamedColor(L"Fog",				XRGBColor(0.36, 0.73, 0.79)),
	SNamedColor(L"Chlorine",		XRGBColor(0.38, 0.79, 0.56)),
	SNamedColor(L"Moss",			XRGBColor(0.44, 0.60, 0.37)),
	SNamedColor(L"Olive",			XRGBColor(0.70, 0.71, 0.35)),
	SNamedColor(L"Banana",			XRGBColor(1.00, 0.91, 0.34)),
	SNamedColor(L"Grapefruit",		XRGBColor(1.00, 0.58, 0.34)),
	SNamedColor(L"Salmon",			XRGBColor(0.94, 0.38, 0.34)),
	SNamedColor(L"Grape",			XRGBColor(0.86, 0.33, 0.68)),

	SNamedColor(L"Violet",			XRGBColor(0.35, 0.09, 0.73)),
	SNamedColor(L"Blue",			XRGBColor(0.09, 0.13, 0.80)),
	SNamedColor(L"Seaweed",			XRGBColor(0.09, 0.38, 0.35)),
	SNamedColor(L"Clover",			XRGBColor(0.18, 0.55, 0.13)),
	SNamedColor(L"Cactus",			XRGBColor(0.55, 0.74, 0.11)),
	SNamedColor(L"Lemon",			XRGBColor(1.00, 0.92, 0.09)),
	SNamedColor(L"Tangerine",		XRGBColor(1.00, 0.69, 0.09)),
	SNamedColor(L"Melon",			XRGBColor(1.00, 0.46, 0.09)),
	SNamedColor(L"Red Orange",		XRGBColor(0.96, 0.25, 0.11)),
	SNamedColor(L"Red",				XRGBColor(0.93, 0.09, 0.12)),

	SNamedColor(L"Royal Violet",	XRGBColor(0.42, 0.09, 0.69)),
	SNamedColor(L"Blue Violet",		XRGBColor(0.27, 0.09, 0.78)),
	SNamedColor(L"Sea Blue",		XRGBColor(0.09, 0.29, 0.51)),
	SNamedColor(L"Pine",			XRGBColor(0.09, 0.46, 0.20)),
	SNamedColor(L"Fern",			XRGBColor(0.36, 0.64, 0.12)),
	SNamedColor(L"Watercress",		XRGBColor(0.73, 0.83, 0.10)),
	SNamedColor(L"Marigold",		XRGBColor(1.00, 0.80, 0.09)),
	SNamedColor(L"Orange",			XRGBColor(1.00, 0.57, 0.09)),
	SNamedColor(L"Fire",			XRGBColor(0.98, 0.31, 0.10)),
	SNamedColor(L"Apple",			XRGBColor(0.94, 0.12, 0.11)),

	SNamedColor(L"Sepia",			XRGBColor(0.16, 0.10, 0.06)),
	SNamedColor(L"Raw Sienna",		XRGBColor(0.36, 0.24, 0.14)),
	SNamedColor(L"Dirt",			XRGBColor(0.55, 0.38, 0.21)),
	SNamedColor(L"Tan",				XRGBColor(0.75, 0.52, 0.29)),
	SNamedColor(L"Warm Marble",		XRGBColor(0.80, 0.78, 0.76)),
	SNamedColor(L"Warm Granite",	XRGBColor(0.55, 0.50, 0.45)),
	SNamedColor(L"Warm Shale",		XRGBColor(0.24, 0.22, 0.20)),
	SNamedColor(L"Cool Marble",		XRGBColor(0.76, 0.76, 0.80)),
	SNamedColor(L"Cool Granite",	XRGBColor(0.45, 0.45, 0.55)),
	SNamedColor(L"Cool Shale",		XRGBColor(0.20, 0.20, 0.24))
};

const SNamedColor kHTMLTable[16] = {
	SNamedColor(L"Black",	 	MakeColor(0x00, 0x00, 0x00)), 		
	SNamedColor(L"Silver",	 	MakeColor(0xC0, 0xC0, 0x00)), 		
	SNamedColor(L"Gray",	 	MakeColor(0x80, 0x80, 0x80)), 		
	SNamedColor(L"White",	 	MakeColor(0xFF, 0xFF, 0xFF)), 		
	SNamedColor(L"Green",	 	MakeColor(0x00, 0x80, 0x00)), 		
	SNamedColor(L"Lime",	 	MakeColor(0x00, 0xFF, 0x00)), 		
	SNamedColor(L"Olive",	 	MakeColor(0x80, 0x80, 0x00)), 		
	SNamedColor(L"Yellow",	 	MakeColor(0xFF, 0xFF, 0x00)), 		
	SNamedColor(L"Maroon",	 	MakeColor(0x80, 0x00, 0x00)), 		
	SNamedColor(L"Red",		 	MakeColor(0xFF, 0x00, 0x00)), 		
	SNamedColor(L"Purple",	 	MakeColor(0x80, 0x00, 0x80)), 		
	SNamedColor(L"Fuchsia",	 	MakeColor(0xFF, 0x00, 0xFF)), 		
	SNamedColor(L"Navy",	 	MakeColor(0x00, 0x00, 0x80)), 		
	SNamedColor(L"Blue",	 	MakeColor(0x00, 0x00, 0xFF)), 		
	SNamedColor(L"Teal",	 	MakeColor(0x00, 0x80, 0x80)), 		
	SNamedColor(L"Aqua",	 	MakeColor(0x00, 0xFF, 0xFF)), 		
};

const SNamedColor kNetscapeTable[140] = {			// from <http://developer.netscape.com:80/docs/manuals/communicator/jsguide4/colors.htm>
	SNamedColor(L"Alice Blue",	 		MakeColor(0xF0, 0xF8, 0xFF)),
	SNamedColor(L"Antique White",	 	MakeColor(0xFA, 0xEB, 0xD7)),
	SNamedColor(L"Aqua",	 			MakeColor(0x00, 0xFF, 0xFF)),
	SNamedColor(L"Aquamarine",	 		MakeColor(0x7F, 0xFF, 0xD4)),
	SNamedColor(L"Azure",	 			MakeColor(0xF0, 0xFF, 0xFF)),
	SNamedColor(L"Beige",	 			MakeColor(0xF5, 0xF5, 0xDC)),
	SNamedColor(L"Bisque",	 			MakeColor(0xFF, 0xE4, 0xC4)),
	SNamedColor(L"Black",	 			MakeColor(0x00, 0x00, 0x00)),
	SNamedColor(L"Blanched Almond",	 	MakeColor(0xFF, 0xEB, 0xCD)),
	SNamedColor(L"Blue",	 			MakeColor(0x00, 0x00, 0xFF)),
	SNamedColor(L"Blue Violet",	 		MakeColor(0x8A, 0x2B, 0xE2)),
	SNamedColor(L"Brown",	 			MakeColor(0xA5, 0x2A, 0x2A)),
	SNamedColor(L"Burlywood",	 		MakeColor(0xDE, 0xB8, 0x87)),
	SNamedColor(L"Cadet Blue",	 		MakeColor(0x5F, 0x9E, 0xA0)),
	SNamedColor(L"Chartreuse",	 		MakeColor(0x7F, 0xFF, 0x00)),
	SNamedColor(L"Chocolate",	 		MakeColor(0xD2, 0x69, 0x1E)),
	SNamedColor(L"Coral",	 			MakeColor(0xFF, 0x7F, 0x50)),
	SNamedColor(L"Cornflower Blue",	 	MakeColor(0x64, 0x95, 0xED)),
	SNamedColor(L"Cornsilk",	 		MakeColor(0xFF, 0xF8, 0xDC)),
	SNamedColor(L"Crimson",	 			MakeColor(0xDC, 0x14, 0x3C)),
	SNamedColor(L"Cyan",	 			MakeColor(0x00, 0xFF, 0xFF)),
	SNamedColor(L"Dark Blue",	 		MakeColor(0x00, 0x00, 0x8B)),
	SNamedColor(L"Dark Cyan",	 		MakeColor(0x00, 0x8B, 0x8B)),
	SNamedColor(L"Dark Goldenrod",	 	MakeColor(0xB8, 0x86, 0x0B)),
	SNamedColor(L"Dark Gray",	 		MakeColor(0xA9, 0xA9, 0xA9)),
	SNamedColor(L"Dark Green",	 		MakeColor(0x00, 0x64, 0x00)),
	SNamedColor(L"Dark Khaki",	 		MakeColor(0xBD, 0xB7, 0x6B)),
	SNamedColor(L"Dark Magenta",	 	MakeColor(0x8B, 0x00, 0x8B)),
	SNamedColor(L"Dark Olive Green",	MakeColor(0x55, 0x6B, 0x2F)),
	SNamedColor(L"Dark Orange",	 		MakeColor(0xFF, 0x8C, 0x00)),
	SNamedColor(L"Dark Orchid",	 		MakeColor(0x99, 0x32, 0xCC)),
	SNamedColor(L"Dark Red",	 		MakeColor(0x8B, 0x00, 0x00)),
	SNamedColor(L"Dark Salmon",	 		MakeColor(0xE9, 0x96, 0x7A)),
	SNamedColor(L"Dark Sea Green",	 	MakeColor(0x8F, 0xBC, 0x8F)),
	SNamedColor(L"Dark Slate Blue",	 	MakeColor(0x48, 0x3D, 0x8B)),
	SNamedColor(L"Dark Slate Gray",	 	MakeColor(0x2F, 0x4F, 0x4F)),
	SNamedColor(L"Darkt Turquoise",	 	MakeColor(0x00, 0xCE, 0xD1)),
	SNamedColor(L"Dark Violet",	 		MakeColor(0x94, 0x00, 0xD3)),
	SNamedColor(L"Deep Pink",	 		MakeColor(0xFF, 0x14, 0x93)),
	SNamedColor(L"Deep Sky Blue",	 	MakeColor(0x00, 0xBF, 0xFF)),
	SNamedColor(L"Dim Gray",	 		MakeColor(0x69, 0x69, 0x69)),
	SNamedColor(L"Dodger Blue",	 		MakeColor(0x1E, 0x90, 0xFF)),
	SNamedColor(L"Firebrick",	 		MakeColor(0xB2, 0x22, 0x22)),
	SNamedColor(L"Floral White",	 	MakeColor(0xFF, 0xFA, 0xF0)),
	SNamedColor(L"Forest Green",	 	MakeColor(0x22, 0x8B, 0x22)),
	SNamedColor(L"Fuchsia",	 			MakeColor(0xFF, 0x00, 0xFF)),
	SNamedColor(L"Gainsboro",	 		MakeColor(0xDC, 0xDC, 0xDC)),
	SNamedColor(L"Ghost White",	 		MakeColor(0xF8, 0xF8, 0xFF)),
	SNamedColor(L"Gold",	 			MakeColor(0xFF, 0xD7, 0x00)),
	SNamedColor(L"Goldenrod",	 		MakeColor(0xDA, 0xA5, 0x20)),
	SNamedColor(L"Gray",	 			MakeColor(0x80, 0x80, 0x80)),
	SNamedColor(L"Green",	 			MakeColor(0x00, 0x80, 0x00)),
	SNamedColor(L"Green Yellow",	 	MakeColor(0xAD, 0xFF, 0x2F)),
	SNamedColor(L"Honeydew",	 		MakeColor(0xF0, 0xFF, 0xF0)),
	SNamedColor(L"Hot Pink",	 		MakeColor(0xFF, 0x69, 0xB4)),
	SNamedColor(L"Indian Red",	 		MakeColor(0xCD, 0x5C, 0x5C)),
	SNamedColor(L"Indigo",	 			MakeColor(0x4B, 0x00, 0x82)),
	SNamedColor(L"Ivory",	 			MakeColor(0xFF, 0xFF, 0xF0)),
	SNamedColor(L"Khaki",	 			MakeColor(0xF0, 0xE6, 0x8C)),
	SNamedColor(L"Lavender",	 		MakeColor(0xE6, 0xE6, 0xFA)),
	SNamedColor(L"Lavender Blush",	 	MakeColor(0xFF, 0xF0, 0xF5)),
	SNamedColor(L"Lawn Green",	 		MakeColor(0x7C, 0xFC, 0x00)),
	SNamedColor(L"Lemon Chiffon",	 	MakeColor(0xFF, 0xFA, 0xCD)),
	SNamedColor(L"Light Blue",	 		MakeColor(0xAD, 0xD8, 0xE6)),
	SNamedColor(L"Light Coral",	 		MakeColor(0xF0, 0x80, 0x80)),
	SNamedColor(L"Light Cyan",	 		MakeColor(0xE0, 0xFF, 0xFF)),
	SNamedColor(L"Light Goldenrod Yellow",MakeColor(0xFA, 0xFA, 0xD2)),
	SNamedColor(L"Light Green",	 		MakeColor(0x90, 0xEE, 0x90)),
	SNamedColor(L"Light Grey",	 		MakeColor(0xD3, 0xD3, 0xD3)),
	SNamedColor(L"Light Pink",	 		MakeColor(0xFF, 0xB6, 0xC1)),
	SNamedColor(L"Light Salmon",	 	MakeColor(0xFF, 0xA0, 0x7A)),
	SNamedColor(L"Light Sea Green",	 	MakeColor(0x20, 0xB2, 0xAA)),
	SNamedColor(L"Light Sky Blue",	 	MakeColor(0x87, 0xCE, 0xFA)),
	SNamedColor(L"Light Slate Gray",	MakeColor(0x77, 0x88, 0x99)),
	SNamedColor(L"Light Steel Blue",	MakeColor(0xB0, 0xC4, 0xDE)),
	SNamedColor(L"Light Yellow",	 	MakeColor(0xFF, 0xFF, 0xE0)),
	SNamedColor(L"Lime",	 			MakeColor(0x00, 0xFF, 0x00)),
	SNamedColor(L"Lime Green",	 		MakeColor(0x32, 0xCD, 0x32)),
	SNamedColor(L"Linen",	 			MakeColor(0xFA, 0xF0, 0xE6)),
	SNamedColor(L"Magenta",	 			MakeColor(0xFF, 0x00, 0xFF)),
	SNamedColor(L"Maroon",	 			MakeColor(0x80, 0x00, 0x00)),
	SNamedColor(L"Medium Aquamarine",	MakeColor(0x66, 0xCD, 0xAA)),
	SNamedColor(L"Medium Blue",	 		MakeColor(0x00, 0x00, 0xCD)),
	SNamedColor(L"Medium Orchid",	 	MakeColor(0xBA, 0x55, 0xD3)),
	SNamedColor(L"Medium Purple",	 	MakeColor(0x93, 0x70, 0xDB)),
	SNamedColor(L"Medium Sea Green",	MakeColor(0x3C, 0xB3, 0x71)),
	SNamedColor(L"Medium Slate Blue",	MakeColor(0x7B, 0x68, 0xEE)),
	SNamedColor(L"Medium Spring Green",	MakeColor(0x00, 0xFA, 0x9A)),
	SNamedColor(L"Medium Turquoise",	MakeColor(0x48, 0xD1, 0xCC)),
	SNamedColor(L"Medium Violet Red",	MakeColor(0xC7, 0x15, 0x85)),
	SNamedColor(L"Midnight Blue",	 	MakeColor(0x19, 0x19, 0x70)),
	SNamedColor(L"Mint Cream",	 		MakeColor(0xF5, 0xFF, 0xFA)),
	SNamedColor(L"Misty Rose",	 		MakeColor(0xFF, 0xE4, 0xE1)),
	SNamedColor(L"Moccasin",	 		MakeColor(0xFF, 0xE4, 0xB5)),
	SNamedColor(L"Navajo White",	 	MakeColor(0xFF, 0xDE, 0xAD)),
	SNamedColor(L"Navy",	 			MakeColor(0x00, 0x00, 0x80)),
	SNamedColor(L"Old Lace",	 		MakeColor(0xFD, 0xF5, 0xE6)),
	SNamedColor(L"Olive",	 			MakeColor(0x80, 0x80, 0x00)),
	SNamedColor(L"Olive Drab",	 		MakeColor(0x6B, 0x8E, 0x23)),
	SNamedColor(L"Orange",	 			MakeColor(0xFF, 0xA5, 0x00)),
	SNamedColor(L"Orange Red",	 		MakeColor(0xFF, 0x45, 0x00)),
	SNamedColor(L"Orchid",	 			MakeColor(0xDA, 0x70, 0xD6)),
	SNamedColor(L"Pale Goldenrod",	 	MakeColor(0xEE, 0xE8, 0xAA)),
	SNamedColor(L"Pale Green",	 		MakeColor(0x98, 0xFB, 0x98)),
	SNamedColor(L"Pale Turquoise",	 	MakeColor(0xAF, 0xEE, 0xEE)),
	SNamedColor(L"Pale Violet Red",	 	MakeColor(0xDB, 0x70, 0x93)),
	SNamedColor(L"Papayawhip",	 		MakeColor(0xFF, 0xEF, 0xD5)),
	SNamedColor(L"Peachpuff",	 		MakeColor(0xFF, 0xDA, 0xB9)),
	SNamedColor(L"Peru",	 			MakeColor(0xCD, 0x85, 0x3F)),
	SNamedColor(L"Pink",	 			MakeColor(0xFF, 0xC0, 0xCB)),
	SNamedColor(L"Plum",	 			MakeColor(0xDD, 0xA0, 0xDD)),
	SNamedColor(L"Powder Blue",	 		MakeColor(0xB0, 0xE0, 0xE6)),
	SNamedColor(L"Purple",	 			MakeColor(0x80, 0x00, 0x80)),
	SNamedColor(L"Red",	 				MakeColor(0xFF, 0x00, 0x00)),
	SNamedColor(L"Rosy Brown",	 		MakeColor(0xBC, 0x8F, 0x8F)),
	SNamedColor(L"Royal Blue",	 		MakeColor(0x41, 0x69, 0xE1)),
	SNamedColor(L"Saddle Brown",	 	MakeColor(0x8B, 0x45, 0x13)),
	SNamedColor(L"Salmon",	 			MakeColor(0xFA, 0x80, 0x72)),
	SNamedColor(L"Sandy Brown",	 		MakeColor(0xF4, 0xA4, 0x60)),
	SNamedColor(L"Sea Green",	 		MakeColor(0x2E, 0x8B, 0x57)),
	SNamedColor(L"Seashell",	 		MakeColor(0xFF, 0xF5, 0xEE)),
	SNamedColor(L"Sienna",	 			MakeColor(0xA0, 0x52, 0x2D)),
	SNamedColor(L"Silver",	 			MakeColor(0xC0, 0xC0, 0xC0)),
	SNamedColor(L"Sky Blue",	 		MakeColor(0x87, 0xCE, 0xEB)),
	SNamedColor(L"Slate Blue",	 		MakeColor(0x6A, 0x5A, 0xCD)),
	SNamedColor(L"Slate Gray",	 		MakeColor(0x70, 0x80, 0x90)),
	SNamedColor(L"Snow",	 			MakeColor(0xFF, 0xFA, 0xFA)),
	SNamedColor(L"Spring Green",	 	MakeColor(0x00, 0xFF, 0x7F)),
	SNamedColor(L"Steel Blue",	 		MakeColor(0x46, 0x82, 0xB4)),
	SNamedColor(L"Tan",	 				MakeColor(0xD2, 0xB4, 0x8C)),
	SNamedColor(L"Teal",	 			MakeColor(0x00, 0x80, 0x80)),
	SNamedColor(L"Thistle",	 			MakeColor(0xD8, 0xBF, 0xD8)),
	SNamedColor(L"Tomato",	 			MakeColor(0xFF, 0x63, 0x47)),
	SNamedColor(L"Turquoise",		 	MakeColor(0x40, 0xE0, 0xD0)),
	SNamedColor(L"Violet",	 			MakeColor(0xEE, 0x82, 0xEE)),
	SNamedColor(L"Wheat",	 			MakeColor(0xF5, 0xDE, 0xB3)),
	SNamedColor(L"White",	 			MakeColor(0xFF, 0xFF, 0xFF)),
	SNamedColor(L"White Smoke",	 		MakeColor(0xF5, 0xF5, 0xF5)),
	SNamedColor(L"Yellow",	 			MakeColor(0xFF, 0xFF, 0x00)),
	SNamedColor(L"Yellow Green",	 	MakeColor(0x9A, 0xCD, 0x32)),
};


// ===================================================================================
//	Global Constants
// ===================================================================================
const XColorSet kWhisperColors(kWhisperTable, kWhisperTable + 12);
const XColorSet kCrayolaColors(kCrayolaTable, kCrayolaTable + 60);	
const XColorSet kHTMLColors(kHTMLTable, kHTMLTable + 16);
const XColorSet kNetscapeColors(kNetscapeTable, kNetscapeTable + 140);


}	// namespace Whisper


