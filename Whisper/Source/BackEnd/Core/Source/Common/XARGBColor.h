/*
 *  File:       XARGBColor.h
 *  Summary:	Alpha weighted RGB color class (the components should be in [0, 1]).
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XARGBColor.h,v $
 *		Revision 1.6  2001/07/13 22:09:36  jesjones
 *		Ctor now uses a default argument for alpha.
 *		
 *		Revision 1.5  2001/04/21 03:39:28  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/03/02 11:24:08  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/12/10 08:50:04  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:25:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#if MAC
	#include <ColorPicker.h>
	#include <QuickDraw.h>
#endif

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XHSVColor;
class XInStream;
class XRGBColor;
class XOutStream;


// ===================================================================================
//	class XARGBColor
//!		Floating point RGB color class that includes an alpha channel (1.0 os opaque).
// ===================================================================================
class CORE_EXPORT XARGBColor {

//-----------------------------------
//	Initialization/Destruction
//
public:
						XARGBColor()										{}
						
						XARGBColor(const OSColor& rhs);
												
						XARGBColor(const XRGBColor& rhs);
						XARGBColor(const XHSVColor& rhs);

						XARGBColor(double red, double green, double blue, double alpha = 1.0);
						/**< Values should be in [0.0, 1.0]. If alpha is 0.0 the color
						is transparent. If alpha is 1.0 the color is opaque. */
						
#if WIN
						XARGBColor(COLORREF rhs);
#endif

//-----------------------------------
//	API
//
public:
	//! @name Assignment Operators
	//@{
			XARGBColor& operator+=(const XARGBColor& rhs);
			XARGBColor& operator-=(const XARGBColor& rhs);

			XARGBColor& operator*=(double rhs);
			XARGBColor& operator/=(double rhs);
	//@}
	
	//! @name Comparison Operators
	//@{
			bool 		operator==(const XARGBColor& rhs) const;
			bool 		operator!=(const XARGBColor& rhs) const				{return !this->operator==(rhs);}
	//@}
	
	//! @name Arithmetic
	//@{
			XARGBColor 	operator+(const XARGBColor& rhs) const				{XARGBColor temp = *this; temp += rhs; return temp;}
			XARGBColor 	operator-(const XARGBColor& rhs) const				{XARGBColor temp = *this; temp -= rhs; return temp;}
	
			XARGBColor 	operator*(double rhs) const							{XARGBColor temp = *this; temp *= rhs; return temp;}
			XARGBColor 	operator/(double rhs) const							{XARGBColor temp = *this; temp /= rhs; return temp;}
			
	friend	XARGBColor 	operator*(double lhs, const XARGBColor& rhs)		{XARGBColor temp = rhs; temp *= lhs; return temp;}
	//@}

	//! @name Lumenance
	//@{
			double		GetLumenance() const;
						/**< Returns a number from 0.0 (black) to 1.0 (pure white)
						corresponding to the lumenance of the color. */
						
			bool		IsDarkerThan(const XARGBColor& rhs) const			{return this->GetLumenance() < rhs.GetLumenance();}
						
			bool		IsLighterThan(const XARGBColor& rhs) const			{return this->GetLumenance() > rhs.GetLumenance();}
	//@}
			
	//! @name Streaming
	//@{
	friend 	XInStream& 	operator>>(XInStream& stream, XARGBColor& value);
	friend 	XOutStream& operator<<(XOutStream& stream, const XARGBColor& value);
			
	friend 	void 			FromStr(const std::wstring& str, XARGBColor& value);
	friend 	std::wstring	ToStr(const XARGBColor& value);
	//@}

//-----------------------------------
//	Internal API
//
protected:
			float 		DoAddComponents(double a, double b);
			float 		DoSubtractComponents(double a, double b)			{return (float) (a <= b ? 0 : a - b);}
						// These are used to pin the components to 0 and 1.

			float 		DoMultiplyComponents(double a, double b);
			float 		DoDivideComponents(double a, double b)				{return (float) (b == 0 ? 1.0 : a/b);}

//-----------------------------------
//	Member Data
//
public:
	float	red;
	float	green;
	float	blue;
	float	alpha;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper


