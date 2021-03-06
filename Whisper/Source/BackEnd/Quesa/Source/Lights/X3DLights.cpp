/*
 *  File:       X3DLights.cpp
 *  Summary:	QuickDraw 3D light objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DLights.cpp,v $
 *		Revision 1.4  2001/04/21 03:36:12  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:29:23  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:16:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DLights.h>

#include <QuesaMath.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateAmbientLight (TQ3Object)
//
//---------------------------------------------------------------
static X3DObject* CreateAmbientLight(TQ3Object inObject)
{
	X3DAmbientLight* object = new X3DAmbientLight(inObject);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateAmbientLight (X3DColorRGB, float, bool)
//
//---------------------------------------------------------------
static TQ3LightObject CreateAmbientLight(const X3DColorRGB& color, float brightness, bool on)
{
	PRECONDITION(brightness >= -10.0);	// check for half-way reasonable values
	PRECONDITION(brightness <= 10.0);
	
	TQ3LightData data;
	
	data.isOn       = (TQ3Boolean) on;
	data.brightness = brightness;
	data.color      = color;

	TQ3LightObject object = Q3AmbientLight_New(&data);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateDirectionalLight (TQ3Object)
//
//---------------------------------------------------------------
static X3DObject* CreateDirectionalLight(TQ3Object inObject)
{
	X3DDirectionalLight* object = new X3DDirectionalLight(inObject);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateDirectionalLight (X3DColorRGB, float, X3DVector, bool, bool)
//
//---------------------------------------------------------------
static TQ3LightObject CreateDirectionalLight(const X3DColorRGB& color, float brightness, 
										 const X3DVector& direction, bool castsShadows, 
										 bool on)
{
	PRECONDITION(brightness >= -10.0);	// check for half-way reasonable values
	PRECONDITION(brightness <= 10.0);
	
	TQ3DirectionalLightData data;
	
	data.lightData.isOn       = (TQ3Boolean) on;
	data.lightData.brightness = brightness;
	data.lightData.color      = color;

	data.direction    = direction;
	data.castsShadows = (TQ3Boolean) castsShadows;

	TQ3LightObject object = Q3DirectionalLight_New(&data);
	
	return object;
}


//---------------------------------------------------------------
//
// CreatePointLight (TQ3Object)
//
//---------------------------------------------------------------
static X3DObject* CreatePointLight(TQ3Object inObject)
{
	X3DPointLight* object = new X3DPointLight(inObject);
	
	return object;
}


//---------------------------------------------------------------
//
// CreatePointLight (X3DColorRGB&, float, X3DPoint, bool, TQ3AttenuationType, bool)
//
//---------------------------------------------------------------
static TQ3LightObject CreatePointLight(const X3DColorRGB& color, float brightness, 
							 const X3DPoint& location, bool castsShadows, 
							 TQ3AttenuationType attenuation, bool on)
{
	PRECONDITION(brightness >= -10.0);	// check for half-way reasonable values
	PRECONDITION(brightness <= 10.0);
	
	TQ3PointLightData data;

	data.lightData.isOn       = (TQ3Boolean) on;
	data.lightData.brightness = brightness;
	data.lightData.color      = color;

	data.castsShadows = (TQ3Boolean) castsShadows;
	data.attenuation  = attenuation;
	data.location     = location;

	TQ3LightObject object = Q3PointLight_New(&data);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateSpotLight
//
//---------------------------------------------------------------
static X3DObject* CreateSpotLight(TQ3Object inObject)
{
	X3DSpotLight* object = new X3DSpotLight(inObject);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DAmbientLight
// ===================================================================================

//---------------------------------------------------------------
//
// X3DAmbientLight::~X3DAmbientLight
//
//---------------------------------------------------------------
X3DAmbientLight::~X3DAmbientLight()
{
}


//---------------------------------------------------------------
//
// X3DAmbientLight::X3DAmbientLight (X3DColorRGB, float, bool)
//
//---------------------------------------------------------------
X3DAmbientLight::X3DAmbientLight(const X3DColorRGB& color, float brightness, bool on) : X3DLight(CreateAmbientLight(color, brightness, on))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DAmbientLight::X3DAmbientLight (TQ3LightData)
//
//---------------------------------------------------------------
X3DAmbientLight::X3DAmbientLight(const TQ3LightData& data) : X3DLight(Q3AmbientLight_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DAmbientLight::X3DAmbientLight (TQ3LightObject)
//
//---------------------------------------------------------------
X3DAmbientLight::X3DAmbientLight(TQ3LightObject object) : X3DLight(object)
{
	this->ConfirmType(kQ3LightTypeAmbient);
}


//---------------------------------------------------------------
//
// X3DAmbientLight::Clone
//
//---------------------------------------------------------------
X3DAmbientLight* X3DAmbientLight::Clone() const
{
	X3DAmbientLight* object = new X3DAmbientLight(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DAmbientLight::IsCompatible 						[static]
//	
//---------------------------------------------------------------
bool X3DAmbientLight::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3LightTypeAmbient);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DDirectionalLight
// ===================================================================================

//---------------------------------------------------------------
//
// X3DDirectionalLight::~X3DDirectionalLight
//
//---------------------------------------------------------------
X3DDirectionalLight::~X3DDirectionalLight()
{
}


//---------------------------------------------------------------
//
// X3DDirectionalLight::X3DDirectionalLight (X3DColorRGB, float, X3DVector, bool, bool)
//
//---------------------------------------------------------------
X3DDirectionalLight::X3DDirectionalLight(const X3DColorRGB& color, float brightness, 
										 const X3DVector& direction, bool castsShadows, 
										 bool on) : X3DLight(CreateDirectionalLight(color, brightness, direction, castsShadows, on))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDirectionalLight::X3DDirectionalLight (TQ3DirectionalLightData)
//
//---------------------------------------------------------------
X3DDirectionalLight::X3DDirectionalLight(const TQ3DirectionalLightData& data) : X3DLight(Q3DirectionalLight_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDirectionalLight::X3DDirectionalLight (TQ3LightObject)
//
//---------------------------------------------------------------
X3DDirectionalLight::X3DDirectionalLight(TQ3LightObject object) : X3DLight(object)
{
	this->ConfirmType(kQ3LightTypeDirectional);
}


//---------------------------------------------------------------
//
// X3DDirectionalLight::Clone
//
//---------------------------------------------------------------
X3DDirectionalLight* X3DDirectionalLight::Clone() const
{
	X3DDirectionalLight* object = new X3DDirectionalLight(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DDirectionalLight::IsCastingShadows
//
//---------------------------------------------------------------
bool X3DDirectionalLight::IsCastingShadows() const
{
	TQ3Boolean shadows;
	
	TQ3Status status = Q3DirectionalLight_GetCastShadowsState(mObject, &shadows);
	ThrowIf3DError(status);
	
	return shadows;
}


//---------------------------------------------------------------
//
// X3DDirectionalLight::SetCastingShadows
//
//---------------------------------------------------------------
void X3DDirectionalLight::SetCastingShadows(bool on)
{
	TQ3Boolean shadows = (TQ3Boolean) on;

	TQ3Status status = Q3DirectionalLight_SetCastShadowsState(mObject, shadows);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDirectionalLight::GetDirection
//
//---------------------------------------------------------------
X3DVector X3DDirectionalLight::GetDirection() const
{
	X3DVector direction;
	
	TQ3Status status = Q3DirectionalLight_GetDirection(mObject, direction);
	ThrowIf3DError(status);
	
	return direction;
}


//---------------------------------------------------------------
//
// X3DDirectionalLight::SetDirection
//
//---------------------------------------------------------------
void X3DDirectionalLight::SetDirection(const X3DVector& direction)
{
	TQ3Status status = Q3DirectionalLight_SetDirection(mObject, direction);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDirectionalLight::GetData
//
//---------------------------------------------------------------
TQ3DirectionalLightData X3DDirectionalLight::GetData() const
{	
	TQ3DirectionalLightData data;
	
	TQ3Status status = Q3DirectionalLight_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DDirectionalLight::SetData
//
//---------------------------------------------------------------
void X3DDirectionalLight::SetData(const TQ3DirectionalLightData& data)
{
	TQ3Status status = Q3DirectionalLight_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDirectionalLight::IsCompatible 					[static]
//	
//---------------------------------------------------------------
bool X3DDirectionalLight::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3LightTypeDirectional);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DPointLight
// ===================================================================================

//---------------------------------------------------------------
//
// X3DPointLight::~X3DPointLight
//
//---------------------------------------------------------------
X3DPointLight::~X3DPointLight()
{
}


//---------------------------------------------------------------
//
// X3DPointLight::X3DPointLight (X3DColorRGB&, float, X3DPoint, bool, TQ3AttenuationType, bool)
//
//---------------------------------------------------------------
X3DPointLight::X3DPointLight(const X3DColorRGB& color, float brightness, 
							 const X3DPoint& location, bool castsShadows, 
							 TQ3AttenuationType attenuation, bool on) : X3DLight(CreatePointLight(color, brightness, location,  castsShadows, attenuation, on))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPointLight::X3DPointLight (TQ3PointLightData)
//
//---------------------------------------------------------------
X3DPointLight::X3DPointLight(const TQ3PointLightData& data) : X3DLight(Q3PointLight_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPointLight::X3DPointLight (TQ3LightObject)
//
//---------------------------------------------------------------
X3DPointLight::X3DPointLight(TQ3LightObject object) : X3DLight(object)
{
	this->ConfirmType(kQ3LightTypePoint);
}


//---------------------------------------------------------------
//
// X3DPointLight::Clone
//
//---------------------------------------------------------------
X3DPointLight* X3DPointLight::Clone() const
{
	X3DPointLight* object = new X3DPointLight(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DPointLight::GetLocation
//
//---------------------------------------------------------------
X3DPoint X3DPointLight::GetLocation() const
{
	X3DPoint location;
	
	TQ3Status status = Q3PointLight_GetLocation(mObject, location);
	ThrowIf3DError(status);
	
	return location;
}


//---------------------------------------------------------------
//
// X3DPointLight::SetLocation
//
//---------------------------------------------------------------
void X3DPointLight::SetLocation(const X3DPoint& location)
{
	TQ3Status status = Q3PointLight_SetLocation(mObject, location);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPointLight::IsCastingShadows
//
//---------------------------------------------------------------
bool X3DPointLight::IsCastingShadows() const
{
	TQ3Boolean shadows;
	
	TQ3Status status = Q3PointLight_GetCastShadowsState(mObject, &shadows);
	ThrowIf3DError(status);
	
	return shadows;
}


//---------------------------------------------------------------
//
// X3DPointLight::SetCastingShadows
//
//---------------------------------------------------------------
void X3DPointLight::SetCastingShadows(bool on)
{
	TQ3Boolean shadows = (TQ3Boolean) on;

	TQ3Status status = Q3PointLight_SetCastShadowsState(mObject, shadows);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPointLight::GetAttenuation
//
//---------------------------------------------------------------
TQ3AttenuationType X3DPointLight::GetAttenuation() const
{
	TQ3AttenuationType attenuation;
	
	TQ3Status status = Q3PointLight_GetAttenuation(mObject, &attenuation);
	ThrowIf3DError(status);
	
	return attenuation;
}


//---------------------------------------------------------------
//
// X3DPointLight::SetAttenuation
//
//---------------------------------------------------------------
void X3DPointLight::SetAttenuation(TQ3AttenuationType attenuation)
{
	TQ3Status status = Q3PointLight_SetAttenuation(mObject, attenuation);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPointLight::GetData
//
//---------------------------------------------------------------
TQ3PointLightData X3DPointLight::GetData() const
{	
	TQ3PointLightData data;
	
	TQ3Status status = Q3PointLight_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DPointLight::SetData
//
//---------------------------------------------------------------
void X3DPointLight::SetData(const TQ3PointLightData& data)
{
	TQ3Status status = Q3PointLight_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPointLight::IsCompatible 							[static]
//	
//---------------------------------------------------------------
bool X3DPointLight::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3LightTypePoint);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DSpotLight
// ===================================================================================

//---------------------------------------------------------------
//
// X3DSpotLight::~X3DSpotLight
//
//---------------------------------------------------------------
X3DSpotLight::~X3DSpotLight()
{
}


//---------------------------------------------------------------
//
// X3DSpotLight::X3DSpotLight (TQ3SpotLightData)
//
//---------------------------------------------------------------
X3DSpotLight::X3DSpotLight(const TQ3SpotLightData& data) : X3DLight(Q3SpotLight_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSpotLight::X3DSpotLight (TQ3LightObject)
//
//---------------------------------------------------------------
X3DSpotLight::X3DSpotLight(TQ3LightObject object) : X3DLight(object)
{
	this->ConfirmType(kQ3LightTypeSpot);
}


//---------------------------------------------------------------
//
// X3DSpotLight::Clone
//
//---------------------------------------------------------------
X3DSpotLight* X3DSpotLight::Clone() const
{
	X3DSpotLight* object = new X3DSpotLight(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DSpotLight::GetLocation
//
//---------------------------------------------------------------
X3DPoint X3DSpotLight::GetLocation() const
{
	X3DPoint location;
	
	TQ3Status status = Q3SpotLight_GetLocation(mObject, location);
	ThrowIf3DError(status);
	
	return location;
}


//---------------------------------------------------------------
//
// X3DSpotLight::SetLocation
//
//---------------------------------------------------------------
void X3DSpotLight::SetLocation(const X3DPoint& location)
{
	TQ3Status status = Q3SpotLight_SetLocation(mObject, location);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSpotLight::GetDirection
//
//---------------------------------------------------------------
X3DVector X3DSpotLight::GetDirection() const
{
	X3DVector direction;
	
	TQ3Status status = Q3SpotLight_GetDirection(mObject, direction);
	ThrowIf3DError(status);
	
	return direction;
}


//---------------------------------------------------------------
//
// X3DSpotLight::SetDirection
//
//---------------------------------------------------------------
void X3DSpotLight::SetDirection(const X3DVector& direction)
{
	TQ3Status status = Q3SpotLight_SetDirection(mObject, direction);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSpotLight::GetHotAngle
//
//---------------------------------------------------------------
float X3DSpotLight::GetHotAngle() const
{
	float angle;
	
	TQ3Status status = Q3SpotLight_GetHotAngle(mObject, &angle);
	ThrowIf3DError(status);
	
	return angle;
}


//---------------------------------------------------------------
//
// X3DSpotLight::SetHotAngle
//
//---------------------------------------------------------------
void X3DSpotLight::SetHotAngle(float angle)
{
	PRECONDITION(angle >= 0.0);
	PRECONDITION(angle <= kQ3PiOver2);

	TQ3Status status = Q3SpotLight_SetHotAngle(mObject, angle);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSpotLight::GetOuterAngle
//
//---------------------------------------------------------------
float X3DSpotLight::GetOuterAngle() const
{
	float angle;
	
	TQ3Status status = Q3SpotLight_GetOuterAngle(mObject, &angle);
	ThrowIf3DError(status);
	
	return angle;
}


//---------------------------------------------------------------
//
// X3DSpotLight::SetOuterAngle
//
//---------------------------------------------------------------
void X3DSpotLight::SetOuterAngle(float angle)
{
	PRECONDITION(angle >= 0.0);
	PRECONDITION(angle <= kQ3PiOver2);

	TQ3Status status = Q3SpotLight_SetOuterAngle(mObject, angle);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSpotLight::GetFallOff
//
//---------------------------------------------------------------
TQ3FallOffType X3DSpotLight::GetFallOff() const
{
	TQ3FallOffType fallOff;
	
	TQ3Status status = Q3SpotLight_GetFallOff(mObject, &fallOff);
	ThrowIf3DError(status);
	
	return fallOff;
}


//---------------------------------------------------------------
//
// X3DSpotLight::SetFallOfff
//
//---------------------------------------------------------------
void X3DSpotLight::SetFallOfff(TQ3FallOffType fallOff)
{
	TQ3Status status = Q3SpotLight_SetFallOff(mObject, fallOff);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSpotLight::IsCastingShadows
//
//---------------------------------------------------------------
bool X3DSpotLight::IsCastingShadows() const
{
	TQ3Boolean shadows;
	
	TQ3Status status = Q3SpotLight_GetCastShadowsState(mObject, &shadows);
	ThrowIf3DError(status);
	
	return shadows;
}


//---------------------------------------------------------------
//
// X3DSpotLight::SetCastingShadows
//
//---------------------------------------------------------------
void X3DSpotLight::SetCastingShadows(bool on)
{
	TQ3Boolean shadows = (TQ3Boolean) on;

	TQ3Status status = Q3SpotLight_SetCastShadowsState(mObject, shadows);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSpotLight::GetAttenuation
//
//---------------------------------------------------------------
TQ3AttenuationType X3DSpotLight::GetAttenuation() const
{
	TQ3AttenuationType attenuation;
	
	TQ3Status status = Q3SpotLight_GetAttenuation(mObject, &attenuation);
	ThrowIf3DError(status);
	
	return attenuation;
}


//---------------------------------------------------------------
//
// X3DSpotLight::SetAttenuation
//
//---------------------------------------------------------------
void X3DSpotLight::SetAttenuation(TQ3AttenuationType attenuation)
{
	TQ3Status status = Q3SpotLight_SetAttenuation(mObject, attenuation);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSpotLight::GetData
//
//---------------------------------------------------------------
TQ3SpotLightData X3DSpotLight::GetData() const
{	
	TQ3SpotLightData data;
	
	TQ3Status status = Q3SpotLight_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DSpotLight::SetData
//
//---------------------------------------------------------------
void X3DSpotLight::SetData(const TQ3SpotLightData& data)
{
	TQ3Status status = Q3SpotLight_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSpotLight::IsCompatible 							[static]
//	
//---------------------------------------------------------------
bool X3DSpotLight::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3LightTypeSpot);
	
	return compatible;
}


}	// namespace Whisper
