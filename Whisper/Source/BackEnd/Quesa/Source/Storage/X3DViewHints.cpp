/*
 *  File:       X3DViewHints.cpp
 *  Summary:	An object that is saved in a metafile to give the reader hints on
 *				how to render a scene.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DViewHints.cpp,v $
 *		Revision 1.4  2001/04/21 03:39:11  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2000/12/10 04:09:02  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:24:34  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DViewHints.h>

#include <X3DUtils.h>
#include <X3DView.h>
#include <XExceptions.h>
#include <XNumbers.h>

namespace Whisper {


// ===================================================================================
//	class X3DViewHints
// ===================================================================================

//---------------------------------------------------------------
//
// X3DViewHints::~X3DViewHints
//
//---------------------------------------------------------------
X3DViewHints::~X3DViewHints()
{	
}


//---------------------------------------------------------------
//
// X3DViewHints::X3DViewHints (X3DView)
//
//---------------------------------------------------------------
X3DViewHints::X3DViewHints(const X3DView& view) : X3DSharedObject(Q3ViewHints_New(view))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewHints::X3DViewHints (TQ3ViewHintsObject)
//
//---------------------------------------------------------------
X3DViewHints::X3DViewHints(TQ3ViewHintsObject object) : X3DSharedObject(object)
{
	this->ConfirmType(kQ3SharedTypeViewHints);
}


//---------------------------------------------------------------
//
// X3DViewHints::Clone
//
//---------------------------------------------------------------
X3DViewHints* X3DViewHints::Clone() const
{
	X3DViewHints* object = new X3DViewHints(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DViewHints::GetRenderer
//
//---------------------------------------------------------------
X3DRenderer X3DViewHints::GetRenderer() const
{
	TQ3RendererObject object;
	
	TQ3Status status = Q3ViewHints_GetRenderer(mObject, &object);
	ThrowIf3DError(status);
	
	X3DRenderer obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DViewHints::SetRenderer
//
//---------------------------------------------------------------
void X3DViewHints::SetRenderer(const X3DRenderer& renderer)
{
	TQ3Status status = Q3ViewHints_SetRenderer(mObject, renderer);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewHints::GetCamera
//
//---------------------------------------------------------------
X3DCamera X3DViewHints::GetCamera() const
{
	TQ3CameraObject object;
	
	TQ3Status status = Q3ViewHints_GetCamera(mObject, &object);
	ThrowIf3DError(status);
	
	X3DCamera obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DViewHints::SetCamera
//
//---------------------------------------------------------------
void X3DViewHints::SetCamera(const X3DCamera& camera)
{
	TQ3Status status = Q3ViewHints_SetCamera(mObject, camera);
	ThrowIf3DError(status);
}
 		

//---------------------------------------------------------------
//
// X3DViewHints::GetLightGroup
//
//---------------------------------------------------------------
X3DLightGroup X3DViewHints::GetLightGroup() const
{
	TQ3GroupObject object;
	
	TQ3Status status = Q3ViewHints_GetLightGroup(mObject, &object);
	ThrowIf3DError(status);
	
	X3DLightGroup obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DViewHints::SetLightGroup
//
//---------------------------------------------------------------
void X3DViewHints::SetLightGroup(const X3DLightGroup& lights)
{
	TQ3Status status = Q3ViewHints_SetLightGroup(mObject, lights);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewHints::GetAttributeSet
//
//---------------------------------------------------------------
X3DAttributeSet X3DViewHints::GetAttributeSet() const
{
	TQ3AttributeSet object;
	
	TQ3Status status = Q3ViewHints_GetAttributeSet(mObject, &object);
	ThrowIf3DError(status);
	
	X3DAttributeSet obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DViewHints::SetAttributeSet
//
//---------------------------------------------------------------
void X3DViewHints::SetAttributeSet(const X3DAttributeSet& attributes)
{
	TQ3Status status = Q3ViewHints_SetAttributeSet(mObject, attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewHints::GetDimensionState
//
//---------------------------------------------------------------
bool X3DViewHints::GetDimensionState() const
{
	TQ3Boolean state;
	
	TQ3Status status = Q3ViewHints_GetDimensionsState(mObject, &state);
	ThrowIf3DError(status);
	
	return state;
}


//---------------------------------------------------------------
//
// X3DViewHints::SetDimensionState
//
//---------------------------------------------------------------
void X3DViewHints::SetDimensionState(bool inState)
{
	TQ3Boolean state = inState ? kQ3True : kQ3False;
	
	TQ3Status status = Q3ViewHints_SetDimensionsState(mObject, state);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewHints::GetDimensions
//
//---------------------------------------------------------------
XSize X3DViewHints::GetDimensions() const
{
	uint32 width, height;

	TQ3Status status = Q3ViewHints_GetDimensions(mObject, &width, &height);
	ThrowIf3DError(status);
	
	return XSize(numeric_cast<int32>(width), numeric_cast<int32>(height));
}


//---------------------------------------------------------------
//
// X3DViewHints::SetDimensions
//
//---------------------------------------------------------------
void X3DViewHints::SetDimensions(const XSize& size)
{
	TQ3Status status = Q3ViewHints_SetDimensions(mObject, numeric_cast<uint32>(size.width), numeric_cast<uint32>(size.height));
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewHints::GetMaskState
//
//---------------------------------------------------------------
bool X3DViewHints::GetMaskState() const
{
	TQ3Boolean state;
	
	TQ3Status status = Q3ViewHints_GetMaskState(mObject, &state);
	ThrowIf3DError(status);
	
	return state;
}


//---------------------------------------------------------------
//
// X3DViewHints::SetMaskState
//
//---------------------------------------------------------------
void X3DViewHints::SetMaskState(bool inState)
{
	TQ3Boolean state = inState ? kQ3True : kQ3False;

	TQ3Status status = Q3ViewHints_SetMaskState(mObject, state);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewHints::GetMask
//
//---------------------------------------------------------------
void X3DViewHints::GetMask(TQ3Bitmap* mask) const
{
	PRECONDITION(mask != nil);

	TQ3Status status = Q3ViewHints_GetMask(mObject, mask);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewHints::SetMask
//
//---------------------------------------------------------------
void X3DViewHints::SetMask(const TQ3Bitmap* mask)
{
	PRECONDITION(mask != nil);

	TQ3Status status = Q3ViewHints_SetMask(mObject, mask);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewHints::GetClearImageMethod
//
//---------------------------------------------------------------
TQ3DrawContextClearImageMethod X3DViewHints::GetClearImageMethod() const
{
	TQ3DrawContextClearImageMethod method;
	
	TQ3Status status = Q3ViewHints_GetClearImageMethod(mObject, &method);
	ThrowIf3DError(status);
	
	return method;
}


//---------------------------------------------------------------
//
// X3DViewHints::SetClearImageMethod
//
//---------------------------------------------------------------
void X3DViewHints::SetClearImageMethod(TQ3DrawContextClearImageMethod method)
{
	TQ3Status status = Q3ViewHints_SetClearImageMethod(mObject, method);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DViewHints::GetClearColor
//
//---------------------------------------------------------------
X3DColorARGB X3DViewHints::GetClearColor() const
{
	X3DColorARGB color;
	
	TQ3Status status = Q3ViewHints_GetClearImageColor(mObject, &color);
	ThrowIf3DError(status);
	
	return color;
}


//---------------------------------------------------------------
//
// X3DViewHints::SetClearColor
//
//---------------------------------------------------------------
void X3DViewHints::SetClearColor(const X3DColorARGB& color)
{
	TQ3Status status = Q3ViewHints_SetClearImageColor(mObject, &color);
	ThrowIf3DError(status);
}



//---------------------------------------------------------------
//
// X3DViewHints::IsCompatible							[static]
//	
//---------------------------------------------------------------
bool X3DViewHints::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3SharedTypeViewHints);
	
	return compatible;
}


}	// namespace Whisper
