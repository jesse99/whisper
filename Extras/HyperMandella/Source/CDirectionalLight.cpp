/*
 *  File:       CDirectionalLight.cpp
 *  Summary:   	Light that casts parallel rays of light in a specific direction.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CDirectionalLight.cpp,v $
 *		Revision 1.1  2000/12/28 02:48:53  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.4  2000/12/17 03:41:56  jesjones
 *		Uses XRGBColor instead of X3DRGBColor.
 *		
 *		Revision 1.3  2000/12/05 08:54:20  jesjones
 *		Added a Clone method.
 *		
 *		Revision 1.2  2000/11/09 08:54:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: CDirectionalLight.cpp,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.1  2000/12/28 02:48:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Moved from the Whisper to the Extras module.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.4  2000/12/17 03:41:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Uses XRGBColor instead of X3DRGBColor.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/05 08:54:20  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Added a Clone method.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 7/23/00	JDJ		Created
 */

#include "AppHeader.h"
#include "IDirectionalLight.h"
#include "ILight.h"
#include "IReflectedLight.h"


// ===================================================================================
//	class CDirectionalLight
// ===================================================================================
class CDirectionalLight : public ILight, public IReflectedLight, public IDirectionalLight {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CDirectionalLight();
	
						CDirectionalLight(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual ILightPtr 	Clone() const;

	virtual bool 		IsOn() const								{return mIsOn;}
	virtual void 		TurnOn(bool on = true)						{mIsOn = on;}

	virtual float 		GetBrightness() const						{return mBrightness;}
	virtual void 		SetBrightness(float brightness)				{mBrightness = brightness;}

	virtual XRGBColor 	GetColor() const							{return mColor;}
	virtual void 		SetColor(const XRGBColor& color)			{mColor = color;}
						
	virtual XRGBColor 	GetIntensity(const X3DPoint& pt) const;

	virtual X3DVector 	GetLightVector(const X3DPoint&) const		{return -mDirection;}

	virtual X3DVector 	GetDirection() const						{return mDirection;}
	virtual void 		SetDirection(const X3DVector& dir)			{mDirection = Normalize(dir);}

//-----------------------------------
//	Member Data
//
private:
	bool		mIsOn;
	float		mBrightness;
	XRGBColor	mColor;

	X3DVector	mDirection;
};

DEFINE_INTERFACE_FACTORY(CDirectionalLight)

//---------------------------------------------------------------
//
// CDirectionalLight::~CDirectionalLight
//
//---------------------------------------------------------------
CDirectionalLight::~CDirectionalLight()
{
}


//---------------------------------------------------------------
//
// CDirectionalLight::CDirectionalLight
//
//---------------------------------------------------------------
CDirectionalLight::CDirectionalLight(XBoss* boss) 
{	
	ILight::DoSetBoss(boss);
	IReflectedLight::DoSetBoss(boss);
	IDirectionalLight::DoSetBoss(boss);

	mIsOn = true;
	mBrightness = 1.0f;
	mColor = kRGBWhite;
	
	mDirection = X3DVector(0.0f, 0.0f, -1.0f);
}


//---------------------------------------------------------------
//
// CDirectionalLight::Clone
//
// $$ May fail if new interfaces are added...
//
//---------------------------------------------------------------
ILightPtr CDirectionalLight::Clone() const
{
	ILightPtr light(L"Directional Light");
	
	light->TurnOn(mIsOn);
	light->SetBrightness(mBrightness);
	light->SetColor(mColor);
	
	IDirectionalLightPtr directional(light);
	directional->SetDirection(mDirection);
	
	return light;
}


//---------------------------------------------------------------
//
// CDirectionalLight::GetIntensity
//
//---------------------------------------------------------------
XRGBColor CDirectionalLight::GetIntensity(const X3DPoint& pt) const
{
	UNUSED(pt);
	
	XRGBColor intensity = kRGBBlack;
	
	if (mIsOn) 
		intensity = mBrightness*mColor;
		
	return intensity;
}


