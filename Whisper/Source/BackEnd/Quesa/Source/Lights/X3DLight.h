/*
 *  File:       X3DLight.h
 *  Summary:	Base class for QuickDraw 3D lights.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DLight.h,v $
 *		Revision 1.4  2001/04/14 07:29:18  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.3  2000/12/13 08:52:11  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:16:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Quesa.h>
#include <QuesaLight.h>

#include <X3DColors.h>
#include <X3DShape.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Constants
//
const bool kUseShadows     = true;
const bool kDontUseShadows = false;


// ===================================================================================
//	class X3DLight
//!		Base class for Quesa lights.
// ===================================================================================
class QUESA_EXPORT X3DLight : public X3DShape {

	typedef X3DShape Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DLight();
						
						X3DLight(TQ3LightObject object);
						
	virtual X3DLight* 	Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3LightObject() const						{return mObject;}
	//@}

	//! @name On/Off
	//@{
			bool 		GetState() const;
						/**< Returns true if the light is on. */
						
			void 		SetState(bool on = true);
	//@}
			
	//! @name Brightness
	//@{
			float 		GetBrightness() const;
						/**< Normally in [0.0, 1.0], but some renderers may support overbright
						lights or negative brightness. */
						
			void 		SetBrightness(float brightness);
	//@}
			
	//! @name Color
	//@{
			X3DColorRGB GetColor() const;
			
			void 		SetColor(const X3DColorRGB& color);
	//@}

	//! @name Data
	//@{
			TQ3LightData GetData() const;

			void 		SetData(const X3DColorRGB& color, float brightness, bool on = true);

			void 		SetData(const TQ3LightData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DLight. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
