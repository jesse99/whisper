/*
 *  File:       XHSVColor.h
 *  Summary:	HSVColor wrapper
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XHSVColor.h,v $
 *		Revision 1.4  2001/03/02 11:24:33  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/12/10 08:53:09  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:38:20  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XARGBColor;
class XRGBColor;


// ===================================================================================
//	class XHSVColor
//!		Color class that uses hue, saturation, and value.
// ===================================================================================
class CORE_EXPORT XHSVColor {

//-----------------------------------
//	Initialization/Destruction
//
public:
						XHSVColor()													{}
																		
						XHSVColor(double hue, double saturation, double balue);
						/**< Values should be in [0.0, 1.0] */

						XHSVColor(const XRGBColor& rhs);
	explicit			XHSVColor(const XARGBColor& rhs);
						
#if MAC
						XHSVColor(const HSVColor& rhs);		
#endif

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
	
	//! @name Comparison Operators
	//@{
			bool 		operator==(const XHSVColor& rhs) const;
			bool 		operator!=(const XHSVColor& rhs) const						{return !this->operator==(rhs);}
	//@}
	
	//! @name Lumenance
	//@{
			double		GetLumenance() const;
						/**< Returns a number from 0.0 (black) to 1.0 (pure white)
						corresponding to the lumenance of the color. */
						
			bool		IsDarkerThan(const XHSVColor& rhs) const					{return this->GetLumenance() < rhs.GetLumenance();}
						
			bool		IsLighterThan(const XHSVColor& rhs) const					{return this->GetLumenance() > rhs.GetLumenance();}
	//@}
			
	//! @name Streaming
	//@{
	friend 	void 			FromStr(const std::wstring& str, XHSVColor& value);
	friend 	std::wstring	ToStr(const XHSVColor& value);
	//@}

//-----------------------------------
//	Member Data
//
public:
	float	hue;
	float	saturation;
	float	value;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
