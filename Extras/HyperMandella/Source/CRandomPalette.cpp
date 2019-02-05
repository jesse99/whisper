/*
 *  File:       CRandomPalette.cpp
 *  Summary:   	Implementation used to create randomized palettes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CRandomPalette.cpp,v $
 *		Revision 1.3  2001/04/21 02:24:58  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2001/04/05 23:52:06  jesjones
 *		Tweaked for the picky Intel compiler.
 *		
 *		Revision 1.1  2001/03/30 04:08:55  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include "IRandomPalette.h"

#include <XHSVColor.h>
#include <XNumbers.h>
#include <XPreference.h>


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// ScaledRandom   
//
//---------------------------------------------------------------
static float ScaledRandom(double exponent)
{
	double x = Random(1.0);
	double y = 1.0 - pow(x, exponent);
	
	return (float) y;
}


//---------------------------------------------------------------
//
// GetDelta
//
//---------------------------------------------------------------
static XHSVColor GetDelta(const XHSVColor& first, const XHSVColor& last, int32 size)
{
	float delta = last.hue - first.hue;
		
	if (last.hue > first.hue && delta > 0.5)		// take the shortest way around the color wheel
		delta -= 1.0f;
	else if (Abs(delta) > 0.5)
		delta += 1.0f;
	
	if (size == 0)
		size = 1;
	
	XHSVColor result;
	result.hue        = delta/size;
	result.saturation = (last.saturation - first.saturation)/size;
	result.value      = (last.value - first.value)/size;
	
	return result;
}

#pragma mark -

// ===================================================================================
//	class CRandomPalette
//!		Implementation used to create randomized palettes.
// ===================================================================================
class CRandomPalette : public IRandomPalette {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CRandomPalette();
	
						CRandomPalette(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual XColorTable GetPalette();						
	virtual XRGBColor 	GetColor();
	
//-----------------------------------
//	Internal API
//
private:
			void 		DoShadeColors(XColorTable* colors, uint32 first, uint32 last);
};

DEFINE_INTERFACE_FACTORY(CRandomPalette)

//---------------------------------------------------------------
//
// CRandomPalette::~CRandomPalette
//
//---------------------------------------------------------------
CRandomPalette::~CRandomPalette()
{
}


//---------------------------------------------------------------
//
// CRandomPalette::CRandomPalette
//
//---------------------------------------------------------------
CRandomPalette::CRandomPalette(XBoss* boss) 
{	
	IRandomPalette::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CRandomPalette::GetPalette
//
//---------------------------------------------------------------
XColorTable CRandomPalette::GetPalette()
{
	XPreference<uint32>	colorCount(L"Number of Colors", kDefaultNumColors);
	XPreference<int32>  complexity(L"Color Complexity", kDefaultComplexity);
	
	uint32 numColors = *colorCount + 1;

	XColorTable colors;
	colors.SetSize(numColors);
	
	XRGBColor color = this->GetColor();
	colors.SetColor(0, color);
	colors.SetColor(numColors-1, color);
	
	ASSERT(*complexity > 0);
	uint32 index = 0;
	
	uint32 delta = std::max(numColors/(*complexity), 1UL);	
	while (index + 2*delta < numColors) {
		colors.SetColor(index + delta, this->GetColor());
		this->DoShadeColors(&colors, index, index + delta);
		index += delta;
	}

	if (index < numColors)
		this->DoShadeColors(&colors, index, numColors - 1UL);
	
	return colors;
}


//---------------------------------------------------------------
//
// CRandomPalette::GetColor
//
//---------------------------------------------------------------
XRGBColor CRandomPalette::GetColor()
{
	XHSVColor color;
	
	XPreference<double>	saturation(L"Color Saturation", kDefaultSaturation);
	XPreference<double>	value(L"Color Value", kDefaultValue);

	color.hue        = Random(1.0f);
	color.saturation = ScaledRandom(*saturation);
	color.value      = ScaledRandom(*value);

	return color;
}
	
#if __MWERKS__
#pragma mark ~
#endif
			
//---------------------------------------------------------------
//
// CRandomPalette::DoShadeColors
//
//---------------------------------------------------------------
void CRandomPalette::DoShadeColors(XColorTable* colors, uint32 first, uint32 last)
{
	PRECONDITION(colors != nil);
	PRECONDITION(last < colors->GetSize());
	PRECONDITION(first <= last);
	
	XHSVColor lo = colors->GetColor(first);
	XHSVColor hi = colors->GetColor(last);
	XHSVColor delta = GetDelta(lo, hi, (int32) (last - first + 1));
	XHSVColor color = lo;

	for (uint32 entry = first; entry < last; entry++) {
		colors->SetColor(entry, color);

		color.hue        += delta.hue;				
		color.saturation += delta.saturation;
		color.value      += delta.value;
		
		if (color.hue > 1.0)
			color.hue = 0.0f;
		else if (color.hue < 0.0)
			color.hue = 1.0f;
	}
}

