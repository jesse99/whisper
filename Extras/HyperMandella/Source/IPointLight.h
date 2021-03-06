/*
 *  File:       IPointLight.h
 *  Summary:   	Interface for a light that emits rays in all directions from a specified point.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IPointLight.h,v $
 *		Revision 1.2  2001/03/25 06:08:29  jesjones
 *		Added a comment.
 *		
 *		Revision 1.1  2000/12/28 02:51:29  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 09:06:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <X3DPrimitives.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


//-----------------------------------
//	Constants
//
enum ELightAttenuation {kNoAttenuation, kInverseDistanceAttenuation, kInverseDistanceSqrAttenuation};


// ===================================================================================
//	class IPointLight
//!		Interface for a light that emits rays in all directions from a specified point.
// ===================================================================================
class IPointLight : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Location -----
	virtual X3DPoint 	GetLocation() const = 0;
			
	virtual void 		SetLocation(const X3DPoint& loc) = 0;
						
	// ----- Attenuation -----
	virtual ELightAttenuation GetAttenuation() const = 0;
			
	virtual void 		SeAttenuation(ELightAttenuation attenuation) = 0;
						/**< Defaults to kNoAttenuation. */
};

typedef XInterfacePtr<IPointLight> IPointLightPtr;
typedef XInterfacePtr<const IPointLight> IConstPointLightPtr;

