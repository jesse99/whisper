/*
 *  File:       XHSVColor.cpp
 *  Summary:	HSVColor wrapper
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XHSVColor.cpp,v $
 *		Revision 1.4  2001/04/27 04:33:12  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/03/02 11:24:30  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.2  2000/11/09 12:38:16  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XHSVColor.h>

#include <XARGBColor.h>
#include <XDebug.h>
#include <XNumbers.h>
#include <XRGBColor.h>
#include <XTextConversions.h>

namespace Whisper {


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// FromStr (wstring, XHSVColor)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& inStr, XHSVColor& value)
{
	FromStr(inStr, value.hue);
	
	std::wstring str = After(inStr, L" ");
	FromStr(str, value.saturation);
	
	str = After(str, L" ");
	FromStr(str, value.value);
}


//---------------------------------------------------------------
//
// ToStr (XHSVColor)
//
//---------------------------------------------------------------
std::wstring ToStr(const XHSVColor& value)
{
	return ToStr(value.hue) + L" " + ToStr(value.saturation) + L" " + ToStr(value.value);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XHSVColor
// ===================================================================================

//---------------------------------------------------------------
//
// XHSVColor::XHSVColor (double, double, double)
//
//---------------------------------------------------------------
XHSVColor::XHSVColor(double theHue, double theSat, double theValue)
{
	PRECONDITION(theHue >= 0.0 && theHue <= 1.0);
	PRECONDITION(theSat >= 0.0 && theSat <= 1.0);
	PRECONDITION(theValue >= 0.0 && theValue <= 1.0);
	
	hue        = (float) theHue;
	saturation = (float) theSat;
	value      = (float) theValue;
}


//---------------------------------------------------------------
//
// XHSVColor::XHSVColor (XRGBColor)
//
//---------------------------------------------------------------
XHSVColor::XHSVColor(const XRGBColor& color)
{	
	double max = Max(color.red, color.green, color.blue);	// From Foley and Van Dam section 13.3.4
	double min = Min(color.red, color.green, color.blue);
	
	double v = max;
	double s = max != 0.0 ? (max - min)/max : 0.0;
	
	double h = 0.0;
	if (s != 0.0) {
		double delta = max - min;
		
		if (color.red == max)
			h = (color.green - color.blue)/delta;
		else if (color.green == max)
			h = 2.0 + (color.blue - color.red)/delta;
		else
			h = 4.0 + (color.red - color.green)/delta;
		
		h *= 60.0;
		
		if (h < 0.0)
			h += 360.0;
	}
	
	hue        = (float) (h/360.0);
	saturation = (float) s;
	value      = (float) v;
}


//---------------------------------------------------------------
//
// XHSVColor::XHSVColor (XARGBColor)
//
//---------------------------------------------------------------
XHSVColor::XHSVColor(const XARGBColor& inColor)
{	
	XRGBColor color(inColor);
	
	*this = color;
}


//---------------------------------------------------------------
//
// XHSVColor::XHSVColor (HSVColor)
//
//---------------------------------------------------------------
#if MAC
XHSVColor::XHSVColor(const HSVColor& rhs)
{
	hue        = (float) (rhs.hue/65535.0);
	saturation = (float) (rhs.saturation/65535.0);
	value      = (float) (rhs.value/65535.0);
}
#endif


//---------------------------------------------------------------
//
// XHSVColor::GetOSColor
//
//---------------------------------------------------------------
OSColor XHSVColor::GetOSColor() const
{	
	XRGBColor color(*this);

	return color.GetOSColor();
}

		
//---------------------------------------------------------------
//
// XHSVColor::GetColorRef
//
//---------------------------------------------------------------
#if WIN
COLORREF XHSVColor::GetColorRef() const
{
	OSColor color = this->GetOSColor();

	COLORREF ref = RGB(color.rgbRed, color.rgbGreen, color.rgbBlue);
	
	return ref;
}
#endif


//---------------------------------------------------------------
//
// XHSVColor::operator==
//
//---------------------------------------------------------------
bool XHSVColor::operator==(const XHSVColor& rhs) const
{
	bool equal = false;
	
	if (Equal(hue, rhs.hue))
		if (Equal(saturation, rhs.saturation))
			if (Equal(value, rhs.value))
				equal = true;

	return equal;
}
	

//---------------------------------------------------------------
//
// Lumenance
//
//---------------------------------------------------------------
double XHSVColor::GetLumenance() const
{
	XRGBColor color(*this);

	double lumenance = color.GetLumenance();
	
	return lumenance;
}


}	// namespace Whisper
