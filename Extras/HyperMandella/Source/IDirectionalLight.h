/*
 *  File:       IDirectionalLight.h
 *  Summary:   	Interface for a light that casts parallel rays of light in a specific direction.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IDirectionalLight.h,v $
 *		Revision 1.2  2001/03/25 06:08:07  jesjones
 *		Added a comment.
 *		
 *		Revision 1.1  2000/12/28 02:51:09  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 09:01:01  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <X3DVectors.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


// ===================================================================================
//	class IDirectionalLight
//!		Interface for a light that casts parallel rays of light in a specific direction.
// ===================================================================================
class IDirectionalLight : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual X3DVector 	GetDirection() const = 0;
			
	virtual void 		SetDirection(const X3DVector& dir) = 0;
};

typedef XInterfacePtr<IDirectionalLight> IDirectionalLightPtr;
typedef XInterfacePtr<const IDirectionalLight> IConstDirectionalLightPtr;

