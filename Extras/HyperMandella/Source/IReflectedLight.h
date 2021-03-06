/*
 *  File:       IReflectedLight.h
 *  Summary:   	Interface for lights that cast rays of light that bounce off surfaces.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IReflectedLight.h,v $
 *		Revision 1.2  2001/03/25 06:08:38  jesjones
 *		Added a comment.
 *		
 *		Revision 1.1  2000/12/28 02:51:35  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 09:06:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <X3DPrimitives.h>
#include <X3DVectors.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


// ===================================================================================
//	class IReflectedLight
//!		Interface for lights that cast rays of light that bounce off surfaces.
// ===================================================================================
class IReflectedLight : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual X3DVector 	GetLightVector(const X3DPoint& pt) const = 0;
						/**< Returns a unit vector from the specified point to the light source. */
};

typedef XInterfacePtr<IReflectedLight> IReflectedLightPtr;
typedef XInterfacePtr<const IReflectedLight> IConstReflectedLightPtr;

