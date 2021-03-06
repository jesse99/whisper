/*
 *  File:       X3DRenderer.cpp
 *  Summary:	QuickDraw 3D renderer object.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DRenderer.cpp,v $
 *		Revision 1.4  2001/04/21 03:37:30  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:31:08  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:19:35  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DRenderer.h>

#include <X3DUtils.h>
#include <XDebug.h>

namespace Whisper {


// ===================================================================================
//	class X3DRenderer
// ===================================================================================

//---------------------------------------------------------------
//
// X3DRenderer::~X3DRenderer
//
//---------------------------------------------------------------
X3DRenderer::~X3DRenderer()
{
}


//---------------------------------------------------------------
//
// X3DRenderer::X3DRenderer (TQ3ObjectType)
//
//---------------------------------------------------------------
X3DRenderer::X3DRenderer(TQ3ObjectType type) : X3DSharedObject(Q3Renderer_NewFromType(type))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRenderer::X3DRenderer (TQ3RendererObject)
//
//---------------------------------------------------------------
X3DRenderer::X3DRenderer(TQ3RendererObject object) : X3DSharedObject(object)
{
	this->ConfirmType(kQ3SharedTypeRenderer);
}


//---------------------------------------------------------------
//
// X3DRenderer::Clone
//
//---------------------------------------------------------------
X3DRenderer* X3DRenderer::Clone() const
{
	X3DRenderer* object = new X3DRenderer(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DRenderer::HasModalConfigure
//
//---------------------------------------------------------------
bool X3DRenderer::HasModalConfigure() const
{
	TQ3Boolean has = Q3Renderer_HasModalConfigure(mObject);
	
	return has;
}


//---------------------------------------------------------------
//
// X3DRenderer::ModalConfigure
//
//---------------------------------------------------------------
bool X3DRenderer::ModalConfigure(TQ3DialogAnchor anchor)
{
	TQ3Boolean canceled;
	
	TQ3Status status = Q3Renderer_ModalConfigure(mObject, anchor, &canceled);
	ThrowIf3DError(status);
	
	return !(canceled == kQ3True);
}


//---------------------------------------------------------------
//
// X3DRenderer::GetConfigurationSize
//
//---------------------------------------------------------------
uint32 X3DRenderer::GetConfigurationSize() const
{
	uint32 actualSize;
	
	TQ3Status status = Q3Renderer_GetConfigurationData(mObject, nil, 0, &actualSize);
	ThrowIf3DError(status);
	
	return actualSize;
}


//---------------------------------------------------------------
//
// X3DRenderer::GetConfigurationData
//
//---------------------------------------------------------------
void X3DRenderer::GetConfigurationData(void* buffer, uint32 bufferSize) const
{
	PRECONDITION(buffer != nil);
	PRECONDITION(bufferSize >= this->GetConfigurationSize());
	
	uint32 actualSize;

	TQ3Status status = Q3Renderer_GetConfigurationData(mObject, (uint8*) buffer, bufferSize, &actualSize);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRenderer::SetConfigurationData
//
//---------------------------------------------------------------
void X3DRenderer::SetConfigurationData(const void* buffer, uint32 bufferSize)
{
	PRECONDITION(buffer != nil);

	TQ3Status status = Q3Renderer_SetConfigurationData(mObject, (uint8*) buffer, bufferSize);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRenderer::IsCompatible 							[static]
//	
//---------------------------------------------------------------
bool X3DRenderer::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3SharedTypeRenderer);
	
	return compatible;
}


}	// namespace Whisper
