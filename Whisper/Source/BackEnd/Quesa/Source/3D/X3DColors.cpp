/*
 *  File:       X3DColors.cpp
 *  Summary:	QuickDraw 3D color classes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DColors.cpp,v $
 *		Revision 1.6  2001/04/21 03:35:20  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/03/02 11:23:22  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.4  2000/12/10 04:08:42  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.3  2000/11/27 07:17:32  jesjones
 *		Tweaked for Pro 6.
 *		
 *		Revision 1.2  2000/11/09 12:14:24  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	  2/01/98	JDJ		X3DColorRGB(HSVColor) no longer uses toolbox.
 *		 <4>	  1/31/98	JDJ		X3DColorRGB scalar operators use doubles instead of floats.
 *		 <3>	  1/30/98	JDJ		Added streaming operators.
 *		 <2>	 10/17/97	JDJ		Removed operator+ and - methods that took scalars.
 *		 <1>	  2/10/97	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <X3DColors.h>

#include <XDebug.h>
#include <XMiscUtils.h>
#include <XNumbers.h>
#include <XStream.h>
#include <XTextConversions.h>

namespace Whisper {


// ===================================================================================
//	class X3DColorRGB
// ===================================================================================

//---------------------------------------------------------------
//
// Constructors
//
//---------------------------------------------------------------
X3DColorRGB::X3DColorRGB(double red, double green, double blue)
{
	PRECONDITION(red >= 0.0 && red <= 1.0);
	PRECONDITION(green >= 0.0 && green <= 1.0);
	PRECONDITION(blue >= 0.0 && blue <= 1.0);
	
	r = (float) red;
	g = (float) green;
	b = (float) blue;
}

#if MAC
X3DColorRGB::X3DColorRGB(const HSVColor& inColor)
{
#if 1
	float h = 6.0f*inColor.hue/65535.0f;
	float s = inColor.saturation/65535.0f;
	float v = inColor.value/65535.0f; 

	int32 i = numeric_cast<int32>(std::floor(h)); 
	float f = h - i; 
	if (!(i & 1)) 
		f = 1 - f; // if i is even 

	float m = v * (1.0f - s); 
	float n = v * (1.0f - s * f); 
	
	switch (i) { 
		case 6: 
		case 0: r = v; g = n; b = m; break;
		case 1: r = n; g = v; b = m; break;
		case 2: r = m; g = v; b = n; break;
		case 3: r = m; g = n; b = v; break;
		case 4: r = n; g = m; b = v; break;
		case 5: r = v; g = m; b = n; break;
	} 

#else
	RGBColor color;
	
	HSV2RGB(&inColor, &color);
	r = color.red/65535.0f; 
	g = color.green/65535.0f; 
	b = color.blue/65535.0f;
#endif
}
#endif


//---------------------------------------------------------------
//
// Comparison operators
//
//---------------------------------------------------------------
bool X3DColorRGB::operator==(const X3DColorRGB& color) const
{
	const double tolerance = 1.0/65536.0;
	
	return Equal(r, color.r, tolerance) && Equal(g, color.g, tolerance) && Equal(b, color.b, tolerance);
}
		
		
//---------------------------------------------------------------
//
// Arithmetic, by another color
//
//---------------------------------------------------------------
X3DColorRGB X3DColorRGB::operator+(const X3DColorRGB& rhs) const
{
	X3DColorRGB temp = *this;
	
	return temp += rhs;
}

X3DColorRGB X3DColorRGB::operator-(const X3DColorRGB& rhs) const
{
	X3DColorRGB temp = *this;
	
	return temp -= rhs;
}

X3DColorRGB& X3DColorRGB::operator+=(const X3DColorRGB& rhs)
{
	r = this->AddComponents(r, rhs.r);
	g = this->AddComponents(g, rhs.g);
	b = this->AddComponents(b, rhs.b);

	return *this;
}

X3DColorRGB& X3DColorRGB::operator-=(const X3DColorRGB& rhs)
{
	r = this->SubtractComponents(r, rhs.r);
	g = this->SubtractComponents(g, rhs.g);
	b = this->SubtractComponents(b, rhs.b);

	return *this;
}
	

//---------------------------------------------------------------
//
// Arithmetic by a scalar
//
//---------------------------------------------------------------
X3DColorRGB X3DColorRGB::operator*(double rhs) const
{
	X3DColorRGB temp = *this;
	
	return temp *= rhs;
}

X3DColorRGB X3DColorRGB::operator/(double rhs) const
{
	X3DColorRGB temp = *this;
	
	return temp /= rhs;
}

	
X3DColorRGB& X3DColorRGB::operator*=(double rhs)
{
	r = this->MultiplyComponents(r, (float) rhs);
	g = this->MultiplyComponents(g, (float) rhs);
	b = this->MultiplyComponents(b, (float) rhs);

	return *this;
}

X3DColorRGB& X3DColorRGB::operator/=(double rhs)
{
	r = this->DivideComponents(r, (float) rhs);
	g = this->DivideComponents(g, (float) rhs);
	b = this->DivideComponents(b, (float) rhs);

	return *this;
}


//---------------------------------------------------------------
//
// Lumenance
//
//---------------------------------------------------------------
double X3DColorRGB::GetLumenance() const
{
	// Calculate approximate brightness using YIQ formula.
	return 0.3008*r + 0.5899*g + 0.1094*b;
}
			

//---------------------------------------------------------------
//
// Helpers
//
//---------------------------------------------------------------
float X3DColorRGB::AddComponents(float a, float b)
{
	a += b;

	if (a > 1.0)
		a = 1.0f;
	
	return a;
}

float X3DColorRGB::SubtractComponents(float a, float b)
{
	return a <= b ? 0 : a - b;
}

float X3DColorRGB::MultiplyComponents(float a, float b)
{
	a *= b;

	if (a > 1.0)
		a = 1.0f;

	return a;
}

float X3DColorRGB::DivideComponents(float a, float b)
{
	return b == 0.0 ? 1.0f : a/b;
}


//---------------------------------------------------------------
//
// operator>> (X3DColorRGB)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, X3DColorRGB& rhs)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == '3RGB');
				
	stream.ReadBytes(&rhs.r, sizeof(rhs.r));
	stream.ReadBytes(&rhs.g, sizeof(rhs.g));
	stream.ReadBytes(&rhs.b, sizeof(rhs.b));
	
	if (stream.NeedsByteSwap()) {
		ByteSwap(rhs.r);
		ByteSwap(rhs.g);
		ByteSwap(rhs.b);
	}
	
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (X3DColorRGB)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const X3DColorRGB& rhs)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('3RGB');
#endif

	stream.WriteBytes(&rhs.r, sizeof(rhs.r));
	stream.WriteBytes(&rhs.g, sizeof(rhs.g));
	stream.WriteBytes(&rhs.b, sizeof(rhs.b));

	return stream;
}


//---------------------------------------------------------------
//
// FromStr (wstring, X3DColorRGB)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& inStr, X3DColorRGB& value)
{
	FromStr(inStr, value.r);
	
	std::wstring str = After(inStr, L" ");
	FromStr(str, value.g);
	
	str = After(str, L" ");
	FromStr(str, value.b);
}


//---------------------------------------------------------------
//
// ToStr (X3DColorRGB)
//
//---------------------------------------------------------------
std::wstring ToStr(const X3DColorRGB& value)
{
	return ToStr(value.r, 3) + L" " + ToStr(value.g, 3) + L" " + ToStr(value.b, 3);
}

#pragma mark -

// ===================================================================================
//	class X3DColorARGB
// ===================================================================================

//---------------------------------------------------------------
//
// Constructors
//
//---------------------------------------------------------------
X3DColorARGB::X3DColorARGB(double red, double green, double blue, double alpha)
{
	PRECONDITION(red >= 0.0 && red <= 1.0);
	PRECONDITION(green >= 0.0 && green <= 1.0);
	PRECONDITION(blue >= 0.0 && blue <= 1.0);
	PRECONDITION(alpha >= 0.0 && alpha <= 1.0);
	
	r = (float) red;
	g = (float) green;
	b = (float) blue;
	a = (float) alpha;
}

#if MAC
X3DColorARGB::X3DColorARGB(const HSVColor& inColor)
{
	RGBColor color;
	
	HSV2RGB(&inColor, &color);
	r = color.red/65535.0f; 
	g = color.green/65535.0f; 
	b = color.blue/65535.0f;
	a = 1.0f;
}
#endif


//---------------------------------------------------------------
//
// Comparison operators
//
//---------------------------------------------------------------
bool X3DColorARGB::operator==(const X3DColorARGB& color) const
{
	const double tolerance = 1.0/65536.0;
	
	return Equal(r, color.r, tolerance) && Equal(g, color.g, tolerance) && Equal(b, color.b, tolerance) && Equal(a, color.a, tolerance);
}
		
		
//---------------------------------------------------------------
//
// Lumenance
//
//---------------------------------------------------------------
double X3DColorARGB::GetLumenance() const
{
	// Calculate approximate brightness using YIQ formula.
	return a*(0.3008*r + 0.5899*g + 0.1094*b);
}
			

//---------------------------------------------------------------
//
// operator>> (X3DColorARGB)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, X3DColorARGB& rhs)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == '3AGB');
				
	stream.ReadBytes(&rhs.a, sizeof(rhs.a));
	stream.ReadBytes(&rhs.r, sizeof(rhs.r));
	stream.ReadBytes(&rhs.g, sizeof(rhs.g));
	stream.ReadBytes(&rhs.b, sizeof(rhs.b));
	
	if (stream.NeedsByteSwap()) {
		ByteSwap(rhs.a);
		ByteSwap(rhs.r);
		ByteSwap(rhs.g);
		ByteSwap(rhs.b);
	}
	
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (X3DColorARGB)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const X3DColorARGB& rhs)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('3AGB');
#endif

	stream.WriteBytes(&rhs.a, sizeof(rhs.a));
	stream.WriteBytes(&rhs.r, sizeof(rhs.r));
	stream.WriteBytes(&rhs.g, sizeof(rhs.g));
	stream.WriteBytes(&rhs.b, sizeof(rhs.b));

	return stream;
}


//---------------------------------------------------------------
//
// FromStr (wstring, X3DColorARGB)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& inStr, X3DColorARGB& value)
{
	FromStr(inStr, value.a);
	
	std::wstring str = After(inStr, L" ");
	FromStr(str, value.r);
	
	str = After(str, L" ");
	FromStr(str, value.g);
	
	str = After(str, L" ");
	FromStr(str, value.b);
}


//---------------------------------------------------------------
//
// ToStr (X3DColorARGB)
//
//---------------------------------------------------------------
std::wstring ToStr(const X3DColorARGB& value)
{
	return ToStr(value.a, 3) + L" " +  ToStr(value.r, 3) + L" " + ToStr(value.g, 3) + L" " + ToStr(value.b, 3);
}


}	// namespace Whisper
