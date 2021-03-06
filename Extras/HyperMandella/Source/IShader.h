/*
 *  File:       IShader.h
 *  Summary:   	Interface for objects that determine the color of fractal surfaces.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IShader.h,v $
 *		Revision 1.2  2001/03/25 06:08:44  jesjones
 *		Added a comment.
 *		
 *		Revision 1.1  2000/12/28 02:51:42  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.3  2000/12/17 03:42:24  jesjones
 *		Uses XRGBColor instead of X3DRGBColor.
 *		
 *		Revision 1.2  2000/11/09 09:07:07  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <X3DColors.h>
#include <X3DPrimitives.h>
#include <X3DVectors.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


//-----------------------------------
//	Forward References
//
class IDocInfo;
typedef XInterfacePtr<const IDocInfo> IConstDocInfoPtr;


// ===================================================================================
//	class IShader
//!		Interface for objects that determine the color of fractal surfaces.
// ===================================================================================
class IShader : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual XRGBColor 	GetColor(const IConstDocInfoPtr& info, const XRGBColor& diffuseColor, const X3DPoint& pt, 
								 const X3DVector& normalV, const X3DVector& viewV) const = 0;
};

typedef XInterfacePtr<IShader> IShaderPtr;
typedef XInterfacePtr<const IShader> IConstShaderPtr;

