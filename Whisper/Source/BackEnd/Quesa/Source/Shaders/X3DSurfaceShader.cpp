/*
 *  File:       X3DSurfaceShader.cpp
 *  Summary:	Base class for QuickDraw 3D surface shaders.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DSurfaceShader.cpp,v $
 *		Revision 1.3  2001/04/14 07:32:31  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:21:44  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <->	  3/07/97	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <X3DSurfaceShader.h>

namespace Whisper {


// ===================================================================================
//	class X3DSurfaceShader
// ===================================================================================

//---------------------------------------------------------------
//
//	X3DSurfaceShader::~X3DSurfaceShader
//
//---------------------------------------------------------------
X3DSurfaceShader::~X3DSurfaceShader()
{
}


//---------------------------------------------------------------
//
//	X3DSurfaceShader::X3DSurfaceShader (TQ3SurfaceShaderObject)
//
//---------------------------------------------------------------
X3DSurfaceShader::X3DSurfaceShader(TQ3SurfaceShaderObject object) : X3DShader(object)
{
	this->ConfirmType(kQ3ShaderTypeSurface);
}


//---------------------------------------------------------------
//
// X3DSurfaceShader::Clone
//
//---------------------------------------------------------------
X3DSurfaceShader* X3DSurfaceShader::Clone() const
{
	X3DSurfaceShader* object = new X3DSurfaceShader(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DSurfaceShader::IsCompatible			 			[static]
//	
//---------------------------------------------------------------
bool X3DSurfaceShader::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3ShaderTypeSurface);
	
	return compatible;
}


}	// namespace Whisper

