/*
 *  File:       X3DShader.cpp
 *  Summary:	Base class for QuickDraw 3D shaders.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DShader.cpp,v $
 *		Revision 1.4  2001/04/21 03:37:51  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:31:44  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:20:18  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DShader.h>

#include <X3DUtils.h>
#include <XDebug.h>

namespace Whisper {


// ===================================================================================
//	class X3DShader
// ===================================================================================

//---------------------------------------------------------------
//
//	X3DShader::~X3DShader
//
//---------------------------------------------------------------
X3DShader::~X3DShader()
{
}


//---------------------------------------------------------------
//
//	X3DShader::X3DShader
//
//---------------------------------------------------------------
X3DShader::X3DShader(TQ3ShaderObject object) : X3DShape(object)
{
	this->ConfirmType(kQ3ShapeTypeShader);
}


//---------------------------------------------------------------
//
// X3DShader::Clone
//
//---------------------------------------------------------------
X3DShader* X3DShader::Clone() const
{
	X3DShader* object = new X3DShader(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
//	X3DShader::Submit
//
//---------------------------------------------------------------
void X3DShader::Submit(TQ3ViewObject view) const
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Shader_Submit(mObject, view);
	ThrowIf3DError(status);
}

	
//---------------------------------------------------------------
//
//	X3DShader::GetUVTransform
//
//---------------------------------------------------------------
TQ3Matrix3x3 X3DShader::GetUVTransform() const
{
	TQ3Matrix3x3 transform;
	
	TQ3Status status = Q3Shader_GetUVTransform(mObject, &transform);
	ThrowIf3DError(status);
	
	return transform;
}


//---------------------------------------------------------------
//
//	X3DShader::SetUVTransform
//
//---------------------------------------------------------------
void X3DShader::SetUVTransform(const TQ3Matrix3x3& transform)
{
	TQ3Status status = Q3Shader_SetUVTransform(mObject, &transform);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
//	X3DShader::GetUBoundary
//
//---------------------------------------------------------------
TQ3ShaderUVBoundary X3DShader::GetUBoundary() const
{
	TQ3ShaderUVBoundary boundary;
	
	TQ3Status status = Q3Shader_GetUBoundary(mObject, &boundary);
	ThrowIf3DError(status);
	
	return boundary;
}


//---------------------------------------------------------------
//
//	X3DShader::SetUBoundary
//
//---------------------------------------------------------------
void X3DShader::SetUBoundary(const TQ3ShaderUVBoundary& boundary)
{
	TQ3Status status = Q3Shader_SetUBoundary(mObject, boundary);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
//	X3DShader::GetVBoundary
//
//---------------------------------------------------------------
TQ3ShaderUVBoundary X3DShader::GetVBoundary() const
{
	TQ3ShaderUVBoundary boundary;
	
	TQ3Status status = Q3Shader_GetVBoundary(mObject, &boundary);
	ThrowIf3DError(status);
	
	return boundary;
}


//---------------------------------------------------------------
//
//	X3DShader::SetVBoundary
//
//---------------------------------------------------------------
void X3DShader::SetVBoundary(const TQ3ShaderUVBoundary& boundary)
{
	TQ3Status status = Q3Shader_SetVBoundary(mObject, boundary);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DShader::IsCompatible 								[static]
//	
//---------------------------------------------------------------
bool X3DShader::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3ShapeTypeShader);
	
	return compatible;
}


}	// namespace Whisper
