/*
 *  File:       XRGBColor.h
 *  Summary:	RGB color class (the components should normally be in [0, 1]).
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XRGBColor.h,v $
 *		Revision 1.6  2001/03/15 09:03:41  jesjones
 *		Exported the friend functions.
 *		
 *		Revision 1.5  2001/03/02 11:25:15  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.4  2000/12/25 00:32:21  jesjones
 *		XRGBColor color values are no longer clamped to [0.0, 1.0]. Added Normalize.
 *		
 *		Revision 1.3  2000/12/10 08:55:14  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:50:00  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#if MAC
	#include <Appearance.h>
	#include <ColorPicker.h>
	#include <QuickDraw.h>
#endif

#include <XCallbacks.h>
#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XARGBColor;
class XHSVColor;
class XInStream;
class XOutStream;
class XRGBColor;


// ===================================================================================
//	Constants
// ===================================================================================
extern CORE_EXPORT const XRGBColor kRGBBlack;
extern CORE_EXPORT const XRGBColor kRGBWhite;
extern CORE_EXPORT const XRGBColor kRGBRed;
extern CORE_EXPORT const XRGBColor kRGBYellow;
extern CORE_EXPORT const XRGBColor kRGBBlue;
extern CORE_EXPORT const XRGBColor kRGBGreen;
extern CORE_EXPORT const XRGBColor kRGBBrown;
extern CORE_EXPORT const XRGBColor kRGBPurple;
extern CORE_EXPORT const XRGBColor kRGBLightBlue;
extern CORE_EXPORT const XRGBColor kRGBGray;
extern CORE_EXPORT const XRGBColor kRGBLightGray;
extern CORE_EXPORT const XRGBColor kRGBDarkGray;


// ===================================================================================
//	class XRGBColor
//!		RGB color class (the components should be in [0, 1]).
// ===================================================================================
class CORE_EXPORT XRGBColor {

//-----------------------------------
//	Initialization/Destruction
//
public:
						XRGBColor()												{}
						
						XRGBColor(const OSColor& rhs);
												
						XRGBColor(double red, double green, double blue);
						/**< Values should be in [0.0, 1.0] */
						
						XRGBColor(const XHSVColor& rhs);
	explicit			XRGBColor(const XARGBColor& rhs);

#if WIN
	explicit			XRGBColor(COLORREF rhs);
#elif MAC
	explicit			XRGBColor(ThemeBrush brush);
#endif

						XRGBColor(const XRGBColor& rhs);

//-----------------------------------
//	API
//
public:
	//! @name Conversion Operators
	//@{
			OSColor 	GetOSColor() const;
						
#if WIN
			COLORREF 	GetColorRef() const;
#endif
	//@}

	//! @name Assignment Operators
	//@{
			XRGBColor& 	operator+=(const XRGBColor& rhs);
			XRGBColor& 	operator-=(const XRGBColor& rhs);

			XRGBColor& 	operator*=(double rhs);
			XRGBColor& 	operator/=(double rhs);
	//@}
	
	//! @name Comparison Operators
	//@{
			bool 		operator==(const XRGBColor& rhs) const;
			bool 		operator!=(const XRGBColor& rhs) const				{return !this->operator==(rhs);}
	//@}
	
	//! @name Arithmetic
	//@{
			XRGBColor 	operator+(const XRGBColor& rhs) const				{XRGBColor temp = *this; temp += rhs; return temp;}
			XRGBColor 	operator-(const XRGBColor& rhs) const				{XRGBColor temp = *this; temp -= rhs; return temp;}
	
			XRGBColor 	operator*(double rhs) const							{XRGBColor temp = *this; temp *= rhs; return temp;}
			XRGBColor 	operator/(double rhs) const							{XRGBColor temp = *this; temp /= rhs; return temp;}
			
	friend	XRGBColor 	operator*(double lhs, const XRGBColor& rhs)			{XRGBColor temp = rhs; temp *= lhs; return temp;}
	//@}

	//! @name Lumenance
	//@{
			double		GetLumenance() const;
						/**< Returns a number from 0.0 (black) to 1.0 (pure white)
						corresponding to the lumenance of the color. */
						
			bool		IsDarkerThan(const XRGBColor& rhs) const			{return this->GetLumenance() < rhs.GetLumenance();}
						
			bool		IsLighterThan(const XRGBColor& rhs) const			{return this->GetLumenance() > rhs.GetLumenance();}
	//@}
			
	//! @name Streaming
	//@{
	friend 	CORE_EXPORT XInStream& 	operator>>(XInStream& stream, XRGBColor& value);
	friend 	CORE_EXPORT XOutStream& operator<<(XOutStream& stream, const XRGBColor& value);
	
	friend 	CORE_EXPORT void 			FromStr(const std::wstring& str, XRGBColor& value);
	friend 	CORE_EXPORT std::wstring	ToStr(const XRGBColor& value);
	//@}

	//! @name Misc
	//@{
	friend 	CORE_EXPORT XRGBColor 	Normalize(const XRGBColor& color);
						/**< Clamp the values to [0.0, 1.0]. */
						
	static	bool 		Pick(const std::wstring& prompt, XRGBColor* color, XCallback1<void, XRGBColor> callback = XCallback1<void, XRGBColor>());
						/**< Pops up a dialog allowing the user to pick a new color.
						Returns true if the user selected a color. Callback
						will be called (on some OS's) as the user selects new
						colors. */
	//@}

//-----------------------------------
//	Member Data
//
public:
	float	red;
	float	green;
	float	blue;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper


