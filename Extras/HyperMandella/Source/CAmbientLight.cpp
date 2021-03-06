/*
 *  File:       CAmbientLight.cpp
 *  Summary:   	Light that is added equally to all surfaces.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CAmbientLight.cpp,v $
 *		Revision 1.1  2000/12/28 02:48:03  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.4  2000/12/17 03:41:52  jesjones
 *		Uses XRGBColor instead of X3DRGBColor.
 *		
 *		Revision 1.3  2000/12/05 08:54:16  jesjones
 *		Added a Clone method.
 *		
 *		Revision 1.2  2000/11/09 08:52:16  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: CAmbientLight.cpp,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.1  2000/12/28 02:48:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Moved from the Whisper to the Extras module.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.4  2000/12/17 03:41:52  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Uses XRGBColor instead of X3DRGBColor.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/05 08:54:16  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Added a Clone method.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 7/23/00	JDJ		Created
 */

#include "AppHeader.h"
#include "ILight.h"


// ===================================================================================
//	class CAmbientLight
// ===================================================================================
class CAmbientLight : public ILight {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CAmbientLight();
	
						CAmbientLight(XBoss* boss);
						
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

//-----------------------------------
//	Member Data
//
private:
	bool		mIsOn;
	float		mBrightness;
	XRGBColor	mColor;
};

DEFINE_INTERFACE_FACTORY(CAmbientLight)

//---------------------------------------------------------------
//
// CAmbientLight::~CAmbientLight
//
//---------------------------------------------------------------
CAmbientLight::~CAmbientLight()
{
}


//---------------------------------------------------------------
//
// CAmbientLight::CAmbientLight
//
//---------------------------------------------------------------
CAmbientLight::CAmbientLight(XBoss* boss) 
{	
	ILight::DoSetBoss(boss);

	mIsOn = true;
	mBrightness = 0.5f;
	mColor = kRGBWhite;
}


//---------------------------------------------------------------
//
// CAmbientLight::Clone
//
// $$ May fail if new interfaces are added...
//
//---------------------------------------------------------------
ILightPtr CAmbientLight::Clone() const
{
	ILightPtr light(L"Ambient Light");
	
	light->TurnOn(mIsOn);
	light->SetBrightness(mBrightness);
	light->SetColor(mColor);
	
	return light;
}


//---------------------------------------------------------------
//
// CAmbientLight::GetIntensity
//
//---------------------------------------------------------------
XRGBColor CAmbientLight::GetIntensity(const X3DPoint& pt) const
{
	UNUSED(pt);
	
	XRGBColor intensity = kRGBBlack;
	
	if (mIsOn) 
		intensity = mBrightness*mColor;
		
	return intensity;
}


