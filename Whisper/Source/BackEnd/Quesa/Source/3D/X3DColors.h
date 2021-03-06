/*
 *  File:       X3DColors.h
 *  Summary:	QuickDraw 3D color classes.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DColorRGB	 - Encapsulates TQ3ColorRGB.
 *				X3DColorARGB - Encapsulates TQ3ColorARGB.
 *
 *  Copyright � 1997-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DColors.h,v $
 *		Revision 1.5  2001/04/14 07:27:53  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.4  2001/03/02 11:23:26  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/12/13 08:50:51  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:14:29  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	  8/06/00	JDJ		Fixed ctors for Windows.
 *		 <4>	  1/31/98	JDJ		X3DColorRGB scalar operators use doubles instead of floats.
 *		 <3>	  1/30/98	JDJ		Added streaming operators.
 *		 <2>	 10/17/97	JDJ		Removed operator+ and - methods that took scalars.
 *									Added a operator*(uint16, TRGBColor) method.
 *		 <1>	  2/10/97	JDJ		Created.
 */

#pragma once

#include <Quesa.h>

#include <XARGBColor.h>
#include <XRGBColor.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XInStream;
class XOutStream;
	
	
// ===================================================================================
//	class X3DColorRGB
//!		Quesa RGB color class.
// ===================================================================================
class QUESA_EXPORT X3DColorRGB : public TQ3ColorRGB {

	typedef TQ3ColorRGB Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DColorRGB()									{}

#if MAC
						X3DColorRGB(const RGBColor& rhs)				{r = rhs.red/65535.0f; g = rhs.green/65535.0f; b = rhs.blue/65535.0f;}
						
						X3DColorRGB(const HSVColor& rhs);
#elif WIN
						X3DColorRGB(const RGBQUAD& rhs)					{r = rhs.rgbRed/255.0f; g = rhs.rgbGreen/255.0f; b = rhs.rgbBlue/255.0f;}
						
	explicit			X3DColorRGB(COLORREF rhs)						{r = GetRValue(rhs)/255.0f; g = GetGValue(rhs)/255.0f; b = GetBValue(rhs)/255.0f;}
#endif
						
						X3DColorRGB(double red, double green, double blue);

						X3DColorRGB(const TQ3ColorRGB& rhs)				{r = rhs.r; g = rhs.g; b = rhs.b;}

						X3DColorRGB(const XRGBColor& rhs)				{r = rhs.red; g = rhs.green; b = rhs.blue;}

//-----------------------------------
//	API
//
public:
	//! @name Comparison operators
	//@{
			bool 		operator==(const X3DColorRGB& color) const;
	
			bool 		operator!=(const X3DColorRGB& color) const		{return !(*this == color);}
	//@}
	
	//! @name Arithmetic, by another color
	//@{
			X3DColorRGB operator+(const X3DColorRGB& color) const;
			X3DColorRGB operator-(const X3DColorRGB& color) const;
		
			X3DColorRGB& operator+=(const X3DColorRGB& color);
			X3DColorRGB& operator-=(const X3DColorRGB& color);
	//@}
	
	//! @name Arithmetic by a scalar
	//@{
			X3DColorRGB operator*(double scalar) const;
			X3DColorRGB operator/(double scalar) const;
			
	friend	X3DColorRGB operator*(double scalar, const X3DColorRGB& rhs)	{return rhs*scalar;}

			X3DColorRGB& operator*=(double scalar);
			X3DColorRGB& operator/=(double scalar);
	//@}

	//! @name Lumenance
	//@{
			double		GetLumenance() const;
						/**< Returns a number from 0.0 (black) to 1.0 (pure white)
						corresponding to the lumenance of the color. */
						
			bool		IsDarkerThan(const X3DColorRGB& rhs) const		{return this->GetLumenance() < rhs.GetLumenance();}
			bool		IsLighterThan(const X3DColorRGB& rhs) const		{return this->GetLumenance() > rhs.GetLumenance();}
	//@}
			
	//! @name Streaming
	//@{
	friend 	XInStream& 	operator>>(XInStream& stream, X3DColorRGB& rhs);
	friend	XOutStream& operator<<(XOutStream& stream, const X3DColorRGB& rhs);
	
	friend 	void 			FromStr(const std::wstring& str, X3DColorRGB& value);
	friend 	std::wstring	ToStr(const X3DColorRGB& value);
	//@}

//-----------------------------------
//	Internal API
//
protected:
			float 		AddComponents(float a, float b);
			float 		SubtractComponents(float a, float b);

			float 		MultiplyComponents(float a, float b);
			float 		DivideComponents(float a, float b);
};


// ===================================================================================
//	class X3DColorARGB
//!		Quesa RGB color with an alpha channel.
// ===================================================================================
class QUESA_EXPORT X3DColorARGB : public TQ3ColorARGB {

	typedef TQ3ColorARGB Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
						X3DColorARGB()									{}
						
#if MAC
						X3DColorARGB(const RGBColor& rhs)				{r = rhs.red/65535.0f; g = rhs.green/65535.0f; b = rhs.blue/65535.0f; a = 1.0f;}
						
						X3DColorARGB(const HSVColor& rhs);
#elif WIN
						X3DColorARGB(const RGBQUAD& rhs)				{r = rhs.rgbRed/255.0f; g = rhs.rgbGreen/255.0f; b = rhs.rgbBlue/255.0f; a = 1.0f;}
						
	explicit			X3DColorARGB(COLORREF rhs)						{r = GetRValue(rhs)/255.0f; g = GetGValue(rhs)/255.0f; b = GetBValue(rhs)/255.0f; a = 1.0f;}
#endif
						
						X3DColorARGB(double red, double green, double blue, double alpha = 1.0);

						X3DColorARGB(const TQ3ColorRGB& rhs)			{r = rhs.r; g = rhs.g; b = rhs.b; a = 1.0f;}

						X3DColorARGB(const TQ3ColorARGB& rhs)			{r = rhs.r; g = rhs.g; b = rhs.b; a = rhs.a;}

						X3DColorARGB(const XRGBColor& rhs)				{r = rhs.red; g = rhs.green; b = rhs.blue; a = 1.0f;}

						X3DColorARGB(const XARGBColor& rhs)				{r = rhs.red; g = rhs.green; b = rhs.blue; a = rhs.alpha;}

//-----------------------------------
//	API
//
public:
	//! @name Comparison operators
	//@{
			bool 		operator==(const X3DColorARGB& color) const;
	
			bool 		operator!=(const X3DColorARGB& color) const		{return !(*this == color);}
	//@}
	
	//! @name Lumenance
	//@{
			double		GetLumenance() const;
						/**< Returns a number from 0.0 (black) to 1.0 (pure white)
						corresponding to the lumenance of the color. */
						
			bool		IsDarkerThan(const X3DColorARGB& rhs) const		{return this->GetLumenance() < rhs.GetLumenance();}
			bool		IsLighterThan(const X3DColorARGB& rhs) const	{return this->GetLumenance() > rhs.GetLumenance();}
	//@}

	//! @name Streaming
	//@{
	friend 	XInStream& 	operator>>(XInStream& stream, X3DColorARGB& rhs);
	friend	XOutStream& operator<<(XOutStream& stream, const X3DColorARGB& rhs);
	
	friend 	void 			FromStr(const std::wstring& str, X3DColorARGB& value);
	friend 	std::wstring	ToStr(const X3DColorARGB& value);
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

