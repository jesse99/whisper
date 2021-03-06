/*
 *  File:       X3DLight.cpp
 *  Summary:	Base class for QuickDraw 3D lights.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DLight.cpp,v $
 *		Revision 1.4  2001/04/21 03:36:06  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:29:15  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:16:39  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DLight.h>

#include <QuesaLight.h>

#include <X3DUtils.h>
#include <XDebug.h>

namespace Whisper {


// ===================================================================================
//	class X3DLight
// ===================================================================================

//---------------------------------------------------------------
//
//	X3DLight::~X3DLight
//
//---------------------------------------------------------------
X3DLight::~X3DLight()
{
}


//---------------------------------------------------------------
//
//	X3DLight::X3DLight
//
//---------------------------------------------------------------
X3DLight::X3DLight(TQ3LightObject object) : X3DShape(object)
{
	this->ConfirmType(kQ3ShapeTypeLight);
}


//---------------------------------------------------------------
//
// X3DLight::Clone
//
//---------------------------------------------------------------
X3DLight* X3DLight::Clone() const
{
	X3DLight* object = new X3DLight(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
//	X3DLight::GetState
//
//---------------------------------------------------------------
bool X3DLight::GetState() const
{
	TQ3Boolean on;
	
	TQ3Status status = Q3Light_GetState(mObject, &on);
	ThrowIf3DError(status);
	
	return on;
}


//---------------------------------------------------------------
//
//	X3DLight::SetState
//
//---------------------------------------------------------------
void X3DLight::SetState(bool inOn)
{
	TQ3Boolean on = (TQ3Boolean) inOn;
	
	TQ3Status status = Q3Light_SetState(mObject, on);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
//	X3DLight::GetBrightness
//
//---------------------------------------------------------------
float X3DLight::GetBrightness() const
{
	float brightness;
	
	TQ3Status status = Q3Light_GetBrightness(mObject, &brightness);
	ThrowIf3DError(status);
	
	return brightness;
}


//---------------------------------------------------------------
//
//	X3DLight::SetBrightness
//
//---------------------------------------------------------------
void X3DLight::SetBrightness(float brightness)
{
	PRECONDITION(brightness >= -10.0);	// check for half-way reasonable values
	PRECONDITION(brightness <= 10.0);
	
	TQ3Status status = Q3Light_SetBrightness(mObject, brightness);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
//	X3DLight::GetColor
//
//---------------------------------------------------------------
X3DColorRGB X3DLight::GetColor() const
{
	TQ3ColorRGB color;
	
	TQ3Status status = Q3Light_GetColor(mObject, &color);
	ThrowIf3DError(status);
	
	return color;
}


//---------------------------------------------------------------
//
//	X3DLight::SetColor
//
//---------------------------------------------------------------
void X3DLight::SetColor(const X3DColorRGB& color)
{
	TQ3Status status = Q3Light_SetColor(mObject, &color);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DLight::GetData
//
//---------------------------------------------------------------
TQ3LightData X3DLight::GetData() const
{
	TQ3LightData data;
	
	TQ3Status status = Q3Light_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DLight::SetData (X3DColorRGB, float, bool)
//
//---------------------------------------------------------------
void X3DLight::SetData(const X3DColorRGB& color, float brightness, bool on)
{
	PRECONDITION(brightness >= -10.0);	// check for half-way reasonable values
	PRECONDITION(brightness <= 10.0);
	
	TQ3LightData data;
	
	data.isOn       = (TQ3Boolean) on;
	data.brightness = brightness;
	data.color      = color;

	TQ3Status status = Q3Light_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DLight::SetData (TQ3LightData)
//
//---------------------------------------------------------------
void X3DLight::SetData(const TQ3LightData& data)
{
	TQ3Status status = Q3Light_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DLight::IsCompatible 								[static]
//	
//---------------------------------------------------------------
bool X3DLight::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3ShapeTypeLight);
	
	return compatible;
}


}	// namespace Whisper
