/*
 *  File:       X3DRenderingLoops.cpp
 *  Summary:	Classes encapsulating QuickDraw 3D rendering loops.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DRenderingLoops.cpp,v $
 *		Revision 1.4  2001/04/21 03:37:44  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:31:28  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:20:00  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DRenderingLoops.h>

#include <X3DFile.h>
#include <X3DPick.h>
#include <X3DPrimitives.h>
#include <X3DUtils.h>
#include <X3DView.h>

namespace Whisper {


// ===================================================================================
//	class X3DRenderLoop
// ===================================================================================

//---------------------------------------------------------------
//
// X3DRenderLoop::~X3DRenderLoop
//
//---------------------------------------------------------------
X3DRenderLoop::~X3DRenderLoop()
{
	if (mStatus == kQ3ViewStatusRetraverse) {		// should only happen if an exception is thrown in the render loop
		(void) Q3View_Cancel(mView);
		(void) Q3View_EndRendering(mView);
		
		mStatus = kQ3ViewStatusError;
	}
}


//---------------------------------------------------------------
//
// X3DRenderLoop::X3DRenderLoop
//
//---------------------------------------------------------------
X3DRenderLoop::X3DRenderLoop(TQ3ViewObject view) : X3DRenderingLoop(view)
{
	TQ3Status status = Q3View_StartRendering(mView);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRenderLoop::Flush
//
//---------------------------------------------------------------
void X3DRenderLoop::Flush()
{
	TQ3Status status = Q3View_Flush(mView);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRenderLoop::Sync
//
//---------------------------------------------------------------
void X3DRenderLoop::Sync()
{
	TQ3Status status = Q3View_Sync(mView);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DRenderLoop::IsRendering
//
//---------------------------------------------------------------
bool X3DRenderLoop::IsRendering()
{
	PRECONDITION(mStatus == kQ3ViewStatusRetraverse);
	
	mStatus = Q3View_EndRendering(mView);
	if (mStatus == kQ3ViewStatusError)
		ThrowIf3DError(kQ3Failure);
		
	return mStatus == kQ3ViewStatusRetraverse;
}

#pragma mark -

// ===================================================================================
//	class X3DBoundingBoxLoop
// ===================================================================================

//---------------------------------------------------------------
//
// X3DBoundingBoxLoop::~X3DBoundingBoxLoop
//
//---------------------------------------------------------------
X3DBoundingBoxLoop::~X3DBoundingBoxLoop()
{
	if (mStatus == kQ3ViewStatusRetraverse) {		// should only happen if an exception is thrown in the render loop
		(void) Q3View_Cancel(mView);

		TQ3BoundingBox bbox;
		(void) Q3View_EndBoundingBox(mView, &bbox);
		
		mStatus = kQ3ViewStatusError;
	}
}


//---------------------------------------------------------------
//
// X3DBoundingBoxLoop::X3DBoundingBoxLoop
//
//---------------------------------------------------------------
X3DBoundingBoxLoop::X3DBoundingBoxLoop(TQ3ViewObject view, X3DRect* box, TQ3ComputeBounds bounds) : X3DRenderingLoop(view)
{
	PRECONDITION(box != nil);
	PRECONDITION(view != nil);
		
	mBox = box;
	
	TQ3Status status = Q3View_StartBoundingBox(mView, bounds);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DBoundingBoxLoop::IsRendering
//
//---------------------------------------------------------------
bool X3DBoundingBoxLoop::IsRendering()
{
	PRECONDITION(mStatus == kQ3ViewStatusRetraverse);
	
	TQ3BoundingBox bbox;
	
	mStatus = Q3View_EndBoundingBox(mView, &bbox);
	if (mStatus == kQ3ViewStatusDone)
		*mBox = bbox.isEmpty ? kZero3DRect : X3DRect(bbox.min, bbox.max);

	else if (mStatus == kQ3ViewStatusError)
		ThrowIf3DError(kQ3Failure);
		
	return mStatus == kQ3ViewStatusRetraverse;
}

#pragma mark -

// ===================================================================================
//	class X3DBoundingSphereLoop
// ===================================================================================

//---------------------------------------------------------------
//
// X3DBoundingSphereLoop::~X3DBoundingSphereLoop
//
//---------------------------------------------------------------
X3DBoundingSphereLoop::~X3DBoundingSphereLoop()
{
	if (mStatus == kQ3ViewStatusRetraverse) {		// should only happen if an exception is thrown in the render loop
		(void) Q3View_Cancel(mView);

		TQ3BoundingSphere bsphere;
		(void) Q3View_EndBoundingSphere(mView, &bsphere);
		
		mStatus = kQ3ViewStatusError;
	}
}


//---------------------------------------------------------------
//
// X3DBoundingSphereLoop::X3DBoundingSphereLoop
//
//---------------------------------------------------------------
X3DBoundingSphereLoop::X3DBoundingSphereLoop(TQ3ViewObject view, X3DSphere* sphere, TQ3ComputeBounds bounds) : X3DRenderingLoop(view)
{	
	PRECONDITION(sphere != nil);
	PRECONDITION(view != nil);
		
	mSphere = sphere;
	
	TQ3Status status = Q3View_StartBoundingSphere(mView, bounds);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DBoundingSphereLoop::IsRendering
//
//---------------------------------------------------------------
bool X3DBoundingSphereLoop::IsRendering()
{
	PRECONDITION(mStatus == kQ3ViewStatusRetraverse);
	
	TQ3BoundingSphere bsphere;
	
	mStatus = Q3View_EndBoundingSphere(mView, &bsphere);
	if (mStatus == kQ3ViewStatusDone)
		*mSphere = bsphere.isEmpty ? kZero3DSphere : X3DSphere(bsphere.origin, bsphere.radius);
	
	else if (mStatus == kQ3ViewStatusError)
		ThrowIf3DError(kQ3Failure);
		
	return mStatus == kQ3ViewStatusRetraverse;
}

#pragma mark -

// ===================================================================================
//	class X3DPickLoop
// ===================================================================================

//---------------------------------------------------------------
//
// X3DPickLoop::~X3DPickLoop
//
//---------------------------------------------------------------
X3DPickLoop::~X3DPickLoop()
{
	if (mStatus == kQ3ViewStatusRetraverse) {		// should only happen if an exception is thrown in the render loop
		(void) Q3View_Cancel(mView);
		(void) Q3View_EndPicking(mView);
		
		mStatus = kQ3ViewStatusError;
	}
}


//---------------------------------------------------------------
//
// X3DPickLoop::X3DPickLoop
//
//---------------------------------------------------------------
X3DPickLoop::X3DPickLoop(TQ3ViewObject view, X3DPick* pick) : X3DRenderingLoop(view)
{
	PRECONDITION(pick != nil);
	PRECONDITION(view != nil);
		
	TQ3Status status = Q3View_StartPicking(mView, *pick);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPickLoop::IsRendering
//
//---------------------------------------------------------------
bool X3DPickLoop::IsRendering()
{
	PRECONDITION(mStatus == kQ3ViewStatusRetraverse);
	
	mStatus = Q3View_EndPicking(mView);
	if (mStatus == kQ3ViewStatusError)
		ThrowIf3DError(kQ3Failure);
		
	return mStatus == kQ3ViewStatusRetraverse;
}

#pragma mark -

// ===================================================================================
//	class X3DWriteLoop
// ===================================================================================

//---------------------------------------------------------------
//
// X3DWriteLoop::~X3DWriteLoop
//
//---------------------------------------------------------------
X3DWriteLoop::~X3DWriteLoop()
{
	if (mStatus == kQ3ViewStatusRetraverse) {		// should only happen if an exception is thrown in the render loop
		(void) Q3View_Cancel(mView);
		(void) Q3View_EndWriting(mView);
		
		mStatus = kQ3ViewStatusError;
	}
}


//---------------------------------------------------------------
//
// X3DWriteLoop::X3DWriteLoop
//
//---------------------------------------------------------------
X3DWriteLoop::X3DWriteLoop(TQ3ViewObject view, X3DFile* file) : X3DRenderingLoop(view)
{
	PRECONDITION(file != nil);
			
	TQ3Status status = Q3View_StartWriting(mView, *file);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DWriteLoop::IsRendering
//
//---------------------------------------------------------------
bool X3DWriteLoop::IsRendering()
{
	PRECONDITION(mStatus == kQ3ViewStatusRetraverse);
	
	mStatus = Q3View_EndWriting(mView);
	if (mStatus == kQ3ViewStatusError)
		ThrowIf3DError(kQ3Failure);
		
	return mStatus == kQ3ViewStatusRetraverse;
}


}	// namespace Whisper
