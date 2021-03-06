/*
 *  File:       X3DLights.h
 *  Summary:	QuickDraw 3D light objects.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DAmbientLight		- A light that is added to all surfaces in a scene 
 *								 	  (there can only be one of these in a scene).
 *				X3DDirectionalLight	- A light that emits parallel rays of light in a 
 *									  specific direction.
 *				X3DPointLight		- A light that emits rays of light in all directions 
 *									  from a specific location.
 *				X3DSpotLight		- A light that emits a circular cone of light in a 
 *									  specific direction from a specific location.
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DLights.h,v $
 *		Revision 1.4  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/13 08:52:16  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:16:54  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <->	  2/10/97	JDJ		Created.
 */

#pragma once

#include <X3DLight.h>
#include <X3DPrimitives.h>
#include <X3DVectors.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DAmbientLight
//!		A light that is added to all surfaces in a scene (there can only be one of these in a scene).
// ===================================================================================
class QUESA_EXPORT X3DAmbientLight : public X3DLight {

	typedef X3DLight Inherited;

public:
	virtual				~X3DAmbientLight();
						
						X3DAmbientLight(const X3DColorRGB& color, float brightness, 
										bool on = true);

						X3DAmbientLight(const TQ3LightData& data);
						
						X3DAmbientLight(TQ3LightObject object);

	virtual X3DAmbientLight* Clone() const;
		
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DAmbientLight. */
};


// ===================================================================================
//	class X3DDirectionalLight
//!		A light that emits parallel rays of light in a specific direction.
// ===================================================================================
class QUESA_EXPORT X3DDirectionalLight : public X3DLight {

	typedef X3DLight Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DDirectionalLight();
						
						X3DDirectionalLight(const X3DColorRGB& color, float brightness, 
											const X3DVector& direction, bool castsShadows = kDontUseShadows, 
											bool on = true);

						X3DDirectionalLight(const TQ3DirectionalLightData& data);

						X3DDirectionalLight(TQ3LightObject object);

	virtual X3DDirectionalLight* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Direction
	//@{
			X3DVector 	GetDirection() const;
						
			void 		SetDirection(const X3DVector& direction);
	//@}
			
	//! @name Shadows
	//@{
			bool 		IsCastingShadows() const;
						
			void 		SetCastingShadows(bool on = true);
	//@}
			
	//! @name Data
	//@{
			TQ3DirectionalLightData GetData() const;

			void 		SetData(const TQ3DirectionalLightData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DDirectionalLight. */
	//@}
};


// ===================================================================================
//	class X3DPointLight
//!		A light that emits rays of light in all directions from a specific location.
// ===================================================================================
class QUESA_EXPORT X3DPointLight : public X3DLight {

	typedef X3DLight Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DPointLight();
						
						X3DPointLight(const X3DColorRGB& color, float brightness, 
									  const X3DPoint& location, bool castsShadows = kDontUseShadows, 
									  TQ3AttenuationType attenuation = kQ3AttenuationTypeNone,
				   					  bool on = true);

						X3DPointLight(const TQ3PointLightData& data);

						X3DPointLight(TQ3LightObject object);

	virtual X3DPointLight* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Location
	//@{
			X3DPoint 	GetLocation() const;
						
			void 		SetLocation(const X3DPoint& location);
	//@}
			
	//! @name Shadows
	//@{
			bool 		IsCastingShadows() const;
						
			void 		SetCastingShadows(bool on = true);
	//@}
			
	//! @name Attenuation
	//@{
			TQ3AttenuationType GetAttenuation() const;
						
			void 		SetAttenuation(TQ3AttenuationType attenuation);
	//@}
			
	//! @name Data
	//@{
			TQ3PointLightData GetData() const;

			void 		SetData(const TQ3PointLightData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DPointLight. */
	//@}
};


// ===================================================================================
//	class X3DSpotLight
//!		A light that emits a circular cone of light in a specific direction from a specific location.
// ===================================================================================
class QUESA_EXPORT X3DSpotLight : public X3DLight {

	typedef X3DLight Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DSpotLight();
						
						X3DSpotLight(const TQ3SpotLightData& data);

						X3DSpotLight(TQ3LightObject object);

	virtual X3DSpotLight* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Location
	//@{
			X3DPoint 	GetLocation() const;
						
			void 		SetLocation(const X3DPoint& location);
	//@}
			
	//! @name Direction
	//@{
			X3DVector 	GetDirection() const;
						
			void 		SetDirection(const X3DVector& direction);
	//@}
			
	//! @name Hot Angle
	//@{
			float 		GetHotAngle() const;
						/**< This is the angle (in radians) from the center of the cone
						to the surface at which the light falls below full intensity. */
						
			void 		SetHotAngle(float angle);
	//@}
			
	//! @name Outer Angle
	//@{
			float 		GetOuterAngle() const;
						/**< This is the angle (in radians) from the center of the cone
						to the surface at which the light falls to zero intensity. */
						
			void 		SetOuterAngle(float angle);
	//@}
			
	//! @name Fall Off
	//@{
			TQ3FallOffType GetFallOff() const;
						/**< This controls the rate at which the light's intensity
						decreases in the region between the hot angle and the
						outer angle. */
						
			void 		SetFallOfff(TQ3FallOffType fallOff);
	//@}
			
	//! @name Shadows
	//@{
			bool 		IsCastingShadows() const;
						
			void 		SetCastingShadows(bool on = true);
	//@}
			
	//! @name Attenuation
	//@{
			TQ3AttenuationType GetAttenuation() const;
						
			void 		SetAttenuation(TQ3AttenuationType attenuation);
	//@}
			
	//! @name Data
	//@{
			TQ3SpotLightData GetData() const;

			void 		SetData(const TQ3SpotLightData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DSpotLight. */
	//@}
};



#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
