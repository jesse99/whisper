/*
 *  File:       X3DIlluminationShader.cpp
 *  Summary:	Base class for QuickDraw 3D illumination shaders.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DIlluminationShader.cpp,v $
 *		Revision 1.3  2001/04/14 07:29:07  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:16:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <->	  2/17/97	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <X3DIlluminationShader.h>

namespace Whisper {


// ===================================================================================
//	class X3DIlluminationShader
// ===================================================================================

//---------------------------------------------------------------
//
//	X3DIlluminationShader::~X3DIlluminationShader
//
//---------------------------------------------------------------
X3DIlluminationShader::~X3DIlluminationShader()
{
}


//---------------------------------------------------------------
//
//	X3DIlluminationShader::X3DIlluminationShader (TQ3ShaderObject)
//
//---------------------------------------------------------------
X3DIlluminationShader::X3DIlluminationShader(TQ3ShaderObject object) : X3DShader(object)
{
	this->ConfirmType(kQ3ShaderTypeIllumination);
}


//---------------------------------------------------------------
//
// X3DIlluminationShader::Clone
//
//---------------------------------------------------------------
X3DIlluminationShader* X3DIlluminationShader::Clone() const
{
	X3DIlluminationShader* object = new X3DIlluminationShader(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DIlluminationShader::IsCompatible 					[static]
//	
//---------------------------------------------------------------
bool X3DIlluminationShader::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3ShaderTypeIllumination);
	
	return compatible;
}


}	// namespace Whisper

