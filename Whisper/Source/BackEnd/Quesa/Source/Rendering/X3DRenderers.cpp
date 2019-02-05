/*
 *  File:       X3DRenderers.cpp
 *  Summary:	QuickDraw 3D renderers.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DRenderers.cpp,v $
 *		Revision 1.4  2001/04/21 03:37:34  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:31:17  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:19:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	  4/19/00	JDJ		Added X3DInteractiveRenderer::CountRAVEDrawContexts 
 *									and GetRAVEDrawContext.
 *		 <2>	 12/03/97	JDJ		Added missing SetVendorID.
 *		 <1>	  2/17/97	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <X3DRenderers.h>

#include <X3DUtils.h>
#include <XTinyVector.h>

namespace Whisper {


// ===================================================================================
//	class X3DInteractiveRenderer
// ===================================================================================

//---------------------------------------------------------------
//
// X3DInteractiveRenderer::~X3DInteractiveRenderer
//
//---------------------------------------------------------------
X3DInteractiveRenderer::~X3DInteractiveRenderer()
{
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::X3DInteractiveRenderer ()
//
//---------------------------------------------------------------
X3DInteractiveRenderer::X3DInteractiveRenderer() : X3DRenderer(kQ3RendererTypeInteractive)
{
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::X3DInteractiveRenderer (TQ3RendererObject)
//
//---------------------------------------------------------------
X3DInteractiveRenderer::X3DInteractiveRenderer(TQ3RendererObject object) : X3DRenderer(object)
{
	this->ConfirmType(kQ3RendererTypeInteractive);
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::Clone
//
//---------------------------------------------------------------
X3DInteractiveRenderer* X3DInteractiveRenderer::Clone() const
{
	X3DInteractiveRenderer* object = new X3DInteractiveRenderer(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::GetCSGEquation
//
//---------------------------------------------------------------
TQ3CSGEquation X3DInteractiveRenderer::GetCSGEquation() const
{
	TQ3CSGEquation equation;
	
	TQ3Status status = Q3InteractiveRenderer_GetCSGEquation(mObject, &equation);
	ThrowIf3DError(status);
	
	return equation;
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::SetCSGEquation
//
//---------------------------------------------------------------
void X3DInteractiveRenderer::SetCSGEquation(TQ3CSGEquation equation)
{
	TQ3Status status = Q3InteractiveRenderer_SetCSGEquation(mObject, equation);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::GetVendorID
//
//---------------------------------------------------------------
long X3DInteractiveRenderer::GetVendorID() const
{
	long vendorID;
	long engineID;
	
	TQ3Status status = Q3InteractiveRenderer_GetPreferences(mObject, &vendorID, &engineID);
	ThrowIf3DError(status);
	
	return vendorID;
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::SetVendorID
//
//---------------------------------------------------------------
void X3DInteractiveRenderer::SetVendorID(long vendor)
{
	long engine = this->GetEngineID();
	
	TQ3Status status = Q3InteractiveRenderer_SetPreferences(mObject, vendor, engine);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::GetEngineID
//
//---------------------------------------------------------------
long X3DInteractiveRenderer::GetEngineID() const
{
	long vendorID;
	long engineID;
	
	TQ3Status status = Q3InteractiveRenderer_GetPreferences(mObject, &vendorID, &engineID);
	ThrowIf3DError(status);
	
	return engineID;
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::SetEngineID
//
//---------------------------------------------------------------
void X3DInteractiveRenderer::SetEngineID(long engine)
{
	long vendor = this->GetVendorID();
	
	TQ3Status status = Q3InteractiveRenderer_SetPreferences(mObject, vendor, engine);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::GetDoubleBufferBypass
//
//---------------------------------------------------------------
bool X3DInteractiveRenderer::GetDoubleBufferBypass() const
{
	TQ3Boolean bypassing;
	
	TQ3Status status = Q3InteractiveRenderer_GetDoubleBufferBypass(mObject, &bypassing);
	ThrowIf3DError(status);
	
	return bypassing;
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::SetDoubleBufferBypass
//
//---------------------------------------------------------------
void X3DInteractiveRenderer::SetDoubleBufferBypass(bool inBypass)
{
	TQ3Boolean bypass = (TQ3Boolean) inBypass;
	
	TQ3Status status = Q3InteractiveRenderer_SetDoubleBufferBypass(mObject, bypass);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::GetRAVEContextHints
//
//---------------------------------------------------------------
uint32 X3DInteractiveRenderer::GetRAVEContextHints() const
{
	uint32 hints;
	
	TQ3Status status = Q3InteractiveRenderer_GetRAVEContextHints(mObject, &hints);
	ThrowIf3DError(status);
	
	return hints;
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::SetRAVEContextHints
//
//---------------------------------------------------------------
void X3DInteractiveRenderer::SetRAVEContextHints(uint32 hints)
{
	TQ3Status status = Q3InteractiveRenderer_SetRAVEContextHints(mObject, hints);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::GetRAVETextureFilter
//
//---------------------------------------------------------------
uint32 X3DInteractiveRenderer::GetRAVETextureFilter() const
{
	uint32 filter;
	
	TQ3Status status = Q3InteractiveRenderer_GetRAVETextureFilter(mObject, &filter);
	ThrowIf3DError(status);
	
	return filter;
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::SetRAVETextureFilter
//
//---------------------------------------------------------------
void X3DInteractiveRenderer::SetRAVETextureFilter(uint32 filter)
{
	TQ3Status status = Q3InteractiveRenderer_SetRAVETextureFilter(mObject, filter);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::CountRAVEDrawContexts
//
//---------------------------------------------------------------
uint32 X3DInteractiveRenderer::CountRAVEDrawContexts() const
{
	uint32 count = 0;
	
	TQ3Status status = Q3InteractiveRenderer_CountRAVEDrawContexts(mObject, &count);
	ThrowIf3DError(status);
	
	return count;
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::GetRAVEDrawContext
//
//---------------------------------------------------------------
void X3DInteractiveRenderer::GetRAVEDrawContext(uint32 index, TQADrawContext*& context, TQAEngine*& engine) const
{
	PRECONDITION(context != nil);
	PRECONDITION(engine != nil);
	PRECONDITION(index < this->CountRAVEDrawContexts());
	
	uint32 count = this->CountRAVEDrawContexts();
	XTinyVector<TQADrawContext*> contexts(count);
	XTinyVector<TQAEngine*> engines(count);

	TQ3Status status = Q3InteractiveRenderer_GetRAVEDrawContexts(mObject, contexts.buffer(), engines.buffer(), &count, nil);
	ThrowIf3DError(status);
	
	context = contexts[index];
	engine = engines[index];
}


//---------------------------------------------------------------
//
// X3DInteractiveRenderer::IsCompatible 				[static]
//	
//---------------------------------------------------------------
bool X3DInteractiveRenderer::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3RendererTypeInteractive);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DWireframeRenderer
// ===================================================================================

//---------------------------------------------------------------
//
// X3DWireframeRenderer::~X3DWireframeRenderer
//
//---------------------------------------------------------------
X3DWireframeRenderer::~X3DWireframeRenderer()
{
}


//---------------------------------------------------------------
//
// X3DWireframeRenderer::X3DWireframeRenderer ()
//
//---------------------------------------------------------------
X3DWireframeRenderer::X3DWireframeRenderer() : X3DRenderer(kQ3RendererTypeWireFrame)
{
}


//---------------------------------------------------------------
//
// X3DWireframeRenderer::X3DWireframeRenderer (TQ3RendererObject)
//
//---------------------------------------------------------------
X3DWireframeRenderer::X3DWireframeRenderer(TQ3RendererObject object) : X3DRenderer(object)
{
	this->ConfirmType(kQ3RendererTypeWireFrame);
}


//---------------------------------------------------------------
//
// X3DWireframeRenderer::Clone
//
//---------------------------------------------------------------
X3DWireframeRenderer* X3DWireframeRenderer::Clone() const
{
	X3DWireframeRenderer* object = new X3DWireframeRenderer(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DWireframeRenderer::IsCompatible 					[static]
//	
//---------------------------------------------------------------
bool X3DWireframeRenderer::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3RendererTypeWireFrame);
	
	return compatible;
}


}	// namespace Whisper
