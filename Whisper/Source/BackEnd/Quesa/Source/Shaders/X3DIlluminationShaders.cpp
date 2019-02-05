/*
 *  File:       X3DIlluminationShaders.cpp
 *  Summary:	QuickDraw 3D illumination shaders.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DIlluminationShaders.cpp,v $
 *		Revision 1.3  2001/04/14 07:29:11  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:16:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <->	  2/17/97	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <X3DIlluminationShaders.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreatePhongShader
//
//---------------------------------------------------------------
static X3DObject* CreatePhongShader(TQ3Object inObject)
{
	X3DPhongShader* object = new X3DPhongShader(inObject);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateLambertShader
//
//---------------------------------------------------------------
static X3DObject* CreateLambertShader(TQ3Object inObject)
{
	X3DLambertShader* object = new X3DLambertShader(inObject);
	
	return object;
}


//---------------------------------------------------------------
//
// CreateNullShader
//
//---------------------------------------------------------------
static X3DObject* CreateNullShader(TQ3Object inObject)
{
	X3DNullShader* object = new X3DNullShader(inObject);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DPhongShader
// ===================================================================================

//---------------------------------------------------------------
//
//	X3DPhongShader::~X3DPhongShader
//
//---------------------------------------------------------------
X3DPhongShader::~X3DPhongShader()
{
}


//---------------------------------------------------------------
//
//	X3DPhongShader::X3DPhongShader ()
//
//---------------------------------------------------------------
X3DPhongShader::X3DPhongShader() : X3DIlluminationShader(Q3PhongIllumination_New())
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
//	X3DPhongShader::X3DPhongShader (TQ3ShaderObject)
//
//---------------------------------------------------------------
X3DPhongShader::X3DPhongShader(TQ3ShaderObject object) : X3DIlluminationShader(object)
{
	this->ConfirmType(kQ3IlluminationTypePhong);
}


//---------------------------------------------------------------
//
// X3DPhongShader::Clone
//
//---------------------------------------------------------------
X3DPhongShader* X3DPhongShader::Clone() const
{
	X3DPhongShader* object = new X3DPhongShader(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DPhongShader::IsCompatible 						[static]
//	
//---------------------------------------------------------------
bool X3DPhongShader::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3IlluminationTypePhong);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DLambertShader
// ===================================================================================

//---------------------------------------------------------------
//
//	X3DLambertShader::~X3DLambertShader
//
//---------------------------------------------------------------
X3DLambertShader::~X3DLambertShader()
{
}


//---------------------------------------------------------------
//
//	X3DLambertShader::X3DLambertShader ()
//
//---------------------------------------------------------------
X3DLambertShader::X3DLambertShader() : X3DIlluminationShader(Q3LambertIllumination_New())
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
//	X3DLambertShader::X3DLambertShader (TQ3ShaderObject)
//
//---------------------------------------------------------------
X3DLambertShader::X3DLambertShader(TQ3ShaderObject object) : X3DIlluminationShader(object)
{
	this->ConfirmType(kQ3IlluminationTypeLambert);
}


//---------------------------------------------------------------
//
// X3DLambertShader::Clone
//
//---------------------------------------------------------------
X3DLambertShader* X3DLambertShader::Clone() const
{
	X3DLambertShader* object = new X3DLambertShader(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DLambertShader::IsCompatible 						[static]
//	
//---------------------------------------------------------------
bool X3DLambertShader::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3IlluminationTypeLambert);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DNullShader
// ===================================================================================

//---------------------------------------------------------------
//
//	X3DNullShader::~X3DNullShader
//
//---------------------------------------------------------------
X3DNullShader::~X3DNullShader()
{
}


//---------------------------------------------------------------
//
//	X3DNullShader::X3DNullShader ()
//
//---------------------------------------------------------------
X3DNullShader::X3DNullShader() : X3DIlluminationShader(Q3NULLIllumination_New())
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
//	X3DNullShader::X3DNullShader (TQ3ShaderObject)
//
//---------------------------------------------------------------
X3DNullShader::X3DNullShader(TQ3ShaderObject object) : X3DIlluminationShader(object)
{
	this->ConfirmType(kQ3IlluminationTypeNULL);
}


//---------------------------------------------------------------
//
// X3DNullShader::Clone
//
//---------------------------------------------------------------
X3DNullShader* X3DNullShader::Clone() const
{
	X3DNullShader* object = new X3DNullShader(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DNullShader::IsCompatible 							[static]
//	
//---------------------------------------------------------------
bool X3DNullShader::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3IlluminationTypeNULL);
	
	return compatible;
}


}	// namespace Whisper
