/*
 *  File:       X3DTexture.cpp
 *  Summary:	Base class for QuickDraw 3D texture map objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DTexture.cpp,v $
 *		Revision 1.3  2001/04/14 07:32:39  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:22:00  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DTexture.h>

#include <QuesaShader.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	class X3DTexture
// ===================================================================================

//---------------------------------------------------------------
//
// X3DTexture::~X3DTexture
//
//---------------------------------------------------------------
X3DTexture::~X3DTexture()
{
}


//---------------------------------------------------------------
//
// X3DTexture::X3DTexture
//
//---------------------------------------------------------------
X3DTexture::X3DTexture(TQ3TextureObject object) : X3DSharedObject(object)
{ 
	this->ConfirmType(kQ3SharedTypeTexture);
}


//---------------------------------------------------------------
//
// X3DTexture::Clone
//
//---------------------------------------------------------------
X3DTexture* X3DTexture::Clone() const
{
	X3DTexture* object = new X3DTexture(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DTexture::GetWidth
//
//---------------------------------------------------------------
uint32 X3DTexture::GetWidth() const
{
	uint32 width;
	
	TQ3Status status = Q3Texture_GetWidth(mObject, &width);
	ThrowIf3DError(status);
	
	return width;
}


//---------------------------------------------------------------
//
// X3DTexture::GetHeight
//
//---------------------------------------------------------------
uint32 X3DTexture::GetHeight() const
{
	uint32 height;
	
	TQ3Status status = Q3Texture_GetHeight(mObject, &height);
	ThrowIf3DError(status);
	
	return height;
}


//---------------------------------------------------------------
//
// X3DTexture::IsCompatible			 					[static]
//	
//---------------------------------------------------------------
bool X3DTexture::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3SharedTypeTexture);
	
	return compatible;
}


}	// namespace Whisper
