/*
 *  File:       X3DSurfaceShaders.cpp
 *  Summary:	QuickDraw 3D surface shaders.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DSurfaceShaders.cpp,v $
 *		Revision 1.4  2001/04/21 03:38:19  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:32:35  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:21:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 12/03/97	JDJ		GetTexture returns an object by value (instead
 *									of via a counted pointer).
 *		 <2>	 12/03/97	JDJ		Renamed X3DTextureShader (from T3TextureShader).
 *		 <1>	  3/07/97	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <X3DSurfaceShaders.h>

#include <X3DUtils.h>
#include <XDebug.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateTextureShader
//
//---------------------------------------------------------------
static X3DObject* CreateTextureShader(TQ3Object inObject)
{
	X3DTextureShader* object = new X3DTextureShader(inObject);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DTextureShader
// ===================================================================================

//---------------------------------------------------------------
//
//	X3DTextureShader::~X3DTextureShader
//
//---------------------------------------------------------------
X3DTextureShader::~X3DTextureShader()
{
}


//---------------------------------------------------------------
//
//	X3DTextureShader::X3DTextureShader
//
//---------------------------------------------------------------
X3DTextureShader::X3DTextureShader(const X3DTexture& texture) : X3DSurfaceShader(Q3TextureShader_New(texture))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
//	X3DTextureShader::X3DTextureShader (TQ3SurfaceShaderObject)
//
//---------------------------------------------------------------
X3DTextureShader::X3DTextureShader(TQ3SurfaceShaderObject object) : X3DSurfaceShader(object)
{
	this->ConfirmType(kQ3SurfaceShaderTypeTexture);
}


//---------------------------------------------------------------
//
// X3DTextureShader::Clone
//
//---------------------------------------------------------------
X3DTextureShader* X3DTextureShader::Clone() const
{
	X3DTextureShader* object = new X3DTextureShader(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
//	X3DTextureShader::GetTexture
//
//---------------------------------------------------------------
X3DTexture X3DTextureShader::GetTexture() const
{
	TQ3TextureObject texture;
	
	TQ3Status status = Q3TextureShader_GetTexture(mObject, &texture);
	ThrowIf3DError(status);
	
	return X3DTexture(texture);
}


//---------------------------------------------------------------
//
//	X3DTextureShader::SetTexture
//
//---------------------------------------------------------------
void X3DTextureShader::SetTexture(const X3DTexture& texture)
{
	PRECONDITION(texture != nil);
	
	TQ3Status status = Q3TextureShader_SetTexture(mObject, texture);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DTextureShader::IsCompatible			 			[static]
//	
//---------------------------------------------------------------
bool X3DTextureShader::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3SurfaceShaderTypeTexture);
	
	return compatible;
}


}	// namespace Whisper
