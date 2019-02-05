/*
 *  File:       IRandomPalette.h
 *  Summary:   	Interface used to create randomized palettes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IRandomPalette.h,v $
 *		Revision 1.1  2001/03/30 04:09:05  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XColorTable.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


//-----------------------------------
//	Constants
//
const uint32 kDefaultNumColors  = 1024;
const int32  kDefaultComplexity = 4;
const double kDefaultSaturation = 3.0;
const double kDefaultValue      = 3.0;


// ===================================================================================
//	class IRandomPalette
//!		Interface used to create randomized palettes.
// ===================================================================================
class IRandomPalette : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual XColorTable GetPalette() = 0;
						/**< Get a random palette. */
						
	virtual XRGBColor 	GetColor() = 0;
};

typedef XInterfacePtr<IRandomPalette> IRandomPalettePtr;
typedef XInterfacePtr<const IRandomPalette> IConstRandomPalettePtr;

