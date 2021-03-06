/*
 *  File:       XARGBColor.cpp
 *  Summary:	Alpha weighted RGB color class (the components should be in [0, 1]).
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XARGBColor.cpp,v $
 *		Revision 1.5  2001/07/13 22:09:08  jesjones
 *		ToStr and FromStr now assume alpha is the last component (instead of the first).
 *		
 *		Revision 1.4  2001/04/21 03:39:24  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/03/02 11:24:05  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.2  2000/11/09 12:25:44  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XARGBColor.h>

#include <XDebug.h>
#include <XMiscUtils.h>
#include <XNumbers.h>
#include <XRGBColor.h>
#include <XStreaming.h>
#include <XTextConversions.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const double kColorTolerance = 1.0/256.0;


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// operator<< (XARGBColor)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const XARGBColor& value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('ACLR');
#endif

	stream << value.alpha << value.red << value.green << value.blue;

	return stream;
}
	

//---------------------------------------------------------------
//
// operator>> (XARGBColor)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, XARGBColor& value)
{	
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'ACLR');
		
	stream >> value.alpha >> value.red >> value.green >> value.blue;
	
	if (stream.NeedsByteSwap()) {
		ByteSwap(value.alpha);
		ByteSwap(value.red);
		ByteSwap(value.green);
		ByteSwap(value.blue);
	}
	
	return stream;
}


//---------------------------------------------------------------
//
// FromStr (wstring, XARGBColor)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& inStr, XARGBColor& value)
{
	FromStr(inStr, value.red);
	
	std::wstring str = After(inStr, L" ");
	FromStr(str, value.green);
	
	str = After(str, L" ");
	FromStr(str, value.blue);
	
	str = After(str, L" ");
	FromStr(str, value.alpha);
}


//---------------------------------------------------------------
//
// ToStr (XARGBColor)
//
//---------------------------------------------------------------
std::wstring ToStr(const XARGBColor& value)
{
	return ToStr(value.red) + L" " + ToStr(value.green) + L" " + ToStr(value.blue) +  + L" " + ToStr(value.alpha);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XARGBColor
// ===================================================================================

//---------------------------------------------------------------
//
// XARGBColor::XARGBColor (OSColor)
//
//---------------------------------------------------------------
XARGBColor::XARGBColor(const OSColor& rhs)
{
	COMPILE_CHECK(offsetof(XARGBColor, red) == 0);		// these are required by OpenGL
	COMPILE_CHECK(offsetof(XARGBColor, green) == sizeof(float));
	COMPILE_CHECK(offsetof(XARGBColor, blue) == 2*sizeof(float));
	COMPILE_CHECK(offsetof(XARGBColor, alpha) == 3*sizeof(float));
	COMPILE_CHECK(sizeof(XARGBColor) == 4*sizeof(float));
	
	alpha = 1.0f;
	
#if MAC
	red   = (float) (rhs.red/65535.0);
	green = (float) (rhs.green/65535.0);
	blue  = (float) (rhs.blue/65535.0);

#elif WIN
	red   = (float) (rhs.rgbRed/255.0);
	green = (float) (rhs.rgbGreen/255.0);
	blue  = (float) (rhs.rgbBlue/255.0);
#endif
}


//---------------------------------------------------------------
//
// XARGBColor::XARGBColor (COLORREF)
//
//---------------------------------------------------------------
#if WIN
XARGBColor::XARGBColor(COLORREF rhs)
{
	alpha = 1.0f;
	
	red   = (float) (GetRValue(rhs)/255.0);
	green = (float) (GetGValue(rhs)/255.0);
	blue  = (float) (GetBValue(rhs)/255.0);
}
#endif


//---------------------------------------------------------------
//
// XARGBColor::XARGBColor (XRGBColor)
//
//---------------------------------------------------------------
XARGBColor::XARGBColor(const XRGBColor& rhs)
{
	PRECONDITION(rhs.red >= 0.0 && rhs.red <= 1.0);
	PRECONDITION(rhs.green >= 0.0 && rhs.green <= 1.0);
	PRECONDITION(rhs.blue >= 0.0 && rhs.blue <= 1.0);
	
	alpha = 1.0f;
	
	red   = rhs.red;
	green = rhs.green;
	blue  = rhs.blue;
}


//---------------------------------------------------------------
//
// XARGBColor::XARGBColor (XHSVColor)
//
//---------------------------------------------------------------
XARGBColor::XARGBColor(const XHSVColor& rhs)
{
	XRGBColor color(rhs);
	
	*this = color;
}


//---------------------------------------------------------------
//
// XARGBColor::XARGBColor (double, double, double, double)
//
//---------------------------------------------------------------
XARGBColor::XARGBColor(double theRed, double theGreen, double theBlue, double theAlpha)
{
	PRECONDITION(theAlpha >= 0.0 && theAlpha <= 1.0);
	PRECONDITION(theRed >= 0.0 && theRed <= 1.0);
	PRECONDITION(theGreen >= 0.0 && theGreen <= 1.0);
	PRECONDITION(theBlue >= 0.0 && theBlue <= 1.0);
	
	alpha = (float) theAlpha;
	red   = (float) theRed;
	green = (float) theGreen;
	blue  = (float) theBlue;
}


//---------------------------------------------------------------
//
// XARGBColor::operator+=
//
//---------------------------------------------------------------
XARGBColor& XARGBColor::operator+=(const XARGBColor& rhs)
{
	alpha = this->DoAddComponents(alpha, rhs.alpha);
	red   = this->DoAddComponents(red, rhs.red);
	green = this->DoAddComponents(green, rhs.green);
	blue  = this->DoAddComponents(blue, rhs.blue);

	return *this;
}


//---------------------------------------------------------------
//
// XARGBColor::operator-=
//
//---------------------------------------------------------------
XARGBColor& XARGBColor::operator-=(const XARGBColor& rhs)
{
	alpha = this->DoSubtractComponents(alpha, rhs.alpha);
	red   = this->DoSubtractComponents(red, rhs.red);
	green = this->DoSubtractComponents(green, rhs.green);
	blue  = this->DoSubtractComponents(blue, rhs.blue);

	return *this;
}


//---------------------------------------------------------------
//
// XARGBColor::operator*=
//
//---------------------------------------------------------------
XARGBColor& XARGBColor::operator*=(double rhs)
{	
	alpha = this->DoMultiplyComponents(alpha, rhs);
	red   = this->DoMultiplyComponents(red, rhs);
	green = this->DoMultiplyComponents(green, rhs);
	blue  = this->DoMultiplyComponents(blue, rhs);

	return *this;
}


//---------------------------------------------------------------
//
// XARGBColor::operator/= 
//
//---------------------------------------------------------------
XARGBColor& XARGBColor::operator/=(double rhs)
{	
	alpha = this->DoDivideComponents(alpha, rhs);
	red   = this->DoDivideComponents(red, rhs);
	green = this->DoDivideComponents(green, rhs);
	blue  = this->DoDivideComponents(blue, rhs);

	return *this;
}


//---------------------------------------------------------------
//
// XARGBColor::operator==
//
//---------------------------------------------------------------
bool XARGBColor::operator==(const XARGBColor& rhs) const				
{
	bool equal = false;
	
	if (Equal(alpha, rhs.alpha, kColorTolerance))
		if (Equal(red, rhs.red, kColorTolerance))
			if (Equal(green, rhs.green, kColorTolerance))
				if (Equal(blue, rhs.blue, kColorTolerance))
					equal = true;

	return equal;
}


//---------------------------------------------------------------
//
// XARGBColor::GetLumenance
//
//---------------------------------------------------------------
double XARGBColor::GetLumenance() const
{
	// calculate approximate brightness using YIQ formula where
	// Y = lumenance = .30R + .59G + .11B
	return alpha*(0.3*red + 0.59*green + 0.11*blue);
}
			
#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XARGBColor::DoAddComponents
//
//---------------------------------------------------------------
float XARGBColor::DoAddComponents(double a, double b)
{
	PRECONDITION(a >= 0.0 && a <= 1.0);
	PRECONDITION(b >= 0.0 && b <= 1.0);
	
	double c = MinMax(0.0, a + b, 1.0);
	
	return (float) c;
}


//---------------------------------------------------------------
//
// XARGBColor::DoMultiplyComponents
//
//---------------------------------------------------------------
float XARGBColor::DoMultiplyComponents(double a, double b)
{
	PRECONDITION(a >= 0.0 && a <= 1.0);
	PRECONDITION(b >= 0.0 && b <= 1.0);
	
	double c = MinMax(0.0, a*b, 1.0);
	
	return (float) c;
}


}	// namespace Whisper


