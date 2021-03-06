/*
 *  File:       CPointLight.cpp
 *  Summary:   	Light that emits rays in all directions from a specified point.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CPointLight.cpp,v $
 *		Revision 1.1  2000/12/28 02:50:32  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.4  2000/12/17 03:42:08  jesjones
 *		Uses XRGBColor instead of X3DRGBColor.
 *		
 *		Revision 1.3  2000/12/05 08:54:24  jesjones
 *		Added a Clone method.
 *		
 *		Revision 1.2  2000/11/09 08:56:59  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: CPointLight.cpp,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.1  2000/12/28 02:50:32  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Moved from the Whisper to the Extras module.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.4  2000/12/17 03:42:08  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Uses XRGBColor instead of X3DRGBColor.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/05 08:54:24  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Added a Clone method.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 7/23/00	JDJ		Created
 */

#include "AppHeader.h"
#include "ILight.h"
#include "IPointLight.h"
#include "IReflectedLight.h"


// ===================================================================================
//	class CPointLight
// ===================================================================================
class CPointLight : public ILight, public IReflectedLight, public IPointLight {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CPointLight();
	
						CPointLight(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual ILightPtr 	Clone() const;

	virtual bool 		IsOn() const									{return mIsOn;}
	virtual void 		TurnOn(bool on = true)							{mIsOn = on;}

	virtual float 		GetBrightness() const							{return mBrightness;}
	virtual void 		SetBrightness(float brightness)					{mBrightness = brightness;}

	virtual XRGBColor 	GetColor() const								{return mColor;}
	virtual void 		SetColor(const XRGBColor& color)				{mColor = color;}
						
	virtual XRGBColor 	GetIntensity(const X3DPoint& pt) const;

	virtual X3DVector 	GetLightVector(const X3DPoint& pt) const		{return Normalize(X3DVector(mLocation - pt));}

	virtual X3DPoint 	GetLocation() const								{return mLocation;}
	virtual void 		SetLocation(const X3DPoint& loc)				{mLocation = loc;}
						
	virtual ELightAttenuation GetAttenuation() const					{return mAttenuation;}
	virtual void 		SeAttenuation(ELightAttenuation attenuation)	{mAttenuation = attenuation;}

//-----------------------------------
//	Member Data
//
private:
	bool				mIsOn;
	float				mBrightness;
	XRGBColor			mColor;

	X3DPoint			mLocation;
	ELightAttenuation	mAttenuation;
};

DEFINE_INTERFACE_FACTORY(CPointLight)

//---------------------------------------------------------------
//
// CPointLight::~CPointLight
//
//---------------------------------------------------------------
CPointLight::~CPointLight()
{
}


//---------------------------------------------------------------
//
// CPointLight::CPointLight
//
//---------------------------------------------------------------
CPointLight::CPointLight(XBoss* boss) 
{	
	ILight::DoSetBoss(boss);
	IPointLight::DoSetBoss(boss);
	IReflectedLight::DoSetBoss(boss);

	mIsOn = true;
	mBrightness = 1.0f;
	mColor = kRGBWhite;
	
	mLocation = kZero3DPt;
	mAttenuation = kNoAttenuation;
}


//---------------------------------------------------------------
//
// CPointLight::Clone
//
// $$ May fail if new interfaces are added...
//
//---------------------------------------------------------------
ILightPtr CPointLight::Clone() const
{
	ILightPtr light(L"Point Light");
	
	light->TurnOn(mIsOn);
	light->SetBrightness(mBrightness);
	light->SetColor(mColor);
	
	IPointLightPtr point(light);
	point->SetLocation(mLocation);
	point->SeAttenuation(mAttenuation);
	
	return light;
}


//---------------------------------------------------------------
//
// CPointLight::GetIntensity
//
//---------------------------------------------------------------
XRGBColor CPointLight::GetIntensity(const X3DPoint& pt) const
{
	XRGBColor intensity = kRGBBlack;
	
	if (mIsOn) {
		intensity = mBrightness*mColor;

		if (mAttenuation != kNoAttenuation) {
			if (mAttenuation == kInverseDistanceAttenuation)
				intensity /= Distance(pt, mLocation);

			else if (mAttenuation == kInverseDistanceSqrAttenuation)
				intensity /= DistanceSquared(pt, mLocation);
				
			else
				DEBUGSTR("Bogus attenuation in CPointLight::GetIntensity");
		}
	}
	
	return intensity;
}


