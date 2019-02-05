/*
 *  File:       X3DDrawContext.cpp
 *  Summary:	Base class for QuickDraw 3D draw contexts.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DDrawContext.cpp,v $
 *		Revision 1.3  2001/04/14 07:28:07  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:15:00  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <->	  2/16/97	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <X3DDrawContext.h>

#include <X3DUtils.h>
#include <XConstants.h>
#include <XRGBColor.h>

namespace Whisper {


// ===================================================================================
//	class X3DDrawContext
// ===================================================================================

//---------------------------------------------------------------
//
// X3DDrawContext::~X3DDrawContext
//
//---------------------------------------------------------------
X3DDrawContext::~X3DDrawContext()
{
}


//---------------------------------------------------------------
//
// X3DDrawContext::X3DDrawContext
//
//---------------------------------------------------------------
X3DDrawContext::X3DDrawContext(TQ3DrawContextObject object) : X3DSharedObject(object)
{ 
	this->ConfirmType(kQ3SharedTypeDrawContext);
}


//---------------------------------------------------------------
//
// X3DDrawContext::Clone
//
//---------------------------------------------------------------
X3DDrawContext* X3DDrawContext::Clone() const
{
	X3DDrawContext* object = new X3DDrawContext(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DDrawContext::GetClearImageColor
//
//---------------------------------------------------------------
X3DColorARGB X3DDrawContext::GetClearImageColor() const
{
	X3DColorARGB color;
	
	TQ3Status status = Q3DrawContext_GetClearImageColor(mObject, &color);
	ThrowIf3DError(status);
	
	return color;
}


//---------------------------------------------------------------
//
// X3DDrawContext::SetClearImageColor
//
//---------------------------------------------------------------
void X3DDrawContext::SetClearImageColor(const X3DColorARGB& color)
{
	TQ3Status status = Q3DrawContext_SetClearImageColor(mObject, &color);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDrawContext::GetPane
//
//---------------------------------------------------------------
X2DRect X3DDrawContext::GetPane() const
{
	TQ3Area area;
	
	TQ3Status status = Q3DrawContext_GetPane(mObject, &area);
	ThrowIf3DError(status);
	
	return X2DRect(area.min, area.max);
}


//---------------------------------------------------------------
//
// X3DDrawContext::SetPane
//
//---------------------------------------------------------------
void X3DDrawContext::SetPane(const X2DRect& box)
{
	TQ3Area area;
	area.min = box[topLeft];
	area.max = box[botRight];
	
	TQ3Status status = Q3DrawContext_SetPane(mObject, &area);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDrawContext::GetPaneState
//
//---------------------------------------------------------------
bool X3DDrawContext::GetPaneState() const
{
	TQ3Boolean state;
	
	TQ3Status status = Q3DrawContext_GetPaneState(mObject, &state);
	ThrowIf3DError(status);
	
	return state;
}


//---------------------------------------------------------------
//
// X3DDrawContext::SetPaneState
//
//---------------------------------------------------------------
void X3DDrawContext::SetPaneState(bool inState)
{
	TQ3Boolean state = (TQ3Boolean) inState;
	
	TQ3Status status = Q3DrawContext_SetPaneState(mObject, state);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDrawContext::GetClearImageMethod
//
//---------------------------------------------------------------
TQ3DrawContextClearImageMethod X3DDrawContext::GetClearImageMethod() const
{
	TQ3DrawContextClearImageMethod method;
	
	TQ3Status status = Q3DrawContext_GetClearImageMethod(mObject, &method);
	ThrowIf3DError(status);
	
	return method;
}


//---------------------------------------------------------------
//
// X3DDrawContext::SetClearImageMethod
//
//---------------------------------------------------------------
void X3DDrawContext::SetClearImageMethod(TQ3DrawContextClearImageMethod method)
{
	TQ3Status status = Q3DrawContext_SetClearImageMethod(mObject, method);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDrawContext::GetMask
//
//---------------------------------------------------------------
TQ3Bitmap X3DDrawContext::GetMask() const
{
	TQ3Bitmap mask;
	
	TQ3Status status = Q3DrawContext_GetMask(mObject, &mask);
	ThrowIf3DError(status);
	
	return mask;
}


//---------------------------------------------------------------
//
// X3DDrawContext::SetMask
//
//---------------------------------------------------------------
void X3DDrawContext::SetMask(const TQ3Bitmap& mask)
{
	TQ3Status status = Q3DrawContext_SetMask(mObject, &mask);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDrawContext::GetMaskState
//
//---------------------------------------------------------------
bool X3DDrawContext::GetMaskState() const
{
	TQ3Boolean state;
	
	TQ3Status status = Q3DrawContext_GetMaskState(mObject, &state);
	ThrowIf3DError(status);
	
	return state;
}


//---------------------------------------------------------------
//
// X3DDrawContext::SetMaskState
//
//---------------------------------------------------------------
void X3DDrawContext::SetMaskState(bool inState)
{
	TQ3Boolean state = (TQ3Boolean) inState;
	
	TQ3Status status = Q3DrawContext_SetMaskState(mObject, state);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDrawContext::GetDoubleBufferState
//
//---------------------------------------------------------------
bool X3DDrawContext::GetDoubleBufferState() const
{
	TQ3Boolean state;
	
	TQ3Status status = Q3DrawContext_GetDoubleBufferState(mObject, &state);
	ThrowIf3DError(status);
	
	return state;
}


//---------------------------------------------------------------
//
// X3DDrawContext::SetDoubleBufferState
//
//---------------------------------------------------------------
void X3DDrawContext::SetDoubleBufferState(bool inState)
{
	TQ3Boolean state = (TQ3Boolean) inState;

	TQ3Status status = Q3DrawContext_SetDoubleBufferState(mObject, state);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDrawContext::GetData
//
//---------------------------------------------------------------
TQ3DrawContextData X3DDrawContext::GetData() const
{
	TQ3DrawContextData data;
	
	TQ3Status status = Q3DrawContext_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DDrawContext::SetData
//
//---------------------------------------------------------------
void X3DDrawContext::SetData(const TQ3DrawContextData& data)
{
	TQ3Status status = Q3DrawContext_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DDrawContext::IsCompatible 						[static]
//
//---------------------------------------------------------------
bool X3DDrawContext::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3SharedTypeDrawContext);
	
	return compatible;
}


}	// namespace Whisper

