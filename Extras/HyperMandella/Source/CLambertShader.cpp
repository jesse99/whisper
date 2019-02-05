/*
 *  File:       CLambertShader.cpp
 *  Summary:   	A shader that only uses ambient and diffuse shading.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CLambertShader.cpp,v $
 *		Revision 1.2  2001/04/21 02:24:22  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.1  2000/12/28 02:50:00  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.3  2000/12/17 03:42:01  jesjones
 *		Uses XRGBColor instead of X3DRGBColor.
 *		
 *		Revision 1.2  2000/11/09 08:55:28  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include "IShader.h"

#include <IHierarchy.h>
#include <XNumbers.h>

#include "IDocInfo.h"
#include "IFractalFunction.h"
#include "ILight.h"
#include "IReflectedLight.h"


// ===================================================================================
//	class CLambertShader
// ===================================================================================
class CLambertShader : public IShader {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CLambertShader();
	
						CLambertShader(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual XRGBColor 	GetColor(const IConstDocInfoPtr& info, const XRGBColor& diffuseColor, const X3DPoint& pt, 
								 const X3DVector& normalV, const X3DVector& viewV) const;
};

DEFINE_INTERFACE_FACTORY(CLambertShader)

//---------------------------------------------------------------
//
// CLambertShader::~CLambertShader
//
//---------------------------------------------------------------
CLambertShader::~CLambertShader()
{
}


//---------------------------------------------------------------
//
// CLambertShader::CLambertShader
//
//---------------------------------------------------------------
CLambertShader::CLambertShader(XBoss* boss) 
{	
	IShader::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CLambertShader::GetColor
//
//---------------------------------------------------------------
XRGBColor CLambertShader::GetColor(const IConstDocInfoPtr& doc, const XRGBColor& diffuseColor, const X3DPoint& pt, const X3DVector& normalV, const X3DVector& viewV) const
{
	PRECONDITION(Equal(normalV.LengthSquared(), 1.0));
	UNUSED(viewV);
		
	double ir = 0.0;	
	double ig = 0.0;	
	double ib = 0.0;	

	const SShaderInfo& info = doc->GetShaderInfo();
	
	// Process the ambient light
	IConstLightPtr ambient = doc->GetAmbientLight();

	XRGBColor intensity;
	if (ambient->IsOn()) {
		intensity = ambient->GetIntensity(pt);
		
		ir += info.ambientCoeff*intensity.red;	
		ig += info.ambientCoeff*intensity.green;
		ib += info.ambientCoeff*intensity.blue;
	}
	
	// Process the point and directional lights
	uint32 numLights = doc->GetNumLights();	
	for (uint32 index = 0; index < numLights; ++index) {
		IConstLightPtr light = doc->GetLight(index);
		
		if (light->IsOn()) {
			XRGBColor intensity = light->GetIntensity(pt);
			
			IConstReflectedLightPtr reflected(light);
			X3DVector lightV = reflected->GetLightVector(pt);
			double dp = DotProduct(normalV, lightV);
			if (dp > 0.0) {
				ir += dp*intensity.red;
				ig += dp*intensity.green;
				ib += dp*intensity.blue;
			}
		}
	}
	
	XRGBColor color;
	color.red   = (float) (info.diffuseCoeff*ir*diffuseColor.red);
	color.green = (float) (info.diffuseCoeff*ig*diffuseColor.green);
	color.blue  = (float) (info.diffuseCoeff*ib*diffuseColor.blue);
	
	return color;
}


