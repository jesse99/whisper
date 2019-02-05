/*
 *  File:       X3DRenderingLoop.cpp
 *  Summary:	Base class for QuickDraw 3D rendering loops.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DRenderingLoop.cpp,v $
 *		Revision 1.4  2001/04/21 03:37:39  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:31:21  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:19:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DRenderingLoop.h>

#include <X3DUtils.h>
#include <X3DView.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class X3DPushState
// ===================================================================================

//---------------------------------------------------------------
//
// X3DPushState::~X3DPushState
//
//---------------------------------------------------------------
X3DPushState::~X3DPushState()
{
	TQ3Status status = Q3Pop_Submit(mView);
	ASSERT(status == kQ3Success);
}


//---------------------------------------------------------------
//
// X3DPushState::X3DPushState
//
//---------------------------------------------------------------
X3DPushState::X3DPushState(TQ3ViewObject view) : mView(view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Push_Submit(mView);
	ThrowIf3DError(status);
}

#pragma mark -

// ===================================================================================
//	class X3DRenderingLoop
// ===================================================================================

//---------------------------------------------------------------
//
// X3DRenderingLoop::~X3DRenderingLoop
//
//---------------------------------------------------------------
X3DRenderingLoop::~X3DRenderingLoop()
{
	PRECONDITION(mStatus != kQ3ViewStatusRetraverse);
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::X3DRenderingLoop
//
//---------------------------------------------------------------
X3DRenderingLoop::X3DRenderingLoop(TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	mView   = view;
	mStatus = kQ3ViewStatusRetraverse;
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::GetLocalToWorldMatrixState
//
//---------------------------------------------------------------
X3DMatrix X3DRenderingLoop::GetLocalToWorldMatrixState() const
{
	X3DMatrix matrix;
	
	TQ3Status status = Q3View_GetLocalToWorldMatrixState(mView, matrix);
	ThrowIf3DError(status);
	
	return matrix;
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::GetWorldToFrustumMatrixState
//
//---------------------------------------------------------------
X3DMatrix X3DRenderingLoop::GetWorldToFrustumMatrixState() const
{
	X3DMatrix matrix;
	
	TQ3Status status = Q3View_GetWorldToFrustumMatrixState(mView, matrix);
	ThrowIf3DError(status);
	
	return matrix;
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::GetFrustumToWindowMatrixState
//
//---------------------------------------------------------------
X3DMatrix X3DRenderingLoop::GetFrustumToWindowMatrixState() const
{
	X3DMatrix matrix;
	
	TQ3Status status = Q3View_GetFrustumToWindowMatrixState(mView, matrix);
	ThrowIf3DError(status);
	
	return matrix;
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::Push
//
//---------------------------------------------------------------
void X3DRenderingLoop::Push()
{
	TQ3Status status = Q3Push_Submit(mView);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::Pop
//
//---------------------------------------------------------------
void X3DRenderingLoop::Pop()
{
	TQ3Status status = Q3Pop_Submit(mView);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::GetBackfacingStyleState
//
//---------------------------------------------------------------
X3DBackfacingStyle X3DRenderingLoop::GetBackfacingStyleState() const
{
	TQ3StyleObject object;
	
	TQ3Status status = Q3View_GetDrawContext(mView, &object);
	ThrowIf3DError(status);
	
	X3DBackfacingStyle obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::GetInterpolationStyleState
//
//---------------------------------------------------------------
X3DInterpolationStyle X3DRenderingLoop::GetInterpolationStyleState() const
{
	TQ3StyleObject object;
	
	TQ3Status status = Q3View_GetDrawContext(mView, &object);
	ThrowIf3DError(status);
	
	X3DInterpolationStyle obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::GetFillStyleState
//
//---------------------------------------------------------------
X3DFillStyle X3DRenderingLoop::GetFillStyleState() const
{
	TQ3StyleObject object;
	
	TQ3Status status = Q3View_GetDrawContext(mView, &object);
	ThrowIf3DError(status);
	
	X3DFillStyle obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::GetHighlightStyleState
//
//---------------------------------------------------------------
X3DHighlightStyle X3DRenderingLoop::GetHighlightStyleState() const
{
	TQ3StyleObject object;
	
	TQ3Status status = Q3View_GetDrawContext(mView, &object);
	ThrowIf3DError(status);
	
	X3DHighlightStyle obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::GetSubdivisionStyleState
//
//---------------------------------------------------------------
X3DSubdivisionStyle X3DRenderingLoop::GetSubdivisionStyleState() const
{
	TQ3StyleObject object;
	
	TQ3Status status = Q3View_GetDrawContext(mView, &object);
	ThrowIf3DError(status);
	
	X3DSubdivisionStyle obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::GetOrientationStyleState
//
//---------------------------------------------------------------
X3DOrientationStyle X3DRenderingLoop::GetOrientationStyleState() const
{
	TQ3StyleObject object;
	
	TQ3Status status = Q3View_GetDrawContext(mView, &object);
	ThrowIf3DError(status);
	
	X3DOrientationStyle obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::GetReceiveShadowsStyleState
//
//---------------------------------------------------------------
X3DReceiveShadowsStyle X3DRenderingLoop::GetReceiveShadowsStyleState() const
{
	TQ3StyleObject object;
	
	TQ3Status status = Q3View_GetDrawContext(mView, &object);
	ThrowIf3DError(status);
	
	X3DReceiveShadowsStyle obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::GetPickIDStyleState
//
//---------------------------------------------------------------
X3DPickIDStyle X3DRenderingLoop::GetPickIDStyleState() const
{
	TQ3StyleObject object;
	
	TQ3Status status = Q3View_GetDrawContext(mView, &object);
	ThrowIf3DError(status);
	
	X3DPickIDStyle obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::GetPickPartsStyleState
//
//---------------------------------------------------------------
X3DPickPartsStyle X3DRenderingLoop::GetPickPartsStyleState() const
{
	TQ3StyleObject object;
	
	TQ3Status status = Q3View_GetDrawContext(mView, &object);
	ThrowIf3DError(status);
	
	X3DPickPartsStyle obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::GetAntiAliasStyleState
//
//---------------------------------------------------------------
X3DAntiAliasStyle X3DRenderingLoop::GetAntiAliasStyleState() const
{
	TQ3StyleObject object;
	
	TQ3Status status = Q3View_GetDrawContext(mView, &object);
	ThrowIf3DError(status);
	
	X3DAntiAliasStyle obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::GetDefaultAttributeSet
//
//---------------------------------------------------------------
TQ3AttributeSet X3DRenderingLoop::GetDefaultAttributeSet() const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3View_GetDefaultAttributeSet(mView, &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::SetDefaultAttributeSet
//
//---------------------------------------------------------------
void X3DRenderingLoop::SetDefaultAttributeSet(TQ3AttributeSet attributes)
{
	TQ3Status status = Q3View_SetDefaultAttributeSet(mView, attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::GetAttributeSetState
//
//---------------------------------------------------------------
TQ3AttributeSet X3DRenderingLoop::GetAttributeSetState() const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3View_GetAttributeSetState(mView, &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::GetAttributeState
//
//---------------------------------------------------------------
void X3DRenderingLoop::GetAttributeState(TQ3AttributeType type, void* data) const
{
	TQ3Status status = Q3View_GetAttributeState(mView, type, data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRenderingLoop::Cancel
//
//---------------------------------------------------------------
void X3DRenderingLoop::Cancel()
{
	TQ3Status status = Q3View_Cancel(mView);
	ThrowIf3DError(status);
}


}	// namespace Whisper
