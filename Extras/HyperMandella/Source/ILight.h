/*
 *  File:       ILight.h
 *  Summary:   	Main interface for light sources.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ILight.h,v $
 *		Revision 1.2  2001/03/25 06:08:20  jesjones
 *		Added a comment.
 *		
 *		Revision 1.1  2000/12/28 02:51:20  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.4  2000/12/17 03:42:20  jesjones
 *		Uses XRGBColor instead of X3DRGBColor.
 *		
 *		Revision 1.3  2000/12/05 09:05:35  jesjones
 *		Added a Clone method.
 *		
 *		Revision 1.2  2000/11/09 09:03:15  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <X3DPrimitives.h>
#include <XInterfacePtr.h>
#include <XRGBColor.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


// ===================================================================================
//	class ILight
//!		Main interface for light sources.
// ===================================================================================
class ILight : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Clone -----
	virtual XInterfacePtr<ILight> Clone() const = 0;

	// ----- On/Off -----
	virtual bool 		IsOn() const = 0;
						/**< Defaults to on. */
			
	virtual void 		TurnOn(bool on = true) = 0;

	// ----- Brightness -----
	virtual float 		GetBrightness() const = 0;
						/**< This will typically be between 0 and 1, but can be greater
						then one if you want to overdrive the light or less than zero
						for shadow lights. It defaults to 1.0. */

	virtual void 		SetBrightness(float brightness) = 0;

	// ----- Color -----
	virtual XRGBColor 	GetColor() const = 0;
						/**< Defaults to kRGBWhite. */
			
	virtual void 		SetColor(const XRGBColor& color) = 0;
						
	// ----- Intensity -----
	virtual XRGBColor 	GetIntensity(const X3DPoint& pt) const = 0;
						/**< Returns the intensity of the light at the specified pt for
						each color component. For special effects the intensities
						may be outside 0.0 to 1.0. Note that this doesn't account
						for intervening objects or incidence angle. */
};

typedef XInterfacePtr<ILight> ILightPtr;
typedef XInterfacePtr<const ILight> IConstLightPtr;

